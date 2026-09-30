/**
 * @file    coolant.h
 * @brief   发动机水温模块（Section 8.4）
 *
 * 数据流：
 *   CAN EMS(0x306).coolant_temp_raw
 *        ↓ COOLANT_Task：原始值线性化 → 查格数表 → 设置 blink_high
 *   sys->lcd.coolant.{segments, blink_high}
 */

#ifndef CLUSTER_COOLANT_H
#define CLUSTER_COOLANT_H

#include "../common/cluster_context.h"

/** 水温模块初始化 */
void COOLANT_Init(ClusterSystem_t *sys);

/**
 * @brief 水温模块周期任务（建议 20ms 调用）
 *
 * 行为：
 *   1) EMS 报文超时 / 水温故障 → 等待 CFG_COOLANT_TIMEOUT_S 后置丢失态
 *   2) 正常情况：原始值 → ℃ → 9 段格数
 *   3) 当格数 ≥ CFG_COOLANT_WARN_SEGMENT 时置 blink_high，由 LCD 层闪烁高温段
 */
void COOLANT_Task(ClusterSystem_t *sys);

/** 取水温当前格数 */
u8 COOLANT_GetSegments(const ClusterSystem_t *sys);

/**
 * @brief 水温原始值线性化为摄氏度
 *
 * @param raw  CAN 原始字节
 * @return 摄氏度（s16），公式：raw × 0.75 - 48
 */
s16 COOLANT_RawToTemp(u8 raw);

#endif /* CLUSTER_COOLANT_H */