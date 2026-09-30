/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2026-05-11 10:01:42
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2026-06-24 15:40:47
 * @FilePath: \instrument\hal\hal_buzzer.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/**
 * @file  hal_buzzer.c
 * @brief 蜂鸣器 HAL（GPIO 直驱有源蜂鸣器，高电平有效）
 * 引脚按实际原理图修改 BUZZER_PORT / BUZZER_PIN
 */
#include "hal_buzzer.h"
#include "pins_driver.h"
#include "YTM32B1MC0.h"

#define BUZZER_PORT   GPIOD
#define BUZZER_PIN    1u

void HAL_Buzzer_On(void)  
{ 
    /* Change channel 1 to 20KHz, 50% duty */
    MPWM_DRV_UpdateChannelPeriodAndDutyCycle(0, 0, MPWM_PERIOD_IN_HZ, 2000U, 0x7000); 
    MPWM_DRV_SetLdok(0, 0x01 << 0);
}

void HAL_Buzzer_Off(void) 
{ 
    MPWM_DRV_UpdateChannelPeriodAndDutyCycle(0, 0, MPWM_PERIOD_IN_HZ, 20000U, 0x0); 
    MPWM_DRV_SetLdok(0, 0x01 << 0);
}
