/*
 * Copyright 2020-2025 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 * @file RamInit1.c
 * @brief 
 * 
 */

#include <stdint.h>
#include <stdbool.h>
#include "device_registers.h"

#define WORD_ALIGN_MASK (3U)
#define WORD_SHIFT      (2U)

typedef enum
{
    INIT_NULL = 0U,   /*!< No initialization */
    INIT_NORMAL = 1U, /*!< Normal initialization */
    INIT_POR = 2U,    /*!< Power on reset initialization */
} RamInitType;


typedef struct
{
    uint32_t *RamStart;   /*!< Start address of section in RAM */
    uint32_t *RomStart;   /*!< Start address of section in ROM */
    uint32_t *RomEnd;     /*!< End address of section in ROM */
    RamInitType InitType;   /*!< Type of initialization*/
} RamCopyLayoutType;

typedef struct
{
    uint32_t *RamStart;   /*!< Start address of section in RAM */
    uint32_t *RamEnd;     /*!< End address of section in RAM */
    RamInitType InitType;   /*!< Type of initialization*/
} RamZeroLayoutType;

extern uint32_t CODE_RAM_ram_start[1U];
extern uint32_t CODE_RAM_rom_start[1U];
extern uint32_t CODE_RAM_rom_end[1U];
extern uint32_t DATA_RAM_ram_start[1U];
extern uint32_t DATA_RAM_rom_start[1U];
extern uint32_t DATA_RAM_rom_end[1U];
extern uint32_t SharedSegment_ram_start[1U];
extern uint32_t SharedSegment_rom_start[1U];
extern uint32_t SharedSegment_rom_end[1U];

extern uint32_t BSS_start[1U];
extern uint32_t BSS_end[1U];

const RamCopyLayoutType CopyLayout[3]={
    {
        .RamStart=CODE_RAM_ram_start,
        .RomStart=CODE_RAM_rom_start,
        .RomEnd=CODE_RAM_rom_end,
        .InitType=INIT_NORMAL,
    },
    {
        .RamStart=DATA_RAM_ram_start,
        .RomStart=DATA_RAM_rom_start,
        .RomEnd=DATA_RAM_rom_end,
        .InitType=INIT_NORMAL,
    },
    {
        .RamStart=SharedSegment_ram_start,
        .RomStart=SharedSegment_rom_start,
        .RomEnd=SharedSegment_rom_end,
        .InitType=INIT_POR,
    },
};

const RamZeroLayoutType ZeroLayout[1]={
    {
        .RamStart=BSS_start,
        .RamEnd=BSS_end,
        .InitType=INIT_NORMAL,
    },
};


/**
 * @brief Check MCU is power on reset or not, user can override this function
 * @return 1: POR, 0: Not POR
 */
#define RCU_RSSR_POR_MASK   (0x1U)
static bool IsPOR(void)
{
    bool RetVal = false;
    /* Check if the MCU is power on reset or not */
    if (RCU_RSSR_POR_MASK == (RCU->RSSR & RCU_RSSR_POR_MASK))
    {
        RetVal = true;
    }

    return RetVal;
}


/**
 * @brief RamInit1 for copying initialized data and zeroing uninitialized data
 */
void RamInit1(){
    bool IsPor = IsPOR();
    const uint32_t *Rom32;
    const uint8_t *Rom8;
    uint32_t *Ram32;
    uint8_t *Ram8;
    volatile uint8_t DataPad;
    volatile uint32_t Len = 0U;
    uint32_t Size = 0U;
    uint32_t MajorLoopCnt = 0U;
    uint32_t MinorLoopCnt = 0U;

    /* Copy initialized table */
    Len = 3;
    for (MajorLoopCnt = 0U; MajorLoopCnt < Len; ++MajorLoopCnt)
    {
        if(((CopyLayout[MajorLoopCnt].InitType == INIT_POR) && (IsPor != true)) || (CopyLayout[MajorLoopCnt].InitType == INIT_NULL))
        {
            continue;
        }
        Rom32 = (const uint32_t *)CopyLayout[MajorLoopCnt].RomStart;
        Ram32 = (uint32_t *)CopyLayout[MajorLoopCnt].RamStart;
        Size = (uint32_t)CopyLayout[MajorLoopCnt].RomEnd - (uint32_t)CopyLayout[MajorLoopCnt].RomStart;
        /* Make sure the data area to be copied must be aligned with 4. Then, copy 4 bytes at per one read */
        DataPad = Size & WORD_ALIGN_MASK;
        Size = (Size - DataPad) >> WORD_SHIFT;
        for (MinorLoopCnt = 0U; MinorLoopCnt < Size; ++MinorLoopCnt)
        {
            Ram32[MinorLoopCnt] = Rom32[MinorLoopCnt];
        }
        /* For the rest of data, copy 1 bytes at per one read */
        Rom8 = (uint8_t *)(&Rom32[MinorLoopCnt]);
        Ram8 = (uint8_t *)(&Ram32[MinorLoopCnt]);
        for (MinorLoopCnt = 0U; MinorLoopCnt < DataPad; ++MinorLoopCnt)
        {
            Ram8[MinorLoopCnt] = Rom8[MinorLoopCnt];
        }
    }

    /* Clear zero table */
    Len = 1;
    for (MajorLoopCnt = 0U; MajorLoopCnt < Len; ++MajorLoopCnt)
    {
        if(((ZeroLayout[MajorLoopCnt].InitType == INIT_POR) && (IsPor != true)) || (ZeroLayout[MajorLoopCnt].InitType == INIT_NULL))
        {
            continue;
        }
        Ram32 = (uint32_t *)ZeroLayout[MajorLoopCnt].RamStart;
        Size = (uint32_t)ZeroLayout[MajorLoopCnt].RamEnd - (uint32_t)ZeroLayout[MajorLoopCnt].RamStart;
        /* Make sure the data area to be zeroed must be aligned with 4. Then, zero 4 bytes at per one read */
        DataPad = Size & WORD_ALIGN_MASK;
        Size = (Size - DataPad) >> WORD_SHIFT;        
        for (MinorLoopCnt = 0U; MinorLoopCnt < Size; ++MinorLoopCnt)
        {
            Ram32[MinorLoopCnt] = 0U;
        }
        /* For the rest of data, zero 1 bytes at per one read */
        Ram8 = (uint8_t *)(&Ram32[MinorLoopCnt]);
        for (MinorLoopCnt = 0U; MinorLoopCnt < DataPad; ++MinorLoopCnt)
        {
            Ram8[MinorLoopCnt] = 0U;
        }
    }
}


