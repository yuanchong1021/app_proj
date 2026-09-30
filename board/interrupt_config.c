/*
 * Copyright 2020-2025 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 * @file interrupt_config.c
 * @brief 
 * 
 */


#include <stddef.h>
#include "interrupt_config.h"


void INT_SYS_ConfigInit(void)
{
    INT_SYS_DisableIRQGlobal();
    INT_SYS_EnableIRQGlobal();
}

