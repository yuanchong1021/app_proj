/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file clock.h
 * @version 1.4.0
 */

#ifndef CLOCK_H
#define CLOCK_H

#include "device_registers.h"
#include "status.h"

/*
 * Include the cpu specific clock API header files.
 */

#if (defined(CPU_YTM32B1LD0))
    /* YTM32B1LD0 Clock System Level API header file */
    #include "../src/clock/YTM32B1LD0/clock_YTM32B1LD0.h"
#elif (defined(YTM32B1L_SERIES))
    /* YTM32B1L series Clock System Level API header file */
    #include "../src/clock/YTM32B1Lx/clock_YTM32B1Lx.h"
#elif (defined(CPU_YTM32B1MD0))
    /* YTM32B1MD0 Clock System Level API header file */
    #include "../src/clock/YTM32B1MD0/clock_YTM32B1MD0.h"
#elif (defined(YTM32B1M_SERIES))
    /* YTM32B1Mx Clock System Level API header file */
    #include "../src/clock/YTM32B1Mx/clock_YTM32B1Mx.h"
#elif (defined(YTM32B1H_SERIES))
    /* YTM32B1Mx Clock System Level API header file */
    #include "../src/clock/YTM32B1Hx/clock_YTM32B1Hx.h"
#elif (defined(YTM32Z1M_SERIES))
    /* YTM32Z1x Clock System Level API header file */
    #include "../src/clock/YTM32Z1x/clock_YTM32Z1x.h"
#elif (defined(YTM32Z1DS_SERIES))
    /* YTM32Z1x Clock System Level API header file */
    #include "../src/clock/YTM32Z1DSx/clock_YTM32Z1DSx.h"
#else
    #error "No valid CPU defined!"
#endif

/*! 
* @addtogroup clock_manager
*/
/*! @{*/

/*******************************************************************************
 * Definitions
 ******************************************************************************/


#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus*/

/*******************************************************************************
 * API
 ******************************************************************************/

/*!
 * @name Dynamic clock setting
 * @{
 */

/*******************************************************************************
 * API
 ******************************************************************************/

/*!
* @brief Gets the clock frequency for a specific clock name.
*
* This function checks the current clock configurations and then calculates
* the clock frequency for a specific clock name defined in clock_names_t.
* Clock modules must be properly configured before using this function.
* See features.h for supported clock names for different chip families.
* The returned value is in Hertz. If it cannot find the clock name
* or the name is not supported for a specific chip family, it returns an
* STATUS_UNSUPPORTED. If frequency is required for a peripheral and the
* module is not clocked, then STATUS_MCU_GATED_OFF status is returned.
* Frequency is returned if a valid address is provided. If frequency is
* required for a peripheral that doesn't support protocol clock, the zero
* value is provided.
*
* @param[in] clockName Clock names defined in clock_names_t
* @param[out] frequency Returned clock frequency value in Hertz
* @return status   Error code defined in status_t
*/
status_t CLOCK_DRV_GetFreq(clock_names_t clockName, uint32_t *frequency);


/*!
 * @brief Set clock configuration according to pre-defined structure.
 *
 * This function sets system to target clock configuration; It sets the
 * clock modules registers for clock mode change. 
 *
 * @param[in] config  Pointer to configuration structure.
 *
 * @return Error code.
 *
 * @note If external clock is used in the target mode, please make sure it is
 * enabled, for example, if the external oscillator is used, please setup correctly.
 *
 * @note If the configuration structure is NULL, the function will set a default
 * configuration for clock.
 */
status_t CLOCK_DRV_Init(clock_user_config_t const *config);

/*! @} */


#if defined(__cplusplus)
}
#endif /* __cplusplus*/

/*! @}*/

#endif /* CLOCK_H */
/*******************************************************************************
 * EOF
 ******************************************************************************/

