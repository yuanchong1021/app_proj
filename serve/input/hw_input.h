/**
 * @file    hw_input.h
 * @brief   硬线输入聚合模块
 *
 * 该模块负责：
 *   1. 在每个调度周期（建议 1ms）内一次性扫描所有 GPIO / ADC 通道
 *   2. 把读到的电平 / 数值快照写入 ClusterSystem_t::hw_input
 *   3. 对低电平有效信号做"先取反再写入"的归一化处理
 *
 * 上层模块（pwr_mgr / led_ind / odo / fuel 等）只读取该快照，
 * 避免每次都直接访问 HAL_GPIO / HAL_ADC。
 *
 * 设计上属于服务层（Service）：屏蔽了"裸 GPIO 读取"细节，
 * 同时为应用层提供了带语义的字段名。
 */

#ifndef CLUSTER_HW_INPUT_H
#define CLUSTER_HW_INPUT_H

#include "../common/cluster_context.h"

/**
 * @brief 硬线输入采样模块初始化
 *
 * 当前实现为空（GPIO 方向 / 上下拉由 driver 层在 HAL_Init 中统一配置）。
 * 保留接口以便将来扩展（例如开启 DMA、配置 ADC 转换序列等）。
 */
void HW_Init(void);

/**
 * @brief 硬线输入采样任务（建议 1ms 调用）
 *
 * @param sys  系统上下文，结果写入 sys->hw_input
 *
 * 流程：
 *   1) 读取所有数字开关量 → 高电平有效字段直接赋值，低电平有效字段取反
 *   2) 读取 ADC 通道：燃油电阻、方向盘按键电阻、外温原始值
 */
void HW_SampleTask(ClusterSystem_t *sys);

#endif /* CLUSTER_HW_INPUT_H */
