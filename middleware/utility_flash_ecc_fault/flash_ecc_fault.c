/*
 * Copyright 2020-2025 Yuntu Microelectronics co.,ltd
 * All rights reserved.
 *
 * YUNTU Confidential. This software is owned or controlled by YUNTU and may
 * only be used strictly in accordance with the applicable license terms. By expressly
 * accepting such terms or by downloading, installing, activating and/or otherwise
 * using the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms. If you do not agree to be
 * bound by the applicable license terms, then you may not retain, install,
 * activate or otherwise use the software. The production use license in
 * Section 2.3 is expressly granted for this software.
 */
/*!
 * @file flash_ecc_fault.c
 */

#include "flash_ecc_fault.h"
#include "interrupt_manager.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* Pointer to runtime state structure.*/
static flash_ecc_fault_state_t * s_FlashEccFaultStatePtr = NULL;

/*******************************************************************************
 * Functions
 ******************************************************************************/
void FLASH_ECC_Fault_Handler_C(hw_stackframe_t *fault_stack);
void FLASH_ECC_Fault_Handler(void) __attribute__( ( naked ) );

void FLASH_ECC_Fault_Handler(void)
{
    __asm volatile(
        "MOVS R0, #4                        \n" // Load immediate value 4 into R0 to test the LR bit.
        "MOV R1, LR                         \n" // Copy Link Register (LR) into R1.
        "TST R1, R0                         \n" // Test if bit2 of LR is set.
        "BNE get_psp                        \n" // If bit2 is set, use Process Stack Pointer (PSP).
        "MRS R0, MSP                        \n" // Otherwise, load Main Stack Pointer (MSP) into R0.
        "B c_handler                        \n" // Branch to call the C fault handler.
    "get_psp:                               \n"
        "MRS R0, PSP                        \n" // Load Process Stack Pointer (PSP) into R0.
    "c_handler:                             \n"
        "LDR R2, =FLASH_ECC_Fault_Handler_C \n" // Load to FLASH_ECC_Fault_Handler_C to R2
        "BX  R2                             \n" // jump address in R2
    );
}

/**
 * @brief Flash ECC fault C handler.
 *
 * @return void
 */
void FLASH_ECC_Fault_Handler_C(hw_stackframe_t *fault_stack)
{
    DEV_ASSERT(s_FlashEccFaultStatePtr != NULL);

    uint16_t opcode = *((uint16_t *)(fault_stack->pc));
    uint8_t instrSize = 0;

    /* Determine the instruction size based on the opcode. */
    if ((opcode & 0xE800) == 0xE800 ||  // 32-bit instruction check (pattern 11101x...)
        (opcode & 0xF000) == 0xF000 ||  // 32-bit instruction check (pattern 11110x...)
        (opcode & 0xF800) == 0xF800)    // 32-bit instruction check (pattern 11111x...)
    {
        instrSize = 4;
    }
    else
    {
        instrSize = 2;
    }

    /* If the Flash ECC unrecovery error flag is set, skip the faulting instruction. */
    if (EFM->STS & EFM_STS_UNRECOVERR_MASK)
    {
        fault_stack->pc += instrSize;
#ifdef SCB_SHCSR_BUSFAULTENA_Msk
        SCB->CFSR = SCB_CFSR_BUSFAULTSR_Msk | SCB_CFSR_PRECISERR_Msk;
#endif /* SCB_SHCSR_BUSFAULTENA_Msk */

        if (s_FlashEccFaultStatePtr->eccCallback != NULL)
        {
            s_FlashEccFaultStatePtr->eccCallback();
        }
    }
#ifdef EFM_STS_CI_UNRECOVERR_MASK
    else if (EFM->STS & EFM_STS_CI_UNRECOVERR_MASK)
    {
        fault_stack->pc += instrSize;
        SCB->CFSR = SCB_CFSR_BUSFAULTSR_Msk | SCB_CFSR_PRECISERR_Msk;

        if (s_FlashEccFaultStatePtr->eccCallback != NULL)
        {
            s_FlashEccFaultStatePtr->eccCallback();
        }
    }
#endif /* EFM_STS_CI_UNRECOVERR_MASK */
    else
    {
        if(s_FlashEccFaultStatePtr->otherFaultCallback != NULL)
        {
            s_FlashEccFaultStatePtr->otherFaultCallback(fault_stack);
        }
    }
}


/**
 * @brief Flash ECC fault handler initialization.
 *
 * This function initializes the Flash ECC fault handler by setting up the
 * necessary parameters and installing the fault handler.
 *
 * @param userConfigPtr Pointer to the user configuration structure.
 * @param state Pointer to the flash ECC fault state structure.
 *
 * @return void
 */
void FLASH_ECC_Fault_Init(const flash_ecc_fault_config_t * userConfigPtr, flash_ecc_fault_state_t * state)
{
    /* Check the parameters */
    DEV_ASSERT(userConfigPtr != NULL);
    DEV_ASSERT(state != NULL);

    /* Set the state pointer */
    s_FlashEccFaultStatePtr = state;

    /* Set the callback function */
    state->eccCallback = userConfigPtr->eccCallback;
    state->otherFaultCallback = userConfigPtr->otherFaultCallback;

    /* Use FLASH_ECC_Fault_Handler to instead of HardFault_Handler */
    INT_SYS_InstallHandler(HardFault_IRQn, FLASH_ECC_Fault_Handler, NULL);
}