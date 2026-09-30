/**
 * @file    rtc_input.h
 * @brief   北京时间显示数据输入服务（数据来源：RTC）
 *
 * 职责：
 *   1. 周期性读取 RTC 当前时间（时/分）
 *   2. 写入 ClusterSystem_t::lcd.clock（hour / minute / valid）
 *
 * 上层 LCD_UpdateClock() 只读取 sys->lcd.clock。
 *
 * ⚠ 驱动占位：
 *   RTC 的实际读取（寄存器/外部 RTC 芯片 I2C 等）需在 RTC_ReadHardware()
 *   中补全。当前提供软件维护时钟作为回退（上电从 0:00 起按系统 tick 走时），
 *   以便在 RTC 驱动接好前也能看到时间在变化。
 */

#ifndef CLUSTER_RTC_INPUT_H
#define CLUSTER_RTC_INPUT_H

#include "../../app/common/cluster_context.h"

/** 初始化 RTC 输入服务 */
void RTC_Init(ClusterSystem_t *sys);

/**
 * @brief RTC 输入周期任务（建议 1000ms 调用）
 *
 * 读取 RTC 时/分写入 sys->lcd.clock。RTC 读取失败时 valid=false。
 */
void RTC_Task(ClusterSystem_t *sys);

/**
 * @brief 设置 RTC 时间（供按键/CAN/串口校时调用）
 * @return true 设置成功
 */
bool RTC_SetTime(uint8_t hour, uint8_t minute);

#endif /* CLUSTER_RTC_INPUT_H */
