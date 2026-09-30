/**
 * @file  hal_nvm.c
 * @brief NVM HAL 实现（乒乓双扇区，直接操作 YTM32B1MC0 EFM 寄存器）
 *
 * 只引用工程中已有的 platform 头文件，不新增任何 platform 文件：
 *   YTM32B1MC0.h          → EFM_Type 寄存器结构体、位域宏
 *   YTM32B1MC0_features.h → EFM 命令码、Flash 地址/尺寸宏
 *   core_common.h         → START/END_FUNCTION_DEFINITION_RAMSECTION
 *
 * ── DFlash 扇区分配（YTM32B1MC0，array0 = 0x10001000~0x100037FF，共 20 扇区）──
 *
 *   扇区 0  0x10001000  ★ Bootloader 刷写记录（BootRecord_t），App 绝不可碰
 *   扇区 1  0x10001200    App NVM 乒乓区 A
 *   扇区 2  0x10001400    App NVM 乒乓区 B
 *   扇区 3~ 0x10001600    预留
 *
 *   扇区大小: 0x200 (512 B)
 *   写单元  : 8 B (2 words)  FEATURE_EFM_WRITE_UNIT_SIZE
 *
 * ⚠ 扇区 0 的归属由 boot_app_if.h 的 BOOT_RECORD_ADDR 定义，两个工程共用同
 *   一份头文件。本文件从 BOOT_RECORD_ADDR 往后推算自己的乒乓区，不要改回
 *   直接用 FEATURE_EFM_DATA_ARRAY0_START_ADDRESS ——
 *   那个地址正好压在刷写记录上，后果是：
 *     刷写成功 → App 跑起来 → 里程一落盘（尤其 uds_jump_to_boot() 里那次
 *     NVM_SaveOdometer）就把扇区 0 擦掉 → 刷写记录 magic 变 0xFFFFFFFF →
 *     下次上电 Boot 判定 App 无效，永远驻留 Boot。表现是「刷完能用，
 *     断电再上电就白屏不出画面」，且没有任何报错。
 *   文件末尾有编译期断言把这条约束钉死。
 *
 * 乒乓写入流程（任一步掉电均不丢有效数据）：
 *   1. 读活跃区全量 → RAM
 *   2. 修改目标字段
 *   3. 更新 seq+1 和 magic=0xA5
 *   4. 擦除备用区
 *   5. 编程备用区
 *   6. 擦除旧活跃区（使其失效）
 *
 * 扇区布局（16 字节，按 8 字节写单元分 2 次写入）：
 *   offset 0   : magic  (0xA5=有效, 擦后0xFF=无效)
 *   offset 1   : seq    (序列号，0xFF 为哨兵跳过)
 *   offset 2~3 : 预留   (0xFF 填充)
 *   offset 4~7 : odo_km (u32 LE)
 *   offset 8~9 : trip_dm(u16 LE)
 *   offset 10~15: 预留
 */

#include "hal_nvm.h"
#include "boot_app_if.h"   /* BOOT_RECORD_ADDR / DFLASH_* ：与 Boot 共用的契约 */
#include "YTM32B1MC0.h"
#include "YTM32B1MC0_features.h"
#include "core_common.h"
#include <string.h>

/* =========================================================================
 * DFlash 地址配置
 *
 * 起点不是数据 Flash 的首扇区，而是「刷写记录之后的第一个扇区」。
 * 扇区 0 归 Bootloader，见文件头注释。
 * ========================================================================= */
#define NVM_SECTOR_SIZE    FEATURE_EFM_DATA_ARRAY_SECTOR_SIZE

/** App 可用的第一个数据 Flash 扇区 = 刷写记录扇区的下一个 */
#define NVM_AREA_BASE_ADDR ((uint32_t)BOOT_RECORD_ADDR + (uint32_t)DFLASH_SECTOR_SIZE)

#define NVM_SECTOR_A_ADDR  (NVM_AREA_BASE_ADDR)
#define NVM_SECTOR_B_ADDR  (NVM_AREA_BASE_ADDR + NVM_SECTOR_SIZE)
#define NVM_PROG_UNIT      FEATURE_EFM_WRITE_UNIT_SIZE         /* 8 B  */
#define NVM_PROG_WORDS     FEATURE_EFM_WRITE_UNIT_WORD_SIZE    /* 2 words */
#define NVM_DATA_SIZE      16u   /* 须为 NVM_PROG_UNIT 整数倍 */

#define NVM_MAGIC          0xA5u
#define OFF_MAGIC          0u
#define OFF_SEQ            1u
#define OFF_ODO            4u
#define OFF_TRIP           8u

/* =========================================================================
 * 编译期布局检查
 *
 * 用「负长度数组」而不是 _Static_assert，兼容 IAR 的 C89/C99 模式。
 * 任何一条不成立都会在编译期报 "negative array size"，比运行期把仪表
 * 刷成砖再回头查要便宜得多。
 * ========================================================================= */
#define NVM_BUILD_ASSERT(name, cond) \
    typedef char nvm_assert_##name[(cond) ? 1 : -1]

/* 1. 共享契约里的数据 Flash 参数必须与芯片 features 头一致 */
NVM_BUILD_ASSERT(dflash_base_matches_silicon,
    (uint32_t)DFLASH_BASE_ADDR == (uint32_t)FEATURE_EFM_DATA_ARRAY0_START_ADDRESS);
NVM_BUILD_ASSERT(dflash_sector_matches_silicon,
    (uint32_t)DFLASH_SECTOR_SIZE == (uint32_t)FEATURE_EFM_DATA_ARRAY_SECTOR_SIZE);

/* 2. ★ 核心约束：App 的乒乓区不得与 Bootloader 刷写记录扇区重叠 */
NVM_BUILD_ASSERT(nvm_does_not_touch_boot_record,
    NVM_SECTOR_A_ADDR >= ((uint32_t)BOOT_RECORD_ADDR + (uint32_t)DFLASH_SECTOR_SIZE));

/* 3. 两个乒乓扇区必须落在数据 Flash array0 之内 */
NVM_BUILD_ASSERT(nvm_within_dflash,
    (NVM_SECTOR_B_ADDR + NVM_SECTOR_SIZE - 1u)
        <= (uint32_t)FEATURE_EFM_DATA_ARRAY0_END_ADDRESS);

/* 4. 数据长度必须是写单元整数倍，否则 dflash_program 的循环会越界 */
NVM_BUILD_ASSERT(nvm_data_size_aligned,
    (NVM_DATA_SIZE % FEATURE_EFM_WRITE_UNIT_SIZE) == 0u);

/* =========================================================================
 * efm_launch_cmd — 执行一次 EFM 命令（关中断 + 同步阻塞）
 *
 * 操作 DFlash 时代码运行在 PFlash，无 RWW 约束，
 * 但仍用 RAMSECTION 宏以便将来操作 PFlash 时代码自动迁移到 RAM。
 * ========================================================================= */
START_FUNCTION_DECLARATION_RAMSECTION
static bool efm_launch_cmd(uint32_t cmd, uint32_t dest,
                            const uint32_t *src, uint32_t word_cnt)
END_FUNCTION_DECLARATION_RAMSECTION

START_FUNCTION_DEFINITION_RAMSECTION
static bool efm_launch_cmd(uint32_t cmd, uint32_t dest,
                            const uint32_t *src, uint32_t word_cnt)
{
    uint32_t primask = __get_PRIMASK();
    __disable_irq();

    /* 清除上次状态标志 */
    EFM->STS = FEATURE_EFM_CMD_STS_CLEAR_MASK;

    /* 使能写操作 */
    EFM_ENABLE_WE_COMMAND(EFM);

    /* 写数据到目标地址（触发 EFM 内部地址锁存） */
    volatile uint32_t *dst = (volatile uint32_t *)(uintptr_t)dest;
    if (src != NULL) {
        for (uint32_t i = 0u; i < word_cnt; i++) {
            dst[i] = src[i];
        }
    } else {
        /* 擦除命令只需触发一次写来锁存地址 */
        dst[0] = 0xFFFFFFFFu;
    }

    EFM_DISABLE_WE_COMMAND(EFM);

    /* 解锁命令寄存器（key=0xfd9573f5，来自 EFM_UNLOCK_CMD_REGISTER 宏定义） */
    EFM_UNLOCK_CMD_REGISTER(EFM);

    /* 写命令寄存器，触发执行 */
    EFM->CMD = EFM_CMD_CMD(cmd);

    /* 轮询等待 DONE 或 ACCERR */
    while (((EFM->STS & EFM_STS_DONE_MASK)   == 0u) &&
           ((EFM->STS & EFM_STS_ACCERR_MASK) == 0u)) { }

    bool ok = ((EFM->STS & FEATURE_EFM_CMD_ERROR_MASK) == 0u);

    __set_PRIMASK(primask);
    return ok;
}
END_FUNCTION_DEFINITION_RAMSECTION

/* =========================================================================
 * 内部辅助
 * ========================================================================= */

static inline uint8_t dflash_read_byte(uint32_t addr)
{
    return *(volatile uint8_t *)(uintptr_t)addr;
}

/**
 * 只允许擦除本模块自己的两个乒乓扇区。
 *
 * 编译期断言已经保证宏配得对，这条是运行期的最后一道保险：万一以后有人
 * 把计算出来的地址当参数传进来（比如加了第三个扇区、或算错了偏移），
 * 宁可擦除失败也绝不能碰到扇区 0 的刷写记录。
 */
static bool dflash_erase_sector(uint32_t addr)
{
    if ((addr != NVM_SECTOR_A_ADDR) && (addr != NVM_SECTOR_B_ADDR)) {
        return false;
    }
    return efm_launch_cmd(FEATURE_EFM_ERASE_SECTOR_CMD_CODE, addr, NULL, 1u);
}

static bool dflash_program(uint32_t dest, const uint8_t *buf, uint8_t len)
{
    /* 同 dflash_erase_sector：绝不允许写到自己两个扇区之外 */
    if ((dest != NVM_SECTOR_A_ADDR) && (dest != NVM_SECTOR_B_ADDR)) {
        return false;
    }

    /* 4 字节对齐编程缓冲（static 保证地址对齐） */
    static uint32_t s_prog_buf[NVM_DATA_SIZE / 4u];
    memset(s_prog_buf, 0xFFu, sizeof(s_prog_buf));
    if (len > NVM_DATA_SIZE) { len = NVM_DATA_SIZE; }
    memcpy(s_prog_buf, buf, len);

    uint32_t addr       = dest;
    uint32_t remaining  = NVM_DATA_SIZE;
    uint32_t word_idx   = 0u;

    while (remaining > 0u) {
        if (!efm_launch_cmd(FEATURE_EFM_PROGRAM_CMD_CODE, addr,
                            &s_prog_buf[word_idx], NVM_PROG_WORDS)) {
            return false;
        }
        addr      += NVM_PROG_UNIT;
        word_idx  += NVM_PROG_WORDS;
        remaining -= NVM_PROG_UNIT;
    }
    return true;
}

static uint8_t sector_seq(uint32_t base)
{
    if (dflash_read_byte(base + OFF_MAGIC) == NVM_MAGIC) {
        return dflash_read_byte(base + OFF_SEQ);
    }
    return 0xFFu;
}

static uint32_t active_sector(void)
{
    uint8_t sa = sector_seq(NVM_SECTOR_A_ADDR);
    uint8_t sb = sector_seq(NVM_SECTOR_B_ADDR);
    bool    av = (sa != 0xFFu);
    bool    bv = (sb != 0xFFu);
    if (av && bv) {
        return ((int8_t)(sa - sb) > 0) ? NVM_SECTOR_A_ADDR : NVM_SECTOR_B_ADDR;
    }
    return av ? NVM_SECTOR_A_ADDR : (bv ? NVM_SECTOR_B_ADDR : NVM_SECTOR_A_ADDR);
}

static uint32_t standby_sector(uint32_t act)
{
    return (act == NVM_SECTOR_A_ADDR) ? NVM_SECTOR_B_ADDR : NVM_SECTOR_A_ADDR;
}

/* =========================================================================
 * 公开接口
 * ========================================================================= */

bool HAL_NVM_Read(u16 addr, u8 *data, u8 len)
{
    if (!data || len == 0u || (uint32_t)addr + len > NVM_DATA_SIZE) { return false; }
    uint32_t act = active_sector();
    if (dflash_read_byte(act + OFF_MAGIC) != NVM_MAGIC) { return false; }
    for (uint8_t i = 0u; i < len; i++) {
        data[i] = dflash_read_byte(act + addr + i);
    }
    return true;
}

bool HAL_NVM_Write(u16 addr, const u8 *data, u8 len)
{
    if (!data || len == 0u || (uint32_t)addr + len > NVM_DATA_SIZE) { return false; }

    uint32_t act     = active_sector();
    uint32_t standby = standby_sector(act);

    /* 1. 读活跃区全量到 RAM */
    uint8_t buf[NVM_DATA_SIZE];
    memset(buf, 0xFFu, sizeof(buf));
    if (dflash_read_byte(act + OFF_MAGIC) == NVM_MAGIC) {
        for (uint8_t i = 0u; i < NVM_DATA_SIZE; i++) {
            buf[i] = dflash_read_byte(act + i);
        }
    }

    /* 2. 修改目标字段 */
    memcpy(&buf[addr], data, len);

    /* 3. 更新序列号和 magic */
    uint8_t new_seq = (uint8_t)(buf[OFF_SEQ] + 1u);
    if (new_seq == 0xFFu) { new_seq = 0u; }
    buf[OFF_MAGIC] = NVM_MAGIC;
    buf[OFF_SEQ]   = new_seq;

    /* 4. 擦除备用区 */
    if (!dflash_erase_sector(standby)) { return false; }

    /* 5. 编程备用区 */
    if (!dflash_program(standby, buf, NVM_DATA_SIZE)) { return false; }

    /* 6. 擦除旧活跃区（失效旧数据） */
    (void)dflash_erase_sector(act);

    return true;
}

/* =========================================================================
 * BL24C08F EEPROM NVM 接口实现
 *
 * 本节通过调用 hal_eeprom_bl24c08f 驱动实现 HAL_NVM_EE_* 系列接口。
 * DFlash 接口（HAL_NVM_Read/Write）保持不变，两套后端相互独立。
 * ========================================================================= */

#include "hal_eeprom_bl24c08f.h"

/* DTC 条目大小（字节）*/
#define NVM_EE_DTC_ENTRY_SIZE   4U
/* DTC 最大槽数：EE_ZONE_DTC_SIZE / NVM_EE_DTC_ENTRY_SIZE = 128/4 = 32 */
#define NVM_EE_DTC_MAX_INDEX    31U

bool HAL_NVM_EE_Init(void)
{
    return HAL_EEPROM_Init();
}

bool HAL_NVM_EE_Write(u16 addr, const u8 *data, u16 len)
{
    return HAL_EEPROM_Write(addr, data, len);
}

bool HAL_NVM_EE_Read(u16 addr, u8 *data, u16 len)
{
    return HAL_EEPROM_Read(addr, data, len);
}

bool HAL_NVM_EE_StoreDTC(u8 index, const u8 *entry)
{
    if ((entry == NULL) || (index > NVM_EE_DTC_MAX_INDEX)) {
        return false;
    }
    u16 addr = (u16)(EE_ZONE_DTC_BASE + (u16)index * NVM_EE_DTC_ENTRY_SIZE);
    return HAL_EEPROM_Write(addr, entry, NVM_EE_DTC_ENTRY_SIZE);
}

bool HAL_NVM_EE_ReadDTC(u8 index, u8 *entry)
{
    if ((entry == NULL) || (index > NVM_EE_DTC_MAX_INDEX)) {
        return false;
    }
    u16 addr = (u16)(EE_ZONE_DTC_BASE + (u16)index * NVM_EE_DTC_ENTRY_SIZE);
    return HAL_EEPROM_Read(addr, entry, NVM_EE_DTC_ENTRY_SIZE);
}

bool HAL_NVM_EE_ClearAllDTC(void)
{
    /*
     * 将 DTC 区整体填充 0xFF（EEPROM 擦除态）。
     * 按页逐块写入，避免单次超过 EEPROM 页写限制。
     */
    static const u8 erase_page[BL24C08F_PAGE_SIZE] = {
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
    };

    u16 pages = (u16)(EE_ZONE_DTC_SIZE / BL24C08F_PAGE_SIZE);  /* 128/16 = 8 */
    for (u16 p = 0U; p < pages; p++) {
        u16 addr = (u16)(EE_ZONE_DTC_BASE + p * BL24C08F_PAGE_SIZE);
        if (!HAL_EEPROM_Write(addr, erase_page, BL24C08F_PAGE_SIZE)) {
            return false;
        }
    }
    return true;
}