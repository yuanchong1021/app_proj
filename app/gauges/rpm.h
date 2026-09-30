/**
 * @file    rpm.h
 * @brief   转速模块（Section 9.2）
 *
 * 数据流：
 *   CAN EMS(0x360) → sys->can_rx.ems.engine_rpm_raw
 *                       ↓ RPM_Task
 *                   sys->lcd.rpm + 步进电机角度
 */

#ifndef CLUSTER_RPM_H
#define CLUSTER_RPM_H

#include "../common/cluster_context.h"

/** 转速模块初始化 */
void RPM_Init(ClusterSystem_t *sys);

/**
 * @brief 转速模块周期任务（建议 10ms 调用）
 *
 * 行为：
 *   1) EMS 报文连续 100ms 超时 → 转速归零
 *   2) rpm_fault 时不更新转速
 *   3) 原始值换算：rpm = engine_rpm_raw × 25 / 100
 *   4) 上限钳制到 CFG_RPM_MAX，驱动转速表步进电机
 */
void RPM_Task(ClusterSystem_t *sys);

/** 取当前显示转速 */
u16 RPM_GetDisplay(const ClusterSystem_t *sys);

#endif /* CLUSTER_RPM_H */
