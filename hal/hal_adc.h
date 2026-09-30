/**
 * @file  hal_adc.h
 * @brief ADC HAL 接口（DMA + 中断驱动）
 */
#ifndef CLUSTER_HAL_ADC_H
#define CLUSTER_HAL_ADC_H

#include "types.h"
#include <stdbool.h>

/*---------------------------------------------------------------------------
 *  ADC 通道索引（= s_result[] 下标，与 adc_config.c 的序列扫描顺序一致）
 *
 *  当前序列只有 2 路（ADC_CHANNEL_COUNT = 2）：
 *    index 0 = ADC_SE4 → 12V 电源分压采集   （已由整车确认）
 *    index 1 = ADC_SE5 → 燃油传感器电阻
 *
 *  ★ 原 hal_adc.h 把 index 0 定义为 ADC_CH_STEERING（方向盘按键），
 *    与 hal_adc.c 的 ADC_IDX_POWER_12V = 0 冲突。现按整车确认统一为电源。
 *
 *  ★ 方向盘按键、外温 NTC 目前都不在采样序列内。启用前必须先扩展
 *    ADC_CHANNEL_COUNT、adc_config.c 的序列与 DMA 传输长度，
 *    否则读到的是别的通道的数据。
 *---------------------------------------------------------------------------*/
#define ADC_CH_POWER_12V 0u   /**< ADC_SE4：12V 电源分压 */
#define ADC_CH_FUEL      1u   /**< ADC_SE5：燃油传感器   */

/* 未启用的通道故意不给可用编号，误用会被 HAL 的边界检查挡住。
 * 启用时改成实际序列下标。 */
#define ADC_CH_STEERING  0xFFu  /**< 未接入采样序列 */
#define ADC_CH_AMBIENT   0xFFu  /**< 未接入采样序列 */

#define SPI0_TX_DMA_CHANNEL             0u
#define SPI0_RX_DMA_CHANNEL             1u
#define ADC_DMA_CHANNEL                 2u

/**
 * @brief 初始化 ADC DMA 循环采样
 * 须在 ADC_DRV_ConfigConverter + DMA_DRV_Init 之后调用一次。
 */
void HAL_ADC_Init(void);

/**
 * @brief 读取传感器电阻值（Ω）
 * @param channel  ADC_CH_FUEL
 * @return 电阻值(Ω)；0=短路，0xFFFF=开路或通道非法
 */
u16 HAL_ADC_ReadOhms(u8 channel);

/**
 * @brief 读取原始 ADC 值（12 位）
 * @param channel  通道索引
 * @return 0~4095；通道非法返回 0
 */
u16 HAL_ADC_ReadRaw(u8 channel);

/**
 * @brief 读取外温 NTC 原始 ADC 值（12 位）
 *
 * @warning 外温 NTC 当前不在采样序列内。调用方若传入 0，读到的是
 *          12V 电源分压而不是 NTC —— app/gauges/ambient.c 目前正是
 *          这么调的（硬编码 0），需要一并修正或暂时停用外温功能。
 */
s16 HAL_ADC_ReadTempRaw(u8 channel);

/**
 * @brief 读取 12V 电源电压（mV，已按分压比折算回电池侧）
 *
 * 换算：V_adc = raw × CFG_ADC_VREF_MV / 4095
 *       VBAT  = V_adc × (R1 + R2) / R2
 * 三个常数见 cfg_power.h，必须按原理图确认。
 *
 * @return 电池电压(mV)；ADC 未就绪时返回 0
 */
u16 HAL_ADC_ReadVbatMv(void);

/** 查询是否已完成至少一轮 DMA 采样 */
bool HAL_ADC_IsReady(void);

/**
 * @brief ADC 就绪自愈轮询，需挂进调度表周期调用（建议 100ms）
 *
 * s_ready 长时间（ADC_READY_TIMEOUT_MS，默认 500ms）不为 true 时，
 * 视为 DMA/ADC 卡在了未就绪状态，自动重新调用 HAL_ADC_Init() 拉起来，
 * 避免一直卡死到下次整机重新上电才恢复。
 */
void HAL_ADC_Task(void);

#endif /* CLUSTER_HAL_ADC_H */