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
 * @file flash_ecc_fault.h
 *
 */

#ifndef __FLASH_ECC_FAULT_H__
#define __FLASH_ECC_FAULT_H__

#include <stdbool.h>
#include <stddef.h>
#include "status.h"
#include "device_registers.h"


/*******************************************************************************
 * Definitions
 ******************************************************************************/
typedef struct
{
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t lr;
    uint32_t pc;
    uint32_t psr;
} hw_stackframe_t;

/*! @brief Call back function pointer data type for flash unrecovery ecc error */
typedef void (*flash_ecc_callback_t)(void);

/*! @brief Call back function pointer data type for other hardfault */
typedef void (*other_fault_callback_t)(hw_stackframe_t *fault_stack);

typedef struct
{
    flash_ecc_callback_t eccCallback; /*!< Callback function pointer */
    other_fault_callback_t otherFaultCallback; /*!< Other fault callback function pointer */
} flash_ecc_fault_config_t;

typedef struct
{
    flash_ecc_callback_t eccCallback; /*!< Flash ECC callback function pointer */
    other_fault_callback_t otherFaultCallback; /*!< Other fault callback function pointer */
} flash_ecc_fault_state_t;

/*******************************************************************************
* Function Prototypes for Flash ECC Fault
*******************************************************************************/
/*!
 * @name Flash ECC Fault APIs
 * @{
 */

#if defined(__cplusplus)
extern "C" {
#endif

/*!
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
void FLASH_ECC_Fault_Init(const flash_ecc_fault_config_t * userConfigPtr, flash_ecc_fault_state_t * state);

#if defined(__cplusplus)
}
#endif

/*@}*/ /* End of Flash ECC FAULT APIs*/
/*! @}*/ /* End of addtogroup flash_ecc_fault */

#endif /* FLASH_ECC_FAULT_H */