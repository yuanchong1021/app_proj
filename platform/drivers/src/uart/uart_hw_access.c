/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file uart_hw_access.c
 * @version 1.4.0
 */

/*!
 * @page misra_violations MISRA-C:2012 violations list
 *
 * PRQA S 2985 Rule 2.2: This operation is redundant. The value of the result is 
 *                       always that of the left-hand operand.
 */

#include "uart_hw_access.h"

/*******************************************************************************
 * Code
 ******************************************************************************/
/*FUNCTION**********************************************************************
 *
 * Function Name : UART_Init
 * Description   : Initializes the UART controller to known state, using
 *                 register reset values defined in the reference manual.
 *END**************************************************************************/
void UART_Init(UART_Type * base)
{
    /* Set the default oversampling ratio (16) and baud-rate divider (4) */
    base->BAUD = ((uint32_t)((FEATURE_UART_DEFAULT_OSR << UART_BAUD_OSRVAL_SHIFT) | \
                 (FEATURE_UART_DEFAULT_DIV << UART_BAUD_DIV_SHIFT)));
    /* Clear the error/interrupt flags */
    base->INTF = FEATURE_UART_INTF_REG_FLAGS_MASK;
    /* Reset all features/interrupts by default */
    base->CTRL0 = 0x00000000;
#if !defined(FEATURE_UART_LITE_VERSION)
    /* Reset match addresses */
    base->MATCH = 0x00000000;
#endif /* FEATURE_UART_LITE_VERSION */
#if FEATURE_UART_FIFO_SIZE > 0U
    /* Reset FIFO and watermark feature */
    base->TXFIFO = FEATURE_UART_FIFO_RESET_MASK;
    base->RXFIFO = FEATURE_UART_FIFO_RESET_MASK;
#endif
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_SetBitCountPerChar
 * Description   : Configures the number of bits per char in UART controller.
 * In some UART instances, the user should disable the transmitter/receiver
 * before calling this function.
 * Generally, this may be applied to all UARTs to ensure safe operation.
 *END**************************************************************************/
void UART_SetBitCountPerChar(UART_Type * base, uart_bit_count_per_char_t bitCountPerChar, bool parity)
{
    uint32_t tmpBitCountPerChar = (uint32_t)bitCountPerChar;
    if (parity)
    {
        tmpBitCountPerChar += 1U;
    }

    base->CTRL1 &= ~UART_CTRL1_FRAMESZ_MASK;
    base->CTRL1 |= (tmpBitCountPerChar << UART_CTRL1_FRAMESZ_SHIFT);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_SetParityMode
 * Description   : Configures parity mode in the UART controller.
 * In some UART instances, the user should disable the transmitter/receiver
 * before calling this function.
 * Generally, this may be applied to all UARTs to ensure safe operation.
 *END**************************************************************************/
void UART_SetParityMode(UART_Type * base, uart_parity_mode_t parityModeType)
{
    base->CTRL1 = (base->CTRL1 & ~UART_CTRL1_PAREN_MASK) | (((uint32_t)parityModeType >> 1U) << UART_CTRL1_PAREN_SHIFT);
    base->CTRL1 = (base->CTRL1 & ~UART_CTRL1_PARSEL_MASK) | (((uint32_t)parityModeType & 1U) << UART_CTRL1_PARSEL_SHIFT); /* PRQA S 2985 */
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_Putchar9
 * Description   : Sends the UART 9-bit character.
 *END**************************************************************************/
void UART_Putchar9(UART_Type * base, uint16_t data)
{
    base->DATA = data;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_Putchar10
 * Description   : Sends the UART 10-bit character.
 *END**************************************************************************/
void UART_Putchar10(UART_Type * base, uint16_t data)
{
    base->DATA = data;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_Getchar7
 * Description   : Gets the UART 7-bit character.
 *END**************************************************************************/
void UART_Getchar7(const UART_Type * base, uint8_t *readData)
{
    DEV_ASSERT(readData != NULL);

    *readData = (uint8_t)(base->DATA & 0x7FU);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_Getchar8
 * Description   : Gets the UART 8-bit character.
 *END**************************************************************************/
void UART_Getchar8(const UART_Type * base, uint8_t *readData)
{
    DEV_ASSERT(readData != NULL);

    *readData = (uint8_t)base->DATA;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_Getchar9
 * Description   : Gets the UART 9-bit character.
 *END**************************************************************************/
void UART_Getchar9(const UART_Type * base, uint16_t *readData)
{
    DEV_ASSERT(readData != NULL);

    /* get data from the uart data register */
    *readData = (uint16_t)(base->DATA & 0x1FFU);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_Getchar10
 * Description   : Gets the UART 10-bit character
 *END**************************************************************************/
void UART_Getchar10(const UART_Type * base, uint16_t *readData)
{
    DEV_ASSERT(readData != NULL);

    /* get data */
    *readData = (uint16_t)(base->DATA & 0x3FFU);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_SetIntMode
 * Description   : Configures the UART module interrupts to enable/disable
 * various interrupt sources.
 *END**************************************************************************/
void UART_SetIntMode(UART_Type * base, uart_interrupt_t intSrc, bool enable)
{
    SDK_ENTER_CRITICAL();
    if (enable){
        base->INTE |= (uint32_t)intSrc;
    } else {
        base->INTE &= ~(uint32_t)intSrc;
    }
    SDK_EXIT_CRITICAL();
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_GetIntMode
 * Description   : Returns whether UART module interrupt is enabled/disabled.
 *END**************************************************************************/
bool UART_GetIntMode(const UART_Type * base, uart_interrupt_t intSrc)
{
    return (base->INTE & (uint32_t)intSrc) != 0U;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_GetStatusFlag
 * Description   : UART get status flag by passing flag enum.
 *END**************************************************************************/
bool UART_GetStatusFlag(const UART_Type * base, uart_status_flag_t statusFlag)
{
    return (base->INTF & (uint32_t)statusFlag) != 0U;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_ClearStatusFlag
 * Description   : UART clears an individual status flag
 * (see uart_status_flag_t for list of status bits).
 *END**************************************************************************/
status_t UART_ClearStatusFlag(UART_Type * base,
                                    uart_status_flag_t statusFlag)
{
    status_t returnCode = STATUS_SUCCESS;
    base->INTF = (uint32_t)statusFlag;
    return (returnCode);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_SetErrorInterrupts
 * Description   : Enable or disable the UART error interrupts.
 *END**************************************************************************/
void UART_SetErrorInterrupts(UART_Type * base, bool enable)
{
    /* Configure the error interrupts */
    UART_SetIntMode(base, UART_INT_RX_OVERRUN, enable);
    UART_SetIntMode(base, UART_INT_PARITY_ERR_FLAG, enable);
    UART_SetIntMode(base, UART_INT_NOISE_ERR_FLAG, enable);
    UART_SetIntMode(base, UART_INT_FRAME_ERR_FLAG, enable);
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
