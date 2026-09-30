/*
 * Copyright 2020-2025 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 * @file ltmr_config.c
 * @brief 
 * 
 */


#include "lptmr_config.h"

const lptmr_config_t LPTMR_Config={
    .dmaRequest=false,
    .interruptEnable=true,
    .freeRun=false,
    .workMode=lpTMR_WORKMODE_TIMER,
    .prescaler=lpTMR_PRESCALE_2,
    .bypassPrescaler=false,
    .compareValue=1000,
    .counterUnits=lpTMR_COUNTER_UNITS_MICROSECONDS,
    .pinSelect=lpTMR_PINSELECT_TMU,
    .pinPolarity=lpTMR_PINPOLARITY_RISING,
    .clockSource=lpTMR_CLOCK_SOURCE_FXOSC,
};

