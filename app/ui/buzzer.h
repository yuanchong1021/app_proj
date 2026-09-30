/**
 * @file    buzzer.h
 * @brief   蜂鸣器报警模块（Section 7）
 *
 * 报警优先级（数值越大越高）：
 *   BUZZER_IDLE     = 0   无报警
 *   BUZZER_GAS_LEAK = 1   燃气泄漏（一次性 12 声）
 *   BUZZER_SEATBELT = 2   安全带未系（5 轮 × 6 声）
 *
 * 抢占规则：
 *   高优先级可以抢占低优先级；安全带被抢占后会通过 pending_restore 恢复继续。
 *
 * 触发来源：
 *   - 安全带：本模块在每个周期内自行判定
 *             （IGN ON 且 车速 > 5kmh 且 未系）
 *   - 燃气泄漏：由 led_ind.c 中 LED_UpdateGasLeak 触发 BUZZER_Trigger
 *
 * 【本次新增】IGN 下电 → 安全带报警音立即关闭：
 *   IGN（去抖后）为 OFF 时安全带报警条件恒不成立，正在鸣叫的会被立刻掐断，
 *   5 轮闭锁 seatbelt_latched 同时解除，下一次 IGN ON 视为全新的报警循环。
 *   去抖时间见 cfg_buzzer.h::CFG_BUZZER_IGN_DEBOUNCE_MS。
 *   燃气泄漏报警不受 IGN 影响（安全事件，熄火后仍可鸣叫）。
 *
 * 【说明】本任务不受上电自检影响，自检期间报警音照常播放。
 */

#ifndef CLUSTER_BUZZER_H
#define CLUSTER_BUZZER_H

#include "../../common/cluster_context.h"

/** 蜂鸣器初始化（清零状态机 + 关 HAL 输出） */
void BUZZER_Init(ClusterSystem_t *sys);

/**
 * @brief 蜂鸣器周期任务（建议 10ms 调用）
 *
 * 行为：
 *   0) IGN 电平去抖；IGN OFF 时安全带报警立即停止并解除闭锁
 *   1) 自动触发安全带报警（IGN ON 且 车速 > 5kmh 且 seatbelt_n=true）
 *   2) 高优先级结束后恢复 pending_restore
 *   3) 调用对应报警的鸣叫子状态机
 */
void BUZZER_Task(ClusterSystem_t *sys);

/**
 * @brief 触发一个报警（外部模块入口）
 *
 * @param sys    系统上下文
 * @param alarm  报警类型
 *
 * 规则：
 *   - 当前已有更高/同优先级报警 → 忽略本次请求
 *   - 当前为 SEATBELT，新请求更高 → 记录 pending_restore=SEATBELT
 *   - 否则切换到新报警，重置内部计数
 */
void BUZZER_Trigger(ClusterSystem_t *sys, BuzzerAlarm_t alarm);

#endif /* CLUSTER_BUZZER_H */