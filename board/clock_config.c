/*
 * Copyright 2020-2025 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 * @file clock_config.c
 * @brief 
 * 
 */

#include "stddef.h"
#include "clock_config.h"

/*! @brief peripheral clock PeripheralClockConfig */

peripheral_clock_config_t clock_config0PeripheralClockConfig[10] = {
    {
        .clkName = DMA_CLK,
        .clkGate = true,
        .divider = DIV_BY_1,
        .clkSrc = CLK_SRC_DISABLED,
    },
    {
        .clkName = GPIO_CLK,
        .clkGate = true,
        .divider = DIV_BY_1,
        .clkSrc = CLK_SRC_FXOSC,
    },
    {
        .clkName = UART1_CLK,
        .clkGate = true,
        .divider = DIV_BY_1,
        .clkSrc = CLK_SRC_FXOSC,
    },
    {
        .clkName = UART2_CLK,
        .clkGate = true,
        .divider = DIV_BY_1,
        .clkSrc = CLK_SRC_FXOSC,
    },
    {
        .clkName = I2C1_CLK,
        .clkGate = true,
        .divider = DIV_BY_1,
        .clkSrc = CLK_SRC_FXOSC,
    },
    {
        .clkName = SPI2_CLK,
        .clkGate = true,
        .divider = DIV_BY_1,
        .clkSrc = CLK_SRC_FXOSC,
    },
    {
        .clkName = FlexCAN0_CLK,
        .clkGate = true,
        .divider = DIV_BY_1,
        .clkSrc = CLK_SRC_DISABLED,
    },
    {
        .clkName = ADC0_CLK,
        .clkGate = true,
        .divider = DIV_BY_1,
        .clkSrc = CLK_SRC_FXOSC,
    },
    {
        .clkName = MPWM0_CLK,
        .clkGate = true,
        .divider = DIV_BY_1,
        .clkSrc = CLK_SRC_DISABLED,
    },
    {
        .clkName = lpTMR0_CLK,
        .clkGate = true,
        .divider = DIV_BY_1,
        .clkSrc = CLK_SRC_DISABLED,
    },
};

const scu_config_t clock_config0ScuConfig = {
    .fircEnable = true,
    .fircDeepSleepEnable = false,
    .sircDeepSleepEnable = true,
    .sircStandbyEnable = true,
    .sysClkSrc = SCU_SYSTEM_CLOCK_SRC_FIRC,
    .fxoscConfig =
        {
            .enable = true,
            .bypassMode = false,
            .gainSelection = 5,
            .frequency = 24000000U,
        },
    .sysDiv = SCU_SYS_CLK_DIV_BY_1,
    .fastBusDiv = SCU_SYS_CLK_DIV_BY_1,
    .slowBusDiv = SCU_SYS_CLK_DIV_BY_2,
    .flashDiv = SCU_SYS_CLK_DIV_BY_4,
    .clockOutConfig =
        {
            .enable = false,
            .source = SCU_CLKOUT_SEL_FIRC_CLK,
            .divider = 1
        },
};

const cmu_config_t clock_config0CmuConfig = {
    .fircClockMonitor={
        .enable = true,                 
        .resetEnable = true,           
        .refClock = CMU_REF_SIRC_CLOCK,  
        .compareHigh = (100 * 128 / 2),        
        .compareLow = (60 * 128 / 2),  
    },
    .fxoscClockMonitor={
        .enable = true,                 
        .resetEnable = true,           
        .refClock = CMU_REF_SIRC_CLOCK,              
        .compareHigh = (30 * 128 / 2),        
        .compareLow = (18 * 128 / 2),         
    },
};

/*! @brief User Configuration structure clock_config0ClockManager */
clock_manager_user_config_t clock_config0ClockManager = {
    .scuConfigPtr = &clock_config0ScuConfig,
    .cmuConfigPtr = &clock_config0CmuConfig,
    .ipcConfig =
        {
            .peripheralClocks = clock_config0PeripheralClockConfig,
            .count = 10,
        },
};

/*! @brief Array of pointers to User configuration structures */
clock_manager_user_config_t const *g_clockManConfigsArr[] = {
    &clock_config0ClockManager,
};

/*! @brief Array of pointers to User defined Callbacks configuration structures */
/* The tool do not support generate Callbacks configuration. It's always empty. */
clock_manager_callback_user_config_t *g_clockManCallbacksArr[] = {(void *)0};
