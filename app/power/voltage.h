/**
 * @file    voltage.h
 * @brief   12V 电源电压监测
 *
 * 数据来源：ADC_SE4（= HAL 通道 ADC_CH_POWER_12V）经分压后采集，
 *           由 HAL_ADC_ReadVbatMv() 折算回电池侧 mV。
 *
 * 判定区间（硬件规格 Section 1.2）：
 *
 *   VOLT_STATE_UNDER    V < 6.5V            总线不能工作，CAN 通信不可用
 *   VOLT_STATE_LOW      6.5V ≤ V < 9V       总线可工作，CAN 通信正常，
 *                                           但低于仪表正常工作电压
 *   VOLT_STATE_NORMAL   9V ≤ V ≤ 18V        正常工作
 *   VOLT_STATE_OVER     V > 18V             总线不能工作，CAN 通信不可用
 *
 * 每个阈值带 ±0.2V（CFG_VOLTAGE_TOLERANCE_MV）迟滞，且状态切换需连续
 * 维持 CFG_VOLTAGE_DEBOUNCE_MS 才生效，避免起动拖动时误判。
 *
 * 建议调度周期：20ms
 */

#ifndef CLUSTER_VOLTAGE_H
#define CLUSTER_VOLTAGE_H

#include "../common/cluster_context.h"

/**
 * @brief 电压监测初始化
 * @note 须在 HAL_ADC_Init() 之后调用
 */
void VOLT_Init(ClusterSystem_t *sys);

/**
 * @brief 电压监测周期任务（建议 20ms）
 *
 * 职责：采样 → 一阶低通滤波 → 带迟滞与确认时间的状态判定
 *       → 写入 sys->voltage
 */
void VOLT_Task(ClusterSystem_t *sys);

/** @brief 取滤波后电压（mV） */
u16 VOLT_GetMv(const ClusterSystem_t *sys);

/** @brief CAN 通信电压是否可用（6.5V ≤ V ≤ 18V） */
bool VOLT_IsCanUsable(const ClusterSystem_t *sys);

/** @brief 是否处于仪表正常工作电压（9V ≤ V ≤ 18V） */
bool VOLT_IsNormal(const ClusterSystem_t *sys);

#endif /* CLUSTER_VOLTAGE_H */