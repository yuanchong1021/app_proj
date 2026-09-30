/**
 * @file    pwr_mgr.h
 * @brief   电源 / 唤醒 / 睡眠状态机
 *
 * 状态转换规则（与硬件规格 Section 1.2 对齐）：
 *
 *   PWR_OFF ─── ign_on=true ──→ PWR_ON （cluster_state = WAKE）
 *   PWR_OFF ─── bat_on 或 硬线唤醒事件 ──→ cluster_state = WAKE（保留 PWR_OFF）
 *   PWR_ON  ─── ign_on=false ──→ PWR_OFF（同时关背光、保存里程到 NVM）
 *
 * 另外：IGN 硬线的 OFF/ACC → ON 边沿（去抖后）会触发 SYS_StartSelfCheck()，
 * 每次点火 ON 重新走一轮指示灯自检。该检测独立于上面的状态机，见 pwr_mgr.c。
 *
 * SOC / 收音机 / 功放三路供电不直接跟随上面的状态机，而是：
 *   供电 = (power_state == PWR_ON) && (9V ≤ VBAT ≤ 18V)
 * 电压超出范围立即切断这三路，恢复后按原顺序重新上电（SOC/收音机 →
 * 5s → 功放）。门限判定复用 voltage.c，确认时间见 cfg_power.h。
 */

#ifndef CLUSTER_PWR_MGR_H
#define CLUSTER_PWR_MGR_H

#include "../common/cluster_context.h"

/**
 * @brief 电源管理模块初始化
 *
 * 上电默认进入 PWR_OFF 状态，待硬线信号（IGN/BAT/转向/车门）触发唤醒。
 */
void PWR_Init(ClusterSystem_t *sys);

/**
 * @brief 电源管理周期任务（建议 5ms 调用）
 *
 * 职责：
 *   1) 根据 IGN / BAT / 转向 / 车门 等硬线状态切换电源/唤醒状态
 *   2) 在 PWR_ON → PWR_OFF 跳变时关闭背光、保存里程到 NVM
 *   3) 在 IGN OFF/ACC → ON 的确认边沿上重新启动指示灯自检
 *   4) 按电压是否在 9~18V 内，通断 SOC / 收音机 / 功放三路供电
 */
void PWR_Task(ClusterSystem_t *sys);

#endif /* CLUSTER_PWR_MGR_H */