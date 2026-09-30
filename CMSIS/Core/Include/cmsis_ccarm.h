/**************************************************************************//**
 * @file     cmsis_ccarm.h
 * @brief    CMSIS compiler CCARM (GHS Compiler for ARM) header file
 * @version  V0.0.1
 * @date     10. March 2024
 ******************************************************************************/
/*
 * Copyright (c) 2009-2024 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __CMSIS_CCARM_H
#define __CMSIS_CCARM_H

/* Define compiler macros for CPU architecture, used in CMSIS 5.
 */
#if !defined(__ARM_ARCH_6M__) && !defined(__ARM_ARCH_7M__) && !defined(__ARM_ARCH_7EM__) && \
    !defined(__ARM_ARCH_8M_BASE__) && !defined(__ARM_ARCH_8M_MAIN__)
  #if defined(__CORE_CORTEXM0PLUS__) && (__CORE_CORTEXM0PLUS__ == 1)
    #define __ARM_ARCH_6M__ 1
  #elif defined(__CORE_CORTEXM7__) && (__CORE_CORTEXM7__ == 1)
    #define __ARM_ARCH_7M__ 1
  #elif defined(__CORE_CORTEXM33__) && (__CORE_CORTEXM33__ == 1)
    #define __ARM_ARCH_8M_MAIN__ 1
  #elif defined(__CORE_CORTEXM23__) && (__CORE_CORTEXM23__ == 1)
    #define __ARM_ARCH_8M_MAIN__ 1
  #else
    #error "Unknown target."
  #endif
#endif

/* CMSIS compiler specific defines */
#ifndef   __ASM
  #define __ASM                                  __asm
#endif
#ifndef   __INLINE
  #define __INLINE                               __inline
#endif
#ifndef   __STATIC_INLINE
  #define __STATIC_INLINE                        static __inline
#endif
#ifndef   __STATIC_FORCEINLINE
  #define __STATIC_FORCEINLINE                   __attribute__((always_inline)) static __inline
#endif
#ifndef   __NO_RETURN
  #define __NO_RETURN                            __attribute__((noreturn))
#endif
#ifndef   __USED
  #define __USED                                 __attribute__((used))
#endif
#ifndef   __WEAK
  #define __WEAK                                 __attribute__((weak))
#endif
#ifndef   __PACKED
  #define __PACKED                               __attribute__((packed))
#endif
#ifndef   __PACKED_STRUCT
  #define __PACKED_STRUCT                        struct __attribute__((packed))
#endif
#ifndef   __PACKED_UNION
  #define __PACKED_UNION                         union __attribute__((packed))
#endif
#ifndef   __UNALIGNED_UINT32        /* deprecated */
  __PACKED_STRUCT T_UINT32 { uint32_t v; };
  #define __UNALIGNED_UINT32(x)                  (((struct T_UINT32 *)(x))->v)
#endif
#ifndef   __UNALIGNED_UINT16_WRITE
  __PACKED_STRUCT T_UINT16_WRITE { uint16_t v; };
  #define __UNALIGNED_UINT16_WRITE(addr, val)    (void)((((struct T_UINT16_WRITE *)(void *)(addr))->v) = (val))
#endif
#ifndef   __UNALIGNED_UINT16_READ
  __PACKED_STRUCT T_UINT16_READ { uint16_t v; };
  #define __UNALIGNED_UINT16_READ(addr)          (((const struct T_UINT16_READ *)(const void *)(addr))->v)
#endif
#ifndef   __UNALIGNED_UINT32_WRITE
  __PACKED_STRUCT T_UINT32_WRITE { uint32_t v; };
  #define __UNALIGNED_UINT32_WRITE(addr, val)    (void)((((struct T_UINT32_WRITE *)(void *)(addr))->v) = (val))
#endif
#ifndef   __UNALIGNED_UINT32_READ
  __PACKED_STRUCT T_UINT32_READ { uint32_t v; };
  #define __UNALIGNED_UINT32_READ(addr)          (((const struct T_UINT32_READ *)(const void *)(addr))->v)
#endif
#ifndef   __ALIGNED
  #define __ALIGNED(x)                           __attribute__((aligned(x)))
#endif
#ifndef   __RESTRICT
  #define __RESTRICT                             restrict
#endif
#ifndef   __COMPILER_BARRIER
  #include "ghs_barrier.h"
  #define __COMPILER_BARRIER()                   __memory_changed()
#endif

#ifndef __PROGRAM_START
#define __PROGRAM_START           _start
#endif

#ifndef __INITIAL_SP
#define __INITIAL_SP              __StackTop
#endif

#ifndef __STACK_LIMIT
#define __STACK_LIMIT             __StackLimit
#endif

#ifndef __VECTOR_TABLE
#define __VECTOR_TABLE            __Vectors
#endif

#ifndef __VECTOR_TABLE_ATTRIBUTE
#define __VECTOR_TABLE_ATTRIBUTE  __attribute__((used, section(".vectors")))
#endif

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
#ifndef __STACK_SEAL
#define __STACK_SEAL              __StackSeal
#endif

#ifndef __TZ_STACK_SEAL_SIZE
#define __TZ_STACK_SEAL_SIZE      8U
#endif

#ifndef __TZ_STACK_SEAL_VALUE
#define __TZ_STACK_SEAL_VALUE     0xFEF5EDA5FEF5EDA5ULL
#endif


__STATIC_FORCEINLINE void __TZ_set_STACKSEAL_S (uint32_t* stackTop) {
  *((uint64 *)stackTop) = __TZ_STACK_SEAL_VALUE;
}
#endif

/* ##########################  Core Instruction Access  ######################### */
/** \defgroup CMSIS_Core_InstructionInterface CMSIS Core Instruction Interface
  Access to dedicated instructions
  @{
*/
#include "arm_ghs.h"

/*@}*/ /* end of group CMSIS_Core_InstructionInterface */


/* ###########################  Core Function Access  ########################### */
/** \ingroup  CMSIS_Core_FunctionInterface
    \defgroup CMSIS_Core_RegAccFunctions CMSIS Core Register Access Functions
  @{
 */

/**
  \brief   Enable IRQ Interrupts
  \details Enables IRQ interrupts by clearing special-purpose register PRIMASK.
           Can only be executed in Privileged modes.
 */
__STATIC_FORCEINLINE void __enable_irq(void)
{
  __ASM ("cpsie i");
}


/**
  \brief   Disable IRQ Interrupts
  \details Disables IRQ interrupts by setting special-purpose register PRIMASK.
           Can only be executed in Privileged modes.
 */
__STATIC_FORCEINLINE void __disable_irq(void)
{
  __ASM ("cpsid i");
}


/**
  \brief   Get Control Register
  \details Returns the content of the Control Register.
  \return               Control Register value
 */
__STATIC_FORCEINLINE uint32_t __get_CONTROL(void)
{
  return __MRS(__CONTROL);
}


#if (defined (__ARM_FEATURE_CMSE ) && (__ARM_FEATURE_CMSE == 3))
/**
  \brief   Get Control Register (non-secure)
  \details Returns the content of the non-secure Control Register when in secure mode.
  \return               non-secure Control Register value
 */
__STATIC_FORCEINLINE uint32_t __TZ_get_CONTROL_NS(void)
{
  return __MRS(__CONTROL_NS);
}
#endif


/**
  \brief   Set Control Register
  \details Writes the given value to the Control Register.
  \param [in]    control  Control Register value to set
 */
__STATIC_FORCEINLINE void __set_CONTROL(uint32_t control)
{
  __MSR(__CONTROL, control);
  __ISB();
}


#if (defined (__ARM_FEATURE_CMSE ) && (__ARM_FEATURE_CMSE == 3))
/**
  \brief   Set Control Register (non-secure)
  \details Writes the given value to the non-secure Control Register when in secure state.
  \param [in]    control  Control Register value to set
 */
__STATIC_FORCEINLINE void __TZ_set_CONTROL_NS(uint32_t control)
{
  __MSR(__CONTROL_NS, control);
  __ISB();
}
#endif


/**
  \brief   Get IPSR Register
  \details Returns the content of the IPSR Register.
  \return               IPSR Register value
 */
__STATIC_FORCEINLINE uint32_t __get_IPSR(void)
{
  return __MRS(__IPSR);
}


/**
  \brief   Get APSR Register
  \details Returns the content of the APSR Register.
  \return               APSR Register value
 */
__STATIC_FORCEINLINE uint32_t __get_APSR(void)
{
  return __MRS(__APSR);
}


/**
  \brief   Get xPSR Register
  \details Returns the content of the xPSR Register.
  \return               xPSR Register value
 */
__STATIC_FORCEINLINE uint32_t __get_xPSR(void)
{
  return __MRS(__XPSR);
}


/**
  \brief   Get Process Stack Pointer
  \details Returns the current value of the Process Stack Pointer (PSP).
  \return               PSP Register value
 */
__STATIC_FORCEINLINE uint32_t __get_PSP(void)
{
  return __MRS(__PSP);
}


#if (defined (__ARM_FEATURE_CMSE ) && (__ARM_FEATURE_CMSE == 3))
/**
  \brief   Get Process Stack Pointer (non-secure)
  \details Returns the current value of the non-secure Process Stack Pointer (PSP) when in secure state.
  \return               PSP Register value
 */
__STATIC_FORCEINLINE uint32_t __TZ_get_PSP_NS(void)
{
  return __MRS(__PSP_NS);
}
#endif


/**
  \brief   Set Process Stack Pointer
  \details Assigns the given value to the Process Stack Pointer (PSP).
  \param [in]    topOfProcStack  Process Stack Pointer value to set
 */
__STATIC_FORCEINLINE void __set_PSP(uint32_t topOfProcStack)
{
  __MSR(__PSP, topOfProcStack);
}


#if (defined (__ARM_FEATURE_CMSE ) && (__ARM_FEATURE_CMSE == 3))
/**
  \brief   Set Process Stack Pointer (non-secure)
  \details Assigns the given value to the non-secure Process Stack Pointer (PSP) when in secure state.
  \param [in]    topOfProcStack  Process Stack Pointer value to set
 */
__STATIC_FORCEINLINE void __TZ_set_PSP_NS(uint32_t topOfProcStack)
{
  __MSR(__PSP_NS, topOfProcStack);
}
#endif


/**
  \brief   Get Main Stack Pointer
  \details Returns the current value of the Main Stack Pointer (MSP).
  \return               MSP Register value
 */
__STATIC_FORCEINLINE uint32_t __get_MSP(void)
{
  return __MRS(__MSP);
}


#if (defined (__ARM_FEATURE_CMSE ) && (__ARM_FEATURE_CMSE == 3))
/**
  \brief   Get Main Stack Pointer (non-secure)
  \details Returns the current value of the non-secure Main Stack Pointer (MSP) when in secure state.
  \return               MSP Register value
 */
__STATIC_FORCEINLINE uint32_t __TZ_get_MSP_NS(void)
{
  return __MRS(__MSP_NS);
}
#endif


/**
  \brief   Set Main Stack Pointer
  \details Assigns the given value to the Main Stack Pointer (MSP).
  \param [in]    topOfMainStack  Main Stack Pointer value to set
 */
__STATIC_FORCEINLINE void __set_MSP(uint32_t topOfMainStack)
{
  __MSR(__MSP, topOfMainStack);
}


#if (defined (__ARM_FEATURE_CMSE ) && (__ARM_FEATURE_CMSE == 3))
/**
  \brief   Set Main Stack Pointer (non-secure)
  \details Assigns the given value to the non-secure Main Stack Pointer (MSP) when in secure state.
  \param [in]    topOfMainStack  Main Stack Pointer value to set
 */
__STATIC_FORCEINLINE void __TZ_set_MSP_NS(uint32_t topOfMainStack)
{
  __MSR(__MSP_NS, topOfMainStack);
}
#endif


#if (defined (__ARM_FEATURE_CMSE ) && (__ARM_FEATURE_CMSE == 3))
/**
  \brief   Get Stack Pointer (non-secure)
  \details Returns the current value of the non-secure Stack Pointer (SP) when in secure state.
  \return               SP Register value
 */
__STATIC_FORCEINLINE uint32_t __TZ_get_SP_NS(void)
{
  return __MRS(__SP_NS);
}


/**
  \brief   Set Stack Pointer (non-secure)
  \details Assigns the given value to the non-secure Stack Pointer (SP) when in secure state.
  \param [in]    topOfStack  Stack Pointer value to set
 */
__STATIC_FORCEINLINE void __TZ_set_SP_NS(uint32_t topOfStack)
{
  __MSR(__SP_NS, topOfStack);
}
#endif


/**
  \brief   Get Priority Mask
  \details Returns the current state of the priority mask bit from the Priority Mask Register.
  \return               Priority Mask value
 */
__STATIC_FORCEINLINE uint32_t __get_PRIMASK(void)
{
  return __MRS(__PRIMASK);
}


#if (defined (__ARM_FEATURE_CMSE ) && (__ARM_FEATURE_CMSE == 3))
/**
  \brief   Get Priority Mask (non-secure)
  \details Returns the current state of the non-secure priority mask bit from the Priority Mask Register when in secure state.
  \return               Priority Mask value
 */
__STATIC_FORCEINLINE uint32_t __TZ_get_PRIMASK_NS(void)
{
  return __MRS(__PRIMASK_NS);
}
#endif


/**
  \brief   Set Priority Mask
  \details Assigns the given value to the Priority Mask Register.
  \param [in]    priMask  Priority Mask
 */
__STATIC_FORCEINLINE void __set_PRIMASK(uint32_t priMask)
{
  __MSR(__PRIMASK, priMask);
}


#if (defined (__ARM_FEATURE_CMSE ) && (__ARM_FEATURE_CMSE == 3))
/**
  \brief   Set Priority Mask (non-secure)
  \details Assigns the given value to the non-secure Priority Mask Register when in secure state.
  \param [in]    priMask  Priority Mask
 */
__STATIC_FORCEINLINE void __TZ_set_PRIMASK_NS(uint32_t priMask)
{
  __MSR(__PRIMASK_NS, priMask);
}
#endif


#if ((defined (__ARM_ARCH_7M__      ) && (__ARM_ARCH_7M__      == 1)) || \
     (defined (__ARM_ARCH_7EM__     ) && (__ARM_ARCH_7EM__     == 1)) || \
     (defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1))    )
/**
  \brief   Enable FIQ
  \details Enables FIQ interrupts by clearing special-purpose register FAULTMASK.
           Can only be executed in Privileged modes.
 */
__STATIC_FORCEINLINE void __enable_fault_irq(void)
{
  __ASM ("cpsie f");
}


/**
  \brief   Disable FIQ
  \details Disables FIQ interrupts by setting special-purpose register FAULTMASK.
           Can only be executed in Privileged modes.
 */
__STATIC_FORCEINLINE void __disable_fault_irq(void)
{
  __ASM ("cpsid f");
}


/**
  \brief   Get Base Priority
  \details Returns the current value of the Base Priority register.
  \return               Base Priority register value
 */
__STATIC_FORCEINLINE uint32_t __get_BASEPRI(void)
{
  return __MRS(__BASEPRI);
}


#if (defined (__ARM_FEATURE_CMSE ) && (__ARM_FEATURE_CMSE == 3))
/**
  \brief   Get Base Priority (non-secure)
  \details Returns the current value of the non-secure Base Priority register when in secure state.
  \return               Base Priority register value
 */
__STATIC_FORCEINLINE uint32_t __TZ_get_BASEPRI_NS(void)
{
  return __MRS(__BASEPRI_NS);
}
#endif


/**
  \brief   Set Base Priority
  \details Assigns the given value to the Base Priority register.
  \param [in]    basePri  Base Priority value to set
 */
__STATIC_FORCEINLINE void __set_BASEPRI(uint32_t basePri)
{
  __MSR(__BASEPRI, basePri);
}


#if (defined (__ARM_FEATURE_CMSE ) && (__ARM_FEATURE_CMSE == 3))
/**
  \brief   Set Base Priority (non-secure)
  \details Assigns the given value to the non-secure Base Priority register when in secure state.
  \param [in]    basePri  Base Priority value to set
 */
__STATIC_FORCEINLINE void __TZ_set_BASEPRI_NS(uint32_t basePri)
{
  __MSR(__BASEPRI_NS, basePri);
}
#endif


/**
  \brief   Set Base Priority with condition
  \details Assigns the given value to the Base Priority register only if BASEPRI masking is disabled,
           or the new value increases the BASEPRI priority level.
  \param [in]    basePri  Base Priority value to set
 */
__STATIC_FORCEINLINE void __set_BASEPRI_MAX(uint32_t basePri)
{
  __MSR(__BASEPRI_MAX, basePri);
}


/**
  \brief   Get Fault Mask
  \details Returns the current value of the Fault Mask register.
  \return               Fault Mask register value
 */
__STATIC_FORCEINLINE uint32_t __get_FAULTMASK(void)
{
  return __MRS(__FAULTMASK);
}


#if (defined (__ARM_FEATURE_CMSE ) && (__ARM_FEATURE_CMSE == 3))
/**
  \brief   Get Fault Mask (non-secure)
  \details Returns the current value of the non-secure Fault Mask register when in secure state.
  \return               Fault Mask register value
 */
__STATIC_FORCEINLINE uint32_t __TZ_get_FAULTMASK_NS(void)
{
  return __MRS(__FAULTMASK_NS);
}
#endif


/**
  \brief   Set Fault Mask
  \details Assigns the given value to the Fault Mask register.
  \param [in]    faultMask  Fault Mask value to set
 */
__STATIC_FORCEINLINE void __set_FAULTMASK(uint32_t faultMask)
{
  __MSR(__FAULTMASK, faultMask);
}


#if (defined (__ARM_FEATURE_CMSE ) && (__ARM_FEATURE_CMSE == 3))
/**
  \brief   Set Fault Mask (non-secure)
  \details Assigns the given value to the non-secure Fault Mask register when in secure state.
  \param [in]    faultMask  Fault Mask value to set
 */
__STATIC_FORCEINLINE void __TZ_set_FAULTMASK_NS(uint32_t faultMask)
{
  __MSR(__FAULTMASK_NS, faultMask);
}
#endif

#endif /* ((defined (__ARM_ARCH_7M__      ) && (__ARM_ARCH_7M__      == 1)) || \
           (defined (__ARM_ARCH_7EM__     ) && (__ARM_ARCH_7EM__     == 1)) || \
           (defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1))    ) */


#if ((defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1)) || \
     (defined (__ARM_ARCH_8M_BASE__ ) && (__ARM_ARCH_8M_BASE__ == 1))    )

/**
  \brief   Get Process Stack Pointer Limit
  Devices without ARMv8-M Main Extensions (i.e. Cortex-M23) lack the non-secure
  Stack Pointer Limit register hence zero is returned always in non-secure
  mode.

  \details Returns the current value of the Process Stack Pointer Limit (PSPLIM).
  \return               PSPLIM Register value
 */
__STATIC_FORCEINLINE uint32_t __get_PSPLIM(void)
{
#if (!(defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1)) && \
    (!defined (__ARM_FEATURE_CMSE) || (__ARM_FEATURE_CMSE < 3)))
    // without main extensions, the non-secure PSPLIM is RAZ/WI
  return 0U;
#else
  return __MRS(__PSPLIM);
#endif
}

#if (defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3))
/**
  \brief   Get Process Stack Pointer Limit (non-secure)
  Devices without ARMv8-M Main Extensions (i.e. Cortex-M23) lack the non-secure
  Stack Pointer Limit register hence zero is returned always.

  \details Returns the current value of the non-secure Process Stack Pointer Limit (PSPLIM) when in secure state.
  \return               PSPLIM Register value
 */
__STATIC_FORCEINLINE uint32_t __TZ_get_PSPLIM_NS(void)
{
#if (!(defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1)))
  // without main extensions, the non-secure PSPLIM is RAZ/WI
  return 0U;
#else
  return __MRS(__PSPLIM_NS);
#endif
}
#endif


/**
  \brief   Set Process Stack Pointer Limit
  Devices without ARMv8-M Main Extensions (i.e. Cortex-M23) lack the non-secure
  Stack Pointer Limit register hence the write is silently ignored in non-secure
  mode.

  \details Assigns the given value to the Process Stack Pointer Limit (PSPLIM).
  \param [in]    ProcStackPtrLimit  Process Stack Pointer Limit value to set
 */
__STATIC_FORCEINLINE void __set_PSPLIM(uint32_t ProcStackPtrLimit)
{
#if (!(defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1)) && \
    (!defined (__ARM_FEATURE_CMSE) || (__ARM_FEATURE_CMSE < 3)))
  // without main extensions, the non-secure PSPLIM is RAZ/WI
  (void)ProcStackPtrLimit;
#else
  __MSR(__PSPLIM, ProcStackPtrLimit);
#endif
}


#if (defined (__ARM_FEATURE_CMSE  ) && (__ARM_FEATURE_CMSE   == 3))
/**
  \brief   Set Process Stack Pointer (non-secure)
  Devices without ARMv8-M Main Extensions (i.e. Cortex-M23) lack the non-secure
  Stack Pointer Limit register hence the write is silently ignored.

  \details Assigns the given value to the non-secure Process Stack Pointer Limit (PSPLIM) when in secure state.
  \param [in]    ProcStackPtrLimit  Process Stack Pointer Limit value to set
 */
__STATIC_FORCEINLINE void __TZ_set_PSPLIM_NS(uint32_t ProcStackPtrLimit)
{
#if (!(defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1)))
  // without main extensions, the non-secure PSPLIM is RAZ/WI
  (void)ProcStackPtrLimit;
#else
  __MSR(__PSPLIM_NS, ProcStackPtrLimit);
#endif
}
#endif


/**
  \brief   Get Main Stack Pointer Limit
  Devices without ARMv8-M Main Extensions (i.e. Cortex-M23) lack the non-secure
  Stack Pointer Limit register hence zero is returned always in non-secure
  mode.

  \details Returns the current value of the Main Stack Pointer Limit (MSPLIM).
  \return               MSPLIM Register value
 */
__STATIC_FORCEINLINE uint32_t __get_MSPLIM(void)
{
#if (!(defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1)) && \
    (!defined (__ARM_FEATURE_CMSE) || (__ARM_FEATURE_CMSE < 3)))
  // without main extensions, the non-secure MSPLIM is RAZ/WI
  return 0U;
#else
  return __MRS(__MSPLIM);
#endif
}


#if (defined (__ARM_FEATURE_CMSE  ) && (__ARM_FEATURE_CMSE   == 3))
/**
  \brief   Get Main Stack Pointer Limit (non-secure)
  Devices without ARMv8-M Main Extensions (i.e. Cortex-M23) lack the non-secure
  Stack Pointer Limit register hence zero is returned always.

  \details Returns the current value of the non-secure Main Stack Pointer Limit(MSPLIM) when in secure state.
  \return               MSPLIM Register value
 */
__STATIC_FORCEINLINE uint32_t __TZ_get_MSPLIM_NS(void)
{
#if (!(defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1)))
  // without main extensions, the non-secure MSPLIM is RAZ/WI
  return 0U;
#else
  return __MRS(__MSPLIM_NS);
#endif
}
#endif


/**
  \brief   Set Main Stack Pointer Limit
  Devices without ARMv8-M Main Extensions (i.e. Cortex-M23) lack the non-secure
  Stack Pointer Limit register hence the write is silently ignored in non-secure
  mode.

  \details Assigns the given value to the Main Stack Pointer Limit (MSPLIM).
  \param [in]    MainStackPtrLimit  Main Stack Pointer Limit value to set
 */
__STATIC_FORCEINLINE void __set_MSPLIM(uint32_t MainStackPtrLimit)
{
#if (!(defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1)) && \
    (!defined (__ARM_FEATURE_CMSE) || (__ARM_FEATURE_CMSE < 3)))
  // without main extensions, the non-secure MSPLIM is RAZ/WI
  (void)MainStackPtrLimit;
#else
  __MSR(__MSPLIM, MainStackPtrLimit);
#endif
}


#if (defined (__ARM_FEATURE_CMSE  ) && (__ARM_FEATURE_CMSE   == 3))
/**
  \brief   Set Main Stack Pointer Limit (non-secure)
  Devices without ARMv8-M Main Extensions (i.e. Cortex-M23) lack the non-secure
  Stack Pointer Limit register hence the write is silently ignored.

  \details Assigns the given value to the non-secure Main Stack Pointer Limit (MSPLIM) when in secure state.
  \param [in]    MainStackPtrLimit  Main Stack Pointer value to set
 */
__STATIC_FORCEINLINE void __TZ_set_MSPLIM_NS(uint32_t MainStackPtrLimit)
{
#if (!(defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1)))
  // without main extensions, the non-secure MSPLIM is RAZ/WI
  (void)MainStackPtrLimit;
#else
  __MSR(__MSPLIM_NS, MainStackPtrLimit);
#endif
}
#endif

#endif /* ((defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1)) || \
           (defined (__ARM_ARCH_8M_BASE__ ) && (__ARM_ARCH_8M_BASE__ == 1))    ) */


/**
  \brief   Get FPSCR
  \details Returns the current value of the Floating Point Status/Control register.
  \return               Floating Point Status/Control register value
 */
__STATIC_FORCEINLINE uint32_t __get_FPSCR(void)
{
#if ((defined (__FPU_PRESENT) && (__FPU_PRESENT == 1U)) && \
     (defined (__FPU_USED   ) && (__FPU_USED    == 1U))     )
  return __VMRS(__VFP_FPSCR);
#else
  return(0U);
#endif
}


/**
  \brief   Set FPSCR
  \details Assigns the given value to the Floating Point Status/Control register.
  \param [in]    fpscr  Floating Point Status/Control value to set
 */
__STATIC_FORCEINLINE void __set_FPSCR(uint32_t fpscr)
{
#if ((defined (__FPU_PRESENT) && (__FPU_PRESENT == 1U)) && \
     (defined (__FPU_USED   ) && (__FPU_USED    == 1U))     )
  __VMSR(__VFP_FPSCR, fpscr);
#else
  (void)fpscr;
#endif
}


/*@} end of CMSIS_Core_RegAccFunctions */


#endif /* __CMSIS_CCARM_H */

