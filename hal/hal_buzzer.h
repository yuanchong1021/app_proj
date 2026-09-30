/**
 * @file    hal_buzzer.h
 * @brief   蜂鸣器硬件抽象层接口
 *
 * 由 driver/buzzer_drv.c 实现。
 * 应用层（app/ui/buzzer.c）只控制 ON / OFF，
 * 鸣叫频率 / 持续时间 / 循环节奏由应用层在 BUZZER_Task 中按时序计算。
 */

#ifndef CLUSTER_HAL_BUZZER_H
#define CLUSTER_HAL_BUZZER_H

#include "mpwm_pwm_driver.h"
#include "mpwm_common.h"
/**
 * @brief 蜂鸣器开（持续输出鸣叫信号）
 */
void HAL_Buzzer_On(void);

/**
 * @brief 蜂鸣器关
 */
void HAL_Buzzer_Off(void);

#endif /* CLUSTER_HAL_BUZZER_H */
