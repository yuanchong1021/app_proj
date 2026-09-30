/*
 * Copyright 2020-2025 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file mpwm_common.h
 * @version 1.4.0
 */

#ifndef MPWM_COMMON_H
#define MPWM_COMMON_H

#include "status.h"
#include "device_registers.h"
#include "clock_manager.h"
#include "interrupt_manager.h"
#include "mpwm_hw_access.h"
#include <stddef.h>

/*!
 * @addtogroup MPWM_DRIVER
 * @brief mpwm common driver
 * @{
 */

/*******************************************************************************
 * Enumerations & Struct
 ******************************************************************************/
typedef enum
{
    MPWM_BUS_CLOCK = 0x0U,   /*!< Bus clock */
    MPWM_FUNC_CLOCK = 0x1U,  /*!< Functional clock */
} mpwm_clk_src_t;

typedef enum
{
    MPWM_CHANNEL_0 = 0x0U,  /*!< channel 0 */
    MPWM_CHANNEL_1 = 0x1U,  /*!< channel 1 */
    MPWM_CHANNEL_2 = 0x2U,  /*!< channel 2 */
    MPWM_CHANNEL_3 = 0x3U,  /*!< channel 3 */
    MPWM_CHANNEL_4 = 0x4U,  /*!< channel 4 */
    MPWM_CHANNEL_5 = 0x5U,  /*!< channel 5 */
    MPWM_CHANNEL_6 = 0x6U,  /*!< channel 6 */
    MPWM_CHANNEL_7 = 0x7U,  /*!< channel 7 */
    MPWM_CHANNEL_8 = 0x8U,  /*!< channel 8  */
    MPWM_CHANNEL_9 = 0x9U,  /*!< channel 9  */
    MPWM_CHANNEL_10 = 0xAU,  /*!< channel 10 */
    MPWM_CHANNEL_11 = 0xBU,  /*!< channel 11 */
    MPWM_CHANNEL_12 = 0xCU,  /*!< channel 12 */
    MPWM_CHANNEL_13 = 0xDU,  /*!< channel 13 */
    MPWM_CHANNEL_14 = 0xEU,  /*!< channel 14 */
    MPWM_CHANNEL_15 = 0xFU,  /*!< channel 15 */
} mpwm_ch_index_t;

typedef enum
{
    MPWM_COMMON_TIMER_MODE,   /*!< Common timer mode */
    MPWM_PWM_MODE,            /*!< PWM mode */
    MPWM_OUTPUT_COMPARE_MODE, /*!< Output compare mode */
    MPWM_INPUT_CAPTURE_MODE,  /*!< Input capture mode */
    MPWM_PULSE_COUNTER_MODE,  /*!< Pulse counter mode */
    MPWM_CENTER_PWM_MODE,     /*!< Center PWM mode */
} mpwm_ch_mode_t;

typedef enum
{
    MPWM_CLK_DIV_BY_1 = 0x0U,  /*!< Divide by 1 */
    MPWM_CLK_DIV_BY_2 = 0x1U,  /*!< Divide by 2 */
    MPWM_CLK_DIV_BY_4 = 0x2U,  /*!< Divide by 4 */
    MPWM_CLK_DIV_BY_8 = 0x3U,  /*!< Divide by 8 */
    MPWM_CLK_DIV_BY_16 = 0x4U,  /*!< Divide by 16 */
    MPWM_CLK_DIV_BY_32 = 0x5U,  /*!< Divide by 32 */
    MPWM_CLK_DIV_BY_64 = 0x6U,  /*!< Divide by 64 */
    MPWM_CLK_DIV_BY_128 = 0x7U,  /*!< Divide by 128 */
} mpwm_ch_clk_prs_t;

typedef enum
{
    MPWM_RELOAD_ON_PERIOD,  /*!< Reload when count is equal to period */
    MPWM_RELOAD_ON_HW_TRIG,  /*!< Reload when hardware trigger arrives */
    MPWM_RELOAD_ON_SW_TRIG,  /*!< Reload when software trigger arrives */
    MPWM_RELOAD_DISABLE,  /*!< Reload mechanism disabled, it activates immediately */
} mpwm_ch_reload_src_t;

typedef enum
{
    MPWM_CONTINUOUS_MODE,  /*!< counter continuous mode */
    MPWM_ONESHOT_MODE,  /*!< Counter one shot mode */
} mpwm_ch_count_mode_t;

typedef enum
{
    MPWM_OUTPUT_INIT_0,  /*!< Output initial value is 0 */
    MPWM_OUTPUT_INIT_1,  /*!< Output initial value is 1 */
} mpwm_ch_output_init_t;

typedef enum
{
    MPWM_NOT_INVERT,  /*!< Not invert output */
    MPWM_INVERT,  /*!< Invert output */
} mpwm_ch_polarity_t;

typedef enum
{
    MPWM_PWM_RIGHT_ALIGN,          /*!< PWM Right alignment */
    MPWM_PWM_LEFT_ALIGN,           /*!< PWM Left alignment */
    MPWM_PWM_CENTER_H_ALIGN,       /*!< PWM Center High alignment */
    MPWM_PWM_CENTER_L_ALIGN,       /*!< PWM Center Low alignment */
} mpwm_ch_pwm_align_mode_t;

typedef struct
{
    bool cmpTrigOutputEn;  /*!< Trigger output enable when matching compare */
    bool perTrigOutputEn;  /*!< Trigger output enable when matching period */
} mpwm_ch_output_trig_config_t;

typedef enum
{
    MPWM_UPDATE_IN_DUTY_CYCLE,  /*!< Update in duty cycle, like 0x1000, 0x4000 and so on */
    MPWM_UPDATE_IN_TICKS,  /*!< Update in ticks */
} mpwm_pwm_update_unit_t;

typedef enum
{
    MPWM_PERIOD_IN_HZ = 0U, /*!< PWM period in Hz */
    MPWM_PERIOD_IN_TICKS = 1U, /*!< PWM period in ticks */
} mpwm_period_unit_t;

typedef struct
{
    mpwm_clk_src_t mpwmClkSrc;                                  /*!< The clock source */
    uint32_t mpwmClkSrcFreq;                                    /*!< The clock source frequency in Hz */
    mpwm_ch_mode_t mpwmChMode[MPWM_CH_COUNT];                   /*!< The channel mode */
    mpwm_ch_pwm_align_mode_t mpwmChAlignMode[MPWM_CH_COUNT];    /*!< The channel align mode */
    uint32_t mpwmChFreq[MPWM_CH_COUNT];                         /*!< The channel counter frequency in Hz */
    uint16_t mpwmChPeriodTicks[MPWM_CH_COUNT];                  /*!< The channel frequency ticks */
    uint16_t mpwmChDutyCycle[MPWM_CH_COUNT];                    /*!< The channel duty cycle (0 ~ 0x8000) */
    mpwm_pwm_update_unit_t mpwmChUpdateUnit[MPWM_CH_COUNT];     /*!< The channel update unit */
} mpwm_state_t;

typedef struct
{
    mpwm_clk_src_t clkSrc;     /*!< MPWM peripheral clock source */
} mpwm_user_config_t;

/*******************************************************************************
 * Variable
 ******************************************************************************/

extern MPWM_Type *g_mpwmBase[MPWM_INSTANCE_COUNT];
extern IRQn_Type g_mpwmOvfIrqId[MPWM_INSTANCE_COUNT][MPWM_CH_COUNT];
extern IRQn_Type g_mpwmChnIrqId[MPWM_INSTANCE_COUNT][MPWM_CH_COUNT];
extern mpwm_state_t *g_mpwmState[MPWM_INSTANCE_COUNT];

/*******************************************************************************
 * API Declaration
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Initialize registers of MPWM and Configure MPWM state
 *
 * @param[in] instance The MPWM peripheral instance
 * @param[in] userConfig The user configuration for MPWM if it has IPC clock source
 * @param[in] state    The MPWM state
 * @return status
 */
status_t MPWM_DRV_Init(uint32_t instance, mpwm_state_t *state);

/*!
 * @brief Reset registers of MPWM
 *
 * @param[in] instance The MPWM peripheral instance
 * @return status
 */
status_t MPWM_DRV_Deinit(uint32_t instance);

/*!
 * @brief Set MPWM clock source
 *
 * @param[in] instance The MPWM peripheral instance
 * @param[in] clkSrc   The clock source
 * @return status
 */
status_t MPWM_DRV_SetClkSrc(uint32_t instance, mpwm_clk_src_t clkSrc);

/*!
 * @brief Get MPWM clock source
 *
 * @param[in] instance The MPWM peripheral instance
 * @return clkSrc      The clock source
 */
mpwm_clk_src_t MPWM_DRV_GetClkSrc(uint32_t instance);

/*!
 * @brief Enable channels' counter together
 *
 * @param[in] instance The MPWM peripheral instance
 * @param[in] channelSet The set of enabled channel(s)
 * @return status
 */
status_t MPWM_DRV_EnableCounter(uint32_t instance, uint16_t channelSet);

/*!
 * @brief Disable channels' counter together
 *
 * @param[in] instance The MPWM peripheral instance
 * @param[in] channelSet The set of disabled channel(s)
 * @return status
 */
status_t MPWM_DRV_DisableCounter(uint32_t instance, uint16_t channelSet);

/*!
 * @brief Set reload source for channel
 *
 * @param[in] instance The MPWM peripheral instance
 * @param[in] channel  The MPWM channel
 * @param[in] src      The reload source
 * @return status
 */
status_t MPWM_DRV_SetReloadSrc(uint32_t instance,
                               uint8_t channel,
                               mpwm_ch_reload_src_t src);

/*!
 * @brief Get reload source of channel
 *
 * @param[in] instance The MPWM peripheral instance
 * @param[in] channel  The MPWM channel
 * @return source
 */
mpwm_ch_reload_src_t MPWM_DRV_GetReloadSrc(uint32_t instance, uint8_t channel);

/*!
 * @brief Set output initial value for channel
 *
 * @param[in] instance The MPWM peripheral instance
 * @param[in] channel  The MPWM channel
 * @param[in] initVal  The initial value
 * @return status
 */
status_t MPWM_DRV_SetChannelOutputInit(uint32_t instance,
                                       uint8_t channel,
                                       mpwm_ch_output_init_t initVal);

/*!
 * @brief Set output initial value for channels together
 *
 * @param[in] instance The MPWM peripheral instance
 * @param[in] initValSet The set of channels' initial value
 * @return status
 */
status_t MPWM_DRV_SetOutputInit(uint32_t instance, uint16_t initValSet);

/*!
 * @brief Get the set of output initial value of all channels
 *
 * @param[in] instance The MPWM peripheral instance
 * @return initValSet
 */
uint16_t MPWM_DRV_GetOutputInit(uint32_t instance);

/*!
 * @brief Set polarity for channels together
 *
 * @param[in] instance The MPWM peripheral instance
 * @param[in] polSet   The set of polarity value of all channels
 * @return status
 */
status_t MPWM_DRV_SetPolarity(uint32_t instance, uint16_t polSet);

/*!
 * @brief Get the set of polarity value of all channels
 *
 * @param[in] instance The MPWM peripheral instance
 * @return polSet
 */
uint16_t MPWM_DRV_GetPolarity(uint32_t instance);

/*!
 * @brief Set polarity for channel
 *
 * @param[in] instance The MPWM peripheral instance
 * @param[in] channel  The MPWM channel
 * @param[in] pol      The polarity
 * @return status
 */
status_t MPWM_DRV_SetChannelPolarity(uint32_t instance,
                                     uint8_t channel,
                                     mpwm_ch_polarity_t pol);

/*!
 * @brief Enable DMA for channel
 *
 * @param[in] instance The MPWM peripheral instance
 * @param[in] channel  The MPWM channel
 * @return status
 */
status_t MPWM_DRV_EnableDma(uint32_t instance, uint8_t channel);

/*!
 * @brief Set software trigger for channel
 *
 * @param[in] instance The MPWM peripheral instance
 * @param[in] channel  The MPWM channel
 * @return status
 */
status_t MPWM_DRV_SetChannelSoftwareTrigger(uint32_t instance, uint8_t channel);

/*!
 * @brief Set software trigger together
 *
 * @param[in] instance The MPWM peripheral instance
 * @param[in] channelSet The set of channels
 * @return status
 */
status_t MPWM_DRV_SetSoftwareTrigger(uint32_t instance, uint16_t channelSet);

/*!
 * @brief Set channel period ticks
 *
 * @param[in] instance The MPWM peripheral instance
 * @param[in] channel  The MPWM channel
 * @param[in] val      The value of period ticks
 * @return status
 */
status_t MPWM_DRV_SetChannelPeriod(uint32_t instance, uint8_t channel, uint16_t value);

/*!
 * @brief Set channel compare ticks
 *
 * @param[in] instance The MPWM peripheral instance
 * @param[in] channel  The MPWM channel
 * @param[in] val      The value of compare ticks
 * @return status
 */
status_t MPWM_DRV_SetChannelCompare(uint32_t instance, uint8_t channel, uint16_t value);

/*!
 * @brief Get frequency of MPWM
 *
 * @param[in] instance The MPWM peripheral instance
 * @return frequency
 */
uint32_t MPWM_DRV_GetFrequency(uint32_t instance);

/*!
 * @brief Convert frequency to period ticks
 *
 * @param[in] instance The MPWM peripheral instance
 * @param[in] channel  The MPWM channel
 * @param[in] freq     The frequency
 * @return ticks       The period ticks
 */
uint32_t MPWM_DRV_ConvertFreqToPeriodTicks(uint32_t instance,
                                           uint8_t channel,
                                           uint32_t freq);

/*!
 * @brief Get timer overflow interrupt flag of channel
 *
 * @param[in] instance The MPWM peripheral instance
 * @param[in] channel  The MPWM channel
 * @return val         0 or 1
 */
bool MPWM_DRV_GetChannelTofIntFlag(uint32_t instance, uint8_t channel);

/*!
 * @brief Clear timer overflow interrupt flag for channel
 *
 * @param[in] instance The MPWM peripheral instance
 * @param[in] channel  The MPWM channel
 */
void MPWM_DRV_ClearChannelTofIntFlag(uint32_t instance, uint8_t channel);

/*!
 * @brief Get interrupt flag for channel event
 *
 * @param[in] instance The MPWM peripheral instance
 * @param[in] channel  The MPWM channel
 * @return val         0 or 1
 */
bool MPWM_DRV_GetChannelIntFlag(uint32_t instance, uint8_t channel);

/*!
 * @brief Clear interrupt flag for channel event
 *
 * @param[in] instance The MPWM peripheral instance
 * @param[in] channel  The MPWM channel
 */
void MPWM_DRV_ClearChannelIntFlag(uint32_t instance, uint8_t channel);

/*!
 * @brief Clear load ok for channel
 *
 * @param[in] instance The MPWM peripheral instance
 * @param[in] channel  The MPWM channel
 */
void MPWM_DRV_ClearChannelLdok(uint32_t instance, uint8_t channel);

/*!
 * @brief Clear load ok together
 *
 * @param[in] instance The MPWM peripheral instance
 * @param[in] channelSet The set of channels
 */
void MPWM_DRV_ClearLdok(uint32_t instance, uint16_t channelSet);

/*!
 * @brief Set loading ok for single channel
 *
 * @param[in] instance The MPWM peripheral instance
 * @param[in] channel  The MPWM channel
 */
void MPWM_DRV_SetChannelLdok(uint32_t instance, uint8_t channel);

/*!
 * @brief Set loading ok to indicate PERIOD or CMP register is \n
 *        ready for loading
 *
 * @param[in] instance The MPWM peripheral instance
 * @param[in] channelSet The set of channel(s) that need to update
 */
void MPWM_DRV_SetLdok(uint32_t instance, uint16_t channelSet);

/*!
 * @brief Read MPWM channel capture count value
 *
 * @param[in] instance The MPWM peripheral instance
 * @param[in] channel  The MPWM channel
 * @return capVal
 */
uint32_t MPWM_DRV_ReadCaptureCount(uint32_t instance, uint8_t channel);

/*!
 * @brief Read MPWM channel current count value(pulse number)
 *
 * @param[in] instance The MPWM peripheral instance
 * @param[in] channel  The MPWM channel
 * @return count
 */
uint16_t MPWM_DRV_ReadCurrentCount(uint32_t instance, uint8_t channel);

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* MPWM_COMMON_H */
/*******************************************************************************
 * EOF
 ******************************************************************************/
