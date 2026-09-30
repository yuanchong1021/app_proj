/*
 * Copyright 2020-2023 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file mpwm_pwm_driver.h
 * @version 1.4.0
 */

#ifndef MPWM_PWM_DRIVER_H
#define MPWM_PWM_DRIVER_H

#include "mpwm_common.h"

/*!
 * @addtogroup mpwm_pwm_driver
 * @brief mpwm pwm driver
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define MPWM_MAX_DUTY_CYCLE            (0x8000U)
#define MPWM_DUTY_TO_TICKS_SHIFT       (15)

/*******************************************************************************
 * Enumeration & Struct
 ******************************************************************************/

typedef struct
{
    uint8_t hwChannelId;
    mpwm_ch_clk_prs_t clkPrs;
    uint32_t frequencyHz;
    mpwm_pwm_update_unit_t unit;
    uint16_t dutyCyclePercent;
    mpwm_ch_pwm_align_mode_t alignMode;
    mpwm_ch_output_init_t outputInit;
    bool debugMode;
    mpwm_ch_reload_src_t reloadSrc;
    mpwm_ch_output_trig_config_t outputTrigConfig;
    bool intEn;
    bool timerOverflowIntEn;
} mpwm_pwm_ch_config_t;

typedef struct
{
    uint8_t channelNum;
    mpwm_pwm_ch_config_t *pwmChannelConfig;
} mpwm_pwm_config_t;

/*******************************************************************************
 * API Declaration
 ******************************************************************************/

/*!
 * @brief Initialize MPWM PWM mode
 *
 * @param[in] instance The MPWM peripheral instance
 * @param[in] config   The MPWM pwm mode configuration parameters
 * @return status
 */
status_t MPWM_DRV_InitPwm(uint32_t instance, const mpwm_pwm_config_t *config);

/*!
 * @brief Deinitialize MPWM pwm mode
 *
 * @param[in] instance The MPWM peripheral instance
 * @param[in] config   The MPWM pwm mode configuration parameters
 * @return status
 */
status_t MPWM_DRV_DeinitPwm(uint32_t instance, const mpwm_pwm_config_t *config);

/*!
 * @brief Update the period of pwm
 *
 * @param[in] instance The MPWM peripheral instance
 * @param[in] channel  The MPWM channel
 * @param[in] frequency The updated frequency
 * @return status
 */
status_t MPWM_DRV_UpdateChannelPeriod(uint32_t instance,
                                      uint8_t channel,
                                      mpwm_period_unit_t unit,
                                      uint32_t frequency);

/*!
 * @brief Calculate channel compare value
 *
 * @param[in] instance  The MPWM peripheral instance
 * @param[in] channel   The MPWM channel
 * @param[in] period    The period of channel
 * @param[in] dutyCycle The duty cycle of channel
 * @return status
 */
uint16_t MPWM_DRV_CalculateCompareValue(uint32_t instance,
                                        uint8_t channel,
                                        uint16_t period,
                                        uint16_t dutyCycle);
/*!
 * @brief Update the duty cycle of pwm
 *
 * @param[in] instance The MPWM peripheral instance
 * @param[in] channel  The MPWM channel
 * @param[in] dutyCycle The value of duty cycle
 * @return status
 */
status_t MPWM_DRV_UpdateChannelDutyCycle(uint32_t instance,
                                         uint8_t channel,
                                         uint16_t dutyCycle);

/*!
 * @brief Update the frequency and duty cycle of pwm
 *
 * @param[in] instance The MPWM peripheral instance
 * @param[in] channel  The MPWM channel
 * @param[in] unit     The unit of period
 * @param[in] frequency The value of frequency
 * @param[in] dutyCycle The value of duty cycle
 * @return status
 */
status_t MPWM_DRV_UpdateChannelPeriodAndDutyCycle(uint32_t instance,
                                                  uint8_t channel,
                                                  mpwm_period_unit_t unit,
                                                  uint32_t frequency,
                                                  uint16_t dutyCycle);

/*! @}*/

#endif /* MPWM_PWM_DRIVER_H */
