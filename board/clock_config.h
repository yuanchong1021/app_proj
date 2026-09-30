/*
 * Copyright 2020-2025 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 * @file clock_config.h
 * @brief 
 * 
 */


#ifndef CLOCK_CONFIG_H
#define CLOCK_CONFIG_H

#include "clock.h"



/*! @brief Count of user configuration structures */
#define CLOCK_MANAGER_CONFIG_CNT                           1U /*!< Count of user configuration */

/*! @brief Count of user Callbacks structures */
#define CLOCK_MANAGER_CALLBACK_CNT                         0U /*!< Count of user Callbacks */

/*! @brief Actived clock configuration */
#define CLOCK_MANAGER_ACTIVE_INDEX  0U

/*! @brief Array of User callbacks */
/* The tool do not support generate Callbacks configuration. It's always empty. */
extern clock_manager_callback_user_config_t *g_clockManCallbacksArr[];

/*! @brief Array of pointers to User configuration structures */
extern clock_manager_user_config_t const *g_clockManConfigsArr[CLOCK_MANAGER_CONFIG_CNT];

#endif /* CLOCK_CONFIG_H */



