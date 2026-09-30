/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2026-05-15 14:02:12
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2026-06-24 11:05:34
 * @FilePath: \instrument\hal\hal_backlight.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/**
 * @file  hal_backlight.h
 * @brief LCD 背光亮度硬件抽象层
 *
 * 背光由独立 GPIO 引脚控制（不通过 SC5037 段码驱动器），
 * 因此从 hal_lcd.h 分离出来，单独成为一个小型 HAL。
 *
 * 当前实现为开关控制（pct > 0 → 全亮，pct == 0 → 关闭）；
 * 若硬件后续支持 PWM 调光，只需修改 hal_backlight.c，接口不变。
 *
 * 调用方：app/ui/backlight.c、app/power/pwr_mgr.c
 */
 
#ifndef CLUSTER_HAL_BACKLIGHT_H
#define CLUSTER_HAL_BACKLIGHT_H
 
#include "types.h"
#include "pins_driver.h"
#include "YTM32B1MC0.h"
#include "mpwm_pwm_driver.h"
#include "mpwm_common.h"
 
/* 背光控制引脚（按实际原理图确认） */
#define BACKLIGHT_PORT   GPIOA
#define BACKLIGHT_PIN    3u
 
/**
 * @brief 设置 LCD 背光亮度
 * @param pct  亮度百分比：0 = 关闭，1 ~ 100 = 点亮
 *             （当前为二值开关；PWM 调光请在 .c 中扩展）
 */
void HAL_BACKLIGHT_Set(u8 pct);
 
#endif /* CLUSTER_HAL_BACKLIGHT_H */