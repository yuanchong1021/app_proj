/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file adc_hw_access.h
 * @version 1.4.0
 */

#ifndef ADC_HW_ACCESS_H
#define ADC_HW_ACCESS_H

#include <stdint.h>
#include <stdbool.h>
#include "device_registers.h"
#include "adc_driver.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define ADC_WAIT_TIMEOUT (1000U)

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined (__cplusplus)
extern "C" {
#endif

/*!
 * @name Converter
 * General ADC functions.
 */
/*! @{*/

/*!
 * @brief Gets the Conversion Active Flag
 *
 * This function checks whether a conversion is currently
 * taking place on the ADC module.
 *
 *
 * @param[in] baseAddr adc base pointer
 * @return Conversion Active Flag state
 */
static inline bool ADC_GetConvActiveFlag(const ADC_Type *const baseAddr)
{
    uint32_t tmp = (uint32_t) baseAddr->CTRL;
    tmp = (tmp & ADC_CTRL_ADSTART_MASK) >> ADC_CTRL_ADSTART_SHIFT;
    return (tmp != 0u) ? true : false;
}

/*!
 * @brief Gets the Adc Enable Flag
 *
 * This function checks whether a conversion is currently
 * enabled on the ADC module.
 *
 *
 * @param[in] baseAddr adc base pointer
 * @return Conversion Active Flag state
 */
static inline bool ADC_GetEnableFlag(const ADC_Type *const baseAddr)
{
    uint32_t tmp = (uint32_t) baseAddr->CTRL;
    tmp = (tmp & ADC_CTRL_ADEN_MASK) >> ADC_CTRL_ADEN_SHIFT;
    return (tmp != 0u) ? true : false;
}


/*!
 * @brief Gets the Sequence Conversion Done Flag
 *
 * This function checks whether a sequence conversion is finished.
 *
 *
 * @param[in] baseAddr adc base pointer
 * @return Sequence Conversion Done Flag state
 */
static inline bool ADC_GetSequenceDoneFlag(const ADC_Type *const baseAddr)
{
    uint32_t tmp = (uint32_t) baseAddr->STS;
    tmp = (tmp & ADC_STS_EOSEQ_MASK) >> ADC_STS_EOSEQ_SHIFT;
    return (tmp != 0u) ? true : false;
}

/*!
 * @brief Gets the current ADC clock divider configuration.
 *
 * This function returns the configured clock divider
 * bitfield value for the ADC instance.
 *
 * @param[in] baseAddr adc base pointer
 * @return the clock divider value. Possible values:
 *        - ADC_CLK_DIVIDE_1 : Divider set to 1.
 *        - ADC_CLK_DIVIDE_2 : Divider set to 2.
 *        - ADC_CLK_DIVIDE_4 : Divider set to 4.
 *        - ADC_CLK_DIVIDE_8 : Divider set to 8.
 */
static inline adc_clk_divide_t ADC_GetClockDivide(const ADC_Type *const baseAddr)
{
    uint32_t tmp = baseAddr->CFG1;
    tmp = (tmp & ADC_CFG1_PRS_MASK) >> ADC_CFG1_PRS_SHIFT;

    /* Enum defines all possible values, so casting is safe */
    return (adc_clk_divide_t) (tmp);
}

/*!
 * @brief Sets the ADC clock divider configuration.
 *
 * This functions configures the ADC instance clock divider.
 *
 * @param[in] baseAddr adc base pointer
 * @param[in] clockDivide clk divider
 *        - ADC_CLK_DIVIDE_1 : Divider set to 1.
 *        - ADC_CLK_DIVIDE_2 : Divider set to 2.
 *        - ADC_CLK_DIVIDE_4 : Divider set to 4.
 *        - ADC_CLK_DIVIDE_8 : Divider set to 8.
 */
static inline void ADC_SetClockDivide(ADC_Type *const baseAddr,
                                      const adc_clk_divide_t clockDivide)
{
    uint32_t tmp = baseAddr->CFG1;
    tmp &= ~(ADC_CFG1_PRS_MASK);
    tmp |= ADC_CFG1_PRS(clockDivide);
    baseAddr->CFG1 = tmp;
}

/*!
 * @brief Gets the Start time in AD clock cycles
 *
 * This function gets the start time (in AD clocks)
 * configured for the ADC.
 * 
 * @param[in] baseAddr adc base pointer
 * @return the Start Time in AD Clocks
 */
static inline uint8_t ADC_GetStartTime(const ADC_Type *const baseAddr)
{
    uint32_t tmp = baseAddr->CFG1;
    tmp = (tmp & ADC_CFG1_STCNT_MASK) >> ADC_CFG1_STCNT_SHIFT;
    return (uint8_t)(tmp);
}

/*!
 * @brief Sets the Start time in AD clock cycles
 *
 * This function configures the start time for the ADC (in
 * ADCK clocks). The actual start time will be the value
 * provided plus 1.  
 *
 * @param[in] baseAddr adc base pointer
 * @param[in] sampletime Start time in AD Clocks
 */
static inline void ADC_SetStartTime(ADC_Type *const baseAddr,
                                    uint8_t starttime)
{
    /* Clip start time to minimum value */
    uint32_t tmp = baseAddr->CFG1;
    tmp &= ~(ADC_CFG1_STCNT_MASK);
    tmp |= ADC_CFG1_STCNT(starttime);
    baseAddr->CFG1 = tmp;
}

/*!
 * @brief Gets the Sample time in AD clock cycles
 *
 * This function gets the sample time (in AD clocks)
 * configured for the ADC. Selection of 2 to 256 ADCK is
 * possible. The value returned by this function is the
 * sample time minus 1. A sample time of 1 is not supported.
 *
 * @param[in] baseAddr adc base pointer
 * @return the Sample Time in AD Clocks
 */
static inline uint8_t ADC_GetSampleTime(const ADC_Type *const baseAddr)
{
    uint32_t tmp = baseAddr->SMP;
    tmp = (tmp & ADC_SMP_SMP_MASK) >> ADC_SMP_SMP_SHIFT;
    return (uint8_t)(tmp);
}

/*!
 * @brief Sets the Sample time in AD clock cycles
 *
 * This function configures the sample time for the ADC (in
 * ADCK clocks). The actual sample time will be the value
 * provided plus 1.  Selection of 2 to 256 ADCK is possible.
 * A real sample time of 1 is not supported (a parameter value of 0
 * will be automatically be changed to 1).
 *
 * @param[in] baseAddr adc base pointer
 * @param[in] sampletime Sample time in AD Clocks
 */
static inline void ADC_SetSampleTime(ADC_Type *const baseAddr,
                                     uint8_t sampletime)
{
    /* Clip sample time to minimum value */
    uint8_t rsampletime = (uint8_t) ((sampletime > 0U) ? sampletime : 1U);
    uint32_t tmp = baseAddr->SMP;
    tmp &= ~(ADC_SMP_SMP_MASK);
    tmp |= ADC_SMP_SMP(rsampletime);
    baseAddr->SMP = tmp;
}

/*!
 * @brief Gets the overrun mode Flag state
 *
 * This function returns the state of the ADC overrun mode flag.
 * ADC overrun mode feature will overwrite the result register when overrun is detected.
 *
 * @param[in] baseAddr adc base pointer
 * @return the overrun mode Flag state
 */
static inline bool ADC_GetOverrunModeFlag(const ADC_Type *const baseAddr)
{
    uint32_t tmp = (uint32_t) baseAddr->CFG0;
    tmp = (tmp & ADC_CFG0_OVRMD_MASK) >> ADC_CFG0_OVRMD_SHIFT;
    return (tmp != 0u) ? true : false;
}

/*!
 * @brief Sets the overrun mode Flag state
 *
 * This function configures the ADC overrun mode Flag. 
 * ADC overrun mode feature will overwrite the result register when overrun is detected.
 *
 * @param[in] baseAddr adc base pointer
 * @param[in] state the overrun mode Flag state
 */
static inline void ADC_SetOverrunModeFlag(ADC_Type *const baseAddr,
                                          const bool state)
{
    uint32_t tmp = (uint32_t) baseAddr->CFG0;
    tmp &= (uint32_t) (~(ADC_CFG0_OVRMD_MASK));
    tmp |= ADC_CFG0_OVRMD(state ? (uint32_t) 1u : (uint32_t) 0u);
    baseAddr->CFG0 = (uint32_t) tmp;
}

/*!
 * @brief Gets the autoff Enable Flag state
 *
 * This function returns the state of the ADC autoff enable flag.
 * ADC autoff feature will power off ADC automatically.
 *
 * @param[in] baseAddr adc base pointer
 * @return the autoff mode Flag state
 */
static inline bool ADC_GetautoOffEnableFlag(const ADC_Type *const baseAddr)
{
    uint32_t tmp = (uint32_t) baseAddr->CFG0;
    tmp = (tmp & ADC_CFG0_AUTOOFF_MASK) >> ADC_CFG0_AUTOOFF_SHIFT;
    return (tmp != 0u) ? true : false;
}

/*!
 * @brief Sets the autoff Enable Flag state
 *
 * This function configures the ADC autoff Enable Flag. 
 * ADC autoff feature will power off ADC automatically.
 *
 * @param[in] baseAddr adc base pointer
 * @param[in] state the overrun mode Flag state
 */
static inline void ADC_SetautoOffEnableFlag(ADC_Type *const baseAddr,
                                            const bool state)
{
    uint32_t tmp = (uint32_t) baseAddr->CFG0;
    tmp &= (uint32_t) (~(ADC_CFG0_AUTOOFF_MASK));
    tmp |= ADC_CFG0_AUTOOFF(state ? (uint32_t) 1u : (uint32_t) 0u);
    baseAddr->CFG0 = (uint32_t) tmp;
}


/*!
 * @brief Gets the Wait Flag state
 *
 * This function returns the state of the ADC wait flag.
 * ADC wait feature will hold adc convert when FIFO is full.
 *
 * @param[in] baseAddr adc base pointer
 * @return the Wait Flag state
 */
static inline bool ADC_GetWaitEnableFlag(const ADC_Type *const baseAddr)
{
    uint32_t tmp = (uint32_t) baseAddr->CFG0;
    tmp = (tmp & ADC_CFG0_WAIT_MASK) >> ADC_CFG0_WAIT_SHIFT;
    return (tmp != 0u) ? true : false;
}

/*!
 * @brief Sets the Wait Flag state
 *
 * This function configures the ADC Wait Flag. 
 * ADC wait feature will hold adc convert when FIFO is full.
 *
 * @param[in] baseAddr adc base pointer
 * @param[in] state the new Wait Flag state
 */
static inline void ADC_SetWaitEnableFlag(ADC_Type *const baseAddr,
                                         const bool state)
{
    uint32_t tmp = (uint32_t) baseAddr->CFG0;
    tmp &= (uint32_t) (~(ADC_CFG0_WAIT_MASK));
    tmp |= ADC_CFG0_WAIT(state ? (uint32_t) 1u : (uint32_t) 0u);
    baseAddr->CFG0 = (uint32_t) tmp;
}

/*!
 * @brief Sets the Low Power state
 *
 * This function configures the ADC Low power Flag. 
 * Keep working enabled/disabled in low power mode..
 *
 * @param[in] baseAddr adc base pointer
 * @param[in] state the new Low Power Flag state
 */
static inline void ADC_SetLowPowerEnableFlag(ADC_Type *const baseAddr,
                                             const bool state)
{
#if FEATURE_ADC_SUPPORT_LOW_POWER_KEEP
    uint32_t tmp = (uint32_t) baseAddr->CFG0;
    tmp &= (uint32_t) (~(ADC_CFG0_LPEN_MASK));
    tmp |= ADC_CFG0_LPEN(state ? (uint32_t) 1u : (uint32_t) 0u);
    baseAddr->CFG0 = (uint32_t) tmp;
#else
    (void) baseAddr;
    (void) state;
#endif /* FEATURE_ADC_SUPPORT_LOW_POWER_KEEP */
}

/*!
 * @brief Gets the ADC Trigger Source
 *
 * This function returns the configured triggering source
 * for the ADC.  When in Hardware trigger mode,  a conversion
 * is started by another peripheral (like TMU).
 * @param[in] baseAddr adc base pointer
 * @return the hardware trigger mode. Possible values:
 * in case of YTM32B1LD0:
 *        - 0 : trigger 0.
 *        - 1 : trigger 1.
 *        - 2 : trigger 2.
 *        - 3 : trigger 3.
 *        - 4 : trigger 4.
 *        - 5 : trigger 5.
 *        - 6 : trigger 6.
 *        - 7 : trigger 7.
 * in case of YTM32B1MD0
 *        - not defined !
 */
static inline adc_trigger_t ADC_GetTriggerSource(const ADC_Type *const baseAddr)
{
#if defined(CPU_YTM32B1LD0)
    uint32_t tmp = baseAddr->CFG0;
    tmp = (tmp & ADC_CFG0_TRIGSRC_MASK) >> ADC_CFG0_TRIGSRC_SHIFT;

    /* Enum defines all possible values, so casting is safe */
    return (adc_trigger_t)(tmp);
#else
    uint32_t tmp = baseAddr->CFG0;
    tmp = 0;

    /* Enum defines all possible values, so casting is safe */
    return (adc_trigger_t) (tmp);
#endif
}

/*!
 * @brief Sets the ADC Trigger Source
 *
 * This function configures the ADC triggering source. 
 * When in Hardware trigger mode, a conversion is started
 * by another peripheral (like TMU).
 *
 * @param[in] baseAddr adc base pointer
 * @param[in] trigger the desired trigger source
 * in case of YTM32B1LD0:
 *        - 0 : trigger 0.
 *        - 1 : trigger 1.
 *        - 2 : trigger 2.
 *        - 3 : trigger 3.
 *        - 4 : trigger 4.
 *        - 5 : trigger 5.
 *        - 6 : trigger 6.
 *        - 7 : trigger 7.
 * in case of YTM32B1MD1,YTM32B1ME0
 *        - not defined !
 */
static inline void ADC_SetTriggerSource(ADC_Type *const baseAddr,
                                        uint8_t trigsrc)
{
#if defined(CPU_YTM32B1LD0)
    uint32_t tmp = baseAddr->CFG0;
    tmp &= ~(ADC_CFG0_TRIGSRC_MASK);
    tmp |= ADC_CFG0_TRIGSRC(trigsrc);
    baseAddr->CFG0 = tmp;
#else
    uint32_t tmp = baseAddr->CFG0;
    (void) trigsrc;
    (void) tmp;
#endif
}


/*!
 * @brief Gets the ADC Trigger Mode
 *
 * This function returns the configured triggering mode
 * for the ADC. In Software Triggering Mode, the user can
 * start conversions by setting an input channel in the
 * ADC measurement channel A (index 0). When in Hardware
 * trigger mode, a conversion is started by another peripheral (
 * like PTU or TMU).
 *
 * @param[in] baseAddr adc base pointer
 * @return the current trigger mode. Possible values:
 *        - ADC_TRIGGER_SOFTWARE : Software triggering.
 *        - ADC_TRIGGER_HARDWARE : Hardware triggering.
 */

static inline adc_trigger_t ADC_GetTriggerMode(const ADC_Type *const baseAddr)
{
    uint32_t tmp = baseAddr->CFG0;
#if defined(FEATURE_ADC_SUPPORT_INJECTION_MODE) && (FEATURE_ADC_SUPPORT_INJECTION_MODE > 0)
    tmp = (tmp & (ADC_CFG0_NORTMD_MASK | ADC_CFG0_INJTMD_MASK)) >> ADC_CFG0_NORTMD_SHIFT;
#else
#if defined (ADC_CFG0_NORTMD_MASK)
    tmp = (tmp & ADC_CFG0_NORTMD_MASK) >> ADC_CFG0_NORTMD_SHIFT;
#else
    tmp = (tmp & ADC_CFG0_TRIGMD_MASK) >> ADC_CFG0_TRIGMD_SHIFT;
#endif /* defined (ADC_CFG0_NORTMD_MASK) */
#endif /* FEATURE_ADC_SUPPORT_INJECTION_MODE */

    /* Enum defines all possible values, so casting is safe */
    return (adc_trigger_t) (tmp);
}


/*!
 * @brief Sets the ADC Trigger Mode
 *
 * This function configures the ADC triggering mode. In
 * Software Triggering Mode, the user can start conversions
 * by setting an input channel in the ADC measurement channel
 * A (index 0). When in Hardware trigger mode, a conversion
 * is started by another peripheral (like PTU or TMU).
 *
 * @param[in] baseAddr adc base pointer
 * @param[in] trigger the desired trigger mode
 * in case of YTM32B1LD0:
 *        - 0 : Software trigger
 *        - 1 : Hardware trigger on rising edge
 *        - 2 : Hardware trigger on falling edge
 *        - 3 : Hardware trigger on both rising and falling edge
 *        - 4 : Hardware trigger on high level voltage
 *        - 5 : Hardware trigger on low level voltage
 * in case of YTM32B1MD0:
 *        - 0 : Software triggering.
 *        - 1 : Hardware triggering on rising edge.
 */
static inline void ADC_SetTriggerMode(ADC_Type *const baseAddr,
                                      const adc_trigger_t trigger)
{
    uint32_t tmp = baseAddr->CFG0;
#if defined(FEATURE_ADC_SUPPORT_INJECTION_MODE) && (FEATURE_ADC_SUPPORT_INJECTION_MODE > 0)
    tmp &= ~(ADC_CFG0_NORTMD_MASK | ADC_CFG0_INJTMD_MASK);
    tmp |= ((uint32_t)(trigger) << ADC_CFG0_NORTMD_SHIFT) & (ADC_CFG0_NORTMD_MASK | ADC_CFG0_INJTMD_MASK);
#else
#if defined (ADC_CFG0_NORTMD_MASK)
    tmp &= ~ADC_CFG0_NORTMD_MASK;
    tmp |= ((uint32_t)(trigger) << ADC_CFG0_NORTMD_SHIFT) & ADC_CFG0_NORTMD_MASK;
#else
    tmp &= ~(ADC_CFG0_TRIGMD_MASK);
    tmp |= ADC_CFG0_TRIGMD((uint32_t)trigger);
#endif /* defined (ADC_CFG0_NORTMD_MASK) */
#endif /* FEATURE_ADC_SUPPORT_INJECTION_MODE */
    baseAddr->CFG0 = tmp;
}

/*!
 * @brief Gets the Resolution Mode configuration
 *
 * This function returns the configured resolution mode for
 * the ADC.
 *
 * @param[in] baseAddr adc base pointer
 * @return the ADC resolution mode. Possible values:
 *        - ADC_RESOLUTION_12BIT : 12-bit resolution mode.
 *        - ADC_RESOLUTION_10BIT : 10-bit resolution mode.
 *        - ADC_RESOLUTION_8BIT  :  8-bit resolution mode.
 *        - ADC_RESOLUTION_6BIT  :  6-bit resolution mode.
 */
static inline adc_resolution_t ADC_GetResolution(const ADC_Type *const baseAddr)
{
    uint32_t tmp = baseAddr->CFG0;
    tmp = (tmp & ADC_CFG0_RES_MASK) >> ADC_CFG0_RES_SHIFT;
    return (adc_resolution_t) tmp;
}

/*!
 * @brief Sets the Resolution Mode configuration
 *
 * This function configures the ADC resolution mode.
 *
 * @param[in] baseAddr adc base pointer
 * @param[in] resolution the adc resolution mode
 *        - ADC_RESOLUTION_12BIT : 12-bit resolution mode.
 *        - ADC_RESOLUTION_10BIT : 10-bit resolution mode.
 *        - ADC_RESOLUTION_8BIT  :  8-bit resolution mode.
 *        - ADC_RESOLUTION_6BIT  :  6-bit resolution mode.
 */
static inline void ADC_SetResolution(ADC_Type *const baseAddr,
                                     const adc_resolution_t resolution)
{
    uint32_t tmp = baseAddr->CFG0;
    tmp &= ~(ADC_CFG0_RES_MASK);
    tmp |= ADC_CFG0_RES(resolution);
    baseAddr->CFG0 = tmp;
}

/*!
 * @brief Gets the DMA Watermark
 *
 * This function returns the watermark setting of ADC FIFO.
 * DMA can be used to transfer completed conversion values
 * from the result registers to RAM without CPU intervention.
 *
 * @param[in] baseAddr adc base pointer
 * @return FIFO watermark value
 */
static inline uint8_t ADC_GetDMAWatermark(const ADC_Type *const baseAddr)
{
    uint32_t tmp = (uint32_t) baseAddr->CFG0;
    tmp = (tmp & ADC_CFG0_WM_MASK) >> ADC_CFG0_WM_SHIFT;
    tmp = ADC_CFG0_WM(tmp);
    return (uint8_t)tmp;
}

/*!
 * @brief Sets the DMA Watermark
 *
 * This function configures the DMA FIFO watermark. DMA can be
 * used to transfer completed conversion values from the
 * result registers to RAM without CPU intervention.
 *
 * @param[in] baseAddr adc base pointer
 * @param[in] state the new DMA watermark value
 */
static inline void ADC_SetDMAWatermark(ADC_Type *const baseAddr,
                                       const uint8_t value)
{
    uint32_t tmp = (uint32_t) baseAddr->CFG0;
    tmp &= (uint32_t) (~(ADC_CFG0_WM_MASK));
    tmp |= ADC_CFG0_WM(value);
    baseAddr->CFG0 = (uint32_t) tmp;
}

/*!
 * @brief Gets the DMA Enable Flag state
 *
 * This function returns the state of the DMA Enable flag.
 * DMA can be used to transfer completed conversion values
 * from the result registers to RAM without CPU intervention.
 *
 * @param[in] baseAddr adc base pointer
 * @return the DMA Enable Flag state
 */
static inline bool ADC_GetDMAEnableFlag(const ADC_Type *const baseAddr)
{
    uint32_t tmp = (uint32_t) baseAddr->CFG0;
    tmp = (tmp & ADC_CFG0_DMAEN_MASK) >> ADC_CFG0_DMAEN_SHIFT;
    return (tmp != 0u) ? true : false;
}

/*!
 * @brief Sets the DMA Enable Flag state
 *
 * This function configures the DMA Enable Flag. DMA can be
 * used to transfer completed conversion values from the
 * result registers to RAM without CPU intervention.
 *
 * @param[in] baseAddr adc base pointer
 * @param[in] state the new DMA Enable Flag state
 */
static inline void ADC_SetDMAEnableFlag(ADC_Type *const baseAddr,
                                        const bool state)
{
    uint32_t tmp = (uint32_t) baseAddr->CFG0;
    tmp &= (uint32_t) (~(ADC_CFG0_DMAEN_MASK));
    tmp |= ADC_CFG0_DMAEN(state ? (uint32_t) 1u : (uint32_t) 0u);
    baseAddr->CFG0 = (uint32_t) tmp;
}

/*! @}*/

/*!
 * @name Hardware Compare.
 * Functions to configure the Hardware Compare feature.
 */
/*! @{*/

/*!
 * @brief Gets the Hardware Compare Enable Flag state
 *
 * This function returns the state of the Hardware Compare
 * Enable Flag. Hardware Compare can be used to check if the
 * ADC result is within or outside of a predefined range.
 *
 * @param[in] baseAddr adc base pointer
 * @param[in] wdgChannel adc wdg channel
 * @return the Hardware Compare Enable Flag state
 */
static inline bool ADC_GetHwCompareEnableFlag(const ADC_Type *const baseAddr, const uint8_t wdgChannel)
{
    uint32_t tmp = (uint32_t) baseAddr->WDCTRL;
    /* Channel Enable Bit Shift */
    /* Channel 0: 7u, Channel 1: 15u, Channel 2: 23u, Channel 3: 31u */
    uint8_t channelEnableBitShift = (uint8_t) (((wdgChannel + 1u) << 3U) - 1u);
    tmp = (tmp >> channelEnableBitShift) & 0x01U;
    return (tmp != 0u) ? true : false;
}

/*!
 * @brief Sets the Hardware Compare Enable Flag state
 *
 * This functions configures the Hardware Compare Enable Flag.
 * Hardware Compare can be used to check if the ADC result
 * is within or outside of a predefined range.
 *
 * @param[in] baseAddr adc base pointer
 * @param[in] wdgChannel adc wdg channel
 * @param[in] state the new Hardware Compare Enable Flag state
 */
static inline void ADC_SetHwCompareEnableFlag(ADC_Type *const baseAddr,
                                              const bool state, const uint8_t wdgChannel)
{
    uint32_t tmp = (uint32_t) baseAddr->WDCTRL;
    /* Channel Enable Bit Shift */
    /* Channel 0: 7u, Channel 1: 15u, Channel 2: 23u, Channel 3: 31u */
    uint8_t channelEnableBitShift = (uint8_t) ((wdgChannel << 3U) + 7u);
    tmp &= (~(((uint32_t)1U << channelEnableBitShift)));
    tmp |= (state ? (uint32_t) 1u : (uint32_t) 0u) << channelEnableBitShift;
    baseAddr->WDCTRL = (uint32_t) tmp;
}

/*!
 * @brief Gets the Hardware Compare All Channel Enable Flag state
 *
 * This function returns the Hardware Compare All Channel
 * Enable Flag. Using this feature, the ADC can be configured
 * to enable hardware compare on all channels.
 *
 * @param[in] baseAddr adc base pointer
 * @return the Hardware Compare All Channel Enable Flag state
 */
static inline bool ADC_GetHwCompareAllEnableFlag(const ADC_Type *const baseAddr)
{
    uint32_t tmp = (uint32_t) baseAddr->WDCTRL;
#if defined(FEATURE_ADC_WDG_CHANNEL_COUNT) && (FEATURE_ADC_WDG_CHANNEL_COUNT > 1)
    tmp = (tmp & ADC_WDCTRL_WD0SGL_MASK) >> ADC_WDCTRL_WD0SGL_SHIFT;
#else
    tmp = (tmp & ADC_WDCTRL_WDSGL_MASK) >> ADC_WDCTRL_WDSGL_SHIFT;
#endif /* (FEATURE_ADC_WDG_CHANNEL_COUNT > 1) */
    return (tmp != 0u) ? false : true;
}

/*!
 * @brief Sets the Hardware Compare All Channel Enable Flag state
 *
 * This function configures the Hardware Compare All Channel
 * Enable Flag. Using this feature, the ADC can be configured
 * to enable hardware compare on all channels.
 *
 * @param[in] baseAddr adc base pointer
 * @param[in] state the new Hardware Compare Greater Than Enable Flag state
 */
static inline void ADC_SetHwCompareAllEnableFlag(ADC_Type *const baseAddr,
                                                 const bool state)
{
    uint32_t tmp = (uint32_t) baseAddr->WDCTRL;
#if defined(FEATURE_ADC_WDG_CHANNEL_COUNT) && (FEATURE_ADC_WDG_CHANNEL_COUNT > 1)
    tmp &= (uint32_t) (~(ADC_WDCTRL_WD0SGL_MASK));
    tmp |= ADC_WDCTRL_WD0SGL(state ? (uint32_t) 0u : (uint32_t) 1u);
#else
    tmp &= (uint32_t) (~(ADC_WDCTRL_WDSGL_MASK));
    tmp |= ADC_WDCTRL_WDSGL(state ? (uint32_t) 0u : (uint32_t) 1u);
#endif /* (FEATURE_ADC_WDG_CHANNEL_COUNT > 1) */
    baseAddr->WDCTRL = (uint32_t) tmp;
}

/*!
 * @brief Gets the Hardware Compare Channel
 *
 * This function returns the channel index of the Hardware Compare.
 *
 * @param[in] baseAddr adc base pointer
 * @param[in] wdgChannel adc watch dog channel
 * @return the Hardware Compare Compare Channel
 */
static inline adc_inputchannel_t ADC_GetHwCompareChannel(const ADC_Type *const baseAddr, const uint8_t wdgChannel)
{
    uint32_t tmp = (uint32_t) baseAddr->WDCTRL;
    /* Channel Compare Bit Shift */
    /* Channel 0: 0u, Channel 1: 8u, Channel 2: 16u, Channel 3: 24u */
    uint8_t channelCmpValBitShift = (uint8_t) ((wdgChannel + 1u) << 3u);
    tmp = (tmp >> channelCmpValBitShift) & 0x3FU;
    return (adc_inputchannel_t)tmp;
}

/*!
 * @brief Sets the Hardware Compare Channel
 *
 * This function configures the channel index of the Hardware Compare.
 *
 * @param[in] baseAddr adc base pointer
 * @param[in] channel adc input channel
 * @param[in] wdgChannel adc watch dog channel
 * @param[in] state the new Hardware Compare channel
 */
static inline void ADC_SetHwCompareRangeEnableFlag(ADC_Type *const baseAddr,
                                                   const adc_inputchannel_t channel,
                                                   const uint8_t wdgChannel)
{
    uint32_t tmp = baseAddr->WDCTRL;
    /* Channel Compare Bit Shift */
    /* Channel 0: 0u, Channel 1: 8u, Channel 2: 16u, Channel 3: 24u */
    uint8_t channelCmpValBitShift = (uint8_t) (wdgChannel << 3);
    tmp &= ~((uint32_t)0x3FU << channelCmpValBitShift);
    tmp |= (uint32_t)((uint32_t)channel & 0x3FU) << channelCmpValBitShift;
    baseAddr->WDCTRL = tmp;
}

/*!
 * @brief Gets the Compare Register High value
 *
 * This function returns the value written in the Hardware
 * Compare Register High. This value defines the upper
 * limit for the Hardware Compare Range. This value is always
 * 12-bit resolution value (for lower resolution modes, internal
 * bit shifting will take place).
 *
 * @param[in] baseAddr adc base pointer
 * @param[in] wdgChannel adc watch dog channel
 * @return the Compare Register High value
 */
static inline uint16_t ADC_GetHwCompareCompHighValue(const ADC_Type *const baseAddr, const uint8_t wdgChannel)
{
#if defined(FEATURE_ADC_WDG_CHANNEL_COUNT) && (FEATURE_ADC_WDG_CHANNEL_COUNT > 1)
    uint32_t tmp = (uint32_t) baseAddr->WDTH[wdgChannel];
    tmp = (tmp & ADC_WDTH_HIGH_MASK) >> ADC_WDTH_HIGH_SHIFT;
#else
    (void)wdgChannel;
    uint32_t tmp = (uint32_t) baseAddr->WDTH;
    tmp = (tmp & ADC_WDTH_HIGH_MASK) >> ADC_WDTH_HIGH_SHIFT;
#endif /* (FEATURE_ADC_WDG_CHANNEL_COUNT > 1) */
    return (uint16_t) tmp;
}

/*!
 * @brief Sets the Compare Register High value
 *
 * This function writes a 12-bit value in the Hardware
 * Compare Register High. This value defines the upper 
 * limit for the Hardware Compare Range. This value is always
 * 12-bit resolution (for lower resolution modes, internal
 * bit shifting will take place).
 *
 * @param[in] baseAddr adc base pointer
 * @param[in] value the new Compare Register High value
 * @param[in] wdgChannel adc watch dog channel
 */
static inline void ADC_SetHwCompareCompHighValue(ADC_Type *const baseAddr,
                                                 const uint16_t value,
                                                 const uint8_t wdgChannel)
{
#if defined(FEATURE_ADC_WDG_CHANNEL_COUNT) && (FEATURE_ADC_WDG_CHANNEL_COUNT > 1)
    uint32_t tmp = (uint32_t) baseAddr->WDTH[wdgChannel];
    tmp &= ~ADC_WDTH_HIGH_MASK;
    tmp |= ADC_WDTH_HIGH(value);
    baseAddr->WDTH[wdgChannel] = tmp;
#else
    uint32_t tmp = (uint32_t) baseAddr->WDTH;
    tmp &= ~ADC_WDTH_HIGH_MASK;
    tmp |= ADC_WDTH_HIGH(value);
    baseAddr->WDTH = tmp;
    (void) wdgChannel;
#endif /* (FEATURE_ADC_WDG_CHANNEL_COUNT > 1) */
}

/*!
 * @brief Gets the Compare Register Low value
 *
 * This function returns the value written in the Hardware
 * Compare Register Low. This value defines the lower
 * limit for the Hardware Compare Range. This value is always
 * 12-bit resolution (for lower resolution modes, internal
 * bit shifting will take place).
 *
 * @param[in] baseAddr adc base pointer
 * @param[in] wdgChannel adc watch dog channel
 * @return the Compare Register Low value
 */
static inline uint16_t ADC_GetHwCompareCompLowValue(const ADC_Type *const baseAddr, const uint8_t wdgChannel)
{
#if defined(FEATURE_ADC_WDG_CHANNEL_COUNT) && (FEATURE_ADC_WDG_CHANNEL_COUNT > 1)
    uint32_t tmp = (uint32_t) baseAddr->WDTH[wdgChannel];
    tmp = (tmp & ADC_WDTH_LOW_MASK) >> ADC_WDTH_LOW_SHIFT;
#else
    uint32_t tmp = (uint32_t) baseAddr->WDTH;
    tmp = (tmp & ADC_WDTH_LOW_MASK) >> ADC_WDTH_LOW_SHIFT;
    (void) wdgChannel;
#endif /* (FEATURE_ADC_WDG_CHANNEL_COUNT > 1) */
    return (uint16_t) tmp;

}

/*!
 * @brief Sets the Compare Register Low value
 *
 * This function writes a 12-bit value in the Hardware
 * Compare Register Low. This value defines the lower
 * limit for the Hardware Compare Range. This value is always
 * 12-bit resolution value (for lower resolution modes, internal
 * bit shifting will take place).
 *
 * @param[in] baseAddr adc base pointer
 * @param[in] value the new Compare Register Low value
 * @param[in] wdgChannel adc watch dog channel
 */
static inline void ADC_SetHwCompareCompLowValue(ADC_Type *const baseAddr,
                                                const uint16_t value,
                                                const uint8_t wdgChannel)
{
#if defined(FEATURE_ADC_WDG_CHANNEL_COUNT) && (FEATURE_ADC_WDG_CHANNEL_COUNT > 1)
    uint32_t tmp = (uint32_t) baseAddr->WDTH[wdgChannel];
    tmp &= ~ADC_WDTH_LOW_MASK;
    tmp |= ADC_WDTH_LOW(value);
    baseAddr->WDTH[wdgChannel] = tmp;
#else
    uint32_t tmp = (uint32_t) baseAddr->WDTH;
    tmp &= ~ADC_WDTH_LOW_MASK;
    tmp |= ADC_WDTH_LOW(value);
    baseAddr->WDTH = tmp;
    (void) wdgChannel;
#endif /* (FEATURE_ADC_WDG_CHANNEL_COUNT > 1) */
}

/*! @}*/

/*!
 * @name Sequence Config.
 * Functions to configure the ADC convert sequence feature.
 */
/*! @{*/

/*!
 * @brief Gets the inject sequence channel ID by sequence index
 *
 * This function returns the inject channel ID of given sequence index.
 *
 * @param[in] baseAddr adc base pointer
 * @param[in] idx Sequence index
 * @return the inject Channel ID of given sequence index
 */
static inline adc_inputchannel_t ADC_GetInjectChannel(const ADC_Type *const baseAddr,
                                                       uint8_t idx)
{
#if defined(FEATURE_ADC_SUPPORT_INJECTION_MODE) && (FEATURE_ADC_SUPPORT_INJECTION_MODE > 0)
    uint8_t tmp = (uint8_t) baseAddr->INJCH[idx];
    tmp = (tmp & ADC_INJCH_INJCH_MASK) >> ADC_INJCH_INJCH_SHIFT;
    return (adc_inputchannel_t) tmp;
#else
    (void) baseAddr;
    (void) idx;
    return (adc_inputchannel_t) 0u;
#endif /* FEATURE_ADC_SUPPORT_INJECTION_MODE */
}

/*!
 * @brief Gets the sequence channel ID by sequence index
 *
 * This function returns the channel ID of given sequence index.
 *
 * @param[in] baseAddr adc base pointer
 * @param[in] idx Sequence index
 * @return the Channel ID of given sequence index
 */
static inline adc_inputchannel_t ADC_GetSequeceChannel(const ADC_Type *const baseAddr,
                                                       uint8_t idx)
{
    uint8_t tmp = (uint8_t) baseAddr->CHSEL[idx];
    tmp = (tmp & ADC_CHSEL_CHSEL_MASK) >> ADC_CHSEL_CHSEL_SHIFT;
    return (adc_inputchannel_t) tmp;
}

/*!
 * @brief Sets the inject sequence channel ID by sequence index
 *
 * This function set the channel ID of given sequence index.
 *
 * @param[in] baseAddr adc base pointer
 * @param[in] idx Sequence index
 * @param[in] channel channel id
 */
static inline void ADC_SetInjectChannel(ADC_Type *const baseAddr,
                                         uint8_t idx,
                                         adc_inputchannel_t channel)
{
#if defined(FEATURE_ADC_SUPPORT_INJECTION_MODE) && (FEATURE_ADC_SUPPORT_INJECTION_MODE > 0)
    uint8_t tmp = (uint8_t) baseAddr->INJCH[idx];
    tmp &= ~ADC_INJCH_INJCH_MASK;
    tmp |= ADC_INJCH_INJCH(channel);
    baseAddr->INJCH[idx] = tmp;
#else
    (void) baseAddr;
    (void) idx;
    (void) channel;
#endif /* FEATURE_ADC_SUPPORT_INJECTION_MODE */
}

/*!
 * @brief Sets the sequence channel ID by sequence index
 *
 * This function set the channel ID of given sequence index.
 *
 * @param[in] baseAddr adc base pointer
 * @param[in] idx Sequence index
 * @param[in] channel channel id
 */
static inline void ADC_SetSequeceChannel(ADC_Type *const baseAddr,
                                         uint8_t idx,
                                         adc_inputchannel_t channel)
{
    uint8_t tmp = (uint8_t) baseAddr->CHSEL[idx];
    tmp &= ~ADC_CHSEL_CHSEL_MASK;
    tmp |= (uint8_t)ADC_CHSEL_CHSEL(channel);
    baseAddr->CHSEL[idx] = tmp;
}

/*!
 * @brief Gets the inject sequence total channel count
 *
 * This function returns the inject channel count of convert sequence.
 *
 * @param[in] baseAddr adc base pointer
 * @return the inject Channel counts of convert sequence
 */
static inline uint8_t ADC_GetInjectTotalChannel(const ADC_Type *const baseAddr)
{
#if defined(FEATURE_ADC_SUPPORT_INJECTION_MODE) && (FEATURE_ADC_SUPPORT_INJECTION_MODE > 0)
    uint32_t tmp = (uint32_t) baseAddr->CFG0;
    tmp = (tmp & ADC_CFG0_INJSEQ_MASK) >> ADC_CFG0_INJSEQ_SHIFT;
    return (uint8_t) tmp + 1;
#else
    (void) baseAddr;
    return 0;
#endif /* FEATURE_ADC_SUPPORT_INJECTION_MODE */
}

/*!
 * @brief Gets the sequence total channel count
 *
 * This function returns the channel count of convert sequence.
 *
 * @param[in] baseAddr adc base pointer
 * @return the Channel counts of convert sequence
 */
static inline uint8_t ADC_GetSequeceTotalChannel(const ADC_Type *const baseAddr)
{
    uint32_t tmp = (uint32_t) baseAddr->CFG0;
    tmp = (tmp & ADC_CFG0_SEQLEN_MASK) >> ADC_CFG0_SEQLEN_SHIFT;
    return (uint8_t) (tmp + 1u);
}

/*!
 * @brief Sets the inject sequence total channel count
 *
 * This function sets the channel count of convert sequence.
 *
 * @param[in] baseAddr adc base pointer
 * @param[in] value Channel counts of inject convert sequence
 */
static inline void ADC_SetInjectTotalChannel(ADC_Type *const baseAddr, uint8_t value)
{
#if defined(FEATURE_ADC_SUPPORT_INJECTION_MODE) && (FEATURE_ADC_SUPPORT_INJECTION_MODE > 0)
    uint32_t tmp = (uint32_t) baseAddr->CFG0;
    tmp &= ~ADC_CFG0_INJSEQ_MASK;
    tmp |= ADC_CFG0_INJSEQ(value - 1);
    baseAddr->CFG0 = tmp;
#else
    (void) baseAddr;
    (void) value;
#endif /* FEATURE_ADC_SUPPORT_INJECTION_MODE */
}

/*!
 * @brief Sets the sequence total channel count
 *
 * This function sets the channel count of convert sequence.
 *
 * @param[in] baseAddr adc base pointer
 * @param[in] value Channel counts of convert sequence
 */
static inline void ADC_SetSequeceTotalChannel(ADC_Type *const baseAddr, uint8_t value)
{
    uint32_t tmp = (uint32_t) baseAddr->CFG0;
    tmp &= ~ADC_CFG0_SEQLEN_MASK;
    tmp |= ADC_CFG0_SEQLEN((uint32_t)value - 1u);
    baseAddr->CFG0 = tmp;
}

/*!
 * @brief Gets the Sequence mode state
 *
 * This functions returns the state of the Sequence Conversion
 * Mode. 
 *
 * @param[in] baseAddr adc base pointer
 * @return the Sequence Mode state
 */
static inline adc_sequence_mode_t ADC_GetSequenceMode(const ADC_Type *const baseAddr)
{
    adc_sequence_mode_t sequenceMode;
#ifdef CPU_YTM32B1ME0
    if ((ADC_CFG0_CONT_MASK & (baseAddr->CFG0)) != 0u)
    {
        sequenceMode = ADC_CONV_CONTINUOUS;
    } else if ((ADC_CFG0_DISCEN_MASK & (baseAddr->CFG0)) != 0u)
    {
        sequenceMode = ADC_CONV_STEP;
    } else
    {
        sequenceMode = ADC_CONV_LOOP;
    }
#else
#ifdef ADC_CFG0_MODE_MASK
    if ((((ADC_CFG0_MODE_MASK & (baseAddr->CFG0)) >> ADC_CFG0_MODE_SHIFT) & 0x01u) != 0u)
    {
        sequenceMode = ADC_CONV_CONTINUOUS;
    } else if ((((ADC_CFG0_MODE_MASK & (baseAddr->CFG0)) >> ADC_CFG0_MODE_SHIFT) & 0x02u) != 0u)
    {
        sequenceMode = ADC_CONV_STEP;
    } else
    {
        sequenceMode = ADC_CONV_LOOP;
    }
#else
    /* Only for YTM32B1LE0x */
    if ((((ADC_CFG0_CONVMD_MASK & (baseAddr->CFG0)) >> ADC_CFG0_CONVMD_SHIFT) & 0x01u) != 0u)
    {
        sequenceMode = ADC_CONV_CONTINUOUS;
    } else if ((((ADC_CFG0_CONVMD_MASK & (baseAddr->CFG0)) >> ADC_CFG0_CONVMD_SHIFT) & 0x02u) != 0u)
    {
        sequenceMode = ADC_CONV_STEP;
    } else
    {
        sequenceMode = ADC_CONV_LOOP;
    }
#endif /* ADC_CFG0_MODE_MASK */
#endif /* CPU_YTM32B1ME0 */

    return sequenceMode;
}

/*!
 * @brief Sets the Sequence Conversion Mode
 *
 * This function configures the Sequence Conversion.
 * 
 * @param[in] baseAddr adc base pointer
 * @param[in] state the new Sequence Conversion Mode
 */
static inline void ADC_SetSequenceMode(ADC_Type *const baseAddr,
                                       adc_sequence_mode_t sequenceMode)
{
#ifdef CPU_YTM32B1ME0
    switch (sequenceMode)
    {
        case ADC_CONV_CONTINUOUS:
            baseAddr->CFG0 |= (ADC_CFG0_CONT_MASK);
            baseAddr->CFG0 &= ~(ADC_CFG0_DISCEN_MASK);
            break;
        case ADC_CONV_STEP:
            baseAddr->CFG0 &= ~(ADC_CFG0_CONT_MASK);
            baseAddr->CFG0 |= (ADC_CFG0_DISCEN_MASK);
            break;
        case ADC_CONV_LOOP:
            baseAddr->CFG0 &= ~(ADC_CFG0_CONT_MASK);
            baseAddr->CFG0 &= ~(ADC_CFG0_DISCEN_MASK);
            break;
        default:
            (void) baseAddr;
            break;
    }
#else
#ifdef ADC_CFG0_MODE_MASK
    switch (sequenceMode)
    {
        case ADC_CONV_CONTINUOUS:
            baseAddr->CFG0 |= (ADC_CFG0_MODE_MASK);
            baseAddr->CFG0 &= ~(ADC_CFG0_MODE_MASK << 1);
            break;
        case ADC_CONV_STEP:
            baseAddr->CFG0 &= ~(ADC_CFG0_MODE_MASK);
            baseAddr->CFG0 |= (ADC_CFG0_MODE_MASK << 1);
            break;
        case ADC_CONV_LOOP:
            baseAddr->CFG0 &= ~(ADC_CFG0_MODE_MASK);
            baseAddr->CFG0 &= ~(ADC_CFG0_MODE_MASK << 1);
            break;
        default:
            (void) baseAddr;
            break;
    }
#else
    /* Only for YTM32B1LE0x */
    switch (sequenceMode)
    {
        case ADC_CONV_CONTINUOUS:
            baseAddr->CFG0 |= (ADC_CFG0_CONVMD_MASK);
            baseAddr->CFG0 &= ~(ADC_CFG0_CONVMD_MASK << 1);
            break;
        case ADC_CONV_STEP:
            baseAddr->CFG0 &= ~(ADC_CFG0_CONVMD_MASK);
            baseAddr->CFG0 |= (ADC_CFG0_CONVMD_MASK << 1);
            break;
        case ADC_CONV_LOOP:
            baseAddr->CFG0 &= ~(ADC_CFG0_CONVMD_MASK);
            baseAddr->CFG0 &= ~(ADC_CFG0_CONVMD_MASK << 1);
            break;
        default:
            (void) baseAddr;
            break;
    }
#endif /* ADC_CFG0_MODE_MASK */
#endif /* CPU_YTM32B1ME0 */
}

/*! @}*/

/*!
 * @brief Gets the End Of Convert Interrupt Enable Flag state
 *
 * This function returns the state of the End Of Convert 
 * Interrupt Enable flag.
 *
 * @param[in] baseAddr adc base pointer
 * @return the End Of Convert Interrupt Enable Flag state
 */
static inline bool ADC_GetEocIntEnableFlag(const ADC_Type *const baseAddr)
{
    uint32_t tmp = (uint32_t)baseAddr->INTE;
    tmp = (tmp & ADC_INTE_EOCIE_MASK) >> ADC_INTE_EOCIE_SHIFT;
    return (tmp != 0u) ? true : false;
}

/*!
 * @brief Sets the Analog Watchdog Interrupt Enable Flag state
 *
 * This function configures the Analog Watchdog Interrupt Enable
 * Flag.
 *
 * @param[in] baseAddr adc base pointer
 * @param[in] state the Analog Watchdog Interrupt Enable Flag state
 */
static inline void ADC_SetAwdIntEnableFlag(ADC_Type *const baseAddr,
                                           const bool state)
{
    uint32_t tmp = (uint32_t) baseAddr->INTE;
    tmp &= (uint32_t) (~(ADC_INTE_WDIE_MASK));
    tmp |= ADC_INTE_WDIE(state ? (uint32_t) 1u : (uint32_t) 0u);
    baseAddr->INTE = (uint32_t) tmp;
}

/*!
 * @brief Sets the Analog Watchdog Effective Mode
 *
 * This function configures the Analog Watchdog Effective Mode
 * Flag.
 *
 * @param[in] baseAddr adc base pointer
 * @param[in] state the Analog Watchdog Effective Mode state
 * @param[in] wdgChannel adc watch dog channel
 */
static inline void ADC_SetAwdEffectiveMode(ADC_Type *const baseAddr,
                                           const bool effective_mode,
                                           const uint8_t wdgChannel)
{
#if defined(FEATURE_ADC_WDG_CHANNEL_COUNT) && (FEATURE_ADC_WDG_CHANNEL_COUNT > 1)
    uint32_t tmp = (uint32_t) baseAddr->WDTH[wdgChannel];
    tmp &= (uint32_t) (~(ADC_WDTH_THMD_MASK));
    tmp |= ADC_WDTH_THMD(effective_mode ? (uint32_t) 1u : (uint32_t) 0u);
    baseAddr->WDTH[wdgChannel] = (uint32_t) tmp;
#else
    (void) wdgChannel;
    uint32_t tmp = (uint32_t) baseAddr->WDTH;
    tmp &= (uint32_t) (~(ADC_WDTH_THMD_MASK));
    tmp |= ADC_WDTH_THMD(effective_mode ? (uint32_t) 1u : (uint32_t) 0u);
    baseAddr->WDTH = (uint32_t) tmp;
#endif /* (FEATURE_ADC_WDG_CHANNEL_COUNT > 1) */
}

/*!
 * @brief Gets the Analog Watchdog Interrupt Enable Flag state
 *
 * This function returns the state of the Analog Watchdog 
 * Interrupt Enable flag.
 *
 * @param[in] baseAddr adc base pointer
 * @return the Analog Watchdog Interrupt Enable Flag state
 */
static inline bool ADC_GetAwdIntEnableFlag(const ADC_Type *const baseAddr)
{
    uint32_t tmp = (uint32_t) baseAddr->INTE;
    tmp = (tmp & ADC_INTE_WDIE_MASK) >> ADC_INTE_WDIE_SHIFT;
    return (tmp != 0u) ? true : false;
}

/*!
 * @brief Sets the Overrun Interrupt Enable Flag state
 *
 * This function configures the Overrun Interrupt Enable
 * Flag.
 *
 * @param[in] baseAddr adc base pointer
 * @param[in] state the Overrun Interrupt Enable Flag state
 */
static inline void ADC_SetOvrIntEnableFlag(ADC_Type *const baseAddr,
                                           const bool state)
{
    uint32_t tmp = (uint32_t) baseAddr->INTE;
    tmp &= (uint32_t) (~(ADC_INTE_OVRIE_MASK));
    tmp |= ADC_INTE_OVRIE(state ? (uint32_t) 1u : (uint32_t) 0u);
    baseAddr->INTE = (uint32_t) tmp;
}

/*!
 * @brief Gets the Overrun Interrupt Enable Flag state
 *
 * This function returns the state of the Overrun 
 * Interrupt Enable flag.
 *
 * @param[in] baseAddr adc base pointer
 * @return the Overrun Interrupt Enable Flag state
 */
static inline bool ADC_GetOvrIntEnableFlag(const ADC_Type *const baseAddr)
{
    uint32_t tmp = (uint32_t) baseAddr->INTE;
    tmp = (tmp & ADC_INTE_OVRIE_MASK) >> ADC_INTE_OVRIE_SHIFT;
    return (tmp != 0u) ? true : false;
}

/*!
 * @brief Sets the Ready Interrupt Enable Flag state
 *
 * This function configures the Ready Interrupt Enable
 * Flag.
 *
 * @param[in] baseAddr adc base pointer
 * @param[in] state the Ready Interrupt Enable Flag state
 */
static inline void ADC_SetReadyIntEnableFlag(ADC_Type *const baseAddr,
                                           const bool state)
{
    uint32_t tmp = (uint32_t) baseAddr->INTE;
    tmp &= (uint32_t) (~(ADC_INTE_ADRDYIE_MASK));
    tmp |= ADC_INTE_ADRDYIE(state ? (uint32_t) 1u : (uint32_t) 0u);
    baseAddr->INTE = (uint32_t) tmp;
}

/*!
 * @brief Gets the Ready Interrupt Enable Flag state
 *
 * This function returns the state of the Ready 
 * Interrupt Enable flag.
 *
 * @param[in] baseAddr adc base pointer
 * @return the Ready Interrupt Enable Flag state
 */
static inline bool ADC_GetReadyIntEnableFlag(const ADC_Type *const baseAddr)
{
    uint32_t tmp = (uint32_t) baseAddr->INTE;
    tmp = (tmp & ADC_INTE_ADRDYIE_MASK) >> ADC_INTE_ADRDYIE_SHIFT;
    return (tmp != 0u) ? true : false;
}

/*!
 * @brief Sets the Sample End Interrupt Enable Flag state
 *
 * This function configures the Sample End Interrupt Enable
 * Flag.
 *
 * @param[in] baseAddr adc base pointer
 * @param[in] state the Sample End Interrupt Enable Flag state
 */
static inline void ADC_SetSampEndIntEnableFlag(ADC_Type *const baseAddr,
                                           const bool state)
{
    uint32_t tmp = (uint32_t) baseAddr->INTE;
    tmp &= (uint32_t) (~(ADC_INTE_EOSMPIE_MASK));
    tmp |= ADC_INTE_EOSMPIE(state ? (uint32_t) 1u : (uint32_t) 0u);
    baseAddr->INTE = (uint32_t) tmp;
}

/*!
 * @brief Gets the Sample End Interrupt Enable Flag state
 *
 * This function returns the state of the Sample End 
 * Interrupt Enable flag.
 *
 * @param[in] baseAddr adc base pointer
 * @return the Sample End Interrupt Enable Flag state
 */
static inline bool ADC_GetSampEndIntEnableFlag(const ADC_Type *const baseAddr)
{
    uint32_t tmp = (uint32_t) baseAddr->INTE;
    tmp = (tmp & ADC_INTE_EOSMPIE_MASK) >> ADC_INTE_EOSMPIE_SHIFT;
    return (tmp != 0u) ? true : false;
}

/*!
 * @brief Sets the End Of Convert Interrupt Enable Flag state
 *
 * This function configures the End Of Convert Interrupt Enable
 * Flag.
 *
 * @param[in] baseAddr adc base pointer
 * @param[in] state the End Of Convert Interrupt Enable Flag state
 */
static inline void ADC_SetEocIntEnableFlag(ADC_Type *const baseAddr,
                                           const bool state)
{
    uint32_t tmp = (uint32_t) baseAddr->INTE;
    tmp &= (uint32_t) (~(ADC_INTE_EOCIE_MASK));
    tmp |= ADC_INTE_EOCIE(state ? (uint32_t) 1u : (uint32_t) 0u);
    baseAddr->INTE = (uint32_t) tmp;
}

/*!
 * @brief Gets the End Of Sequence Interrupt Enable Flag state
 *
 * This function returns the state of the End Of Sequence
 * Interrupt Enable flag.
 *
 * @param[in] baseAddr adc base pointer
 * @return the End Of Sequence Interrupt Enable Flag state
 */
static inline bool ADC_GetEoSeqIntEnableFlag(const ADC_Type *const baseAddr)
{
    uint32_t tmp = (uint32_t) baseAddr->INTE;
    tmp = (tmp & ADC_INTE_EOSEQIE_MASK) >> ADC_INTE_EOSEQIE_SHIFT;
    return (tmp != 0u) ? true : false;
}

/*!
 * @brief Sets the inject Convert Interrupt Enable Flag state
 *
 * This function configures the inject Convert Interrupt Enable
 * Flag.
 *
 * @param[in] baseAddr adc base pointer
 * @param[in] state the inject Convert Interrupt Enable Flag state
 */
static inline void ADC_SetInjectIntEnableFlag(ADC_Type *const baseAddr,
                                           const bool state)
{
#if defined(FEATURE_ADC_SUPPORT_INJECTION_MODE) && (FEATURE_ADC_SUPPORT_INJECTION_MODE > 0)
    uint32_t tmp = (uint32_t) baseAddr->INTE;
    tmp &= (uint32_t) (~(ADC_INTE_INJIE_MASK));
    tmp |= ADC_INTE_INJIE(state ? (uint32_t) 1u : (uint32_t) 0u);
    baseAddr->INTE = (uint32_t) tmp;
#else
    (void) baseAddr;
    (void) state;
#endif /* FEATURE_ADC_SUPPORT_INJECTION_MODE */
}

/*!
 * @brief Gets the Inject Sequence Interrupt Enable Flag state
 *
 * This function returns the state of the Inject Sequence
 * Interrupt Enable flag.
 *
 * @param[in] baseAddr adc base pointer
 * @return the Inject Sequence Interrupt Enable Flag state
 */
static inline bool ADC_GetInjectIntEnableFlag(const ADC_Type *const baseAddr)
{
#if defined(FEATURE_ADC_SUPPORT_INJECTION_MODE) && (FEATURE_ADC_SUPPORT_INJECTION_MODE > 0)
    uint32_t tmp = (uint32_t) baseAddr->INTE;
    tmp = (tmp & ADC_INTE_INJIE_MASK) >> ADC_INTE_INJIE_SHIFT;
    return (tmp != 0u) ? true : false;
#else
    (void) baseAddr;
    return true;
#endif /* FEATURE_ADC_SUPPORT_INJECTION_MODE */
}

/*!
 * @brief Sets the inject Convert Error Interrupt Enable Flag state
 *
 * This function configures the inject Convert Error Interrupt Enable
 * Flag.
 *
 * @param[in] baseAddr adc base pointer
 * @param[in] state the inject Convert Error Interrupt Enable Flag state
 */
static inline void ADC_SetInjectErrIntEnableFlag(ADC_Type *const baseAddr,
                                           const bool state)
{
#if defined(FEATURE_ADC_SUPPORT_INJECTION_MODE) && (FEATURE_ADC_SUPPORT_INJECTION_MODE > 0)
    uint32_t tmp = (uint32_t) baseAddr->INTE;
    tmp &= (uint32_t) (~(ADC_INTE_INJERRIE_MASK));
    tmp |= ADC_INTE_INJERRIE(state ? (uint32_t) 1u : (uint32_t) 0u);
    baseAddr->INTE = (uint32_t) tmp;
#else
    (void) baseAddr;
    (void) state;
#endif /* FEATURE_ADC_SUPPORT_INJECTION_MODE */
}

/*!
 * @brief Gets the Inject Sequence Error Interrupt Enable Flag state
 *
 * This function returns the state of the Inject Sequence Error
 * Interrupt Enable flag.
 *
 * @param[in] baseAddr adc base pointer
 * @return the Inject Sequence Error Interrupt Enable Flag state
 */
static inline bool ADC_GetInjectErrIntEnableFlag(const ADC_Type *const baseAddr)
{
#if defined(FEATURE_ADC_SUPPORT_INJECTION_MODE) && (FEATURE_ADC_SUPPORT_INJECTION_MODE > 0)
    uint32_t tmp = (uint32_t) baseAddr->INTE;
    tmp = (tmp & ADC_INTE_INJERRIE_MASK) >> ADC_INTE_INJERRIE_SHIFT;
    return (tmp != 0u) ? true : false;
#else
    (void) baseAddr;
    return true;
#endif /* FEATURE_ADC_SUPPORT_INJECTION_MODE */
}

/*!
 * @brief Sets the End Of Sequence Interrupt Enable Flag state
 *
 * This function configures the End Of Sequence Interrupt Enable
 * Flag.
 *
 * @param[in] baseAddr adc base pointer
 * @param[in] state the End Of Sequence Interrupt Enable Flag state
 */
static inline void ADC_SetEoSeqIntEnableFlag(ADC_Type *const baseAddr,
                                             const bool state)
{
    uint32_t tmp = (uint32_t) baseAddr->INTE;
    tmp &= (uint32_t) (~(ADC_INTE_EOSEQIE_MASK));
    tmp |= ADC_INTE_EOSEQIE(state ? (uint32_t) 1u : (uint32_t) 0u);
    baseAddr->INTE = (uint32_t) tmp;
}

/*!
 * @brief Clear the End Of Conversion Interrupt Flag
 *
 * This function is used to clear EOC flag.
 * Flag.
 *
 * @param[in] baseAddr adc base pointer
 */
static inline void ADC_ClearEocFlagCmd(ADC_Type *const baseAddr)
{
    uint16_t timeout_cnt = 0U;
    /* W1C to coresponding bit in STS because of W1C each */
    baseAddr->STS = ADC_STS_EOC_MASK;
    /* E600005: Wait status clear */
    while((baseAddr->STS & ADC_STS_EOC_MASK) != 0u)
    {
        if(timeout_cnt > ADC_WAIT_TIMEOUT)
        {
            break;
        }
        timeout_cnt++;
    }
}

/*!
 * @brief Clear the End Of Sequence Interrupt Flag
 *
 * This function is used to clear EOSEQ flag.
 * Flag.
 *
 * @param[in] baseAddr adc base pointer
 */
static inline void ADC_ClearEoseqFlagCmd(ADC_Type *const baseAddr)
{
    uint16_t timeout_cnt = 0U;
    /* W1C to coresponding bit in STS because of W1C each */
    baseAddr->STS = (uint32_t) ADC_STS_EOSEQ_MASK;
    /* E600005: Wait status clear */
    while((baseAddr->STS & ADC_STS_EOSEQ_MASK) != 0u)
    {
        if(timeout_cnt > ADC_WAIT_TIMEOUT)
        {
            break;
        }
        timeout_cnt++;
    }
}

/*!
 * @brief Clear the OverRun Interrupt Flag
 *
 * This function is used to clear OVR flag.
 * Flag.
 *
 * @param[in] baseAddr adc base pointer
 */
static inline void ADC_ClearOvrFlagCmd(ADC_Type *const baseAddr)
{
    uint16_t timeout_cnt = 0U;
    /* W1C to coresponding bit in STS because of W1C each */
    baseAddr->STS = (uint32_t) ADC_STS_OVR_MASK;
    /* E600005: Wait status clear */
    while((baseAddr->STS & ADC_STS_OVR_MASK) != 0u)
    {
        if(timeout_cnt > ADC_WAIT_TIMEOUT)
        {
            break;
        }
        timeout_cnt++;
    }
}

/*!
 * @brief Clear the Watchdog Interrupt Flag
 *
 * This function is used to clear WD flag.
 * Flag.
 *
 * @param[in] baseAddr adc base pointer
 * @param[in] wdgChannel adc watch dog channel
 */
static inline void ADC_ClearWdFlagCmd(ADC_Type *const baseAddr, const uint8_t wdgChannel)
{
    uint16_t timeout_cnt = 0U;
    /* W1C to coresponding bit in STS because of W1C each */
#if defined(FEATURE_ADC_WDG_CHANNEL_COUNT) && (FEATURE_ADC_WDG_CHANNEL_COUNT > 1)
    baseAddr->STS = (uint32_t) (ADC_STS_WD0_MASK << wdgChannel);
    /* E600005: Wait status clear */
    while((baseAddr->STS & (ADC_STS_WD0_MASK << wdgChannel)) != 0u)
    {
        if(timeout_cnt > ADC_WAIT_TIMEOUT)
        {
            break;
        }
        timeout_cnt++;
    }
#else
    baseAddr->STS = (uint32_t) ADC_STS_WD_MASK;
    /* E600005: Wait status clear */
    while((baseAddr->STS & ADC_STS_WD_MASK) != 0u)
    {
        if(timeout_cnt > ADC_WAIT_TIMEOUT)
        {
            break;
        }
        timeout_cnt++;
    }
    (void) wdgChannel;
#endif /* (FEATURE_ADC_WDG_CHANNEL_COUNT > 1) */
}

/*!
 * @brief Clear the Ready Interrupt Flag
 *
 * This function is used to clear Ready flag.
 * Flag.
 *
 * @param[in] baseAddr adc base pointer
 */
static inline void ADC_ClearReadyFlagCmd(ADC_Type *const baseAddr)
{
    uint16_t timeout_cnt = 0U;
    /* W1C to coresponding bit in STS because of W1C each */
    baseAddr->STS = (uint32_t) ADC_STS_ADRDY_MASK;
    /* E600005: Wait status clear */
    while((baseAddr->STS & ADC_STS_ADRDY_MASK) != 0u)
    {
        if(timeout_cnt > ADC_WAIT_TIMEOUT)
        {
            break;
        }
        timeout_cnt++;
    }
}

/*!
 * @brief Clear the Sample End Interrupt Flag
 *
 * This function is used to clear Sample End flag.
 * Flag.
 *
 * @param[in] baseAddr adc base pointer
 */
static inline void ADC_ClearSampEndFlagCmd(ADC_Type *const baseAddr)
{
    uint16_t timeout_cnt = 0U;
    /* W1C to coresponding bit in STS because of W1C each */
    baseAddr->STS = (uint32_t) ADC_STS_EOSMP_MASK;
    /* E600005: Wait status clear */
    while((baseAddr->STS & ADC_STS_EOSMP_MASK) != 0u)
    {
        if(timeout_cnt > ADC_WAIT_TIMEOUT)
        {
            break;
        }
        timeout_cnt++;
    }
}

/*!
 * @brief Gets the result from result FIFO
 *
 * This function returns the conversion result from FIFO.
 * Flag.
 *
 * @param[in] baseAddr adc base pointer
 */
static inline uint16_t ADC_ReadFIFO(ADC_Type *const baseAddr)
{
    return (uint16_t)baseAddr->FIFO;
}

/*!
 * @brief Gets the Conversion Complete flag
 *
 * This function returns the state of the Conversion Complete
 * flag for a control channel.
 *
 * @param[in] baseAddr adc base pointer
 */
static inline bool ADC_GetConvCompleteFlag(ADC_Type *const baseAddr)
{
    uint32_t tmp = baseAddr->STS;
    tmp = (tmp & ADC_STS_EOC_MASK) >> ADC_STS_EOC_SHIFT;
    return (tmp != 0u) ? true : false;
}

/*!
 * @brief Stop ADC
 *
 * This function stop the ADC
 *
 * @param[in] baseAddr adc base pointer
 */
static inline void ADC_Stop(ADC_Type *const baseAddr)
{
    if (ADC_GetConvActiveFlag(baseAddr))
    {
        baseAddr->CTRL |= ADC_CTRL_ADSTOP_MASK;
        while ((ADC_CTRL_ADSTOP_MASK & baseAddr->CTRL) != 0u)
        {
            /* wait ADC sequence stop */
        }
    }
}

/*!
 * @brief Start ADC
 *
 * This function start the ADC
 *
 * @param[in] baseAddr adc base pointer
 */
static inline void ADC_Start(ADC_Type *const baseAddr)
{
    uint32_t tmp = baseAddr->CTRL;
    tmp &= (uint32_t) (~(ADC_CTRL_ADSTART_MASK));
    tmp |= ADC_CTRL_ADSTART(1);
    baseAddr->CTRL = (uint32_t) tmp;
}

/*!
 * @brief Disable ADC
 *
 * This function disable ADC and let ADC enter low power mode
 *
 * @param[in] baseAddr adc base pointer
 */
static inline void ADC_Disable(ADC_Type *const baseAddr)
{
    baseAddr->CTRL |= ADC_CTRL_ADDIS_MASK;
    while ((ADC_CTRL_ADDIS_MASK & baseAddr->CTRL) != 0u)
    {
        /* wait ADC diabled */
    }
}

/*!
 * @brief Enable ADC
 *
 * This function enable the ADC
 *
 * @param[in] baseAddr adc base pointer
 */
static inline void ADC_Enable(ADC_Type *const baseAddr)
{
#ifdef ADC_ERRATA_E0002
    /* Workarounds */
    uint32_t adcRegister[8];
    uint8_t chsel[8], i;
    /* Copy all register */
    for(i = 0; i < 8U; i ++)
    {
        adcRegister[i] = *(volatile uint32_t *)((uint32_t)baseAddr + (0x4UL * i));
        chsel[i] = baseAddr->CHSEL[i];
    }
    /* Software reset ADC */
#if (ADC_INSTANCE_COUNT > 1)
    if(ADC0 == baseAddr)
    {
        IPC->CTRL[IPC_ADC0_INDEX] |= IPC_CTRL_SWREN_MASK;
        IPC->CTRL[IPC_ADC0_INDEX] &= ~IPC_CTRL_SWREN_MASK;
    }else
    {
        IPC->CTRL[IPC_ADC1_INDEX] |= IPC_CTRL_SWREN_MASK;
        IPC->CTRL[IPC_ADC1_INDEX] &= ~IPC_CTRL_SWREN_MASK;
    }
#else
    IPC->CTRL[IPC_ADC0_INDEX] |= IPC_CTRL_SWREN_MASK;
    IPC->CTRL[IPC_ADC0_INDEX] &= ~IPC_CTRL_SWREN_MASK;
#endif
    /* Set register value saved */
    for(i = 0; i < 8U; i ++)
    {
        *(volatile uint32_t *)((uint32_t)baseAddr + (0x4UL * i)) = adcRegister[i];
        baseAddr->CHSEL[i] = chsel[i];
    }
#endif
    if (0u == (ADC_CTRL_ADEN_MASK & baseAddr->CTRL))
    {
        baseAddr->CTRL |= ADC_CTRL_ADEN_MASK;
    }
}


#if defined (__cplusplus)
}
#endif


#endif /* ADC_HW_ACCESS_H */
/*******************************************************************************
 * EOF
 ******************************************************************************/
