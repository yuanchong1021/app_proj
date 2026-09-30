/**
 * @file    speed.h
 * @brief   车速模块（Section 9.1）
 *
 * 数据流：
 *   CAN ABS(0x330) → sys->can_rx.abs.vehicle_speed_raw
 *                        ↓ SPEED_Task
 *                   sys->lcd.speed_kmh + 步进电机角度
 */

#ifndef CLUSTER_SPEED_H
#define CLUSTER_SPEED_H

#include "cluster_context.h"

/**
 * @brief 车速模块初始化（清零内部状态与显示缓存）
 */
void SPEED_Init(ClusterSystem_t *sys);

/**
 * @brief 车速模块周期任务（建议 10ms 调用）
 *
 * 行为：
 *   1) 处理 ABS 报文超时（连续 200ms 未收 → 车速归零）
 *   2) 校验 speed_valid 位（0=有效）
 *   3) 原始值线性化 + 校准缩放系数
 *   4) 抑制低速抖动 + 滞回更新
 *   5) 同时驱动步进电机与 sys->lcd.speed_kmh 缓存
 */
void SPEED_Task(ClusterSystem_t *sys);

/**
 * @brief 取当前显示车速（kmh）
 */
u16 SPEED_GetDisplayKmh(const ClusterSystem_t *sys);

#endif /* CLUSTER_SPEED_H */
