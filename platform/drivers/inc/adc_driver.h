/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*! 
 * @file adc_driver.h
 * @version 1.4.0
 */

#ifndef ADC_DRIVER_H
#define ADC_DRIVER_H

#include <stdint.h>
#include <stdbool.h>
#include "device_registers.h"

/*!
 * @addtogroup adc_driver
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*!
 * @brief Conversion resolution selection
 *
 * Implements : adc_resolution_t_Class
 */
typedef enum
{
    ADC_RESOLUTION_12BIT = 0x00U,   /*!< 12-bit resolution mode */
    ADC_RESOLUTION_10BIT = 0x01U,   /*!< 10-bit resolution mode */
    ADC_RESOLUTION_8BIT = 0x02U,    /*!<  8-bit resolution mode */
    ADC_RESOLUTION_6BIT = 0x03U     /*!<  6-bit resolution mode */
} adc_resolution_t;

/*!
 * @brief Result alignment type selection
 *
 * Implements : adc_align_t_Class
 */
typedef enum
{
#if defined (CPU_YTM32Z1MD0)
    ADC_ALIGN_RIGHT = 0x00U,   /*!< Right alignment. */
    ADC_ALIGN_LEFT = 0x01U,    /*!< Left alignment. */
#else
    ADC_ALIGN_LEFT = 0x00U,    /*!< Left alignment. */
    ADC_ALIGN_RIGHT = 0x01U    /*!< Right alignment. */
#endif /* defined (CPU_YTM32Z1MD0) */
} adc_align_t;

/*!
 * @brief Trigger type selection
 *
 * Implements : adc_trigger_t_Class
 */
typedef enum
{
#if defined(FEATURE_ADC_SUPPORT_INJECTION_MODE) && (FEATURE_ADC_SUPPORT_INJECTION_MODE > 0)
    ADC_TRIG_INJECT_SW_NORMAL_SW = 0x00U,        /*!< Software trigger for normal conversion and disable
                                                     injected conversion. */
    ADC_TRIG_INJECT_SW_NORMAL_HW = 0x01U,        /*!< Hardware trigger for normal conversion and disable
                                                        injected conversion. */
    ADC_TRIG_INJECT_HW_NORMAL_SW = 0x02U,        /*!< Hardware trigger for normal conversion and hardware trigger for
                                                        injected conversion. */
    ADC_TRIG_INJECT_HW_NORMAL_HW = 0x03U,        /*!< Software trigger for normal conversion and hardware trigger for
                                                       injected conversion. */
#else
    ADC_TRIGGER_SOFTWARE = 0x00U,               /*!< Software trigger. */
    ADC_TRIGGER_HARDWARE = 0x01U,               /*!< Hardware trigger. */
#endif /* FEATURE_ADC_SUPPORT_INJECTION_MODE */
} adc_trigger_t;

/*!
 * @brief Sequence mode selection
 *
 * Implements : adc_sequence_mode_t_Class
 */
typedef enum
{
    ADC_CONV_STEP = 0x00U,          /*!< One channel convert per trigger. */
    ADC_CONV_LOOP = 0x01U,          /*!< One loop convert per trigger. */
    ADC_CONV_CONTINUOUS = 0x02U     /*!< Continuous convert per trigger. */
} adc_sequence_mode_t;

#if defined (CPU_YTM32Z1MD0)
/*!
 * @brief Mode of each sequence
 *
 * Implements : adc_sequence_independent_mode_t_Class
 */
typedef enum
{
    ADC_SEQUENCE_MODE = 0x00U,      /*!< Current sequence in sequence mode. */
    ADC_SINGLE_MODE = 0x01U         /*!< Current sequence in single mode. */
} adc_sequence_independent_mode_t;

/*!
 * @brief Sequence combine mode selection
 *
 * Implements : adc_sequence_combine_mode_t_Class
 */
typedef enum
{
    ADC_MODE_SEQ0123 = 0x1U,             /*!< Sequence 0-3 form a 16-channel sequence. */
    ADC_MODE_SEQ0_SEQ123 = 0x3U,         /*!< Sequence 0-3 form two channel sequence, one 4-channel sequence and one
                                              12-channel sequence. */
    ADC_MODE_SEQ01_SEQ23 = 0x5U,         /*!< Sequence 0-3 form two 8-channel sequences. */
    ADC_MODE_SEQ0_SEQ1_SEQ23 = 0x7U,     /*!< Sequence 0-3 form three channel sequence, two 4-channel sequences and
                                              one 8-channel sequence. */
    ADC_MODE_SEQ0_SEQ1_SEQ2_SEQ3 = 0xFU, /*!< Sequence 0-3 form four 4-channel sequences. */
} adc_sequence_combine_mode_t;
#endif /* defined (CPU_YTM32Z1MD0) */

/*!
 * @brief Enumeration of input channels assignable to a control channel.
 *
 * Implements : adc_inputchannel_t_Class
 */
typedef enum
{
    ADC_INPUTCHAN_EXT0 = 0x00U,               /*!< External input channel 0 */
    ADC_INPUTCHAN_EXT1 = 0x01U,               /*!< External input channel 1 */
    ADC_INPUTCHAN_EXT2 = 0x02U,               /*!< External input channel 2 */
    ADC_INPUTCHAN_EXT3 = 0x03U,               /*!< External input channel 3 */
    ADC_INPUTCHAN_EXT4 = 0x04U,               /*!< External input channel 4 */
    ADC_INPUTCHAN_EXT5 = 0x05U,               /*!< External input channel 5 */
    ADC_INPUTCHAN_EXT6 = 0x06U,               /*!< External input channel 6 */
    ADC_INPUTCHAN_EXT7 = 0x07U,               /*!< External input channel 7 */
    ADC_INPUTCHAN_EXT8 = 0x08U,               /*!< External input channel 8 */
    ADC_INPUTCHAN_EXT9 = 0x09U,               /*!< External input channel 9 */
    ADC_INPUTCHAN_EXT10 = 0x0AU,              /*!< External input channel 10 */
    ADC_INPUTCHAN_EXT11 = 0x0BU,              /*!< External input channel 11 */
    ADC_INPUTCHAN_EXT12 = 0x0CU,              /*!< External input channel 12 */
    ADC_INPUTCHAN_EXT13 = 0x0DU,              /*!< External input channel 13 */
    ADC_INPUTCHAN_EXT14 = 0x0EU,              /*!< External input channel 14 */
    ADC_INPUTCHAN_EXT15 = 0x0FU,              /*!< External input channel 15 */
 #if (FEATURE_ADC_MAX_NUM_EXT_CHANS > 16)
    ADC_INPUTCHAN_EXT16 = 0x10U,              /*!< External input channel 16 */
    ADC_INPUTCHAN_EXT17 = 0x11U,              /*!< External input channel 17 */
    ADC_INPUTCHAN_EXT18 = 0x12U,              /*!< External input channel 18 */
    ADC_INPUTCHAN_EXT19 = 0x13U,              /*!< External input channel 19 */
    ADC_INPUTCHAN_EXT20 = 0x14U,              /*!< External input channel 20 */
    ADC_INPUTCHAN_EXT21 = 0x15U,              /*!< External input channel 21 */
    ADC_INPUTCHAN_EXT22 = 0x16U,              /*!< External input channel 22 */
    ADC_INPUTCHAN_EXT23 = 0x17U,              /*!< External input channel 23 */
 #endif
 #if (FEATURE_ADC_MAX_NUM_EXT_CHANS > 24)
    ADC_INPUTCHAN_EXT24 = 0x18U,              /*!< External input channel 24 */
    ADC_INPUTCHAN_EXT25 = 0x19U,              /*!< External input channel 25 */
    ADC_INPUTCHAN_EXT26 = 0x1AU,              /*!< External input channel 26 */
    ADC_INPUTCHAN_EXT27 = 0x1BU,              /*!< External input channel 27 */
    ADC_INPUTCHAN_EXT28 = 0x1CU,              /*!< External input channel 28 */
    ADC_INPUTCHAN_EXT29 = 0x1DU,              /*!< External input channel 29 */
    ADC_INPUTCHAN_EXT30 = 0x1EU,              /*!< External input channel 30 */
    ADC_INPUTCHAN_EXT31 = 0x1FU,              /*!< External input channel 31 */
 #endif
 #if (FEATURE_ADC_MAX_NUM_EXT_CHANS > 32)
    ADC_INPUTCHAN_EXT32 = 0x20U,              /*!< External input channel 32 */
    ADC_INPUTCHAN_EXT33 = 0x21U,              /*!< External input channel 33 */
    ADC_INPUTCHAN_EXT34 = 0x22U,              /*!< External input channel 34 */
    ADC_INPUTCHAN_EXT35 = 0x23U,              /*!< External input channel 35 */
    ADC_INPUTCHAN_EXT36 = 0x24U,              /*!< External input channel 36 */
    ADC_INPUTCHAN_EXT37 = 0x25U,              /*!< External input channel 37 */
    ADC_INPUTCHAN_EXT38 = 0x26U,              /*!< External input channel 38 */
    ADC_INPUTCHAN_EXT39 = 0x27U,              /*!< External input channel 39 */
 #endif
 #if (FEATURE_ADC_MAX_NUM_EXT_CHANS > 40)
    ADC_INPUTCHAN_EXT40 = 0x28U,              /*!< External input channel 40 */
    ADC_INPUTCHAN_EXT41 = 0x29U,              /*!< External input channel 41 */
    ADC_INPUTCHAN_EXT42 = 0x2AU,              /*!< External input channel 42 */
    ADC_INPUTCHAN_EXT43 = 0x2BU,              /*!< External input channel 43 */
    ADC_INPUTCHAN_EXT44 = 0x2CU,              /*!< External input channel 44 */
    ADC_INPUTCHAN_EXT45 = 0x2DU,              /*!< External input channel 45 */
    ADC_INPUTCHAN_EXT46 = 0x2EU,              /*!< External input channel 46 */
    ADC_INPUTCHAN_EXT47 = 0x2FU,              /*!< External input channel 47 */
 #endif
 #if (FEATURE_ADC_MAX_NUM_EXT_CHANS > 48)
    ADC_INPUTCHAN_EXT48 = 0x30U,              /*!< External input channel 48 */
    ADC_INPUTCHAN_EXT49 = 0x31U,              /*!< External input channel 49 */
    ADC_INPUTCHAN_EXT50 = 0x32U,              /*!< External input channel 50 */
    ADC_INPUTCHAN_EXT51 = 0x33U,              /*!< External input channel 51 */
    ADC_INPUTCHAN_EXT52 = 0x34U,              /*!< External input channel 52 */
    ADC_INPUTCHAN_EXT53 = 0x35U,              /*!< External input channel 53 */
    ADC_INPUTCHAN_EXT54 = 0x36U,              /*!< External input channel 54 */
    ADC_INPUTCHAN_EXT55 = 0x37U,              /*!< External input channel 55 */
 #endif
} adc_inputchannel_t;

#if defined (CPU_YTM32Z1MD0)
typedef struct
{
    uint8_t sampleTimeSelect;
    adc_inputchannel_t channel;
} adc_independent_channel_config_t;
typedef struct
{
    uint8_t sequenceLen;
    adc_independent_channel_config_t channels[ADC_MAX_SUPPORT_SEQUENCE_CHANS];
    adc_sequence_independent_mode_t sequenceMode;
    adc_trigger_t trigger;
    bool sequenceIntEnable;
} adc_sequence_independent_config_t;
#endif /* defined (CPU_YTM32Z1MD0) */

/*!
 * @brief Defines the channel sequence conversion configuration
 *
 * This structure is used to configure the sequence conversion
 * feature for the ADC
 *
 * Implements : adc_sequence_config_t_Class
 */
typedef struct
{
#if defined (CPU_YTM32Z1MD0)
    bool adcConvertContinue;
    adc_sequence_combine_mode_t seqCombineMode;
    adc_sequence_independent_config_t sequenceIndependentConfig[ADC_MAX_SUPPORT_SEQUENCE_NUM];
    bool sequenceErrIntEnable;
    bool singleErrIntEnable;
#else
    adc_inputchannel_t channels[ADC_CHSEL_COUNT];   /*!< Sequence of channels */
    uint8_t totalChannels;                          /*!< Total channels count sequence */
    adc_sequence_mode_t sequenceMode;               /*!< Sequence mode of conversion */
    bool sequenceIntEnable;                         /*!< Sequence end interrupt enable */
#endif /* defined (CPU_YTM32Z1MD0) */
    bool convIntEnable;                             /*!< Conversion end interrupt enable */
    bool ovrunIntEnable;                            /*!< Overrun interrupt enable */
    bool sampIntEnable;                             /*!< Sample end interrupt enable */
    bool readyIntEnable;                            /*!< Ready interrupt enable */    
} adc_sequence_config_t;

#if defined(FEATURE_ADC_SUPPORT_INJECTION_MODE) && (FEATURE_ADC_SUPPORT_INJECTION_MODE > 0)
/*!
 * @brief Defines the inject channel sequence conversion configuration
 *
 * This structure is used to configure the inject sequence conversion
 * feature for the ADC
 *
 * Implements : adc_inject_config_t_Class
 */
typedef struct
{
    bool injectEnable;                              /*!< Inject conversion enable */
    adc_inputchannel_t channels[ADC_INJCH_COUNT];   /*!< Sequence of channels */
    uint8_t totalChannels;                          /*!< Total channels count sequence */
    bool convIntEnable;                             /*!< Conversion end interrupt enable */
    bool errorIntEnable;                            /*!< Sample end interrupt enable */
} adc_inject_config_t;
#endif /* FEATURE_ADC_SUPPORT_INJECTION_MODE */

typedef uint8_t adc_clk_divide_t;

/*!
 * @brief AWG effective type selection
 *
 * Implements : adc_awg_effective_mode_t_Class
 */
typedef enum
{
    ADC_AWG_EFFECTIVE_OUTSIDE = 0x00U,   /*!< Effective outside. */
    ADC_AWG_EFFECTIVE_INSIDE = 0x01U     /*!< Effective inside. */
} adc_awg_effective_mode_t;

/*!
 * @brief Defines the hardware compare configuration
 *
 * This structure is used to configure the hardware compare
 * feature for the ADC
 *
 * Implements : adc_compare_config_t_Class
 */
typedef struct
{
#if defined(FEATURE_ADC_WDG_CHANNEL_COUNT) && (FEATURE_ADC_WDG_CHANNEL_COUNT > 1)
    uint8_t comWdchIndex;                   /*!< The index of watch dog configuration */
#endif /* FEATURE_ADC_WDG_CHANNEL_COUNT */
    bool compareEnable;                     /*!< Enable the compare feature */
    bool compareAllChannelEnable;           /*!< Enable all channel compare */
    adc_inputchannel_t compChalSel;         /*!< Compare channel when AWDG on a single channel */
    uint16_t compHigh;                      /*!< First Compare Value */
    uint16_t compLow;                       /*!< Second Compare Value */
    bool compIntEnable;                     /*!< Watch Dog Interrupt Enable */
    adc_awg_effective_mode_t effectiveMode; /*!< Watch Dog Effective Mode Selection */
} adc_compare_config_t;


/*!
 * @brief Defines the converter configuration
 *
 * This structure is used to configure the ADC converter
 *
 * Implements : adc_converter_config_t_Class
 */
typedef struct
{
    adc_clk_divide_t clockDivider;          /*!< Divider of the input clock for the ADC */
    uint8_t startTime;                      /*!< ADC startup time in AD Clocks */
    uint8_t sampleTime;                     /*!< Sample time in AD Clocks */
#if defined(FEATURE_ADC_SUPPORT_SECOND_SMP) && (FEATURE_ADC_SUPPORT_SECOND_SMP > 0)
    uint8_t sampleTime1;                     /*!< Another sample time in AD Clocks */
#endif

    bool overrunMode;                       /*!< Overrun management mode */
    bool autoOffEnable;                     /*!< Auto-off mode */
    bool waitEnable;                        /*!< Enable ADC wait FIFO read */
    uint8_t triggerSource;                  /*!< ADC trigger source */
    adc_trigger_t trigger;                  /*!< ADC trigger type (software, hardware) */
    adc_align_t align;                      /*!< ADC alignment */
    adc_resolution_t resolution;            /*!< ADC resolution (8,10,12 bit) */
    uint8_t dmaWaterMark;                   /*!< DMA water mark for the ADC result fifo */
    bool dmaEnable;                         /*!< Enable DMA for the ADC */
    adc_sequence_config_t sequenceConfig;   /*!< ADC sequence configuration */
#if defined(FEATURE_ADC_SUPPORT_INJECTION_MODE) && (FEATURE_ADC_SUPPORT_INJECTION_MODE > 0)
    adc_inject_config_t injectConfig;       /*!< ADC inject conversion configuration*/
#endif /* FEATURE_ADC_SUPPORT_INJECTION_MODE */
#if defined(FEATURE_ADC_WDG_CHANNEL_COUNT) && (FEATURE_ADC_WDG_CHANNEL_COUNT > 1)
    uint8_t adcCompareCnt;                   /*!< The count of ADC hardware compare configuration */
    adc_compare_config_t* compareConfig;     /*!< ADC hardware compare configuration */
#else
    adc_compare_config_t compareConfig;     /*!< ADC hardware compare configuration */
#endif /* FEATURE_ADC_WDG_CHANNEL_COUNT */
} adc_converter_config_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined (__cplusplus)
extern "C" {
#endif

/*!
 * @name Converter
 */
/*! @{*/

/*!
 * @brief Initializes the converter configuration structure
 *
 * This function initializes the members of the adc_converter_config_t
 * structure to default values (Reference Manual resets). This function should
 * be called on a structure before using it to configure the converter with
 * ADC_DRV_ConfigConverter(), otherwise all members must be written
 * (initialized) by the user. This function insures that all members are written
 * with safe values, so the user can modify only the desired members.
 *
 * @param[out] config the configuration structure
 */
void ADC_DRV_InitConverterStruct(adc_converter_config_t *const config);

/*!
 * @brief Configures the converter with the given configuration structure
 *
 * This function configures the ADC converter with the options
 * provided in the provided structure.
 *
 * @param[in] instance instance number
 * @param[in] config the configuration structure
 */
void ADC_DRV_ConfigConverter(const uint32_t instance,
                             const adc_converter_config_t *const config);

/*!
 * @brief Gets the current converter configuration
 *
 * This functions returns the configuration for converter in
 * the form of a configuration structure.
 *
 * @param[in] instance instance number
 * @param[out] config the configuration structure
 */
void ADC_DRV_GetConverterConfig(const uint32_t instance,
                                adc_converter_config_t *const config);

/*! @}*/

/*!
 * @name Hardware Compare
 * The Hardware Compare feature of the YTM32B1LD0 ADC is a versatile mechanism that
 * can be used to monitor that a value is within certain values. Measurements can
 * be monitored to be within certain ranges:
 *  - less than/ greater than a fixed value
 *  - inside or outside of a certain range
 *
 * Two compare values can be configured (the second value is used only for range
 * function mode). The compare values must be written in 12-bit resolution mode
 * regardless of the actual used resolution mode.
 *
 * Once the hardware compare feature is enabled, a conversion is considered
 * complete only when the measured value is within the allowable range set by
 * the configuration.
 */
/*! @{*/

/*!
 * @brief Initializes the Hardware Compare configuration structure
 *
 * This function initializes the Hardware Compare configuration
 * structure to default values (Reference Manual resets). This function should be
 * called before configuring the Hardware Compare feature (ADC_DRV_ConfigHwCompare),
 * otherwise all members must be written by the caller. This function insures
 * that all members are written with safe values, so the user can modify the
 * desired members.
 *
 * @param[out] config the configuration structure
 */
void ADC_DRV_InitHwCompareStruct(adc_compare_config_t *const config);

/*!
 * @brief Configures the Hardware Compare feature with the given configuration
 * structure
 *
 * This functions sets the configuration for the Hardware
 * Compare feature using the configuration structure.
 *
 * @param[in] instance instance number
 * @param[in] config the configuration structure
 */
void ADC_DRV_ConfigHwCompare(const uint32_t instance,
                             const adc_compare_config_t *const config);

#if defined(FEATURE_ADC_WDG_CHANNEL_COUNT) && (FEATURE_ADC_WDG_CHANNEL_COUNT > 1)
/*!
 * @brief Configures the Hardware Compare feature with the given configuration
 * structure
 *
 * This functions sets the configuration for the Hardware
 * Compare feature using the configuration structure.
 *
 * @param[in] instance instance number
 * @param[in] config pointer to the configuration structure
 * @param[in] count The count of ADC hardware compare configuration
 */
void ADC_DRV_ConfigHwCompareGroup(const uint32_t instance,
                                  adc_compare_config_t *const config, 
                                  uint8_t count);
#endif /* FEATURE_ADC_WDG_CHANNEL_COUNT */

/*!
 * @brief Gets the current Hardware Compare configuration
 *
 * This function returns the configuration for the Hardware
 * Compare feature.
 *
 * @param[in] instance instance number
 * @param[out] config the configuration structure
 */
void ADC_DRV_GetHwCompareConfig(const uint32_t instance,
                                adc_compare_config_t *const config);

/*! @}*/

/*!
 * @name Hardware Compare
 * The Hardware Compare feature of the ADC is a versatile mechanism that
 * can be used to monitor that a value is within certain values. Measurements can
 * be monitored to be within certain ranges:
 *  - less than/ greater than a fixed value
 *  - inside or outside of a certain range
 *
 * Two compare values can be configured (the second value is used only for range
 * function mode). The compare values must be written in 12-bit resolution mode
 * regardless of the actual used resolution mode.
 *
 * Once the hardware compare feature is enabled, a conversion is considered
 * complete only when the measured value is within the allowable range set by
 * the configuration.
 */
/*! @{*/

/*!
 * @brief Initializes the sequence configuration structure
 *
 * This function initializes the members of the adc_sequence_config_t
 * structure to default values (Reference Manual resets). This function should
 * be called on a structure before using it to configure the sequence with
 * ADC_DRV_ConfigSequence(), otherwise all members must be written
 * (initialized) by the user. This function insures that all members are written
 * with safe values, so the user can modify only the desired members.
 *
 * @param[out] config the configuration structure
 */
void ADC_DRV_InitSequenceStruct(adc_sequence_config_t *const config);

/*!
 * @brief Configures the sequence convert feature with the given configuration
 * structure
 *
 * This functions sets the configuration for the sequence
 * convert feature using the configuration structure.
 *
 * @param[in] instance instance number
 * @param[in] config the configuration structure
 */
void ADC_DRV_ConfigSequence(const uint32_t instance,
                            const adc_sequence_config_t *const config);

/*!
 * @brief Gets the current sequence convert configuration
 *
 * This function returns the configuration for the sequence
 * convert feature.
 *
 * @param[in] instance instance number
 * @param[out] config the configuration structure
 */
void ADC_DRV_GetSequenceConfig(const uint32_t instance,
                               adc_sequence_config_t *const config);

#if defined(FEATURE_ADC_SUPPORT_INJECTION_MODE) && (FEATURE_ADC_SUPPORT_INJECTION_MODE > 0)
/*!
 * @brief Configures the inject sequence convert feature with the given
 * configuration structure
 *
 * This functions sets the inject configuration for the sequence
 * convert feature using the configuration structure.
 *
 * @param[in] instance instance number
 * @param[in] config the inject configuration structure
 */
void ADC_DRV_ConfigInject(const uint32_t instance,
                          const adc_inject_config_t *const config);

/*!
 * @brief Gets the current inject sequence convert configuration
 *
 * This function returns the inject configuration for the sequence
 * convert feature.
 *
 * @param[in] instance instance number
 * @param[out] config the inject configuration structure
 */
void ADC_DRV_GetInjectConfig(const uint32_t instance,
                             adc_inject_config_t *const config);
#endif /* FEATURE_ADC_SUPPORT_INJECTION_MODE */
/*! @}*/

/*!
 * @name Converter
 * Converter specific methods. These are used to configure and use the A/D
 * Converter specific functionality, including:
 *  - clock input and divider
 *  - sample time in A/D clocks
 *  - resolution
 *  - trigger source
 *  - voltage reference
 *  - enable DMA
 *  - enable continuous conversion on one channel
 *
 * To start a conversion, a control channel (see \ref chan_config "Channel Configuration")
 * and a trigger source must be configured. Once a conversion is started, the user
 * application can wait for it to be finished by calling the ADC_DRV_WaitConvDone() function.
 *
 * Only the first control channel can be triggered by software. To start a
 * conversion in this case, an input channel must be written in the channel
 * selection register using the ADC_DRV_ConfigChan() method. Writing a value to
 * the control channel while a conversion is being performed on that channel
 * will start a new conversion.
 *
 */
/*! @{*/

/*!
 * @brief Resets the converter (sets all configurations to reset values)
 *
 * This function resets all the internal ADC registers to
 * reset values.
 *
 * @param[in] instance instance number
 */
void ADC_DRV_Reset(const uint32_t instance);

/*!
 * @brief Enable the converter
 *
 * This function set ADC to active mode
 *
 * @param[in] instance instance number
 */
void ADC_DRV_Enable(const uint32_t instance);

/*!
 * @brief Disable the converter
 *
 * This function set ADC to low power mode
 *
 * @param[in] instance instance number
 */
void ADC_DRV_Disable(const uint32_t instance);

/*!
 * @brief Start the converter
 *
 * This function start ADC conversion if and in software trigger mode or 
 * set ADC waiting for external trigger
 *
 * @param[in] instance instance number
 */
void ADC_DRV_Start(const uint32_t instance);

/*!
 * @brief Stop the converter
 *
 * This function stop ADC ongoing conversion and set ADC accept new configuration
 *
 * @param[in] instance instance number
 */
void ADC_DRV_Stop(const uint32_t instance);

/*!
 * @brief Waits for a conversion to finish
 *
 * This functions waits for a conversion to complete by
 * continuously polling the Conversion Active Flag.
 *
 * @param[in] instance instance number
 */
void ADC_DRV_WaitConvDone(const uint32_t instance);

/*!
 * @brief Waits for a sequence conversion to finish
 *
 * This functions waits for a sequence conversion to complete by
 * continuously polling the Sequence Conversion done Flag.
 *
 * @param[in] instance instance number
 */
void ADC_DRV_WaitSequenceDone(const uint32_t instance);

/*!
 * @brief Gets the control channel Conversion Complete Flag state
 *
 * This function returns the state of the Conversion Complete
 * flag for a control channel. This flag is set when a conversion
 * is complete or the condition generated by the Hardware
 * Compare feature is evaluated to true.
 *
 * @param[in] instance instance number
 * @return the Conversion Complete Flag state
 */
bool ADC_DRV_GetConvCompleteFlag(const uint32_t instance);

/*!
 * @brief Gets the control channel End of Conversion Flag state
 *
 * This function returns the state of the End of Conversion
 * flag for a control channel. This flag is set when a conversion
 * is complete or the condition generated by the Hardware
 * Compare feature is evaluated to true.
 *
 * @param[in] instance instance number
 * @return the End of Conversion Flag state
 */
bool ADC_DRV_GetEndOfConversionFlag(const uint32_t instance);

/*!
 * @brief Gets Full of Conversion Flag state
 *
 * This function returns the state of the Full of Conversion
 * flag for status of FIFO. This flag is set when FIFO storing
 * converted datas is full or the condition generated by the Hardware
 * Compare feature is evaluated to true.
 *
 * @param[in] instance instance number
 * @return the Full Flag state
 */
bool ADC_DRV_GetFullOfConversionFlag(const uint32_t instance);

/*!
 * @brief Gets OverRun of Conversion Flag state
 *
 * This function returns the state of the OverRun of Conversion
 * flag for status of FIFO. This flag is set when a new EOC comes 
 * while FIFO storing converted datas is already full 
 * or the condition generated by the Hardware
 * Compare feature is evaluated to true.
 *
 * @param[in] instance instance number
 * @return the OverRun Flag state
 */
bool ADC_DRV_GetOvrRunOfConversionFlag(const uint32_t instance);

/*!
 * @brief Gets Watchdog Flag state
 *
 * This function returns the state of the Watchdog Flag
 * This flag is set by hardware when the converted
 * voltage crossses the values programmed in the ADC_TR 
 * or the condition generated by the Hardware
 * Compare feature is evaluated to true.
 *
 * @param[in] instance instance number
 * @param[in] wdgChannel WDG channel number
 * @return the OverRun Flag state
 */
 #if defined(FEATURE_ADC_WDG_CHANNEL_COUNT) && (FEATURE_ADC_WDG_CHANNEL_COUNT > 1)
bool ADC_DRV_GetWatchdogFlag(const uint32_t instance, const uint8_t wdgChannel);
#else /* FEATURE_ADC_WDG_CHANNEL_COUNT == 1 */
bool ADC_DRV_GetWatchdogFlag(const uint32_t instance);
#endif /* FEATURE_ADC_WDG_CHANNEL_COUNT */

/*!
 * @brief Gets End of Sequence Conversion Flag state
 *
 * This function returns the state of the EOSEQ Flag
 * This flag is set by hardware at the end of conversion of a sequnce 
 * or the condition generated by the Hardware
 * Compare feature is evaluated to true.
 *
 * @param[in] instance instance number
 * @return the EOSEQ Flag state
 */
bool ADC_DRV_GetEndOfSequenceFlag(const uint32_t instance);

/*!
 * @brief Gets Ready Flag state
 *
 * This function returns the state of the Ready Flag
 * This flag is set by hardware when adc is ready.
 *
 * @param[in] instance instance number
 * @return the Ready Flag state
 */
bool ADC_DRV_GetReadyFlag(const uint32_t instance);

/*!
 * @brief Gets SampEnd Flag state
 *
 * This function returns the state of the Sample End Flag
 * This flag is set by hardware when adc sample is end.
 *
 * @param[in] instance instance number
 * @return the Ready Flag state
 */
bool ADC_DRV_GetSampEndFlag(const uint32_t instance);

/*!
 * @brief Gets the result from result FIFO
 *
 * This function returns the conversion result from FIFO
 *
 * @param[in] instance instance number
 * @return the result raw value
 */
uint16_t ADC_DRV_ReadFIFO(const uint32_t instance);


/*!
 * @brief Gets the result(Tag and Data) from result FIFO
 *
 * This function returns the conversion result from FIFO
 * which is a conbined data consist of Tag and Data of a
 * channel in sequence.
 *
 * @param[in] instance instance number
 * @return the result raw value with Tag and Data combined
 */
uint32_t ADC_DRV_ReadSeqtagAndData(const uint32_t instance);
/*! @}*/


/*!
 * @name Interrupts
 * This method returns the interrupt number for an ADC instance, which can be used
 * to configure the interrupt, like in Interrupt Manager.
 */
/*! @{*/

/*!
 * @brief Returns the interrupt number for the ADC instance.
 *
 * This function returns the interrupt number for the specified ADC instance.
 *
 * @param[in] instance instance number of the ADC
 * @return irq_number: the interrupt number (index) of the ADC instance, used to configure the interrupt
 */
IRQn_Type ADC_DRV_GetInterruptNumber(const uint32_t instance);

/*!
 * @brief Clear watchdog flag
 *
 * This function is used to clear WD flag.
 *
 * @param[in] instance instance number
 */
void ADC_DRV_ClearWdFlagCmd(const uint32_t instance);

/*!
 * @brief Clear Ready flag
 *
 * This function is used to clear Ready flag.
 *
 * @param[in] instance instance number
 */
void ADC_DRV_ClearReadyFlagCmd(const uint32_t instance);

/*!
 * @brief Clear Sample End flag
 *
 * This function is used to clear Sample End flag.
 *
 * @param[in] instance instance number
 */
void ADC_DRV_ClearSampEndFlagCmd(const uint32_t instance);

/*!
 * @brief Clear overrun flag
 *
 * This function is used to clear OVR flag.
 *
 * @param[in] instance instance number
 */
void ADC_DRV_ClearOvrFlagCmd(const uint32_t instance);

/*!
 * @brief Clear end of sequence flag
 *
 * This function is used to clear EOSEQ flag.
 *
 * @param[in] instance instance number
 */
void ADC_DRV_ClearEoseqFlagCmd(const uint32_t instance);

/*!
 * @brief Clear end of conversion flag
 *
 * This function is used to clear EOC flag.
 *
 * @param[in] instance instance number
 */
void ADC_DRV_ClearEocFlagCmd(const uint32_t instance);

#if defined (__cplusplus)
}
#endif

/*! @}*/

#endif /* ADC_DRIVER_H */
/*******************************************************************************
 * EOF
 ******************************************************************************/

