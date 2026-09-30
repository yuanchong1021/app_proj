/*
 * Copyright 2020-2025 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 * @file power_config.c
 * @brief 
 * 
 */


#include <stddef.h>
#include "power_config.h"




/* pwrMan_InitConfigRun */
power_manager_user_config_t pwrMan_InitConfigRun={
    .powerMode=POWER_MANAGER_RUN,
    .sleepOnExitValue=false,
};
/* pwrMan_InitConfigSleep */
power_manager_user_config_t pwrMan_InitConfigSleep={
    .powerMode=POWER_MANAGER_SLEEP,
    .sleepOnExitValue=false,
};
/* pwrMan_InitConfigDeepSleep */
power_manager_user_config_t pwrMan_InitConfigDeepSleep={
    .powerMode=POWER_MANAGER_DEEPSLEEP,
    .sleepOnExitValue=false,
};
/* pwrMan_InitConfigStandBy */
power_manager_user_config_t pwrMan_InitConfigStandBy={
    .powerMode=POWER_MANAGER_STANDBY,
    .sleepOnExitValue=false,
};





power_manager_user_config_t* powerConfigsArr[4]={
    &pwrMan_InitConfigRun,
    &pwrMan_InitConfigSleep,
    &pwrMan_InitConfigDeepSleep,
    &pwrMan_InitConfigStandBy,
};


