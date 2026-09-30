/**
 * @file    fuel.h
 * @brief   燃油（汽油）显示模块（Section 8.1）
 *
 * 数据来源：油位电阻传感器（硬线 ADC，sys->hw_input.fuel_ohms）
 *
 * 输出：
 *   sys->lcd.fuel.segments       当前格数 0~9
 *   sys->lcd.fuel.blink          F1/F2 低油量闪烁
 *   sys->lcd.fuel.fault_blink    开路/短路故障：9 格全闪
 *   sys->lcd.fuel.cng_switching  本模块不写（CNG 模块写）
 *
 * 故障判定：
 *   - 进入故障：阻值越界连续保持 CFG_FUEL_FAULT_HOLD_MS（20s）
 *   - 退出故障：阻值正常连续保持 CFG_FUEL_RECOVERY_MS（5s）
 */

#ifndef CLUSTER_FUEL_H
#define CLUSTER_FUEL_H

#include "../common/cluster_context.h"

typedef enum {
    FUEL_SENSOR_NORMAL = 0,
    FUEL_SENSOR_OPEN,    // 开路（阻值 > 上限）
    FUEL_SENSOR_SHORT,   // 短路（阻值 < 下限，通常为接地）
} FuelSensorFault_t;

/** 燃油模块初始化 */
void FUEL_Init(ClusterSystem_t *sys);

/**
 * @brief 燃油模块周期任务（建议 20ms 调用）
 *
 * 自检阶段（selfcheck_done == false）不读取真实油位，避免上电跳格。
 */
void FUEL_Task(ClusterSystem_t *sys);

/** 取当前显示格数 */
u8 FUEL_GetSegments(const ClusterSystem_t *sys);

/**
 * @brief 取燃油百分比（用于外发到 CAN 0x380 / 0x381）
 * @return 0~100；故障时返回 0x7F
 */
u8 FUEL_GetPercent(const ClusterSystem_t *sys);

/** 获取燃油传感器故障 */
FuelSensorFault_t FUEL_GetSensorFault(void);

#endif /* CLUSTER_FUEL_H */
