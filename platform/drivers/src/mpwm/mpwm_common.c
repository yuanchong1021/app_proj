/*
 * Copyright 2020-2025 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file mpwm_common.c
 * @version 1.4.0
 */

#include "mpwm_common.h"

/*******************************************************************************
 * Global Variable
 ******************************************************************************/

MPWM_Type *g_mpwmBase[MPWM_INSTANCE_COUNT] = MPWM_BASE_PTRS;
IRQn_Type g_mpwmChnIrqId[MPWM_INSTANCE_COUNT][MPWM_CH_COUNT] = MPWM_CHN_IRQS;
IRQn_Type g_mpwmOvfIrqId[MPWM_INSTANCE_COUNT][MPWM_CH_COUNT] = MPWM_OVF_IRQS;
mpwm_state_t *g_mpwmState[MPWM_INSTANCE_COUNT];
static clock_names_t g_mpwmIpcClkId[MPWM_INSTANCE_COUNT] = MPWM_IPC_CLK;
static clock_names_t g_mpwmClk[MPWM_INSTANCE_COUNT] = MPWM_CLK;

/*******************************************************************************
 * Declaration
 ******************************************************************************/

static void MPWM_Reset(uint32_t instance);

/*******************************************************************************
 * API Definition
 ******************************************************************************/

/*!
 * @brief Reset all registers of MPWM
 */
static void MPWM_Reset(uint32_t instance)
{
    DEV_ASSERT(instance < MPWM_INSTANCE_COUNT);
    CLOCK_DRV_ResetModule(g_mpwmIpcClkId[instance]);
}

/*!
 * @brief Initialize registers of MPWM and Configure MPWM state
 */
status_t MPWM_DRV_Init(uint32_t instance, mpwm_state_t *state)
{
    DEV_ASSERT(instance < MPWM_INSTANCE_COUNT);
    status_t status = STATUS_SUCCESS;

    /* Check if specified instance is initialized */
    if (g_mpwmState[instance] != NULL)
    {
        status = STATUS_ERROR;
    }
    else
    {
        /* Reset MPWM all registers */
        MPWM_Reset(instance);

        for (uint8_t i = 0; i < MPWM_CH_COUNT; i++)
        {
            state->mpwmChMode[i] = MPWM_COMMON_TIMER_MODE;
            state->mpwmChFreq[i] = 0U;
            state->mpwmChAlignMode[i] = MPWM_PWM_RIGHT_ALIGN;
            state->mpwmChPeriodTicks[i] = 0U;
            state->mpwmChDutyCycle[i] = 0U;
            state->mpwmChUpdateUnit[i] = MPWM_UPDATE_IN_DUTY_CYCLE;
        }

        g_mpwmState[instance] = state;

        /* Bus clock by default */
        status = MPWM_DRV_SetClkSrc(instance, MPWM_BUS_CLOCK);
    }

    return status;
}

/*!
 * @brief Reset registers of MPWM
 */
status_t MPWM_DRV_Deinit(uint32_t instance)
{
    DEV_ASSERT(instance < MPWM_INSTANCE_COUNT);

    MPWM_Reset(instance);

    g_mpwmState[instance] = NULL;

    return STATUS_SUCCESS;
}

/*!
 * @brief Set MPWM clock source
 */
status_t MPWM_DRV_SetClkSrc(uint32_t instance, mpwm_clk_src_t clkSrc)
{
    DEV_ASSERT(instance < MPWM_INSTANCE_COUNT);
    status_t status = STATUS_SUCCESS;
    mpwm_state_t *state = g_mpwmState[instance];

#if defined(FEATURE_MPWM_HAS_IPC_CLOCK_SOURCE) && (FEATURE_MPWM_HAS_IPC_CLOCK_SOURCE == 1)
    MPWM_Type *base = g_mpwmBase[instance];
    base->CTRL = (base->CTRL & ~MPWM_CTRL_CLKSRC_MASK) | ((uint32_t)clkSrc << MPWM_CTRL_CLKSRC_SHIFT);
#endif /* FEATURE_MPWM_HAS_IPC_CLOCK_SOURCE */

    state->mpwmClkSrc = clkSrc;

    /* Configure state structure */
    state->mpwmClkSrcFreq = MPWM_DRV_GetFrequency(instance);

    if (MPWM_BUS_CLOCK != state->mpwmClkSrc)
    {
        uint32_t fastBusClkFreq = 0U;
        (void)CLOCK_DRV_GetFreq(FAST_BUS_CLK, &fastBusClkFreq);
        /* Functional clock frequency should be less than or equal to FAST_BUS_CLK/2 */
        if (state->mpwmClkSrcFreq > (fastBusClkFreq / 2U))
        {
            status = STATUS_MPWM_ERROR_FUNC_CLK_FREQ;
        }
    }

    return status;
}

/*!
 * @brief Get MPWM clock source
 */
mpwm_clk_src_t MPWM_DRV_GetClkSrc(uint32_t instance)
{
    DEV_ASSERT(instance < MPWM_INSTANCE_COUNT);
    mpwm_clk_src_t clkSrc;

#if defined(FEATURE_MPWM_HAS_IPC_CLOCK_SOURCE) && (FEATURE_MPWM_HAS_IPC_CLOCK_SOURCE == 1)
    MPWM_Type *base = g_mpwmBase[instance];
    uint32_t tmp = (base->CTRL & MPWM_CTRL_CLKSRC_MASK) >> MPWM_CTRL_CLKSRC_SHIFT;
    clkSrc = (mpwm_clk_src_t)tmp;
#else
    (void)instance;
    clkSrc = MPWM_BUS_CLOCK;
#endif /* FEATURE_MPWM_HAS_IPC_CLOCK_SOURCE */

    return clkSrc;
}

/*!
 * @brief Enable channels' counter together
 */
status_t MPWM_DRV_EnableCounter(uint32_t instance, uint16_t channelSet)
{
    DEV_ASSERT(instance < MPWM_INSTANCE_COUNT);

    MPWM_Type *base = g_mpwmBase[instance];

    MPWM_EnableCounter(base, channelSet);

    return STATUS_SUCCESS;
}

/*!
 * @brief Disable channels' counter together
 */
status_t MPWM_DRV_DisableCounter(uint32_t instance, uint16_t channelSet)
{
    DEV_ASSERT(instance < MPWM_INSTANCE_COUNT);

    MPWM_Type *base = g_mpwmBase[instance];

    MPWM_DisableCounter(base, channelSet);

    return STATUS_SUCCESS;
}

/*!
 * @brief Set reload source for channel
 */
status_t MPWM_DRV_SetReloadSrc(uint32_t instance,
                               uint8_t channel,
                               mpwm_ch_reload_src_t src)
{
    DEV_ASSERT(instance < MPWM_INSTANCE_COUNT);
    DEV_ASSERT(channel < MPWM_CH_COUNT);

    MPWM_Type *base = g_mpwmBase[instance];

    MPWM_SetReloadSrc(base, channel, (uint8_t)src);

    return STATUS_SUCCESS;
}

/*!
 * @brief Get reload source of channel
 */
mpwm_ch_reload_src_t MPWM_DRV_GetReloadSrc(uint32_t instance, uint8_t channel)
{
    DEV_ASSERT(instance < MPWM_INSTANCE_COUNT);
    DEV_ASSERT(channel < MPWM_CH_COUNT);

    MPWM_Type *base = g_mpwmBase[instance];
    mpwm_ch_reload_src_t reloadSrc;

    reloadSrc = (mpwm_ch_reload_src_t)MPWM_GetReloadSrc(base, channel);

    return reloadSrc;
}

/*!
 * @brief Set output initial value for channel
 */
status_t MPWM_DRV_SetChannelOutputInit(uint32_t instance,
                                       uint8_t channel,
                                       mpwm_ch_output_init_t initVal)
{
    DEV_ASSERT(instance < MPWM_INSTANCE_COUNT);
    DEV_ASSERT(channel < MPWM_CH_COUNT);

    MPWM_Type *base = g_mpwmBase[instance];

    MPWM_SetChannelOutputInit(base, channel, (uint8_t)initVal);

    return STATUS_SUCCESS;
}

/*!
 * @brief Set output initial value for channels together
 */
status_t MPWM_DRV_SetOutputInit(uint32_t instance, uint16_t initValSet)
{
    DEV_ASSERT(instance < MPWM_INSTANCE_COUNT);

    MPWM_Type *base = g_mpwmBase[instance];

    MPWM_SetOutputInit(base, initValSet);

    return STATUS_SUCCESS;
}

/*!
 * @brief Get the set of output initial value of all channels
 */
uint16_t MPWM_DRV_GetOutputInit(uint32_t instance)
{
    DEV_ASSERT(instance < MPWM_INSTANCE_COUNT);

    MPWM_Type *base = g_mpwmBase[instance];
    uint16_t initValSet;

    initValSet = (uint16_t)MPWM_GetOutputInit(base);

    return initValSet;
}

/*!
 * @brief Set polarity for channel
 */
status_t MPWM_DRV_SetChannelPolarity(uint32_t instance,
                                     uint8_t channel,
                                     mpwm_ch_polarity_t pol)
{
    DEV_ASSERT(instance < MPWM_INSTANCE_COUNT);
    DEV_ASSERT(channel < MPWM_CH_COUNT);

    MPWM_Type *base = g_mpwmBase[instance];

    MPWM_SetChannelPolarity(base, channel, (uint8_t)pol);

    return STATUS_SUCCESS;
}

/*!
 * @brief Set polarity for channels together
 */
status_t MPWM_DRV_SetPolarity(uint32_t instance, uint16_t polSet)
{
    DEV_ASSERT(instance < MPWM_INSTANCE_COUNT);

    MPWM_Type *base = g_mpwmBase[instance];

    MPWM_SetPolarity(base, polSet);

    return STATUS_SUCCESS;
}

/*!
 * @brief Get the set of polarity value of all channels
 */
uint16_t MPWM_DRV_GetPolarity(uint32_t instance)
{
    DEV_ASSERT(instance < MPWM_INSTANCE_COUNT);

    MPWM_Type *base = g_mpwmBase[instance];
    uint16_t polSet;

    polSet = (uint16_t)MPWM_GetPolarity(base);

    return polSet;

}

/*!
 * @brief Enable DMA for channel
 */
status_t MPWM_DRV_EnableDma(uint32_t instance, uint8_t channel)
{
    DEV_ASSERT(instance < MPWM_INSTANCE_COUNT);

    MPWM_Type *base = g_mpwmBase[instance];
    MPWM_SetDmaEnState(base, channel, true);

    return STATUS_SUCCESS;
}

/*!
 * @brief Set software trigger for channel
 */
status_t MPWM_DRV_SetChannelSoftwareTrigger(uint32_t instance, uint8_t channel)
{
    DEV_ASSERT(instance < MPWM_INSTANCE_COUNT);
    DEV_ASSERT(channel < MPWM_CH_COUNT);

    MPWM_Type *base = g_mpwmBase[instance];

    MPWM_SetChannelSwTrig(base, channel);

    return STATUS_SUCCESS;
}

/*!
 * @brief Set software trigger together
 */
status_t MPWM_DRV_SetSoftwareTrigger(uint32_t instance, uint16_t channelSet)
{
    DEV_ASSERT(instance < MPWM_INSTANCE_COUNT);

    MPWM_Type *base = g_mpwmBase[instance];

    MPWM_SetSwTrig(base, channelSet);

    return STATUS_SUCCESS;
}

/*!
 * @brief Set channel period ticks
 */
status_t MPWM_DRV_SetChannelPeriod(uint32_t instance, uint8_t channel, uint16_t value)
{
    DEV_ASSERT(instance < MPWM_INSTANCE_COUNT);

    MPWM_Type *base = g_mpwmBase[instance];

    MPWM_SetChannelPeriod(base, channel, value);

    return STATUS_SUCCESS;
}

/*!
 * @brief Set channel compare ticks
 */
status_t MPWM_DRV_SetChannelCompare(uint32_t instance, uint8_t channel, uint16_t value)
{
    DEV_ASSERT(instance < MPWM_INSTANCE_COUNT);

    MPWM_Type *base = g_mpwmBase[instance];

    MPWM_SetChannelCompare(base, channel, value);

    return STATUS_SUCCESS;
}

/*!
 * @brief Get frequency of MPWM
 */
uint32_t MPWM_DRV_GetFrequency(uint32_t instance)
{
    DEV_ASSERT(instance < MPWM_INSTANCE_COUNT);

    uint32_t clkFreq;

#if defined(FEATURE_MPWM_HAS_IPC_CLOCK_SOURCE) && (FEATURE_MPWM_HAS_IPC_CLOCK_SOURCE == 1)
    if (MPWM_FUNC_CLOCK == MPWM_DRV_GetClkSrc(instance))
    {
        (void)CLOCK_SYS_GetFreq(g_mpwmIpcClkId[instance], &clkFreq);
    }
    else
#endif
    {
        (void)CLOCK_SYS_GetFreq(g_mpwmClk[instance], &clkFreq);
    }

    return clkFreq;
}

/*!
 * @brief Convert frequency to period ticks
 */
uint32_t MPWM_DRV_ConvertFreqToPeriodTicks(uint32_t instance,
                                           uint8_t channel,
                                           uint32_t freq)
{
    DEV_ASSERT(instance < MPWM_INSTANCE_COUNT);
    DEV_ASSERT(freq != 0);

    uint32_t mpwmChFreq = 0U;
    mpwm_state_t *state = g_mpwmState[instance];
    mpwmChFreq = state->mpwmChFreq[channel];

    return (mpwmChFreq / freq);
}

/*!
 * @brief Get timer overflow interrupt flag of channel
 */
bool MPWM_DRV_GetChannelTofIntFlag(uint32_t instance, uint8_t channel)
{
    DEV_ASSERT(instance < MPWM_INSTANCE_COUNT);
    DEV_ASSERT(channel < MPWM_CH_COUNT);

    MPWM_Type *base = g_mpwmBase[instance];

    return ((base->STS0 >> channel) & 0x1U) != 0U;
}

/*!
 * @brief Clear timer overflow interrupt flag for channel
 */
void MPWM_DRV_ClearChannelTofIntFlag(uint32_t instance, uint8_t channel)
{
    DEV_ASSERT(instance < MPWM_INSTANCE_COUNT);
    DEV_ASSERT(channel < MPWM_CH_COUNT);

    MPWM_Type *base = g_mpwmBase[instance];

    base->STS0 = ((uint32_t)0x1U << channel);
}

/*!
 * @brief Get interrupt flag for channel event
 */
bool MPWM_DRV_GetChannelIntFlag(uint32_t instance, uint8_t channel)
{
    DEV_ASSERT(instance < MPWM_INSTANCE_COUNT);
    DEV_ASSERT(channel < MPWM_CH_COUNT);

    MPWM_Type *base = g_mpwmBase[instance];

    return ((base->STS1 >> channel) & 0x1U) != 0U;
}

/*!
 * @brief Clear interrupt flag for channel event
 */
void MPWM_DRV_ClearChannelIntFlag(uint32_t instance, uint8_t channel)
{
    DEV_ASSERT(instance < MPWM_INSTANCE_COUNT);
    DEV_ASSERT(channel < MPWM_CH_COUNT);

    MPWM_Type *base = g_mpwmBase[instance];

    base->STS1 = ((uint32_t)0x1U << channel);
}

/*!
 * @brief Clear load ok for channel
 */
void MPWM_DRV_ClearChannelLdok(uint32_t instance, uint8_t channel)
{
    DEV_ASSERT(instance < MPWM_INSTANCE_COUNT);
    DEV_ASSERT(channel < MPWM_CH_COUNT);

    MPWM_Type *base = g_mpwmBase[instance];

    MPWM_ClearLdok(base, channel);
}

/*!
 * @brief Clear load ok together
 */
void MPWM_DRV_ClearLdok(uint32_t instance, uint16_t channelSet)
{
    DEV_ASSERT(instance < MPWM_INSTANCE_COUNT);

    MPWM_Type *base = g_mpwmBase[instance];
    uint8_t index;
    uint16_t val;

    for (index = 0U; index < MPWM_CH_COUNT; index++)
    {
        val = (channelSet >> index) & 0x1U;
        if (val != 0U)
        {
            MPWM_ClearLdok(base, index);
        }
    }
}

/*!
 * @brief Set loading ok for single channel
 */
void MPWM_DRV_SetChannelLdok(uint32_t instance, uint8_t channel)
{
    DEV_ASSERT(instance < MPWM_INSTANCE_COUNT);
    DEV_ASSERT(channel < MPWM_CH_COUNT);

    MPWM_Type *base = g_mpwmBase[instance];

    MPWM_SetLdok(base, channel);
}

/*!
 * @brief Set loading ok to indicate PERIOD or CMP register is \n
 *        ready for loading
 */
void MPWM_DRV_SetLdok(uint32_t instance, uint16_t channelSet)
{
    DEV_ASSERT(instance < MPWM_INSTANCE_COUNT);

    MPWM_Type *base = g_mpwmBase[instance];

#if FEATURE_MPWM_HAS_SIMULTANEOUS_UPDATE
    base->LDOK = channelSet;
#else
    for (uint8_t index = 0; index < MPWM_CH_COUNT; index++)
    {
        uint16_t val = (channelSet >> index) & 0x1U;
        if (val != 0U)
        {
            MPWM_SetLdok(base, index);
        }
    }
#endif
}

/*!
 * @brief Read MPWM channel capture count value
 */
uint32_t MPWM_DRV_ReadCaptureCount(uint32_t instance, uint8_t channel)
{
    DEV_ASSERT(instance < MPWM_INSTANCE_COUNT);
    DEV_ASSERT(channel < MPWM_CH_COUNT);

    MPWM_Type *base = g_mpwmBase[instance];

    return (uint32_t)(base->CH[channel].CAP);
}

/*!
 * @brief Read MPWM channel current count value(pulse number)
 */
uint16_t MPWM_DRV_ReadCurrentCount(uint32_t instance, uint8_t channel)
{
    DEV_ASSERT(instance < MPWM_INSTANCE_COUNT);
    DEV_ASSERT(channel < MPWM_CH_COUNT);

    MPWM_Type *base = g_mpwmBase[instance];

    return (uint16_t)(base->CH[channel].CNT);
}

/*******************************************************************************
* EOF
*******************************************************************************/
