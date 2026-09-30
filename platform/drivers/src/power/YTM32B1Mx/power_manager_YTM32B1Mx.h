/*
 * Copyright (c) 2021-2025 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file power_manager_YTM32B1Mx.h
 * @version 1.4.0
 */

#ifndef POWER_MANAGER_YTM32B1Mx_H
#define POWER_MANAGER_YTM32B1Mx_H

#include "device_registers.h"
#include "status.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*!
 * @brief Power modes enumeration.
 *
 * Defines power modes. Used in the power mode configuration structure
 * (power_manager_user_config_t). From ARM core perspective, Power modes
 * can be generally divided into run modes, sleep and deepsleep modes
 * List of power modes supported by specific chip along with requirements for entering
 * and exiting of these modes can be found in chip documentation.
 * List of all supported power modes:\n
 *  \li POWER_MANAGER_RUN - RUN mode.
 *  \li POWER_MANAGER_SLEEP - SLEEP mode.
 *  \li POWER_MANAGER_DEEPSLEEP - DEEPSLEEP mode.
 * Implements power_manager_modes_t_Class
 */
typedef enum
{
    POWER_MANAGER_RUN,              /*!< Run mode. */
    POWER_MANAGER_SLEEP,            /*!< Sleep mode.  */
    POWER_MANAGER_DEEPSLEEP,        /*!< Deepsleep mode. */
    POWER_MANAGER_STANDBY,          /*!< Standby mode. */
#if defined(FEATURE_SOC_SUPPORT_POWERDOWN_MODE) && FEATURE_SOC_SUPPORT_POWERDOWN_MODE
    POWER_MANAGER_POWERDOWN,        /*!< Power down mode. */
#endif
#if defined(FEATURE_SOC_SUPPORT_DEEPPOWERDOWN_MODE) && FEATURE_SOC_SUPPORT_DEEPPOWERDOWN_MODE
    POWER_MANAGER_DEEPPOWERDOWN,    /*!< Deep power down mode. */
#endif
} power_manager_modes_t;

/*!
 * @brief Power mode user configuration structure.
 *
 * List of power mode configuration structure members depends on power options available
 * for the specific chip. Complete list contains:
 * mode - power mode. List of available modes is chip-specific. See power_manager_modes_t
 * list of modes.
 * sleepOnExitOption - Controls whether the sleep-on-exit option value is used(when set to true)
 * or ignored(when set to false). See sleepOnExitValue.
 * sleepOnExitValue - When set to true, ARM core returns to sleep or deep sleep
 * state after interrupt service finishes. When set to false, core stays
 * woken-up.
 * Implements power_manager_user_config_t_Class
 */
typedef struct
{
    power_manager_modes_t powerMode;
    bool sleepOnExitValue;
} power_manager_user_config_t;

/*!
 * @brief Power Modes
 *
 */
typedef enum
{
    STAT_RUN          = 0x01,              /*!< 0000_0001 - Current power mode is RUN*/
    STAT_SLEEP        = 0x02,              /*!< 0000_0010 - Current power mode is SLEEP*/
    STAT_DEEPSLEEP    = 0x04,              /*!< 0000_0100 - Current power mode is DEEPSLEEP*/
    STAT_STANDBY      = 0x08,              /*!< 0000_1000 - Current power mode is STANDBY*/
    STAT_INVALID      = 0xFF               /*!< 1111_1111 - Non-existing power mode*/
} power_mode_stat_t;

/*!
 * @brief System Reset Source Name definitions
 * Implements rcu_source_names_t_Class
 */
typedef enum
{
    RCU_EXTERNAL_PIN             = 1U,       /*!< External pin reset */
#if defined(FEATURE_RCU_HAS_CMU_LOSS_OF_CLOCK) && (FEATURE_RCU_HAS_CMU_LOSS_OF_CLOCK == 1)
    RCU_CMU_LOSS_OF_CLOCK        = 2U,       /*!< Clock monitor reset */
#endif
    RCU_SOFTWARE                 = 3U,       /*!< Software reset */
    RCU_WDG                      = 4U,       /*!< Watch dog reset */
    RCU_FORCE_POWER_ON           = 5U,       /*!< Force power on reset */
    RCU_POWER_ON                 = 6U,       /*!< Power on reset */
    RCU_CORE_LOCKUP              = 7U,       /*!< Core lockup reset */
    RCU_DEEPSLEEP_ACK_TIMEOUT    = 8U,       /*!<Deepsleep acknowledge timeout reset */
    RCU_SRC_NAME_MAX
} rcu_source_names_t;
/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief This function implementation-specific configuration of power modes.
 *
 * This function performs the actual implementation-specific initialization based on the provided power mode configurations.
 * In addition, This function get all clock source were enabled. This one was used for update init clock when CPU 
    jump from very low power mode to run or high speed run mode.
 *
 * @return Operation status
 *        - STATUS_SUCCESS: Operation was successful.
 *        - STATUS_ERROR: Operation failed.
 */
status_t POWER_SYS_DoInit(void);

/*!
 * @brief This function implementation-specific de-initialization of power manager.
 *
 * This function performs the actual implementation-specific de-initialization.
 *
 * @return Operation status
 *        - STATUS_SUCCESS: Operation was successful.
 *        - STATUS_ERROR: Operation failed.
 */
status_t POWER_SYS_DoDeinit(void);

/*!
 * @brief This function configures the power mode.
 *
 * This function performs the actual implementation-specific logic to switch to one of the defined power modes.
 *
 * @param configPtr: Pointer to user configuration structure
 * @return Operation status
 *        - STATUS_SUCCESS: Operation was successful.
 *        - STATUS_MCU_TRANSITION_FAILED: Operation failed.
 */
status_t POWER_SYS_DoSetMode(const power_manager_user_config_t * const configPtr);

/*!
 * @brief Gets the default power_manager configuration structure.
 *
 * This function gets the power_manager configuration structure of the default power mode.
 *
 * @param[out] defaultConfig : Pointer to power mode configuration structure of the default power mode.
 */
static inline void POWER_SYS_DoGetDefaultConfig(power_manager_user_config_t * const defaultConfig)
{
    defaultConfig->powerMode = POWER_MANAGER_RUN;    /*!< Power manager mode  */
    defaultConfig->sleepOnExitValue = false;         /*!< Sleep on exit value */
}

#if defined(__cplusplus)
}
#endif

#endif /* POWER_MANAGER_YTM32B1Mx_H */

/*! @}*/
/*******************************************************************************
 * EOF
 ******************************************************************************/
