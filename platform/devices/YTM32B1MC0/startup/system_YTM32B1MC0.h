/*
 * Copyright (c) 2021-2025 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file system_YTM32B1MC0.h
 * @version 1.4.0
 */

#ifndef SYSTEM_YTM32B1MC0_H_
#define SYSTEM_YTM32B1MC0_H_                        /**< Symbol preventing repeated inclusion */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * CPU Settings.
 *****************************************************************************/

/* Watchdog disable */
#ifndef DISABLE_WDOG
  #define DISABLE_WDOG                 1
#endif

/* Value of the external crystal or oscillator clock frequency in Hz */
#ifndef CPU_XTAL_CLK_HZ
  #define CPU_XTAL_CLK_HZ                8000000u
#endif

/* Value of the fast internal oscillator clock frequency in Hz  */
#ifndef CPU_INT_FAST_CLK_HZ
  #define CPU_INT_FAST_CLK_HZ            48000000u
#endif

/* Default System clock value */
#ifndef DEFAULT_SYSTEM_CLOCK
 #define DEFAULT_SYSTEM_CLOCK            48000000u
#endif

/**
 * @brief System clock frequency (core clock)
 *
 * The system clock frequency supplied to the SysTick timer and the processor
 * core clock. This variable can be used by the user application to setup the
 * SysTick timer or configure other parameters. It may also be used by debugger to
 * query the frequency of the debug timer or configure the trace clock speed
 * SystemCoreClock is initialized with a correct predefined value.
 */
extern uint32_t SystemCoreClock;

/**
 * @brief Setup the SoC.
 *
 * This function disables the watchdog.
 * if the corresponding feature macro is enabled.
 * SystemInit is called from startup_device file.
 */
void SystemInit(void);

/**
 * @brief Updates the SystemCoreClock variable.
 *
 * It must be called whenever the core clock is changed during program
 * execution. SystemCoreClockUpdate() evaluates the clock register settings and calculates
 * the current core clock.
 * This function must be called when user does not want to use clock manager component.
 * If clock manager is used, the CLOCK_SYS_GetFreq function must be used with CORE_CLOCK
 * parameter.
 *
 */
void SystemCoreClockUpdate(void);

/**
 * @brief Initiates a system reset.
 *
 * This function is used to initiate a system reset
 */
void SystemSoftwareReset(void);

/**
 * @brief Get chip UID.
 *
 * This function is used to get chip unit ID from reserved flash area.
 * @return UID in 64-bit format.
 */
void SystemGetUid(uint32_t *uidh, uint32_t *uidl);

#ifdef __cplusplus
}
#endif

/*! @}*/
#endif  /* #if !defined(SYSTEM_YTM32B1MC0_H_) */
