/*
 * Copyright (c) 2021-2025 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file system_YTM32B1MC0.c
 * @version 1.4.0
 */

/*!
 * @page misra_violations MISRA-C:2012 violations list
 *
 * PRQA S 2016 Rule 16.4: This 'switch' statement 'default' clause is empty.
 *
 * PRQA S 1533 Rule 8.9: The object '%1s' is only referenced by function '%2s'. 
 *
 */

#include "device_registers.h"
#include "system_YTM32B1MC0.h"

/* ----------------------------------------------------------------------------
   -- Core clock
   ---------------------------------------------------------------------------- */

uint32_t SystemCoreClock = DEFAULT_SYSTEM_CLOCK; /*PRQA S 1533*/

/*FUNCTION**********************************************************************
 *
 * Function Name : SystemInit
 * Description   : This function disables the watchdog, enables FPU
 * and the power mode protection if the corresponding feature macro
 * is enabled. SystemInit is called from startup_device file.
 *
 * Implements    : SystemInit_Activity
 *END**************************************************************************/
void SystemInit(void)
{
/**************************************************************************/
                      /* FPU ENABLE*/
/**************************************************************************/
#ifdef ENABLE_FPU
  /* Enable CP10 and CP11 coprocessors */
  SCB->CPACR |= (3UL << 20 | 3UL << 22);
#endif /* ENABLE_FPU */
/**************************************************************************/
/* EFM CONTROL */
/**************************************************************************/
    EFM->CTRL |= EFM_CTRL_LPEN_MASK | EFM_CTRL_PREFETCH_EN_MASK;
/**************************************************************************/
/* RCU CONTROL */
/**************************************************************************/
    CIM->CTRL |= CIM_CTRL_LOCKUPEN_MASK;
/**************************************************************************/
/* WDOG DISABLE*/
/**************************************************************************/
#if (DISABLE_WDOG)
    WDG0->SVCR = 0xB631;
    WDG0->SVCR = 0xC278;
    WDG0->CR &= ~WDG_CR_EN_MASK;
#endif /* (DISABLE_WDOG) */
}

/*FUNCTION**********************************************************************
 *
 * Function Name : SystemCoreClockUpdate
 * Description   : This function must be called whenever the core clock is changed
 * during program execution. It evaluates the clock register settings and calculates
 * the current core clock.
 *
 * Implements    : SystemCoreClockUpdate_Activity
 *END**************************************************************************/
void SystemCoreClockUpdate(void)
{
#if defined(FEATURE_SCU_SUPPORT_PLL) && (FEATURE_SCU_SUPPORT_PLL)
    uint32_t refClkFreq;
    uint32_t feedbackDiv;
    uint32_t referenceDiv;
#endif
    switch ((SCU->STS & SCU_STS_CLKST_MASK) >> SCU_STS_CLKST_SHIFT)
    {
        case 0x0:
            /* FIRC */
            SystemCoreClock = FEATURE_SCU_FIRC_FREQ;
            break;
#if defined(FEATURE_SCU_SUPPORT_PLL) && (FEATURE_SCU_SUPPORT_PLL)
        case 0x1:
            /* PLL */
            if (SCU_PLL_CTRL_REFCLKSRCSEL_MASK == (SCU->PLL_CTRL & SCU_PLL_CTRL_REFCLKSRCSEL_MASK))
            {
                /* Select FIRC as PLL reference clock */
                refClkFreq = FEATURE_SCU_FIRC_FREQ;
            }
            else
            {
                /* Select FXOSC as PLL reference clock */
                refClkFreq = FEATURE_SCU_FXOSC_FREQ;
            }
            /* Fout = (Fref * Ndiv) / ( 2 * Npre) */
            feedbackDiv = ((SCU->PLL_CTRL & SCU_PLL_CTRL_FBDIV_MASK) >> SCU_PLL_CTRL_FBDIV_SHIFT) + 1U;
            referenceDiv = ((SCU->PLL_CTRL & SCU_PLL_CTRL_REFDIV_MASK) >> SCU_PLL_CTRL_REFDIV_SHIFT) + 1U;
            SystemCoreClock = ((refClkFreq * feedbackDiv) / referenceDiv) >> 1;
            break;
#endif /* FEATURE_SCU_SUPPORT_PLL */
        case 0x2:
            /* FXOSC */
            SystemCoreClock = FEATURE_SCU_FXOSC_FREQ;
            break;
        case 0x3:
            /* SIRC */
            SystemCoreClock = FEATURE_SCU_SIRC_FREQ;
            break;
        default: /*PRQA S 2016*/
            break;
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : SystemSoftwareReset
 * Description   : This function is used to initiate a system reset
 *
 * Implements    : SystemSoftwareReset_Activity
 *END**************************************************************************/
void SystemSoftwareReset(void)
{
    NVIC_SystemReset();
}

/*FUNCTION**********************************************************************
 *
 * Function Name : SystemGetUid
 * Description   : This function is only used to get chip unit ID from reserved flash area.
 *
 * Implements    : SystemGetUid_Activity
 *END**************************************************************************/
void SystemGetUid(uint32_t *uidh, uint32_t *uidl)
{
    /* UID store in FLASH */
    *uidh =  ((((uint32_t)(*(uint8_t*)0x10013F28) & 0xFU) <<  0U)
            | (((uint32_t)(*(uint8_t*)0x10013F29) & 0xFU) <<  4U)
            | (((uint32_t)(*(uint8_t*)0x10013F2A) & 0xFU) <<  8U)
            | (((uint32_t)(*(uint8_t*)0x10013F2B) & 0xFU) << 12U)
            | (((uint32_t)(*(uint8_t*)0x10013F2C) & 0xFU) << 16U)
            | (((uint32_t)(*(uint8_t*)0x10013F2D) & 0xFU) << 20U));
    *uidl =  ((((uint32_t)(*(uint8_t*)0x10013F40) & 0xFFU) <<  0U)
            | (((uint32_t)(*(uint8_t*)0x10013F38) & 0xFFU) <<  8U)
            | (((uint32_t)(*(uint8_t*)0x10013F30) & 0xFFU) << 16U)
            | (((uint32_t)(*(uint8_t*)0x10013F58) & 0xFFU) << 24U));
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
