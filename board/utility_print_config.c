/*
 * Copyright 2020-2025 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 * @file utility_print_config.c
 * @brief 
 * 
 */



#include "utility_print_config.h"
#include "uart_config.h"

status_t UTILITY_PRINT_Init(void)
{
    status_t status=STATUS_SUCCESS;
    status=UART_DRV_Init(1, &uart_config0_State,&uart_config0);
    return status;
}

void printf_char(char ch)
{
    UART_DRV_SendDataPolling(1, (const uint8_t *) &ch, 1);
}


