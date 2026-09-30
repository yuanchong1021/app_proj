/*
 * Copyright 2020-2025 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 * @file adc_config.c
 * @brief 
 * 
 */


#include "adc_config.h"



/* adc_config0 */
const adc_converter_config_t adc_config0={
    .clockDivider=1,
    .startTime=160,
    .sampleTime=80,
    .overrunMode=false,
    .autoOffEnable=false,
    .waitEnable=false,
    .trigger=ADC_TRIGGER_SOFTWARE,
    .align=ADC_ALIGN_RIGHT,
    .resolution=ADC_RESOLUTION_12BIT,
    .dmaWaterMark=1,
    .dmaEnable=true,
    .sequenceConfig={
        .sequenceMode=ADC_CONV_CONTINUOUS,
        .sequenceIntEnable=false,
        .convIntEnable=false,
        .readyIntEnable=false,
        .ovrunIntEnable=false,
        .sampIntEnable=false,
        .channels={
            ADC_INPUTCHAN_EXT4,
            ADC_INPUTCHAN_EXT5,
        },
        .totalChannels=2,
    },
    .compareConfig={
        .compareEnable=false,
        .compareAllChannelEnable=false,

        .compHigh=4095,
        .compLow=0,
        .compIntEnable=false,
    },
};


