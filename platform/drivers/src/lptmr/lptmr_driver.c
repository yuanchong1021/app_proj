/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file lptmr_driver.c
 * @version 1.4.0
 */

#include "lptmr_driver.h"
#include "lptmr_hw_access.h"
#include "clock_manager.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Takes into consideration that LPTMR compare events take place
 * when "the CNR equals the value of the CMR and increments" */
#define lpTMR_MAX_CMR_NTICKS (lpTMR_CMP_CMP_MASK + 1u)
#define lpTMR_MAX_PRESCALER  (1u << lpTMR_PRS_PRES_WIDTH)

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*! @cond DRIVER_INTERNAL_USE_ONLY */

/* Table of base addresses for LPTMR instances */
static lpTMR_Type* const g_lptmrBase[lpTMR_INSTANCE_COUNT] = lpTMR_BASE_PTRS;
/* Table to save LPTMR clock names as defined in clock manager. */
#if FEATURE_lpTMR_CLKSRC_SUPPORT_IPC
static const clock_names_t s_lptmrClkNames[lpTMR_INSTANCE_COUNT] = lpTMR_CLOCK_NAMES;
#endif /* FEATURE_lpTMR_CLKSRC_SUPPORT_IPC */

/* lpTMR current clock frequency */
static uint32_t s_lptmrClkFreq;

/*******************************************************************************
 * Private Functions
 ******************************************************************************/
static inline uint8_t lptmr_cfg2p(
    const lptmr_prescaler_t prescval,
    const bool bypass
    );

static inline uint64_t lptmr_us2nn(
    const uint32_t clkfreq,
    const uint32_t us
    );

static inline uint64_t lptmr_compute_nticks(
    uint64_t nn,
    uint8_t p
    );

static inline bool nticks2compare_ticks(
    uint64_t nticks,
    uint16_t* ticks
    );

static bool lptmr_Ticks2Us(
    const uint32_t clkfreq,
    const lptmr_prescaler_t pval,
    const bool bypass,
    const uint16_t ticks,
    uint32_t* const us
    );

static bool lptmr_ChooseClkConfig(
    const uint32_t clkfreq,
    const uint32_t us,
    lptmr_prescaler_t* const prescval,
    bool* const bypass,
    uint16_t* const ticks
    );

/*TIMER MODE CONFIGURATION******************************************************
 *
 * Timer Mode - Prescaler settings calculations
 * --------------------------------------------
 *
 * Timer Mode configuration takes a period (timeout) value expressed in
 * micro-seconds. To convert this to LPTMR prescaler (and compare value)
 * settings, the closest match must be found.
 * For best precision, the lowest prescaler that allows the corresponding
 * compare value to fit in the 16-bit register will be chosen.
 *
 * Algorithm for choosing prescaler and compare values:
 * =============================================================================
 * In: tper_us (period in microseconds), fclk (input clock frequency in Hertz)
 * Out: nticks (timer ticks), p (prescaler coefficient, 2^p = prescaler value)
 * ---
 * 1) Compute nn = tper_us * fclk / 1000000
 * 2) for p = 0..16
 *  2.1) nticks = nn / 2^p
 *  2.2) if nticks < 0x10000
 *      2.2.1) STOP, found nticks and p
 * 3) nticks = 0xFFFF, p = 16
 * =============================================================================
 *
 * A few names used throughout the static functions affecting Timer mode:
 *  nn - total number of timer ticks (undivided, unprescaled) that is necessary
 *      for a particular timeout.
 *      nn = (tper_us * fclk) / 1000000 = nticks * npresc
 *
 *  tper_us - a period (or timeout) expressed in microsecond units. In most
 *      functions will be denoted as 'us' for microseconds.
 *
 *  nticks - number of timer ticks that is necessary for a particular timeout,
 *      after prescaling
 *
 *  npresc - prescaler value (1, 2, 4 ... 65536)
 *
 *  p - prescaler coefficient, 2^p = npresc
 *
 *  fclk - input clock frequency, in Hertz. In most function will be denoted as
 *      'clkfreq'.
 *END**************************************************************************/

/*FUNCTION**********************************************************************
 *
 * Function Name : lptmr_cfg2p
 * Description   : Transform prescaler settings (bypass on/off, prescaler value)
 *  to prescaler coefficient value (2's power), p.
 * Return: the value of p.
 *END**************************************************************************/
static inline uint8_t lptmr_cfg2p(
    const lptmr_prescaler_t prescval,
    const bool bypass
    )
{
    uint8_t p = 0u;

    if (!bypass)
    {
        p = (uint8_t)(((uint8_t)prescval) + 1u);
    }

    return p;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : lptmr_us2nn
 * Description   : Transform microseconds to undivided (unprescaled) timer units,
 * nn.
 * Return: the value of nn.
 *END**************************************************************************/
static inline uint64_t lptmr_us2nn(
    const uint32_t clkfreq,
    const uint32_t us
    )
{
    /* Approximate the timeout in undivided (unprescaled) timer ticks.
        - us is the timeout in microseconds (1/10^6 seconds)
        - clkfreq is the frequency in Hertz
        Operation:
        nn = (us/1000000) * clkfreq
        In C:
        For better precision, first to the multiplication (us * clkfreq)
        To overcome the truncation of the div operator in C, add half of the
        denominator before the division. Hence:
        nn = (us * clkfreq + 500000) / 1000000
    */
    /* There is no risk of overflow since us is 32-bit wide and clkfreq can be
       a theoretical maximum of ~100 MHz (platform maximum), which is over the
       maximum input of the LPTMR anyway
     */
    uint64_t nn = (uint64_t)( (uint64_t)us * (uint64_t)clkfreq );
    nn = (nn + 500000u) / 1000000u;
    return nn;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : lptmr_compute_nticks
 * Description   : Compute total number of divided (prescaled) timer ticks,
 * nticks.
 * Return: the value of nticks.
 *END**************************************************************************/
static inline uint64_t lptmr_compute_nticks(
    uint64_t nn,
    uint8_t p
    )
{
    uint64_t npresc = (uint64_t) 1u << p;
    DEV_ASSERT(npresc != 0u);

    /* integer division */
    uint64_t nticks = ((nn + (npresc >> 1u)) / npresc);

    return nticks;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : nticks2compare_ticks
 * Description   : Transform the value of divided (prescaled) timer ticks, nticks
 * to a 16-bit value to be written to the hardware register. Cap or underflow
 * cause an error.
 * Return: the success state.
 *  - true: no underflow or overflow detected
 *  - false: value written was capped, underflow or overflow detected
 *
 *END**************************************************************************/
static inline bool nticks2compare_ticks(
    uint64_t nticks,
    uint16_t* ticks
    )
{
    bool success = true;

    /* if nticks fits, write the value to ticks */
    if (nticks <= lpTMR_MAX_CMR_NTICKS)
    {
        if (nticks == 0u)
        {
            /* timeout period (us) too low for prescaler settings */
            *ticks = 0u;
            success = false;
        }
        else{
            /* According to RM, the LPTMR compare events take place when "the CNR equals the value of the CMR and increments".
             * The additional increment is compensated here by decrementing the calculated compare value with 1, before being written to CMR. */
            *ticks = (uint16_t)(nticks - 1u);
        }
    }
    else {
        /* timeout period (us) too high for prescaler settings */
        *ticks = lpTMR_CMP_CMP_MASK;
        success = false;
    }

    return success;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : lptmr_Ticks2Us
 * Description   : Transform timer ticks to microseconds using the given
 * prescaler settings. Clock frequency must be valid (different from 0).
 * Possible return values:
 * - true: conversion success
 * - false: conversion failed, result did not fit in 32-bit.
 *
 *END**************************************************************************/
static bool lptmr_Ticks2Us(
    const uint32_t clkfreq,
    const lptmr_prescaler_t pval,
    const bool bypass,
    const uint16_t ticks,
    uint32_t* const us
    )
{
    bool success = true;
    uint8_t p = lptmr_cfg2p(pval, bypass);
    uint64_t nn = ( (uint64_t)ticks + 1u ) << p;
    uint64_t us_real = (nn * 1000000u) / (clkfreq);
    uint32_t us_local;

    if ( us_real <= (0xFFFFFFFFu) )
    {
        us_local = (uint32_t)us_real;
    }
    else
    {
        us_local = 0xFFFFFFFFu;
        success = false;
    }

    *us = us_local;
    return success;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : lptmr_ChooseClkConfig
 * Description   : Choose clocking configuration (prescaler value, timer ticks)
 * for the desired timeout period, given in microseconds. Input clock frequency,
 * clkfreq, must be greater than 0.
 * Possible return values:
 * - true: configuration found
 * - false: configuration mismatch, desired timeout period is too small or too
 * big for the clock settings.
 *
 *END**************************************************************************/
static bool lptmr_ChooseClkConfig(
    const uint32_t clkfreq,
    const uint32_t us,
    lptmr_prescaler_t* const prescval,
    bool* const bypass,
    uint16_t* const ticks
    )
{
    uint8_t p;
    uint64_t nticks = 0ULL;
    bool success;

    uint64_t nn = lptmr_us2nn(clkfreq, us);

    /* Find the lowest prescaler value that allows the compare value in 16-bits */
    for (p = 0u; p <= lpTMR_MAX_PRESCALER; p++)
    {
        nticks = lptmr_compute_nticks(nn, p);

        if (nticks <= lpTMR_MAX_CMR_NTICKS)
        {
            /* Search finished, value will fit in the 16-bit register */
            break;
        }
    }

    success = nticks2compare_ticks(nticks, ticks);

    /* Convert p to prescaler configuration */
    if (p == 0u)
    {
        /* Prescaler value of 1 */
        *bypass = true;
        *prescval = lpTMR_PRESCALE_2;
    }
    else{
        *bypass = false;
        p--; /* Decrement to match lptmr_prescaler_t.  */
        *prescval = (lptmr_prescaler_t) p;
    }

    return success;
}


/*******************************************************************************
 * Public Functions
 ******************************************************************************/


/*FUNCTION**********************************************************************
 *
 * Function Name : lpTMR_DRV_InitConfigStruct
 * Description   : Initialize a configuration structure with default values.
 *
 * Implements : lpTMR_DRV_InitConfigStruct_Activity
 *END**************************************************************************/
void lpTMR_DRV_InitConfigStruct(lptmr_config_t * const config)
{
    DEV_ASSERT(config != NULL);

    /* General parameters */
#if defined(lpTMR_DIE_DMAEN_MASK)
    config->dmaRequest      = false;
#endif /* lpTMR_DIE_DMAEN_MASK */
    config->interruptEnable = false;
    config->freeRun         = false;
    config->workMode        = lpTMR_WORKMODE_TIMER;

    /* Counter parameters */
    config->prescaler       = lpTMR_PRESCALE_2;
    config->bypassPrescaler = false;
    config->compareValue    = 0u;
    config->counterUnits    = lpTMR_COUNTER_UNITS_TICKS;

    /* Pulse Counter specific parameters */
    config->pinSelect       = lpTMR_PINSELECT_TMU;
    config->pinPolarity     = lpTMR_PINPOLARITY_RISING;
#if (defined(FEATURE_lpTMR_HAS_CLOCK_SELECTION) && FEATURE_lpTMR_HAS_CLOCK_SELECTION)
    config->clockSource     = (lptmr_clock_source_t) 1U;
#endif /* FEATURE_lpTMR_HAS_CLOCK_SELECTION */
}

/*FUNCTION**********************************************************************
 *
 * Function Name : lpTMR_DRV_Init
 * Description   : Initialize a LPTMR instance based on the input configuration
 * structure.
 *
 * When (counterUnits == lpTMR_COUNTER_UNITS_MICROSECONDS) the function will
 * automatically configure the timer for the input compareValue in microseconds.
 * The input parameters for 'prescaler' and 'bypassPrescaler' will be ignored
 * - their values will be adapted by the function, to best fit the input compareValue
 * (in microseconds) for the operating clock frequency.
 *
 * lpTMR_COUNTER_UNITS_MICROSECONDS may only be used for lpTMR_WORKMODE_TIMER mode.
 * Otherwise the function shall not convert 'compareValue' in ticks
 * and this is likely to cause erroneous behavior.
 *
 * When (counterUnits == lpTMR_COUNTER_UNITS_TICKS) the function will use the
 * 'prescaler' and 'bypassPrescaler' provided in the input configuration structure.
 *
 * When (counterUnits == lpTMR_COUNTER_UNITS_TICKS), 'compareValue' must be lower
 * than 0xFFFFu. Only the least significant 16bits of 'compareValue' will be used.
 * When (counterUnits == lpTMR_COUNTER_UNITS_MICROSECONDS), 'compareValue'
 * may take any 32bits unsigned value.
 *
 * Implements : lpTMR_DRV_Init_Activity
 *END**************************************************************************/
void lpTMR_DRV_Init(const uint32_t instance,
                    const lptmr_config_t * const config,
                    const bool startCounter)
{
    DEV_ASSERT(instance < lpTMR_INSTANCE_COUNT);
    DEV_ASSERT(config != NULL);

    lpTMR_Type* const base = g_lptmrBase[instance];

    lpTMR_DRV_SetConfig(instance, config);

    /* Start the counter if requested */
    if (startCounter)
    {
        lpTMR_Enable(base);
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : lpTMR_DRV_Deinit
 * Description   : De-initialize the LPTMR (stop the counter and reset all registers to default value).
 *
 * Implements : lpTMR_DRV_Deinit_Activity
 *END**************************************************************************/
void lpTMR_DRV_Deinit(const uint32_t instance)
{
    DEV_ASSERT(instance < lpTMR_INSTANCE_COUNT);

    lpTMR_Type* const base = g_lptmrBase[instance];
    lpTMR_Disable(base);

    lpTMR_Init(base);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : lpTMR_DRV_SetConfig
 * Description   : Configure a LPTMR instance based on the input configuration
 * structure.
 *
 * When (counterUnits == lpTMR_COUNTER_UNITS_MICROSECONDS) the function will
 * automatically configure the timer for the input compareValue in microseconds.
 * The input parameters for 'prescaler' and 'bypassPrescaler' will be ignored
 * - their values will be adapted by the function, to best fit the input compareValue
 * (in microseconds) for the operating clock frequency.
 *
 * lpTMR_COUNTER_UNITS_MICROSECONDS may only be used for lpTMR_WORKMODE_TIMER mode.
 * Otherwise the function shall not convert 'compareValue' in ticks
 * and this is likely to cause erroneous behavior.
 *
 * When (counterUnits == lpTMR_COUNTER_UNITS_TICKS) the function will use the
 * 'prescaler' and 'bypassPrescaler' provided in the input configuration structure.
 *
 * When (counterUnits == lpTMR_COUNTER_UNITS_TICKS), 'compareValue' must be lower
 * than 0xFFFFu. Only the least significant 16bits of 'compareValue' will be used.
 * When (counterUnits == lpTMR_COUNTER_UNITS_MICROSECONDS), 'compareValue'
 * may take any 32bits unsigned value.
 *
 * Implements : lpTMR_DRV_SetConfig_Activity
 *END**************************************************************************/
void lpTMR_DRV_SetConfig(const uint32_t instance,
                         const lptmr_config_t * const config)
{
    DEV_ASSERT(instance < lpTMR_INSTANCE_COUNT);
    DEV_ASSERT(config != NULL);

    lpTMR_Type* const base          = g_lptmrBase[instance];
    uint32_t configCmpValue         = config->compareValue;
    lptmr_workmode_t configWorkMode = config->workMode;
    uint16_t cmpValueTicks          = 0U;
    lptmr_prescaler_t prescVal      = config->prescaler;
    bool prescBypass                = config->bypassPrescaler;
    lptmr_counter_units_t configCounterUnits = config->counterUnits;
    uint32_t clkFreq;

    if(configWorkMode == lpTMR_WORKMODE_TIMER)
    {
#if (defined(FEATURE_lpTMR_HAS_CLOCK_SELECTION) && (FEATURE_lpTMR_HAS_CLOCK_SELECTION == 1U))
        /* Get the LPTMR clock as configured in the clock manager */
        switch (config->clockSource)
        {
        #if defined(FEATURE_lpTMR_CLKSRC_SUPPORT_FIRC) && (FEATURE_lpTMR_CLKSRC_SUPPORT_FIRC == 1U)
        case lpTMR_CLOCK_SOURCE_FIRC:
            (void)CLOCK_SYS_GetFreq(IPC_FIRC_CLK, &clkFreq);
            break;
        #endif /* FEATURE_lpTMR_CLKSRC_SUPPORT_FIRC */
        #if defined(FEATURE_lpTMR_CLKSRC_SUPPORT_IPC) && (FEATURE_lpTMR_CLKSRC_SUPPORT_IPC == 1U)
        case lpTMR_CLOCK_SOURCE_IPC:
            (void)CLOCK_SYS_GetFreq(s_lptmrClkNames[instance], &clkFreq);
            break;
        #endif /* FEATURE_lpTMR_CLKSRC_SUPPORT_IPC */
        #if defined(FEATURE_lpTMR_CLKSRC_SUPPORT_SIRC) && (FEATURE_lpTMR_CLKSRC_SUPPORT_SIRC == 1U)
        case lpTMR_CLOCK_SOURCE_SIRC:
            (void)CLOCK_SYS_GetFreq(IPC_SIRC_CLK, &clkFreq);
            break;
        #endif /* FEATURE_lpTMR_CLKSRC_SUPPORT_SIRC */
        #if defined(FEATURE_lpTMR_CLKSRC_SUPPORT_SIRC_DIV4) && (FEATURE_lpTMR_CLKSRC_SUPPORT_SIRC_DIV4 == 1U)
                case lpTMR_CLOCK_SOURCE_SIRC_DIV4:
            (void)CLOCK_SYS_GetFreq(IPC_SIRC_DIV4_CLK, &clkFreq);
            break;
        #endif /* FEATURE_lpTMR_CLKSRC_SUPPORT_SIRC */
        #if defined(FEATURE_lpTMR_CLKSRC_SUPPORT_SXOSC) && (FEATURE_lpTMR_CLKSRC_SUPPORT_SXOSC == 1U)
        case lpTMR_CLOCK_SOURCE_SXOSC:
            (void)CLOCK_SYS_GetFreq(IPC_SXOSC_CLK, &clkFreq);
            break;
        #endif /* FEATURE_lpTMR_CLKSRC_SUPPORT_SXOSC */
        #if defined(FEATURE_lpTMR_CLKSRC_SUPPORT_FXOSC) && (FEATURE_lpTMR_CLKSRC_SUPPORT_FXOSC == 1U)
        case lpTMR_CLOCK_SOURCE_FXOSC:
            (void)CLOCK_SYS_GetFreq(IPC_FXOSC_CLK, &clkFreq);
            break;
        #endif /* FEATURE_lpTMR_CLKSRC_SUPPORT_FXOSC */
        #if defined(FEATURE_lpTMR_CLKSRC_SUPPORT_OSC) && (FEATURE_lpTMR_CLKSRC_SUPPORT_OSC == 1U)
        case lpTMR_CLOCK_SOURCE_OSC:
            (void)CLOCK_SYS_GetFreq(IPC_OSC_CLK, &clkFreq);
            break;
        #endif /* FEATURE_lpTMR_CLKSRC_SUPPORT_LPO */
        #if defined(FEATURE_lpTMR_CLKSRC_SUPPORT_LPO) && (FEATURE_lpTMR_CLKSRC_SUPPORT_LPO == 1U)
        case lpTMR_CLOCK_SOURCE_LPO:
            (void)CLOCK_SYS_GetFreq(IPC_LPO_CLK, &clkFreq);
            break;
        #endif /* FEATURE_lpTMR_CLKSRC_SUPPORT_LPO */
        default:
            clkFreq = 0U;
            break;
        }
#elif (FEATURE_lpTMR_CLKSRC_SUPPORT_IPC)
        /* Get the lpTMR clock as configured in the clock manager */
        (void)CLOCK_SYS_GetFreq(s_lptmrClkNames[instance], &clkFreq);
#endif /* FEATURE_lpTMR_HAS_CLOCK_SELECTION */

        DEV_ASSERT(clkFreq != 0U); /* Clock frequency equal to '0', signals invalid value.  */
        s_lptmrClkFreq = clkFreq;

        if(configCounterUnits == lpTMR_COUNTER_UNITS_MICROSECONDS)
        {
            bool chooseClkConfigStatus;

            /* When workmode is set to Timer Mode and compare value is provided in microseconds,
             * then the input parameters for prescale value and prescaleBypass are ignored.
             * The prescaleValue, prescaleBypass and cmpValue in ticks, are calculated to best fit
             * the input configCmpValue (in us) for the current operating clk frequency.  */
            chooseClkConfigStatus = lptmr_ChooseClkConfig(clkFreq, configCmpValue, &prescVal, &prescBypass, &cmpValueTicks);
            DEV_ASSERT(chooseClkConfigStatus == true);
            (void) chooseClkConfigStatus;
        }
        else
        {
            DEV_ASSERT(configCounterUnits == lpTMR_COUNTER_UNITS_TICKS);
            DEV_ASSERT(configCmpValue <= lpTMR_CMP_CMP_MASK); /* Compare Value in Tick Units must fit in CMR. */

            cmpValueTicks = (uint16_t)(configCmpValue & lpTMR_CMP_CMP_MASK);
        }
    }
    else
    {
        /* If configWorkMode is not lpTMR_WORKMODE_TIMER, then it must be lpTMR_WORKMODE_PULSECOUNTER. */
        DEV_ASSERT(configWorkMode == lpTMR_WORKMODE_PULSECOUNTER);

        /* Only lpTMR_COUNTER_UNITS_TICKS can be used when LPTMR is configured as Pulse Counter. */
        DEV_ASSERT(config->counterUnits == lpTMR_COUNTER_UNITS_TICKS);
        /* Glitch filter does not support lpTMR_PRESCALE_2. */
        DEV_ASSERT(prescBypass || (prescVal != lpTMR_PRESCALE_2));

        DEV_ASSERT(configCmpValue <= lpTMR_CMP_CMP_MASK); /* Compare Value in Tick Units must fit in CMR. */

        cmpValueTicks = (uint16_t)(configCmpValue & lpTMR_CMP_CMP_MASK);
    }

    /* Initialize and write configuration parameters. */
    lpTMR_Init(base);

#if defined(lpTMR_DIE_DMAEN_MASK)
    lpTMR_SetDmaRequest   (base, config->dmaRequest);
#endif /* lpTMR_DIE_DMAEN_MASK */
    lpTMR_SetInterrupt    (base, config->interruptEnable);
    lpTMR_SetFreeRunning  (base, config->freeRun);
    lpTMR_SetWorkMode     (base, configWorkMode);
    lpTMR_SetPrescaler    (base, prescVal);
    lpTMR_SetBypass       (base, prescBypass);
    lpTMR_SetCompareValue (base, cmpValueTicks);
    lpTMR_SetPinSelect    (base, config->pinSelect);
    lpTMR_SetPinPolarity  (base, config->pinPolarity);
#if defined(FEATURE_lpTMR_HAS_CLOCK_SELECTION) && (FEATURE_lpTMR_HAS_CLOCK_SELECTION)
    lpTMR_SetClockSource(base, config->clockSource);
#endif /* FEATURE_lpTMR_HAS_CLOCK_SELECTION */
}

/*FUNCTION**********************************************************************
 *
 * Function Name : lpTMR_DRV_GetConfig
 * Description   : Get the current configuration of the LPTMR instance.
 * Always returns compareValue in lpTMR_COUNTER_UNITS_TICKS.
 *
 * Implements : lpTMR_DRV_GetConfig_Activity
 *END**************************************************************************/
void lpTMR_DRV_GetConfig(const uint32_t instance,
                         lptmr_config_t * const config)
{
    DEV_ASSERT(instance < lpTMR_INSTANCE_COUNT);
    DEV_ASSERT(config != NULL);

    const lpTMR_Type* const base = g_lptmrBase[instance];

    /* Read current configuration */
#if defined(lpTMR_DIE_DMAEN_MASK)
    config->dmaRequest      = lpTMR_GetDmaRequest(base);
#endif /* lpTMR_DIE_DMAEN_MASK */
    config->interruptEnable = lpTMR_GetInterruptEnable(base);
    config->freeRun         = lpTMR_GetFreeRunning(base);
    config->workMode        = lpTMR_GetWorkMode(base);
    config->prescaler       = lpTMR_GetPrescaler(base);
    config->bypassPrescaler = lpTMR_GetBypass(base);
    config->compareValue    = lpTMR_GetCompareValue(base);
    config->counterUnits    = lpTMR_COUNTER_UNITS_TICKS;
    config->pinSelect       = lpTMR_GetPinSelect(base);
    config->pinPolarity     = lpTMR_GetPinPolarity(base);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : lpTMR_DRV_SetCompareValueByCount
 * Description   : Set the compare value in counter tick units, for a LPTMR instance.
 * Possible return values:
 * - STATUS_SUCCESS: completed successfully
 * - STATUS_ERROR: cannot reconfigure compare value (TCF not set)
 * - STATUS_TIMEOUT: compare value is smaller than current counter value
 *
 * Implements : lpTMR_DRV_SetCompareValueByCount_Activity
 *END**************************************************************************/
status_t lpTMR_DRV_SetCompareValueByCount(const uint32_t instance,
                                          const uint16_t compareValueByCount)
{
    DEV_ASSERT(instance < lpTMR_INSTANCE_COUNT);

    lpTMR_Type* const base  = g_lptmrBase[instance];
    status_t statusCode     = STATUS_SUCCESS;

    bool timerEnabled = lpTMR_GetEnable(base);
    bool compareFlag  = lpTMR_GetCompareFlag(base);

    uint16_t counterVal;

    /* Check if a valid clock is selected for the timer/glitch filter */
#if (defined (DEV_ERROR_DETECT) || defined (CUSTOM_DEVASSERT))
    bool bypass = lpTMR_GetBypass(base);
    lptmr_workmode_t workMode = lpTMR_GetWorkMode(base);
    (void) bypass;
    (void) workMode;
#endif /* (defined (DEV_ERROR_DETECT) || defined (CUSTOM_DEVASSERT)) */


    /* The compare value can only be written if counter is disabled or the compare flag is set. */
    if (timerEnabled && !compareFlag)
    {
        statusCode = STATUS_ERROR;
    }
    else
    {
        /* Check if new value is below the current counter value */
        lpTMR_SetCompareValue(base, compareValueByCount);
        counterVal = lpTMR_GetCounterValue(base);
        if (counterVal >= compareValueByCount)
        {
            statusCode = STATUS_TIMEOUT;
        }
    }

    return statusCode;
}


/*FUNCTION**********************************************************************
 *
 * Function Name : lpTMR_DRV_GetCompareValueByCount
 * Description   : Get the compare value of timer in ticks units.
 *
 * Implements : lpTMR_DRV_GetCompareValueByCount_Activity
 *END**************************************************************************/
void lpTMR_DRV_GetCompareValueByCount(const uint32_t instance,
                                      uint16_t * const compareValueByCount)
{
    DEV_ASSERT(instance < lpTMR_INSTANCE_COUNT);

    const lpTMR_Type* const base = g_lptmrBase[instance];

    *compareValueByCount = lpTMR_GetCompareValue(base);
}


/*FUNCTION**********************************************************************
 *
 * Function Name : LPTMR_DRV_SetCompareValueUs
 * Description   : Set the compare value for Timer Mode in microseconds,
 * for a LPTMR instance.
 * Can be used only in Timer Mode.
 * Possible return values:
 * - STATUS_SUCCESS: completed successfully
 * - STATUS_ERROR: cannot reconfigure compare value
 * - STATUS_TIMEOUT: compare value greater then current counter value
 *
 * Implements : lpTMR_DRV_SetCompareValueByUs_Activity
 *END**************************************************************************/
status_t lpTMR_DRV_SetCompareValueByUs(const uint32_t instance,
                                       const uint32_t compareValueUs)
{
    DEV_ASSERT(instance < lpTMR_INSTANCE_COUNT);
    DEV_ASSERT(s_lptmrClkFreq != 0U); /* Check the calculated clock frequency: '0' - invalid*/

    status_t returnCode     = STATUS_SUCCESS;
    lpTMR_Type* const base  = g_lptmrBase[instance];
    bool timerEnabled, compareFlag;
    uint16_t cmpValTicks, currentCounterVal;
    lptmr_prescaler_t prescVal;
    bool prescBypass;

    /* This function can only be used if LPTMR is configured in Timer Mode. */
    DEV_ASSERT(lpTMR_GetWorkMode(base) == lpTMR_WORKMODE_TIMER);

    timerEnabled = lpTMR_GetEnable(base);
    compareFlag  = lpTMR_GetCompareFlag(base);
    /* The compare value can only be written if counter is disabled or the compare flag is set. */
    if (timerEnabled && !compareFlag)
    {
        returnCode = STATUS_ERROR;
    }
    else
    {
        bool chooseClkConfigStatus;

        /* When workmode is set to Timer Mode and compare value is provided in microseconds,
         * then the input parameters for prescale value and prescaleBypass are ignored.
         * The prescaleValue, prescaleBypass and cmpValue in ticks, are calculated to best fit
         * the input configCmpValue (in us) for the current operating clk frequency.  */
        chooseClkConfigStatus = lptmr_ChooseClkConfig(s_lptmrClkFreq, compareValueUs, &prescVal, &prescBypass, &cmpValTicks);
        DEV_ASSERT(chooseClkConfigStatus == true);
        (void) chooseClkConfigStatus;

        /* Write value and check if written successfully */
        lpTMR_SetCompareValue(base, cmpValTicks);
        lpTMR_SetPrescaler(base, prescVal);
        lpTMR_SetBypass(base, prescBypass);
        currentCounterVal = lpTMR_GetCounterValue(base);

        if (currentCounterVal >= cmpValTicks)
        {
            returnCode = STATUS_TIMEOUT;
        }
    }

    return returnCode;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : lpTMR_DRV_GetCompareValueByUs
 * Description   : Get the compare value in microseconds representation.
 * Can be used only in Timer Mode.
 *
 * Implements : lpTMR_DRV_GetCompareValueByUs_Activity
 *END**************************************************************************/
void lpTMR_DRV_GetCompareValueByUs(const uint32_t instance,
                                   uint32_t * const compareValueUs)
{
    DEV_ASSERT(instance < lpTMR_INSTANCE_COUNT);
    DEV_ASSERT(compareValueUs != NULL);
    DEV_ASSERT(s_lptmrClkFreq != 0U);

    const lpTMR_Type* const base = g_lptmrBase[instance];
    uint16_t cmpValTicks;
    lptmr_prescaler_t prescVal;
    bool prescBypass, conversionStatus;

    /* This function can only be used if LPTMR is configured in Timer Mode. */
    DEV_ASSERT(lpTMR_GetWorkMode(base) == lpTMR_WORKMODE_TIMER);

    /* Get prescaler value and prescaler bypass state.*/
    prescVal    = lpTMR_GetPrescaler(base);
    prescBypass = lpTMR_GetBypass(base);
    cmpValTicks = lpTMR_GetCompareValue(base);

    /* Convert current compare value from ticks to microseconds. */
    conversionStatus = lptmr_Ticks2Us(s_lptmrClkFreq, prescVal, prescBypass, cmpValTicks, compareValueUs);
    DEV_ASSERT(conversionStatus == true); /* Check the conversion status. */
    (void) conversionStatus;
}


/*FUNCTION**********************************************************************
 *
 * Function Name : lpTMR_DRV_GetCompareFlag
 * Description   : Get the current state of the Compare Flag of a LPTMR instance
 *
 * Implements : lpTMR_DRV_GetCompareFlag_Activity
 *END**************************************************************************/
bool lpTMR_DRV_GetCompareFlag(const uint32_t instance)
{
    DEV_ASSERT(instance < lpTMR_INSTANCE_COUNT);

    const lpTMR_Type* const base = g_lptmrBase[instance];
    bool compareFlag = lpTMR_GetCompareFlag(base);

    return compareFlag;
}


/*FUNCTION**********************************************************************
 *
 * Function Name : lpTMR_DRV_ClearCompareFlag
 * Description   : Clear the Compare Flag.
 *
 * Implements : lpTMR_DRV_ClearCompareFlag_Activity
 *END**************************************************************************/
void lpTMR_DRV_ClearCompareFlag(const uint32_t instance)
{
    DEV_ASSERT(instance < lpTMR_INSTANCE_COUNT);

    lpTMR_Type* const base = g_lptmrBase[instance];

    lpTMR_ClearCompareFlag(base);
}


/*FUNCTION**********************************************************************
 *
 * Function Name : lpTMR_DRV_IsRunning
 * Description   : Get the running state of a LPTMR instance.
 * Possible return values:
 * - true: Timer/Counter started
 * - false: Timer/Counter stopped
 *
 * Implements : lpTMR_DRV_IsRunning_Activity
 *END**************************************************************************/
bool lpTMR_DRV_IsRunning(const uint32_t instance)
{
    DEV_ASSERT(instance < lpTMR_INSTANCE_COUNT);

    const lpTMR_Type* const base = g_lptmrBase[instance];

    bool runningState = lpTMR_GetEnable(base);

    return runningState;
}


/*FUNCTION**********************************************************************
 *
 * Function Name : lpTMR_DRV_SetInterrupt
 * Description   : Enable/disable the LPTMR interrupt.
 *
 * Implements : lpTMR_DRV_SetInterrupt_Activity
 *END**************************************************************************/
void lpTMR_DRV_SetInterrupt(const uint32_t instance,
                            const bool enableInterrupt)
{
    DEV_ASSERT(instance < lpTMR_INSTANCE_COUNT);

    lpTMR_Type* const base = g_lptmrBase[instance];

    lpTMR_SetInterrupt(base, enableInterrupt);
}


/*FUNCTION**********************************************************************
 *
 * Function Name : LPTMR_DRV_GetCounterValueTicks
 * Description   : Get the current Counter Value in timer ticks representation.
 * Return:
 *  - the counter value.
 *
 * Implements : lpTMR_DRV_GetCounterValueByCount_Activity
 *END**************************************************************************/
uint16_t lpTMR_DRV_GetCounterValueByCount(const uint32_t instance)
{
    DEV_ASSERT(instance < lpTMR_INSTANCE_COUNT);

    lpTMR_Type* const base = g_lptmrBase[instance];

    uint16_t counterVal = lpTMR_GetCounterValue(base);

    return counterVal;
}


/*FUNCTION**********************************************************************
 *
 * Function Name : lpTMR_DRV_StartCounter
 * Description   : Enable (start) the counter.
 *
 * Implements : lpTMR_DRV_StartCounter_Activity
 *END**************************************************************************/
void lpTMR_DRV_StartCounter(const uint32_t instance)
{
    DEV_ASSERT(instance < lpTMR_INSTANCE_COUNT);

    lpTMR_Type* const base = g_lptmrBase[instance];

    /* Check if a valid clock is selected for the timer/glitch filter */
#if (defined (DEV_ERROR_DETECT) || defined (CUSTOM_DEVASSERT))
    bool bypass = lpTMR_GetBypass(base);
    lptmr_workmode_t workMode = lpTMR_GetWorkMode(base);
    (void) bypass;
    (void) workMode;
#endif /* (defined (DEV_ERROR_DETECT) || defined (CUSTOM_DEVASSERT)) */

    lpTMR_Enable(base);
}


/*FUNCTION**********************************************************************
 *
 * Function Name : lpTMR_DRV_StopCounter
 * Description   : Disable (stop) the counter.
 *
 * Implements : lpTMR_DRV_StopCounter_Activity
 *END**************************************************************************/
void lpTMR_DRV_StopCounter(const uint32_t instance)
{
    DEV_ASSERT(instance < lpTMR_INSTANCE_COUNT);

    lpTMR_Type* const base = g_lptmrBase[instance];

    lpTMR_Disable(base);
}


/*FUNCTION**********************************************************************
 *
 * Function Name : lpTMR_DRV_SetPinConfiguration
 * Description   : Set the Input Pin configuration for Pulse Counter mode.
 *
 * Implements : lpTMR_DRV_SetPinConfiguration_Activity
 *END**************************************************************************/
void lpTMR_DRV_SetPinConfiguration(const uint32_t instance,
                                   const lptmr_pinselect_t pinSelect,
                                   const lptmr_pinpolarity_t pinPolarity)
{
    DEV_ASSERT(instance < lpTMR_INSTANCE_COUNT);

    lpTMR_Type* const base = g_lptmrBase[instance];

    lpTMR_SetPinSelect(base, pinSelect);
    lpTMR_SetPinPolarity(base, pinPolarity);
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
