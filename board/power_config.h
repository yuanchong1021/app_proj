/*
 * Copyright 2020-2025 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 * @file power_config.h
 * @brief 
 * 
 */




#ifndef __POWER_CONFIG_H__
#define __POWER_CONFIG_H__

#include "power_manager.h"

#define POWER_MANAGER_CONFIG_CNT                (4U)
#define POWER_MANAGER_CALLBACK_CNT              (0U)

extern power_manager_user_config_t* powerConfigsArr[4];
#endif


