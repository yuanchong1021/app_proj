/**
 * @file    odo_log.h
 * @brief   里程日志式存储（自研，替代 hal_nvm 乒乓与云途 Fee）
 *
 * ── 为什么自研 ──────────────────────────────────────────────────────────
 *
 * 原 hal_nvm 乒乓：每次保存"擦备用扇区→写→擦旧活跃扇区"，且 ODO 与 TRIP
 * 分两次调用，一次保存擦 4 次扇区。60 秒周期下单扇区 10 万次寿命只够约
 * 833 小时行驶，按每天 2 小时算一年多就写穿。
 *
 * 云途 Fee：日志式追加，方向对，但每条块实例带 32 字节块头，存 6 字节
 * 有效数据要占 40 字节，放大 6.7 倍，一个扇区只装得下 12 条。
 *
 * 本模块：记录 16 字节（2 个写单元），一个扇区装 32 条，是 Fee 的 2.7 倍。
 * 60 秒周期 + 8 个扇区（仅 4 KB）下的寿命占用：
 *
 *   乘用车  2h/天 × 10 年 →  1.7%   余量 58 倍
 *   商用车 12h/天 × 15 年 → 15.4%   余量  6 倍
 *   极端   16h/天 × 20 年 → 27.4%   余量  4 倍
 *
 * ── 原理 ────────────────────────────────────────────────────────────────
 *
 * 不再把记录固定在某处反复重写，而是每次往下一个空槽追加一条新记录。
 * Flash 在已擦除区域内可以逐个写单元编程而不必重新擦除，所以一个扇区
 * 能连续追加 32 次，写满了才擦下一个扇区。扇区之间轮转，磨损自然均衡。
 *
 * 追加写从不修改已有数据，这是掉电安全的根本来源：任何时刻断电，上一条
 * 有效记录都完好无损。
 *
 * ── 有效性判据 ──────────────────────────────────────────────────────────
 *
 * 一条记录有效，当且仅当 seq != 0xFFFFFFFF 且 CRC32 校验通过。
 * 擦除态 12 字节 0xFF 的 CRC32 是 0xBB99FF8A，不等于末尾的 0xFFFFFFFF，
 * 所以空槽天然校验不过；seq 判据是第二道保险，防止写单元乱序落盘时
 * 空的 seq 被当成最大值。
 */

#ifndef CLUSTER_ODO_LOG_H
#define CLUSTER_ODO_LOG_H

#ifdef ODOLOG_SIM
#include <stdint.h>
#include <stdbool.h>
#else
#include "types.h"
#endif

/**
 * @brief 扫描日志区，建立内部状态
 *
 * 必须在任何 Read/Write 之前调用。内部会遍历所有扇区的所有槽位，
 * 找出 seq 最大的有效记录作为当前值，并定位下一个可写槽。
 *
 * 扫描量为 扇区数 × 32 条（默认 256 条），每条 16 字节读取加一次
 * 12 字节 CRC32，耗时在几十微秒量级，可以放在上电流程里同步执行。
 *
 * @return true = 日志区可用（含"全空、首次上电"的情况）
 *         false = Flash 访问异常，此后 Read 返回默认值、Write 直接失败
 */
bool OdoLog_Init(void);

/**
 * @brief 读取当前里程
 *
 * @param[out] odo_km   总里程，km
 * @param[out] trip_dm  小计里程，0.1 km
 * @return true = 读到有效记录；false = 日志区为空（首次上电）或未初始化，
 *         此时两个输出参数被置 0
 *
 * 数据在 Init 时已扫描到 RAM，本函数不访问 Flash，可随时调用。
 */
bool OdoLog_Read(u32 *odo_km, u16 *trip_dm);

/**
 * @brief 追加一条里程记录
 *
 * @param odo_km   总里程，km
 * @param trip_dm  小计里程，0.1 km
 * @return true = 已落盘
 *
 * 同步阻塞，返回时数据已在 Flash 中。这对以下调用点是必需的：
 *   - uds_jump_to_boot()：返回后紧接软复位
 *   - lowpower.c / pwr_mgr.c：返回后掉电
 *
 * 正常写入约 100 微秒（2 个写单元编程）。每 32 次写入中有一次需要先擦
 * 下一个扇区，那一次额外约 5.8 毫秒。
 */
bool OdoLog_Write(u32 odo_km, u16 trip_dm);

/**
 * @brief 取运行统计，用于诊断与产线检查
 *
 * @param[out] seq          当前记录的序号，等于本机历史累计写入次数
 * @param[out] sector_idx   当前活跃扇区序号 [0, 扇区数)
 * @param[out] slot_idx     下一个可写槽位 [0, 32]
 *
 * seq 可以用来估算 Flash 磨损程度：擦除次数 ≈ seq / 32 / 扇区数。
 * 任一出参可传 NULL。
 */
void OdoLog_GetStats(u32 *seq, u8 *sector_idx, u8 *slot_idx);

/**
 * @brief 取最近一次 Flash 操作失败的现场
 *
 * @param[out] sts   失败瞬间的 EFM->STS；特殊值 0xDEAD0001/0xDEAD0002
 *                   表示地址越界、命令根本没发出去
 * @param[out] addr  失败的目标地址
 * @param[out] op    1=擦除  2=编程
 * @param[out] cnt   累计失败次数；为 0 说明从未失败
 *
 * 写入失败本身是静默的（里程每分钟还会再存，单次失败可自愈），
 * 但排查时需要知道硬件到底拒绝在哪一步。任一出参可传 NULL。
 *
 * STS 关键位：ACCERR(0x8) 访问错误  CMD_FAIL(0x10) 命令失败
 *             CI_WEERR(0x2000000) 写使能错  CI_RDVIOLERR(0x4000000) 读违例
 */
void OdoLog_GetLastError(u32 *sts, u32 *addr, u8 *op, u32 *cnt);

#endif /* CLUSTER_ODO_LOG_H */