/**
 * @file    tpms_data.h
 * @brief   胎压数据处理模块（Section 8.3）
 *
 * 该模块只做"胎压数据"的处理，不负责发声报警（蜂鸣器）：
 *
 *   CAN TPMS(0x390).press_*_raw / status_*
 *        ↓ TPMS_Task
 *   sys->lcd.tpms.{press_*_kpa, show_*, blink_*}
 *
 * 蜂鸣器报警逻辑放在 app/ui/buzzer.c 中，避免本模块同时承担数据与执行器职责。
 */

#ifndef CLUSTER_TPMS_DATA_H
#define CLUSTER_TPMS_DATA_H

#include "../common/cluster_context.h"

/** 胎压模块初始化：清零所有 4 路胎压显示数据 */
void TPMS_Init(ClusterSystem_t *sys);

/**
 * @brief 胎压数据处理任务（建议 20ms 调用）
 *
 * 行为：
 *   1) 自检阶段不处理（保留显示）
 *   2) CAN 超时：4 路全部 show=false + blink=true
 *   3) 正常：原始值 → kPa；状态 != NO_WARN 时设置 blink_*
 */
void TPMS_Task(ClusterSystem_t *sys);

/**
 * @brief 胎压原始值 → kPa
 *
 * @param raw  CAN 原始字节（0xFF 表示无效）
 * @return     kPa 值（0~CFG_TPMS_MAX_PRESSURE_KPA）；0xFFFF 表示无效
 */
u16 TPMS_RawToKpa(u8 raw);

#endif /* CLUSTER_TPMS_DATA_H */
