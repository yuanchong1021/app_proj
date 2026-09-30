/*
 * Copyright 2020-2025 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 * @file can_config.c
 * @brief 
 * 
 */



#include "can_config.h"


/*flexcanInitConfig0*/

flexcan_state_t flexcanInitConfig0_State;

const flexcan_user_config_t flexcanInitConfig0 = {
    .max_num_mb = 32UL,
    .num_id_filters = FLEXCAN_RX_FIFO_ID_FILTERS_8,
    .is_rx_fifo_needed = false,
    .flexcanMode = FLEXCAN_NORMAL_MODE,
    .payload = FLEXCAN_PAYLOAD_SIZE_8,
    .fd_enable = false,
    .pe_clock = FLEXCAN_CLK_SOURCE_OSC,
    .transfer_type = FLEXCAN_RXFIFO_USING_INTERRUPTS,
    .bitrate = {
        .propSeg = 4,    /*!< Propagation segment*/
        .phaseSeg1 = 7,  /*!< Phase segment 1*/
        .phaseSeg2 = 1,  /*!< Phase segment 2*/
        .preDivider = 2, /*!< Clock prescaler division factor*/
        .rJumpwidth = 1, /*!< Resync jump width*/
    },
    .bitrate_cbt = {
        .propSeg = 6,    /*!< Propagation segment*/
        .phaseSeg1 = 2,  /*!< Phase segment 1*/
        .phaseSeg2 = 1,  /*!< Phase segment 2*/
        .preDivider = 0, /*!< Clock prescaler division factor*/
        .rJumpwidth = 1, /*!< Resync jump width*/
    },
    .rxFifoDMAChannel=0,
};


