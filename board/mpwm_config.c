/*
 * Copyright 2020-2025 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 * @file mpwm_config.c
 * @brief 
 * 
 */


#include <stddef.h>
#include "mpwm_config.h"

/*
 * Common state
 */
mpwm_state_t MPWM_State0;


/*
 * Common timer
 */


/*
 * PWM
 */

mpwm_pwm_ch_config_t MPWM_PWM_Config0ChConfig[2]={
    {
        .hwChannelId = 1,
        .clkPrs = MPWM_CLK_DIV_BY_2,
        .frequencyHz = 10000,
        .unit = MPWM_UPDATE_IN_DUTY_CYCLE,
        .dutyCyclePercent = 0x2000,
        .alignMode = MPWM_PWM_RIGHT_ALIGN,
        .outputInit = MPWM_OUTPUT_INIT_0,
        .debugMode = false,
        .reloadSrc = MPWM_RELOAD_ON_PERIOD,
        .outputTrigConfig = {
            false,
            false,
        },
        .intEn = false,
        .timerOverflowIntEn = false,
    },
    {
        .hwChannelId = 0,
        .clkPrs = MPWM_CLK_DIV_BY_2,
        .frequencyHz = 10000,
        .unit = MPWM_UPDATE_IN_DUTY_CYCLE,
        .dutyCyclePercent = 0X2000,
        .alignMode = MPWM_PWM_RIGHT_ALIGN,
        .outputInit = MPWM_OUTPUT_INIT_0,
        .debugMode = false,
        .reloadSrc = MPWM_RELOAD_ON_PERIOD,
        .outputTrigConfig = {
            false,
            false,
        },
        .intEn = false,
        .timerOverflowIntEn = false,
    },
};

mpwm_pwm_config_t MPWM_PWM_Config0={
    .channelNum=2,
    .pwmChannelConfig=MPWM_PWM_Config0ChConfig,
};


/*
 * Output compare
 */


/*
 * Input capture
 */


/*
 * Pulse count
 */


