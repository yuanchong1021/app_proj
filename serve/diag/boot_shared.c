/**
 * @file    boot_shared.c
 * @brief   Boot 与 App 之间的共享变量实体定义（两个工程共用同一份源文件）
 *
 * ★ 这个文件必须同时加进 Boot 工程和 App 工程编译，且两边都用相对路径
 *   引用同一份，不要各存一份拷贝。
 *
 * ── 为什么用段而不用硬编码地址 ──────────────────────────────────────────
 *
 *   旧写法是 *(volatile u32 *)0x20007FFC。问题有两个：
 *
 *   1. 那个地址落在 STACK 区（0x20007C00~0x20007FFF）里面，而
 *      RamInit0.S 对 STACK 的 0x5A5A5A5A 填充是每次复位都做、不看复位源的
 *      —— 只有 SharedSegment 那一段被 POR 位门控住了。标志根本存不住。
 *
 *   2. 链接脚本挪一次共享区，所有硬编码点都要跟着改，漏一处就是
 *      「刷写能完成但 Boot 永远不跳 App，且没有任何报错」。
 *
 *   改用 .SharedSegment 段之后，共享区的位置只由 board/yt_linker.icf
 *   第 46~48 行决定，代码不用动。
 *
 * ── 为什么是「普通已初始化变量」而不是 __no_init ────────────────────────
 *
 *   icf 里 .SharedSegment 同时列在 initialize manually 和 do not initialize
 *   里（和 .data / .code_ram 一样，是 SDK 的固定写法）：
 *     - initialize manually  → ILINK 为该段生成 .SharedSegment_init，
 *                              内容存在 Flash 的 TEXT 区（icf 第 192~202 行）
 *     - do not initialize    → ILINK 的启动代码不自动做这次拷贝
 *   真正的拷贝由 RamInit1.c 的 CopyLayout[2] 完成，而它的 InitType 是
 *   INIT_POR —— 只有 IsPOR() 为真时才拷。
 *
 *   于是语义正好是想要的：
 *     上电复位 → flag 被拷成 BOOT_FLAG_NONE（确定值，不是随机数）
 *     软复位   → 原样保留，App 写进去的请求标志能传给 Boot
 *
 *   如果写成 __no_init，上电时 flag 是随机的，就又得靠复位源判断去兜底了。
 */

#include "boot_app_if.h"

#if defined(__ICCARM__)

/* 段名必须与 icf 里 block SharedSegment { section .SharedSegment } 完全一致。
 * __root 防止变量在某个编译单元里没被引用时被链接器丢掉
 * （icf 第 388~391 行已经有 keep，这里是双保险）。
 *
 * 等价写法：BootShared_t g_bootShared @ ".SharedSegment" = { ... };
 */
#pragma location = ".SharedSegment"
__root BootShared_t g_bootShared = { BOOT_FLAG_NONE, 0UL };

#else
#error "boot_shared.c 目前只适配了 IAR，换工具链需自行给出段放置写法"
#endif