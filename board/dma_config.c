/*
 * Copyright 2020-2025 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 * @file dma_config.c
 * @brief 
 * 
 */


#include <stddef.h>
#include "dma_config.h"


/*dma_config0*/


const dma_channel_config_t dma_config0 = {
    .virtChnConfig=0,
    .source=DMA_REQ_UART2_RX,
    .callback=NULL,
    .callbackParam=NULL,
};
/*dma_config2*/


const dma_channel_config_t dma_config2 = {
    .virtChnConfig=2,
    .source=DMA_REQ_ADC0,
    .callback=NULL,
    .callbackParam=NULL,
};
/*dma_config1*/


const dma_channel_config_t dma_config1 = {
    .virtChnConfig=1,
    .source=DMA_REQ_UART2_TX,
    .callback=NULL,
    .callbackParam=NULL,
};


const dma_channel_config_t *const dmaChnConfigArray[NUM_OF_CONFIGURED_DMA_CHANNEL] = {
    &dma_config0,
    &dma_config2,
    &dma_config1,
};

const dma_user_config_t dmaController_InitConfig = {
    .haltOnError = false,
};

dma_chn_state_t dma_config0_State;
dma_chn_state_t dma_config2_State;
dma_chn_state_t dma_config1_State;



dma_chn_state_t *const dmaChnState[NUM_OF_CONFIGURED_DMA_CHANNEL]={
    &dma_config0_State,
    &dma_config2_State,
    &dma_config1_State,
};

dma_state_t dmaState;


