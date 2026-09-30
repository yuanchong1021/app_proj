/*
 * Copyright 2020-2025 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 * @file utility_flash_ecc_fault_config.c
 * @brief 
 * 
 */


#include <stddef.h>
#include "utility_flash_ecc_fault_config.h"


/*flash_ecc_fault_config0*/

flash_ecc_fault_state_t flash_ecc_fault_config0_State;




const flash_ecc_fault_config_t flash_ecc_fault_config0={
    .eccCallback=NULL,
    .otherFaultCallback=NULL,
};

