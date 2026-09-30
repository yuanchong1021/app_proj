/**
 * @file    lowpower.h
 * @brief   ACC OFF 低功耗（休眠 / 唤醒）状态机
 *
 * ===========================================================================
 *  状态机
 * ===========================================================================
 *
 *      ┌──────────────────────────────────────────────────────┐
 *      │                                                      │
 *      ▼                                                      │
 *   LPM_RUN ──(休眠条件连续满足 CFG_LPM_ACC_OFF_HOLD_MS)──► LPM_PRE_SLEEP
 *      ▲                                                      │
 *      │                                          (关负载、存 NVM、清屏)
 *      │                                          等 CFG_LPM_PRESLEEP_MS
 *      │                                                      ▼
 *   LPM_WAKEUP ◄──────(唤醒中断，且唤醒源有效)────────── LPM_SLEEP
 *  (恢复外设、重挂 CAN、                                （MCU DeepSleep，
 *   重置时间戳、按需自检)                                 WFI 阻塞在此）
 *
 *  休眠条件（**全部**满足才算）：
 *     ① IGN 硬线 OFF 且 power_state != PWR_ON
 *     ② 转向 / 双闪不活动，且已消失超过 CFG_LPM_TURN_HOLD_MS
 *     ③ 车门已关，且关闭超过 CFG_LPM_DOOR_HOLD_MS
 *     ④ CAN 总线静默超过 CFG_LPM_CAN_IDLE_MS
 *     ⑤ 蜂鸣器不在鸣叫
 *     ⑥ 本次唤醒已保持清醒超过 CFG_LPM_MIN_AWAKE_MS
 *     ⑦ 没有模块通过 LPM_Inhibit() 申请禁止休眠
 *
 *  ★ 为什么要有 ②③ 的"消失后再保持一段"★
 *  双闪的硬线是周期性断开的（约 0.8s 一次）。若信号一消失就允许睡，
 *  仪表会跟着双闪反复睡/醒，转向图标乱闪、功耗反而比不睡还高。
 *
 *  ★ 更重要的原因：当前唤醒源只有 IGN ★
 *  车门 / 转向 / CAN 只作为休眠抑制条件，**不能唤醒**已经睡着的仪表。
 *  所以一旦在双闪亮着时睡过去，转向图标会定格熄灭，要等拧钥匙才恢复。
 *  ②③ 的保持窗口在这个前提下是唯一的兜底，不是可有可无的优化。
 *  详见 cfg_lowpower.h 的"休眠抑制条件"一节。
 *
 * ===========================================================================
 *  与 pwr_mgr.c 的分工
 * ===========================================================================
 *   pwr_mgr.c  管"给谁供电"：SOC / 收音机 / 功放三路 + 背光 + 点火状态机。
 *   lowpower.c 管"MCU 自己睡不睡"：只在 pwr_mgr 已经断完外设电之后才动手。
 *   两者不重叠，lowpower.c 不修改 power_state。
 */

#ifndef CLUSTER_LOWPOWER_H
#define CLUSTER_LOWPOWER_H

#include "../common/cluster_context.h"

/**
 * @brief 低功耗模块初始化（Cluster_Init 中调用一次）
 *
 * 置状态为 LPM_RUN，并把"最近一次活动时刻"设为当前时刻，
 * 保证上电后至少 CFG_LPM_ACC_OFF_HOLD_MS 内不会立刻进睡。
 */
void LPM_Init(ClusterSystem_t *sys);

/**
 * @brief 低功耗周期任务（建议 10ms 调用，登记在 scheduler 中）
 *
 * ⚠ 本函数在进入休眠时会**阻塞**在 WFI，直到被唤醒中断打断。
 *   这是有意设计：合作式调度器里没有别的地方可以安全地停下来。
 *   阻塞期间不会有任何任务运行，唤醒后本函数完成恢复再返回，
 *   调度器从下一轮继续正常运行。
 */
void LPM_Task(ClusterSystem_t *sys);

/**
 * @brief 临时禁止 / 允许休眠
 *
 * @param inhibit true = 禁止休眠，false = 解除禁止
 *
 * 供有"不能被打断的长事务"的模块使用（例如正在写 EEPROM、
 * 正在做 UDS 诊断刷写）。注意这是**单一标志**不是计数器，
 * 多个模块同时使用时需要各自维护自己的条件后再统一置位。
 */
void LPM_Inhibit(ClusterSystem_t *sys, bool inhibit);

/**
 * @brief 喂一次"活动"（重置休眠倒计时）
 *
 * 任何认为"用户还在操作仪表"的事件都可以调用，例如按键、
 * 串口收到 SOC 的指令等。等价于把休眠倒计时拉回起点。
 */
void LPM_KeepAwake(ClusterSystem_t *sys);

#endif /* CLUSTER_LOWPOWER_H */