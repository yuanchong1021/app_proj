/**
 * @file    cng.h
 * @brief   CNG（天然气）显示模块（Section 8.2）
 *
 * 数据流：
 *   CAN GAS(0x305).cng_mode / cng_fuel_percent
 *        ↓ CNG_Task
 *   sys->fuel_mode（汽油/CNG/切换中）
 *   sys->lcd.fuel.{segments, blink, cng_switching, mode}
 *
 * 与 fuel.c 共用 sys->lcd.fuel：
 *   - 汽油模式：fuel.c 写 segments / blink / fault_blink
 *   - CNG  模式：cng.c 写 segments / blink / cng_switching
 */

#ifndef CLUSTER_CNG_H
#define CLUSTER_CNG_H

#include "../common/cluster_context.h"

/** CNG 模块初始化 */
void CNG_Init(ClusterSystem_t *sys);

/**
 * @brief CNG 模块周期任务（建议 20ms 调用）
 *
 * 行为：
 *   1) 自检阶段：仅根据上电检测到的 cng_mode 锁定燃料模式（汽油 or CNG）
 *   2) 自检完成后：根据 CAN 实时 cng_mode 更新模式与显示
 *   3) 切换模式时设置 cng_switching = true，让 LCD 层闪烁 CNG 图标
 */
void CNG_Task(ClusterSystem_t *sys);

/**
 * @brief 取 CNG 当前格数
 * @return CNG 模式下返回当前格数；其它模式返回 0
 */
u8 CNG_GetSegments(const ClusterSystem_t *sys);

#endif /* CLUSTER_CNG_H */
