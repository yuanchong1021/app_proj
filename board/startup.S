/*
 * Copyright 2020-2025 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 * @file statup.S
 * @brief 
 * 
 */

    MODULE  ?cstartup
    EXTERN  main
    EXTERN  SystemInit
    EXTERN  RamInit0
    EXTERN  RamInit1
    EXTERN  RamInit2
    EXTERN  VectorTableCopy
    EXTERN STACK_end

    PUBWEAK Reset_Handler
    SECTION .text:CODE:REORDER:NOROOT(2)
    THUMB
_start:
Reset_Handler:
    CPSID   I               ; Mask interrupts
    ;; Init the rest of the registers
    LDR     R1,=0
    LDR     R2,=0
    LDR     R3,=0
    LDR     R4,=0
    LDR     R5,=0
    LDR     R6,=0
    LDR     R7,=0
    MOV     R8,R7
    MOV     R9,R7
    MOV     R10,R7
    MOV     R11,R7
    MOV     R12,R7

    /* RamInit 0 Stage, focus on ecc init, asm code*/
    BL     RamInit0

    /* Initialize the stack pointer */
    LDR     r0,=STACK_end
    MOV     r13,r0

    /* RamInit 1 Stage, focus on copy data,clear bss, c code*/
    LDR     r0,=RamInit1
    BLX     r0

    /* Copy Vector Table for interrupt, c code */
#ifndef __NO_VECTOR_TABLE_COPY
    /* Call the to copy vector table from flash to ram */
    LDR     r0,=VectorTableCopy
    BLX     r0
#endif
    /* SystemInit, user can init PLL to speed up left startup code, c code*/
#ifndef __NO_SYSTEM_INIT
    /* Call the system init routine */
    LDR     r0,=SystemInit
    BLX     r0
#endif
    /* RamInit 2 Stage, focus on others ram init, c code */
    LDR     r0,=RamInit2
    BLX     r0

    /* Unmask interrupts */
    CPSIE   I               

    /* Call the main routine */
    BL      main
JumpToSelf:
    B       JumpToSelf

    END