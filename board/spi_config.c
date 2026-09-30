/*
 * Copyright 2020-2025 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 * @file spi_config.c
 * @brief 
 * 
 */


#include <stddef.h>
#include "spi_config.h"

/* SPI master */
/*spi_MasterConfig0*/



spi_state_t spi_MasterConfig0_State;
const spi_master_config_t spi_MasterConfig0 = {
    .bitsPerSec=1000000U,
    .whichPcs=SPI_PCS0,
    .pcsPolarity=SPI_ACTIVE_LOW,
    .isPcsContinuous=false,
    .bitcount=8,
    .clkPhase=SPI_CLOCK_PHASE_1ST_EDGE,
    .clkPolarity=SPI_SCK_ACTIVE_HIGH,
    .lsbFirst=false,
    .transferType=SPI_USING_INTERRUPTS,
    .rxDMAChannel=1,
    .txDMAChannel=0,
    .callback=NULL,
    .callbackParam=NULL,
    .width=SPI_SINGLE_BIT_XFER,
};
/* SPI slave */

