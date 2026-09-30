/*
 * Copyright 2020-2025 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file mpwm_pwm_driver.c
 * @version 1.4.0
 */

/*!
 * @page misra_violations MISRA-C:2012 violations list
 *
 * PRQA S 2016 Rule 16.4: This 'switch' statement 'default' clause is empty.
 *
 */

#include "mpwm_pwm_driver.h"
#include "mpwm_hw_access.h"

/*******************************************************************************
 * Declaration
 ******************************************************************************/



/*******************************************************************************
 * API Definition
 ******************************************************************************/

/*!
 * @brief Initialize MPWM PWM mode
 */
status_t MPWM_DRV_InitPwm(uint32_t instance, const mpwm_pwm_config_t *config)
{
    DEV_ASSERT(instance < MPWM_INSTANCE_COUNT);
    DEV_ASSERT(config != NULL);

    MPWM_Type *base = g_mpwmBase[instance];
    status_t status = STATUS_SUCCESS;
    mpwm_state_t *state = g_mpwmState[instance];
    uint16_t outputInitSet = 0;
    uint16_t outputInit = 0;
    uint32_t polaritySet = 0;
    mpwm_ch_pwm_align_mode_t align;
    uint8_t index;
    uint8_t channel;
    mpwm_ch_reload_src_t rldSrc;
    bool dbgMode;
    mpwm_ch_clk_prs_t prs;
    uint32_t freq;
    uint32_t periodTicks;
    uint16_t dutyCycle;
    bool perOutputTrigEn;
    bool cmpOutputTrigEn;
    bool intEn;
    bool tofIntEn;

    for (index = 0; index < config->channelNum; index++)
    {
        channel = config->pwmChannelConfig[index].hwChannelId;
        intEn = config->pwmChannelConfig[index].intEn;
        tofIntEn = config->pwmChannelConfig[index].timerOverflowIntEn;
        outputInit = (uint16_t)config->pwmChannelConfig[index].outputInit;
        align = config->pwmChannelConfig[index].alignMode;

        switch (align)
        {
            case MPWM_PWM_RIGHT_ALIGN:
            case MPWM_PWM_CENTER_H_ALIGN:
                polaritySet &= ~((uint32_t)0x1U << channel);
                break;
            case MPWM_PWM_LEFT_ALIGN:
                polaritySet |= ((uint32_t)0x1U << channel);
                outputInit = ~outputInit & 0x1U;
                break;
            default: /*PRQA S 2016*/
                break;
        }

        outputInitSet |= outputInit << channel;
        state->mpwmChAlignMode[channel] = config->pwmChannelConfig[index].alignMode;
        state->mpwmChUpdateUnit[channel] = config->pwmChannelConfig[index].unit;

        if (intEn)
        {
            /* Set interrupt enable */
            MPWM_SetChannelIntEnState(base, channel, intEn);
            INT_SYS_EnableIRQ(g_mpwmChnIrqId[instance][channel]);
        }

        if (tofIntEn)
        {
            /* Set timer overflow interrupt enable */
            MPWM_SetChannelTofIntEnState(base, channel, tofIntEn);
            INT_SYS_EnableIRQ(g_mpwmOvfIrqId[instance][channel]);
        }
    }

    /* Set polarity together */
    MPWM_SetPolarity(base, polaritySet);

    /* Set output initial value together */
    MPWM_SetOutputInit(base, outputInitSet);

    for (index = 0; index < config->channelNum; index++)
    {
        channel = config->pwmChannelConfig[index].hwChannelId;
        rldSrc = config->pwmChannelConfig[index].reloadSrc;
        dbgMode = config->pwmChannelConfig[index].debugMode;
        prs = config->pwmChannelConfig[index].clkPrs;
        freq = config->pwmChannelConfig[index].frequencyHz;
        dutyCycle = config->pwmChannelConfig[index].dutyCyclePercent;
        perOutputTrigEn = config->pwmChannelConfig[index].outputTrigConfig.perTrigOutputEn;
        cmpOutputTrigEn = config->pwmChannelConfig[index].outputTrigConfig.cmpTrigOutputEn;

        /* Set channel counter clock prescaler */
        MPWM_SetClkPrs(base, channel, (uint8_t)prs);

        /* Get channel counter frequency */
        state->mpwmChFreq[channel] = (state->mpwmClkSrcFreq / ((uint32_t)1U << (uint8_t)prs));

        /* Set reload source */
        MPWM_SetReloadSrc(base, channel, (uint8_t)rldSrc);

        /* Set debug mode disable or not */
        MPWM_SetDebugMode(base, channel, dbgMode);

        /* Set period output trigger enable or not */
        MPWM_SetPerOutputTrigEnState(base, channel, perOutputTrigEn);

        /* Set compare output trigger enable or not */
        MPWM_SetCmpOutputTrigEnState(base, channel, cmpOutputTrigEn);

        /* Convert frequency in Hz to ticks */
        periodTicks = MPWM_DRV_ConvertFreqToPeriodTicks(instance, channel, freq);

        /* Set channel period */
        MPWM_SetChannelPeriod(base, channel, periodTicks);

        state->mpwmChPeriodTicks[channel] = (uint16_t)periodTicks;

        /* Set channel duty cycle */
        status = MPWM_DRV_UpdateChannelDutyCycle(instance, channel, dutyCycle);

        /* Set channel as pwm mode */
        if ((state->mpwmChAlignMode[channel] == MPWM_PWM_RIGHT_ALIGN) ||
            (state->mpwmChAlignMode[channel] == MPWM_PWM_LEFT_ALIGN))
        {
            MPWM_SetChannelMode(base, channel, (uint8_t)MPWM_PWM_MODE);
            state->mpwmChMode[channel] = MPWM_PWM_MODE;
        }
        else
        {
            MPWM_SetChannelMode(base, channel, (uint8_t)MPWM_CENTER_PWM_MODE);
            state->mpwmChMode[channel] = MPWM_CENTER_PWM_MODE;
        }
    }

    return status;
}

/*!
 * @brief Deinitialize MPWM pwm mode
 */
status_t MPWM_DRV_DeinitPwm(uint32_t instance, const mpwm_pwm_config_t *config)
{
    DEV_ASSERT(instance < MPWM_INSTANCE_COUNT);
    DEV_ASSERT(config != NULL);

    MPWM_Type *base = g_mpwmBase[instance];
    uint8_t index;
    uint8_t channel;
    uint16_t channelSet = 0U;

    for (index = 0; index < config->channelNum; index++)
    {
        channel = config->pwmChannelConfig[index].hwChannelId;

        channelSet |= ((uint16_t)0x1U << channel);
    }

    /* Disable counters of configured channels */
    MPWM_DisableCounter(base, channelSet);

    /* Clear output initial value of all channels */
    MPWM_SetOutputInit(base, 0U);

    /* Clear polarity of all channels */
    MPWM_SetPolarity(base, 0);

    for (index = 0; index < config->channelNum; index++)
    {
        channel = config->pwmChannelConfig[index].hwChannelId;

        MPWM_SetClkPrs(base, channel, 0);

        /* Set channel as pwm mode */
        MPWM_SetChannelMode(base, channel, (uint8_t)MPWM_PWM_MODE);

        /* Set reload source */
        MPWM_SetReloadSrc(base, channel, (uint8_t)MPWM_RELOAD_ON_PERIOD);

        /* Set hardware trigger enable or not */
        MPWM_SetHwTrigEnState(base, channel, false);

        /* Set debug mode disable or not */
        MPWM_SetDebugMode(base, channel, false);

        /* Set period output trigger enable or not */
        MPWM_SetPerOutputTrigEnState(base, channel, false);

        /* Set compare output trigger enable or not */
        MPWM_SetCmpOutputTrigEnState(base, channel, false);

        /* Clear channel period ticks */
        MPWM_SetChannelPeriod(base, channel, 0);

        /* Clear channel compare ticks */
        MPWM_SetChannelCompare(base, channel, 0);
    }

    return STATUS_SUCCESS;
}

/*!
 * @brief Update the period of pwm
 */
status_t MPWM_DRV_UpdateChannelPeriod(uint32_t instance,
                                      uint8_t channel,
                                      mpwm_period_unit_t unit,
                                      uint32_t frequency)
{
    DEV_ASSERT(instance < MPWM_INSTANCE_COUNT);
    DEV_ASSERT(channel < MPWM_CH_COUNT);

    status_t status = STATUS_SUCCESS;
    mpwm_state_t *state = g_mpwmState[instance];
    MPWM_Type *base = g_mpwmBase[instance];
    uint32_t periodTicks;

    if (unit == MPWM_PERIOD_IN_HZ)
    {
        /* Convert frequency in Hz to ticks */
        periodTicks = MPWM_DRV_ConvertFreqToPeriodTicks(instance, channel, frequency);
    }
    else
    {
        periodTicks = frequency;
    }

    /* If period ticks overflow */
    if (periodTicks > 0xFFFFU)
    {
        status = STATUS_InvalidArgument;
    }
    else
    {
        state->mpwmChPeriodTicks[channel] = (uint16_t)periodTicks;
    
        status = MPWM_DRV_UpdateChannelDutyCycle(instance, channel, state->mpwmChDutyCycle[channel]);
    
        /* Set channel period */
        MPWM_SetChannelPeriod(base, channel, periodTicks);
    }

    return status;
}

/*
 * @brief Calculate the compare value for channel
 */
uint16_t MPWM_DRV_CalculateCompareValue(uint32_t instance, uint8_t channel, uint16_t period, uint16_t dutyCycle)
{
    uint16_t cmp = 0;
    mpwm_state_t *state = g_mpwmState[instance];
    mpwm_ch_pwm_align_mode_t align;
    mpwm_pwm_update_unit_t unit;

    align = state->mpwmChAlignMode[channel];
    unit = state->mpwmChUpdateUnit[channel];

    switch (align)
    {
        case MPWM_PWM_RIGHT_ALIGN:
            if (unit == MPWM_UPDATE_IN_DUTY_CYCLE)
            {
                cmp = (period * (0x8000U - dutyCycle)) >> MPWM_DUTY_TO_TICKS_SHIFT;
            }
            else /* MPWM_UPDATE_IN_TICKS */
            {
                cmp = period - dutyCycle;
            }
            break;
        case MPWM_PWM_LEFT_ALIGN:
            if (unit == MPWM_UPDATE_IN_DUTY_CYCLE)
            {
                cmp = (period * dutyCycle) >> MPWM_DUTY_TO_TICKS_SHIFT;
            }
            else /* MPWM_UPDATE_IN_TICKS */
            {
                cmp = dutyCycle;
            }
            break;
        case MPWM_PWM_CENTER_H_ALIGN:
            if (unit == MPWM_UPDATE_IN_DUTY_CYCLE)
            {
                cmp = (period * (0x4000U - (dutyCycle >> 1U))) >> MPWM_DUTY_TO_TICKS_SHIFT;
            }
            else /* MPWM_UPDATE_IN_TICKS */
            {
                cmp = (period - dutyCycle) >> 1U;
            }
            break;
        case MPWM_PWM_CENTER_L_ALIGN:
            if (unit == MPWM_UPDATE_IN_DUTY_CYCLE)
            {
                if (dutyCycle == 0x8000U)
                {
                    cmp = 0U;
                }
                else
                {
                    cmp = (period * (0x4000U + ((0x8000U - dutyCycle) >> 1U))) >> MPWM_DUTY_TO_TICKS_SHIFT;
                }
            }
            else /* MPWM_UPDATE_IN_TICKS */
            {
                if (dutyCycle == period)
                {
                    cmp = 0U;
                }
                else
                {
                    cmp = (period + (period - dutyCycle)) >> 1U;
                }
            }
            break;
        default:
            cmp = 0U;
            break;
    }

    return cmp;
}

/*!
 * @brief Update the duty cycle of pwm
 */
status_t MPWM_DRV_UpdateChannelDutyCycle(uint32_t instance,
                                         uint8_t channel,
                                         uint16_t dutyCycle)
{
    DEV_ASSERT(instance < MPWM_INSTANCE_COUNT);
    DEV_ASSERT(channel < MPWM_CH_COUNT);

    status_t status = STATUS_SUCCESS;
    MPWM_Type *base = g_mpwmBase[instance];
    mpwm_state_t *state = g_mpwmState[instance];
    uint16_t edge = 0;
    uint16_t period;

    /* Get channel period ticks */
    period = state->mpwmChPeriodTicks[channel];

    if ((dutyCycle <= MPWM_MAX_DUTY_CYCLE) || (dutyCycle <= period))
    {
        edge = MPWM_DRV_CalculateCompareValue(instance, channel, period, dutyCycle);

        /* Clear LDOK */
        MPWM_ClearLdok(base, channel);

        /* Set channel compare */
        MPWM_SetChannelCompare(base, channel, edge);

        state->mpwmChDutyCycle[channel] = dutyCycle;
    }
    else
    {
        status = STATUS_ERROR;
    }

    return status;
}

/*!
 * @brief Update the frequency and duty cycle of pwm
 */
status_t MPWM_DRV_UpdateChannelPeriodAndDutyCycle(uint32_t instance,
                                                  uint8_t channel,
                                                  mpwm_period_unit_t unit,
                                                  uint32_t frequency,
                                                  uint16_t dutyCycle)
{
    DEV_ASSERT(instance < MPWM_INSTANCE_COUNT);
    DEV_ASSERT(channel < MPWM_CH_COUNT);

    status_t status = STATUS_SUCCESS;
    mpwm_state_t *state = g_mpwmState[instance];
    MPWM_Type *base = g_mpwmBase[instance];
    uint32_t period = 0;
    uint16_t edge = 0;

    if (unit == MPWM_PERIOD_IN_HZ)
    {
        /* Convert frequency in Hz to ticks */
        period = MPWM_DRV_ConvertFreqToPeriodTicks(instance, channel, frequency);
    }
    else
    {
        period = frequency;
    }
    
    /* If period ticks overflow */
    if (period > 0xFFFFU)
    {
        status = STATUS_InvalidArgument;
    }
    else
    {
        state->mpwmChPeriodTicks[channel] = (uint16_t)period;
    
        if (dutyCycle <= MPWM_MAX_DUTY_CYCLE)
        {
            edge = MPWM_DRV_CalculateCompareValue(instance, channel, (uint16_t)period, dutyCycle);
    
            /* Clear LDOK */
            MPWM_ClearLdok(base, channel);
    
            /* Set channel period */
            MPWM_SetChannelPeriod(base, channel, period);
    
            /* Set channel compare */
            MPWM_SetChannelCompare(base, channel, edge);
    
            state->mpwmChDutyCycle[channel] = dutyCycle;
        }
        else
        {
            status = STATUS_ERROR;
        }
    }

    return status;
}

/*******************************************************************************
* EOF
*******************************************************************************/
