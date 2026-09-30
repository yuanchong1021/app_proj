/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*! 
 * @file lptmr_driver.h
 * @version 1.4.0
 */

#ifndef lpTMR_DRIVER_H
#define lpTMR_DRIVER_H

#include <stdint.h>
#include <stdbool.h>
#include "status.h"
#include "device_registers.h"

/*!
 * @addtogroup lptmr_driver
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief Pulse Counter Input selection
 *  Implements : lptmr_pinselect_t_Class
 */
typedef enum
{
    lpTMR_PINSELECT_TMU = 0x00u, /*!< Count pulses from TMU trigger */
#if FEATURE_lpTMR_HAS_INPUT_ALT1_SELECTION
    lpTMR_PINSELECT_ALT1 = 0x01u, /*!< Count pulses from pin alternative 1 */
#endif
    lpTMR_PINSELECT_ALT2 = 0x02u, /*!< Count pulses from pin alternative 2 */
    lpTMR_PINSELECT_ALT3 = 0x03u  /*!< Count pulses from pin alternative 3 */
} lptmr_pinselect_t;

/*! @brief Pulse Counter input polarity
 *  Implements : lptmr_pinpolarity_t_Class
 */
typedef enum
{
    lpTMR_PINPOLARITY_RISING = 0u, /*!< Count pulse on rising edge */
    lpTMR_PINPOLARITY_FALLING = 1u  /*!< Count pulse on falling edge */
} lptmr_pinpolarity_t;

/*! @brief Work Mode
 *  Implements : lptmr_workmode_t_Class
 */
typedef enum
{
    lpTMR_WORKMODE_TIMER = 0u, /*!< Timer */
    lpTMR_WORKMODE_PULSECOUNTER = 1u  /*!< Pulse counter */
} lptmr_workmode_t;

/*! @brief Prescaler Selection
 *  Implements : lptmr_prescaler_t_Class
 */
typedef enum
{
    lpTMR_PRESCALE_2 = 0x00u, /*!< Timer mode: prescaler 2, Glitch filter mode: invalid */
    lpTMR_PRESCALE_4_GLITCHFILTER_2 = 0x01u, /*!< Timer mode: prescaler 4, Glitch filter mode: 2 clocks */
    lpTMR_PRESCALE_8_GLITCHFILTER_4 = 0x02u, /*!< Timer mode: prescaler 8, Glitch filter mode: 4 clocks */
    lpTMR_PRESCALE_16_GLITCHFILTER_8 = 0x03u, /*!< Timer mode: prescaler 16, Glitch filter mode: 8 clocks */
    lpTMR_PRESCALE_32_GLITCHFILTER_16 = 0x04u, /*!< Timer mode: prescaler 32, Glitch filter mode: 16 clocks */
    lpTMR_PRESCALE_64_GLITCHFILTER_32 = 0x05u, /*!< Timer mode: prescaler 64, Glitch filter mode: 32 clocks */
    lpTMR_PRESCALE_128_GLITCHFILTER_64 = 0x06u, /*!< Timer mode: prescaler 128, Glitch filter mode: 64 clocks */
    lpTMR_PRESCALE_256_GLITCHFILTER_128 = 0x07u, /*!< Timer mode: prescaler 256, Glitch filter mode: 128 clocks */
    lpTMR_PRESCALE_512_GLITCHFILTER_256 = 0x08u, /*!< Timer mode: prescaler 512, Glitch filter mode: 256 clocks */
    lpTMR_PRESCALE_1024_GLITCHFILTER_512 = 0x09u, /*!< Timer mode: prescaler 1024, Glitch filter mode: 512 clocks */
    lpTMR_PRESCALE_2048_GLITCHFILTER_1024 = 0x0Au, /*!< Timer mode: prescaler 2048, Glitch filter mode: 1024 clocks */
    lpTMR_PRESCALE_4096_GLITCHFILTER_2048 = 0x0Bu, /*!< Timer mode: prescaler 4096, Glitch filter mode: 2048 clocks */
    lpTMR_PRESCALE_8192_GLITCHFILTER_4096 = 0x0Cu, /*!< Timer mode: prescaler 8192, Glitch filter mode: 4096 clocks */
    lpTMR_PRESCALE_16384_GLITCHFILTER_8192 = 0x0Du, /*!< Timer mode: prescaler 16384, Glitch filter mode: 8192 clocks */
    lpTMR_PRESCALE_32768_GLITCHFILTER_16384 = 0x0Eu, /*!< Timer mode: prescaler 32768, Glitch filter mode: 16384 clocks */
    lpTMR_PRESCALE_65536_GLITCHFILTER_32768 = 0x0Fu  /*!< Timer mode: prescaler 65536, Glitch filter mode: 32768 clocks */
} lptmr_prescaler_t;

/*!
 * @brief Defines the lpTMR counter units available for configuring or reading the timer compare value.
 *
 * Implements : lptmr_counter_units_t_Class
 */
typedef enum
{
    lpTMR_COUNTER_UNITS_TICKS = 0x00U,
    lpTMR_COUNTER_UNITS_MICROSECONDS = 0x01U
} lptmr_counter_units_t;

#if defined(FEATURE_lpTMR_HAS_CLOCK_SELECTION) && (FEATURE_lpTMR_HAS_CLOCK_SELECTION == 1U)
/*!
 * @brief Defines the lpTMR counter clock source.
 *
 * Implements : lptmr_clock_source_t_Class
 */
typedef enum
{
#if defined(FEATURE_lpTMR_CLKSRC_SUPPORT_FIRC) && (FEATURE_lpTMR_CLKSRC_SUPPORT_FIRC == 1U)
    lpTMR_CLOCK_SOURCE_FIRC = 0x00U,
#endif /* FEATURE_lpTMR_CLKSRC_SUPPORT_FIRC */
#if defined(FEATURE_lpTMR_CLKSRC_SUPPORT_IPC) && (FEATURE_lpTMR_CLKSRC_SUPPORT_IPC == 1U)
    lpTMR_CLOCK_SOURCE_IPC = 0x00U,
#endif /* FEATURE_lpTMR_CLKSRC_SUPPORT_IPC */
#if defined(FEATURE_lpTMR_CLKSRC_SUPPORT_SIRC) && (FEATURE_lpTMR_CLKSRC_SUPPORT_SIRC == 1U)
    lpTMR_CLOCK_SOURCE_SIRC = 0x01U,
#endif /* FEATURE_lpTMR_CLKSRC_SUPPORT_SIRC */
#if defined(FEATURE_lpTMR_CLKSRC_SUPPORT_SIRC_DIV4) && (FEATURE_lpTMR_CLKSRC_SUPPORT_SIRC_DIV4 == 1U)
    lpTMR_CLOCK_SOURCE_SIRC_DIV4 = 0x01U,
#endif /* FEATURE_lpTMR_CLKSRC_SUPPORT_SIRC_DIV4 */
#if defined(FEATURE_lpTMR_CLKSRC_SUPPORT_SXOSC) && (FEATURE_lpTMR_CLKSRC_SUPPORT_SXOSC == 1U)
    lpTMR_CLOCK_SOURCE_SXOSC = 0x02U,
#endif /* FEATURE_lpTMR_CLKSRC_SUPPORT_SXOSC */
#if defined(FEATURE_lpTMR_CLKSRC_SUPPORT_FXOSC) && (FEATURE_lpTMR_CLKSRC_SUPPORT_FXOSC == 1U)
    lpTMR_CLOCK_SOURCE_FXOSC = 0x02U,
#endif /* FEATURE_lpTMR_CLKSRC_SUPPORT_FXOSC */
#if defined(FEATURE_lpTMR_CLKSRC_SUPPORT_LPO) && (FEATURE_lpTMR_CLKSRC_SUPPORT_LPO == 1U)
#if defined(MCU_YTM32Z1MD0)
    lpTMR_CLOCK_SOURCE_OSC = 0x00U,
    lpTMR_CLOCK_SOURCE_LPO = 0x01U,
#else
    lpTMR_CLOCK_SOURCE_LPO = 0x03U,
#endif /* MCU_YTM32Z1MD0 */
#endif /* FEATURE_lpTMR_CLKSRC_SUPPORT_LPO */
} lptmr_clock_source_t;
#endif /* FEATURE_lpTMR_HAS_CLOCK_SELECTION */

/*!
 * @brief Defines the configuration structure for lpTMR.
 *
 * Implements : lptmr_config_t_Class
 */
typedef struct
{
    /* General parameters */
#if defined(lpTMR_DIE_DMAEN_MASK)
    bool dmaRequest;                    /*!< Enable/Disable DMA requests */
#endif /* lpTMR_DIE_DMAEN_MASK */
    bool interruptEnable;               /*!< Enable/Disable Interrupt */
    bool freeRun;                       /*!< Enable/Disable Free Running Mode */
    lptmr_workmode_t workMode;          /*!< Time/Pulse Counter Mode */
    /* Counter parameters */
    lptmr_prescaler_t prescaler;        /*!< Prescaler Selection */
    bool bypassPrescaler;               /*!< Enable/Disable prescaler bypass */
    uint32_t compareValue;              /*!< Compare value */
    lptmr_counter_units_t counterUnits; /*!< Compare value units */
    /* Pulse Counter specific parameters */
    lptmr_pinselect_t pinSelect;        /*!< Pin selection for Pulse-Counter */
    lptmr_pinpolarity_t pinPolarity;    /*!< Pin Polarity for Pulse-Counter */
#ifdef FEATURE_lpTMR_HAS_CLOCK_SELECTION
    lptmr_clock_source_t clockSource;   /*!< Clock source selection */
#endif /* FEATURE_lpTMR_HAS_CLOCK_SELECTION */
} lptmr_config_t;

/*******************************************************************************
 * Function prototypes
 ******************************************************************************/
/*!
 * @name lpTMR Driver Functions
 * @{
 */

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Initialize a configuration structure with default values.
 *
 * @param[out] config - Pointer to the configuration structure to be initialized
 */
void lpTMR_DRV_InitConfigStruct(lptmr_config_t *const config);

/*!
 * @brief Initialize a lpTMR instance with values from an input configuration structure.
 *
 * When (counterUnits == lpTMR_COUNTER_UNITS_MICROSECONDS) the function will
 * automatically configure the timer for the input compareValue in microseconds.
 * The input parameters for 'prescaler' and 'bypassPrescaler' will be ignored - their values
 * will be adapted by the function, to best fit the input compareValue
 * (in microseconds) for the operating clock frequency.
 *
 * lpTMR_COUNTER_UNITS_MICROSECONDS may only be used for lpTMR_WORKMODE_TIMER mode.
 * Otherwise the function shall not convert 'compareValue' in ticks
 * and this is likely to cause erroneous behavior.
 *
 * When (counterUnits == lpTMR_COUNTER_UNITS_TICKS) the function will use the
 * 'prescaler' and 'bypassPrescaler' provided in the input configuration structure.
 *
 * @param[in] instance     - lpTMR instance number
 * @param[in] config       - Pointer to the input configuration structure
 * @param[in] startCounter - Flag for starting the counter immediately after configuration
 */
void lpTMR_DRV_Init(const uint32_t instance,
                    const lptmr_config_t *const config,
                    const bool startCounter);

/*!
 * @brief Configure a lpTMR instance.
 *
 * When (counterUnits == lpTMR_COUNTER_UNITS_MICROSECONDS) the function will
 * automatically configure the timer for the input compareValue in microseconds.
 * The input parameters for 'prescaler' and 'bypassPrescaler' will be ignored - their values
 * will be adapted by the function, to best fit the input compareValue
 * (in microseconds) for the operating clock frequency.
 *
 * lpTMR_COUNTER_UNITS_MICROSECONDS may only be used for lpTMR_WORKMODE_TIMER mode.
 * Otherwise the function shall not convert 'compareValue' in ticks
 * and this is likely to cause erroneous behavior.
 *
 * When (counterUnits == lpTMR_COUNTER_UNITS_TICKS) the function will use the
 * 'prescaler' and 'bypassPrescaler' provided in the input configuration structure.
 *
 * @param[in] instance - lpTMR instance number
 * @param[in] config   - Pointer to the input configuration structure
 */
void lpTMR_DRV_SetConfig(const uint32_t instance,
                         const lptmr_config_t *const config);

/*!
 * @brief Get the current configuration of a lpTMR instance.
 *
 * @param[in] instance - lpTMR instance number
 * @param[out] config  - Pointer to the output configuration structure
 */
void lpTMR_DRV_GetConfig(const uint32_t instance,
                         lptmr_config_t *const config);

/*!
 * @brief De-initialize a lpTMR instance
 *
 * @param[in] instance - lpTMR instance number
 */
void lpTMR_DRV_Deinit(const uint32_t instance);

/*!
 * @brief Set the compare value in counter tick units, for a lpTMR instance.
 *
 * @param[in] instance            - lpTMR instance number
 * @param[in] compareValueByCount - The compare value in counter ticks, to be written
 * @return Operation status:
 * - STATUS_SUCCESS: completed successfully
 * - STATUS_ERROR: cannot reconfigure compare value (TCF not set)
 * - STATUS_TIMEOUT: compare value greater then current counter value
 */
status_t lpTMR_DRV_SetCompareValueByCount(const uint32_t instance,
                                          const uint16_t compareValueByCount);

/*!
 * @brief Get the compare value in counter tick units, of a lpTMR instance.
 *
 * @param[in] instance             - lpTMR instance number
 * @param[out] compareValueByCount - Pointer to current compare value, in counter ticks
 */
void lpTMR_DRV_GetCompareValueByCount(const uint32_t instance,
                                      uint16_t *const compareValueByCount);

/*!
 * @brief Set the compare value for Timer Mode in microseconds, for a lpTMR instance.
 *
 * @param[in] instance       - lpTMR peripheral instance number
 * @param[in] compareValueUs - Compare value in microseconds
 * @return Operation status:
 * - STATUS_SUCCESS: completed successfully
 * - STATUS_ERROR: cannot reconfigure compare value
 * - STATUS_TIMEOUT: compare value greater then current counter value
 */
status_t lpTMR_DRV_SetCompareValueByUs(const uint32_t instance,
                                       const uint32_t compareValueUs);

/*!
 * @brief Get the compare value in microseconds, of a lpTMR instance.
 *
 * @param[in] instance        - lpTMR instance number
 * @param[out] compareValueUs - Pointer to current compare value, in microseconds
 */
void lpTMR_DRV_GetCompareValueByUs(const uint32_t instance,
                                   uint32_t *const compareValueUs);

/*!
 * @brief Get the current state of the Compare Flag of a lpTMR instance.
 *
 * @param[in] instance - lpTMR instance number
 * @return The state of the Compare Flag
 */
bool lpTMR_DRV_GetCompareFlag(const uint32_t instance);

/*!
 * @brief Clear the Compare Flag of a lpTMR instance.
 *
 * @param[in] instance - lpTMR instance number
 */
void lpTMR_DRV_ClearCompareFlag(const uint32_t instance);

/*!
 * @brief Get the run state of a lpTMR instance.
 *
 * @param[in] instance - lpTMR instance number
 * @return The run state of the lpTMR instance:
 *  - true: Timer/Counter started
 *  - false: Timer/Counter stopped
 */
bool lpTMR_DRV_IsRunning(const uint32_t instance);

/*!
 * @brief Enable/disable the lpTMR interrupt.
 *
 * @param[in] instance        - lpTMR instance number
 * @param[in] enableInterrupt - The new state of the lpTMR interrupt enable flag.
 */
void lpTMR_DRV_SetInterrupt(const uint32_t instance,
                            const bool enableInterrupt);

/*!
 * @brief Get the current counter value in counter tick units.
 *
 * @param[in] instance - lpTMR instance number
 * @return The current counter value
 */
uint16_t lpTMR_DRV_GetCounterValueByCount(const uint32_t instance);

/*!
 * @brief Enable the lpTMR / Start the counter
 *
 * @param[in] instance - lpTMR instance number
 */
void lpTMR_DRV_StartCounter(const uint32_t instance);

/*!
 * @brief Disable the lpTMR / Stop the counter
 *
 * @param[in] instance - lpTMR instance number
 */
void lpTMR_DRV_StopCounter(const uint32_t instance);

/*!
 * @brief Set the Input Pin configuration for Pulse Counter mode
 *
 * @param[in] instance    - lpTMR instance number
 * @param[in] pinSelect   - lpTMR pin selection
 * @param[in] pinPolarity - Polarity on which to increment counter (rising/falling edge)
 */
void lpTMR_DRV_SetPinConfiguration(const uint32_t instance,
                                   const lptmr_pinselect_t pinSelect,
                                   const lptmr_pinpolarity_t pinPolarity);

#if defined(__cplusplus)
}
#endif

/*! @}*/

/*! @}*/

#endif /* lpTMR_DRIVER_H */
/*******************************************************************************
 * EOF
 ******************************************************************************/
