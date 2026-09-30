/*
 * Copyright 2020-2025 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 * @file dma_config.h
 * @brief 
 * 
 */




#ifndef __DMA_CONFIG_H__
#define __DMA_CONFIG_H__




#include "dma_driver.h"



#define NUM_OF_CONFIGURED_DMA_CHANNEL 3U


extern dma_state_t dmaState;
extern dma_chn_state_t *const dmaChnState[NUM_OF_CONFIGURED_DMA_CHANNEL];
extern const dma_user_config_t dmaController_InitConfig;
extern const dma_channel_config_t *const dmaChnConfigArray[NUM_OF_CONFIGURED_DMA_CHANNEL];

#endif

