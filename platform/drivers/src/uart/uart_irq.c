/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file uart_irq.c
 * @version 1.4.0
 */

/*!
 * @page misra_violations MISRA-C:2012 violations list
 *
 * PRQA S 1533 Rule 8.9: The object '%1s' is only referenced by function '%2s'.
 *
 */

#include "uart_irq.h"

/*******************************************************************************
 * Code
 ******************************************************************************/

#if (UART_INSTANCE_COUNT > 0U)
/* Implementation of UART0 handler named in startup code. */
void UART0_IrqHandler(void)
{
    UART_DRV_IRQHandler(0);
}
#endif

#if (UART_INSTANCE_COUNT > 1U)
/* Implementation of UART1 handler named in startup code. */
void UART1_IrqHandler(void)
{
    UART_DRV_IRQHandler(1);
}
#endif

#if (UART_INSTANCE_COUNT > 2U)
/* Implementation of UART2 handler named in startup code. */
void UART2_IrqHandler(void)
{
    UART_DRV_IRQHandler(2);
}
#endif

#if (UART_INSTANCE_COUNT > 3U)
/* Implementation of UART3 handler named in startup code. */
void UART3_IrqHandler(void)
{
    UART_DRV_IRQHandler(3);
}
#endif

#if (UART_INSTANCE_COUNT > 4U)
/* Implementation of UART4 handler named in startup code. */
void UART4_IrqHandler(void)
{
    UART_DRV_IRQHandler(4);
}
#endif

#if (UART_INSTANCE_COUNT > 5U)
/* Implementation of UART5 handler named in startup code. */
void UART5_IrqHandler(void)
{
    UART_DRV_IRQHandler(5);
}
#endif

/* Array storing references to UART irq handlers */
isr_t g_uartIsr[UART_INSTANCE_COUNT] = /*PRQA S 1533*/
{
#if (UART_INSTANCE_COUNT > 0U)
    UART0_IrqHandler,
#endif
#if (UART_INSTANCE_COUNT > 1U)
    UART1_IrqHandler,
#endif
#if (UART_INSTANCE_COUNT > 2U)
    UART2_IrqHandler,
#endif
#if (UART_INSTANCE_COUNT > 3U)
    UART3_IrqHandler,
#endif
#if (UART_INSTANCE_COUNT > 4U)
    UART4_IrqHandler,
#endif
#if (UART_INSTANCE_COUNT > 5U)
    UART5_IrqHandler,
#endif
};

/*******************************************************************************
 * EOF
 ******************************************************************************/
