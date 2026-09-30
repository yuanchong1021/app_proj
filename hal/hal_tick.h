/**
 * @file  hal_tick.h
 * @brief 系统时基 HAL（LPTMR0，1ms 中断）
 */
#ifndef CLUSTER_HAL_TICK_H
#define CLUSTER_HAL_TICK_H

#include "types.h"

/** 使能 LPTMR0 中断并启动计数，须在 Board_Init 后调用一次 */
void HAL_Tick_Init(void);

/** 返回系统毫秒时间戳（u32，约 49.7 天回绕） */
u32 HAL_GetTickMs(void);

/**
 * @brief 暂停 1ms 时基（进入休眠前调用）
 *
 * 停 LPTMR 计数并屏蔽其中断。s_tick_ms **冻结**在调用时刻，不再增长。
 *
 * ★ 冻结是有意设计，不是缺陷 ★
 * 休眠期间没有任何任务在跑，也没有报文在收。若让 tick 继续走，唤醒瞬间
 * 所有 last_recv_ms 会一次性变成"很久以前"，CAN_CheckTimeouts 会把全部
 * 报文判成超时，故障灯闪一大片。冻结之后时间戳与 tick 始终自洽。
 *
 * 代价：休眠时长不计入 system_tick_ms。仪表显示的北京时间来自 DS1307，
 * 由 RTC_Task 每秒重读，不受影响；里程/自检都是相对时间，也不受影响。
 */
void HAL_Tick_Suspend(void);

/** 恢复 1ms 时基（唤醒后调用），从冻结值继续累加 */
void HAL_Tick_Resume(void);

#endif /* CLUSTER_HAL_TICK_H */
