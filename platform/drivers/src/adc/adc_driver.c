/*
 * Copyright (c) 2021-2025 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file adc_driver.c
 * @version 1.4.0
 */

#include <stddef.h>
#include "adc_driver.h"
#include "adc_hw_access.h"
#include "clock_manager.h"


/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* Table of base addresses for ADC instances. */
static ADC_Type *const s_adcBase[ADC_INSTANCE_COUNT] = ADC_BASE_PTRS;

/*FUNCTION**********************************************************************
 *
 * Function Name : ADC_DRV_InitConverterStruct
 * Description   : This function initializes the members of the adc_converter_config_t
 * structure to default values (Reference Manual resets). This function should be called
 * on a structure before using it to configure the converter (ADC_DRV_ConfigConverter), otherwise all members
 * must be written (initialized) by the caller. This function insures that all members are written
 * with safe values, so the user can modify only the desired members.
 *
 * Implements : ADC_DRV_InitConverterStruct_Activity
 *END**************************************************************************/
void ADC_DRV_InitConverterStruct(adc_converter_config_t *const config)
{
    DEV_ASSERT(config != NULL);


    config->clockDivider = ADC_DEFAULT_CLOCK_DIVIDER;
    config->startTime = ADC_DEFAULT_START_TIME;
    config->sampleTime = ADC_DEFAULT_SAMPLE_TIME;

    config->overrunMode = false;
    config->autoOffEnable = false;
    config->waitEnable = false;
    config->triggerSource = 0x00;
#if defined(FEATURE_ADC_SUPPORT_INJECTION_MODE) && (FEATURE_ADC_SUPPORT_INJECTION_MODE > 0)
    config->trigger = ADC_TRIG_INJECT_SW_NORMAL_SW;
#else
    config->trigger = ADC_TRIGGER_SOFTWARE;
#endif /* FEATURE_ADC_SUPPORT_INJECTION_MODE */
    config->align = ADC_ALIGN_RIGHT;
    config->resolution = ADC_RESOLUTION_12BIT;
    config->dmaWaterMark = 0x01;
    config->dmaEnable = false;

    config->sequenceConfig.channels[0] = ADC_INPUTCHAN_EXT0;
    config->sequenceConfig.totalChannels = 1;
    config->sequenceConfig.sequenceMode = ADC_CONV_LOOP;
    config->sequenceConfig.sequenceIntEnable = false;
    config->sequenceConfig.convIntEnable = false;
    config->sequenceConfig.ovrunIntEnable = false;
    config->sequenceConfig.sampIntEnable = false;
    config->sequenceConfig.readyIntEnable = false;

#if defined(FEATURE_ADC_WDG_CHANNEL_COUNT) && (FEATURE_ADC_WDG_CHANNEL_COUNT > 1)
    config->adcCompareCnt = 0;
#else
    config->compareConfig.compareEnable = false;
    config->compareConfig.compareAllChannelEnable = false;
    config->compareConfig.compHigh = 0xFFF;
    config->compareConfig.compLow = 0x000;
    config->compareConfig.compIntEnable = false;
#endif /* FEATURE_ADC_WDG_CHANNEL_COUNT */
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ADC_DRV_ConfigConverter
 * Description   : This function configures the ADC converter with the options
 * provided in the configuration structure.
 *
 * Implements : ADC_DRV_ConfigConverter_Activity
 *END**************************************************************************/
void ADC_DRV_ConfigConverter(const uint32_t instance,
                             const adc_converter_config_t *const config)
{
    DEV_ASSERT(instance < ADC_INSTANCE_COUNT);
    DEV_ASSERT(config != NULL);

    ADC_Type *const base = s_adcBase[instance];
    clock_names_t adc_clocks[ADC_INSTANCE_COUNT] = ADC_CLOCKS;
    uint32_t adc_freq = 0u;
    status_t clk_status = CLOCK_SYS_GetFreq(adc_clocks[instance], &adc_freq);
    DEV_ASSERT(clk_status == STATUS_SUCCESS);
    (void) clk_status;

    adc_freq = adc_freq / (uint32_t) (1UL + ((uint32_t) (config->clockDivider)));
    DEV_ASSERT((adc_freq >= ADC_CLOCK_FREQ_MIN_RUNTIME) && (adc_freq <= ADC_CLOCK_FREQ_MAX_RUNTIME));
    (void) adc_freq;

    ADC_SetClockDivide(base, config->clockDivider);
    ADC_SetStartTime(base, config->startTime);
    ADC_SetSampleTime(base, config->sampleTime);
    ADC_SetOverrunModeFlag(base, config->overrunMode);
    ADC_SetautoOffEnableFlag(base, config->autoOffEnable);
    ADC_SetResolution(base, config->resolution);
    ADC_SetTriggerSource(base, config->triggerSource);
    ADC_SetTriggerMode(base, config->trigger);
    ADC_SetWaitEnableFlag(base, config->waitEnable);
    ADC_SetDMAEnableFlag(base, config->dmaEnable);
    ADC_SetDMAWatermark(base, config->dmaWaterMark);
    ADC_DRV_ConfigSequence(instance, &config->sequenceConfig);
#if FEATURE_ADC_SUPPORT_LOW_POWER_KEEP
    ADC_SetLowPowerEnableFlag(base, false);
#endif
#if defined(FEATURE_ADC_SUPPORT_INJECTION_MODE) && (FEATURE_ADC_SUPPORT_INJECTION_MODE > 0)
    ADC_DRV_ConfigInject(instance, &config->injectConfig);
#endif /* FEATURE_ADC_SUPPORT_INJECTION_MODE */
#if defined(FEATURE_ADC_WDG_CHANNEL_COUNT) && (FEATURE_ADC_WDG_CHANNEL_COUNT > 1)
    ADC_DRV_ConfigHwCompareGroup(instance, config->compareConfig, config->adcCompareCnt);
#else
    ADC_DRV_ConfigHwCompare(instance, &config->compareConfig);
#endif /* FEATURE_ADC_WDG_CHANNEL_COUNT */
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ADC_DRV_GetConverterConfig
 * Description   : This functions returns the current converter configuration in
 * the form of a configuration structure.
 *
 * Implements : ADC_DRV_GetConverterConfig_Activity
 *END**************************************************************************/
void ADC_DRV_GetConverterConfig(const uint32_t instance,
                                adc_converter_config_t *const config)
{
    DEV_ASSERT(instance < ADC_INSTANCE_COUNT);
    DEV_ASSERT(config != NULL);

    ADC_Type *const base = s_adcBase[instance];
    config->clockDivider = ADC_GetClockDivide(base);
    config->sampleTime = ADC_GetSampleTime(base);
    config->resolution = ADC_GetResolution(base);
    config->trigger = ADC_GetTriggerMode(base);
    config->waitEnable = ADC_GetWaitEnableFlag(base);
    config->dmaEnable = ADC_GetDMAEnableFlag(base);
    config->dmaWaterMark = ADC_GetDMAWatermark(base);
    ADC_DRV_GetSequenceConfig(instance, &config->sequenceConfig);
#if defined(FEATURE_ADC_SUPPORT_INJECTION_MODE) && (FEATURE_ADC_SUPPORT_INJECTION_MODE > 0)
    ADC_DRV_GetInjectConfig(instance, &config->injectConfig);
#endif /* FEATURE_ADC_SUPPORT_INJECTION_MODE */
#if defined(FEATURE_ADC_WDG_CHANNEL_COUNT) && (FEATURE_ADC_WDG_CHANNEL_COUNT > 1)
#else
    ADC_DRV_GetHwCompareConfig(instance, &config->compareConfig);
#endif /* FEATURE_ADC_WDG_CHANNEL_COUNT */
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ADC_DRV_Reset
 * Description   : This function writes all the internal ADC registers with
 * their Reference Manual reset values.
 *
 * Implements : ADC_DRV_Reset_Activity
 *END**************************************************************************/
void ADC_DRV_Reset(const uint32_t instance)
{
    DEV_ASSERT(instance < ADC_INSTANCE_COUNT);

    ADC_DRV_Disable(instance);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ADC_DRV_Enable
 * Description   : This function enable the ADC.
 *
 * Implements : ADC_DRV_Enable_Activity
 *END**************************************************************************/
void ADC_DRV_Enable(const uint32_t instance)
{
    DEV_ASSERT(instance < ADC_INSTANCE_COUNT);

    ADC_Type *const baseAddr = s_adcBase[instance];

    ADC_Enable(baseAddr);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ADC_DRV_Disable
 * Description   : This function disable ADC and let ADC enter low power mode.
 * Disable ADC will abort all ongoing conversion.
 * 
 * Implements : ADC_DRV_Disable_Activity
 *END**************************************************************************/
void ADC_DRV_Disable(const uint32_t instance)
{
    DEV_ASSERT(instance < ADC_INSTANCE_COUNT);

    ADC_Type *const baseAddr = s_adcBase[instance];

    if (ADC_GetEnableFlag(baseAddr))
    {
        ADC_Stop(baseAddr);
        ADC_Disable(baseAddr);
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ADC_DRV_Start
 * Description   : This function start the ADC.
 * If software trigger is enabled, ADC will start sequence convert.
 * If hardware trigger is enabled, ADC will wait valid trigger then start 
 * sequence convert.
 *
 * Implements : ADC_DRV_Start_Activity
 *END**************************************************************************/
void ADC_DRV_Start(const uint32_t instance)
{
    DEV_ASSERT(instance < ADC_INSTANCE_COUNT);

    ADC_Type *const baseAddr = s_adcBase[instance];

    ADC_Enable(baseAddr);

    ADC_Start(baseAddr);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ADC_DRV_Stop
 * Description   : This function stop the ADC.
 * Stop ADC will abort all ongoing conversion.
 *
 * Implements : ADC_DRV_Stop_Activity
 *END**************************************************************************/
void ADC_DRV_Stop(const uint32_t instance)
{
    DEV_ASSERT(instance < ADC_INSTANCE_COUNT);

    ADC_Type *const baseAddr = s_adcBase[instance];

    ADC_Stop(baseAddr);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ADC_DRV_InitHwCompareStruct
 * Description   : This function initializes the Hardware Compare configuration
 * structure to default values (Reference Manual resets). This function should be
 * called before configuring the Hardware Compare feature (ADC_DRV_ConfigHwCompare),
 * otherwise all members must be written by the caller. This function insures that all
 * members are written with safe values, so the user can modify the desired members.
 *
 * Implements : ADC_DRV_InitHwCompareStruct_Activity
 *END**************************************************************************/
void ADC_DRV_InitHwCompareStruct(adc_compare_config_t *const config)
{
    DEV_ASSERT(config != NULL);

    config->compareEnable = false;
    config->compareAllChannelEnable = false;
    config->compHigh = 0U;
    config->compLow = 0U;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ADC_DRV_ConfigHwCompare
 * Description   : This functions sets the configuration for the Hardware
 * Compare feature using the configuration structure.
 *
 * Implements : ADC_DRV_ConfigHwCompare_Activity
 *END**************************************************************************/
void ADC_DRV_ConfigHwCompare(const uint32_t instance,
                             const adc_compare_config_t *const config)
{
    DEV_ASSERT(instance < ADC_INSTANCE_COUNT);
    DEV_ASSERT(config != NULL);

    uint8_t wdgChannel = 0;
    ADC_Type *const base = s_adcBase[instance];
    ADC_SetHwCompareEnableFlag(base, config->compareEnable, wdgChannel);
    ADC_SetHwCompareAllEnableFlag(base, config->compareAllChannelEnable);
    ADC_SetHwCompareRangeEnableFlag(base, config->compChalSel, wdgChannel);
    ADC_SetHwCompareCompHighValue(base, config->compHigh, wdgChannel);
    ADC_SetHwCompareCompLowValue(base, config->compLow, wdgChannel);
    ADC_SetAwdIntEnableFlag(base, config->compIntEnable);
    ADC_SetAwdEffectiveMode(base, (config->effectiveMode == ADC_AWG_EFFECTIVE_INSIDE) ? true : false, wdgChannel);
}

#if defined(FEATURE_ADC_WDG_CHANNEL_COUNT) && (FEATURE_ADC_WDG_CHANNEL_COUNT > 1)
/*FUNCTION**********************************************************************
 *
 * Function Name : ADC_DRV_ConfigHwCompareGroup
 * Description   : This functions sets the configuration for the Hardware
 * Compare feature using the configuration structure.
 *
 * Implements : ADC_DRV_ConfigHwCompareGroup_Activity
 *END**************************************************************************/
void ADC_DRV_ConfigHwCompareGroup(const uint32_t instance,
                                  adc_compare_config_t *const config, 
                                  uint8_t count)
{
    DEV_ASSERT(instance < ADC_INSTANCE_COUNT);
    if (count != 0u)
    {
        DEV_ASSERT(config != NULL);
    
        ADC_Type *const base = s_adcBase[instance];
        for(uint8_t i = 0; i < count; i ++)
        {
            ADC_SetHwCompareEnableFlag(base, config[i].compareEnable, config[i].comWdchIndex);
            ADC_SetHwCompareRangeEnableFlag(base, config[i].compChalSel, config[i].comWdchIndex);
            ADC_SetHwCompareCompHighValue(base, config[i].compHigh, config[i].comWdchIndex);
            ADC_SetHwCompareCompLowValue(base, config[i].compLow, config[i].comWdchIndex);
            ADC_SetAwdIntEnableFlag(base, config[i].compIntEnable);
            ADC_SetAwdEffectiveMode(base, (config->effectiveMode == ADC_AWG_EFFECTIVE_INSIDE) ? true : false, config[i].comWdchIndex);
            /* Only WD0CH has this feature */
            if(config[i].comWdchIndex == 0u)
            {
                ADC_SetHwCompareAllEnableFlag(base, config[i].compareAllChannelEnable);
            }
        }
    }
}
#endif /* FEATURE_ADC_WDG_CHANNEL_COUNT */

/*FUNCTION**********************************************************************
 *
 * Function Name : ADC_DRV_GetHwCompareConfig
 * Description   : This function returns the configuration for the Hardware
 * Compare feature.
 *
 * Implements : ADC_DRV_GetHwCompareConfig_Activity
 *END**************************************************************************/
void ADC_DRV_GetHwCompareConfig(const uint32_t instance,
                                adc_compare_config_t *const config)
{
    DEV_ASSERT(instance < ADC_INSTANCE_COUNT);
    DEV_ASSERT(config != NULL);

    uint8_t wdgChannel = 0;
    ADC_Type *const base = s_adcBase[instance];
    config->compareEnable = ADC_GetHwCompareEnableFlag(base, wdgChannel);
    config->compareAllChannelEnable = ADC_GetHwCompareAllEnableFlag(base);
    config->compHigh = ADC_GetHwCompareCompHighValue(base, wdgChannel);
    config->compLow = ADC_GetHwCompareCompLowValue(base, wdgChannel);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ADC_DRV_InitSequenceStruct
 * Description   : This function initializes the control sequence
 * configuration structure to default values (Reference Manual resets). This function should
 * be called on a structure before using it to configure a sequence (ADC_DRV_ConfigSequence), otherwise
 * all members must be written by the caller. This function insures that all members are written
 * with safe values, so the user can modify only the desired members.
 *
 * Implements : ADC_DRV_InitSequenceStruct_Activity
 *END**************************************************************************/
void ADC_DRV_InitSequenceStruct(adc_sequence_config_t *const config)
{
    DEV_ASSERT(config != NULL);

    config->channels[0] = ADC_INPUTCHAN_EXT0;
    config->totalChannels = 1;
    config->sequenceMode = ADC_CONV_LOOP;
    config->sequenceIntEnable = false;
    config->convIntEnable = false;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ADC_DRV_ConfigSequence
 * Description   : This function sets a control sequence configuration.
 *
 * Configuring any control sequence while it is actively controlling a conversion
 * (sw or hw triggered) will implicitly abort the on-going conversion.
 *
 * Implements : ADC_DRV_ConfigSequence_Activity
 *END**************************************************************************/
void ADC_DRV_ConfigSequence(const uint32_t instance,
                            const adc_sequence_config_t *const config)
{
    uint8_t i;
    DEV_ASSERT(instance < ADC_INSTANCE_COUNT);
    DEV_ASSERT(config != NULL);
    DEV_ASSERT(0 < config->totalChannels);
    DEV_ASSERT(ADC_CHSEL_COUNT >= config->totalChannels);

    ADC_Type *const base = s_adcBase[instance];
    /* Stop ADC conversion before re-configuration */
    ADC_Stop(base);
    for (i = 0; i < ADC_CHSEL_COUNT; i++)
    {
        ADC_SetSequeceChannel(base, i, config->channels[i]);
    }
    ADC_SetSequeceTotalChannel(base, config->totalChannels);
    ADC_SetSequenceMode(base, config->sequenceMode);
    ADC_SetEocIntEnableFlag(base, config->convIntEnable);
    ADC_SetEoSeqIntEnableFlag(base, config->sequenceIntEnable);
    ADC_SetOvrIntEnableFlag(base, config->ovrunIntEnable);
    ADC_SetSampEndIntEnableFlag(base, config->sampIntEnable);
    ADC_SetReadyIntEnableFlag(base, config->readyIntEnable);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ADC_DRV_GetSequenceConfig
 * Description   : This function returns the current configuration for a control
 * sequence.
 *
 * Implements : ADC_DRV_GetSequenceConfig_Activity
 *END**************************************************************************/
void ADC_DRV_GetSequenceConfig(const uint32_t instance,
                               adc_sequence_config_t *const config)
{
    uint8_t i;
    DEV_ASSERT(instance < ADC_INSTANCE_COUNT);
    DEV_ASSERT(config != NULL);

    ADC_Type *const base = s_adcBase[instance];
    for (i = 0; i < ADC_CHSEL_COUNT; i++)
    {
        config->channels[i] = ADC_GetSequeceChannel(base, i);
    }
    config->totalChannels = ADC_GetSequeceTotalChannel(base) + 1u;
    config->sequenceMode = ADC_GetSequenceMode(base);
    config->convIntEnable = ADC_GetEocIntEnableFlag(base);
    config->sequenceIntEnable = ADC_GetEoSeqIntEnableFlag(base);
}

#if defined(FEATURE_ADC_SUPPORT_INJECTION_MODE) && (FEATURE_ADC_SUPPORT_INJECTION_MODE > 0)
/*FUNCTION**********************************************************************
 *
 * Function Name : ADC_DRV_ConfigInject
 * Description   : This function sets a inject control sequence configuration.
 *
 * Configuring any control sequence while it is actively controlling a conversion
 * (sw or hw triggered) will implicitly abort the on-going conversion.
 *
 * Implements : ADC_DRV_ConfigInject_Activity
 *END**************************************************************************/
void ADC_DRV_ConfigInject(const uint32_t instance,
                            const adc_inject_config_t *const config)
{
    uint8_t i;
    DEV_ASSERT(instance < ADC_INSTANCE_COUNT);
    DEV_ASSERT(config != NULL);
    if (config->injectEnable)
    {
        DEV_ASSERT(0 < config->totalChannels);
        DEV_ASSERT(ADC_INJCH_COUNT >= config->totalChannels);

        ADC_Type *const base = s_adcBase[instance];
        /* Stop ADC conversion before re-configuration */
        ADC_Stop(base);
        for (i = 0; i < ADC_INJCH_COUNT; i++)
        {
            ADC_SetInjectChannel(base, i, config->channels[i]);
        }
        ADC_SetInjectTotalChannel(base, config->totalChannels);
        ADC_SetInjectIntEnableFlag(base, config->convIntEnable);
        ADC_SetInjectErrIntEnableFlag(base, config->errorIntEnable);
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ADC_DRV_GetInjectConfig
 * Description   : This function returns the current configuration for a inject
 * control sequence.
 *
 * Implements : ADC_DRV_GetInjectConfig_Activity
 *END**************************************************************************/
void ADC_DRV_GetInjectConfig(const uint32_t instance,
                               adc_inject_config_t *const config)
{
    uint8_t i;
    DEV_ASSERT(instance < ADC_INSTANCE_COUNT);
    DEV_ASSERT(config != NULL);

    if (config->injectEnable)
    {
        ADC_Type *const base = s_adcBase[instance];
        for (i = 0; i < ADC_INJCH_COUNT; i++)
        {
            config->channels[i] = ADC_GetSequeceChannel(base, i);
        }
        config->totalChannels = ADC_GetInjectTotalChannel(base) + 1;
        config->convIntEnable = ADC_GetInjectIntEnableFlag(base);
        config->errorIntEnable = ADC_GetInjectErrIntEnableFlag(base);
    }
}
#endif /* FEATURE_ADC_SUPPORT_INJECTION_MODE */

/*FUNCTION**********************************************************************
 *
 * Function Name : ADC_DRV_WaitConvDone
 * Description   : This functions waits for a conversion to complete by
 * continuously polling the Conversion Active Flag.
 *
 * Implements : ADC_DRV_WaitConvDone_Activity
 *END**************************************************************************/
void ADC_DRV_WaitConvDone(const uint32_t instance)
{
    DEV_ASSERT(instance < ADC_INSTANCE_COUNT);

    ADC_Type *const base = s_adcBase[instance];
    while (ADC_GetConvActiveFlag(base) == true)
    {
        /* Wait for conversion to finish */
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ADC_DRV_WaitSequenceDone
 * Description   : This functions waits for a sequence conversion to complete by
 * continuously polling the sequence done Flag.
 *
 * Implements : ADC_DRV_WaitSequenceDone_Activity
 *END**************************************************************************/
void ADC_DRV_WaitSequenceDone(const uint32_t instance)
{
    DEV_ASSERT(instance < ADC_INSTANCE_COUNT);

    ADC_Type *const base = s_adcBase[instance];
    while (ADC_GetSequenceDoneFlag(base) == false)
    {
        /* Wait for sequence conversion to finish */
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ADC_DRV_GetConvCompleteFlag
 * Description   : This function returns the state of the Conversion Complete
 * flag for a control channel. This flag is set when a conversion is complete
 * or the condition generated by the Hardware Compare feature is evaluated to true.
 *
 * Implements : ADC_DRV_GetConvCompleteFlag_Activity
 *END**************************************************************************/
bool ADC_DRV_GetConvCompleteFlag(const uint32_t instance)
{
    DEV_ASSERT(instance < ADC_INSTANCE_COUNT);

    ADC_Type *const base = s_adcBase[instance];

    return (ADC_GetConvCompleteFlag(base));
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ADC_DRV_GetEndOfConversionFlag
 * Description   : This function returns the state of the Conversion Complete
 * flag for a control channel. This flag is set when a conversion is complete
 * or the condition generated by the Hardware Compare feature is evaluated to true.
 *
 * Implements : ADC_DRV_GetEndOfConversionFlag_Activity
 *END**************************************************************************/
bool ADC_DRV_GetEndOfConversionFlag(const uint32_t instance)
{
    DEV_ASSERT(instance < ADC_INSTANCE_COUNT);

    ADC_Type *const base = s_adcBase[instance];

    uint32_t tmp = base->STS;
    tmp = (tmp & ADC_STS_EOC_MASK) >> ADC_STS_EOC_SHIFT;
    return (tmp != 0u) ? true : false;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ADC_DRV_GetFullOfConversionFlag
 * Description   : This function returns the state of the Conversion Full
 * flag for a control channel. This flag is set when FIFO storing converted data is full
 * or the condition generated by the Hardware Compare feature is evaluated to true.
 *
 * Implements : ADC_DRV_GetFullOfConversionFlag_Activity
 *END**************************************************************************/
bool ADC_DRV_GetFullOfConversionFlag(const uint32_t instance)
{
    DEV_ASSERT(instance < ADC_INSTANCE_COUNT);

    ADC_Type *const base = s_adcBase[instance];

    uint32_t tmp = base->STS;
    tmp = (tmp & ADC_STS_FULL_MASK) >> ADC_STS_FULL_SHIFT;
    return (tmp != 0u) ? true : false;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ADC_DRV_GetOvrRunOfConversionFlag
 * Description   : This function returns the state of the Conversion OverRun
 * flag for a control channel. This flag is set when a new EOC comes
 * while FIFO storing  converted datas is already full
 * or the condition generated by the Hardware Compare feature is evaluated to true.
 *
 * Implements : ADC_DRV_GetOvrRunOfConversionFlag_Activity
 *END**************************************************************************/
bool ADC_DRV_GetOvrRunOfConversionFlag(const uint32_t instance)
{
    DEV_ASSERT(instance < ADC_INSTANCE_COUNT);

    ADC_Type *const base = s_adcBase[instance];

    uint32_t tmp = base->STS;
    tmp = (tmp & ADC_STS_OVR_MASK) >> ADC_STS_OVR_SHIFT;
    return (tmp != 0u) ? true : false;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ADC_DRV_GetWatchdogFlag
 * Description   : This function returns the Watchdog flag by hareware when the
 * converted voltage crosses the values programmed in the ADC_TR
 *
 * Implements : ADC_DRV_GetWatchdogFlag_Activity
 *END**************************************************************************/
#if defined(FEATURE_ADC_WDG_CHANNEL_COUNT) && (FEATURE_ADC_WDG_CHANNEL_COUNT > 1)
bool ADC_DRV_GetWatchdogFlag(const uint32_t instance, const uint8_t wdgChannel)
#else /* FEATURE_ADC_WDG_CHANNEL_COUNT == 1U */
bool ADC_DRV_GetWatchdogFlag(const uint32_t instance)
#endif /* FEATURE_ADC_WDG_CHANNEL_COUNT */
{
    DEV_ASSERT(instance < ADC_INSTANCE_COUNT);

    ADC_Type *const base = s_adcBase[instance];

    uint32_t tmp = base->STS;
#if defined(FEATURE_ADC_WDG_CHANNEL_COUNT) && (FEATURE_ADC_WDG_CHANNEL_COUNT > 1)
    tmp = (tmp >> (ADC_STS_WD0_SHIFT + wdgChannel)) & 0x01U;
#else
    tmp = (tmp & ADC_STS_WD_MASK) >> ADC_STS_WD_SHIFT;
#endif /* FEATURE_ADC_WDG_CHANNEL_COUNT */
    return (tmp != 0u) ? true : false;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ADC_DRV_GetEndOfSequenceFlag
 * Description   : This function returns the state of the Sequence Conversion Complete
 * flag for a control channel. This flag is at the end of conversion of a sequence
 * or the condition generated by the Hardware Compare feature is evaluated to true.
 *
 * Implements : ADC_DRV_GetEndOfSequenceFlag_Activity
 *END**************************************************************************/
bool ADC_DRV_GetEndOfSequenceFlag(const uint32_t instance)
{
    DEV_ASSERT(instance < ADC_INSTANCE_COUNT);

    ADC_Type *const base = s_adcBase[instance];

    uint32_t tmp = base->STS;
    tmp = (tmp & ADC_STS_EOSEQ_MASK) >> ADC_STS_EOSEQ_SHIFT;
    return (tmp != 0u) ? true : false;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ADC_DRV_GetReadyFlag
 * Description   : This function returns the state of the ADC Ready
 * flag for a control channel.
 *
 * Implements : ADC_DRV_GetReadyFlag_Activity
 *END**************************************************************************/
bool ADC_DRV_GetReadyFlag(const uint32_t instance)
{
    DEV_ASSERT(instance < ADC_INSTANCE_COUNT);

    ADC_Type *const base = s_adcBase[instance];

    uint32_t tmp = base->STS;
    tmp = tmp & ADC_STS_ADRDY_MASK;
    return (tmp != 0u) ? true : false;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ADC_DRV_GetSampEndFlag
 * Description   : This function returns the state of the Sample End
 * flag for a control channel.
 *
 * Implements : ADC_DRV_GetSampEndFlag_Activity
 *END**************************************************************************/
bool ADC_DRV_GetSampEndFlag(const uint32_t instance)
{
    DEV_ASSERT(instance < ADC_INSTANCE_COUNT);

    ADC_Type *const base = s_adcBase[instance];

    uint32_t tmp = base->STS;
    tmp = (tmp & ADC_STS_EOSMP_MASK) >> ADC_STS_EOSMP_SHIFT;
    return (tmp != 0u) ? true : false;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ADC_DRV_ReadFIFO
 * Description   : This function returns the conversion result from FIFO.
 *
 * Implements : ADC_DRV_ReadFIFO_Activity
 *END**************************************************************************/
uint16_t ADC_DRV_ReadFIFO(const uint32_t instance)
{
    ADC_Type *const base = s_adcBase[instance];

    return (ADC_ReadFIFO(base));
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ADC_DRV_ReadSeqtagAndData
 * Description   : This function returns the conversion result from FIFO,
 * which is a conbined data consist of Tag and Data of a channel in Sequence.
 *
 * Implements : ADC_DRV_ReadSeqtagAndData_Activity
 *END**************************************************************************/
uint32_t ADC_DRV_ReadSeqtagAndData(const uint32_t instance)
{
    ADC_Type *const base = s_adcBase[instance];

    return ((uint32_t) base->FIFO);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ADC_DRV_GetInterruptNumber
 * Description   : This function returns the interrupt number for the specified ADC instance.
 *
 * Implements : ADC_DRV_GetInterruptNumber_Activity
 *END**************************************************************************/
IRQn_Type ADC_DRV_GetInterruptNumber(const uint32_t instance)
{
    DEV_ASSERT(instance < ADC_INSTANCE_COUNT);

    static const IRQn_Type adcIrqId[ADC_INSTANCE_COUNT] = ADC_IRQS;
    IRQn_Type irqId = adcIrqId[instance];

    return irqId;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ADC_DRV_ClearReadyFlagCmd
 * Description   : This function is used to clear Eeady flag.
 *
 * Implements    : ADC_DRV_ClearReadyFlagCmd_Activity
 *END**************************************************************************/

void ADC_DRV_ClearReadyFlagCmd(const uint32_t instance)
{
    DEV_ASSERT(instance < ADC_INSTANCE_COUNT);

    ADC_Type *const baseAddr = s_adcBase[instance];

    ADC_ClearReadyFlagCmd(baseAddr);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ADC_DRV_ClearSampEndFlagCmd
 * Description   : This function is used to clear Sample End flag.
 *
 * Implements    : ADC_DRV_ClearSampEndFlagCmd_Activity
 *END**************************************************************************/

void ADC_DRV_ClearSampEndFlagCmd(const uint32_t instance)
{
    DEV_ASSERT(instance < ADC_INSTANCE_COUNT);

    ADC_Type *const baseAddr = s_adcBase[instance];

    ADC_ClearSampEndFlagCmd(baseAddr);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ADC_DRV_ClearWdFlagCmd
 * Description   : This function is used to clear AWD flag.
 *
 * Implements    : ADC_DRV_ClearWdFlagCmd_Activity
 *END**************************************************************************/

void ADC_DRV_ClearWdFlagCmd(const uint32_t instance)
{
    DEV_ASSERT(instance < ADC_INSTANCE_COUNT);

    ADC_Type *const baseAddr = s_adcBase[instance];
    ADC_ClearWdFlagCmd(baseAddr, 0);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ADC_DRV_ClearOvrFlagCmd
 * Description   : This function is used to clear OVR flag.
 *
 * Implements    : ADC_DRV_ClearOvrFlagCmd_Activity
 *END**************************************************************************/

void ADC_DRV_ClearOvrFlagCmd(const uint32_t instance)
{
    DEV_ASSERT(instance < ADC_INSTANCE_COUNT);

    ADC_Type *const baseAddr = s_adcBase[instance];

    ADC_ClearOvrFlagCmd(baseAddr);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ADC_DRV_ClearEoseqFlagCmd
 * Description   : This function is used to clear EOSEQ flag.
 *
 * Implements    : ADC_DRV_ClearEoseqFlagCmd_Activity
 *END**************************************************************************/

void ADC_DRV_ClearEoseqFlagCmd(const uint32_t instance)
{
    DEV_ASSERT(instance < ADC_INSTANCE_COUNT);

    ADC_Type *const baseAddr = s_adcBase[instance];

    ADC_ClearEoseqFlagCmd(baseAddr);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ADC_DRV_ClearEocFlagCmd
 * Description   : This function is used to clear EOC flag.
 *
 * Implements    : ADC_DRV_ClearEocFlagCmd_Activity
 *END**************************************************************************/

void ADC_DRV_ClearEocFlagCmd(const uint32_t instance)
{
    DEV_ASSERT(instance < ADC_INSTANCE_COUNT);

    ADC_Type *const baseAddr = s_adcBase[instance];

    ADC_ClearEocFlagCmd(baseAddr);
}

/******************************************************************************
 * EOF
 *****************************************************************************/
