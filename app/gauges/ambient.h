/**
 * @file    ambient.h
 * @brief   车外温度模块（Section 9.3）
 *
 * 数据来源：NTC 热敏电阻 + ADC 通道（HAL_ADC_ReadTempRaw(0)）
 * 输出：sys->lcd.ambient.{temp_c, valid}
 *
 * 更新频率：每 CFG_AMBIENT_UPDATE_S 秒（默认 10s）刷新一次。
 * 任务调用频率为 100ms，内部累积达 10s 才进行一次实际换算，
 * 避免短时间气流影响导致温度跳变。
 */

#ifndef CLUSTER_AMBIENT_H
#define CLUSTER_AMBIENT_H

#include "../../common/cluster_context.h"

/** 外温模块初始化 */
void AMBIENT_Init(ClusterSystem_t *sys);

/** 外温模块周期任务（建议 100ms 调用） */
void AMBIENT_Task(ClusterSystem_t *sys);

/**
 * @brief NTC 电阻 → 摄氏度（基于内部分段插值表）
 *
 * @param ohms_x1000  电阻值 × 1000（即单位 mΩ → 千分之一 Ω）
 *                    使用 ×1000 精度可在整数运算下完成插值
 * @return 摄氏度；输入超出范围时返回 0x7FFF（无效值标志）
 */
s16 AMBIENT_OhmToTemp(u32 ohms_x1000);

#endif /* CLUSTER_AMBIENT_H */
