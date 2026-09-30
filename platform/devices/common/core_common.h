/*
 * Copyright 2020-2025 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file core_common.h
 * @version 1.4.0
 */

/*!
 * @page misra_violations MISRA-C:2012 violations list
 *
 * PRQA S 0603 Rule 21.1: The macro identifier '%1s' is reserved.
 *
 * PRQA S 0602 Rule 1.3: The identifier '%1s' is reserved for use by the library.
 */

/*
 * Tool Chains:
 *   GNUC flag is defined also by ARM compiler - it shows the current major version of the compatible GCC version
 *   __GNUC__   : GNU Compiler Collection
 *   __ghs__    : Green Hills ARM Compiler
 *   __ICCARM__ : IAR ARM Compiler
 *   __DCC__    : Wind River Diab Compiler
 *   __ARMCC_VERSION:  ARM Compiler
 */

#ifndef CORE_COMMON_H
#define CORE_COMMON_H


#ifdef __cplusplus
extern "C" {
#endif

/** \brief  BKPT_ASM
 *
 *   Macro to be used to trigger an debug interrupt
 */
#define BKPT_ASM __asm("BKPT #0\n\t")
        

/** \brief  Enable FPU
 *
 *   ENABLE_FPU indicates whether SystemInit will enable the Floating point unit (FPU)
 */
#if defined (__GNUC__) || defined (__ARMCC_VERSION)
#if defined (__VFP_FP__) && !defined (__SOFTFP__)
#define ENABLE_FPU
#endif

#elif defined (__ICCARM__)
#if defined __ARMVFP__
#define ENABLE_FPU
#endif

#elif defined (__ghs__) || defined (__DCC__)
#if defined (__VFP__)
#define ENABLE_FPU
#endif
#endif /* if defined (__GNUC__) */

/** \brief  Enable interrupts
 */
#if defined (__GNUC__) 
#define ENABLE_INTERRUPTS() __asm volatile ("cpsie i" : : : "memory");
#else
#define ENABLE_INTERRUPTS() __asm("cpsie i")
#endif


/** \brief  Disable interrupts
 */
#if defined (__GNUC__)
#define DISABLE_INTERRUPTS() __asm volatile ("cpsid i" : : : "memory");
#else
#define DISABLE_INTERRUPTS() __asm("cpsid i")
#endif


/** \brief  Enter low-power standby state
 *    WFI (Wait For Interrupt) makes the processor suspend execution (Clock is stopped) until an IRQ interrupts.
 */
#if defined (__GNUC__)
#define STANDBY() __asm volatile ("wfi")
#else
#define STANDBY() __asm("wfi")
#endif

/** \brief  No-op
 */
#define NOP() __asm volatile ("nop")

/** \brief  Reverse byte order in a word.
 */
#if defined (__GNUC__) || defined (__ICCARM__) || defined (__ghs__) || defined (__ARMCC_VERSION)
#define REV_BYTES_32(a, b) __asm volatile ("rev %0, %1" : "=r" (b) : "r" (a))
#else
#define REV_BYTES_32(a, b) (b = ((a & 0xFF000000U) >> 24U) | ((a & 0xFF0000U) >> 8U) \
                                | ((a & 0xFF00U) << 8U) | ((a & 0xFFU) << 24U))
#endif

/** \brief  Reverse byte order in each halfword independently.
 */
#if defined (__GNUC__) || defined (__ICCARM__) || defined (__ghs__) || defined (__ARMCC_VERSION)
#define REV_BYTES_16(a, b) __asm volatile ("rev16 %0, %1" : "=r" (b) : "r" (a))
#else
#define REV_BYTES_16(a, b) (b = ((a & 0xFF000000U) >> 8U) | ((a & 0xFF0000U) << 8U) \
                                | ((a & 0xFF00U) >> 8U) | ((a & 0xFFU) << 8U))
#endif

/** \brief  Places a function in RAM.
 */
#if defined ( __GNUC__ ) || defined (__ARMCC_VERSION)
    #define START_FUNCTION_DECLARATION_RAMSECTION
    #define END_FUNCTION_DECLARATION_RAMSECTION        __attribute__((section (".code_ram")));
#elif defined ( __ghs__ )
    #define START_FUNCTION_DECLARATION_RAMSECTION      _Pragma("ghs callmode=far")
    #define END_FUNCTION_DECLARATION_RAMSECTION        __attribute__((section (".code_ram")));\
                                                       _Pragma("ghs callmode=default")
#elif defined ( __ICCARM__ )
    #define START_FUNCTION_DECLARATION_RAMSECTION
    #define END_FUNCTION_DECLARATION_RAMSECTION        __attribute__((section (".code_ram")));
#elif defined ( __DCC__ )
    #define START_FUNCTION_DECLARATION_RAMSECTION      _Pragma("section CODE \".code_ram\"") \
                                                       _Pragma("use_section CODE")
    #define END_FUNCTION_DECLARATION_RAMSECTION        ; \
                                                       _Pragma("section CODE \".text\"")
#else
    /* Keep compatibility with software analysis tools */
    #define START_FUNCTION_DECLARATION_RAMSECTION      
    #define END_FUNCTION_DECLARATION_RAMSECTION        ;
#endif
                                                   
    /* For GCC, IAR, GHS, Diab and ARMC there is no need to specify the section when
    defining a function, it is enough to specify it at the declaration. This
    also enables compatibility with software analysis tools. */
    #define START_FUNCTION_DEFINITION_RAMSECTION
    #define END_FUNCTION_DEFINITION_RAMSECTION

#if defined (__ICCARM__)
    #define DISABLE_CHECK_RAMSECTION_FUNCTION_CALL     _Pragma("diag_suppress=Ta022, Ta023")
    #define ENABLE_CHECK_RAMSECTION_FUNCTION_CALL      _Pragma("diag_default=Ta022, Ta023")
#else
    #define DISABLE_CHECK_RAMSECTION_FUNCTION_CALL
    #define ENABLE_CHECK_RAMSECTION_FUNCTION_CALL
#endif

/** \brief  Get Core ID
 *
 *   GET_CORE_ID returns the processor identification number for cm33
 */
#if defined(NUMBER_OF_CORES) && (NUMBER_OF_CORES > 1)
#if defined(CPU_YTM32B1HB0)
    /* Get the core ID from the CIM->CORE_NUM register */
    #define GET_CORE_ID()   (*((volatile uint32_t *)0x401ec074u) & 0x3U)
#endif /* CPU_YTM32B1HB0 */
#else
#define GET_CORE_ID()	0U
#endif /* NUMBER_OF_CORES */

/** \brief  Data alignment.
 */
#if defined ( __GNUC__ ) || defined ( __ghs__ ) || defined ( __DCC__ ) || defined (__ARMCC_VERSION) || defined (__ICCARM__)
    #define ALIGNED(x)      __attribute__((aligned(x)))
#else
    /* Keep compatibility with software analysis tools */
    #define ALIGNED(x)
#endif

/** \brief  Weak functions.
 */
#if defined ( __GNUC__ ) || defined ( __ghs__ ) || defined ( __DCC__ ) || defined (__ARMCC_VERSION)
    #ifndef __weak
        #define __weak        __attribute__((weak)) /*PRQA S 0603, 0602*/
    #endif
    #ifndef __packed
        #define __packed      __attribute__((packed)) /*PRQA S 0603, 0602*/
    #endif
#else
    /* Keep compatibility with software analysis tools */
    #ifndef __weak
        #define __weak
    #endif
    #ifndef __packed
        #define __packed
    #endif
#endif

/** \brief  Endianness.
 */
#define CORE_LITTLE_ENDIAN

#ifdef __cplusplus
}
#endif

#endif /* CORE_COMMON_H */

/*******************************************************************************
 * EOF
 ******************************************************************************/

