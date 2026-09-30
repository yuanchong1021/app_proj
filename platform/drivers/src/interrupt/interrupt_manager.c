/*
 * Copyright 2020-2025 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file interrupt_manager.c
 * @version 1.4.0
 */

 /*!
 * @page misra_violations MISRA-C:2012 violations list
 *
 * PRQA S 0305 Rule 11.1: Cast between a pointer to function and an integral type.
 *
 */

#include "interrupt_manager.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*!
 * @brief Counter to manage the nested callings of global disable/enable interrupt.
 */
static int32_t g_interruptDisableCount = 0;

/*******************************************************************************
 * Code
 ******************************************************************************/

/*FUNCTION**********************************************************************
 *
 * Function Name : INT_SYS_InstallHandler
 * Description   : Install an interrupt handler routine for a given IRQ number
 * This function will let application register/replace the interrupt
 * handler for specified IRQ number. IRQ number is different than Vector
 * number. IRQ 0 will start from Vector 16 address. Refer to reference
 * manual for details. Also refer to startup_<CPU>.s file for each chip
 * family to find out the default interrupt handler for each device. This
 * function will convert the IRQ number to vector number by adding 16 to
 * it.
 *
 * Note          : This method is applicable only if interrupt vector is
 *                 copied in RAM, __flash_vector_table__ symbol is used to
 *                 control this from linker options.
 * Implements INT_SYS_InstallHandler_Activity
 *
 *END**************************************************************************/
void INT_SYS_InstallHandler(IRQn_Type irqNumber,
                            isr_t newHandler,
                            isr_t *oldHandler)
{

#if (defined(DEV_ERROR_DETECT) || defined(CUSTOM_DEVASSERT))

    /* Check IRQ number - dev_irqNumber is used to avoid compiler warning */
    int32_t dev_irqNumber = (int32_t)irqNumber;
    DEV_ASSERT(FEATURE_INTERRUPT_IRQ_MIN <= irqNumber);
    DEV_ASSERT(dev_irqNumber <= (int32_t)FEATURE_INTERRUPT_IRQ_MAX);
#ifdef __NO_VECTOR_TABLE_COPY
    DEV_ASSERT(false);
#endif
#endif /*(defined(DEV_ERROR_DETECT) || defined(CUSTOM_DEVASSERT)) */
    
    uint32_t * pVectorRam = (uint32_t *)(SCB->VTOR);
    /* Save the former handler pointer */
    if (oldHandler != (isr_t *) 0)
    {
        *oldHandler = (isr_t)pVectorRam[((int32_t)irqNumber) + 16]; /*PRQA S 0305*/
    }

    /* Set handler into vector table */
    pVectorRam[((int32_t)irqNumber) + 16] = (uint32_t)newHandler; /*PRQA S 0305*/

#if defined(__DCACHE_PRESENT) && __DCACHE_PRESENT
    if ((SCB->CCR & SCB_CCR_DC_Msk) != 0U)
    {
        SCB_CleanDCache();
        SCB_InvalidateICache();
    }
#endif
}

/*FUNCTION**********************************************************************
 *
 * Function Name : INT_SYS_EnableIRQ
 * Description   : Enables an interrupt for a given IRQ number.
 * It calls the system NVIC API to access the interrupt control
 * register and interrupt routing.
 * The input IRQ number does not include the core interrupt, only
 * the peripheral interrupt, from 0 to a maximum supported IRQ.
 * Implements INT_SYS_EnableIRQ_Activity
 *END**************************************************************************/
void INT_SYS_EnableIRQ(IRQn_Type irqNumber)
{
    NVIC_EnableIRQ(irqNumber);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : INT_SYS_DisableIRQ
 * Description   : Disable individual interrupt for a specified IRQ
 * It  calls the system NVIC API to access the interrupt control register
 * and interrupt routing.
 * Implements INT_SYS_DisableIRQ_Activity
 *
 *END**************************************************************************/
void INT_SYS_DisableIRQ(IRQn_Type irqNumber)
{
    NVIC_DisableIRQ(irqNumber);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : INT_SYS_EnableIRQGlobal
 * Description   : Enable system interrupt
 * This function will enable the global interrupt by calling the core API
 * Implements INT_SYS_EnableIRQGlobal_Activity
 *
 *END**************************************************************************/
void INT_SYS_EnableIRQGlobal(void)
{
    /* Check and update */
    if (g_interruptDisableCount > 0)
    {
        g_interruptDisableCount--;

        if (g_interruptDisableCount <= 0)
        {
            /* Enable the global interrupt */
            __enable_irq();
        }
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : INT_SYS_DisableIRQGlobal
 * Description   : Disable system interrupt
 * This function will disable the global interrupt by calling the core API
 * Implements INT_SYS_DisableIRQGlobal_Activity
 *
 *END**************************************************************************/
void INT_SYS_DisableIRQGlobal(void)
{
    /* Disable the global interrupt */
    __disable_irq();

    /* Update counter*/
    g_interruptDisableCount++;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : INT_SYS_SetPriority
 * Description   : Set the priority of an interrupt
 * This function will set the priority of an interrupt.
 * Note: The priority cannot be set for every core interrupt.
 * Implements INT_SYS_SetPriority_Activity
 *
 *END**************************************************************************/
void INT_SYS_SetPriority(IRQn_Type irqNumber, uint8_t priority)
{
    NVIC_SetPriority(irqNumber, priority);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : INT_SYS_GetPriority
 * Description   : Get the priority of an interrupt
 * This function will get the priority of an interrupt.
 * Note: The priority cannot be obtained for every core interrupt.
 * Implements INT_SYS_GetPriority_Activity
 *
 *END**************************************************************************/
uint8_t INT_SYS_GetPriority(IRQn_Type irqNumber)
{
    return (uint8_t)NVIC_GetPriority(irqNumber);
}

#if FEATURE_INTERRUPT_HAS_PENDING_STATE
/*FUNCTION**********************************************************************
 *
 * Function Name : INT_SYS_ClearPending
 * Description   : Clear Pending Interrupt
 * This function clears the pending bit of a peripheral interrupt
 * or a directed interrupt to this CPU (if available).
 * Implements INT_SYS_ClearPending_Activity
 *
 *END**************************************************************************/
void INT_SYS_ClearPending(IRQn_Type irqNumber)
{
    NVIC_ClearPendingIRQ(irqNumber);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : INT_SYS_SetPending
 * Description   : Set Pending Interrupt
 * This function configures the pending bit of a peripheral interrupt.
 * Implements INT_SYS_SetPending_Activity
 *
 *END**************************************************************************/
void INT_SYS_SetPending(IRQn_Type irqNumber)
{
    NVIC_SetPendingIRQ(irqNumber);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : INT_SYS_GetPending
 * Description   : Get Pending Interrupt
 * This function gets the pending bit of a peripheral interrupt or a directed
 * interrupt to this CPU (if available).
 * Implements INT_SYS_GetPending_Activity
 *
 *END**************************************************************************/
uint32_t INT_SYS_GetPending(IRQn_Type irqNumber)
{
    return NVIC_GetPendingIRQ(irqNumber);
}
#endif /* FEATURE_INTERRUPT_HAS_PENDING_STATE */

#if FEATURE_INTERRUPT_HAS_ACTIVE_STATE
/*FUNCTION**********************************************************************
 *
 * Function Name : INT_SYS_GetActive
 * Description   : Get active state of a peripheral interrupt
 * This function gets the active state of a peripheral interrupt.
 * Implements INT_SYS_GetActive_Activity
 *
 *END**************************************************************************/
uint32_t INT_SYS_GetActive(IRQn_Type irqNumber)
{
    return NVIC_GetActive(irqNumber);
}
#endif /* FEATURE_INTERRUPT_HAS_ACTIVE_STATE */

/*! @}*/
/*******************************************************************************
 * EOF
 ******************************************************************************/
