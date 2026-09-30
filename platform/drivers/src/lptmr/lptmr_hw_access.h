/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file lptmr_hw_access.h
 * @version 1.4.0
 */

#ifndef LPTMR_HW_ACCESS_H
#define LPTMR_HW_ACCESS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "device_registers.h"
#include "lptmr_driver.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Initialize the LPTMR instance to reset values.
 *
 * This function initializes all registers of the LPTMR instance to a known state (the register
 * are written with their reset values from the Reference Manual).
 *
 * @param[in] base - LPTMR base pointer
 *
 */
void lpTMR_Init(lpTMR_Type* const base);

#if defined(lpTMR_DIE_DMAEN_MASK)
/*!
 * @brief Get the DMA Request Enable Flag
 *
 * This function checks whether a DMA Request feature of the LPTMR is enabled.
 * The DMA Request is issued when a Compare Match is asserted. If enabled, the
 * Compare Match/Interrupt Pending flag is cleared when the DMA controller is
 * done.
 *
 * @param[in] base - LPTMR base pointer
 * @return DMA Request enable
 *      - true: enable DMA Request
 *      - false: disable DMA Request
 */
static inline bool lpTMR_GetDmaRequest(const lpTMR_Type* const base)
{
    DEV_ASSERT(base != NULL);

    uint32_t tmp = base->DIE;

    tmp = (tmp & lpTMR_DIE_DMAEN_MASK) >> lpTMR_DIE_DMAEN_SHIFT;

    return ((tmp == 1u) ? true : false);
}

/*!
 * @brief Configure the DMA Request Enable Flag state
 *
 * This function configures the DMA Request feature of the LPTMR. If enabled,
 * a DMA Request is issued when the Compare Match event occurs. If enabled, the
 * Compare Match/Interrupt Pending flag is cleared when the DMA controller is
 * done.
 *
 * @param[in] base   - LPTMR base pointer
 * @param[in] enable - The new state of the DMA Request Enable Flag
 *      - true: enable DMA Request
 *      - false: disable DMA Request
 */
static inline void lpTMR_SetDmaRequest(lpTMR_Type* const base,
                                       bool enable)
{
    DEV_ASSERT(base != NULL);

    uint32_t tmp = base->DIE;
    tmp &= ~(lpTMR_DIE_DMAEN_MASK);
    tmp |= lpTMR_DIE_DMAEN(enable ? (uint32_t)1u : (uint32_t)0u);
    base->DIE = tmp;
}
#endif /* lpTMR_DIE_DMAEN_MASK */

/*!
 * @brief Get the Compare Flag state
 *
 * This function checks whether a Compare Match event has occurred or if there is
 * an Interrupt Pending.
 *
 * @param[in] base - LPTMR base pointer
 * @return The Compare Flag state
 *      - true: Compare Match/Interrupt Pending asserted
 *      - false: Compare Match/Interrupt Pending not asserted
 */
static inline bool lpTMR_GetCompareFlag(const lpTMR_Type* const base)
{
    DEV_ASSERT(base != NULL);

    uint32_t tmp = base->STS;
    tmp = (tmp & lpTMR_STS_CCF_MASK) >> lpTMR_STS_CCF_SHIFT;

    return ((tmp == 1u) ? true : false);
}

/*!
 * @brief Clear the Compare Flag
 *
 * This function clears the Compare Flag/Interrupt Pending state.
 *
 * @param[in] base - LPTMR base pointer
 */
static inline void lpTMR_ClearCompareFlag(lpTMR_Type* const base)
{
    DEV_ASSERT(base != NULL);

    uint32_t tmp = base->STS;
    tmp |= (lpTMR_STS_CCF_MASK);
    base->STS = tmp;
#ifdef ERRATA_ARM_838869
    /* Read-after-write sequence to guarantee required serialization of memory operations */
    (void)base->CSR;
#endif
}

/*!
 * @brief Get the Interrupt Enable state
 *
 * This function returns the Interrupt Enable state for the LPTMR. If enabled,
 * an interrupt is generated when a Compare Match event occurs.
 *
 * @param[in] base - LPTMR base pointer
 * @return Interrupt Enable state
 *      - true: Interrupt enabled
 *      - false: Interrupt disabled
 */
static inline bool lpTMR_GetInterruptEnable(const lpTMR_Type* const base)
{
    DEV_ASSERT(base != NULL);

    uint32_t tmp = base->DIE;
    tmp = (tmp & lpTMR_DIE_IE_MASK) >> lpTMR_DIE_IE_SHIFT;

    return ((tmp == 1u) ? true : false);
}

/*!
 * @brief Configure the Interrupt Enable state
 *
 * This function configures the Interrupt Enable state for the LPTMR. If enabled,
 * an interrupt is generated when a Compare Match event occurs.
 *
 * @param[in] base   - LPTMR base pointer
 * @param[in] enable - The new state for the interrupt
 *          - true: enable Interrupt
 *          - false: disable Interrupt
 */
static inline void lpTMR_SetInterrupt(lpTMR_Type* const base,
                                      bool enable)
{
    DEV_ASSERT(base != NULL);

    uint32_t tmp = base->DIE;
    tmp &= ~(lpTMR_DIE_IE_MASK);
    tmp |= lpTMR_DIE_IE(enable ? (uint32_t)1u : (uint32_t)0u);
    base->DIE = tmp;
}

/*!
 * @brief Get the Pin select for Counter Mode
 *
 * This function returns the configured Input Pin for Pulse Counter Mode.
 *
 * @param[in] base - LPTMR base pointer
 * @return Input pin selection
 *          - lpTMR_PINSELECT_TMU: count pulses from TMU output
 *          - LPTMR_PINSELECT_ALT1: count pulses from pin alt 1
 *          - lpTMR_PINSELECT_ALT2: count pulses from pin alt 2
 *          - LPTMR_PINSELECT_ALT3: count pulses from pin alt 3
 */
static inline lptmr_pinselect_t lpTMR_GetPinSelect(const lpTMR_Type* const base)
{
    DEV_ASSERT(base != NULL);

    uint32_t tmp = base->CTRL;
    tmp = (tmp & lpTMR_CTRL_PINSEL_MASK) >> lpTMR_CTRL_PINSEL_SHIFT;
    return (lptmr_pinselect_t)(tmp);
}

/*!
 * @brief Configure the Pin selection for Pulse Counter Mode
 *
 * This function configures the input Pin selection for Pulse Counter Mode.
 * This feature can be configured only when the LPTMR is disabled.
 *
 * @param[in] base   - LPTMR base pointer
 * @param[in] pinsel - Pin selection
 *          - lpTMR_PINSELECT_TMU: count pulses from TMU output
 *          - LPTMR_PINSELECT_ALT1: count pulses from pin alt 1
 *          - lpTMR_PINSELECT_ALT2: count pulses from pin alt 2
 *          - LPTMR_PINSELECT_ALT3: count pulses from pin alt 3
 */
static inline void lpTMR_SetPinSelect(lpTMR_Type* const base,
                                      const lptmr_pinselect_t pinsel)
{
    DEV_ASSERT(base != NULL);

    uint32_t tmp = base->CTRL;
    tmp &= ~(lpTMR_CTRL_PINSEL_MASK);
    tmp |= lpTMR_CTRL_PINSEL(pinsel);
    base->CTRL = tmp;
}

/*!
 * @brief Get Pin Polarity for Pulse Counter Mode
 *
 * This function returns the configured pin polarity that triggers an increment
 * in Pulse Counter Mode.
 *
 * @param[in] base - LPTMR base pointer
 * @return The pin polarity for Pulse Counter Mode
 *          - lpTMR_PINPOLARITY_RISING: count pulse on Rising Edge
 *          - lpTMR_PINPOLARITY_FALLING: count pulse on Falling Edge
 */
static inline lptmr_pinpolarity_t lpTMR_GetPinPolarity(const lpTMR_Type* const base)
{
    DEV_ASSERT(base != NULL);

    uint32_t tmp = base->STS;
    tmp = (tmp & lpTMR_CTRL_PINPOL_MASK) >> lpTMR_CTRL_PINPOL_SHIFT;

    return (lptmr_pinpolarity_t)((tmp == 0u) ? lpTMR_PINPOLARITY_RISING : lpTMR_PINPOLARITY_FALLING);
}

/*!
 * @brief Configure Pin Polarity for Pulse Counter Mode
 *
 * This function configures the pin polarity that triggers an increment in Pulse
 * Counter Mode. This feature can be configured only when the LPTMR is disabled.
 *
 * @param[in] base - LPTMR base pointer
 * @param[in] pol  - The pin polarity to count in Pulse Counter Mode
 *          - lpTMR_PINPOLARITY_RISING: count pulse on Rising Edge
 *          - lpTMR_PINPOLARITY_FALLING: count pulse on Falling Edge
 */
static inline void lpTMR_SetPinPolarity(lpTMR_Type* const base,
                                        const lptmr_pinpolarity_t pol)
{
    DEV_ASSERT(base != NULL);

    uint32_t tmp = base->CTRL;
    tmp &= ~(lpTMR_CTRL_PINPOL_MASK);
    tmp |= lpTMR_CTRL_PINPOL(pol);
    base->CTRL = tmp;
}

/*!
 * @brief Get the Free Running state
 *
 * This function checks whether the Free Running feature of the LPTMR is enabled
 * or disabled.
 *
 * @param[in] base - LPTMR base pointer
 * @return Free running mode state
 *          - true: Free Running Mode enabled. Reset counter on 16-bit overflow
 *          - false: Free Running Mode disabled. Reset counter on Compare Match.
 */
static inline bool lpTMR_GetFreeRunning(const lpTMR_Type* const base)
{
    DEV_ASSERT(base != NULL);

    uint32_t tmp = base->CTRL;
    tmp = (tmp & lpTMR_CTRL_TMODE_MASK) >> lpTMR_CTRL_TMODE_SHIFT;

    return ((tmp == 1u) ? true : false);
}

/*!
 * @brief Configure the Free Running state
 *
 * This function configures the Free Running feature of the LPTMR. This feature
 * can be configured only when the LPTMR is disabled.
 *
 * @param[in] base   - LPTMR base pointer
 * @param[in] enable - The new Free Running state
 *          - true: Free Running Mode enabled. Reset counter on 16-bit overflow
 *          - false: Free Running Mode disabled. Reset counter on Compare Match.
 */
static inline void lpTMR_SetFreeRunning(lpTMR_Type* const base,
                                        const bool enable)
{
    DEV_ASSERT(base != NULL);

    uint32_t tmp = base->CTRL;
    tmp &= ~(lpTMR_CTRL_TMODE_MASK);
    tmp |= lpTMR_CTRL_TMODE(enable ? (uint32_t)1u : (uint32_t)0u);
    base->CTRL = tmp;
}

/*!
 * @brief Get current Work Mode
 *
 * This function returns the currently configured Work Mode for the LPTMR.
 *
 *
 * @param[in] base - LPTMR base pointer
 * @return Work Mode
 *          - lpTMR_WORKMODE_TIMER: LPTMR is in Timer Mode
 *          - lpTMR_WORKMODE_PULSECOUNTER: LPTMR is in Pulse Counter Mode
 */
static inline lptmr_workmode_t lpTMR_GetWorkMode(const lpTMR_Type* const base)
{
    DEV_ASSERT(base != NULL);

    uint32_t tmp = base->CTRL;
    tmp = (tmp & lpTMR_CTRL_MODE_MASK) >> lpTMR_CTRL_MODE_SHIFT;

    return (lptmr_workmode_t)((tmp == 0u) ? lpTMR_WORKMODE_TIMER : lpTMR_WORKMODE_PULSECOUNTER);
}

/*!
 * @brief Configure the Work Mode
 *
 * This function configures the LPTMR to either Timer Mode or Pulse Counter
 * Mode. This feature can be configured only when the LPTMR is disabled.
 *
 * @param[in] base - LPTMR base pointer
 * @param[in] mode - New Work Mode
 *          - lpTMR_WORKMODE_TIMER: lpTMR set to Timer Mode
 *          - lpTMR_WORKMODE_PULSECOUNTER: lpTMR set to Pulse Counter Mode
 */
static inline void lpTMR_SetWorkMode(lpTMR_Type* const base,
                                     const lptmr_workmode_t mode)
{
    DEV_ASSERT(base != NULL);

    uint32_t tmp = base->CTRL;
    tmp &= ~(lpTMR_CTRL_MODE_MASK);
    tmp |= lpTMR_CTRL_MODE(mode);
    base->CTRL = tmp;
}

/*!
 * @brief Get the Enable state.
 *
 * Prior to reconfiguring the LPTMR, it is necessary to disable it.
 *
 * @param[in] base - LPTMR base pointer
 * @return The state of the LPTMR
 *          - true: LPTMR enabled
 *          - false: LPTMR disabled
 */
static inline bool lpTMR_GetEnable(const lpTMR_Type* const base)
{
    DEV_ASSERT(base != NULL);

    uint32_t tmp = base->CTRL;
    tmp = (tmp & lpTMR_CTRL_EN_MASK) >> lpTMR_CTRL_EN_SHIFT;

    return ((tmp == 1u) ? true : false);
}

/*!
 * @brief Enable the LPTMR
 *
 * Enable the LPTMR. Starts the timer/counter.
 *
 *
 * @param[in] base - LPTMR base pointer
 */
static inline void lpTMR_Enable(lpTMR_Type* const base)
{
    DEV_ASSERT(base != NULL);

    uint32_t tmp = base->CTRL;
    tmp &= ~(lpTMR_CTRL_EN_MASK);
    tmp |= lpTMR_CTRL_EN(1u);
    base->CTRL = tmp;
}

/*!
 * @brief Disable the LPTMR
 *
 * Disable the LPTMR. Stop the Counter/Timer and allow reconfiguration.
 *
 * @param[in] base - LPTMR base pointer
 */
static inline void lpTMR_Disable(lpTMR_Type* const base)
{
    DEV_ASSERT(base != NULL);

    uint32_t tmp = base->CTRL;
    tmp &= ~(lpTMR_CTRL_EN_MASK);
    tmp |= lpTMR_CTRL_EN(0u);
    base->CTRL = tmp;
}

/*!
 * @brief Get Prescaler/Glitch Filter divider value
 *
 * This function returns the currently configured Prescaler/Glitch Filter divider
 * value.
 *
 * @param[in] base - LPTMR base pointer
 * @return The Prescaler/Glitch filter value
 *          - lpTMR_PRESCALE_2: Timer mode: prescaler 2, Glitch filter mode: invalid
 *          - lpTMR_PRESCALE_4_GLITCHFILTER_2: Timer mode: prescaler 4, Glitch filter mode: 2 clocks
 *          - lpTMR_PRESCALE_8_GLITCHFILTER_4: Timer mode: prescaler 8, Glitch filter mode: 4 clocks
 *          - lpTMR_PRESCALE_16_GLITCHFILTER_8: Timer mode: prescaler 16, Glitch filter mode: 8 clocks
 *          - lpTMR_PRESCALE_32_GLITCHFILTER_16: Timer mode: prescaler 32, Glitch filter mode: 16 clocks
 *          - lpTMR_PRESCALE_64_GLITCHFILTER_32: Timer mode: prescaler 64, Glitch filter mode: 32 clocks
 *          - lpTMR_PRESCALE_128_GLITCHFILTER_64: Timer mode: prescaler 128, Glitch filter mode: 64 clocks
 *          - lpTMR_PRESCALE_256_GLITCHFILTER_128: Timer mode: prescaler 256, Glitch filter mode: 128 clocks
 *          - lpTMR_PRESCALE_512_GLITCHFILTER_256: Timer mode: prescaler 512, Glitch filter mode: 256 clocks
 *          - lpTMR_PRESCALE_1024_GLITCHFILTER_512: Timer mode: prescaler 1024, Glitch filter mode: 512 clocks
 *          - lpTMR_PRESCALE_2048_GLITCHFILTER_1024: Timer mode: prescaler 2048, Glitch filter mode: 1024 clocks
 *          - lpTMR_PRESCALE_4096_GLITCHFILTER_2048: Timer mode: prescaler 4096, Glitch filter mode: 2048 clocks
 *          - lpTMR_PRESCALE_8192_GLITCHFILTER_4096: Timer mode: prescaler 8192, Glitch filter mode: 4096 clocks
 *          - lpTMR_PRESCALE_16384_GLITCHFILTER_8192: Timer mode: prescaler 16384, Glitch filter mode: 8192 clocks
 *          - lpTMR_PRESCALE_32768_GLITCHFILTER_16384: Timer mode: prescaler 32768, Glitch filter mode: 16384 clocks
 *          - lpTMR_PRESCALE_65536_GLITCHFILTER_32768: Timer mode: prescaler 65536, Glitch filter mode: 32768 clocks
 */
static inline lptmr_prescaler_t lpTMR_GetPrescaler(const lpTMR_Type* const base)
{
    DEV_ASSERT(base != NULL);

    uint32_t tmp = base->PRS;
    tmp = (tmp & lpTMR_PRS_PRES_MASK) >> lpTMR_PRS_PRES_SHIFT;
    return (lptmr_prescaler_t)(tmp);
}

/*!
 * @brief Configure the Prescaler/Glitch Filter divider value
 *
 * This function configures the value for the Prescaler/Glitch Filter. This
 * feature can be configured only when the LPTMR is disabled.
 *
 * @param[in] base  - LPTMR base pointer
 * @param[in] presc - The new Prescaler value
 *          - lpTMR_PRESCALE_2: Timer mode: prescaler 2, Glitch filter mode: invalid
 *          - lpTMR_PRESCALE_4_GLITCHFILTER_2: Timer mode: prescaler 4, Glitch filter mode: 2 clocks
 *          - lpTMR_PRESCALE_8_GLITCHFILTER_4: Timer mode: prescaler 8, Glitch filter mode: 4 clocks
 *          - lpTMR_PRESCALE_16_GLITCHFILTER_8: Timer mode: prescaler 16, Glitch filter mode: 8 clocks
 *          - lpTMR_PRESCALE_32_GLITCHFILTER_16: Timer mode: prescaler 32, Glitch filter mode: 16 clocks
 *          - lpTMR_PRESCALE_64_GLITCHFILTER_32: Timer mode: prescaler 64, Glitch filter mode: 32 clocks
 *          - lpTMR_PRESCALE_128_GLITCHFILTER_64: Timer mode: prescaler 128, Glitch filter mode: 64 clocks
 *          - lpTMR_PRESCALE_256_GLITCHFILTER_128: Timer mode: prescaler 256, Glitch filter mode: 128 clocks
 *          - lpTMR_PRESCALE_512_GLITCHFILTER_256: Timer mode: prescaler 512, Glitch filter mode: 256 clocks
 *          - lpTMR_PRESCALE_1024_GLITCHFILTER_512: Timer mode: prescaler 1024, Glitch filter mode: 512 clocks
 *          - lpTMR_PRESCALE_2048_GLITCHFILTER_1024: Timer mode: prescaler 2048, Glitch filter mode: 1024 clocks
 *          - lpTMR_PRESCALE_4096_GLITCHFILTER_2048: Timer mode: prescaler 4096, Glitch filter mode: 2048 clocks
 *          - lpTMR_PRESCALE_8192_GLITCHFILTER_4096: Timer mode: prescaler 8192, Glitch filter mode: 4096 clocks
 *          - lpTMR_PRESCALE_16384_GLITCHFILTER_8192: Timer mode: prescaler 16384, Glitch filter mode: 8192 clocks
 *          - lpTMR_PRESCALE_32768_GLITCHFILTER_16384: Timer mode: prescaler 32768, Glitch filter mode: 16384 clocks
 *          - lpTMR_PRESCALE_65536_GLITCHFILTER_32768: Timer mode: prescaler 65536, Glitch filter mode: 32768 clocks
 */
static inline void lpTMR_SetPrescaler(lpTMR_Type* const base,
                                      const lptmr_prescaler_t presc)
{
    DEV_ASSERT(base != NULL);

    uint32_t tmp = base->PRS;
    tmp &= ~(lpTMR_PRS_PRES_MASK);
    tmp |= lpTMR_PRS_PRES(presc);
    base->PRS = tmp;
}
#ifdef FEATURE_lpTMR_HAS_CLOCK_SELECTION
/*!
 * @brief Configure the clock source for the LPTMR
 *
 * This function configure the clock source for lpTMR. This
 * feature can be configured only when the LPTMR is disabled.
 */
 static inline void lpTMR_SetClockSource(lpTMR_Type* const base,
                                         const lptmr_clock_source_t source)
{
    DEV_ASSERT(base != NULL);

    uint32_t tmp = base->PRS;
    tmp &= ~(lpTMR_PRS_CLKSEL_MASK);
    tmp |= lpTMR_PRS_CLKSEL(source);
    base->PRS = tmp;
}
#endif /* FEATURE_lpTMR_HAS_CLOCK_SELECTION */

/*!
 * @brief Get the Prescaler/Glitch Filter Bypass enable state
 *
 * This function checks whether the Prescaler/Glitch Filter Bypass is enabled.
 *
 * @param[in] base - LPTMR base pointer
 * @return The Prescaler Bypass state
 *          - true: Prescaler/Glitch Filter Bypass enabled
 *          - false: Prescaler/Glitch Filter Bypass disabled
 */
static inline bool lpTMR_GetBypass(const lpTMR_Type* const base)
{
    DEV_ASSERT(base != NULL);

    uint32_t tmp = base->PRS;
    tmp = (tmp & lpTMR_PRS_BYPASS_MASK) >> lpTMR_PRS_BYPASS_SHIFT;

    return ((tmp == 1u) ? true : false);
}

/*!
 * @brief Configure the Prescaler/Glitch Filter Bypass enable state
 *
 * This function configures the Prescaler/Glitch filter Bypass. This feature
 * can be configured only when the LPTMR is disabled.
 *
 * @param[in] base  - LPTMR base pointer
 * @param[in] enable - The new Prescaler/Glitch Filter Bypass state
 *          - true: Prescaler/Glitch Filter Bypass enabled
 *          - false: Prescaler/Glitch Filter Bypass disabled
 */
static inline void lpTMR_SetBypass(lpTMR_Type* const base,
                                   const bool enable)
{
    DEV_ASSERT(base != NULL);

    uint32_t tmp = base->PRS;
    tmp &= ~(lpTMR_PRS_BYPASS_MASK);
    tmp |= lpTMR_PRS_BYPASS(enable ? (uint32_t)1u : (uint32_t)0u);
    base->PRS = tmp;
}



/*!
 * @brief Get the Compare Value
 *
 * This function returns the current Compare Value.
 *
 * @param[in] base - LPTMR base pointer
 * @return The Compare Value
 */
static inline uint16_t lpTMR_GetCompareValue(const lpTMR_Type* const base)
{
    DEV_ASSERT(base != NULL);

    uint32_t tmp = base->CMP;
    tmp = (tmp & lpTMR_CMP_CMP_MASK) >> lpTMR_CMP_CMP_SHIFT;
    return (uint16_t)(tmp);
}

/*!
 * @brief Configure the Compare Value
 *
 * This function configures the Compare Value. If set to 0, the Compare Match
 * event and the hardware trigger assert and remain asserted until the timer is
 * disabled.
 *
 * @param[in] base - LPTMR base pointer
 * @param[in] compval - The new Compare Value
 */
static inline void lpTMR_SetCompareValue(lpTMR_Type* const base,
                                         const uint16_t compval)
{
    DEV_ASSERT(base != NULL);

    uint32_t tmp = base->CMP;
    tmp &= ~(lpTMR_CMP_CMP_MASK);
    tmp |= lpTMR_CMP_CMP(compval);
    base->CMP = tmp;
}

/*!
 * @brief Get the current Counter Value
 *
 * This function returns the Counter Value.
 *
 * @param[in] base - LPTMR base pointer
 * @return The Counter Value
 */
static inline uint16_t lpTMR_GetCounterValue(lpTMR_Type* const base)
{
    DEV_ASSERT(base != NULL);

    /* Write latch value before reading register */
    base->LCNT = (0u);
    uint16_t cnr = (uint16_t)base->CNT;
    return cnr;
}

#if defined(__cplusplus)
}
#endif

#endif /* LPTMR_HW_ACCESS_H */
/*******************************************************************************
 * EOF
 ******************************************************************************/
