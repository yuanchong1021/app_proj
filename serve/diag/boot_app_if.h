/**
 * @file    boot_app_if.h
 * @brief   Boot 与 App 之间的接口契约（两个工程共用同一份）
 *
 * ★ 本文件必须被 Boot 工程和 App 工程同时包含，且内容完全一致。
 *   建议放在版本库的公共目录，两边用相对路径引用，不要各存一份拷贝。
 *   里面定义的地址、结构体、魔数一旦两边不一致，表现是刷写能完成但
 *   Boot 永远不跳 App，且没有任何报错——非常难查。
 *
 * ── Flash 分区 ──────────────────────────────────────────────────────────
 *   0x00000000 ─ 0x000003FF   Boot 中断向量表      1KB
 *   0x00000400 ─ 0x00007FFF   Boot 代码            31KB
 *   0x00008000 ─ 0x000083FF   App  中断向量表      1KB
 *   0x00008400 ─ 0x0000846F   App  信息块          112B
 *   0x00008470 ─ 0x0003FFFF   App  代码            ~223KB
 *   0x10001000 ─ 0x100037FF   数据 Flash（刷写记录）
 *
 * ── SRAM 分区（以链接脚本为准，本表仅作说明）────────────────────────────
 *   0x20000000 ─ 0x200003FF   运行时中断向量表副本    IVT_RAM
 *   0x20000400 ─ 0x20007BF7   .data / .bss / .code_ram    RAM
 *   0x20007BF8 ─ 0x20007BFF   Boot/App 共享区         SharedSegment
 *   0x20007C00 ─ 0x20007FFF   栈（1024 字节）         STACK
 *
 *   ★ 共享区的地址由 board/yt_linker.icf 第 46~48 行的 SharedSegment
 *     区域决定。代码里一律通过 g_bootShared 访问，不要再硬编码地址，
 *     这样以后挪动共享区只需要改 icf。
 *
 *   ★ 跨复位保留的机制：g_bootShared 放在 .SharedSegment 段，
 *     RamInit1.c 的 CopyLayout 里该段是 INIT_POR —— 只有上电复位才从
 *     Flash 拷回初值，软复位时原样保留；RamInit0.S 对 SharedSegment 的
 *     0x5A5A5A5A 填充同样被 RCU->RSSR 的 POR 位门控住。
 *     两条都依赖「RSSR 在每次复位后被读走并清掉」，见 boot_board.c。
 *
 *   ★ App 侧的 app_side/yt_linker.icf 目前还没有 SharedSegment 区域，
 *     且 RAM_memory_end 是 0x20007BFF（正好盖住共享区）。做 App 时必须
 *     照 Boot 这份 icf 补齐，并把 STACK 改回 0x20007C00~0x20007FFF/1024。
 */
#ifndef BOOT_APP_IF_H
#define BOOT_APP_IF_H

#include <stdint.h>

/*===========================================================================
 *  1. Flash 分区
 *===========================================================================*/
#define FLASH_BASE_ADDR         (0x00000000UL)
#define FLASH_END_ADDR          (0x0003FFFFUL)   /**< 256KB，见 features 头 */
#define FLASH_SECTOR_SIZE       (0x200UL)        /**< 扇区 512 字节         */
#define FLASH_WRITE_UNIT        (8UL)            /**< 写入单元 8 字节       */

#define BOOT_START_ADDR         (0x00000000UL)
#define BOOT_END_ADDR           (0x00007FFFUL)

#define APP_START_ADDR          (0x00008000UL)
#define APP_END_ADDR            (0x0003FFFFUL)
#define APP_MAX_SIZE            (APP_END_ADDR - APP_START_ADDR + 1UL)

/** App 信息块地址：紧跟 App 向量表之后 */
#define APP_INFO_ADDR           (0x00008400UL)

/*===========================================================================
 *  2. 数据 Flash
 *===========================================================================*/
#define DFLASH_BASE_ADDR        (0x10001000UL)
#define DFLASH_SECTOR_SIZE      (0x200UL)
#define BOOT_RECORD_ADDR        (DFLASH_BASE_ADDR)

/*===========================================================================
 *  3. Boot / App 共享变量
 *
 *  实体定义在 shared/boot_shared.c，Boot 工程与 App 工程各自把这一个源
 *  文件加进编译即可，不要各存一份拷贝。
 *
 *  ⚠ 结构体大小必须正好等于 icf 里 SharedSegment 区域的长度（当前 8 字节）。
 *    icf 第 386 行有 check that size(block SharedSegment_block) ==
 *    size(block SharedSegment_rom_block)，超长会在链接期报错；少了不会报错，
 *    加字段时自己核对。
 *===========================================================================*/
#define BOOT_FLAG_PROG_REQUEST  (0x72717374UL)   /**< "rqst" */
#define BOOT_FLAG_NONE          (0x00000000UL)

typedef struct {
    uint32_t flag;        /**< BOOT_FLAG_NONE / BOOT_FLAG_PROG_REQUEST */
    uint32_t reserved;    /**< 预留，同时把结构体补齐到 8 字节 */
} BootShared_t;

extern BootShared_t g_bootShared;

/*===========================================================================
 *  4. App 信息块
 *
 *  App 侧用 #pragma location 把 g_appInfo 定位到 APP_INFO_ADDR；
 *  Boot 侧直接按该地址读，用于：
 *    - 回答 0x22 的四个标识 DID（步骤 7~10 / 29~32）
 *    - 步骤 23 的依赖检查（校验 magic 与 signature）
 *===========================================================================*/
#define APP_INFO_MAGIC          (0x4F464E49UL)       /**< 'I','N','F','O' 小端 */
#define APP_SIGNATURE_TEXT      "INSTRUMENT-SX12 "   /**< 恰好 16 字符 */
#define APP_SIGNATURE_LEN       (16U)

typedef struct {
    uint32_t magic;              /**< APP_INFO_MAGIC                      */
    uint32_t infoVersion;        /**< 结构版本，当前 1                     */
    uint8_t  swVersion[16];      /**< F189 OEM 软件版本号（对外 10 字节） */
    uint8_t  swNumber[16];       /**< F188 OEM 软件号（对外 16 字节）     */
    uint8_t  hwVersion[16];      /**< F191 OEM 硬件版本号（对外 10 字节） */
    uint8_t  partNumber[16];     /**< F187 OEM 零部件号（对外 16 字节）   */
    uint8_t  signature[16];      /**< 项目签名，须等于 APP_SIGNATURE_TEXT */
    uint32_t reserved[6];
} AppInfo_t;                     /* 112 字节，8 的倍数 */

/*===========================================================================
 *  5. 刷写记录（存于数据 Flash，仅 Boot 读写）
 *===========================================================================*/
#define BOOT_RECORD_MAGIC       (0x444C4156UL)   /**< 'V','A','L','D' 小端 */
#define FINGERPRINT_LEN         (66U)            /**< F15C 长度，见流程步骤 18 */

typedef struct {
    uint32_t magic;                        /**< BOOT_RECORD_MAGIC       */
    uint32_t appLen;                       /**< 已刷写的 App 字节数      */
    uint32_t appCrc;                       /**< 校验通过的 CRC32        */
    uint32_t progCount;                    /**< 累计刷写成功次数         */
    uint8_t  fingerprint[FINGERPRINT_LEN]; /**< F15C 刷写指纹           */
    uint8_t  pad[2];
    uint32_t recCrc;                       /**< 前 84 字节的 CRC32      */
} BootRecord_t;                            /* 88 字节，8 的倍数 */

/*===========================================================================
 *  6. 诊断 CAN ID（Boot 与 App 必须一致）
 *===========================================================================*/
#define UDS_ID_PHYS_REQ         (0x7C6UL)   /**< 诊断仪 → ECU，物理寻址 */
#define UDS_ID_FUNC_REQ         (0x7DFUL)   /**< 诊断仪 → ECU，功能寻址 */
#define UDS_ID_RESP             (0x7CEUL)   /**< ECU → 诊断仪            */

#endif /* BOOT_APP_IF_H */