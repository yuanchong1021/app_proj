/*
 * Copyright 2020-2025 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 * @file RamInit0.S
 * @brief 
 * 
 */

   EXTERN IVT_RAM_memory_start
   EXTERN IVT_RAM_memory_end
   EXTERN RAM_memory_start
   EXTERN RAM_memory_end
   EXTERN SharedSegment_memory_start
   EXTERN SharedSegment_memory_end
   EXTERN STACK_memory_start
   EXTERN STACK_memory_end
   PUBWEAK RamInit0
   SECTION .text:CODE:REORDER:NOROOT(2)
   THUMB
RamInit0:
_GET_POR:
    LDR     R1, =0x4007F000
    LDR     R4, [R1]
    LDR     R3, =0x1
/* R4 store POR result */
    ANDS    R4, R3
_IVT_RAM_start:
/* 8bytes align */
    LDR     R1, =IVT_RAM_memory_start
    LDR     R2, =IVT_RAM_memory_end
    
    SUBS    R2, R2, R1
    SUBS    R2, #1
    BLE     _IVT_RAM_end

    LDR     R0, =0x5A5A5A5A
    LDR     R3, =0x5A5A5A5A
 _IVT_RAM_loop:
    STM     R1!, {R0,R3}
    SUBS    R2, #8
    BGE      _IVT_RAM_loop
 _IVT_RAM_end:
_RAM_start:
/* 8bytes align */
    LDR     R1, =RAM_memory_start
    LDR     R2, =RAM_memory_end
    
    SUBS    R2, R2, R1
    SUBS    R2, #1
    BLE     _RAM_end

    LDR     R0, =0x5A5A5A5A
    LDR     R3, =0x5A5A5A5A
 _RAM_loop:
    STM     R1!, {R0,R3}
    SUBS    R2, #8
    BGE      _RAM_loop
 _RAM_end:
    CMP     R4, #0
    BEQ     _SharedSegment_end
_SharedSegment_start:
/* 8bytes align */
    LDR     R1, =SharedSegment_memory_start
    LDR     R2, =SharedSegment_memory_end
    
    SUBS    R2, R2, R1
    SUBS    R2, #1
    BLE     _SharedSegment_end

    LDR     R0, =0x5A5A5A5A
    LDR     R3, =0x5A5A5A5A
 _SharedSegment_loop:
    STM     R1!, {R0,R3}
    SUBS    R2, #8
    BGE      _SharedSegment_loop
 _SharedSegment_end:
_STACK_start:
/* 8bytes align */
    LDR     R1, =STACK_memory_start
    LDR     R2, =STACK_memory_end
    
    SUBS    R2, R2, R1
    SUBS    R2, #1
    BLE     _STACK_end

    LDR     R0, =0x5A5A5A5A
    LDR     R3, =0x5A5A5A5A
 _STACK_loop:
    STM     R1!, {R0,R3}
    SUBS    R2, #8
    BGE      _STACK_loop
 _STACK_end:
   BX LR

   END

