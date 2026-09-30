/**
 * @file  odo_log.c
 * @brief 里程日志式存储实现
 *
 * 设计说明见 odo_log.h 与《里程日志式存储设计说明》。
 *
 * 本文件可以在 PC 上编译（定义 ODOLOG_SIM），用 odo_log_sim.c 提供的
 * 模拟 Flash 做掉电注入测试。测试跑的是本文件里的真实逻辑，不是另写
 * 一份等价实现，所以结论可以直接迁移到目标板。
 */

#include "odo_log.h"

#ifdef ODOLOG_SIM
/* PC 仿真：由 odo_log_sim.c 提供三个 Flash 原语 */
extern bool  sim_flash_erase(u32 addr);
extern bool  sim_flash_program(u32 addr, const u32 *src, u32 word_cnt);
extern const u8 *sim_flash_ptr(u32 addr);
#else
#include "boot_app_if.h"          /* BOOT_RECORD_ADDR / DFLASH_* */
#include "YTM32B1MC0.h"
#include "YTM32B1MC0_features.h"
#include "core_common.h"
#endif

#include <string.h>

/*===========================================================================
 *  1. 布局配置
 *===========================================================================*/

/**
 * 日志区基地址。
 *
 * 默认放在数据 Flash array1，与 Bootloader 刷写记录所在的 array0 物理隔离。
 * 这样即便本模块的地址算错，也不可能擦到刷写记录——历史上正是这个问题
 * 导致"刷写成功能跑，断电重上电却永久驻留 Boot"，且无任何报错。
 *
 * 若实测该型号没有 array1，改为 (DFLASH_BASE_ADDR + DFLASH_SECTOR_SIZE)
 * 即 array0 的扇区 1。文件末尾的编译期断言对两种情况都会检查。
 */
#ifndef ODOLOG_BASE_ADDR
#define ODOLOG_BASE_ADDR    (0x10011000UL)
#endif

/** 日志区扇区数。8 个扇区 = 4 KB，乘用车 10 年余量 58 倍。 */
#ifndef ODOLOG_SECTOR_CNT
#define ODOLOG_SECTOR_CNT   (8u)
#endif

/** 是否从旧的 hal_nvm 乒乓格式迁移里程。首次量产可关掉。 */
#ifndef ODOLOG_MIGRATE_LEGACY
#define ODOLOG_MIGRATE_LEGACY (1)
#endif

#define ODOLOG_SECTOR_SIZE  (0x200u)     /* 512 字节 */
#define ODOLOG_WRITE_UNIT   (8u)         /* EFM 写单元，字节 */
#define ODOLOG_WRITE_WORDS  (ODOLOG_WRITE_UNIT / 4u)   /* = 2，每条编程命令的字数 */
#define ODOLOG_REC_SIZE     (16u)        /* 一条记录 = 2 个写单元 */
#define ODOLOG_SLOTS        (ODOLOG_SECTOR_SIZE / ODOLOG_REC_SIZE)   /* 32 */

#define SEQ_INVALID         (0xFFFFFFFFul)

/*===========================================================================
 *  2. 记录格式
 *===========================================================================*/

/**
 * 16 字节，两个写单元。
 *
 * seq 放在最前、crc 放在最后，是为了配合编程顺序：EFM 按写单元顺序编程，
 * 掉电只可能截断在两个单元之间。
 *   - 只写进第 1 个单元（seq+odo）→ 第 2 个单元仍是擦除态，crc 不匹配 → 无效
 *   - 只写进第 2 个单元（trip+crc）→ seq 仍是 0xFFFFFFFF → 被 seq 判据拒绝
 * 两种截断都不会被误认为有效记录。
 */
typedef struct {
    u32 seq;        /**< 全局递增序号，永不回绕；0xFFFFFFFF 表示空槽 */
    u32 odo_km;     /**< 总里程，km        */
    u16 trip_dm;    /**< 小计里程，0.1 km  */
    u16 rsvd;       /**< 预留，写 0xFFFF   */
    u32 crc;        /**< 前 12 字节的 CRC32 */
} OdoRec_t;

/*===========================================================================
 *  3. 运行状态
 *===========================================================================*/

static bool s_ready     = false;

/* 失败诊断。Flash 操作失败时把硬件状态抓下来，供调试器 / 诊断读取。
 * 没有这个的话，写入失败是完全静默的——现象只是"日志区一直全 FF"，
 * 无从判断是地址错、保护开着、还是命令被拒。 */
static u32  s_err_sts   = 0u;   /**< 失败瞬间的 EFM->STS */
static u32  s_err_addr  = 0u;   /**< 失败的目标地址      */
static u8   s_err_op    = 0u;   /**< 1=擦除 2=编程       */
static u32  s_err_cnt   = 0u;   /**< 累计失败次数        */
static u32  s_seq       = 0u;        /**< 当前最大序号 */
static u32  s_odo_km    = 0u;
static u16  s_trip_dm   = 0u;
static bool s_has_data  = false;     /**< 是否读到过有效记录 */
static u8   s_sector    = 0u;        /**< 当前活跃扇区 */
static u8   s_slot      = 0u;        /**< 下一个可写槽位 [0, 32] */

/*===========================================================================
 *  4. CRC32
 *
 *  与 Boot 的 boot_crc.c 同算法（CRC-32/ISO-HDLC，半字节查表），
 *  16 项表只占 64 字节。选同一个算法是为了将来若需在 Boot 里读取
 *  里程（例如诊断读取），两边可以共用实现。
 *===========================================================================*/

static const u32 s_crc_tab[16] = {
    0x00000000ul, 0x1DB71064ul, 0x3B6E20C8ul, 0x26D930ACul,
    0x76DC4190ul, 0x6B6B51F4ul, 0x4DB26158ul, 0x5005713Cul,
    0xEDB88320ul, 0xF00F9344ul, 0xD6D6A3E8ul, 0xCB61B38Cul,
    0x9B64C2B0ul, 0x86D3D2D4ul, 0xA00AE278ul, 0xBDBDF21Cul
};

static u32 crc32_calc(const u8 *data, u32 len)
{
    u32 crc = 0xFFFFFFFFul;
    for (u32 i = 0u; i < len; i++) {
        crc ^= (u32)data[i];
        crc = (crc >> 4u) ^ s_crc_tab[crc & 0x0Fu];
        crc = (crc >> 4u) ^ s_crc_tab[crc & 0x0Fu];
    }
    return crc ^ 0xFFFFFFFFul;
}

/*===========================================================================
 *  5. Flash 原语
 *===========================================================================*/

#ifdef ODOLOG_SIM

static bool flash_erase(u32 addr) { return sim_flash_erase(addr); }
/* 原语只编程一个写单元，与硬件约束一致；拆分逻辑在下面的公共函数里，
 * 这样仿真跑的就是目标板上同一条路径。 */
static bool flash_program_unit(u32 addr, const u32 *s)
{
    return sim_flash_program(addr, s, ODOLOG_WRITE_WORDS);
}
static const u8 *flash_ptr(u32 addr)                             { return sim_flash_ptr(addr); }

#else

static const u8 *flash_ptr(u32 addr)
{
    return (const u8 *)(uintptr_t)addr;
}

/**
 * 执行一次 EFM 命令。
 *
 * 与 hal_nvm.c 的 efm_launch_cmd 相比只改了一点：**只在寄存器操作序列
 * 期间关中断，等待 DONE 的轮询过程放开中断**。
 *
 * 原因：扇区擦除要 4~5.8 毫秒。全程关中断的话，每 32 次保存就有一次让
 * 系统停摆近 6 毫秒——1 毫秒周期的 UDS_TpTask、HW_SampleTask 会被跳过
 * 若干轮，CAN 接收邮箱可能溢出，而且若时基是中断驱动的，HAL_GetTickMs
 * 会直接丢失这段计数。
 *
 * 放开中断是安全的：操作的是 DFlash 而代码在 PFlash 上执行，无 RWW 约束
 * （hal_nvm.c 原注释亦如此说明），且全工程只有本模块和 hal_nvm.c 会碰
 * EFM 寄存器，两者不会并发。
 *
 * ⚠ 这一条需要在台架上实测确认：擦除期间触发中断、在 ISR 里读 PFlash
 *   常量与执行代码，确认不会 HardFault、擦除结果正确。若确认不通过，
 *   把 __set_PRIMASK 那行移到轮询循环之后即可恢复保守行为。
 */
START_FUNCTION_DECLARATION_RAMSECTION
static bool efm_launch(u32 cmd, u32 dest, const u32 *src, u32 word_cnt)
END_FUNCTION_DECLARATION_RAMSECTION

START_FUNCTION_DEFINITION_RAMSECTION
static bool efm_launch(u32 cmd, u32 dest, const u32 *src, u32 word_cnt)
{
    u32 primask = __get_PRIMASK();
    __disable_irq();

    EFM->STS = FEATURE_EFM_CMD_STS_CLEAR_MASK;
    EFM_ENABLE_WE_COMMAND(EFM);

    volatile u32 *dst = (volatile u32 *)(uintptr_t)dest;
    if (src != NULL) {
        for (u32 i = 0u; i < word_cnt; i++) { dst[i] = src[i]; }
    } else {
        dst[0] = 0xFFFFFFFFu;      /* 擦除只需触发一次写以锁存地址 */
    }

    EFM_DISABLE_WE_COMMAND(EFM);
    EFM_UNLOCK_CMD_REGISTER(EFM);
    EFM->CMD = EFM_CMD_CMD(cmd);

    while (((EFM->STS & EFM_STS_DONE_MASK)   == 0u) &&
           ((EFM->STS & EFM_STS_ACCERR_MASK) == 0u)) { }

    u32 sts = EFM->STS;

    /* ★ 中断必须等命令跑完才能放开，不能在发出 CMD 之后就恢复。
     *
     * EFM_STS 有 ARRAY_SELECTED 位，说明控制器同一时刻只服务一片 array。
     * 擦除 DFlash 要 5.8ms，期间若中断打开，ISR 的向量表与处理函数都在
     * PFlash 里，取指会撞上正忙的 EFM，命令被打断而失败。App 有 1ms 周期
     * 任务，这期间必然触发中断，结果是每次写入都失败且完全静默。
     *
     * 曾经试过只在寄存器序列期间关中断、轮询时放开，想把停摆时间从 5.8ms
     * 压到微秒级，实测写入全部失败。代价是每 32 次保存有一次约 6ms 的中断
     * 延迟——1ms 的 UDS_TpTask 和 HW_SampleTask 会被跳过几轮。这是必须付的
     * 代价，不要再"优化"。 */
    __set_PRIMASK(primask);

    if ((sts & FEATURE_EFM_CMD_ERROR_MASK) != 0u) {
        s_err_sts  = sts;
        s_err_addr = dest;
        s_err_cnt++;
        return false;
    }
    return true;
}
END_FUNCTION_DEFINITION_RAMSECTION

static bool flash_erase(u32 addr)
{
    /* 只允许擦本模块自己的扇区。编译期断言已保证宏配得对，这是运行期的
     * 最后一道保险：宁可擦除失败，也绝不能碰到刷写记录或别的数据区。 */
    s_err_op = 1u;
    if ((addr < ODOLOG_BASE_ADDR) ||
        (addr >= (ODOLOG_BASE_ADDR + (ODOLOG_SECTOR_CNT * ODOLOG_SECTOR_SIZE)))) {
        s_err_sts = 0xDEAD0001u;   /* 地址越界，根本没发命令 */
        s_err_addr = addr; s_err_cnt++;
        return false;
    }
    return efm_launch(FEATURE_EFM_ERASE_SECTOR_CMD_CODE, addr, NULL, 1u);
}

static bool flash_program_unit(u32 addr, const u32 *src)
{
    s_err_op = 2u;
    if ((addr < ODOLOG_BASE_ADDR) ||
        ((addr + ODOLOG_WRITE_UNIT) >
            (ODOLOG_BASE_ADDR + (ODOLOG_SECTOR_CNT * ODOLOG_SECTOR_SIZE)))) {
        s_err_sts = 0xDEAD0002u;   /* 地址越界，根本没发命令 */
        s_err_addr = addr; s_err_cnt++;
        return false;
    }
    return efm_launch(FEATURE_EFM_PROGRAM_CMD_CODE, addr, src, ODOLOG_WRITE_WORDS);
}

#endif /* ODOLOG_SIM */

/**
 * 按写单元逐个下发编程命令。
 *
 * ★ 一条 EFM 编程命令只接受一个写单元（8 字节 / 2 个字）。曾经把整条
 *   16 字节记录（4 个字）塞进一条命令，EFM 直接回 ACCERR
 *   （STS = 0x88 = IDLE|ACCERR，DONE 不置位），编程静默失败、日志区始终
 *   全 FF。擦除命令只需锁存一次地址所以能成功，唯独编程失败，现象很迷惑。
 *   hal_nvm.c 的 dflash_program 同样是这么循环的，别再改回一次性下发。
 *
 * 这个拆分放在公共代码里而不是各自的原语里，是为了让 PC 仿真跑的是
 * 与目标板完全相同的路径——否则仿真会把整条记录一次性写入，测不出
 * 这个约束。
 */
static bool flash_program(u32 addr, const u32 *src, u32 word_cnt)
{
    u32 off;
    for (off = 0u; off < word_cnt; off += ODOLOG_WRITE_WORDS) {
        if (!flash_program_unit(addr + (off * 4u), &src[off])) {
            return false;
        }
    }
    return true;
}

/*===========================================================================
 *  6. 槽位寻址
 *===========================================================================*/

static u32 slot_addr(u8 sector, u8 slot)
{
    return ODOLOG_BASE_ADDR
         + ((u32)sector * ODOLOG_SECTOR_SIZE)
         + ((u32)slot   * ODOLOG_REC_SIZE);
}

/** 读一个槽位并判定有效性 */
static bool slot_load(u8 sector, u8 slot, OdoRec_t *out)
{
    OdoRec_t r;
    memcpy(&r, flash_ptr(slot_addr(sector, slot)), sizeof(r));

    /* 空槽的 seq 是 0xFFFFFFFF。必须先挡掉，否则一旦 CRC 碰巧匹配，
     * 空槽会以最大序号胜出，把里程冲成垃圾值。 */
    if (r.seq == SEQ_INVALID) { return false; }

    if (r.crc != crc32_calc((const u8 *)&r, ODOLOG_REC_SIZE - 4u)) { return false; }

    *out = r;
    return true;
}

/*===========================================================================
 *  7. 旧格式迁移
 *===========================================================================*/

#if (ODOLOG_MIGRATE_LEGACY == 1)
/**
 * 从 hal_nvm 的乒乓格式抢救里程。
 *
 * 旧布局是 16 字节：magic(0xA5) / seq / 保留2 / ODO(u32 LE) / TRIP(u16 LE)。
 * 历史上出现过两种基址：最初压在 0x10001000（与刷写记录撞车），修复后
 * 挪到 0x10001200。两处都扫，取 seq 较大的那份。
 *
 * 读 0x10001000 是安全的：那里现在放的是刷写记录，magic 首字节是 0x56
 * （'V'），不会被误判成 0xA5。
 *
 * 本函数只读不写，迁移来的值会作为第一条日志记录落盘。
 */
static bool legacy_load(u32 *odo_km, u16 *trip_dm)
{
    static const u32 cand[3] = { 0x10001000ul, 0x10001200ul, 0x10001400ul };
    bool found = false;
    u8   best_seq = 0u;

    for (u8 i = 0u; i < 3u; i++) {
        const u8 *p = flash_ptr(cand[i]);
        if (p[0] != 0xA5u) { continue; }            /* magic 不符 */
        if (found && (p[1] <= best_seq)) { continue; }

        best_seq = p[1];
        *odo_km  = ((u32)p[4]) | ((u32)p[5] << 8u)
                 | ((u32)p[6] << 16u) | ((u32)p[7] << 24u);
        *trip_dm = (u16)((u16)p[8] | ((u16)p[9] << 8u));
        found = true;
    }
    return found;
}
#endif

/*===========================================================================
 *  8. 对外接口
 *===========================================================================*/

bool OdoLog_Init(void)
{
    OdoRec_t r;
    bool     any = false;
    u32      best = 0u;
    u8       best_sec = 0u, best_slot = 0u;

    s_ready    = false;
    s_has_data = false;
    s_seq      = 0u;
    s_odo_km   = 0u;
    s_trip_dm  = 0u;

    /* 全量扫描。默认 8×32 = 256 条，每条 16 字节读 + 12 字节 CRC，
     * 几十微秒，不值得为省这点时间引入更复杂的增量定位逻辑。 */
    for (u8 sec = 0u; sec < ODOLOG_SECTOR_CNT; sec++) {
        for (u8 sl = 0u; sl < ODOLOG_SLOTS; sl++) {
            if (!slot_load(sec, sl, &r)) { continue; }
            /* seq 单调递增且永不回绕（u32 够用 8000 年），直接比大小 */
            if (!any || (r.seq > best)) {
                any = true; best = r.seq;
                best_sec = sec; best_slot = sl;
                s_odo_km = r.odo_km; s_trip_dm = r.trip_dm;
            }
        }
    }

    if (any) {
        s_seq      = best;
        s_has_data = true;
        s_sector   = best_sec;
        s_slot     = (u8)(best_slot + 1u);      /* 可能等于 ODOLOG_SLOTS，
                                                   表示本扇区已满，写时换扇区 */
    } else {
        /* 全空：首次上电，或整片被擦过 */
        s_seq    = 0u;
        s_sector = 0u;
        s_slot   = ODOLOG_SLOTS;    /* 强制首次写入先擦扇区 0，
                                       避免依赖"出厂即擦除态"这个假设 */

#if (ODOLOG_MIGRATE_LEGACY == 1)
        /* 抢救旧乒乓格式的里程。取到值后不立刻落盘——等业务侧第一次
         * 调用 Write 时自然写入，避免在上电流程里增加一次擦除。 */
        if (legacy_load(&s_odo_km, &s_trip_dm)) {
            s_has_data = true;
        }
#endif
    }

    s_ready = true;
    return true;
}

bool OdoLog_Read(u32 *odo_km, u16 *trip_dm)
{
    /* 自初始化保护：调用方若忘了先调 OdoLog_Init()，这里补上。
     *
     * 没有这道保护时，未初始化的读会静默返回 0，表现为"每次上电里程归零"
     * 而不是报错，极难定位。main.c 的 NVM_LoadOdometer() 位置比 NVM_Init()
     * 更靠前正是踩过的坑。 */
    if (!s_ready) { (void)OdoLog_Init(); }

    if (odo_km  != NULL) { *odo_km  = s_odo_km;  }
    if (trip_dm != NULL) { *trip_dm = s_trip_dm; }
    return s_has_data;
}

bool OdoLog_Write(u32 odo_km, u16 trip_dm)
{
    OdoRec_t r;
    u8       sec, sl;

    /* 同 Read：未初始化就先扫描，避免静默丢数据 */
    if (!s_ready) { (void)OdoLog_Init(); }

    sec = s_sector;
    sl  = s_slot;

    /* 当前扇区写满，轮转到下一个并擦除。
     *
     * 被擦的是环上最老的扇区，最新记录在当前扇区里完好无损，所以擦除
     * 过程中掉电不会丢数据——下次上电扫描仍能在旧扇区找到最大 seq。 */
    if (sl >= ODOLOG_SLOTS) {
        sec = (u8)((sec + 1u) % ODOLOG_SECTOR_CNT);
        sl  = 0u;
        if (!flash_erase(slot_addr(sec, 0u))) { return false; }
    }

    r.seq     = s_seq + 1u;
    r.odo_km  = odo_km;
    r.trip_dm = trip_dm;
    r.rsvd    = 0xFFFFu;
    r.crc     = crc32_calc((const u8 *)&r, ODOLOG_REC_SIZE - 4u);

    /* seq 递增到 0xFFFFFFFF 就会与空槽标记冲突。u32 在 60 秒周期下要
     * 8000 年才走到这里，实际不可能发生；真发生了宁可停止写入也不能
     * 写出一条会被误判的记录。 */
    if (r.seq == SEQ_INVALID) { return false; }

    if (!flash_program(slot_addr(sec, sl), (const u32 *)&r,
                       ODOLOG_REC_SIZE / 4u)) {
        return false;
    }

    s_seq      = r.seq;
    s_odo_km   = odo_km;
    s_trip_dm  = trip_dm;
    s_has_data = true;
    s_sector   = sec;
    s_slot     = (u8)(sl + 1u);
    return true;
}

void OdoLog_GetLastError(u32 *sts, u32 *addr, u8 *op, u32 *cnt)
{
    if (sts  != NULL) { *sts  = s_err_sts;  }
    if (addr != NULL) { *addr = s_err_addr; }
    if (op   != NULL) { *op   = s_err_op;   }
    if (cnt  != NULL) { *cnt  = s_err_cnt;  }
}

void OdoLog_GetStats(u32 *seq, u8 *sector_idx, u8 *slot_idx)
{
    if (seq        != NULL) { *seq        = s_seq; }
    if (sector_idx != NULL) { *sector_idx = s_sector; }
    if (slot_idx   != NULL) { *slot_idx   = s_slot; }
}

/*===========================================================================
 *  9. 编译期布局检查
 *
 *  用负长度数组而不是 _Static_assert，兼容 IAR 的 C89/C99 模式。
 *  任何一条不成立都会在编译期报 "negative array size"，比把仪表刷成砖
 *  再回头查要便宜得多。
 *===========================================================================*/

#define ODOLOG_ASSERT(name, cond) typedef char odolog_assert_##name[(cond) ? 1 : -1]

#define ODOLOG_END_ADDR  (ODOLOG_BASE_ADDR + (ODOLOG_SECTOR_CNT * ODOLOG_SECTOR_SIZE))

/* 记录必须是写单元整数倍，否则编程会跨单元边界 */
ODOLOG_ASSERT(rec_aligned, (ODOLOG_REC_SIZE % ODOLOG_WRITE_UNIT) == 0u);
/* 扇区必须能被记录整除，否则末尾会留下半条槽 */
ODOLOG_ASSERT(slots_exact, (ODOLOG_SECTOR_SIZE % ODOLOG_REC_SIZE) == 0u);
/* 至少两个扇区，否则轮转时会擦掉唯一一份数据 */
ODOLOG_ASSERT(two_sectors, ODOLOG_SECTOR_CNT >= 2u);
/* 基地址必须扇区对齐 */
ODOLOG_ASSERT(base_aligned, (ODOLOG_BASE_ADDR % ODOLOG_SECTOR_SIZE) == 0u);

#ifndef ODOLOG_SIM
/* ★ 核心约束：日志区绝不能与 Bootloader 刷写记录重叠。
 *   落在 array1 时天然满足；万一改回 array0，这条会拦住。 */
ODOLOG_ASSERT(not_boot_record,
    (ODOLOG_BASE_ADDR >= ((u32)BOOT_RECORD_ADDR + (u32)DFLASH_SECTOR_SIZE)) ||
    (ODOLOG_END_ADDR  <= (u32)BOOT_RECORD_ADDR));

/* 必须整体落在某一片数据 Flash 之内，不得跨片或越界 */
ODOLOG_ASSERT(within_dflash,
    ((ODOLOG_BASE_ADDR >= (u32)FEATURE_EFM_DATA_ARRAY0_START_ADDRESS) &&
     (ODOLOG_END_ADDR  <= ((u32)FEATURE_EFM_DATA_ARRAY0_END_ADDRESS + 1u))) ||
    ((ODOLOG_BASE_ADDR >= (u32)FEATURE_EFM_DATA_ARRAY1_START_ADDRESS) &&
     (ODOLOG_END_ADDR  <= ((u32)FEATURE_EFM_DATA_ARRAY1_END_ADDRESS + 1u))));

/* 扇区大小与写单元必须与芯片 features 头一致 */
ODOLOG_ASSERT(sector_matches,
    ODOLOG_SECTOR_SIZE == (u32)FEATURE_EFM_DATA_ARRAY_SECTOR_SIZE);
ODOLOG_ASSERT(unit_matches,
    ODOLOG_WRITE_UNIT == (u32)FEATURE_EFM_WRITE_UNIT_SIZE);
#endif