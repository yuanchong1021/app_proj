/*
** ###################################################################
**     Processor:           YTM32B1MC0
**     Version:             rev. 1.3, 2025-08-12
**
**     Abstract:
**         Peripheral Access Layer for YTM32B1MC0
**
**     Copyright (c) 2020 - 2025 Yuntu Microelectronics Co., Ltd
**     All rights reserved.
**
** ###################################################################
*/

/*!
 * @file YTM32B1MC0.h
 * @version 1.3
 * @date 2025-08-12
 * @brief Peripheral Access Layer for YTM32B1MC0
 *
 * This file contains register definitions and macros for easy access to their
 * bit fields.
 *
 * This file assumes LITTLE endian system.
 */

/*!
 * @page misra_violations MISRA-C:2012 violations list
 *
 * PRQA S 0603 Rule 21.1: The macro identifier '%1s' is reserved.
 *
 * PRQA S 0602 Rule 1.3: The identifier '%1s' is reserved for use by the library.
 *
 * PRQA S 0750 Rule 19.2: A union type specifier has been defined.
 *
 * PRQA S 1039 Rule 1.2: Treating array of length one as potentially flexible member.
 */

/* ----------------------------------------------------------------------------
   -- MCU activation
   ---------------------------------------------------------------------------- */

/* Prevention from multiple including the same memory map */
#ifndef YTM32B1MC0_H  /* Check if memory map has not been already included */
#define YTM32B1MC0_H
#define MCU_YTM32B1MC0

/* Check if another memory map has not been also included */
#if (defined(MCU_ACTIVE))
  #error YTM32B1MC0 memory map: There is already included another memory map. Only one memory map can be included.
#endif /* (defined(MCU_ACTIVE)) */
#define MCU_ACTIVE

/** Memory map major version (memory maps with equal major version number are
 * compatible) */
#define MCU_MEM_MAP_VERSION 0x0100u
/** Memory map minor version */
#define MCU_MEM_MAP_VERSION_MINOR 0x0003u

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* ----------------------------------------------------------------------------
   -- Generic macros
   ---------------------------------------------------------------------------- */


/**
* @brief 32 bits memory read macro.
*/
#if !defined(REG_READ32)
  #define REG_READ32(address)               (*(volatile uint32_t*)(address))
#endif

/**
* @brief 32 bits memory write macro.
*/
#if !defined(REG_WRITE32)
  #define REG_WRITE32(address, value)       ((*(volatile uint32_t*)(address))= (uint32_t)(value))
#endif

/**
* @brief 32 bits bits setting macro.
*/
#if !defined(REG_BIT_SET32)
  #define REG_BIT_SET32(address, mask)      ((*(volatile uint32_t*)(address))|= (uint32_t)(mask))
#endif

/**
* @brief 32 bits bits clearing macro.
*/
#if !defined(REG_BIT_CLEAR32)
  #define REG_BIT_CLEAR32(address, mask)    ((*(volatile uint32_t*)(address))&= ((uint32_t)~((uint32_t)(mask))))
#endif

/**
* @brief 32 bit clear bits and set with new value
* @note It is user's responsability to make sure that value has only "mask" bits set - (value&~mask)==0
*/
#if !defined(REG_RMW32)
  #define REG_RMW32(address, mask, value)   (REG_WRITE32((address), ((REG_READ32(address)& ((uint32_t)~((uint32_t)(mask))))| ((uint32_t)(value)))))
#endif
/* ----------------------------------------------------------------------------
   -- Interrupt vector numbers for YTM32B1MC0
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup Interrupt_vector_numbers_YTM32B1L Interrupt vector numbers for YTM32B1L
 * @{
 */

/** Interrupt Number Definitions */
#define NUMBER_OF_INT_VECTORS 174u          /**< Number of interrupts in the Vector table */

/**
 * @brief Defines the Interrupt Numbers definitions
 *
 * This enumeration is used to configure the interrupts.
 *
 * Implements : IRQn_Type_Class
 */
typedef enum
{
  /* Auxiliary constants */
  NotAvail_IRQn               = -128,                /**< Not available device specific interrupt */

  /* Core interrupts */
  NMI_IRQn                    =  -14,                /**< NMI Interrupt */ 
  HardFault_IRQn              =  -13,                /**< HardFault Interrupt */ 
  MemManage_IRQn              =  -12,                /**< MemManage Interrupt */ 
  BusFault_IRQn               =  -11,                /**< BusFault Interrupt */ 
  UsageFault_IRQn             =  -10,                /**< UsageFault Interrupt */ 
  SVC_IRQn                    =   -5,                /**< SVC Interrupt */ 
  DebugMon_IRQn               =   -4,                /**< DebugMon Interrupt */ 
  PendSV_IRQn                 =   -2,                /**< PendSV Interrupt */ 
  SysTick_IRQn                =   -1,                /**< SysTick Interrupt */ 
  /* Device specific interrupts */
  DMA0_IRQn                   =    0,                /**< DMA0 Interrupt */ 
  DMA1_IRQn                   =    1,                /**< DMA1 Interrupt */ 
  DMA2_IRQn                   =    2,                /**< DMA2 Interrupt */ 
  DMA3_IRQn                   =    3,                /**< DMA3 Interrupt */ 
  Reserved5_IRQn              =    4,                /**< Reserved5 Interrupt */ 
  Reserved6_IRQn              =    5,                /**< Reserved6 Interrupt */ 
  Reserved7_IRQn              =    6,                /**< Reserved7 Interrupt */ 
  Reserved8_IRQn              =    7,                /**< Reserved8 Interrupt */ 
  Reserved9_IRQn              =    8,                /**< Reserved9 Interrupt */ 
  Reserved10_IRQn             =    9,                /**< Reserved10 Interrupt */ 
  Reserved11_IRQn             =   10,                /**< Reserved11 Interrupt */ 
  Reserved12_IRQn             =   11,                /**< Reserved12 Interrupt */ 
  Reserved13_IRQn             =   12,                /**< Reserved13 Interrupt */ 
  Reserved14_IRQn             =   13,                /**< Reserved14 Interrupt */ 
  Reserved15_IRQn             =   14,                /**< Reserved15 Interrupt */ 
  Reserved16_IRQn             =   15,                /**< Reserved16 Interrupt */ 
  DMA_Error_IRQn              =   16,                /**< DMA_Error Interrupt */ 
  Reserved17_IRQn             =   17,                /**< Reserved17 Interrupt */ 
  EFM_IRQn                    =   18,                /**< EFM Interrupt */ 
  EFM_Error_IRQn              =   19,                /**< EFM_Error Interrupt */ 
  PCU_IRQn                    =   20,                /**< PCU Interrupt */ 
  EFM_Ecc_IRQn                =   21,                /**< EFM_Ecc Interrupt */ 
  WDG0_IRQn                   =   22,                /**< WDG0 Interrupt */ 
  Reserved18_IRQn             =   23,                /**< Reserved18 Interrupt */ 
  I2C0_Master_IRQn            =   24,                /**< I2C0_Master Interrupt */ 
  I2C0_Slave_IRQn             =   25,                /**< I2C0_Slave Interrupt */ 
  SPI0_IRQn                   =   26,                /**< SPI0 Interrupt */ 
  SPI1_IRQn                   =   27,                /**< SPI1 Interrupt */ 
  SPI2_IRQn                   =   28,                /**< SPI2 Interrupt */ 
  I2C1_Master_IRQn            =   29,                /**< I2C1_Master Interrupt */ 
  Reserved19_IRQn             =   30,                /**< Reserved19 Interrupt */ 
  UART0_IRQn                  =   31,                /**< UART0 Interrupt */ 
  Reserved20_IRQn             =   32,                /**< Reserved20 Interrupt */ 
  UART1_IRQn                  =   33,                /**< UART1 Interrupt */ 
  Reserved21_IRQn             =   34,                /**< Reserved21 Interrupt */ 
  UART2_IRQn                  =   35,                /**< UART2 Interrupt */ 
  Reserved22_IRQn             =   36,                /**< Reserved22 Interrupt */ 
  Reserved23_IRQn             =   37,                /**< Reserved23 Interrupt */ 
  Reserved24_IRQn             =   38,                /**< Reserved24 Interrupt */ 
  ADC0_IRQn                   =   39,                /**< ADC0 Interrupt */ 
  Reserved25_IRQn             =   40,                /**< Reserved25 Interrupt */ 
  ACMP0_IRQn                  =   41,                /**< ACMP0 Interrupt */ 
  Reserved26_IRQn             =   42,                /**< Reserved26 Interrupt */ 
  Reserved27_IRQn             =   43,                /**< Reserved27 Interrupt */ 
  EMU0_SB_IRQn                =   44,                /**< EMU0_SB Interrupt */ 
  EMU0_DB_IRQn                =   45,                /**< EMU0_DB Interrupt */ 
  Reserved28_IRQn             =   46,                /**< Reserved28 Interrupt */ 
  Reserved29_IRQn             =   47,                /**< Reserved29 Interrupt */ 
  pTMR0_Ch0_IRQn              =   48,                /**< pTMR0_Ch0 Interrupt */ 
  pTMR0_Ch1_IRQn              =   49,                /**< pTMR0_Ch1 Interrupt */ 
  pTMR0_Ch2_IRQn              =   50,                /**< pTMR0_Ch2 Interrupt */ 
  pTMR0_Ch3_IRQn              =   51,                /**< pTMR0_Ch3 Interrupt */ 
  Reserved30_IRQn             =   52,                /**< Reserved30 Interrupt */ 
  Reserved31_IRQn             =   53,                /**< Reserved31 Interrupt */ 
  Reserved32_IRQn             =   54,                /**< Reserved32 Interrupt */ 
  Reserved33_IRQn             =   55,                /**< Reserved33 Interrupt */ 
  Reserved34_IRQn             =   56,                /**< Reserved34 Interrupt */ 
  SCU_IRQn                    =   57,                /**< SCU Interrupt */ 
  lpTMR0_IRQn                 =   58,                /**< lpTMR0 Interrupt */ 
  GPIOA_IRQn                  =   59,                /**< GPIOA Interrupt */ 
  GPIOB_IRQn                  =   60,                /**< GPIOB Interrupt */ 
  GPIOC_IRQn                  =   61,                /**< GPIOC Interrupt */ 
  GPIOD_IRQn                  =   62,                /**< GPIOD Interrupt */ 
  GPIOE_IRQn                  =   63,                /**< GPIOE Interrupt */ 
  Reserved35_IRQn             =   64,                /**< Reserved35 Interrupt */ 
  Reserved36_IRQn             =   65,                /**< Reserved36 Interrupt */ 
  Reserved37_IRQn             =   66,                /**< Reserved37 Interrupt */ 
  Reserved38_IRQn             =   67,                /**< Reserved38 Interrupt */ 
  Reserved39_IRQn             =   68,                /**< Reserved39 Interrupt */ 
  Reserved40_IRQn             =   69,                /**< Reserved40 Interrupt */ 
  Reserved41_IRQn             =   70,                /**< Reserved41 Interrupt */ 
  Reserved42_IRQn             =   71,                /**< Reserved42 Interrupt */ 
  Reserved43_IRQn             =   72,                /**< Reserved43 Interrupt */ 
  Reserved44_IRQn             =   73,                /**< Reserved44 Interrupt */ 
  Reserved45_IRQn             =   74,                /**< Reserved45 Interrupt */ 
  Reserved46_IRQn             =   75,                /**< Reserved46 Interrupt */ 
  Reserved47_IRQn             =   76,                /**< Reserved47 Interrupt */ 
  Reserved48_IRQn             =   77,                /**< Reserved48 Interrupt */ 
  CAN0_ORed_IRQn              =   78,                /**< CAN0_ORed Interrupt */ 
  CAN0_Error_IRQn             =   79,                /**< CAN0_Error Interrupt */ 
  CAN0_Wake_Up_IRQn           =   80,                /**< CAN0_Wake_Up Interrupt */ 
  CAN0_ORed_0_15_MB_IRQn      =   81,                /**< CAN0_ORed_0_15_MB Interrupt */ 
  CAN0_ORed_16_31_MB_IRQn     =   82,                /**< CAN0_ORed_16_31_MB Interrupt */ 
  CAN0_ORed_32_47_MB_IRQn     =   83,                /**< CAN0_ORed_32_47_MB Interrupt */ 
  CAN0_ORed_48_63_MB_IRQn     =   84,                /**< CAN0_ORed_48_63_MB Interrupt */ 
  CAN1_ORed_IRQn              =   85,                /**< CAN1_ORed Interrupt */ 
  CAN1_Error_IRQn             =   86,                /**< CAN1_Error Interrupt */ 
  CAN1_Wake_Up_IRQn           =   87,                /**< CAN1_Wake_Up Interrupt */ 
  CAN1_ORed_0_15_MB_IRQn      =   88,                /**< CAN1_ORed_0_15_MB Interrupt */ 
  CAN1_ORed_16_31_MB_IRQn     =   89,                /**< CAN1_ORed_16_31_MB Interrupt */ 
  CAN1_ORed_32_47_MB_IRQn     =   90,                /**< CAN1_ORed_32_47_MB Interrupt */ 
  CAN1_ORed_48_63_MB_IRQn     =   91,                /**< CAN1_ORed_48_63_MB Interrupt */ 
  Reserved49_IRQn             =   92,                /**< Reserved49 Interrupt */ 
  Reserved50_IRQn             =   93,                /**< Reserved50 Interrupt */ 
  Reserved51_IRQn             =   94,                /**< Reserved51 Interrupt */ 
  Reserved52_IRQn             =   95,                /**< Reserved52 Interrupt */ 
  Reserved53_IRQn             =   96,                /**< Reserved53 Interrupt */ 
  Reserved54_IRQn             =   97,                /**< Reserved54 Interrupt */ 
  Reserved55_IRQn             =   98,                /**< Reserved55 Interrupt */ 
  eTMR0_Ch0_Ch1_IRQn          =   99,                /**< eTMR0_Ch0_Ch1 Interrupt */ 
  eTMR0_Ch2_Ch3_IRQn          =  100,                /**< eTMR0_Ch2_Ch3 Interrupt */ 
  eTMR0_Ch4_Ch5_IRQn          =  101,                /**< eTMR0_Ch4_Ch5 Interrupt */ 
  eTMR0_Ch6_Ch7_IRQn          =  102,                /**< eTMR0_Ch6_Ch7 Interrupt */ 
  eTMR0_Fault_IRQn            =  103,                /**< eTMR0_Fault Interrupt */ 
  eTMR0_Ovf_IRQn              =  104,                /**< eTMR0_Ovf Interrupt */ 
  eTMR1_Ch0_Ch1_IRQn          =  105,                /**< eTMR1_Ch0_Ch1 Interrupt */ 
  eTMR1_Ch2_Ch3_IRQn          =  106,                /**< eTMR1_Ch2_Ch3 Interrupt */ 
  eTMR1_Ch4_Ch5_IRQn          =  107,                /**< eTMR1_Ch4_Ch5 Interrupt */ 
  eTMR1_Ch6_Ch7_IRQn          =  108,                /**< eTMR1_Ch6_Ch7 Interrupt */ 
  eTMR1_Fault_IRQn            =  109,                /**< eTMR1_Fault Interrupt */ 
  eTMR1_Ovf_IRQn              =  110,                /**< eTMR1_Ovf Interrupt */ 
  Reserved56_IRQn             =  111,                /**< Reserved56 Interrupt */ 
  Reserved57_IRQn             =  112,                /**< Reserved57 Interrupt */ 
  Reserved58_IRQn             =  113,                /**< Reserved58 Interrupt */ 
  Reserved59_IRQn             =  114,                /**< Reserved59 Interrupt */ 
  Reserved60_IRQn             =  115,                /**< Reserved60 Interrupt */ 
  Reserved61_IRQn             =  116,                /**< Reserved61 Interrupt */ 
  Reserved62_IRQn             =  117,                /**< Reserved62 Interrupt */ 
  Reserved63_IRQn             =  118,                /**< Reserved63 Interrupt */ 
  Reserved64_IRQn             =  119,                /**< Reserved64 Interrupt */ 
  Reserved65_IRQn             =  120,                /**< Reserved65 Interrupt */ 
  Reserved66_IRQn             =  121,                /**< Reserved66 Interrupt */ 
  Reserved67_IRQn             =  122,                /**< Reserved67 Interrupt */ 
  Reserved68_IRQn             =  123,                /**< Reserved68 Interrupt */ 
  Reserved69_IRQn             =  124,                /**< Reserved69 Interrupt */ 
  Reserved70_IRQn             =  125,                /**< Reserved70 Interrupt */ 
  Reserved71_IRQn             =  126,                /**< Reserved71 Interrupt */ 
  Reserved72_IRQn             =  127,                /**< Reserved72 Interrupt */ 
  Reserved73_IRQn             =  128,                /**< Reserved73 Interrupt */ 
  Reserved74_IRQn             =  129,                /**< Reserved74 Interrupt */ 
  Reserved75_IRQn             =  130,                /**< Reserved75 Interrupt */ 
  Reserved76_IRQn             =  131,                /**< Reserved76 Interrupt */ 
  Reserved77_IRQn             =  132,                /**< Reserved77 Interrupt */ 
  Reserved78_IRQn             =  133,                /**< Reserved78 Interrupt */ 
  Reserved79_IRQn             =  134,                /**< Reserved79 Interrupt */ 
  MPWM0_Ch0_Ovf_IRQn          =  135,                /**< MPWM0_Ch0_Ovf Interrupt */ 
  MPWM0_Ch1_Ovf_IRQn          =  136,                /**< MPWM0_Ch1_Ovf Interrupt */ 
  MPWM0_Ch2_Ovf_IRQn          =  137,                /**< MPWM0_Ch2_Ovf Interrupt */ 
  MPWM0_Ch3_Ovf_IRQn          =  138,                /**< MPWM0_Ch3_Ovf Interrupt */ 
  MPWM0_Ch4_Ovf_IRQn          =  139,                /**< MPWM0_Ch4_Ovf Interrupt */ 
  MPWM0_Ch5_Ovf_IRQn          =  140,                /**< MPWM0_Ch5_Ovf Interrupt */ 
  MPWM0_Ch6_Ovf_IRQn          =  141,                /**< MPWM0_Ch6_Ovf Interrupt */ 
  MPWM0_Ch7_Ovf_IRQn          =  142,                /**< MPWM0_Ch7_Ovf Interrupt */ 
  MPWM0_Ch0_IRQn              =  143,                /**< MPWM0_Ch0 Interrupt */ 
  MPWM0_Ch1_IRQn              =  144,                /**< MPWM0_Ch1 Interrupt */ 
  MPWM0_Ch2_IRQn              =  145,                /**< MPWM0_Ch2 Interrupt */ 
  MPWM0_Ch3_IRQn              =  146,                /**< MPWM0_Ch3 Interrupt */ 
  MPWM0_Ch4_IRQn              =  147,                /**< MPWM0_Ch4 Interrupt */ 
  MPWM0_Ch5_IRQn              =  148,                /**< MPWM0_Ch5 Interrupt */ 
  MPWM0_Ch6_IRQn              =  149,                /**< MPWM0_Ch6 Interrupt */ 
  MPWM0_Ch7_IRQn              =  150,                /**< MPWM0_Ch7 Interrupt */ 
  Reserved80_IRQn             =  151,                /**< Reserved80 Interrupt */ 
  Reserved81_IRQn             =  152,                /**< Reserved81 Interrupt */ 
  Reserved82_IRQn             =  153,                /**< Reserved82 Interrupt */ 
  Reserved83_IRQn             =  154,                /**< Reserved83 Interrupt */ 
  Reserved84_IRQn             =  155,                /**< Reserved84 Interrupt */ 
  TRNG_IRQn                   =  156,                /**< TRNG Interrupt */ 
  HCU_IRQn                    =  157,                /**< HCU Interrupt */ 
} IRQn_Type;

/*!
 * @}
 */ /* end of group Interrupt_vector_numbers YTM32B1MC0 */

/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */

/* -------  Start of section using anonymous unions and disabling warnings  ------- */
#if   defined (__CC_ARM)
  #pragma push
  #pragma anon_unions
#elif defined (__ICCARM__)
  #pragma language=extended
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wc11-extensions"
  #pragma clang diagnostic ignored "-Wreserved-id-macro"
#elif defined (__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined (__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined (__TASKING__)
  #pragma warning 586
#elif defined (__CSMC__)
  /* anonymous unions are enabled by default */
#elif defined (__ghs__)
  /* anonymous unions are enabled by default */
#else
  #pragma warning not supported compiler type
#endif


/* --------  Configuration of Core Peripherals  ----------------------------------- */
/*PRQA S 0602, 0603 ++*/
#define __CM33_REV                0x0004U   // Core revision r0p4
#define __SAUREGION_PRESENT       0U        // SAU regions present
#define __MPU_PRESENT             1U        // MPU present
#define __VTOR_PRESENT            1U        // VTOR present
#define __NVIC_PRIO_BITS          3U        // Number of Bits used for Priority Levels
#define __Vendor_SysTickConfig    0U        // Set to 1 if different SysTick Config is used
#define __FPU_PRESENT             0U        // FPU present
#define __DSP_PRESENT             0U        // DSP extension present
/*PRQA S 0602, 0603 ++*/

#include "core_cm33.h"                                      /* Processor and core peripherals */
#include "core_common.h"                                    /* Processor common */
#include "system_YTM32B1MC0.h"                              /* System Header */

/* IO definitions (access restrictions to peripheral registers) */
/**
*   IO Type Qualifiers are used
*   \li to specify the access to peripheral variables.
*   \li for automatic generation of peripheral register debug information.
*/
#ifndef __IO
#ifdef __cplusplus
  #define   __I     volatile             /*!< Defines 'read only' permissions                 */
#else
  #define   __I     volatile const       /*!< Defines 'read only' permissions                 */
#endif
#define     __O     volatile             /*!< Defines 'write only' permissions                */
#define     __IO    volatile             /*!< Defines 'read / write' permissions              */
#endif

/* --------  End of section using anonymous unions and disabling warnings  -------- */
#if   defined (__CC_ARM)
  #pragma pop
#elif defined (__ICCARM__)
  /* leave anonymous unions enabled */
#elif (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
  #pragma clang diagnostic pop
#elif defined (__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined (__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined (__TASKING__)
  #pragma warning restore
#elif defined (__CSMC__)
  /* anonymous unions are enabled by default */
#elif defined (__ghs__)
  /* anonymous unions are enabled by default */
#else
  #pragma warning not supported compiler type
#endif
/* ----------------------------------------------------------------------------
   -- Device Peripheral Access Layer for YTM32B1MC0
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup Peripheral_access_layer_YTM32B1MC0 Device Peripheral Access Layer for YTM32B1MC0
 * @{
 */

/* @brief This module covers memory mapped registers available on SoC */

/* -------------------------------------------------------------
   -- EFM Peripheral Access Layer
   ------------------------------------------------------------- */

/*!
 * @addtogroup EFM_Peripheral_Access_Layer EFM Peripheral Access Layer
 * @{
 */


/** EFM - Size of Registers Arrays */
#define EFM_ADDR_PROT_COUNT                                                   (2U)

/* EFM Register Layout Typedef */
typedef struct {
    __IO uint32_t CTRL;                                                       /**< Control Register, offset: 0x0000 */
    __IO uint32_t STS;                                                        /**< Status Register, offset: 0x0004 */
    __IO uint32_t CMD;                                                        /**< Command Register, offset: 0x0008 */
    __IO uint32_t CMD_PROT;                                                   /**< Command Protect Register, offset: 0x000C */
    __IO uint32_t TIMING1;                                                    /**< Timing 1 Control Register, offset: 0x0010 */
    __IO uint32_t TIMING2;                                                    /**< Timing 2 Control Register, offset: 0x0014 */
    __IO uint8_t  RESERVED6[ 8];                                              /**< RESERVED6, offset: 0x0018 */
    __IO uint32_t ERR_INJECT_CTRL;                                            /**< Error Inject Control, offset: 0x0020 */
    __IO uint32_t ADDR_PROT[ 2];                                              /**< Address Protect Register, offset: 0x0024 */
    __IO uint8_t  RESERVED9[ 4];                                              /**< RESERVED9, offset: 0x002C */
    __IO uint32_t ECCERR_ADDR;                                                /**< ECC Error Address, offset: 0x0030 */
    __IO uint32_t ECCERR_CNT;                                                 /**< ECC Error Count, offset: 0x0034 */
    __IO uint8_t  RESERVED12[456];                                            /**< RESERVED12, offset: 0x0038 */
    __O  uint32_t CMD_UNLOCK;                                                 /**< EFM Command Unlock, offset: 0x0200 */
    __IO uint32_t CUS_KEY;                                                    /**< EFM Customer Key, offset: 0x0204 */
    __IO uint8_t  RESERVED15[248];                                            /**< RESERVED15, offset: 0x0208 */
    __IO uint32_t CTRL_OVRD;                                                  /**< Override Control Register, offset: 0x0300 */
    __IO uint32_t RESERVED17;                                                 /**< RESERVED17, offset: 0x0304 */
    __IO uint32_t RESERVED18;                                                 /**< RESERVED18, offset: 0x0308 */

} EFM_Type, *EFM_MemMapPtr;

/** Number of instances of the EFM module. */
#define EFM_INSTANCE_COUNT                                                    (1U)

/* EFM base address */
#define EFM_BASE                                                              (0x40010000U)
#define EFM                                                                   ((EFM_Type *)(EFM_BASE))

/** Array initializer of EFM peripheral base addresses */
#define EFM_BASE_ADDRS                                                        { EFM_BASE }
/** Array initializer of EFM peripheral base pointers */
#define EFM_BASE_PTRS                                                         { EFM }
/** Number of interrupt channels for the EFM. */
#define EFM_IRQS_CH_COUNT                                                     (1U)
/** Interrupt vectors for the EFM peripheral type */
#define EFM_IRQS                                                              { EFM_IRQn }
/** Number of interrupt channels for the Read collision type of EFM. */
#define EFM_READ_COLLISION_IRQS_CH_COUNT                                      (1U)
#define EFM_READ_COLLISION_IRQS                                               { EFM_Error_IRQn }
/** Number of interrupt channels for the ECC error type of EFM. */
#define EFM_ECC_IRQS_CH_COUNT                                                 (1U)
#define EFM_ECC_IRQS                                                          { EFM_Ecc_IRQn }

/* EFM Register Mask */
/* EFM_CTRL Register */
#define EFM_CTRL_LPEN_MASK                                                    (0x80000000U)
#define EFM_CTRL_LPEN_SHIFT                                                   (31U)
#define EFM_CTRL_LPEN(x)                                                      (((uint32_t)(((uint32_t)(x)) << EFM_CTRL_LPEN_SHIFT)) & EFM_CTRL_LPEN_MASK)
#define EFM_CTRL_FWUP_EN_MASK                                                 (0x40000000U)
#define EFM_CTRL_FWUP_EN_SHIFT                                                (30U)
#define EFM_CTRL_FWUP_EN(x)                                                   (((uint32_t)(((uint32_t)(x)) << EFM_CTRL_FWUP_EN_SHIFT)) & EFM_CTRL_FWUP_EN_MASK)
#define EFM_CTRL_PRESCALER_MASK                                               (0xFF0000U)
#define EFM_CTRL_PRESCALER_SHIFT                                              (16U)
#define EFM_CTRL_PRESCALER(x)                                                 (((uint32_t)(((uint32_t)(x)) << EFM_CTRL_PRESCALER_SHIFT)) & EFM_CTRL_PRESCALER_MASK)
#define EFM_CTRL_PREFETCH_EN_MASK                                             (0x8000U)
#define EFM_CTRL_PREFETCH_EN_SHIFT                                            (15U)
#define EFM_CTRL_PREFETCH_EN(x)                                               (((uint32_t)(((uint32_t)(x)) << EFM_CTRL_PREFETCH_EN_SHIFT)) & EFM_CTRL_PREFETCH_EN_MASK)
#define EFM_CTRL_WE_MASK                                                      (0x1000U)
#define EFM_CTRL_WE_SHIFT                                                     (12U)
#define EFM_CTRL_WE(x)                                                        (((uint32_t)(((uint32_t)(x)) << EFM_CTRL_WE_SHIFT)) & EFM_CTRL_WE_MASK)
#define EFM_CTRL_RWS_MASK                                                     (0x300U)
#define EFM_CTRL_RWS_SHIFT                                                    (8U)
#define EFM_CTRL_RWS(x)                                                       (((uint32_t)(((uint32_t)(x)) << EFM_CTRL_RWS_SHIFT)) & EFM_CTRL_RWS_MASK)
#define EFM_CTRL_CMD_VERIFY_EN_MASK                                           (0x10U)
#define EFM_CTRL_CMD_VERIFY_EN_SHIFT                                          (4U)
#define EFM_CTRL_CMD_VERIFY_EN(x)                                             (((uint32_t)(((uint32_t)(x)) << EFM_CTRL_CMD_VERIFY_EN_SHIFT)) & EFM_CTRL_CMD_VERIFY_EN_MASK)
#define EFM_CTRL_ACCERRIE_MASK                                                (0x8U)
#define EFM_CTRL_ACCERRIE_SHIFT                                               (3U)
#define EFM_CTRL_ACCERRIE(x)                                                  (((uint32_t)(((uint32_t)(x)) << EFM_CTRL_ACCERRIE_SHIFT)) & EFM_CTRL_ACCERRIE_MASK)
#define EFM_CTRL_UNRECOVERRIE_MASK                                            (0x4U)
#define EFM_CTRL_UNRECOVERRIE_SHIFT                                           (2U)
#define EFM_CTRL_UNRECOVERRIE(x)                                              (((uint32_t)(((uint32_t)(x)) << EFM_CTRL_UNRECOVERRIE_SHIFT)) & EFM_CTRL_UNRECOVERRIE_MASK)
#define EFM_CTRL_RECOVERRIE_MASK                                              (0x2U)
#define EFM_CTRL_RECOVERRIE_SHIFT                                             (1U)
#define EFM_CTRL_RECOVERRIE(x)                                                (((uint32_t)(((uint32_t)(x)) << EFM_CTRL_RECOVERRIE_SHIFT)) & EFM_CTRL_RECOVERRIE_MASK)
#define EFM_CTRL_DONEIE_MASK                                                  (0x1U)
#define EFM_CTRL_DONEIE_SHIFT                                                 (0U)
#define EFM_CTRL_DONEIE(x)                                                    (((uint32_t)(((uint32_t)(x)) << EFM_CTRL_DONEIE_SHIFT)) & EFM_CTRL_DONEIE_MASK)
/* EFM_STS Register */
#define EFM_STS_CI_RDVIOLERR_MASK                                             (0x4000000U)
#define EFM_STS_CI_RDVIOLERR_SHIFT                                            (26U)
#define EFM_STS_CI_RDVIOLERR(x)                                               (((uint32_t)(((uint32_t)(x)) << EFM_STS_CI_RDVIOLERR_SHIFT)) & EFM_STS_CI_RDVIOLERR_MASK)
#define EFM_STS_CI_WEERR_MASK                                                 (0x2000000U)
#define EFM_STS_CI_WEERR_SHIFT                                                (25U)
#define EFM_STS_CI_WEERR(x)                                                   (((uint32_t)(((uint32_t)(x)) << EFM_STS_CI_WEERR_SHIFT)) & EFM_STS_CI_WEERR_MASK)
#define EFM_STS_CI_BUSYERR_MASK                                               (0x1000000U)
#define EFM_STS_CI_BUSYERR_SHIFT                                              (24U)
#define EFM_STS_CI_BUSYERR(x)                                                 (((uint32_t)(((uint32_t)(x)) << EFM_STS_CI_BUSYERR_SHIFT)) & EFM_STS_CI_BUSYERR_MASK)
#define EFM_STS_BOOT_INFO_MASK                                                (0x8000U)
#define EFM_STS_BOOT_INFO_SHIFT                                               (15U)
#define EFM_STS_BOOT_INFO(x)                                                  (((uint32_t)(((uint32_t)(x)) << EFM_STS_BOOT_INFO_SHIFT)) & EFM_STS_BOOT_INFO_MASK)
#define EFM_STS_IDLE_MASK                                                     (0x80U)
#define EFM_STS_IDLE_SHIFT                                                    (7U)
#define EFM_STS_IDLE(x)                                                       (((uint32_t)(((uint32_t)(x)) << EFM_STS_IDLE_SHIFT)) & EFM_STS_IDLE_MASK)
#define EFM_STS_ARRAY_SELECTED_MASK                                           (0x40U)
#define EFM_STS_ARRAY_SELECTED_SHIFT                                          (6U)
#define EFM_STS_ARRAY_SELECTED(x)                                             (((uint32_t)(((uint32_t)(x)) << EFM_STS_ARRAY_SELECTED_SHIFT)) & EFM_STS_ARRAY_SELECTED_MASK)
#define EFM_STS_CMD_FAIL_MASK                                                 (0x10U)
#define EFM_STS_CMD_FAIL_SHIFT                                                (4U)
#define EFM_STS_CMD_FAIL(x)                                                   (((uint32_t)(((uint32_t)(x)) << EFM_STS_CMD_FAIL_SHIFT)) & EFM_STS_CMD_FAIL_MASK)
#define EFM_STS_ACCERR_MASK                                                   (0x8U)
#define EFM_STS_ACCERR_SHIFT                                                  (3U)
#define EFM_STS_ACCERR(x)                                                     (((uint32_t)(((uint32_t)(x)) << EFM_STS_ACCERR_SHIFT)) & EFM_STS_ACCERR_MASK)
#define EFM_STS_UNRECOVERR_MASK                                               (0x4U)
#define EFM_STS_UNRECOVERR_SHIFT                                              (2U)
#define EFM_STS_UNRECOVERR(x)                                                 (((uint32_t)(((uint32_t)(x)) << EFM_STS_UNRECOVERR_SHIFT)) & EFM_STS_UNRECOVERR_MASK)
#define EFM_STS_RECOVERR_MASK                                                 (0x2U)
#define EFM_STS_RECOVERR_SHIFT                                                (1U)
#define EFM_STS_RECOVERR(x)                                                   (((uint32_t)(((uint32_t)(x)) << EFM_STS_RECOVERR_SHIFT)) & EFM_STS_RECOVERR_MASK)
#define EFM_STS_DONE_MASK                                                     (0x1U)
#define EFM_STS_DONE_SHIFT                                                    (0U)
#define EFM_STS_DONE(x)                                                       (((uint32_t)(((uint32_t)(x)) << EFM_STS_DONE_SHIFT)) & EFM_STS_DONE_MASK)
/* EFM_CMD Register */
#define EFM_CMD_CMD_MASK                                                      (0xFFU)
#define EFM_CMD_CMD_SHIFT                                                     (0U)
#define EFM_CMD_CMD(x)                                                        (((uint32_t)(((uint32_t)(x)) << EFM_CMD_CMD_SHIFT)) & EFM_CMD_CMD_MASK)
/* EFM_CMD_PROT Register */
#define EFM_CMD_PROT_CMD_PROT_MASK                                            (0xFFFFFFFFU)
#define EFM_CMD_PROT_CMD_PROT_SHIFT                                           (0U)
#define EFM_CMD_PROT_CMD_PROT(x)                                              (((uint32_t)(((uint32_t)(x)) << EFM_CMD_PROT_CMD_PROT_SHIFT)) & EFM_CMD_PROT_CMD_PROT_MASK)
/* EFM_TIMING1 Register */
#define EFM_TIMING1_TPROG_MASK                                                (0xFFU)
#define EFM_TIMING1_TPROG_SHIFT                                               (0U)
#define EFM_TIMING1_TPROG(x)                                                  (((uint32_t)(((uint32_t)(x)) << EFM_TIMING1_TPROG_SHIFT)) & EFM_TIMING1_TPROG_MASK)
/* EFM_TIMING2 Register */
#define EFM_TIMING2_TERASE_MASK                                               (0xFFFFU)
#define EFM_TIMING2_TERASE_SHIFT                                              (0U)
#define EFM_TIMING2_TERASE(x)                                                 (((uint32_t)(((uint32_t)(x)) << EFM_TIMING2_TERASE_SHIFT)) & EFM_TIMING2_TERASE_MASK)
/* EFM_ERR_INJECT_CTRL Register */
#define EFM_ERR_INJECT_CTRL_ECCERR_INJ_EN_MASK                                (0xC0000000U)
#define EFM_ERR_INJECT_CTRL_ECCERR_INJ_EN_SHIFT                               (30U)
#define EFM_ERR_INJECT_CTRL_ECCERR_INJ_EN(x)                                  (((uint32_t)(((uint32_t)(x)) << EFM_ERR_INJECT_CTRL_ECCERR_INJ_EN_SHIFT)) & EFM_ERR_INJECT_CTRL_ECCERR_INJ_EN_MASK)
/* EFM_ADDR_PROT Register */
#define EFM_ADDR_PROT_ADDR_PROT_MASK                                          (0xFFFFFFFFU)
#define EFM_ADDR_PROT_ADDR_PROT_SHIFT                                         (0U)
#define EFM_ADDR_PROT_ADDR_PROT(x)                                            (((uint32_t)(((uint32_t)(x)) << EFM_ADDR_PROT_ADDR_PROT_SHIFT)) & EFM_ADDR_PROT_ADDR_PROT_MASK)
/* EFM_ECCERR_ADDR Register */
#define EFM_ECCERR_ADDR_ERR_FLASH_ADDR_MASK                                   (0xFFFFFFFFU)
#define EFM_ECCERR_ADDR_ERR_FLASH_ADDR_SHIFT                                  (0U)
#define EFM_ECCERR_ADDR_ERR_FLASH_ADDR(x)                                     (((uint32_t)(((uint32_t)(x)) << EFM_ECCERR_ADDR_ERR_FLASH_ADDR_SHIFT)) & EFM_ECCERR_ADDR_ERR_FLASH_ADDR_MASK)
/* EFM_ECCERR_CNT Register */
#define EFM_ECCERR_CNT_ECCERR_CNT_MASK                                        (0x3FFFU)
#define EFM_ECCERR_CNT_ECCERR_CNT_SHIFT                                       (0U)
#define EFM_ECCERR_CNT_ECCERR_CNT(x)                                          (((uint32_t)(((uint32_t)(x)) << EFM_ECCERR_CNT_ECCERR_CNT_SHIFT)) & EFM_ECCERR_CNT_ECCERR_CNT_MASK)
/* EFM_CMD_UNLOCK Register */
#define EFM_CMD_UNLOCK_CMD_UNLOCK_KEY_MASK                                    (0xFFFFFFFFU)
#define EFM_CMD_UNLOCK_CMD_UNLOCK_KEY_SHIFT                                   (0U)
#define EFM_CMD_UNLOCK_CMD_UNLOCK_KEY(x)                                      (((uint32_t)(((uint32_t)(x)) << EFM_CMD_UNLOCK_CMD_UNLOCK_KEY_SHIFT)) & EFM_CMD_UNLOCK_CMD_UNLOCK_KEY_MASK)
/* EFM_CUS_KEY Register */
#define EFM_CUS_KEY_CUS_KEY_MATCH_MASK                                        (0x80000000U)
#define EFM_CUS_KEY_CUS_KEY_MATCH_SHIFT                                       (31U)
#define EFM_CUS_KEY_CUS_KEY_MATCH(x)                                          (((uint32_t)(((uint32_t)(x)) << EFM_CUS_KEY_CUS_KEY_MATCH_SHIFT)) & EFM_CUS_KEY_CUS_KEY_MATCH_MASK)
#define EFM_CUS_KEY_CUS_KEY_MISMATCH_MASK                                     (0x40000000U)
#define EFM_CUS_KEY_CUS_KEY_MISMATCH_SHIFT                                    (30U)
#define EFM_CUS_KEY_CUS_KEY_MISMATCH(x)                                       (((uint32_t)(((uint32_t)(x)) << EFM_CUS_KEY_CUS_KEY_MISMATCH_SHIFT)) & EFM_CUS_KEY_CUS_KEY_MISMATCH_MASK)
#define EFM_CUS_KEY_CUS_KEY_MASK                                              (0xFFFFFFU)
#define EFM_CUS_KEY_CUS_KEY_SHIFT                                             (0U)
#define EFM_CUS_KEY_CUS_KEY(x)                                                (((uint32_t)(((uint32_t)(x)) << EFM_CUS_KEY_CUS_KEY_SHIFT)) & EFM_CUS_KEY_CUS_KEY_MASK)
/* EFM_CTRL_OVRD Register */
#define EFM_CTRL_OVRD_DBG_DISABLE_MASK                                        (0x8U)
#define EFM_CTRL_OVRD_DBG_DISABLE_SHIFT                                       (3U)
#define EFM_CTRL_OVRD_DBG_DISABLE(x)                                          (((uint32_t)(((uint32_t)(x)) << EFM_CTRL_OVRD_DBG_DISABLE_SHIFT)) & EFM_CTRL_OVRD_DBG_DISABLE_MASK)


/*!
 * @}
 */ /* end of group EFM_Register_Masks */


/*!
 * @}
 */ /* end of group EFM_Peripheral_Access_Layer */


/* -------------------------------------------------------------
   -- GPIO Peripheral Access Layer
   ------------------------------------------------------------- */

/*!
 * @addtogroup GPIO_Peripheral_Access_Layer GPIO Peripheral Access Layer
 * @{
 */


/** GPIO - Size of Registers Arrays */
#define GPIO_PCR_COUNT                                                        (32U)

/* GPIO Register Layout Typedef */
typedef struct {
    __IO uint32_t PDOR;                                                       /**< Port Data Output Register, offset: 0x0000 */
    __O  uint32_t PSOR;                                                       /**< Port Set Output Register, offset: 0x0004 */
    __O  uint32_t PCOR;                                                       /**< Port Clear Output Register, offset: 0x0008 */
    __O  uint32_t PTOR;                                                       /**< Port Toggle Output Register, offset: 0x000C */
    __I  uint32_t PDIR;                                                       /**< Port Data Input Register, offset: 0x0010 */
    __IO uint32_t POER;                                                       /**< Port Output Enable Register, offset: 0x0014 */
    __IO uint32_t PIER;                                                       /**< Port Input Enable Register, offset: 0x0018 */
    __IO uint32_t PIFR;                                                       /**< Port Interrupt Status Flag Register, offset: 0x001C */
    __IO uint8_t  RESERVED8[96];                                              /**< RESERVED8, offset: 0x0020 */
    __IO uint32_t PCR[32];                                                    /**< Port Control Register, offset: 0x0080 */

} GPIO_Type, *GPIO_MemMapPtr;

/** Number of instances of the GPIO module. */
#define GPIO_INSTANCE_COUNT                                                   (5U)

/* GPIOA  base address */
#define GPIOA_BASE                                                            (0x40011000U)
#define GPIOA                                                                 ((GPIO_Type *)(GPIOA_BASE))
/* GPIOB  base address */
#define GPIOB_BASE                                                            (0x40011100U)
#define GPIOB                                                                 ((GPIO_Type *)(GPIOB_BASE))
/* GPIOC  base address */
#define GPIOC_BASE                                                            (0x40011200U)
#define GPIOC                                                                 ((GPIO_Type *)(GPIOC_BASE))
/* GPIOD  base address */
#define GPIOD_BASE                                                            (0x40011300U)
#define GPIOD                                                                 ((GPIO_Type *)(GPIOD_BASE))
/* GPIOE  base address */
#define GPIOE_BASE                                                            (0x40011400U)
#define GPIOE                                                                 ((GPIO_Type *)(GPIOE_BASE))

/** Array initializer of GPIO peripheral base addresses */
#define GPIO_BASE_ADDRS                                                       { GPIOA_BASE,  GPIOB_BASE,  GPIOC_BASE,  GPIOD_BASE,  GPIOE_BASE }
/** Array initializer of GPIO peripheral base pointers */
#define GPIO_BASE_PTRS                                                        { GPIOA,  GPIOB,  GPIOC,  GPIOD,  GPIOE }
/** Number of interrupt channels for the GPIO. */
#define GPIO_IRQS_CH_COUNT                                                    (1U)
/** Interrupt vectors for the GPIO peripheral type */
#define GPIO_IRQS                                                             { GPIOA_IRQn, GPIOB_IRQn, GPIOC_IRQn, GPIOD_IRQn, GPIOE_IRQn }

/* GPIO Register Mask */
/* GPIO_PDOR Register */
#define GPIO_PDOR_PDO_MASK                                                    (0xFFFFFFFFU)
#define GPIO_PDOR_PDO_SHIFT                                                   (0U)
#define GPIO_PDOR_PDO(x)                                                      (((uint32_t)(((uint32_t)(x)) << GPIO_PDOR_PDO_SHIFT)) & GPIO_PDOR_PDO_MASK)
/* GPIO_PSOR Register */
#define GPIO_PSOR_PSO_MASK                                                    (0xFFFFFFFFU)
#define GPIO_PSOR_PSO_SHIFT                                                   (0U)
#define GPIO_PSOR_PSO(x)                                                      (((uint32_t)(((uint32_t)(x)) << GPIO_PSOR_PSO_SHIFT)) & GPIO_PSOR_PSO_MASK)
/* GPIO_PCOR Register */
#define GPIO_PCOR_PCO_MASK                                                    (0xFFFFFFFFU)
#define GPIO_PCOR_PCO_SHIFT                                                   (0U)
#define GPIO_PCOR_PCO(x)                                                      (((uint32_t)(((uint32_t)(x)) << GPIO_PCOR_PCO_SHIFT)) & GPIO_PCOR_PCO_MASK)
/* GPIO_PTOR Register */
#define GPIO_PTOR_PTO_MASK                                                    (0xFFFFFFFFU)
#define GPIO_PTOR_PTO_SHIFT                                                   (0U)
#define GPIO_PTOR_PTO(x)                                                      (((uint32_t)(((uint32_t)(x)) << GPIO_PTOR_PTO_SHIFT)) & GPIO_PTOR_PTO_MASK)
/* GPIO_PDIR Register */
#define GPIO_PDIR_PDI_MASK                                                    (0xFFFFFFFFU)
#define GPIO_PDIR_PDI_SHIFT                                                   (0U)
#define GPIO_PDIR_PDI(x)                                                      (((uint32_t)(((uint32_t)(x)) << GPIO_PDIR_PDI_SHIFT)) & GPIO_PDIR_PDI_MASK)
/* GPIO_POER Register */
#define GPIO_POER_POE_MASK                                                    (0xFFFFFFFFU)
#define GPIO_POER_POE_SHIFT                                                   (0U)
#define GPIO_POER_POE(x)                                                      (((uint32_t)(((uint32_t)(x)) << GPIO_POER_POE_SHIFT)) & GPIO_POER_POE_MASK)
/* GPIO_PIER Register */
#define GPIO_PIER_PIE_MASK                                                    (0xFFFFFFFFU)
#define GPIO_PIER_PIE_SHIFT                                                   (0U)
#define GPIO_PIER_PIE(x)                                                      (((uint32_t)(((uint32_t)(x)) << GPIO_PIER_PIE_SHIFT)) & GPIO_PIER_PIE_MASK)
/* GPIO_PIFR Register */
#define GPIO_PIFR_PIF_MASK                                                    (0xFFFFFFFFU)
#define GPIO_PIFR_PIF_SHIFT                                                   (0U)
#define GPIO_PIFR_PIF(x)                                                      (((uint32_t)(((uint32_t)(x)) << GPIO_PIFR_PIF_SHIFT)) & GPIO_PIFR_PIF_MASK)
/* GPIO_PCR Register */
#define GPIO_PCR_INVE_MASK                                                    (0x10U)
#define GPIO_PCR_INVE_SHIFT                                                   (4U)
#define GPIO_PCR_INVE(x)                                                      (((uint32_t)(((uint32_t)(x)) << GPIO_PCR_INVE_SHIFT)) & GPIO_PCR_INVE_MASK)
#define GPIO_PCR_IRQC_MASK                                                    (0xFU)
#define GPIO_PCR_IRQC_SHIFT                                                   (0U)
#define GPIO_PCR_IRQC(x)                                                      (((uint32_t)(((uint32_t)(x)) << GPIO_PCR_IRQC_SHIFT)) & GPIO_PCR_IRQC_MASK)


/*!
 * @}
 */ /* end of group GPIO_Register_Masks */


/*!
 * @}
 */ /* end of group GPIO_Peripheral_Access_Layer */


/* -------------------------------------------------------------
   -- PCTRL Peripheral Access Layer
   ------------------------------------------------------------- */

/*!
 * @addtogroup PCTRL_Peripheral_Access_Layer PCTRL Peripheral Access Layer
 * @{
 */


/** PCTRL - Size of Registers Arrays */
#define PCTRL_PCR_COUNT                                                       (32U)

/* PCTRL Register Layout Typedef */
typedef struct {
    __IO uint32_t PCR[32];                                                    /**< Port Control Register, offset: 0x0000 */

} PCTRL_Type, *PCTRL_MemMapPtr;

/** Number of instances of the PCTRL module. */
#define PCTRL_INSTANCE_COUNT                                                  (5U)

/* PCTRLA  base address */
#define PCTRLA_BASE                                                           (0x40012000U)
#define PCTRLA                                                                ((PCTRL_Type *)(PCTRLA_BASE))
/* PCTRLB  base address */
#define PCTRLB_BASE                                                           (0x40013000U)
#define PCTRLB                                                                ((PCTRL_Type *)(PCTRLB_BASE))
/* PCTRLC  base address */
#define PCTRLC_BASE                                                           (0x40014000U)
#define PCTRLC                                                                ((PCTRL_Type *)(PCTRLC_BASE))
/* PCTRLD  base address */
#define PCTRLD_BASE                                                           (0x40015000U)
#define PCTRLD                                                                ((PCTRL_Type *)(PCTRLD_BASE))
/* PCTRLE  base address */
#define PCTRLE_BASE                                                           (0x40016000U)
#define PCTRLE                                                                ((PCTRL_Type *)(PCTRLE_BASE))

/** Array initializer of PCTRL peripheral base addresses */
#define PCTRL_BASE_ADDRS                                                      { PCTRLA_BASE,  PCTRLB_BASE,  PCTRLC_BASE,  PCTRLD_BASE,  PCTRLE_BASE }
/** Array initializer of PCTRL peripheral base pointers */
#define PCTRL_BASE_PTRS                                                       { PCTRLA,  PCTRLB,  PCTRLC,  PCTRLD,  PCTRLE }

/* PCTRL Register Mask */
/* PCTRL_PCR Register */
#define PCTRL_PCR_LOCK_MASK                                                   (0x80000000U)
#define PCTRL_PCR_LOCK_SHIFT                                                  (31U)
#define PCTRL_PCR_LOCK(x)                                                     (((uint32_t)(((uint32_t)(x)) << PCTRL_PCR_LOCK_SHIFT)) & PCTRL_PCR_LOCK_MASK)
#define PCTRL_PCR_DFW_WIDTH                                                   (3)
#define PCTRL_PCR_DFW_MASK                                                    (0xE0000U)
#define PCTRL_PCR_DFW_SHIFT                                                   (17U)
#define PCTRL_PCR_DFW(x)                                                      (((uint32_t)(((uint32_t)(x)) << PCTRL_PCR_DFW_SHIFT)) & PCTRL_PCR_DFW_MASK)
#define PCTRL_PCR_DFE_MASK                                                    (0x10000U)
#define PCTRL_PCR_DFE_SHIFT                                                   (16U)
#define PCTRL_PCR_DFE(x)                                                      (((uint32_t)(((uint32_t)(x)) << PCTRL_PCR_DFE_SHIFT)) & PCTRL_PCR_DFE_MASK)
#define PCTRL_PCR_MUX_WIDTH                                                   (3)
#define PCTRL_PCR_MUX_MASK                                                    (0x700U)
#define PCTRL_PCR_MUX_SHIFT                                                   (8U)
#define PCTRL_PCR_MUX(x)                                                      (((uint32_t)(((uint32_t)(x)) << PCTRL_PCR_MUX_SHIFT)) & PCTRL_PCR_MUX_MASK)
#define PCTRL_PCR_DSE_MASK                                                    (0x40U)
#define PCTRL_PCR_DSE_SHIFT                                                   (6U)
#define PCTRL_PCR_DSE(x)                                                      (((uint32_t)(((uint32_t)(x)) << PCTRL_PCR_DSE_SHIFT)) & PCTRL_PCR_DSE_MASK)
#define PCTRL_PCR_PFE_MASK                                                    (0x10U)
#define PCTRL_PCR_PFE_SHIFT                                                   (4U)
#define PCTRL_PCR_PFE(x)                                                      (((uint32_t)(((uint32_t)(x)) << PCTRL_PCR_PFE_SHIFT)) & PCTRL_PCR_PFE_MASK)
#define PCTRL_PCR_PE_MASK                                                     (0x2U)
#define PCTRL_PCR_PE_SHIFT                                                    (1U)
#define PCTRL_PCR_PE(x)                                                       (((uint32_t)(((uint32_t)(x)) << PCTRL_PCR_PE_SHIFT)) & PCTRL_PCR_PE_MASK)
#define PCTRL_PCR_PS_MASK                                                     (0x1U)
#define PCTRL_PCR_PS_SHIFT                                                    (0U)
#define PCTRL_PCR_PS(x)                                                       (((uint32_t)(((uint32_t)(x)) << PCTRL_PCR_PS_SHIFT)) & PCTRL_PCR_PS_MASK)


/*!
 * @}
 */ /* end of group PCTRL_Register_Masks */


/*!
 * @}
 */ /* end of group PCTRL_Peripheral_Access_Layer */


/* -------------------------------------------------------------
   -- DMA Peripheral Access Layer
   ------------------------------------------------------------- */

/*!
 * @addtogroup DMA_Peripheral_Access_Layer DMA Peripheral Access Layer
 * @{
 */


/** DMA - Size of Registers Arrays */
#define DMA_CHMUX_COUNT                                                       (4U)
#define DMA_CTS_COUNT                                                         (4U)

/* DMA Register Layout Typedef */
typedef struct {
    __IO uint32_t CTRL;                                                       /**< Control Register, offset: 0x0000 */
    __IO uint32_t CHTLDIF;                                                    /**< Channel Trigger Loop Done Interrupt Flag Register, offset: 0x0004 */
    __IO uint32_t CHTLHDIF;                                                   /**< Channel Trigger Loop Half Done Interrupt Flag Register, offset: 0x0008 */
    __IO uint32_t CHEIF;                                                      /**< DMA Channel Error Interrupt Flag Register, offset: 0x000C */
    __IO uint32_t CHEIE;                                                      /**< DMA Channel Error Interrupt Enable Register, offset: 0x0010 */
    __IO uint32_t ERS;                                                        /**< Error Status Register, offset: 0x0014 */
    __IO uint32_t REQEN;                                                      /**< Hardware Request Enable Register, offset: 0x0018 */
    __IO uint32_t REQS;                                                       /**< Request Status Register, offset: 0x001C */
    __IO uint32_t START;                                                      /**< Channel Start Register, offset: 0x0020 */
    __IO uint32_t DONE;                                                       /**< Channel Done Register, offset: 0x0024 */
    __IO uint8_t  RESERVED10[24];                                             /**< RESERVED10, offset: 0x0028 */
    __IO uint8_t  CHMUX[ 4];                                                  /**< Channel Mux Register, offset: 0x0040 */
    __IO uint8_t  RESERVED12[956];                                            /**< RESERVED12, offset: 0x0044 */
    struct {
        __IO uint32_t SADDR;                                                  /**< Source Address Register, offset: 0x0400 */
        __IO uint16_t SOFF;                                                   /**< Source Address Offset Register, offset: 0x0404 */
        __IO uint16_t TCR;                                                    /**< Transfer Control Register, offset: 0x0406 */
        union {                                                               /*PRQA S 0750*/
            __IO uint32_t BCNT;                                               /**< Transfer Loop Byte Count, offset: 0x0408 */
            __IO uint32_t LODIS;                                              /**< Transfer Loop Offset Disable Mapping, offset: 0x0408 */
            __IO uint32_t LOEN;                                               /**< Transfer Loop Offset Enable Mapping, offset: 0x0408 */

        } BCNT;
        __IO uint32_t STO;                                                    /**< Source Trigger Loop Offset Register, offset: 0x040C */
        __IO uint32_t DADDR;                                                  /**< Destination Address Register, offset: 0x0410 */
        __IO uint16_t DOFF;                                                   /**< Destination Address Offset Register, offset: 0x0414 */
        union {                                                               /*PRQA S 0750*/
            __IO uint16_t LKDIS;                                              /**< Trigger Count with Link Disable, offset: 0x0416 */
            __IO uint16_t LKEN;                                               /**< Trigger Count with Link Enable, offset: 0x0416 */

        } TCNT;
        union {                                                               /*PRQA S 0750*/
            __IO uint32_t DTO;                                                /**< Destination Trigger Loop Offset Register, offset: 0x0418 */
            __IO uint32_t RLD;                                                /**< RAM Reload Address Register, offset: 0x0418 */

        } DTO_RLD;
        __IO uint16_t CSR;                                                    /**< Control and Status Register, offset: 0x041C */
        __IO uint16_t TCNTRV;                                                 /**< TCNT Reload Value Register, offset: 0x041E */

    } CTS[4];

} DMA_Type, *DMA_MemMapPtr;

/** Number of instances of the DMA module. */
#define DMA_INSTANCE_COUNT                                                    (1U)

/* DMA0  base address */
#define DMA0_BASE                                                             (0x40008000U)
#define DMA0                                                                  ((DMA_Type *)(DMA0_BASE))

/** Array initializer of DMA peripheral base addresses */
#define DMA_BASE_ADDRS                                                        { DMA0_BASE }
/** Array initializer of DMA peripheral base pointers */
#define DMA_BASE_PTRS                                                         { DMA0 }
/** Number of interrupt channels for the channel done interrupt type of DMA. */
#define DMA_CHN_IRQS_CH_COUNT                                                 (4U)
#define DMA_CHN_IRQS                                                          { DMA0_IRQn, DMA1_IRQn, DMA2_IRQn, DMA3_IRQn }
/** Number of interrupt channels for the channel error type of DMA. */
#define DMA_ERROR_IRQS_CH_COUNT                                               (1U)
#define DMA_ERROR_IRQS                                                        { DMA_Error_IRQn }

/* DMA Register Mask */
/* DMA_CTRL Register */
#define DMA_CTRL_ACTIVE_MASK                                                  (0x80000000U)
#define DMA_CTRL_ACTIVE_SHIFT                                                 (31U)
#define DMA_CTRL_ACTIVE(x)                                                    (((uint32_t)(((uint32_t)(x)) << DMA_CTRL_ACTIVE_SHIFT)) & DMA_CTRL_ACTIVE_MASK)
#define DMA_CTRL_ACTCH_MASK                                                   (0x1F000000U)
#define DMA_CTRL_ACTCH_SHIFT                                                  (24U)
#define DMA_CTRL_ACTCH(x)                                                     (((uint32_t)(((uint32_t)(x)) << DMA_CTRL_ACTCH_SHIFT)) & DMA_CTRL_ACTCH_MASK)
#define DMA_CTRL_CX_MASK                                                      (0x80000U)
#define DMA_CTRL_CX_SHIFT                                                     (19U)
#define DMA_CTRL_CX(x)                                                        (((uint32_t)(((uint32_t)(x)) << DMA_CTRL_CX_SHIFT)) & DMA_CTRL_CX_MASK)
#define DMA_CTRL_ECX_MASK                                                     (0x40000U)
#define DMA_CTRL_ECX_SHIFT                                                    (18U)
#define DMA_CTRL_ECX(x)                                                       (((uint32_t)(((uint32_t)(x)) << DMA_CTRL_ECX_SHIFT)) & DMA_CTRL_ECX_MASK)
#define DMA_CTRL_POE_MASK                                                     (0x10000U)
#define DMA_CTRL_POE_SHIFT                                                    (16U)
#define DMA_CTRL_POE(x)                                                       (((uint32_t)(((uint32_t)(x)) << DMA_CTRL_POE_SHIFT)) & DMA_CTRL_POE_MASK)
#define DMA_CTRL_LOEN_MASK                                                    (0x400U)
#define DMA_CTRL_LOEN_SHIFT                                                   (10U)
#define DMA_CTRL_LOEN(x)                                                      (((uint32_t)(((uint32_t)(x)) << DMA_CTRL_LOEN_SHIFT)) & DMA_CTRL_LOEN_MASK)
#define DMA_CTRL_CLM_MASK                                                     (0x200U)
#define DMA_CTRL_CLM_SHIFT                                                    (9U)
#define DMA_CTRL_CLM(x)                                                       (((uint32_t)(((uint32_t)(x)) << DMA_CTRL_CLM_SHIFT)) & DMA_CTRL_CLM_MASK)
#define DMA_CTRL_DBGDIS_MASK                                                  (0x4U)
#define DMA_CTRL_DBGDIS_SHIFT                                                 (2U)
#define DMA_CTRL_DBGDIS(x)                                                    (((uint32_t)(((uint32_t)(x)) << DMA_CTRL_DBGDIS_SHIFT)) & DMA_CTRL_DBGDIS_MASK)
#define DMA_CTRL_CLRAD_MASK                                                   (0x2U)
#define DMA_CTRL_CLRAD_SHIFT                                                  (1U)
#define DMA_CTRL_CLRAD(x)                                                     (((uint32_t)(((uint32_t)(x)) << DMA_CTRL_CLRAD_SHIFT)) & DMA_CTRL_CLRAD_MASK)
#define DMA_CTRL_STACH_MASK                                                   (0x1U)
#define DMA_CTRL_STACH_SHIFT                                                  (0U)
#define DMA_CTRL_STACH(x)                                                     (((uint32_t)(((uint32_t)(x)) << DMA_CTRL_STACH_SHIFT)) & DMA_CTRL_STACH_MASK)
/* DMA_CHTLDIF Register */
#define DMA_CHTLDIF_CH_MASK                                                   (0xFU)
#define DMA_CHTLDIF_CH_SHIFT                                                  (0U)
#define DMA_CHTLDIF_CH(x)                                                     (((uint32_t)(((uint32_t)(x)) << DMA_CHTLDIF_CH_SHIFT)) & DMA_CHTLDIF_CH_MASK)
/* DMA_CHTLHDIF Register */
#define DMA_CHTLHDIF_CH_MASK                                                  (0xFU)
#define DMA_CHTLHDIF_CH_SHIFT                                                 (0U)
#define DMA_CHTLHDIF_CH(x)                                                    (((uint32_t)(((uint32_t)(x)) << DMA_CHTLHDIF_CH_SHIFT)) & DMA_CHTLHDIF_CH_MASK)
/* DMA_CHEIF Register */
#define DMA_CHEIF_CH_MASK                                                     (0xFU)
#define DMA_CHEIF_CH_SHIFT                                                    (0U)
#define DMA_CHEIF_CH(x)                                                       (((uint32_t)(((uint32_t)(x)) << DMA_CHEIF_CH_SHIFT)) & DMA_CHEIF_CH_MASK)
/* DMA_CHEIE Register */
#define DMA_CHEIE_CH_MASK                                                     (0xFU)
#define DMA_CHEIE_CH_SHIFT                                                    (0U)
#define DMA_CHEIE_CH(x)                                                       (((uint32_t)(((uint32_t)(x)) << DMA_CHEIE_CH_SHIFT)) & DMA_CHEIE_CH_MASK)
/* DMA_ERS Register */
#define DMA_ERS_VALID_MASK                                                    (0x80000000U)
#define DMA_ERS_VALID_SHIFT                                                   (31U)
#define DMA_ERS_VALID(x)                                                      (((uint32_t)(((uint32_t)(x)) << DMA_ERS_VALID_SHIFT)) & DMA_ERS_VALID_MASK)
#define DMA_ERS_ECX_MASK                                                      (0x40000U)
#define DMA_ERS_ECX_SHIFT                                                     (18U)
#define DMA_ERS_ECX(x)                                                        (((uint32_t)(((uint32_t)(x)) << DMA_ERS_ECX_SHIFT)) & DMA_ERS_ECX_MASK)
#define DMA_ERS_SAE_MASK                                                      (0x8000U)
#define DMA_ERS_SAE_SHIFT                                                     (15U)
#define DMA_ERS_SAE(x)                                                        (((uint32_t)(((uint32_t)(x)) << DMA_ERS_SAE_SHIFT)) & DMA_ERS_SAE_MASK)
#define DMA_ERS_SOE_MASK                                                      (0x4000U)
#define DMA_ERS_SOE_SHIFT                                                     (14U)
#define DMA_ERS_SOE(x)                                                        (((uint32_t)(((uint32_t)(x)) << DMA_ERS_SOE_SHIFT)) & DMA_ERS_SOE_MASK)
#define DMA_ERS_DAE_MASK                                                      (0x2000U)
#define DMA_ERS_DAE_SHIFT                                                     (13U)
#define DMA_ERS_DAE(x)                                                        (((uint32_t)(((uint32_t)(x)) << DMA_ERS_DAE_SHIFT)) & DMA_ERS_DAE_MASK)
#define DMA_ERS_DOE_MASK                                                      (0x1000U)
#define DMA_ERS_DOE_SHIFT                                                     (12U)
#define DMA_ERS_DOE(x)                                                        (((uint32_t)(((uint32_t)(x)) << DMA_ERS_DOE_SHIFT)) & DMA_ERS_DOE_MASK)
#define DMA_ERS_BTCE_MASK                                                     (0x800U)
#define DMA_ERS_BTCE_SHIFT                                                    (11U)
#define DMA_ERS_BTCE(x)                                                       (((uint32_t)(((uint32_t)(x)) << DMA_ERS_BTCE_SHIFT)) & DMA_ERS_BTCE_MASK)
#define DMA_ERS_RLDE_MASK                                                     (0x400U)
#define DMA_ERS_RLDE_SHIFT                                                    (10U)
#define DMA_ERS_RLDE(x)                                                       (((uint32_t)(((uint32_t)(x)) << DMA_ERS_RLDE_SHIFT)) & DMA_ERS_RLDE_MASK)
#define DMA_ERS_SBE_MASK                                                      (0x200U)
#define DMA_ERS_SBE_SHIFT                                                     (9U)
#define DMA_ERS_SBE(x)                                                        (((uint32_t)(((uint32_t)(x)) << DMA_ERS_SBE_SHIFT)) & DMA_ERS_SBE_MASK)
#define DMA_ERS_DBE_MASK                                                      (0x100U)
#define DMA_ERS_DBE_SHIFT                                                     (8U)
#define DMA_ERS_DBE(x)                                                        (((uint32_t)(((uint32_t)(x)) << DMA_ERS_DBE_SHIFT)) & DMA_ERS_DBE_MASK)
#define DMA_ERS_ECH_WIDTH                                                     (4)
#define DMA_ERS_ECH_MASK                                                      (0x3U)
#define DMA_ERS_ECH_SHIFT                                                     (0U)
#define DMA_ERS_ECH(x)                                                        (((uint32_t)(((uint32_t)(x)) << DMA_ERS_ECH_SHIFT)) & DMA_ERS_ECH_MASK)
/* DMA_REQEN Register */
#define DMA_REQEN_CH_WIDTH                                                    (16)
#define DMA_REQEN_CH_MASK                                                     (0xFU)
#define DMA_REQEN_CH_SHIFT                                                    (0U)
#define DMA_REQEN_CH(x)                                                       (((uint32_t)(((uint32_t)(x)) << DMA_REQEN_CH_SHIFT)) & DMA_REQEN_CH_MASK)
/* DMA_REQS Register */
#define DMA_REQS_CH_WIDTH                                                     (16)
#define DMA_REQS_CH_MASK                                                      (0xFU)
#define DMA_REQS_CH_SHIFT                                                     (0U)
#define DMA_REQS_CH(x)                                                        (((uint32_t)(((uint32_t)(x)) << DMA_REQS_CH_SHIFT)) & DMA_REQS_CH_MASK)
/* DMA_START Register */
#define DMA_START_CH_WIDTH                                                    (16)
#define DMA_START_CH_MASK                                                     (0xFU)
#define DMA_START_CH_SHIFT                                                    (0U)
#define DMA_START_CH(x)                                                       (((uint32_t)(((uint32_t)(x)) << DMA_START_CH_SHIFT)) & DMA_START_CH_MASK)
/* DMA_DONE Register */
#define DMA_DONE_CH_WIDTH                                                     (16)
#define DMA_DONE_CH_MASK                                                      (0xFU)
#define DMA_DONE_CH_SHIFT                                                     (0U)
#define DMA_DONE_CH(x)                                                        (((uint32_t)(((uint32_t)(x)) << DMA_DONE_CH_SHIFT)) & DMA_DONE_CH_MASK)
/* DMA_CHMUX Register */
#define DMA_CHMUX_CH_WIDTH                                                    (6)
#define DMA_CHMUX_CH_MASK                                                     (0x3FU)
#define DMA_CHMUX_CH_SHIFT                                                    (0U)
#define DMA_CHMUX_CH(x)                                                       (((uint32_t)(((uint32_t)(x)) << DMA_CHMUX_CH_SHIFT)) & DMA_CHMUX_CH_MASK)
/* DMA_CTS_SADDR Register */
#define DMA_CTS_SADDR_SADDR_WIDTH                                             (32)
#define DMA_CTS_SADDR_SADDR_MASK                                              (0xFFFFFFFFU)
#define DMA_CTS_SADDR_SADDR_SHIFT                                             (0U)
#define DMA_CTS_SADDR_SADDR(x)                                                (((uint32_t)(((uint32_t)(x)) << DMA_CTS_SADDR_SADDR_SHIFT)) & DMA_CTS_SADDR_SADDR_MASK)
/* DMA_CTS_SOFF Register */
#define DMA_CTS_SOFF_SOFF_WIDTH                                               (32)
#define DMA_CTS_SOFF_SOFF_MASK                                                (0xFFFFU)
#define DMA_CTS_SOFF_SOFF_SHIFT                                               (0U)
#define DMA_CTS_SOFF_SOFF(x)                                                  (((uint32_t)(((uint32_t)(x)) << DMA_CTS_SOFF_SOFF_SHIFT)) & DMA_CTS_SOFF_SOFF_MASK)
/* DMA_CTS_TCR Register */
#define DMA_CTS_TCR_SMOD_WIDTH                                                (5)
#define DMA_CTS_TCR_SMOD_MASK                                                 (0xF800U)
#define DMA_CTS_TCR_SMOD_SHIFT                                                (11U)
#define DMA_CTS_TCR_SMOD(x)                                                   (((uint32_t)(((uint32_t)(x)) << DMA_CTS_TCR_SMOD_SHIFT)) & DMA_CTS_TCR_SMOD_MASK)
#define DMA_CTS_TCR_SSIZE_WIDTH                                               (3)
#define DMA_CTS_TCR_SSIZE_MASK                                                (0x700U)
#define DMA_CTS_TCR_SSIZE_SHIFT                                               (8U)
#define DMA_CTS_TCR_SSIZE(x)                                                  (((uint32_t)(((uint32_t)(x)) << DMA_CTS_TCR_SSIZE_SHIFT)) & DMA_CTS_TCR_SSIZE_MASK)
#define DMA_CTS_TCR_DMOD_WIDTH                                                (5)
#define DMA_CTS_TCR_DMOD_MASK                                                 (0xF8U)
#define DMA_CTS_TCR_DMOD_SHIFT                                                (3U)
#define DMA_CTS_TCR_DMOD(x)                                                   (((uint32_t)(((uint32_t)(x)) << DMA_CTS_TCR_DMOD_SHIFT)) & DMA_CTS_TCR_DMOD_MASK)
#define DMA_CTS_TCR_DSIZE_WIDTH                                               (3)
#define DMA_CTS_TCR_DSIZE_MASK                                                (0x7U)
#define DMA_CTS_TCR_DSIZE_SHIFT                                               (0U)
#define DMA_CTS_TCR_DSIZE(x)                                                  (((uint32_t)(((uint32_t)(x)) << DMA_CTS_TCR_DSIZE_SHIFT)) & DMA_CTS_TCR_DSIZE_MASK)
/* DMA_CTS_BCNT_BCNT Register */
#define DMA_CTS_BCNT_BCNT_BCNT_MASK                                           (0xFFFFFFFFU)
#define DMA_CTS_BCNT_BCNT_BCNT_SHIFT                                          (0U)
#define DMA_CTS_BCNT_BCNT_BCNT(x)                                             (((uint32_t)(((uint32_t)(x)) << DMA_CTS_BCNT_BCNT_BCNT_SHIFT)) & DMA_CTS_BCNT_BCNT_BCNT_MASK)
/* DMA_CTS_BCNT_LODIS Register */
#define DMA_CTS_BCNT_LODIS_SLOE_MASK                                          (0x80000000U)
#define DMA_CTS_BCNT_LODIS_SLOE_SHIFT                                         (31U)
#define DMA_CTS_BCNT_LODIS_SLOE(x)                                            (((uint32_t)(((uint32_t)(x)) << DMA_CTS_BCNT_LODIS_SLOE_SHIFT)) & DMA_CTS_BCNT_LODIS_SLOE_MASK)
#define DMA_CTS_BCNT_LODIS_DLOE_MASK                                          (0x40000000U)
#define DMA_CTS_BCNT_LODIS_DLOE_SHIFT                                         (30U)
#define DMA_CTS_BCNT_LODIS_DLOE(x)                                            (((uint32_t)(((uint32_t)(x)) << DMA_CTS_BCNT_LODIS_DLOE_SHIFT)) & DMA_CTS_BCNT_LODIS_DLOE_MASK)
#define DMA_CTS_BCNT_LODIS_BCNT_MASK                                          (0x3FFFFFFFU)
#define DMA_CTS_BCNT_LODIS_BCNT_SHIFT                                         (0U)
#define DMA_CTS_BCNT_LODIS_BCNT(x)                                            (((uint32_t)(((uint32_t)(x)) << DMA_CTS_BCNT_LODIS_BCNT_SHIFT)) & DMA_CTS_BCNT_LODIS_BCNT_MASK)
/* DMA_CTS_BCNT_LOEN Register */
#define DMA_CTS_BCNT_LOEN_SLOE_MASK                                           (0x80000000U)
#define DMA_CTS_BCNT_LOEN_SLOE_SHIFT                                          (31U)
#define DMA_CTS_BCNT_LOEN_SLOE(x)                                             (((uint32_t)(((uint32_t)(x)) << DMA_CTS_BCNT_LOEN_SLOE_SHIFT)) & DMA_CTS_BCNT_LOEN_SLOE_MASK)
#define DMA_CTS_BCNT_LOEN_DLOE_MASK                                           (0x40000000U)
#define DMA_CTS_BCNT_LOEN_DLOE_SHIFT                                          (30U)
#define DMA_CTS_BCNT_LOEN_DLOE(x)                                             (((uint32_t)(((uint32_t)(x)) << DMA_CTS_BCNT_LOEN_DLOE_SHIFT)) & DMA_CTS_BCNT_LOEN_DLOE_MASK)
#define DMA_CTS_BCNT_LOEN_OFFSET_MASK                                         (0x3FFFFC00U)
#define DMA_CTS_BCNT_LOEN_OFFSET_SHIFT                                        (10U)
#define DMA_CTS_BCNT_LOEN_OFFSET(x)                                           (((uint32_t)(((uint32_t)(x)) << DMA_CTS_BCNT_LOEN_OFFSET_SHIFT)) & DMA_CTS_BCNT_LOEN_OFFSET_MASK)
#define DMA_CTS_BCNT_LOEN_BCNT_MASK                                           (0x3FFU)
#define DMA_CTS_BCNT_LOEN_BCNT_SHIFT                                          (0U)
#define DMA_CTS_BCNT_LOEN_BCNT(x)                                             (((uint32_t)(((uint32_t)(x)) << DMA_CTS_BCNT_LOEN_BCNT_SHIFT)) & DMA_CTS_BCNT_LOEN_BCNT_MASK)

/* DMA_CTS_STO Register */
#define DMA_CTS_STO_STLOFF_WIDTH                                              (31)
#define DMA_CTS_STO_STLOFF_MASK                                               (0xFFFFFFFFU)
#define DMA_CTS_STO_STLOFF_SHIFT                                              (0U)
#define DMA_CTS_STO_STLOFF(x)                                                 (((uint32_t)(((uint32_t)(x)) << DMA_CTS_STO_STLOFF_SHIFT)) & DMA_CTS_STO_STLOFF_MASK)
/* DMA_CTS_DADDR Register */
#define DMA_CTS_DADDR_DADDR_WIDTH                                             (31)
#define DMA_CTS_DADDR_DADDR_MASK                                              (0xFFFFFFFFU)
#define DMA_CTS_DADDR_DADDR_SHIFT                                             (0U)
#define DMA_CTS_DADDR_DADDR(x)                                                (((uint32_t)(((uint32_t)(x)) << DMA_CTS_DADDR_DADDR_SHIFT)) & DMA_CTS_DADDR_DADDR_MASK)
/* DMA_CTS_DOFF Register */
#define DMA_CTS_DOFF_DOFF_WIDTH                                               (16)
#define DMA_CTS_DOFF_DOFF_MASK                                                (0xFFFFU)
#define DMA_CTS_DOFF_DOFF_SHIFT                                               (0U)
#define DMA_CTS_DOFF_DOFF(x)                                                  (((uint32_t)(((uint32_t)(x)) << DMA_CTS_DOFF_DOFF_SHIFT)) & DMA_CTS_DOFF_DOFF_MASK)
/* DMA_CTS_TCNT_LKDIS Register */
#define DMA_CTS_TCNT_LKDIS_LKEN_MASK                                          (0x8000U)
#define DMA_CTS_TCNT_LKDIS_LKEN_SHIFT                                         (15U)
#define DMA_CTS_TCNT_LKDIS_LKEN(x)                                            (((uint32_t)(((uint32_t)(x)) << DMA_CTS_TCNT_LKDIS_LKEN_SHIFT)) & DMA_CTS_TCNT_LKDIS_LKEN_MASK)
#define DMA_CTS_TCNT_LKDIS_TCNT_MASK                                          (0x7FFFU)
#define DMA_CTS_TCNT_LKDIS_TCNT_SHIFT                                         (0U)
#define DMA_CTS_TCNT_LKDIS_TCNT(x)                                            (((uint32_t)(((uint32_t)(x)) << DMA_CTS_TCNT_LKDIS_TCNT_SHIFT)) & DMA_CTS_TCNT_LKDIS_TCNT_MASK)
/* DMA_CTS_TCNT_LKEN Register */
#define DMA_CTS_TCNT_LKEN_LKEN_MASK                                           (0x8000U)
#define DMA_CTS_TCNT_LKEN_LKEN_SHIFT                                          (15U)
#define DMA_CTS_TCNT_LKEN_LKEN(x)                                             (((uint32_t)(((uint32_t)(x)) << DMA_CTS_TCNT_LKEN_LKEN_SHIFT)) & DMA_CTS_TCNT_LKEN_LKEN_MASK)
#define DMA_CTS_TCNT_LKEN_LKCH_MASK                                           (0x600U)
#define DMA_CTS_TCNT_LKEN_LKCH_SHIFT                                          (9U)
#define DMA_CTS_TCNT_LKEN_LKCH(x)                                             (((uint32_t)(((uint32_t)(x)) << DMA_CTS_TCNT_LKEN_LKCH_SHIFT)) & DMA_CTS_TCNT_LKEN_LKCH_MASK)
#define DMA_CTS_TCNT_LKEN_TCNT_MASK                                           (0x1FFU)
#define DMA_CTS_TCNT_LKEN_TCNT_SHIFT                                          (0U)
#define DMA_CTS_TCNT_LKEN_TCNT(x)                                             (((uint32_t)(((uint32_t)(x)) << DMA_CTS_TCNT_LKEN_TCNT_SHIFT)) & DMA_CTS_TCNT_LKEN_TCNT_MASK)

/* DMA_CTS_DTO_RLD_DTO Register */
#define DMA_CTS_DTO_RLD_DTO_OFFSET_MASK                                       (0xFFFFFFFFU)
#define DMA_CTS_DTO_RLD_DTO_OFFSET_SHIFT                                      (0U)
#define DMA_CTS_DTO_RLD_DTO_OFFSET(x)                                         (((uint32_t)(((uint32_t)(x)) << DMA_CTS_DTO_RLD_DTO_OFFSET_SHIFT)) & DMA_CTS_DTO_RLD_DTO_OFFSET_MASK)
/* DMA_CTS_DTO_RLD_RLD Register */
#define DMA_CTS_DTO_RLD_RLD_ADDR_MASK                                         (0xFFFFFFFFU)
#define DMA_CTS_DTO_RLD_RLD_ADDR_SHIFT                                        (0U)
#define DMA_CTS_DTO_RLD_RLD_ADDR(x)                                           (((uint32_t)(((uint32_t)(x)) << DMA_CTS_DTO_RLD_RLD_ADDR_SHIFT)) & DMA_CTS_DTO_RLD_RLD_ADDR_MASK)

/* DMA_CTS_CSR Register */
#define DMA_CTS_CSR_TDINT_MASK                                                (0x8000U)
#define DMA_CTS_CSR_TDINT_SHIFT                                               (15U)
#define DMA_CTS_CSR_TDINT(x)                                                  (((uint32_t)(((uint32_t)(x)) << DMA_CTS_CSR_TDINT_SHIFT)) & DMA_CTS_CSR_TDINT_MASK)
#define DMA_CTS_CSR_THDINT_MASK                                               (0x4000U)
#define DMA_CTS_CSR_THDINT_SHIFT                                              (14U)
#define DMA_CTS_CSR_THDINT(x)                                                 (((uint32_t)(((uint32_t)(x)) << DMA_CTS_CSR_THDINT_SHIFT)) & DMA_CTS_CSR_THDINT_MASK)
#define DMA_CTS_CSR_TLKCH_MASK                                                (0x300U)
#define DMA_CTS_CSR_TLKCH_SHIFT                                               (8U)
#define DMA_CTS_CSR_TLKCH(x)                                                  (((uint32_t)(((uint32_t)(x)) << DMA_CTS_CSR_TLKCH_SHIFT)) & DMA_CTS_CSR_TLKCH_MASK)
#define DMA_CTS_CSR_TLKEN_MASK                                                (0x80U)
#define DMA_CTS_CSR_TLKEN_SHIFT                                               (7U)
#define DMA_CTS_CSR_TLKEN(x)                                                  (((uint32_t)(((uint32_t)(x)) << DMA_CTS_CSR_TLKEN_SHIFT)) & DMA_CTS_CSR_TLKEN_MASK)
#define DMA_CTS_CSR_RLDEN_MASK                                                (0x40U)
#define DMA_CTS_CSR_RLDEN_SHIFT                                               (6U)
#define DMA_CTS_CSR_RLDEN(x)                                                  (((uint32_t)(((uint32_t)(x)) << DMA_CTS_CSR_RLDEN_SHIFT)) & DMA_CTS_CSR_RLDEN_MASK)
#define DMA_CTS_CSR_BWC_MASK                                                  (0x30U)
#define DMA_CTS_CSR_BWC_SHIFT                                                 (4U)
#define DMA_CTS_CSR_BWC(x)                                                    (((uint32_t)(((uint32_t)(x)) << DMA_CTS_CSR_BWC_SHIFT)) & DMA_CTS_CSR_BWC_MASK)
#define DMA_CTS_CSR_DREQ_MASK                                                 (0x8U)
#define DMA_CTS_CSR_DREQ_SHIFT                                                (3U)
#define DMA_CTS_CSR_DREQ(x)                                                   (((uint32_t)(((uint32_t)(x)) << DMA_CTS_CSR_DREQ_SHIFT)) & DMA_CTS_CSR_DREQ_MASK)
#define DMA_CTS_CSR_ACTIVE_MASK                                               (0x4U)
#define DMA_CTS_CSR_ACTIVE_SHIFT                                              (2U)
#define DMA_CTS_CSR_ACTIVE(x)                                                 (((uint32_t)(((uint32_t)(x)) << DMA_CTS_CSR_ACTIVE_SHIFT)) & DMA_CTS_CSR_ACTIVE_MASK)
#define DMA_CTS_CSR_DONE_MASK                                                 (0x2U)
#define DMA_CTS_CSR_DONE_SHIFT                                                (1U)
#define DMA_CTS_CSR_DONE(x)                                                   (((uint32_t)(((uint32_t)(x)) << DMA_CTS_CSR_DONE_SHIFT)) & DMA_CTS_CSR_DONE_MASK)
#define DMA_CTS_CSR_START_MASK                                                (0x1U)
#define DMA_CTS_CSR_START_SHIFT                                               (0U)
#define DMA_CTS_CSR_START(x)                                                  (((uint32_t)(((uint32_t)(x)) << DMA_CTS_CSR_START_SHIFT)) & DMA_CTS_CSR_START_MASK)
/* DMA_CTS_TCNTRV Register */
#define DMA_CTS_TCNTRV_RV_MASK                                                (0xFFFFU)
#define DMA_CTS_TCNTRV_RV_SHIFT                                               (0U)
#define DMA_CTS_TCNTRV_RV(x)                                                  (((uint32_t)(((uint32_t)(x)) << DMA_CTS_TCNTRV_RV_SHIFT)) & DMA_CTS_TCNTRV_RV_MASK)



/*!
 * @}
 */ /* end of group DMA_Register_Masks */


/*!
 * @}
 */ /* end of group DMA_Peripheral_Access_Layer */


/* -------------------------------------------------------------
   -- CIM Peripheral Access Layer
   ------------------------------------------------------------- */

/*!
 * @addtogroup CIM_Peripheral_Access_Layer CIM Peripheral Access Layer
 * @{
 */


/** CIM - Size of Registers Arrays */

/* CIM Register Layout Typedef */
typedef struct {
    __IO uint32_t CTRL;                                                       /**< CIM Control Register, offset: 0x0000 */
    __IO uint32_t RESERVED1[ 3];                                              /**< RESERVED1, offset: 0x0004 */
    __IO uint32_t RESERVED2;                                                  /**< RESERVED2, offset: 0x0010 */
    __IO uint32_t SWTRIG;                                                     /**< Software Trigger Register, offset: 0x0014 */
    __IO uint32_t ETMROPT0;                                                   /**< eTMR Option Register 0, offset: 0x0018 */
    __IO uint32_t ETMROPT1;                                                   /**< eTMR Option Register 1, offset: 0x001C */
    __IO uint32_t RESERVED6[ 8];                                              /**< RESERVED6, offset: 0x0020 */
    __I  uint32_t DIEINFO;                                                    /**< Die Information Register, offset: 0x0040 */
    __I  uint32_t CHIPINFO;                                                   /**< Chip Information Register, offset: 0x0044 */

} CIM_Type, *CIM_MemMapPtr;

/** Number of instances of the CIM module. */
#define CIM_INSTANCE_COUNT                                                    (1U)

/* CIM base address */
#define CIM_BASE                                                              (0x4007B000U)
#define CIM                                                                   ((CIM_Type *)(CIM_BASE))

/** Array initializer of CIM peripheral base addresses */
#define CIM_BASE_ADDRS                                                        { CIM_BASE }
/** Array initializer of CIM peripheral base pointers */
#define CIM_BASE_PTRS                                                         { CIM }

/* CIM Register Mask */
/* CIM_CTRL Register */
#define CIM_CTRL_ADC0_TRIG_SEL_MASK                                           (0x10000U)
#define CIM_CTRL_ADC0_TRIG_SEL_SHIFT                                          (16U)
#define CIM_CTRL_ADC0_TRIG_SEL(x)                                             (((uint32_t)(((uint32_t)(x)) << CIM_CTRL_ADC0_TRIG_SEL_SHIFT)) & CIM_CTRL_ADC0_TRIG_SEL_MASK)
#define CIM_CTRL_ACMP0_TRIG_SEL_MASK                                          (0x300U)
#define CIM_CTRL_ACMP0_TRIG_SEL_SHIFT                                         (8U)
#define CIM_CTRL_ACMP0_TRIG_SEL(x)                                            (((uint32_t)(((uint32_t)(x)) << CIM_CTRL_ACMP0_TRIG_SEL_SHIFT)) & CIM_CTRL_ACMP0_TRIG_SEL_MASK)
#define CIM_CTRL_LOCKUPEN_MASK                                                (0x4U)
#define CIM_CTRL_LOCKUPEN_SHIFT                                               (2U)
#define CIM_CTRL_LOCKUPEN(x)                                                  (((uint32_t)(((uint32_t)(x)) << CIM_CTRL_LOCKUPEN_SHIFT)) & CIM_CTRL_LOCKUPEN_MASK)
#define CIM_CTRL_NMIEN_MASK                                                   (0x2U)
#define CIM_CTRL_NMIEN_SHIFT                                                  (1U)
#define CIM_CTRL_NMIEN(x)                                                     (((uint32_t)(((uint32_t)(x)) << CIM_CTRL_NMIEN_SHIFT)) & CIM_CTRL_NMIEN_MASK)
/* CIM_SWTRIG Register */
#define CIM_SWTRIG_SWTCNT_MASK                                                (0xFEU)
#define CIM_SWTRIG_SWTCNT_SHIFT                                               (1U)
#define CIM_SWTRIG_SWTCNT(x)                                                  (((uint32_t)(((uint32_t)(x)) << CIM_SWTRIG_SWTCNT_SHIFT)) & CIM_SWTRIG_SWTCNT_MASK)
#define CIM_SWTRIG_SWT_MASK                                                   (0x1U)
#define CIM_SWTRIG_SWT_SHIFT                                                  (0U)
#define CIM_SWTRIG_SWT(x)                                                     (((uint32_t)(((uint32_t)(x)) << CIM_SWTRIG_SWT_SHIFT)) & CIM_SWTRIG_SWT_MASK)
/* CIM_ETMROPT0 Register */
#define CIM_ETMROPT0_ETMR1_FAULT_SEL_MASK                                     (0x700000U)
#define CIM_ETMROPT0_ETMR1_FAULT_SEL_SHIFT                                    (20U)
#define CIM_ETMROPT0_ETMR1_FAULT_SEL(x)                                       (((uint32_t)(((uint32_t)(x)) << CIM_ETMROPT0_ETMR1_FAULT_SEL_SHIFT)) & CIM_ETMROPT0_ETMR1_FAULT_SEL_MASK)
#define CIM_ETMROPT0_ETMR0_FAULT_SEL_MASK                                     (0x70000U)
#define CIM_ETMROPT0_ETMR0_FAULT_SEL_SHIFT                                    (16U)
#define CIM_ETMROPT0_ETMR0_FAULT_SEL(x)                                       (((uint32_t)(((uint32_t)(x)) << CIM_ETMROPT0_ETMR0_FAULT_SEL_SHIFT)) & CIM_ETMROPT0_ETMR0_FAULT_SEL_MASK)
#define CIM_ETMROPT0_ETMR1_EXTCLK_SEL_MASK                                    (0xCU)
#define CIM_ETMROPT0_ETMR1_EXTCLK_SEL_SHIFT                                   (2U)
#define CIM_ETMROPT0_ETMR1_EXTCLK_SEL(x)                                      (((uint32_t)(((uint32_t)(x)) << CIM_ETMROPT0_ETMR1_EXTCLK_SEL_SHIFT)) & CIM_ETMROPT0_ETMR1_EXTCLK_SEL_MASK)
#define CIM_ETMROPT0_ETMR0_EXTCLK_SEL_MASK                                    (0x3U)
#define CIM_ETMROPT0_ETMR0_EXTCLK_SEL_SHIFT                                   (0U)
#define CIM_ETMROPT0_ETMR0_EXTCLK_SEL(x)                                      (((uint32_t)(((uint32_t)(x)) << CIM_ETMROPT0_ETMR0_EXTCLK_SEL_SHIFT)) & CIM_ETMROPT0_ETMR0_EXTCLK_SEL_MASK)
/* CIM_ETMROPT1 Register */
#define CIM_ETMROPT1_ETMR0_CH7OUTSEL_MASK                                     (0x800000U)
#define CIM_ETMROPT1_ETMR0_CH7OUTSEL_SHIFT                                    (23U)
#define CIM_ETMROPT1_ETMR0_CH7OUTSEL(x)                                       (((uint32_t)(((uint32_t)(x)) << CIM_ETMROPT1_ETMR0_CH7OUTSEL_SHIFT)) & CIM_ETMROPT1_ETMR0_CH7OUTSEL_MASK)
#define CIM_ETMROPT1_ETMR0_CH6OUTSEL_MASK                                     (0x400000U)
#define CIM_ETMROPT1_ETMR0_CH6OUTSEL_SHIFT                                    (22U)
#define CIM_ETMROPT1_ETMR0_CH6OUTSEL(x)                                       (((uint32_t)(((uint32_t)(x)) << CIM_ETMROPT1_ETMR0_CH6OUTSEL_SHIFT)) & CIM_ETMROPT1_ETMR0_CH6OUTSEL_MASK)
#define CIM_ETMROPT1_ETMR0_CH5OUTSEL_MASK                                     (0x200000U)
#define CIM_ETMROPT1_ETMR0_CH5OUTSEL_SHIFT                                    (21U)
#define CIM_ETMROPT1_ETMR0_CH5OUTSEL(x)                                       (((uint32_t)(((uint32_t)(x)) << CIM_ETMROPT1_ETMR0_CH5OUTSEL_SHIFT)) & CIM_ETMROPT1_ETMR0_CH5OUTSEL_MASK)
#define CIM_ETMROPT1_ETMR0_CH4OUTSEL_MASK                                     (0x100000U)
#define CIM_ETMROPT1_ETMR0_CH4OUTSEL_SHIFT                                    (20U)
#define CIM_ETMROPT1_ETMR0_CH4OUTSEL(x)                                       (((uint32_t)(((uint32_t)(x)) << CIM_ETMROPT1_ETMR0_CH4OUTSEL_SHIFT)) & CIM_ETMROPT1_ETMR0_CH4OUTSEL_MASK)
#define CIM_ETMROPT1_ETMR0_CH3OUTSEL_MASK                                     (0x80000U)
#define CIM_ETMROPT1_ETMR0_CH3OUTSEL_SHIFT                                    (19U)
#define CIM_ETMROPT1_ETMR0_CH3OUTSEL(x)                                       (((uint32_t)(((uint32_t)(x)) << CIM_ETMROPT1_ETMR0_CH3OUTSEL_SHIFT)) & CIM_ETMROPT1_ETMR0_CH3OUTSEL_MASK)
#define CIM_ETMROPT1_ETMR0_CH2OUTSEL_MASK                                     (0x40000U)
#define CIM_ETMROPT1_ETMR0_CH2OUTSEL_SHIFT                                    (18U)
#define CIM_ETMROPT1_ETMR0_CH2OUTSEL(x)                                       (((uint32_t)(((uint32_t)(x)) << CIM_ETMROPT1_ETMR0_CH2OUTSEL_SHIFT)) & CIM_ETMROPT1_ETMR0_CH2OUTSEL_MASK)
#define CIM_ETMROPT1_ETMR0_CH1OUTSEL_MASK                                     (0x20000U)
#define CIM_ETMROPT1_ETMR0_CH1OUTSEL_SHIFT                                    (17U)
#define CIM_ETMROPT1_ETMR0_CH1OUTSEL(x)                                       (((uint32_t)(((uint32_t)(x)) << CIM_ETMROPT1_ETMR0_CH1OUTSEL_SHIFT)) & CIM_ETMROPT1_ETMR0_CH1OUTSEL_MASK)
#define CIM_ETMROPT1_ETMR0_CH0OUTSEL_MASK                                     (0x10000U)
#define CIM_ETMROPT1_ETMR0_CH0OUTSEL_SHIFT                                    (16U)
#define CIM_ETMROPT1_ETMR0_CH0OUTSEL(x)                                       (((uint32_t)(((uint32_t)(x)) << CIM_ETMROPT1_ETMR0_CH0OUTSEL_SHIFT)) & CIM_ETMROPT1_ETMR0_CH0OUTSEL_MASK)
#define CIM_ETMROPT1_MPWM0CH0SEL_MASK                                         (0x1000U)
#define CIM_ETMROPT1_MPWM0CH0SEL_SHIFT                                        (12U)
#define CIM_ETMROPT1_MPWM0CH0SEL(x)                                           (((uint32_t)(((uint32_t)(x)) << CIM_ETMROPT1_MPWM0CH0SEL_SHIFT)) & CIM_ETMROPT1_MPWM0CH0SEL_MASK)
#define CIM_ETMROPT1_MPWM0CH6SEL_MASK                                         (0x400U)
#define CIM_ETMROPT1_MPWM0CH6SEL_SHIFT                                        (10U)
#define CIM_ETMROPT1_MPWM0CH6SEL(x)                                           (((uint32_t)(((uint32_t)(x)) << CIM_ETMROPT1_MPWM0CH6SEL_SHIFT)) & CIM_ETMROPT1_MPWM0CH6SEL_MASK)
#define CIM_ETMROPT1_ETMR1CH0SEL_MASK                                         (0x100U)
#define CIM_ETMROPT1_ETMR1CH0SEL_SHIFT                                        (8U)
#define CIM_ETMROPT1_ETMR1CH0SEL(x)                                           (((uint32_t)(((uint32_t)(x)) << CIM_ETMROPT1_ETMR1CH0SEL_SHIFT)) & CIM_ETMROPT1_ETMR1CH0SEL_MASK)
#define CIM_ETMROPT1_ETMR1SS1C_MASK                                           (0x2U)
#define CIM_ETMROPT1_ETMR1SS1C_SHIFT                                          (1U)
#define CIM_ETMROPT1_ETMR1SS1C(x)                                             (((uint32_t)(((uint32_t)(x)) << CIM_ETMROPT1_ETMR1SS1C_SHIFT)) & CIM_ETMROPT1_ETMR1SS1C_MASK)
#define CIM_ETMROPT1_ETMR0SS1C_MASK                                           (0x1U)
#define CIM_ETMROPT1_ETMR0SS1C_SHIFT                                          (0U)
#define CIM_ETMROPT1_ETMR0SS1C(x)                                             (((uint32_t)(((uint32_t)(x)) << CIM_ETMROPT1_ETMR0SS1C_SHIFT)) & CIM_ETMROPT1_ETMR0SS1C_MASK)
/* CIM_DIEINFO Register */
#define CIM_DIEINFO_FAMID_MASK                                                (0xF0000000U)
#define CIM_DIEINFO_FAMID_SHIFT                                               (28U)
#define CIM_DIEINFO_FAMID(x)                                                  (((uint32_t)(((uint32_t)(x)) << CIM_DIEINFO_FAMID_SHIFT)) & CIM_DIEINFO_FAMID_MASK)
#define CIM_DIEINFO_GENID_MASK                                                (0xF000000U)
#define CIM_DIEINFO_GENID_SHIFT                                               (24U)
#define CIM_DIEINFO_GENID(x)                                                  (((uint32_t)(((uint32_t)(x)) << CIM_DIEINFO_GENID_SHIFT)) & CIM_DIEINFO_GENID_MASK)
#define CIM_DIEINFO_SUBFAMIDH_MASK                                            (0xF00000U)
#define CIM_DIEINFO_SUBFAMIDH_SHIFT                                           (20U)
#define CIM_DIEINFO_SUBFAMIDH(x)                                              (((uint32_t)(((uint32_t)(x)) << CIM_DIEINFO_SUBFAMIDH_SHIFT)) & CIM_DIEINFO_SUBFAMIDH_MASK)
#define CIM_DIEINFO_SUBFAMIDL_MASK                                            (0xF0000U)
#define CIM_DIEINFO_SUBFAMIDL_SHIFT                                           (16U)
#define CIM_DIEINFO_SUBFAMIDL(x)                                              (((uint32_t)(((uint32_t)(x)) << CIM_DIEINFO_SUBFAMIDL_SHIFT)) & CIM_DIEINFO_SUBFAMIDL_MASK)
#define CIM_DIEINFO_MAJVER_MASK                                               (0xF0U)
#define CIM_DIEINFO_MAJVER_SHIFT                                              (4U)
#define CIM_DIEINFO_MAJVER(x)                                                 (((uint32_t)(((uint32_t)(x)) << CIM_DIEINFO_MAJVER_SHIFT)) & CIM_DIEINFO_MAJVER_MASK)
#define CIM_DIEINFO_MINVER_MASK                                               (0xFU)
#define CIM_DIEINFO_MINVER_SHIFT                                              (0U)
#define CIM_DIEINFO_MINVER(x)                                                 (((uint32_t)(((uint32_t)(x)) << CIM_DIEINFO_MINVER_SHIFT)) & CIM_DIEINFO_MINVER_MASK)
/* CIM_CHIPINFO Register */
#define CIM_CHIPINFO_FLEXCAN1_EN_MASK                                         (0x8000000U)
#define CIM_CHIPINFO_FLEXCAN1_EN_SHIFT                                        (27U)
#define CIM_CHIPINFO_FLEXCAN1_EN(x)                                           (((uint32_t)(((uint32_t)(x)) << CIM_CHIPINFO_FLEXCAN1_EN_SHIFT)) & CIM_CHIPINFO_FLEXCAN1_EN_MASK)
#define CIM_CHIPINFO_FLEXCAN0_EN_MASK                                         (0x4000000U)
#define CIM_CHIPINFO_FLEXCAN0_EN_SHIFT                                        (26U)
#define CIM_CHIPINFO_FLEXCAN0_EN(x)                                           (((uint32_t)(((uint32_t)(x)) << CIM_CHIPINFO_FLEXCAN0_EN_SHIFT)) & CIM_CHIPINFO_FLEXCAN0_EN_MASK)
#define CIM_CHIPINFO_FLEXCAN1_FD_EN_MASK                                      (0x2000000U)
#define CIM_CHIPINFO_FLEXCAN1_FD_EN_SHIFT                                     (25U)
#define CIM_CHIPINFO_FLEXCAN1_FD_EN(x)                                        (((uint32_t)(((uint32_t)(x)) << CIM_CHIPINFO_FLEXCAN1_FD_EN_SHIFT)) & CIM_CHIPINFO_FLEXCAN1_FD_EN_MASK)
#define CIM_CHIPINFO_FLEXCAN0_FD_EN_MASK                                      (0x1000000U)
#define CIM_CHIPINFO_FLEXCAN0_FD_EN_SHIFT                                     (24U)
#define CIM_CHIPINFO_FLEXCAN0_FD_EN(x)                                        (((uint32_t)(((uint32_t)(x)) << CIM_CHIPINFO_FLEXCAN0_FD_EN_SHIFT)) & CIM_CHIPINFO_FLEXCAN0_FD_EN_MASK)
#define CIM_CHIPINFO_PKGS_MASK                                                (0xF00000U)
#define CIM_CHIPINFO_PKGS_SHIFT                                               (20U)
#define CIM_CHIPINFO_PKGS(x)                                                  (((uint32_t)(((uint32_t)(x)) << CIM_CHIPINFO_PKGS_SHIFT)) & CIM_CHIPINFO_PKGS_MASK)
#define CIM_CHIPINFO_SRAMS_MASK                                               (0xF0000U)
#define CIM_CHIPINFO_SRAMS_SHIFT                                              (16U)
#define CIM_CHIPINFO_SRAMS(x)                                                 (((uint32_t)(((uint32_t)(x)) << CIM_CHIPINFO_SRAMS_SHIFT)) & CIM_CHIPINFO_SRAMS_MASK)
#define CIM_CHIPINFO_PFLASHS_MASK                                             (0xF00U)
#define CIM_CHIPINFO_PFLASHS_SHIFT                                            (8U)
#define CIM_CHIPINFO_PFLASHS(x)                                               (((uint32_t)(((uint32_t)(x)) << CIM_CHIPINFO_PFLASHS_SHIFT)) & CIM_CHIPINFO_PFLASHS_MASK)


/*!
 * @}
 */ /* end of group CIM_Register_Masks */


/*!
 * @}
 */ /* end of group CIM_Peripheral_Access_Layer */


/* -------------------------------------------------------------
   -- TMU Peripheral Access Layer
   ------------------------------------------------------------- */

/*!
 * @addtogroup TMU_Peripheral_Access_Layer TMU Peripheral Access Layer
 * @{
 */


/** TMU - Size of Registers Arrays */
#define TMU_MUX_COUNT                                                         (16U)

/* TMU Register Layout Typedef */
typedef struct {
    __IO uint32_t MUX[16];                                                    /**< MUX, offset: 0x0000 */

} TMU_Type, *TMU_MemMapPtr;

/** Number of instances of the TMU module. */
#define TMU_INSTANCE_COUNT                                                    (1U)

/* TMU base address */
#define TMU_BASE                                                              (0x40048000U)
#define TMU                                                                   ((TMU_Type *)(TMU_BASE))

/** Array initializer of TMU peripheral base addresses */
#define TMU_BASE_ADDRS                                                        { TMU_BASE }
/** Array initializer of TMU peripheral base pointers */
#define TMU_BASE_PTRS                                                         { TMU }

/* TMU Register Mask */
/* TMU_MUX Register */
#define TMU_MUX_LOCK_MASK                                                     (0x80000000U)
#define TMU_MUX_LOCK_SHIFT                                                    (31U)
#define TMU_MUX_LOCK(x)                                                       (((uint32_t)(((uint32_t)(x)) << TMU_MUX_LOCK_SHIFT)) & TMU_MUX_LOCK_MASK)
#define TMU_MUX_SEL3_WIDTH                                                    (6)
#define TMU_MUX_SEL3_MASK                                                     (0x3F000000U)
#define TMU_MUX_SEL3_SHIFT                                                    (24U)
#define TMU_MUX_SEL3(x)                                                       (((uint32_t)(((uint32_t)(x)) << TMU_MUX_SEL3_SHIFT)) & TMU_MUX_SEL3_MASK)
#define TMU_MUX_SEL2_WIDTH                                                    (6)
#define TMU_MUX_SEL2_MASK                                                     (0x3F0000U)
#define TMU_MUX_SEL2_SHIFT                                                    (16U)
#define TMU_MUX_SEL2(x)                                                       (((uint32_t)(((uint32_t)(x)) << TMU_MUX_SEL2_SHIFT)) & TMU_MUX_SEL2_MASK)
#define TMU_MUX_SEL1_WIDTH                                                    (6)
#define TMU_MUX_SEL1_MASK                                                     (0x3F00U)
#define TMU_MUX_SEL1_SHIFT                                                    (8U)
#define TMU_MUX_SEL1(x)                                                       (((uint32_t)(((uint32_t)(x)) << TMU_MUX_SEL1_SHIFT)) & TMU_MUX_SEL1_MASK)
#define TMU_MUX_SEL0_WIDTH                                                    (6)
#define TMU_MUX_SEL0_MASK                                                     (0x3FU)
#define TMU_MUX_SEL0_SHIFT                                                    (0U)
#define TMU_MUX_SEL0(x)                                                       (((uint32_t)(((uint32_t)(x)) << TMU_MUX_SEL0_SHIFT)) & TMU_MUX_SEL0_MASK)


/*!
 * @}
 */ /* end of group TMU_Register_Masks */


/*!
 * @}
 */ /* end of group TMU_Peripheral_Access_Layer */


/* -------------------------------------------------------------
   -- SCU Peripheral Access Layer
   ------------------------------------------------------------- */

/*!
 * @addtogroup SCU_Peripheral_Access_Layer SCU Peripheral Access Layer
 * @{
 */


/** SCU - Size of Registers Arrays */
#define SCU_CMUCMP_COUNT                                                      (2U)

/* SCU Register Layout Typedef */
typedef struct {
    __IO uint32_t CLKS;                                                       /**< Clock Selection Register, offset: 0x0000 */
    __IO uint32_t DIV;                                                        /**< System Clock Divider, offset: 0x0004 */
    __I  uint32_t STS;                                                        /**< Status Register, offset: 0x0008 */
    __I  uint32_t DIVSTS;                                                     /**< Clock Divider Status Register, offset: 0x000C */
    __IO uint32_t FIRC_CTRL;                                                  /**< FIRC Control Register, offset: 0x0010 */
    __IO uint32_t SIRC_CTRL;                                                  /**< SIRC Control Register, offset: 0x0014 */
    __IO uint32_t FXOSC_CTRL;                                                 /**< FXOSC Control Register, offset: 0x0018 */
    __IO uint8_t  RESERVED7[16];                                              /**< RESERVED7, offset: 0x001C */
    __IO uint32_t CLKO;                                                       /**< Clock Output Control Register, offset: 0x002C */
    __IO uint32_t RESERVED9;                                                  /**< RESERVED9, offset: 0x0030 */
    __IO uint32_t RESERVED10;                                                 /**< RESERVED10, offset: 0x0034 */
    __IO uint32_t RESERVED11;                                                 /**< RESERVED11, offset: 0x0038 */
    __IO uint8_t  RESERVED12[ 4];                                             /**< RESERVED12, offset: 0x003C */
    __IO uint32_t CMU_CTRL;                                                   /**< Clock Monitor Unit Control Register, offset: 0x0040 */
    __IO uint32_t CMUSTS;                                                     /**< Clock Monitor Unit Status Register, offset: 0x0044 */
    struct {
        __IO uint32_t HIGH;                                                   /**< Clock Monitor Unit High Comparison Value Register, offset: 0x0048 */
        __IO uint32_t LOW;                                                    /**< Clock Monitor Unit Low Comparison Value Register, offset: 0x004C */

    } CMUCMP[2];

} SCU_Type, *SCU_MemMapPtr;

/** Number of instances of the SCU module. */
#define SCU_INSTANCE_COUNT                                                    (1U)

/* SCU base address */
#define SCU_BASE                                                              (0x4007C000U)
#define SCU                                                                   ((SCU_Type *)(SCU_BASE))

/** Array initializer of SCU peripheral base addresses */
#define SCU_BASE_ADDRS                                                        { SCU_BASE }
/** Array initializer of SCU peripheral base pointers */
#define SCU_BASE_PTRS                                                         { SCU }
/** Number of interrupt channels for the SCU. */
#define SCU_IRQS_CH_COUNT                                                     (1U)
/** Interrupt vectors for the SCU peripheral type */
#define SCU_IRQS                                                              { SCU_IRQn }

/* SCU Register Mask */
/* SCU_CLKS Register */
#define SCU_CLKS_CLKSRCSEL_MASK                                               (0x3U)
#define SCU_CLKS_CLKSRCSEL_SHIFT                                              (0U)
#define SCU_CLKS_CLKSRCSEL(x)                                                 (((uint32_t)(((uint32_t)(x)) << SCU_CLKS_CLKSRCSEL_SHIFT)) & SCU_CLKS_CLKSRCSEL_MASK)
/* SCU_DIV Register */
#define SCU_DIV_CPDIVS_MASK                                                   (0xF0000U)
#define SCU_DIV_CPDIVS_SHIFT                                                  (16U)
#define SCU_DIV_CPDIVS(x)                                                     (((uint32_t)(((uint32_t)(x)) << SCU_DIV_CPDIVS_SHIFT)) & SCU_DIV_CPDIVS_MASK)
#define SCU_DIV_FBDIVS_MASK                                                   (0xF00U)
#define SCU_DIV_FBDIVS_SHIFT                                                  (8U)
#define SCU_DIV_FBDIVS(x)                                                     (((uint32_t)(((uint32_t)(x)) << SCU_DIV_FBDIVS_SHIFT)) & SCU_DIV_FBDIVS_MASK)
#define SCU_DIV_SBDIVS_MASK                                                   (0xFU)
#define SCU_DIV_SBDIVS_SHIFT                                                  (0U)
#define SCU_DIV_SBDIVS(x)                                                     (((uint32_t)(((uint32_t)(x)) << SCU_DIV_SBDIVS_SHIFT)) & SCU_DIV_SBDIVS_MASK)
/* SCU_STS Register */
#define SCU_STS_FXOSC_VALID_MASK                                              (0x8U)
#define SCU_STS_FXOSC_VALID_SHIFT                                             (3U)
#define SCU_STS_FXOSC_VALID(x)                                                (((uint32_t)(((uint32_t)(x)) << SCU_STS_FXOSC_VALID_SHIFT)) & SCU_STS_FXOSC_VALID_MASK)
#define SCU_STS_FIRC_VALID_MASK                                               (0x4U)
#define SCU_STS_FIRC_VALID_SHIFT                                              (2U)
#define SCU_STS_FIRC_VALID(x)                                                 (((uint32_t)(((uint32_t)(x)) << SCU_STS_FIRC_VALID_SHIFT)) & SCU_STS_FIRC_VALID_MASK)
#define SCU_STS_CLKST_MASK                                                    (0x3U)
#define SCU_STS_CLKST_SHIFT                                                   (0U)
#define SCU_STS_CLKST(x)                                                      (((uint32_t)(((uint32_t)(x)) << SCU_STS_CLKST_SHIFT)) & SCU_STS_CLKST_MASK)
/* SCU_DIVSTS Register */
#define SCU_DIVSTS_CPDIVST_MASK                                               (0xF0000U)
#define SCU_DIVSTS_CPDIVST_SHIFT                                              (16U)
#define SCU_DIVSTS_CPDIVST(x)                                                 (((uint32_t)(((uint32_t)(x)) << SCU_DIVSTS_CPDIVST_SHIFT)) & SCU_DIVSTS_CPDIVST_MASK)
#define SCU_DIVSTS_FBDIVST_MASK                                               (0xF00U)
#define SCU_DIVSTS_FBDIVST_SHIFT                                              (8U)
#define SCU_DIVSTS_FBDIVST(x)                                                 (((uint32_t)(((uint32_t)(x)) << SCU_DIVSTS_FBDIVST_SHIFT)) & SCU_DIVSTS_FBDIVST_MASK)
#define SCU_DIVSTS_SBDIVST_MASK                                               (0xFU)
#define SCU_DIVSTS_SBDIVST_SHIFT                                              (0U)
#define SCU_DIVSTS_SBDIVST(x)                                                 (((uint32_t)(((uint32_t)(x)) << SCU_DIVSTS_SBDIVST_SHIFT)) & SCU_DIVSTS_SBDIVST_MASK)
/* SCU_FIRC_CTRL Register */
#define SCU_FIRC_CTRL_FIRC_DLY_EN_MASK                                        (0x8U)
#define SCU_FIRC_CTRL_FIRC_DLY_EN_SHIFT                                       (3U)
#define SCU_FIRC_CTRL_FIRC_DLY_EN(x)                                          (((uint32_t)(((uint32_t)(x)) << SCU_FIRC_CTRL_FIRC_DLY_EN_SHIFT)) & SCU_FIRC_CTRL_FIRC_DLY_EN_MASK)
#define SCU_FIRC_CTRL_FIRC_DS_EN_MASK                                         (0x2U)
#define SCU_FIRC_CTRL_FIRC_DS_EN_SHIFT                                        (1U)
#define SCU_FIRC_CTRL_FIRC_DS_EN(x)                                           (((uint32_t)(((uint32_t)(x)) << SCU_FIRC_CTRL_FIRC_DS_EN_SHIFT)) & SCU_FIRC_CTRL_FIRC_DS_EN_MASK)
#define SCU_FIRC_CTRL_FIRC_EN_MASK                                            (0x1U)
#define SCU_FIRC_CTRL_FIRC_EN_SHIFT                                           (0U)
#define SCU_FIRC_CTRL_FIRC_EN(x)                                              (((uint32_t)(((uint32_t)(x)) << SCU_FIRC_CTRL_FIRC_EN_SHIFT)) & SCU_FIRC_CTRL_FIRC_EN_MASK)
/* SCU_SIRC_CTRL Register */
#define SCU_SIRC_CTRL_SIRC_STB_EN_MASK                                        (0x4U)
#define SCU_SIRC_CTRL_SIRC_STB_EN_SHIFT                                       (2U)
#define SCU_SIRC_CTRL_SIRC_STB_EN(x)                                          (((uint32_t)(((uint32_t)(x)) << SCU_SIRC_CTRL_SIRC_STB_EN_SHIFT)) & SCU_SIRC_CTRL_SIRC_STB_EN_MASK)
#define SCU_SIRC_CTRL_SIRC_DS_EN_MASK                                         (0x2U)
#define SCU_SIRC_CTRL_SIRC_DS_EN_SHIFT                                        (1U)
#define SCU_SIRC_CTRL_SIRC_DS_EN(x)                                           (((uint32_t)(((uint32_t)(x)) << SCU_SIRC_CTRL_SIRC_DS_EN_SHIFT)) & SCU_SIRC_CTRL_SIRC_DS_EN_MASK)
/* SCU_FXOSC_CTRL Register */
#define SCU_FXOSC_CTRL_ALCDIS_MASK                                            (0x4000U)
#define SCU_FXOSC_CTRL_ALCDIS_SHIFT                                           (14U)
#define SCU_FXOSC_CTRL_ALCDIS(x)                                              (((uint32_t)(((uint32_t)(x)) << SCU_FXOSC_CTRL_ALCDIS_SHIFT)) & SCU_FXOSC_CTRL_ALCDIS_MASK)
#define SCU_FXOSC_CTRL_DLY_SEL_MASK                                           (0x3000U)
#define SCU_FXOSC_CTRL_DLY_SEL_SHIFT                                          (12U)
#define SCU_FXOSC_CTRL_DLY_SEL(x)                                             (((uint32_t)(((uint32_t)(x)) << SCU_FXOSC_CTRL_DLY_SEL_SHIFT)) & SCU_FXOSC_CTRL_DLY_SEL_MASK)
#define SCU_FXOSC_CTRL_GMSEL_MASK                                             (0x700U)
#define SCU_FXOSC_CTRL_GMSEL_SHIFT                                            (8U)
#define SCU_FXOSC_CTRL_GMSEL(x)                                               (((uint32_t)(((uint32_t)(x)) << SCU_FXOSC_CTRL_GMSEL_SHIFT)) & SCU_FXOSC_CTRL_GMSEL_MASK)
#define SCU_FXOSC_CTRL_FXOSC_MODE_MASK                                        (0x8U)
#define SCU_FXOSC_CTRL_FXOSC_MODE_SHIFT                                       (3U)
#define SCU_FXOSC_CTRL_FXOSC_MODE(x)                                          (((uint32_t)(((uint32_t)(x)) << SCU_FXOSC_CTRL_FXOSC_MODE_SHIFT)) & SCU_FXOSC_CTRL_FXOSC_MODE_MASK)
#define SCU_FXOSC_CTRL_FXOSC_DS_EN_MASK                                       (0x2U)
#define SCU_FXOSC_CTRL_FXOSC_DS_EN_SHIFT                                      (1U)
#define SCU_FXOSC_CTRL_FXOSC_DS_EN(x)                                         (((uint32_t)(((uint32_t)(x)) << SCU_FXOSC_CTRL_FXOSC_DS_EN_SHIFT)) & SCU_FXOSC_CTRL_FXOSC_DS_EN_MASK)
#define SCU_FXOSC_CTRL_FXOSC_EN_MASK                                          (0x1U)
#define SCU_FXOSC_CTRL_FXOSC_EN_SHIFT                                         (0U)
#define SCU_FXOSC_CTRL_FXOSC_EN(x)                                            (((uint32_t)(((uint32_t)(x)) << SCU_FXOSC_CTRL_FXOSC_EN_SHIFT)) & SCU_FXOSC_CTRL_FXOSC_EN_MASK)
/* SCU_CLKO Register */
#define SCU_CLKO_CLKODIV_MASK                                                 (0xFF00U)
#define SCU_CLKO_CLKODIV_SHIFT                                                (8U)
#define SCU_CLKO_CLKODIV(x)                                                   (((uint32_t)(((uint32_t)(x)) << SCU_CLKO_CLKODIV_SHIFT)) & SCU_CLKO_CLKODIV_MASK)
#define SCU_CLKO_CLKOSEL_MASK                                                 (0x7U)
#define SCU_CLKO_CLKOSEL_SHIFT                                                (0U)
#define SCU_CLKO_CLKOSEL(x)                                                   (((uint32_t)(((uint32_t)(x)) << SCU_CLKO_CLKOSEL_SHIFT)) & SCU_CLKO_CLKOSEL_MASK)
/* SCU_CMU_CTRL Register */
#define SCU_CMU_CTRL_CMU1_RE_MASK                                             (0x200U)
#define SCU_CMU_CTRL_CMU1_RE_SHIFT                                            (9U)
#define SCU_CMU_CTRL_CMU1_RE(x)                                               (((uint32_t)(((uint32_t)(x)) << SCU_CMU_CTRL_CMU1_RE_SHIFT)) & SCU_CMU_CTRL_CMU1_RE_MASK)
#define SCU_CMU_CTRL_CMU0_RE_MASK                                             (0x100U)
#define SCU_CMU_CTRL_CMU0_RE_SHIFT                                            (8U)
#define SCU_CMU_CTRL_CMU0_RE(x)                                               (((uint32_t)(((uint32_t)(x)) << SCU_CMU_CTRL_CMU0_RE_SHIFT)) & SCU_CMU_CTRL_CMU0_RE_MASK)
#define SCU_CMU_CTRL_CMU1_EN_MASK                                             (0x2U)
#define SCU_CMU_CTRL_CMU1_EN_SHIFT                                            (1U)
#define SCU_CMU_CTRL_CMU1_EN(x)                                               (((uint32_t)(((uint32_t)(x)) << SCU_CMU_CTRL_CMU1_EN_SHIFT)) & SCU_CMU_CTRL_CMU1_EN_MASK)
#define SCU_CMU_CTRL_CMU0_EN_MASK                                             (0x1U)
#define SCU_CMU_CTRL_CMU0_EN_SHIFT                                            (0U)
#define SCU_CMU_CTRL_CMU0_EN(x)                                               (((uint32_t)(((uint32_t)(x)) << SCU_CMU_CTRL_CMU0_EN_SHIFT)) & SCU_CMU_CTRL_CMU0_EN_MASK)
/* SCU_CMUSTS Register */
#define SCU_CMUSTS_CMU1_LOSC_MASK                                             (0x20000U)
#define SCU_CMUSTS_CMU1_LOSC_SHIFT                                            (17U)
#define SCU_CMUSTS_CMU1_LOSC(x)                                               (((uint32_t)(((uint32_t)(x)) << SCU_CMUSTS_CMU1_LOSC_SHIFT)) & SCU_CMUSTS_CMU1_LOSC_MASK)
#define SCU_CMUSTS_CMU0_LOSC_MASK                                             (0x10000U)
#define SCU_CMUSTS_CMU0_LOSC_SHIFT                                            (16U)
#define SCU_CMUSTS_CMU0_LOSC(x)                                               (((uint32_t)(((uint32_t)(x)) << SCU_CMUSTS_CMU0_LOSC_SHIFT)) & SCU_CMUSTS_CMU0_LOSC_MASK)
#define SCU_CMUSTS_CMU1_LOSR_MASK                                             (0x200U)
#define SCU_CMUSTS_CMU1_LOSR_SHIFT                                            (9U)
#define SCU_CMUSTS_CMU1_LOSR(x)                                               (((uint32_t)(((uint32_t)(x)) << SCU_CMUSTS_CMU1_LOSR_SHIFT)) & SCU_CMUSTS_CMU1_LOSR_MASK)
#define SCU_CMUSTS_CMU0_LOSR_MASK                                             (0x100U)
#define SCU_CMUSTS_CMU0_LOSR_SHIFT                                            (8U)
#define SCU_CMUSTS_CMU0_LOSR(x)                                               (((uint32_t)(((uint32_t)(x)) << SCU_CMUSTS_CMU0_LOSR_SHIFT)) & SCU_CMUSTS_CMU0_LOSR_MASK)
#define SCU_CMUSTS_CMU1_OUTRNG_MASK                                           (0x2U)
#define SCU_CMUSTS_CMU1_OUTRNG_SHIFT                                          (1U)
#define SCU_CMUSTS_CMU1_OUTRNG(x)                                             (((uint32_t)(((uint32_t)(x)) << SCU_CMUSTS_CMU1_OUTRNG_SHIFT)) & SCU_CMUSTS_CMU1_OUTRNG_MASK)
#define SCU_CMUSTS_CMU0_OUTRNG_MASK                                           (0x1U)
#define SCU_CMUSTS_CMU0_OUTRNG_SHIFT                                          (0U)
#define SCU_CMUSTS_CMU0_OUTRNG(x)                                             (((uint32_t)(((uint32_t)(x)) << SCU_CMUSTS_CMU0_OUTRNG_SHIFT)) & SCU_CMUSTS_CMU0_OUTRNG_MASK)
/* SCU_CMUCMP_HIGH Register */
#define SCU_CMUCMP_HIGH_VAL_MASK                                              (0xFFFFU)
#define SCU_CMUCMP_HIGH_VAL_SHIFT                                             (0U)
#define SCU_CMUCMP_HIGH_VAL(x)                                                (((uint32_t)(((uint32_t)(x)) << SCU_CMUCMP_HIGH_VAL_SHIFT)) & SCU_CMUCMP_HIGH_VAL_MASK)
/* SCU_CMUCMP_LOW Register */
#define SCU_CMUCMP_LOW_VAL_MASK                                               (0xFFFFU)
#define SCU_CMUCMP_LOW_VAL_SHIFT                                              (0U)
#define SCU_CMUCMP_LOW_VAL(x)                                                 (((uint32_t)(((uint32_t)(x)) << SCU_CMUCMP_LOW_VAL_SHIFT)) & SCU_CMUCMP_LOW_VAL_MASK)



/*!
 * @}
 */ /* end of group SCU_Register_Masks */


/*!
 * @}
 */ /* end of group SCU_Peripheral_Access_Layer */


/* -------------------------------------------------------------
   -- IPC Peripheral Access Layer
   ------------------------------------------------------------- */

/*!
 * @addtogroup IPC_Peripheral_Access_Layer IPC Peripheral Access Layer
 * @{
 */


/** IPC - Size of Registers Arrays */
#define IPC_CTRL_COUNT                                                        (128U)

/* IPC Register Layout Typedef */
typedef struct {
    __IO uint32_t CTRL[128];                                                  /**< Control Register, offset: 0x0000 */

} IPC_Type, *IPC_MemMapPtr;

/** Number of instances of the IPC module. */
#define IPC_INSTANCE_COUNT                                                    (1U)

/* IPC base address */
#define IPC_BASE                                                              (0x4007D000U)
#define IPC                                                                   ((IPC_Type *)(IPC_BASE))

/** Array initializer of IPC peripheral base addresses */
#define IPC_BASE_ADDRS                                                        { IPC_BASE }
/** Array initializer of IPC peripheral base pointers */
#define IPC_BASE_PTRS                                                         { IPC }

/* IPC Register Mask */
/* IPC_CTRL Register */
#define IPC_CTRL_DIV_WIDTH                                                    (4)
#define IPC_CTRL_DIV_MASK                                                     (0xF0000U)
#define IPC_CTRL_DIV_SHIFT                                                    (16U)
#define IPC_CTRL_DIV(x)                                                       (((uint32_t)(((uint32_t)(x)) << IPC_CTRL_DIV_SHIFT)) & IPC_CTRL_DIV_MASK)
#define IPC_CTRL_SRCSEL_MASK                                                  (0x700U)
#define IPC_CTRL_SRCSEL_SHIFT                                                 (8U)
#define IPC_CTRL_SRCSEL(x)                                                    (((uint32_t)(((uint32_t)(x)) << IPC_CTRL_SRCSEL_SHIFT)) & IPC_CTRL_SRCSEL_MASK)
#define IPC_CTRL_SWREN_MASK                                                   (0x2U)
#define IPC_CTRL_SWREN_SHIFT                                                  (1U)
#define IPC_CTRL_SWREN(x)                                                     (((uint32_t)(((uint32_t)(x)) << IPC_CTRL_SWREN_SHIFT)) & IPC_CTRL_SWREN_MASK)
#define IPC_CTRL_CLKEN_MASK                                                   (0x1U)
#define IPC_CTRL_CLKEN_SHIFT                                                  (0U)
#define IPC_CTRL_CLKEN(x)                                                     (((uint32_t)(((uint32_t)(x)) << IPC_CTRL_CLKEN_SHIFT)) & IPC_CTRL_CLKEN_MASK)


/*!
 * @}
 */ /* end of group IPC_Register_Masks */


/*!
 * @}
 */ /* end of group IPC_Peripheral_Access_Layer */


/* -------------------------------------------------------------
   -- RCU Peripheral Access Layer
   ------------------------------------------------------------- */

/*!
 * @addtogroup RCU_Peripheral_Access_Layer RCU Peripheral Access Layer
 * @{
 */


/** RCU - Size of Registers Arrays */

/* RCU Register Layout Typedef */
typedef struct {
    __IO uint32_t RSSR;                                                       /**< Reset Source Status Register, offset: 0x0000 */
    __IO uint32_t RPFR;                                                       /**< Reset Pin Filter Register, offset: 0x0004 */

} RCU_Type, *RCU_MemMapPtr;

/** Number of instances of the RCU module. */
#define RCU_INSTANCE_COUNT                                                    (1U)

/* RCU base address */
#define RCU_BASE                                                              (0x4007F000U)
#define RCU                                                                   ((RCU_Type *)(RCU_BASE))
/*-------------------------- PPU PROTECTED REGISTER --------------------------- */
#define RCU_MIRROR_BASE                                                       (0x4007F800U)
#define RCU_MIRROR                                                            ((RCU_Type *)(RCU_MIRROR_BASE))

#define RCU_PPU_BASE                                                          (0x4007F000U)
#define RCU_PPU                                                               ((PPU_Type *)(RCU_BASE))
/*-------------------------- PPU PROTECTED REGISTER --------------------------- */


/** Array initializer of RCU peripheral base addresses */
#define RCU_BASE_ADDRS                                                        { RCU_BASE }
/** Array initializer of RCU peripheral base pointers */
#define RCU_BASE_PTRS                                                         { RCU }

/* RCU Register Mask */
/* RCU_RSSR Register */
#define RCU_RSSR_LVD_MASK                                                     (0x100000U)
#define RCU_RSSR_LVD_SHIFT                                                    (20U)
#define RCU_RSSR_LVD(x)                                                       (((uint32_t)(((uint32_t)(x)) << RCU_RSSR_LVD_SHIFT)) & RCU_RSSR_LVD_MASK)
#define RCU_RSSR_LPACK_MASK                                                   (0x80000U)
#define RCU_RSSR_LPACK_SHIFT                                                  (19U)
#define RCU_RSSR_LPACK(x)                                                     (((uint32_t)(((uint32_t)(x)) << RCU_RSSR_LPACK_SHIFT)) & RCU_RSSR_LPACK_MASK)
#define RCU_RSSR_WDG_MASK                                                     (0x40000U)
#define RCU_RSSR_WDG_SHIFT                                                    (18U)
#define RCU_RSSR_WDG(x)                                                       (((uint32_t)(((uint32_t)(x)) << RCU_RSSR_WDG_SHIFT)) & RCU_RSSR_WDG_MASK)
#define RCU_RSSR_CMU_MASK                                                     (0x20000U)
#define RCU_RSSR_CMU_SHIFT                                                    (17U)
#define RCU_RSSR_CMU(x)                                                       (((uint32_t)(((uint32_t)(x)) << RCU_RSSR_CMU_SHIFT)) & RCU_RSSR_CMU_MASK)
#define RCU_RSSR_LOCKUP_MASK                                                  (0x10000U)
#define RCU_RSSR_LOCKUP_SHIFT                                                 (16U)
#define RCU_RSSR_LOCKUP(x)                                                    (((uint32_t)(((uint32_t)(x)) << RCU_RSSR_LOCKUP_SHIFT)) & RCU_RSSR_LOCKUP_MASK)
#define RCU_RSSR_DBG_MASK                                                     (0x200U)
#define RCU_RSSR_DBG_SHIFT                                                    (9U)
#define RCU_RSSR_DBG(x)                                                       (((uint32_t)(((uint32_t)(x)) << RCU_RSSR_DBG_SHIFT)) & RCU_RSSR_DBG_MASK)
#define RCU_RSSR_SW_MASK                                                      (0x100U)
#define RCU_RSSR_SW_SHIFT                                                     (8U)
#define RCU_RSSR_SW(x)                                                        (((uint32_t)(((uint32_t)(x)) << RCU_RSSR_SW_SHIFT)) & RCU_RSSR_SW_MASK)
#define RCU_RSSR_PIN_MASK                                                     (0x2U)
#define RCU_RSSR_PIN_SHIFT                                                    (1U)
#define RCU_RSSR_PIN(x)                                                       (((uint32_t)(((uint32_t)(x)) << RCU_RSSR_PIN_SHIFT)) & RCU_RSSR_PIN_MASK)
#define RCU_RSSR_POR_BOR_MASK                                                 (0x1U)
#define RCU_RSSR_POR_BOR_SHIFT                                                (0U)
#define RCU_RSSR_POR_BOR(x)                                                   (((uint32_t)(((uint32_t)(x)) << RCU_RSSR_POR_BOR_SHIFT)) & RCU_RSSR_POR_BOR_MASK)
/* RCU_RPFR Register */
#define RCU_RPFR_RSTPIN_OBEN_MASK                                             (0x10000U)
#define RCU_RPFR_RSTPIN_OBEN_SHIFT                                            (16U)
#define RCU_RPFR_RSTPIN_OBEN(x)                                               (((uint32_t)(((uint32_t)(x)) << RCU_RPFR_RSTPIN_OBEN_SHIFT)) & RCU_RPFR_RSTPIN_OBEN_MASK)
#define RCU_RPFR_FLTVAL_MASK                                                  (0xFF00U)
#define RCU_RPFR_FLTVAL_SHIFT                                                 (8U)
#define RCU_RPFR_FLTVAL(x)                                                    (((uint32_t)(((uint32_t)(x)) << RCU_RPFR_FLTVAL_SHIFT)) & RCU_RPFR_FLTVAL_MASK)
#define RCU_RPFR_FLTCLKSEL_MASK                                               (0x4U)
#define RCU_RPFR_FLTCLKSEL_SHIFT                                              (2U)
#define RCU_RPFR_FLTCLKSEL(x)                                                 (((uint32_t)(((uint32_t)(x)) << RCU_RPFR_FLTCLKSEL_SHIFT)) & RCU_RPFR_FLTCLKSEL_MASK)
#define RCU_RPFR_FLTSTEN_MASK                                                 (0x2U)
#define RCU_RPFR_FLTSTEN_SHIFT                                                (1U)
#define RCU_RPFR_FLTSTEN(x)                                                   (((uint32_t)(((uint32_t)(x)) << RCU_RPFR_FLTSTEN_SHIFT)) & RCU_RPFR_FLTSTEN_MASK)
#define RCU_RPFR_FLTEN_MASK                                                   (0x1U)
#define RCU_RPFR_FLTEN_SHIFT                                                  (0U)
#define RCU_RPFR_FLTEN(x)                                                     (((uint32_t)(((uint32_t)(x)) << RCU_RPFR_FLTEN_SHIFT)) & RCU_RPFR_FLTEN_MASK)


/*!
 * @}
 */ /* end of group RCU_Register_Masks */


/*!
 * @}
 */ /* end of group RCU_Peripheral_Access_Layer */


/* -------------------------------------------------------------
   -- PCU Peripheral Access Layer
   ------------------------------------------------------------- */

/*!
 * @addtogroup PCU_Peripheral_Access_Layer PCU Peripheral Access Layer
 * @{
 */


/** PCU - Size of Registers Arrays */

/* PCU Register Layout Typedef */
typedef struct {
    __IO uint32_t BORCR;                                                      /**< BOR Control Register, offset: 0x0000 */
    __IO uint32_t LVDCR;                                                      /**< Low Voltage Detection Control Register, offset: 0x0004 */
    __IO uint32_t CSR;                                                        /**< Control and Status Register, offset: 0x0008 */
    __IO uint32_t RESERVED3;                                                  /**< RESERVED3, offset: 0x000C */
    __IO uint32_t RESERVED4;                                                  /**< RESERVED4, offset: 0x0010 */

} PCU_Type, *PCU_MemMapPtr;

/** Number of instances of the PCU module. */
#define PCU_INSTANCE_COUNT                                                    (1U)

/* PCU base address */
#define PCU_BASE                                                              (0x4007E000U)
#define PCU                                                                   ((PCU_Type *)(PCU_BASE))
/*-------------------------- PPU PROTECTED REGISTER --------------------------- */
#define PCU_MIRROR_BASE                                                       (0x4007E800U)
#define PCU_MIRROR                                                            ((PCU_Type *)(PCU_MIRROR_BASE))

#define PCU_PPU_BASE                                                          (0x4007E000U)
#define PCU_PPU                                                               ((PPU_Type *)(PCU_BASE))
/*-------------------------- PPU PROTECTED REGISTER --------------------------- */


/** Array initializer of PCU peripheral base addresses */
#define PCU_BASE_ADDRS                                                        { PCU_BASE }
/** Array initializer of PCU peripheral base pointers */
#define PCU_BASE_PTRS                                                         { PCU }

/* PCU Register Mask */
/* PCU_BORCR Register */
#define PCU_BORCR_BORLPMD_MASK                                                (0x2U)
#define PCU_BORCR_BORLPMD_SHIFT                                               (1U)
#define PCU_BORCR_BORLPMD(x)                                                  (((uint32_t)(((uint32_t)(x)) << PCU_BORCR_BORLPMD_SHIFT)) & PCU_BORCR_BORLPMD_MASK)
#define PCU_BORCR_BOREN_MASK                                                  (0x1U)
#define PCU_BORCR_BOREN_SHIFT                                                 (0U)
#define PCU_BORCR_BOREN(x)                                                    (((uint32_t)(((uint32_t)(x)) << PCU_BORCR_BOREN_SHIFT)) & PCU_BORCR_BOREN_MASK)
/* PCU_LVDCR Register */
#define PCU_LVDCR_LVD5VHYS_MASK                                               (0x300000U)
#define PCU_LVDCR_LVD5VHYS_SHIFT                                              (20U)
#define PCU_LVDCR_LVD5VHYS(x)                                                 (((uint32_t)(((uint32_t)(x)) << PCU_LVDCR_LVD5VHYS_SHIFT)) & PCU_LVDCR_LVD5VHYS_MASK)
#define PCU_LVDCR_LVD5VTRIM_MASK                                              (0xF0000U)
#define PCU_LVDCR_LVD5VTRIM_SHIFT                                             (16U)
#define PCU_LVDCR_LVD5VTRIM(x)                                                (((uint32_t)(((uint32_t)(x)) << PCU_LVDCR_LVD5VTRIM_SHIFT)) & PCU_LVDCR_LVD5VTRIM_MASK)
#define PCU_LVDCR_LVD5VOUT_MASK                                               (0x8000U)
#define PCU_LVDCR_LVD5VOUT_SHIFT                                              (15U)
#define PCU_LVDCR_LVD5VOUT(x)                                                 (((uint32_t)(((uint32_t)(x)) << PCU_LVDCR_LVD5VOUT_SHIFT)) & PCU_LVDCR_LVD5VOUT_MASK)
#define PCU_LVDCR_LVD5VFLAG_MASK                                              (0x4000U)
#define PCU_LVDCR_LVD5VFLAG_SHIFT                                             (14U)
#define PCU_LVDCR_LVD5VFLAG(x)                                                (((uint32_t)(((uint32_t)(x)) << PCU_LVDCR_LVD5VFLAG_SHIFT)) & PCU_LVDCR_LVD5VFLAG_MASK)
#define PCU_LVDCR_LVD5VIE_MASK                                                (0x2000U)
#define PCU_LVDCR_LVD5VIE_SHIFT                                               (13U)
#define PCU_LVDCR_LVD5VIE(x)                                                  (((uint32_t)(((uint32_t)(x)) << PCU_LVDCR_LVD5VIE_SHIFT)) & PCU_LVDCR_LVD5VIE_MASK)
#define PCU_LVDCR_LVD5VRE_MASK                                                (0x1000U)
#define PCU_LVDCR_LVD5VRE_SHIFT                                               (12U)
#define PCU_LVDCR_LVD5VRE(x)                                                  (((uint32_t)(((uint32_t)(x)) << PCU_LVDCR_LVD5VRE_SHIFT)) & PCU_LVDCR_LVD5VRE_MASK)
#define PCU_LVDCR_LVD1P5VFLAG_MASK                                            (0x200U)
#define PCU_LVDCR_LVD1P5VFLAG_SHIFT                                           (9U)
#define PCU_LVDCR_LVD1P5VFLAG(x)                                              (((uint32_t)(((uint32_t)(x)) << PCU_LVDCR_LVD1P5VFLAG_SHIFT)) & PCU_LVDCR_LVD1P5VFLAG_MASK)
#define PCU_LVDCR_LVD1P5VEN_MASK                                              (0x100U)
#define PCU_LVDCR_LVD1P5VEN_SHIFT                                             (8U)
#define PCU_LVDCR_LVD1P5VEN(x)                                                (((uint32_t)(((uint32_t)(x)) << PCU_LVDCR_LVD1P5VEN_SHIFT)) & PCU_LVDCR_LVD1P5VEN_MASK)
#define PCU_LVDCR_FLTCFG_MASK                                                 (0x18U)
#define PCU_LVDCR_FLTCFG_SHIFT                                                (3U)
#define PCU_LVDCR_FLTCFG(x)                                                   (((uint32_t)(((uint32_t)(x)) << PCU_LVDCR_FLTCFG_SHIFT)) & PCU_LVDCR_FLTCFG_MASK)
#define PCU_LVDCR_LVD5VFLTEN_MASK                                             (0x4U)
#define PCU_LVDCR_LVD5VFLTEN_SHIFT                                            (2U)
#define PCU_LVDCR_LVD5VFLTEN(x)                                               (((uint32_t)(((uint32_t)(x)) << PCU_LVDCR_LVD5VFLTEN_SHIFT)) & PCU_LVDCR_LVD5VFLTEN_MASK)
#define PCU_LVDCR_LVD5VLPMD_MASK                                              (0x2U)
#define PCU_LVDCR_LVD5VLPMD_SHIFT                                             (1U)
#define PCU_LVDCR_LVD5VLPMD(x)                                                (((uint32_t)(((uint32_t)(x)) << PCU_LVDCR_LVD5VLPMD_SHIFT)) & PCU_LVDCR_LVD5VLPMD_MASK)
#define PCU_LVDCR_LVD5VEN_MASK                                                (0x1U)
#define PCU_LVDCR_LVD5VEN_SHIFT                                               (0U)
#define PCU_LVDCR_LVD5VEN(x)                                                  (((uint32_t)(((uint32_t)(x)) << PCU_LVDCR_LVD5VEN_SHIFT)) & PCU_LVDCR_LVD5VEN_MASK)
/* PCU_CSR Register */
#define PCU_CSR_SET1V_MASK                                                    (0x400000U)
#define PCU_CSR_SET1V_SHIFT                                                   (22U)
#define PCU_CSR_SET1V(x)                                                      (((uint32_t)(((uint32_t)(x)) << PCU_CSR_SET1V_SHIFT)) & PCU_CSR_SET1V_MASK)
#define PCU_CSR_PMCHPRDY_MASK                                                 (0x200000U)
#define PCU_CSR_PMCHPRDY_SHIFT                                                (21U)
#define PCU_CSR_PMCHPRDY(x)                                                   (((uint32_t)(((uint32_t)(x)) << PCU_CSR_PMCHPRDY_SHIFT)) & PCU_CSR_PMCHPRDY_MASK)
#define PCU_CSR_ESCFG_MASK                                                    (0xF0000U)
#define PCU_CSR_ESCFG_SHIFT                                                   (16U)
#define PCU_CSR_ESCFG(x)                                                      (((uint32_t)(((uint32_t)(x)) << PCU_CSR_ESCFG_SHIFT)) & PCU_CSR_ESCFG_MASK)
#define PCU_CSR_PMCHPRDYDCFG_MASK                                             (0x30U)
#define PCU_CSR_PMCHPRDYDCFG_SHIFT                                            (4U)
#define PCU_CSR_PMCHPRDYDCFG(x)                                               (((uint32_t)(((uint32_t)(x)) << PCU_CSR_PMCHPRDYDCFG_SHIFT)) & PCU_CSR_PMCHPRDYDCFG_MASK)
#define PCU_CSR_PMCLPEN_MASK                                                  (0x1U)
#define PCU_CSR_PMCLPEN_SHIFT                                                 (0U)
#define PCU_CSR_PMCLPEN(x)                                                    (((uint32_t)(((uint32_t)(x)) << PCU_CSR_PMCLPEN_SHIFT)) & PCU_CSR_PMCLPEN_MASK)


/*!
 * @}
 */ /* end of group PCU_Register_Masks */


/*!
 * @}
 */ /* end of group PCU_Peripheral_Access_Layer */


/* -------------------------------------------------------------
   -- CAN Peripheral Access Layer
   ------------------------------------------------------------- */

/*!
 * @addtogroup CAN_Peripheral_Access_Layer CAN Peripheral Access Layer
 * @{
 */


/** CAN - Size of Registers Arrays */
#define CAN_RAM_COUNT                                                         (256U)
#define CAN_RXIMR_COUNT                                                       (64U)
#define CAN_WMB_COUNT                                                         (4U)
#define CAN_HR_TIME_STAMPn_COUNT                                              (128U)
#define CAN_ERFFELn_COUNT                                                     (128U)

/* CAN Register Layout Typedef */
typedef struct {
    __IO uint32_t MCR;                                                        /**< Module Configuration Register, offset: 0x0000 */
    __IO uint32_t CTRL1;                                                      /**< Countrol 1 Register, offset: 0x0004 */
    __IO uint32_t TIMER;                                                      /**< Free Running Timer Register, offset: 0x0008 */
    __IO uint32_t RESERVED3[ 1];                                              /**< RESERVED3, offset: 0x000C */
    __IO uint32_t RXMGMASK;                                                   /**< Rx Mailboxes Global Mask Register, offset: 0x0010 */
    __IO uint32_t RX14MASK;                                                   /**< Rx 14 Mask Register, offset: 0x0014 */
    __IO uint32_t RX15MASK;                                                   /**< Rx 15 Mask Register, offset: 0x0018 */
    __IO uint32_t ECR;                                                        /**< Error Counter Register, offset: 0x001C */
    __IO uint32_t ESR1;                                                       /**< Error and Status Register, offset: 0x0020 */
    __IO uint32_t IMASK2;                                                     /**< Interrupt Mask 2 Register, offset: 0x0024 */
    __IO uint32_t IMASK1;                                                     /**< Interrupt Mask 1 Register, offset: 0x0028 */
    __IO uint32_t IFLAG2;                                                     /**< Interrupt Flag 2 Register, offset: 0x002C */
    __IO uint32_t IFLAG1;                                                     /**< Interrupt Flag 1 Register, offset: 0x0030 */
    __IO uint32_t CTRL2;                                                      /**< Control 2 Register, offset: 0x0034 */
    __IO uint32_t ESR2;                                                       /**< Error and Status 2 Register, offset: 0x0038 */
    __IO uint32_t RESERVED15[ 2];                                             /**< RESERVED15, offset: 0x003C */
    __IO uint32_t CRCR;                                                       /**< CRC Register, offset: 0x0044 */
    __IO uint32_t RXFGMASK;                                                   /**< Legacy Rx FIFO Global Mask Register, offset: 0x0048 */
    __IO uint32_t RXFIR;                                                      /**< Legacy Rx FIFO Information Register, offset: 0x004C */
    __IO uint32_t CBT;                                                        /**< Bit Timing Register, offset: 0x0050 */
    __IO uint32_t RESERVED20[ 5];                                             /**< RESERVED20, offset: 0x0054 */
    __IO uint32_t IMASK4;                                                     /**< Interrupt Mask 4 Register, offset: 0x0068 */
    __IO uint32_t IMASK3;                                                     /**< Interrupt Mask 3 Register, offset: 0x006C */
    __IO uint32_t IFLAG4;                                                     /**< Interrupt Flags 4 Register, offset: 0x0070 */
    __IO uint32_t IFLAG3;                                                     /**< Interrupt Flags 3 Register, offset: 0x0074 */
    __IO uint32_t RESERVED25[ 2];                                             /**< RESERVED25, offset: 0x0078 */
    __IO uint32_t RAM[256];                                                   /**< Embeded RAM, offset: 0x0080 */
    __IO uint32_t RESERVED27[256];                                            /**< RESERVED27, offset: 0x0480 */
    __IO uint32_t RXIMR[64];                                                  /**< RX Individual Mask Register, offset: 0x0880 */
    __IO uint32_t RESERVED29[88];                                             /**< RESERVED29, offset: 0x0980 */
    __IO uint32_t MECR;                                                       /**< Memory Error Control Register, offset: 0x0AE0 */
    __IO uint32_t ERRIAR;                                                     /**< Error Injection Address Register, offset: 0x0AE4 */
    __IO uint32_t ERRIDPR;                                                    /**< Error Injection Data Pattern Register, offset: 0x0AE8 */
    __IO uint32_t ERRIPPR;                                                    /**< Error Injection Parity Pattern Register, offset: 0x0AEC */
    __IO uint32_t RERRAR;                                                     /**< Error Report Address Register, offset: 0x0AF0 */
    __IO uint32_t RERRDR;                                                     /**< Error Report Data Register, offset: 0x0AF4 */
    __IO uint32_t RERRSYNR;                                                   /**< Error Report Syndrome Register, offset: 0x0AF8 */
    __IO uint32_t ERRSR;                                                      /**< Error Status Register, offset: 0x0AFC */
    __IO uint32_t CTRL1_PN;                                                   /**< Pretended Networking Control 1 Register, offset: 0x0B00 */
    __IO uint32_t CTRL2_PN;                                                   /**< Pretended Networking Control 2 Register, offset: 0x0B04 */
    __IO uint32_t WU_MTC;                                                     /**< Pretended Networking Wake Up Match Register, offset: 0x0B08 */
    __IO uint32_t FLT_ID1;                                                    /**< Pretended Networking ID Filter 1 Register, offset: 0x0B0C */
    __IO uint32_t FLT_DLC;                                                    /**< Pretended Networking DLC Filter Register, offset: 0x0B10 */
    __IO uint32_t PL1_LO;                                                     /**< Pretended Networking Payload Low Filter 1 Register, offset: 0x0B14 */
    __IO uint32_t PL1_HI;                                                     /**< Pretended Networking Payload High Filter 1 Register, offset: 0x0B18 */
    __IO uint32_t FLT_ID2_IDMASK;                                             /**< Pretended Networking ID Filter 2 Register, offset: 0x0B1C */
    __IO uint32_t PL2_PLMASK_LO;                                              /**< Pretended Networking Payload Low Filter 2 Register, offset: 0x0B20 */
    __IO uint32_t PL2_PLMASK_HI;                                              /**< Pretended Networking Payload High Filter 2 Register, offset: 0x0B24 */
    __IO uint8_t  RESERVED48[24];                                             /**< RESERVED48, offset: 0x0B28 */
    struct {
        __IO uint32_t WMBn_CS;                                                /**< Wakeup Message Buffer Control Status Register, offset: 0x0B40 */
        __IO uint32_t WMBn_ID;                                                /**< Wakeup Message Buffer ID, offset: 0x0B44 */
        __IO uint32_t WMBn_D03;                                               /**< Wakeup Message Buffer Data Byte 0~3, offset: 0x0B48 */
        __IO uint32_t WMBn_D47;                                               /**< Wakeup Message Buffer Data Byte 4~7, offset: 0x0B4C */

    } WMB[4];
    __IO uint8_t  RESERVED50[112];                                            /**< RESERVED50, offset: 0x0B80 */
    __IO uint32_t EPRS;                                                       /**< Enhanced CAN Bit Timing Prescalers Register, offset: 0x0BF0 */
    __IO uint32_t ENCBT;                                                      /**< Enhanced Nominal CAN Bit Timing Register, offset: 0x0BF4 */
    __IO uint32_t EDCBT;                                                      /**< Enhanced Data Phase CAN Bit Timing Register, offset: 0x0BF8 */
    __IO uint32_t ETDC;                                                       /**< Enhanced Transceiver Delay Compensation Register, offset: 0x0BFC */
    __IO uint32_t FDCTRL;                                                     /**< FD Control Register, offset: 0x0C00 */
    __IO uint32_t FDCBT;                                                      /**< FD Bit Timing Register, offset: 0x0C04 */
    __IO uint32_t FDCRC;                                                      /**< FD CRC Register, offset: 0x0C08 */
    __IO uint32_t ERFCR;                                                      /**< Enhanced Rx FIFO Control Register, offset: 0x0C0C */
    __IO uint32_t ERFIER;                                                     /**< Enhanced Rx FIFO Interrupt Enable Register, offset: 0x0C10 */
    __IO uint32_t ERFSR;                                                      /**< Enhanced Rx FIFO Status Register, offset: 0x0C14 */
    __IO uint32_t RESERVED61[ 6];                                             /**< RESERVED61, offset: 0x0C18 */
    __IO uint32_t HR_TIME_STAMPn[128];                                        /**< High Resolution Time Stamp Registers, offset: 0x0C30 */
    __IO uint32_t RESERVED63[2164];                                           /**< RESERVED63, offset: 0x0E30 */
    __IO uint32_t ERFFELn[128];                                               /**< Enhanced Rx FIFO Filter Element Registers, offset: 0x3000 */

} CAN_Type, *CAN_MemMapPtr;

/** Number of instances of the CAN module. */
#define CAN_INSTANCE_COUNT                                                    (2U)

/* CAN0  base address */
#define CAN0_BASE                                                             (0x40030000U)
#define CAN0                                                                  ((CAN_Type *)(CAN0_BASE))
/* CAN1  base address */
#define CAN1_BASE                                                             (0x40034000U)
#define CAN1                                                                  ((CAN_Type *)(CAN1_BASE))

/** Array initializer of CAN peripheral base addresses */
#define CAN_BASE_ADDRS                                                        { CAN0_BASE,  CAN1_BASE }
/** Array initializer of CAN peripheral base pointers */
#define CAN_BASE_PTRS                                                         { CAN0,  CAN1 }
/** Number of interrupt channels for the Rx Waring IRQS type of CAN. */
#define CAN_RX_Warning_IRQS_CH_COUNT                                          (2U)
#define CAN_RX_Warning_IRQS                                                   { CAN0_ORed_IRQn, CAN1_ORed_IRQn }
/** Number of interrupt channels for the Tx Waring IRQS type of CAN. */
#define CAN_TX_Warning_IRQS_CH_COUNT                                          (2U)
#define CAN_TX_Warning_IRQS                                                   { CAN0_ORed_IRQn, CAN1_ORed_IRQn }
/** Number of interrupt channels for the Error IRQS type of CAN. */
#define CAN_Error_IRQS_CH_COUNT                                               (2U)
#define CAN_Error_IRQS                                                        { CAN0_Error_IRQn, CAN1_Error_IRQn }
/** Number of interrupt channels for the Bus Off IRQS type of CAN. */
#define CAN_Bus_Off_IRQS_CH_COUNT                                             (2U)
#define CAN_Bus_Off_IRQS                                                      { CAN0_ORed_IRQn, CAN1_ORed_IRQn }
/** Number of interrupt channels for the Wake Up IRQS type of CAN. */
#define CAN_Wake_Up_IRQS_CH_COUNT                                             (2U)
#define CAN_Wake_Up_IRQS                                                      { CAN0_Wake_Up_IRQn, CAN1_Wake_Up_IRQn }
/** Number of interrupt channels for the Mailbox buffer 0 - 15 type of CAN. */
#define CAN_ORed_0_15_MB_IRQS_CH_COUNT                                        (2U)
#define CAN_ORed_0_15_MB_IRQS                                                 { CAN0_ORed_0_15_MB_IRQn, CAN1_ORed_0_15_MB_IRQn }
/** Number of interrupt channels for the Mailbox buffer 16 - 31 type of CAN. */
#define CAN_ORed_16_31_MB_IRQS_CH_COUNT                                       (2U)
#define CAN_ORed_16_31_MB_IRQS                                                { CAN0_ORed_16_31_MB_IRQn, CAN1_ORed_16_31_MB_IRQn }
/** Number of interrupt channels for the Mailbox buffer 32 - 47 type of CAN. */
#define CAN_ORed_32_47_MB_IRQS_CH_COUNT                                       (2U)
#define CAN_ORed_32_47_MB_IRQS                                                { CAN0_ORed_32_47_MB_IRQn, CAN1_ORed_32_47_MB_IRQn }
/** Number of interrupt channels for the Mailbox buffer 48 - 64 type of CAN. */
#define CAN_ORed_48_63_MB_IRQS_CH_COUNT                                       (2U)
#define CAN_ORed_48_63_MB_IRQS                                                { CAN0_ORed_48_63_MB_IRQn, CAN1_ORed_48_63_MB_IRQn }

/* CAN Register Mask */
/* CAN_MCR Register */
#define CAN_MCR_MDIS_MASK                                                     (0x80000000U)
#define CAN_MCR_MDIS_SHIFT                                                    (31U)
#define CAN_MCR_MDIS(x)                                                       (((uint32_t)(((uint32_t)(x)) << CAN_MCR_MDIS_SHIFT)) & CAN_MCR_MDIS_MASK)
#define CAN_MCR_FRZ_MASK                                                      (0x40000000U)
#define CAN_MCR_FRZ_SHIFT                                                     (30U)
#define CAN_MCR_FRZ(x)                                                        (((uint32_t)(((uint32_t)(x)) << CAN_MCR_FRZ_SHIFT)) & CAN_MCR_FRZ_MASK)
#define CAN_MCR_RFEN_MASK                                                     (0x20000000U)
#define CAN_MCR_RFEN_SHIFT                                                    (29U)
#define CAN_MCR_RFEN(x)                                                       (((uint32_t)(((uint32_t)(x)) << CAN_MCR_RFEN_SHIFT)) & CAN_MCR_RFEN_MASK)
#define CAN_MCR_HALT_MASK                                                     (0x10000000U)
#define CAN_MCR_HALT_SHIFT                                                    (28U)
#define CAN_MCR_HALT(x)                                                       (((uint32_t)(((uint32_t)(x)) << CAN_MCR_HALT_SHIFT)) & CAN_MCR_HALT_MASK)
#define CAN_MCR_NOTRDY_MASK                                                   (0x8000000U)
#define CAN_MCR_NOTRDY_SHIFT                                                  (27U)
#define CAN_MCR_NOTRDY(x)                                                     (((uint32_t)(((uint32_t)(x)) << CAN_MCR_NOTRDY_SHIFT)) & CAN_MCR_NOTRDY_MASK)
#define CAN_MCR_WAKMSK_MASK                                                   (0x4000000U)
#define CAN_MCR_WAKMSK_SHIFT                                                  (26U)
#define CAN_MCR_WAKMSK(x)                                                     (((uint32_t)(((uint32_t)(x)) << CAN_MCR_WAKMSK_SHIFT)) & CAN_MCR_WAKMSK_MASK)
#define CAN_MCR_SOFTRST_MASK                                                  (0x2000000U)
#define CAN_MCR_SOFTRST_SHIFT                                                 (25U)
#define CAN_MCR_SOFTRST(x)                                                    (((uint32_t)(((uint32_t)(x)) << CAN_MCR_SOFTRST_SHIFT)) & CAN_MCR_SOFTRST_MASK)
#define CAN_MCR_FRZACK_MASK                                                   (0x1000000U)
#define CAN_MCR_FRZACK_SHIFT                                                  (24U)
#define CAN_MCR_FRZACK(x)                                                     (((uint32_t)(((uint32_t)(x)) << CAN_MCR_FRZACK_SHIFT)) & CAN_MCR_FRZACK_MASK)
#define CAN_MCR_SUPV_MASK                                                     (0x800000U)
#define CAN_MCR_SUPV_SHIFT                                                    (23U)
#define CAN_MCR_SUPV(x)                                                       (((uint32_t)(((uint32_t)(x)) << CAN_MCR_SUPV_SHIFT)) & CAN_MCR_SUPV_MASK)
#define CAN_MCR_SLFWAK_MASK                                                   (0x400000U)
#define CAN_MCR_SLFWAK_SHIFT                                                  (22U)
#define CAN_MCR_SLFWAK(x)                                                     (((uint32_t)(((uint32_t)(x)) << CAN_MCR_SLFWAK_SHIFT)) & CAN_MCR_SLFWAK_MASK)
#define CAN_MCR_WRNEN_MASK                                                    (0x200000U)
#define CAN_MCR_WRNEN_SHIFT                                                   (21U)
#define CAN_MCR_WRNEN(x)                                                      (((uint32_t)(((uint32_t)(x)) << CAN_MCR_WRNEN_SHIFT)) & CAN_MCR_WRNEN_MASK)
#define CAN_MCR_LPMACK_MASK                                                   (0x100000U)
#define CAN_MCR_LPMACK_SHIFT                                                  (20U)
#define CAN_MCR_LPMACK(x)                                                     (((uint32_t)(((uint32_t)(x)) << CAN_MCR_LPMACK_SHIFT)) & CAN_MCR_LPMACK_MASK)
#define CAN_MCR_WAKSRC_MASK                                                   (0x80000U)
#define CAN_MCR_WAKSRC_SHIFT                                                  (19U)
#define CAN_MCR_WAKSRC(x)                                                     (((uint32_t)(((uint32_t)(x)) << CAN_MCR_WAKSRC_SHIFT)) & CAN_MCR_WAKSRC_MASK)
#define CAN_MCR_DOZE_MASK                                                     (0x40000U)
#define CAN_MCR_DOZE_SHIFT                                                    (18U)
#define CAN_MCR_DOZE(x)                                                       (((uint32_t)(((uint32_t)(x)) << CAN_MCR_DOZE_SHIFT)) & CAN_MCR_DOZE_MASK)
#define CAN_MCR_SRXDIS_MASK                                                   (0x20000U)
#define CAN_MCR_SRXDIS_SHIFT                                                  (17U)
#define CAN_MCR_SRXDIS(x)                                                     (((uint32_t)(((uint32_t)(x)) << CAN_MCR_SRXDIS_SHIFT)) & CAN_MCR_SRXDIS_MASK)
#define CAN_MCR_IRMQ_MASK                                                     (0x10000U)
#define CAN_MCR_IRMQ_SHIFT                                                    (16U)
#define CAN_MCR_IRMQ(x)                                                       (((uint32_t)(((uint32_t)(x)) << CAN_MCR_IRMQ_SHIFT)) & CAN_MCR_IRMQ_MASK)
#define CAN_MCR_DMA_MASK                                                      (0x8000U)
#define CAN_MCR_DMA_SHIFT                                                     (15U)
#define CAN_MCR_DMA(x)                                                        (((uint32_t)(((uint32_t)(x)) << CAN_MCR_DMA_SHIFT)) & CAN_MCR_DMA_MASK)
#define CAN_MCR_PNET_EN_MASK                                                  (0x4000U)
#define CAN_MCR_PNET_EN_SHIFT                                                 (14U)
#define CAN_MCR_PNET_EN(x)                                                    (((uint32_t)(((uint32_t)(x)) << CAN_MCR_PNET_EN_SHIFT)) & CAN_MCR_PNET_EN_MASK)
#define CAN_MCR_LPRIOEN_MASK                                                  (0x2000U)
#define CAN_MCR_LPRIOEN_SHIFT                                                 (13U)
#define CAN_MCR_LPRIOEN(x)                                                    (((uint32_t)(((uint32_t)(x)) << CAN_MCR_LPRIOEN_SHIFT)) & CAN_MCR_LPRIOEN_MASK)
#define CAN_MCR_AEN_MASK                                                      (0x1000U)
#define CAN_MCR_AEN_SHIFT                                                     (12U)
#define CAN_MCR_AEN(x)                                                        (((uint32_t)(((uint32_t)(x)) << CAN_MCR_AEN_SHIFT)) & CAN_MCR_AEN_MASK)
#define CAN_MCR_FDEN_MASK                                                     (0x800U)
#define CAN_MCR_FDEN_SHIFT                                                    (11U)
#define CAN_MCR_FDEN(x)                                                       (((uint32_t)(((uint32_t)(x)) << CAN_MCR_FDEN_SHIFT)) & CAN_MCR_FDEN_MASK)
#define CAN_MCR_IDAM_MASK                                                     (0x300U)
#define CAN_MCR_IDAM_SHIFT                                                    (8U)
#define CAN_MCR_IDAM(x)                                                       (((uint32_t)(((uint32_t)(x)) << CAN_MCR_IDAM_SHIFT)) & CAN_MCR_IDAM_MASK)
#define CAN_MCR_MAXMB_MASK                                                    (0x7FU)
#define CAN_MCR_MAXMB_SHIFT                                                   (0U)
#define CAN_MCR_MAXMB(x)                                                      (((uint32_t)(((uint32_t)(x)) << CAN_MCR_MAXMB_SHIFT)) & CAN_MCR_MAXMB_MASK)
/* CAN_CTRL1 Register */
#define CAN_CTRL1_PRESDIV_MASK                                                (0xFF000000U)
#define CAN_CTRL1_PRESDIV_SHIFT                                               (24U)
#define CAN_CTRL1_PRESDIV(x)                                                  (((uint32_t)(((uint32_t)(x)) << CAN_CTRL1_PRESDIV_SHIFT)) & CAN_CTRL1_PRESDIV_MASK)
#define CAN_CTRL1_RJW_MASK                                                    (0xC00000U)
#define CAN_CTRL1_RJW_SHIFT                                                   (22U)
#define CAN_CTRL1_RJW(x)                                                      (((uint32_t)(((uint32_t)(x)) << CAN_CTRL1_RJW_SHIFT)) & CAN_CTRL1_RJW_MASK)
#define CAN_CTRL1_PSEG1_MASK                                                  (0x380000U)
#define CAN_CTRL1_PSEG1_SHIFT                                                 (19U)
#define CAN_CTRL1_PSEG1(x)                                                    (((uint32_t)(((uint32_t)(x)) << CAN_CTRL1_PSEG1_SHIFT)) & CAN_CTRL1_PSEG1_MASK)
#define CAN_CTRL1_PSEG2_MASK                                                  (0x70000U)
#define CAN_CTRL1_PSEG2_SHIFT                                                 (16U)
#define CAN_CTRL1_PSEG2(x)                                                    (((uint32_t)(((uint32_t)(x)) << CAN_CTRL1_PSEG2_SHIFT)) & CAN_CTRL1_PSEG2_MASK)
#define CAN_CTRL1_BOFFMSK_MASK                                                (0x8000U)
#define CAN_CTRL1_BOFFMSK_SHIFT                                               (15U)
#define CAN_CTRL1_BOFFMSK(x)                                                  (((uint32_t)(((uint32_t)(x)) << CAN_CTRL1_BOFFMSK_SHIFT)) & CAN_CTRL1_BOFFMSK_MASK)
#define CAN_CTRL1_ERRMSK_MASK                                                 (0x4000U)
#define CAN_CTRL1_ERRMSK_SHIFT                                                (14U)
#define CAN_CTRL1_ERRMSK(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_CTRL1_ERRMSK_SHIFT)) & CAN_CTRL1_ERRMSK_MASK)
#define CAN_CTRL1_CLKSRC_MASK                                                 (0x2000U)
#define CAN_CTRL1_CLKSRC_SHIFT                                                (13U)
#define CAN_CTRL1_CLKSRC(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_CTRL1_CLKSRC_SHIFT)) & CAN_CTRL1_CLKSRC_MASK)
#define CAN_CTRL1_LPB_MASK                                                    (0x1000U)
#define CAN_CTRL1_LPB_SHIFT                                                   (12U)
#define CAN_CTRL1_LPB(x)                                                      (((uint32_t)(((uint32_t)(x)) << CAN_CTRL1_LPB_SHIFT)) & CAN_CTRL1_LPB_MASK)
#define CAN_CTRL1_TWRNMSK_MASK                                                (0x800U)
#define CAN_CTRL1_TWRNMSK_SHIFT                                               (11U)
#define CAN_CTRL1_TWRNMSK(x)                                                  (((uint32_t)(((uint32_t)(x)) << CAN_CTRL1_TWRNMSK_SHIFT)) & CAN_CTRL1_TWRNMSK_MASK)
#define CAN_CTRL1_RWRNMSK_MASK                                                (0x400U)
#define CAN_CTRL1_RWRNMSK_SHIFT                                               (10U)
#define CAN_CTRL1_RWRNMSK(x)                                                  (((uint32_t)(((uint32_t)(x)) << CAN_CTRL1_RWRNMSK_SHIFT)) & CAN_CTRL1_RWRNMSK_MASK)
#define CAN_CTRL1_SMP_MASK                                                    (0x80U)
#define CAN_CTRL1_SMP_SHIFT                                                   (7U)
#define CAN_CTRL1_SMP(x)                                                      (((uint32_t)(((uint32_t)(x)) << CAN_CTRL1_SMP_SHIFT)) & CAN_CTRL1_SMP_MASK)
#define CAN_CTRL1_BOFFREC_MASK                                                (0x40U)
#define CAN_CTRL1_BOFFREC_SHIFT                                               (6U)
#define CAN_CTRL1_BOFFREC(x)                                                  (((uint32_t)(((uint32_t)(x)) << CAN_CTRL1_BOFFREC_SHIFT)) & CAN_CTRL1_BOFFREC_MASK)
#define CAN_CTRL1_TSYN_MASK                                                   (0x20U)
#define CAN_CTRL1_TSYN_SHIFT                                                  (5U)
#define CAN_CTRL1_TSYN(x)                                                     (((uint32_t)(((uint32_t)(x)) << CAN_CTRL1_TSYN_SHIFT)) & CAN_CTRL1_TSYN_MASK)
#define CAN_CTRL1_LBUF_MASK                                                   (0x10U)
#define CAN_CTRL1_LBUF_SHIFT                                                  (4U)
#define CAN_CTRL1_LBUF(x)                                                     (((uint32_t)(((uint32_t)(x)) << CAN_CTRL1_LBUF_SHIFT)) & CAN_CTRL1_LBUF_MASK)
#define CAN_CTRL1_LOM_MASK                                                    (0x8U)
#define CAN_CTRL1_LOM_SHIFT                                                   (3U)
#define CAN_CTRL1_LOM(x)                                                      (((uint32_t)(((uint32_t)(x)) << CAN_CTRL1_LOM_SHIFT)) & CAN_CTRL1_LOM_MASK)
#define CAN_CTRL1_PROPSEG_MASK                                                (0x7U)
#define CAN_CTRL1_PROPSEG_SHIFT                                               (0U)
#define CAN_CTRL1_PROPSEG(x)                                                  (((uint32_t)(((uint32_t)(x)) << CAN_CTRL1_PROPSEG_SHIFT)) & CAN_CTRL1_PROPSEG_MASK)
/* CAN_TIMER Register */
#define CAN_TIMER_TIMER_MASK                                                  (0xFFFFU)
#define CAN_TIMER_TIMER_SHIFT                                                 (0U)
#define CAN_TIMER_TIMER(x)                                                    (((uint32_t)(((uint32_t)(x)) << CAN_TIMER_TIMER_SHIFT)) & CAN_TIMER_TIMER_MASK)
/* CAN_RXMGMASK Register */
#define CAN_RXMGMASK_MG_MASK                                                  (0xFFFFFFFFU)
#define CAN_RXMGMASK_MG_SHIFT                                                 (0U)
#define CAN_RXMGMASK_MG(x)                                                    (((uint32_t)(((uint32_t)(x)) << CAN_RXMGMASK_MG_SHIFT)) & CAN_RXMGMASK_MG_MASK)
/* CAN_RX14MASK Register */
#define CAN_RX14MASK_RX14M_MASK                                               (0xFFFFFFFFU)
#define CAN_RX14MASK_RX14M_SHIFT                                              (0U)
#define CAN_RX14MASK_RX14M(x)                                                 (((uint32_t)(((uint32_t)(x)) << CAN_RX14MASK_RX14M_SHIFT)) & CAN_RX14MASK_RX14M_MASK)
/* CAN_RX15MASK Register */
#define CAN_RX15MASK_RX15M_MASK                                               (0xFFFFFFFFU)
#define CAN_RX15MASK_RX15M_SHIFT                                              (0U)
#define CAN_RX15MASK_RX15M(x)                                                 (((uint32_t)(((uint32_t)(x)) << CAN_RX15MASK_RX15M_SHIFT)) & CAN_RX15MASK_RX15M_MASK)
/* CAN_ECR Register */
#define CAN_ECR_RXERRCNT_FAST_MASK                                            (0xFF000000U)
#define CAN_ECR_RXERRCNT_FAST_SHIFT                                           (24U)
#define CAN_ECR_RXERRCNT_FAST(x)                                              (((uint32_t)(((uint32_t)(x)) << CAN_ECR_RXERRCNT_FAST_SHIFT)) & CAN_ECR_RXERRCNT_FAST_MASK)
#define CAN_ECR_TXERRCNT_FAST_MASK                                            (0xFF0000U)
#define CAN_ECR_TXERRCNT_FAST_SHIFT                                           (16U)
#define CAN_ECR_TXERRCNT_FAST(x)                                              (((uint32_t)(((uint32_t)(x)) << CAN_ECR_TXERRCNT_FAST_SHIFT)) & CAN_ECR_TXERRCNT_FAST_MASK)
#define CAN_ECR_RXERRCNT_MASK                                                 (0xFF00U)
#define CAN_ECR_RXERRCNT_SHIFT                                                (8U)
#define CAN_ECR_RXERRCNT(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_ECR_RXERRCNT_SHIFT)) & CAN_ECR_RXERRCNT_MASK)
#define CAN_ECR_TXERRCNT_MASK                                                 (0xFFU)
#define CAN_ECR_TXERRCNT_SHIFT                                                (0U)
#define CAN_ECR_TXERRCNT(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_ECR_TXERRCNT_SHIFT)) & CAN_ECR_TXERRCNT_MASK)
/* CAN_ESR1 Register */
#define CAN_ESR1_BIT1ERR_FAST_MASK                                            (0x80000000U)
#define CAN_ESR1_BIT1ERR_FAST_SHIFT                                           (31U)
#define CAN_ESR1_BIT1ERR_FAST(x)                                              (((uint32_t)(((uint32_t)(x)) << CAN_ESR1_BIT1ERR_FAST_SHIFT)) & CAN_ESR1_BIT1ERR_FAST_MASK)
#define CAN_ESR1_BIT0ERR_FAST_MASK                                            (0x40000000U)
#define CAN_ESR1_BIT0ERR_FAST_SHIFT                                           (30U)
#define CAN_ESR1_BIT0ERR_FAST(x)                                              (((uint32_t)(((uint32_t)(x)) << CAN_ESR1_BIT0ERR_FAST_SHIFT)) & CAN_ESR1_BIT0ERR_FAST_MASK)
#define CAN_ESR1_CRCERR_FAST_MASK                                             (0x10000000U)
#define CAN_ESR1_CRCERR_FAST_SHIFT                                            (28U)
#define CAN_ESR1_CRCERR_FAST(x)                                               (((uint32_t)(((uint32_t)(x)) << CAN_ESR1_CRCERR_FAST_SHIFT)) & CAN_ESR1_CRCERR_FAST_MASK)
#define CAN_ESR1_FRMERR_FAST_MASK                                             (0x8000000U)
#define CAN_ESR1_FRMERR_FAST_SHIFT                                            (27U)
#define CAN_ESR1_FRMERR_FAST(x)                                               (((uint32_t)(((uint32_t)(x)) << CAN_ESR1_FRMERR_FAST_SHIFT)) & CAN_ESR1_FRMERR_FAST_MASK)
#define CAN_ESR1_STFERR_FAST_MASK                                             (0x4000000U)
#define CAN_ESR1_STFERR_FAST_SHIFT                                            (26U)
#define CAN_ESR1_STFERR_FAST(x)                                               (((uint32_t)(((uint32_t)(x)) << CAN_ESR1_STFERR_FAST_SHIFT)) & CAN_ESR1_STFERR_FAST_MASK)
#define CAN_ESR1_ERROVR_MASK                                                  (0x200000U)
#define CAN_ESR1_ERROVR_SHIFT                                                 (21U)
#define CAN_ESR1_ERROVR(x)                                                    (((uint32_t)(((uint32_t)(x)) << CAN_ESR1_ERROVR_SHIFT)) & CAN_ESR1_ERROVR_MASK)
#define CAN_ESR1_ERRINT_FAST_MASK                                             (0x100000U)
#define CAN_ESR1_ERRINT_FAST_SHIFT                                            (20U)
#define CAN_ESR1_ERRINT_FAST(x)                                               (((uint32_t)(((uint32_t)(x)) << CAN_ESR1_ERRINT_FAST_SHIFT)) & CAN_ESR1_ERRINT_FAST_MASK)
#define CAN_ESR1_BOFFDONEINT_MASK                                             (0x80000U)
#define CAN_ESR1_BOFFDONEINT_SHIFT                                            (19U)
#define CAN_ESR1_BOFFDONEINT(x)                                               (((uint32_t)(((uint32_t)(x)) << CAN_ESR1_BOFFDONEINT_SHIFT)) & CAN_ESR1_BOFFDONEINT_MASK)
#define CAN_ESR1_SYNCH_MASK                                                   (0x40000U)
#define CAN_ESR1_SYNCH_SHIFT                                                  (18U)
#define CAN_ESR1_SYNCH(x)                                                     (((uint32_t)(((uint32_t)(x)) << CAN_ESR1_SYNCH_SHIFT)) & CAN_ESR1_SYNCH_MASK)
#define CAN_ESR1_TWRNINT_MASK                                                 (0x20000U)
#define CAN_ESR1_TWRNINT_SHIFT                                                (17U)
#define CAN_ESR1_TWRNINT(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_ESR1_TWRNINT_SHIFT)) & CAN_ESR1_TWRNINT_MASK)
#define CAN_ESR1_RWRNINT_MASK                                                 (0x10000U)
#define CAN_ESR1_RWRNINT_SHIFT                                                (16U)
#define CAN_ESR1_RWRNINT(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_ESR1_RWRNINT_SHIFT)) & CAN_ESR1_RWRNINT_MASK)
#define CAN_ESR1_BIT1ERR_MASK                                                 (0x8000U)
#define CAN_ESR1_BIT1ERR_SHIFT                                                (15U)
#define CAN_ESR1_BIT1ERR(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_ESR1_BIT1ERR_SHIFT)) & CAN_ESR1_BIT1ERR_MASK)
#define CAN_ESR1_BIT0ERR_MASK                                                 (0x4000U)
#define CAN_ESR1_BIT0ERR_SHIFT                                                (14U)
#define CAN_ESR1_BIT0ERR(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_ESR1_BIT0ERR_SHIFT)) & CAN_ESR1_BIT0ERR_MASK)
#define CAN_ESR1_ACKERR_MASK                                                  (0x2000U)
#define CAN_ESR1_ACKERR_SHIFT                                                 (13U)
#define CAN_ESR1_ACKERR(x)                                                    (((uint32_t)(((uint32_t)(x)) << CAN_ESR1_ACKERR_SHIFT)) & CAN_ESR1_ACKERR_MASK)
#define CAN_ESR1_CRCERR_MASK                                                  (0x1000U)
#define CAN_ESR1_CRCERR_SHIFT                                                 (12U)
#define CAN_ESR1_CRCERR(x)                                                    (((uint32_t)(((uint32_t)(x)) << CAN_ESR1_CRCERR_SHIFT)) & CAN_ESR1_CRCERR_MASK)
#define CAN_ESR1_FRMERR_MASK                                                  (0x800U)
#define CAN_ESR1_FRMERR_SHIFT                                                 (11U)
#define CAN_ESR1_FRMERR(x)                                                    (((uint32_t)(((uint32_t)(x)) << CAN_ESR1_FRMERR_SHIFT)) & CAN_ESR1_FRMERR_MASK)
#define CAN_ESR1_STFERR_MASK                                                  (0x400U)
#define CAN_ESR1_STFERR_SHIFT                                                 (10U)
#define CAN_ESR1_STFERR(x)                                                    (((uint32_t)(((uint32_t)(x)) << CAN_ESR1_STFERR_SHIFT)) & CAN_ESR1_STFERR_MASK)
#define CAN_ESR1_TXWRN_MASK                                                   (0x200U)
#define CAN_ESR1_TXWRN_SHIFT                                                  (9U)
#define CAN_ESR1_TXWRN(x)                                                     (((uint32_t)(((uint32_t)(x)) << CAN_ESR1_TXWRN_SHIFT)) & CAN_ESR1_TXWRN_MASK)
#define CAN_ESR1_RXWRN_MASK                                                   (0x100U)
#define CAN_ESR1_RXWRN_SHIFT                                                  (8U)
#define CAN_ESR1_RXWRN(x)                                                     (((uint32_t)(((uint32_t)(x)) << CAN_ESR1_RXWRN_SHIFT)) & CAN_ESR1_RXWRN_MASK)
#define CAN_ESR1_IDLE_MASK                                                    (0x80U)
#define CAN_ESR1_IDLE_SHIFT                                                   (7U)
#define CAN_ESR1_IDLE(x)                                                      (((uint32_t)(((uint32_t)(x)) << CAN_ESR1_IDLE_SHIFT)) & CAN_ESR1_IDLE_MASK)
#define CAN_ESR1_TX_MASK                                                      (0x40U)
#define CAN_ESR1_TX_SHIFT                                                     (6U)
#define CAN_ESR1_TX(x)                                                        (((uint32_t)(((uint32_t)(x)) << CAN_ESR1_TX_SHIFT)) & CAN_ESR1_TX_MASK)
#define CAN_ESR1_FLTCONF_MASK                                                 (0x30U)
#define CAN_ESR1_FLTCONF_SHIFT                                                (4U)
#define CAN_ESR1_FLTCONF(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_ESR1_FLTCONF_SHIFT)) & CAN_ESR1_FLTCONF_MASK)
#define CAN_ESR1_RX_MASK                                                      (0x8U)
#define CAN_ESR1_RX_SHIFT                                                     (3U)
#define CAN_ESR1_RX(x)                                                        (((uint32_t)(((uint32_t)(x)) << CAN_ESR1_RX_SHIFT)) & CAN_ESR1_RX_MASK)
#define CAN_ESR1_BOFFINT_MASK                                                 (0x4U)
#define CAN_ESR1_BOFFINT_SHIFT                                                (2U)
#define CAN_ESR1_BOFFINT(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_ESR1_BOFFINT_SHIFT)) & CAN_ESR1_BOFFINT_MASK)
#define CAN_ESR1_ERRINT_MASK                                                  (0x2U)
#define CAN_ESR1_ERRINT_SHIFT                                                 (1U)
#define CAN_ESR1_ERRINT(x)                                                    (((uint32_t)(((uint32_t)(x)) << CAN_ESR1_ERRINT_SHIFT)) & CAN_ESR1_ERRINT_MASK)
#define CAN_ESR1_WAKINT_MASK                                                  (0x1U)
#define CAN_ESR1_WAKINT_SHIFT                                                 (0U)
#define CAN_ESR1_WAKINT(x)                                                    (((uint32_t)(((uint32_t)(x)) << CAN_ESR1_WAKINT_SHIFT)) & CAN_ESR1_WAKINT_MASK)
/* CAN_IMASK2 Register */
#define CAN_IMASK2_BUF63TO32M_MASK                                            (0xFFFFFFFFU)
#define CAN_IMASK2_BUF63TO32M_SHIFT                                           (0U)
#define CAN_IMASK2_BUF63TO32M(x)                                              (((uint32_t)(((uint32_t)(x)) << CAN_IMASK2_BUF63TO32M_SHIFT)) & CAN_IMASK2_BUF63TO32M_MASK)
/* CAN_IMASK1 Register */
#define CAN_IMASK1_BUF31TO0M_MASK                                             (0xFFFFFFFFU)
#define CAN_IMASK1_BUF31TO0M_SHIFT                                            (0U)
#define CAN_IMASK1_BUF31TO0M(x)                                               (((uint32_t)(((uint32_t)(x)) << CAN_IMASK1_BUF31TO0M_SHIFT)) & CAN_IMASK1_BUF31TO0M_MASK)
/* CAN_IFLAG2 Register */
#define CAN_IFLAG2_BUF63TO32I_MASK                                            (0xFFFFFFFFU)
#define CAN_IFLAG2_BUF63TO32I_SHIFT                                           (0U)
#define CAN_IFLAG2_BUF63TO32I(x)                                              (((uint32_t)(((uint32_t)(x)) << CAN_IFLAG2_BUF63TO32I_SHIFT)) & CAN_IFLAG2_BUF63TO32I_MASK)
/* CAN_IFLAG1 Register */
#define CAN_IFLAG1_BUF31TO8I_MASK                                             (0xFFFFFF00U)
#define CAN_IFLAG1_BUF31TO8I_SHIFT                                            (8U)
#define CAN_IFLAG1_BUF31TO8I(x)                                               (((uint32_t)(((uint32_t)(x)) << CAN_IFLAG1_BUF31TO8I_SHIFT)) & CAN_IFLAG1_BUF31TO8I_MASK)
#define CAN_IFLAG1_BUF7I_MASK                                                 (0x80U)
#define CAN_IFLAG1_BUF7I_SHIFT                                                (7U)
#define CAN_IFLAG1_BUF7I(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_IFLAG1_BUF7I_SHIFT)) & CAN_IFLAG1_BUF7I_MASK)
#define CAN_IFLAG1_BUF6I_MASK                                                 (0x40U)
#define CAN_IFLAG1_BUF6I_SHIFT                                                (6U)
#define CAN_IFLAG1_BUF6I(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_IFLAG1_BUF6I_SHIFT)) & CAN_IFLAG1_BUF6I_MASK)
#define CAN_IFLAG1_BUF5I_MASK                                                 (0x20U)
#define CAN_IFLAG1_BUF5I_SHIFT                                                (5U)
#define CAN_IFLAG1_BUF5I(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_IFLAG1_BUF5I_SHIFT)) & CAN_IFLAG1_BUF5I_MASK)
#define CAN_IFLAG1_BUF4TO1I_MASK                                              (0x1EU)
#define CAN_IFLAG1_BUF4TO1I_SHIFT                                             (1U)
#define CAN_IFLAG1_BUF4TO1I(x)                                                (((uint32_t)(((uint32_t)(x)) << CAN_IFLAG1_BUF4TO1I_SHIFT)) & CAN_IFLAG1_BUF4TO1I_MASK)
#define CAN_IFLAG1_BUF0I_MASK                                                 (0x1U)
#define CAN_IFLAG1_BUF0I_SHIFT                                                (0U)
#define CAN_IFLAG1_BUF0I(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_IFLAG1_BUF0I_SHIFT)) & CAN_IFLAG1_BUF0I_MASK)
/* CAN_CTRL2 Register */
#define CAN_CTRL2_ERRMSK_FAST_MASK                                            (0x80000000U)
#define CAN_CTRL2_ERRMSK_FAST_SHIFT                                           (31U)
#define CAN_CTRL2_ERRMSK_FAST(x)                                              (((uint32_t)(((uint32_t)(x)) << CAN_CTRL2_ERRMSK_FAST_SHIFT)) & CAN_CTRL2_ERRMSK_FAST_MASK)
#define CAN_CTRL2_BOFFDONEMSK_MASK                                            (0x40000000U)
#define CAN_CTRL2_BOFFDONEMSK_SHIFT                                           (30U)
#define CAN_CTRL2_BOFFDONEMSK(x)                                              (((uint32_t)(((uint32_t)(x)) << CAN_CTRL2_BOFFDONEMSK_SHIFT)) & CAN_CTRL2_BOFFDONEMSK_MASK)
#define CAN_CTRL2_ECRWRE_MASK                                                 (0x20000000U)
#define CAN_CTRL2_ECRWRE_SHIFT                                                (29U)
#define CAN_CTRL2_ECRWRE(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_CTRL2_ECRWRE_SHIFT)) & CAN_CTRL2_ECRWRE_MASK)
#define CAN_CTRL2_WRMFRZ_MASK                                                 (0x10000000U)
#define CAN_CTRL2_WRMFRZ_SHIFT                                                (28U)
#define CAN_CTRL2_WRMFRZ(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_CTRL2_WRMFRZ_SHIFT)) & CAN_CTRL2_WRMFRZ_MASK)
#define CAN_CTRL2_RFFN_MASK                                                   (0xF000000U)
#define CAN_CTRL2_RFFN_SHIFT                                                  (24U)
#define CAN_CTRL2_RFFN(x)                                                     (((uint32_t)(((uint32_t)(x)) << CAN_CTRL2_RFFN_SHIFT)) & CAN_CTRL2_RFFN_MASK)
#define CAN_CTRL2_TASD_MASK                                                   (0xF80000U)
#define CAN_CTRL2_TASD_SHIFT                                                  (19U)
#define CAN_CTRL2_TASD(x)                                                     (((uint32_t)(((uint32_t)(x)) << CAN_CTRL2_TASD_SHIFT)) & CAN_CTRL2_TASD_MASK)
#define CAN_CTRL2_MRP_MASK                                                    (0x40000U)
#define CAN_CTRL2_MRP_SHIFT                                                   (18U)
#define CAN_CTRL2_MRP(x)                                                      (((uint32_t)(((uint32_t)(x)) << CAN_CTRL2_MRP_SHIFT)) & CAN_CTRL2_MRP_MASK)
#define CAN_CTRL2_RRS_MASK                                                    (0x20000U)
#define CAN_CTRL2_RRS_SHIFT                                                   (17U)
#define CAN_CTRL2_RRS(x)                                                      (((uint32_t)(((uint32_t)(x)) << CAN_CTRL2_RRS_SHIFT)) & CAN_CTRL2_RRS_MASK)
#define CAN_CTRL2_EACEN_MASK                                                  (0x10000U)
#define CAN_CTRL2_EACEN_SHIFT                                                 (16U)
#define CAN_CTRL2_EACEN(x)                                                    (((uint32_t)(((uint32_t)(x)) << CAN_CTRL2_EACEN_SHIFT)) & CAN_CTRL2_EACEN_MASK)
#define CAN_CTRL2_TIMER_SRC_MASK                                              (0x8000U)
#define CAN_CTRL2_TIMER_SRC_SHIFT                                             (15U)
#define CAN_CTRL2_TIMER_SRC(x)                                                (((uint32_t)(((uint32_t)(x)) << CAN_CTRL2_TIMER_SRC_SHIFT)) & CAN_CTRL2_TIMER_SRC_MASK)
#define CAN_CTRL2_PREXCEN_MASK                                                (0x4000U)
#define CAN_CTRL2_PREXCEN_SHIFT                                               (14U)
#define CAN_CTRL2_PREXCEN(x)                                                  (((uint32_t)(((uint32_t)(x)) << CAN_CTRL2_PREXCEN_SHIFT)) & CAN_CTRL2_PREXCEN_MASK)
#define CAN_CTRL2_BTE_MASK                                                    (0x2000U)
#define CAN_CTRL2_BTE_SHIFT                                                   (13U)
#define CAN_CTRL2_BTE(x)                                                      (((uint32_t)(((uint32_t)(x)) << CAN_CTRL2_BTE_SHIFT)) & CAN_CTRL2_BTE_MASK)
#define CAN_CTRL2_ISOCANFDEN_MASK                                             (0x1000U)
#define CAN_CTRL2_ISOCANFDEN_SHIFT                                            (12U)
#define CAN_CTRL2_ISOCANFDEN(x)                                               (((uint32_t)(((uint32_t)(x)) << CAN_CTRL2_ISOCANFDEN_SHIFT)) & CAN_CTRL2_ISOCANFDEN_MASK)
#define CAN_CTRL2_EDFLTDIS_MASK                                               (0x800U)
#define CAN_CTRL2_EDFLTDIS_SHIFT                                              (11U)
#define CAN_CTRL2_EDFLTDIS(x)                                                 (((uint32_t)(((uint32_t)(x)) << CAN_CTRL2_EDFLTDIS_SHIFT)) & CAN_CTRL2_EDFLTDIS_MASK)
#define CAN_CTRL2_MBTSBASE_MASK                                               (0x300U)
#define CAN_CTRL2_MBTSBASE_SHIFT                                              (8U)
#define CAN_CTRL2_MBTSBASE(x)                                                 (((uint32_t)(((uint32_t)(x)) << CAN_CTRL2_MBTSBASE_SHIFT)) & CAN_CTRL2_MBTSBASE_MASK)
#define CAN_CTRL2_TSTAMPCAP_MASK                                              (0xC0U)
#define CAN_CTRL2_TSTAMPCAP_SHIFT                                             (6U)
#define CAN_CTRL2_TSTAMPCAP(x)                                                (((uint32_t)(((uint32_t)(x)) << CAN_CTRL2_TSTAMPCAP_SHIFT)) & CAN_CTRL2_TSTAMPCAP_MASK)
/* CAN_ESR2 Register */
#define CAN_ESR2_TWAINT_MASK                                                  (0x80000000U)
#define CAN_ESR2_TWAINT_SHIFT                                                 (31U)
#define CAN_ESR2_TWAINT(x)                                                    (((uint32_t)(((uint32_t)(x)) << CAN_ESR2_TWAINT_SHIFT)) & CAN_ESR2_TWAINT_MASK)
#define CAN_ESR2_LPTM_MASK                                                    (0x7F0000U)
#define CAN_ESR2_LPTM_SHIFT                                                   (16U)
#define CAN_ESR2_LPTM(x)                                                      (((uint32_t)(((uint32_t)(x)) << CAN_ESR2_LPTM_SHIFT)) & CAN_ESR2_LPTM_MASK)
#define CAN_ESR2_TWAMSK_MASK                                                  (0x8000U)
#define CAN_ESR2_TWAMSK_SHIFT                                                 (15U)
#define CAN_ESR2_TWAMSK(x)                                                    (((uint32_t)(((uint32_t)(x)) << CAN_ESR2_TWAMSK_SHIFT)) & CAN_ESR2_TWAMSK_MASK)
#define CAN_ESR2_VPS_MASK                                                     (0x4000U)
#define CAN_ESR2_VPS_SHIFT                                                    (14U)
#define CAN_ESR2_VPS(x)                                                       (((uint32_t)(((uint32_t)(x)) << CAN_ESR2_VPS_SHIFT)) & CAN_ESR2_VPS_MASK)
#define CAN_ESR2_IMB_MASK                                                     (0x2000U)
#define CAN_ESR2_IMB_SHIFT                                                    (13U)
#define CAN_ESR2_IMB(x)                                                       (((uint32_t)(((uint32_t)(x)) << CAN_ESR2_IMB_SHIFT)) & CAN_ESR2_IMB_MASK)
/* CAN_CRCR Register */
#define CAN_CRCR_MBCRC_MASK                                                   (0x7F0000U)
#define CAN_CRCR_MBCRC_SHIFT                                                  (16U)
#define CAN_CRCR_MBCRC(x)                                                     (((uint32_t)(((uint32_t)(x)) << CAN_CRCR_MBCRC_SHIFT)) & CAN_CRCR_MBCRC_MASK)
#define CAN_CRCR_TXCRC_MASK                                                   (0x7FFFU)
#define CAN_CRCR_TXCRC_SHIFT                                                  (0U)
#define CAN_CRCR_TXCRC(x)                                                     (((uint32_t)(((uint32_t)(x)) << CAN_CRCR_TXCRC_SHIFT)) & CAN_CRCR_TXCRC_MASK)
/* CAN_RXFGMASK Register */
#define CAN_RXFGMASK_FGM_MASK                                                 (0xFFFFFFFFU)
#define CAN_RXFGMASK_FGM_SHIFT                                                (0U)
#define CAN_RXFGMASK_FGM(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_RXFGMASK_FGM_SHIFT)) & CAN_RXFGMASK_FGM_MASK)
/* CAN_RXFIR Register */
#define CAN_RXFIR_IDHIT_MASK                                                  (0x1FFU)
#define CAN_RXFIR_IDHIT_SHIFT                                                 (0U)
#define CAN_RXFIR_IDHIT(x)                                                    (((uint32_t)(((uint32_t)(x)) << CAN_RXFIR_IDHIT_SHIFT)) & CAN_RXFIR_IDHIT_MASK)
/* CAN_CBT Register */
#define CAN_CBT_BTF_MASK                                                      (0x80000000U)
#define CAN_CBT_BTF_SHIFT                                                     (31U)
#define CAN_CBT_BTF(x)                                                        (((uint32_t)(((uint32_t)(x)) << CAN_CBT_BTF_SHIFT)) & CAN_CBT_BTF_MASK)
#define CAN_CBT_EPRESDIV_MASK                                                 (0x7FE00000U)
#define CAN_CBT_EPRESDIV_SHIFT                                                (21U)
#define CAN_CBT_EPRESDIV(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_CBT_EPRESDIV_SHIFT)) & CAN_CBT_EPRESDIV_MASK)
#define CAN_CBT_ERJW_MASK                                                     (0x1F0000U)
#define CAN_CBT_ERJW_SHIFT                                                    (16U)
#define CAN_CBT_ERJW(x)                                                       (((uint32_t)(((uint32_t)(x)) << CAN_CBT_ERJW_SHIFT)) & CAN_CBT_ERJW_MASK)
#define CAN_CBT_EPROPSEG_MASK                                                 (0xFC00U)
#define CAN_CBT_EPROPSEG_SHIFT                                                (10U)
#define CAN_CBT_EPROPSEG(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_CBT_EPROPSEG_SHIFT)) & CAN_CBT_EPROPSEG_MASK)
#define CAN_CBT_EPSEG1_MASK                                                   (0x3E0U)
#define CAN_CBT_EPSEG1_SHIFT                                                  (5U)
#define CAN_CBT_EPSEG1(x)                                                     (((uint32_t)(((uint32_t)(x)) << CAN_CBT_EPSEG1_SHIFT)) & CAN_CBT_EPSEG1_MASK)
#define CAN_CBT_EPSEG2_MASK                                                   (0x1FU)
#define CAN_CBT_EPSEG2_SHIFT                                                  (0U)
#define CAN_CBT_EPSEG2(x)                                                     (((uint32_t)(((uint32_t)(x)) << CAN_CBT_EPSEG2_SHIFT)) & CAN_CBT_EPSEG2_MASK)
/* CAN_IMASK4 Register */
#define CAN_IMASK4_BUF127TO96M_MASK                                           (0xFFFFFFFFU)
#define CAN_IMASK4_BUF127TO96M_SHIFT                                          (0U)
#define CAN_IMASK4_BUF127TO96M(x)                                             (((uint32_t)(((uint32_t)(x)) << CAN_IMASK4_BUF127TO96M_SHIFT)) & CAN_IMASK4_BUF127TO96M_MASK)
/* CAN_IMASK3 Register */
#define CAN_IMASK3_BUF95TO64M_MASK                                            (0xFFFFFFFFU)
#define CAN_IMASK3_BUF95TO64M_SHIFT                                           (0U)
#define CAN_IMASK3_BUF95TO64M(x)                                              (((uint32_t)(((uint32_t)(x)) << CAN_IMASK3_BUF95TO64M_SHIFT)) & CAN_IMASK3_BUF95TO64M_MASK)
/* CAN_IFLAG4 Register */
#define CAN_IFLAG4_BUF127TO96_MASK                                            (0xFFFFFFFFU)
#define CAN_IFLAG4_BUF127TO96_SHIFT                                           (0U)
#define CAN_IFLAG4_BUF127TO96(x)                                              (((uint32_t)(((uint32_t)(x)) << CAN_IFLAG4_BUF127TO96_SHIFT)) & CAN_IFLAG4_BUF127TO96_MASK)
/* CAN_IFLAG3 Register */
#define CAN_IFLAG3_BUF95TO64_MASK                                             (0xFFFFFFFFU)
#define CAN_IFLAG3_BUF95TO64_SHIFT                                            (0U)
#define CAN_IFLAG3_BUF95TO64(x)                                               (((uint32_t)(((uint32_t)(x)) << CAN_IFLAG3_BUF95TO64_SHIFT)) & CAN_IFLAG3_BUF95TO64_MASK)
/* CAN_RAM Register */
#define CAN_RAM_RAM_MASK                                                      (0xFFFFFFFFU)
#define CAN_RAM_RAM_SHIFT                                                     (0U)
#define CAN_RAM_RAM(x)                                                        (((uint32_t)(((uint32_t)(x)) << CAN_RAM_RAM_SHIFT)) & CAN_RAM_RAM_MASK)
/* CAN_RXIMR Register */
#define CAN_RXIMR_MI_MASK                                                     (0xFFFFFFFFU)
#define CAN_RXIMR_MI_SHIFT                                                    (0U)
#define CAN_RXIMR_MI(x)                                                       (((uint32_t)(((uint32_t)(x)) << CAN_RXIMR_MI_SHIFT)) & CAN_RXIMR_MI_MASK)
/* CAN_MECR Register */
#define CAN_MECR_ECRWRDIS_MASK                                                (0x80000000U)
#define CAN_MECR_ECRWRDIS_SHIFT                                               (31U)
#define CAN_MECR_ECRWRDIS(x)                                                  (((uint32_t)(((uint32_t)(x)) << CAN_MECR_ECRWRDIS_SHIFT)) & CAN_MECR_ECRWRDIS_MASK)
#define CAN_MECR_HANCEI_MSK_MASK                                              (0x80000U)
#define CAN_MECR_HANCEI_MSK_SHIFT                                             (19U)
#define CAN_MECR_HANCEI_MSK(x)                                                (((uint32_t)(((uint32_t)(x)) << CAN_MECR_HANCEI_MSK_SHIFT)) & CAN_MECR_HANCEI_MSK_MASK)
#define CAN_MECR_FANCEI_MSK_MASK                                              (0x40000U)
#define CAN_MECR_FANCEI_MSK_SHIFT                                             (18U)
#define CAN_MECR_FANCEI_MSK(x)                                                (((uint32_t)(((uint32_t)(x)) << CAN_MECR_FANCEI_MSK_SHIFT)) & CAN_MECR_FANCEI_MSK_MASK)
#define CAN_MECR_CEI_MSK_MASK                                                 (0x10000U)
#define CAN_MECR_CEI_MSK_SHIFT                                                (16U)
#define CAN_MECR_CEI_MSK(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_MECR_CEI_MSK_SHIFT)) & CAN_MECR_CEI_MSK_MASK)
#define CAN_MECR_HAERRIE_MASK                                                 (0x8000U)
#define CAN_MECR_HAERRIE_SHIFT                                                (15U)
#define CAN_MECR_HAERRIE(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_MECR_HAERRIE_SHIFT)) & CAN_MECR_HAERRIE_MASK)
#define CAN_MECR_FAERRIE_MASK                                                 (0x4000U)
#define CAN_MECR_FAERRIE_SHIFT                                                (14U)
#define CAN_MECR_FAERRIE(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_MECR_FAERRIE_SHIFT)) & CAN_MECR_FAERRIE_MASK)
#define CAN_MECR_EXTERRIE_MASK                                                (0x2000U)
#define CAN_MECR_EXTERRIE_SHIFT                                               (13U)
#define CAN_MECR_EXTERRIE(x)                                                  (((uint32_t)(((uint32_t)(x)) << CAN_MECR_EXTERRIE_SHIFT)) & CAN_MECR_EXTERRIE_MASK)
#define CAN_MECR_RERRDIS_MASK                                                 (0x200U)
#define CAN_MECR_RERRDIS_SHIFT                                                (9U)
#define CAN_MECR_RERRDIS(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_MECR_RERRDIS_SHIFT)) & CAN_MECR_RERRDIS_MASK)
#define CAN_MECR_ECCDIS_MASK                                                  (0x100U)
#define CAN_MECR_ECCDIS_SHIFT                                                 (8U)
#define CAN_MECR_ECCDIS(x)                                                    (((uint32_t)(((uint32_t)(x)) << CAN_MECR_ECCDIS_SHIFT)) & CAN_MECR_ECCDIS_MASK)
#define CAN_MECR_NCEFAFRZ_MASK                                                (0x80U)
#define CAN_MECR_NCEFAFRZ_SHIFT                                               (7U)
#define CAN_MECR_NCEFAFRZ(x)                                                  (((uint32_t)(((uint32_t)(x)) << CAN_MECR_NCEFAFRZ_SHIFT)) & CAN_MECR_NCEFAFRZ_MASK)
/* CAN_ERRIAR Register */
#define CAN_ERRIAR_INJADDR_H_MASK                                             (0x3FFCU)
#define CAN_ERRIAR_INJADDR_H_SHIFT                                            (2U)
#define CAN_ERRIAR_INJADDR_H(x)                                               (((uint32_t)(((uint32_t)(x)) << CAN_ERRIAR_INJADDR_H_SHIFT)) & CAN_ERRIAR_INJADDR_H_MASK)
#define CAN_ERRIAR_INJADDR_L_MASK                                             (0x3U)
#define CAN_ERRIAR_INJADDR_L_SHIFT                                            (0U)
#define CAN_ERRIAR_INJADDR_L(x)                                               (((uint32_t)(((uint32_t)(x)) << CAN_ERRIAR_INJADDR_L_SHIFT)) & CAN_ERRIAR_INJADDR_L_MASK)
/* CAN_ERRIDPR Register */
#define CAN_ERRIDPR_DFLIP_MASK                                                (0xFFFFFFFFU)
#define CAN_ERRIDPR_DFLIP_SHIFT                                               (0U)
#define CAN_ERRIDPR_DFLIP(x)                                                  (((uint32_t)(((uint32_t)(x)) << CAN_ERRIDPR_DFLIP_SHIFT)) & CAN_ERRIDPR_DFLIP_MASK)
/* CAN_ERRIPPR Register */
#define CAN_ERRIPPR_PFLIP3_MASK                                               (0x1F000000U)
#define CAN_ERRIPPR_PFLIP3_SHIFT                                              (24U)
#define CAN_ERRIPPR_PFLIP3(x)                                                 (((uint32_t)(((uint32_t)(x)) << CAN_ERRIPPR_PFLIP3_SHIFT)) & CAN_ERRIPPR_PFLIP3_MASK)
#define CAN_ERRIPPR_PFLIP2_MASK                                               (0x1F0000U)
#define CAN_ERRIPPR_PFLIP2_SHIFT                                              (16U)
#define CAN_ERRIPPR_PFLIP2(x)                                                 (((uint32_t)(((uint32_t)(x)) << CAN_ERRIPPR_PFLIP2_SHIFT)) & CAN_ERRIPPR_PFLIP2_MASK)
#define CAN_ERRIPPR_PFLIP1_MASK                                               (0x1F00U)
#define CAN_ERRIPPR_PFLIP1_SHIFT                                              (8U)
#define CAN_ERRIPPR_PFLIP1(x)                                                 (((uint32_t)(((uint32_t)(x)) << CAN_ERRIPPR_PFLIP1_SHIFT)) & CAN_ERRIPPR_PFLIP1_MASK)
#define CAN_ERRIPPR_PFLIP0_MASK                                               (0x1FU)
#define CAN_ERRIPPR_PFLIP0_SHIFT                                              (0U)
#define CAN_ERRIPPR_PFLIP0(x)                                                 (((uint32_t)(((uint32_t)(x)) << CAN_ERRIPPR_PFLIP0_SHIFT)) & CAN_ERRIPPR_PFLIP0_MASK)
/* CAN_RERRAR Register */
#define CAN_RERRAR_NCE_MASK                                                   (0x1000000U)
#define CAN_RERRAR_NCE_SHIFT                                                  (24U)
#define CAN_RERRAR_NCE(x)                                                     (((uint32_t)(((uint32_t)(x)) << CAN_RERRAR_NCE_SHIFT)) & CAN_RERRAR_NCE_MASK)
#define CAN_RERRAR_SAID_MASK                                                  (0x70000U)
#define CAN_RERRAR_SAID_SHIFT                                                 (16U)
#define CAN_RERRAR_SAID(x)                                                    (((uint32_t)(((uint32_t)(x)) << CAN_RERRAR_SAID_SHIFT)) & CAN_RERRAR_SAID_MASK)
#define CAN_RERRAR_ERRADDR_MASK                                               (0x3FFFU)
#define CAN_RERRAR_ERRADDR_SHIFT                                              (0U)
#define CAN_RERRAR_ERRADDR(x)                                                 (((uint32_t)(((uint32_t)(x)) << CAN_RERRAR_ERRADDR_SHIFT)) & CAN_RERRAR_ERRADDR_MASK)
/* CAN_RERRDR Register */
#define CAN_RERRDR_RDATA_MASK                                                 (0xFFFFFFFFU)
#define CAN_RERRDR_RDATA_SHIFT                                                (0U)
#define CAN_RERRDR_RDATA(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_RERRDR_RDATA_SHIFT)) & CAN_RERRDR_RDATA_MASK)
/* CAN_RERRSYNR Register */
#define CAN_RERRSYNR_BE3_MASK                                                 (0x80000000U)
#define CAN_RERRSYNR_BE3_SHIFT                                                (31U)
#define CAN_RERRSYNR_BE3(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_RERRSYNR_BE3_SHIFT)) & CAN_RERRSYNR_BE3_MASK)
#define CAN_RERRSYNR_SYND3_MASK                                               (0x1F000000U)
#define CAN_RERRSYNR_SYND3_SHIFT                                              (24U)
#define CAN_RERRSYNR_SYND3(x)                                                 (((uint32_t)(((uint32_t)(x)) << CAN_RERRSYNR_SYND3_SHIFT)) & CAN_RERRSYNR_SYND3_MASK)
#define CAN_RERRSYNR_BE2_MASK                                                 (0x800000U)
#define CAN_RERRSYNR_BE2_SHIFT                                                (23U)
#define CAN_RERRSYNR_BE2(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_RERRSYNR_BE2_SHIFT)) & CAN_RERRSYNR_BE2_MASK)
#define CAN_RERRSYNR_SYND2_MASK                                               (0x1F0000U)
#define CAN_RERRSYNR_SYND2_SHIFT                                              (16U)
#define CAN_RERRSYNR_SYND2(x)                                                 (((uint32_t)(((uint32_t)(x)) << CAN_RERRSYNR_SYND2_SHIFT)) & CAN_RERRSYNR_SYND2_MASK)
#define CAN_RERRSYNR_BE1_MASK                                                 (0x8000U)
#define CAN_RERRSYNR_BE1_SHIFT                                                (15U)
#define CAN_RERRSYNR_BE1(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_RERRSYNR_BE1_SHIFT)) & CAN_RERRSYNR_BE1_MASK)
#define CAN_RERRSYNR_SYND1_MASK                                               (0x1F00U)
#define CAN_RERRSYNR_SYND1_SHIFT                                              (8U)
#define CAN_RERRSYNR_SYND1(x)                                                 (((uint32_t)(((uint32_t)(x)) << CAN_RERRSYNR_SYND1_SHIFT)) & CAN_RERRSYNR_SYND1_MASK)
#define CAN_RERRSYNR_BE0_MASK                                                 (0x80U)
#define CAN_RERRSYNR_BE0_SHIFT                                                (7U)
#define CAN_RERRSYNR_BE0(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_RERRSYNR_BE0_SHIFT)) & CAN_RERRSYNR_BE0_MASK)
#define CAN_RERRSYNR_SYND0_MASK                                               (0x1FU)
#define CAN_RERRSYNR_SYND0_SHIFT                                              (0U)
#define CAN_RERRSYNR_SYND0(x)                                                 (((uint32_t)(((uint32_t)(x)) << CAN_RERRSYNR_SYND0_SHIFT)) & CAN_RERRSYNR_SYND0_MASK)
/* CAN_ERRSR Register */
#define CAN_ERRSR_HANCEIF_MASK                                                (0x80000U)
#define CAN_ERRSR_HANCEIF_SHIFT                                               (19U)
#define CAN_ERRSR_HANCEIF(x)                                                  (((uint32_t)(((uint32_t)(x)) << CAN_ERRSR_HANCEIF_SHIFT)) & CAN_ERRSR_HANCEIF_MASK)
#define CAN_ERRSR_FANCEIF_MASK                                                (0x40000U)
#define CAN_ERRSR_FANCEIF_SHIFT                                               (18U)
#define CAN_ERRSR_FANCEIF(x)                                                  (((uint32_t)(((uint32_t)(x)) << CAN_ERRSR_FANCEIF_SHIFT)) & CAN_ERRSR_FANCEIF_MASK)
#define CAN_ERRSR_CEIF_MASK                                                   (0x10000U)
#define CAN_ERRSR_CEIF_SHIFT                                                  (16U)
#define CAN_ERRSR_CEIF(x)                                                     (((uint32_t)(((uint32_t)(x)) << CAN_ERRSR_CEIF_SHIFT)) & CAN_ERRSR_CEIF_MASK)
#define CAN_ERRSR_HANCEIOF_MASK                                               (0x8U)
#define CAN_ERRSR_HANCEIOF_SHIFT                                              (3U)
#define CAN_ERRSR_HANCEIOF(x)                                                 (((uint32_t)(((uint32_t)(x)) << CAN_ERRSR_HANCEIOF_SHIFT)) & CAN_ERRSR_HANCEIOF_MASK)
#define CAN_ERRSR_FANCEIOF_MASK                                               (0x4U)
#define CAN_ERRSR_FANCEIOF_SHIFT                                              (2U)
#define CAN_ERRSR_FANCEIOF(x)                                                 (((uint32_t)(((uint32_t)(x)) << CAN_ERRSR_FANCEIOF_SHIFT)) & CAN_ERRSR_FANCEIOF_MASK)
#define CAN_ERRSR_CEIOF_MASK                                                  (0x1U)
#define CAN_ERRSR_CEIOF_SHIFT                                                 (0U)
#define CAN_ERRSR_CEIOF(x)                                                    (((uint32_t)(((uint32_t)(x)) << CAN_ERRSR_CEIOF_SHIFT)) & CAN_ERRSR_CEIOF_MASK)
/* CAN_CTRL1_PN Register */
#define CAN_CTRL1_PN_WTOF_MSK_MASK                                            (0x20000U)
#define CAN_CTRL1_PN_WTOF_MSK_SHIFT                                           (17U)
#define CAN_CTRL1_PN_WTOF_MSK(x)                                              (((uint32_t)(((uint32_t)(x)) << CAN_CTRL1_PN_WTOF_MSK_SHIFT)) & CAN_CTRL1_PN_WTOF_MSK_MASK)
#define CAN_CTRL1_PN_WUMF_MSK_MASK                                            (0x10000U)
#define CAN_CTRL1_PN_WUMF_MSK_SHIFT                                           (16U)
#define CAN_CTRL1_PN_WUMF_MSK(x)                                              (((uint32_t)(((uint32_t)(x)) << CAN_CTRL1_PN_WUMF_MSK_SHIFT)) & CAN_CTRL1_PN_WUMF_MSK_MASK)
#define CAN_CTRL1_PN_NMATCH_MASK                                              (0xFF00U)
#define CAN_CTRL1_PN_NMATCH_SHIFT                                             (8U)
#define CAN_CTRL1_PN_NMATCH(x)                                                (((uint32_t)(((uint32_t)(x)) << CAN_CTRL1_PN_NMATCH_SHIFT)) & CAN_CTRL1_PN_NMATCH_MASK)
#define CAN_CTRL1_PN_PLFS_MASK                                                (0x30U)
#define CAN_CTRL1_PN_PLFS_SHIFT                                               (4U)
#define CAN_CTRL1_PN_PLFS(x)                                                  (((uint32_t)(((uint32_t)(x)) << CAN_CTRL1_PN_PLFS_SHIFT)) & CAN_CTRL1_PN_PLFS_MASK)
#define CAN_CTRL1_PN_IDFS_MASK                                                (0xCU)
#define CAN_CTRL1_PN_IDFS_SHIFT                                               (2U)
#define CAN_CTRL1_PN_IDFS(x)                                                  (((uint32_t)(((uint32_t)(x)) << CAN_CTRL1_PN_IDFS_SHIFT)) & CAN_CTRL1_PN_IDFS_MASK)
#define CAN_CTRL1_PN_FCS_MASK                                                 (0x3U)
#define CAN_CTRL1_PN_FCS_SHIFT                                                (0U)
#define CAN_CTRL1_PN_FCS(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_CTRL1_PN_FCS_SHIFT)) & CAN_CTRL1_PN_FCS_MASK)
/* CAN_CTRL2_PN Register */
#define CAN_CTRL2_PN_MATCHTO_MASK                                             (0xFFFFU)
#define CAN_CTRL2_PN_MATCHTO_SHIFT                                            (0U)
#define CAN_CTRL2_PN_MATCHTO(x)                                               (((uint32_t)(((uint32_t)(x)) << CAN_CTRL2_PN_MATCHTO_SHIFT)) & CAN_CTRL2_PN_MATCHTO_MASK)
/* CAN_WU_MTC Register */
#define CAN_WU_MTC_WTOF_MASK                                                  (0x20000U)
#define CAN_WU_MTC_WTOF_SHIFT                                                 (17U)
#define CAN_WU_MTC_WTOF(x)                                                    (((uint32_t)(((uint32_t)(x)) << CAN_WU_MTC_WTOF_SHIFT)) & CAN_WU_MTC_WTOF_MASK)
#define CAN_WU_MTC_WUMF_MASK                                                  (0x10000U)
#define CAN_WU_MTC_WUMF_SHIFT                                                 (16U)
#define CAN_WU_MTC_WUMF(x)                                                    (((uint32_t)(((uint32_t)(x)) << CAN_WU_MTC_WUMF_SHIFT)) & CAN_WU_MTC_WUMF_MASK)
#define CAN_WU_MTC_MCOUNTER_MASK                                              (0xFF00U)
#define CAN_WU_MTC_MCOUNTER_SHIFT                                             (8U)
#define CAN_WU_MTC_MCOUNTER(x)                                                (((uint32_t)(((uint32_t)(x)) << CAN_WU_MTC_MCOUNTER_SHIFT)) & CAN_WU_MTC_MCOUNTER_MASK)
/* CAN_FLT_ID1 Register */
#define CAN_FLT_ID1_FLT_IDE_MASK                                              (0x40000000U)
#define CAN_FLT_ID1_FLT_IDE_SHIFT                                             (30U)
#define CAN_FLT_ID1_FLT_IDE(x)                                                (((uint32_t)(((uint32_t)(x)) << CAN_FLT_ID1_FLT_IDE_SHIFT)) & CAN_FLT_ID1_FLT_IDE_MASK)
#define CAN_FLT_ID1_FLT_RTR_MASK                                              (0x20000000U)
#define CAN_FLT_ID1_FLT_RTR_SHIFT                                             (29U)
#define CAN_FLT_ID1_FLT_RTR(x)                                                (((uint32_t)(((uint32_t)(x)) << CAN_FLT_ID1_FLT_RTR_SHIFT)) & CAN_FLT_ID1_FLT_RTR_MASK)
#define CAN_FLT_ID1_FLT_ID1_MASK                                              (0x1FFFFFFFU)
#define CAN_FLT_ID1_FLT_ID1_SHIFT                                             (0U)
#define CAN_FLT_ID1_FLT_ID1(x)                                                (((uint32_t)(((uint32_t)(x)) << CAN_FLT_ID1_FLT_ID1_SHIFT)) & CAN_FLT_ID1_FLT_ID1_MASK)
/* CAN_FLT_DLC Register */
#define CAN_FLT_DLC_FLT_DLC_LO_MASK                                           (0xF0000U)
#define CAN_FLT_DLC_FLT_DLC_LO_SHIFT                                          (16U)
#define CAN_FLT_DLC_FLT_DLC_LO(x)                                             (((uint32_t)(((uint32_t)(x)) << CAN_FLT_DLC_FLT_DLC_LO_SHIFT)) & CAN_FLT_DLC_FLT_DLC_LO_MASK)
#define CAN_FLT_DLC_FLT_DLC_HI_MASK                                           (0xFU)
#define CAN_FLT_DLC_FLT_DLC_HI_SHIFT                                          (0U)
#define CAN_FLT_DLC_FLT_DLC_HI(x)                                             (((uint32_t)(((uint32_t)(x)) << CAN_FLT_DLC_FLT_DLC_HI_SHIFT)) & CAN_FLT_DLC_FLT_DLC_HI_MASK)
/* CAN_PL1_LO Register */
#define CAN_PL1_LO_Data_byte_0_MASK                                           (0xFF000000U)
#define CAN_PL1_LO_Data_byte_0_SHIFT                                          (24U)
#define CAN_PL1_LO_Data_byte_0(x)                                             (((uint32_t)(((uint32_t)(x)) << CAN_PL1_LO_Data_byte_0_SHIFT)) & CAN_PL1_LO_Data_byte_0_MASK)
#define CAN_PL1_LO_Data_byte_1_MASK                                           (0xFF0000U)
#define CAN_PL1_LO_Data_byte_1_SHIFT                                          (16U)
#define CAN_PL1_LO_Data_byte_1(x)                                             (((uint32_t)(((uint32_t)(x)) << CAN_PL1_LO_Data_byte_1_SHIFT)) & CAN_PL1_LO_Data_byte_1_MASK)
#define CAN_PL1_LO_Data_byte_2_MASK                                           (0xFF00U)
#define CAN_PL1_LO_Data_byte_2_SHIFT                                          (8U)
#define CAN_PL1_LO_Data_byte_2(x)                                             (((uint32_t)(((uint32_t)(x)) << CAN_PL1_LO_Data_byte_2_SHIFT)) & CAN_PL1_LO_Data_byte_2_MASK)
#define CAN_PL1_LO_Data_byte_3_MASK                                           (0xFFU)
#define CAN_PL1_LO_Data_byte_3_SHIFT                                          (0U)
#define CAN_PL1_LO_Data_byte_3(x)                                             (((uint32_t)(((uint32_t)(x)) << CAN_PL1_LO_Data_byte_3_SHIFT)) & CAN_PL1_LO_Data_byte_3_MASK)
/* CAN_PL1_HI Register */
#define CAN_PL1_HI_Data_byte_4_MASK                                           (0xFF000000U)
#define CAN_PL1_HI_Data_byte_4_SHIFT                                          (24U)
#define CAN_PL1_HI_Data_byte_4(x)                                             (((uint32_t)(((uint32_t)(x)) << CAN_PL1_HI_Data_byte_4_SHIFT)) & CAN_PL1_HI_Data_byte_4_MASK)
#define CAN_PL1_HI_Data_byte_5_MASK                                           (0xFF0000U)
#define CAN_PL1_HI_Data_byte_5_SHIFT                                          (16U)
#define CAN_PL1_HI_Data_byte_5(x)                                             (((uint32_t)(((uint32_t)(x)) << CAN_PL1_HI_Data_byte_5_SHIFT)) & CAN_PL1_HI_Data_byte_5_MASK)
#define CAN_PL1_HI_Data_byte_6_MASK                                           (0xFF00U)
#define CAN_PL1_HI_Data_byte_6_SHIFT                                          (8U)
#define CAN_PL1_HI_Data_byte_6(x)                                             (((uint32_t)(((uint32_t)(x)) << CAN_PL1_HI_Data_byte_6_SHIFT)) & CAN_PL1_HI_Data_byte_6_MASK)
#define CAN_PL1_HI_Data_byte_7_MASK                                           (0xFFU)
#define CAN_PL1_HI_Data_byte_7_SHIFT                                          (0U)
#define CAN_PL1_HI_Data_byte_7(x)                                             (((uint32_t)(((uint32_t)(x)) << CAN_PL1_HI_Data_byte_7_SHIFT)) & CAN_PL1_HI_Data_byte_7_MASK)
/* CAN_FLT_ID2_IDMASK Register */
#define CAN_FLT_ID2_IDMASK_IDE_MSK_MASK                                       (0x40000000U)
#define CAN_FLT_ID2_IDMASK_IDE_MSK_SHIFT                                      (30U)
#define CAN_FLT_ID2_IDMASK_IDE_MSK(x)                                         (((uint32_t)(((uint32_t)(x)) << CAN_FLT_ID2_IDMASK_IDE_MSK_SHIFT)) & CAN_FLT_ID2_IDMASK_IDE_MSK_MASK)
#define CAN_FLT_ID2_IDMASK_RTR_MSK_MASK                                       (0x20000000U)
#define CAN_FLT_ID2_IDMASK_RTR_MSK_SHIFT                                      (29U)
#define CAN_FLT_ID2_IDMASK_RTR_MSK(x)                                         (((uint32_t)(((uint32_t)(x)) << CAN_FLT_ID2_IDMASK_RTR_MSK_SHIFT)) & CAN_FLT_ID2_IDMASK_RTR_MSK_MASK)
#define CAN_FLT_ID2_IDMASK_FLT_ID2_IDMASK_MASK                                (0x1FFFFFFFU)
#define CAN_FLT_ID2_IDMASK_FLT_ID2_IDMASK_SHIFT                               (0U)
#define CAN_FLT_ID2_IDMASK_FLT_ID2_IDMASK(x)                                  (((uint32_t)(((uint32_t)(x)) << CAN_FLT_ID2_IDMASK_FLT_ID2_IDMASK_SHIFT)) & CAN_FLT_ID2_IDMASK_FLT_ID2_IDMASK_MASK)
/* CAN_PL2_PLMASK_LO Register */
#define CAN_PL2_PLMASK_LO_Data_byte_0_MASK                                    (0xFF000000U)
#define CAN_PL2_PLMASK_LO_Data_byte_0_SHIFT                                   (24U)
#define CAN_PL2_PLMASK_LO_Data_byte_0(x)                                      (((uint32_t)(((uint32_t)(x)) << CAN_PL2_PLMASK_LO_Data_byte_0_SHIFT)) & CAN_PL2_PLMASK_LO_Data_byte_0_MASK)
#define CAN_PL2_PLMASK_LO_Data_byte_1_MASK                                    (0xFF0000U)
#define CAN_PL2_PLMASK_LO_Data_byte_1_SHIFT                                   (16U)
#define CAN_PL2_PLMASK_LO_Data_byte_1(x)                                      (((uint32_t)(((uint32_t)(x)) << CAN_PL2_PLMASK_LO_Data_byte_1_SHIFT)) & CAN_PL2_PLMASK_LO_Data_byte_1_MASK)
#define CAN_PL2_PLMASK_LO_Data_byte_2_MASK                                    (0xFF00U)
#define CAN_PL2_PLMASK_LO_Data_byte_2_SHIFT                                   (8U)
#define CAN_PL2_PLMASK_LO_Data_byte_2(x)                                      (((uint32_t)(((uint32_t)(x)) << CAN_PL2_PLMASK_LO_Data_byte_2_SHIFT)) & CAN_PL2_PLMASK_LO_Data_byte_2_MASK)
#define CAN_PL2_PLMASK_LO_Data_byte_3_MASK                                    (0xFFU)
#define CAN_PL2_PLMASK_LO_Data_byte_3_SHIFT                                   (0U)
#define CAN_PL2_PLMASK_LO_Data_byte_3(x)                                      (((uint32_t)(((uint32_t)(x)) << CAN_PL2_PLMASK_LO_Data_byte_3_SHIFT)) & CAN_PL2_PLMASK_LO_Data_byte_3_MASK)
/* CAN_PL2_PLMASK_HI Register */
#define CAN_PL2_PLMASK_HI_Data_byte_4_MASK                                    (0xFF000000U)
#define CAN_PL2_PLMASK_HI_Data_byte_4_SHIFT                                   (24U)
#define CAN_PL2_PLMASK_HI_Data_byte_4(x)                                      (((uint32_t)(((uint32_t)(x)) << CAN_PL2_PLMASK_HI_Data_byte_4_SHIFT)) & CAN_PL2_PLMASK_HI_Data_byte_4_MASK)
#define CAN_PL2_PLMASK_HI_Data_byte_5_MASK                                    (0xFF0000U)
#define CAN_PL2_PLMASK_HI_Data_byte_5_SHIFT                                   (16U)
#define CAN_PL2_PLMASK_HI_Data_byte_5(x)                                      (((uint32_t)(((uint32_t)(x)) << CAN_PL2_PLMASK_HI_Data_byte_5_SHIFT)) & CAN_PL2_PLMASK_HI_Data_byte_5_MASK)
#define CAN_PL2_PLMASK_HI_Data_byte_6_MASK                                    (0xFF00U)
#define CAN_PL2_PLMASK_HI_Data_byte_6_SHIFT                                   (8U)
#define CAN_PL2_PLMASK_HI_Data_byte_6(x)                                      (((uint32_t)(((uint32_t)(x)) << CAN_PL2_PLMASK_HI_Data_byte_6_SHIFT)) & CAN_PL2_PLMASK_HI_Data_byte_6_MASK)
#define CAN_PL2_PLMASK_HI_Data_byte_7_MASK                                    (0xFFU)
#define CAN_PL2_PLMASK_HI_Data_byte_7_SHIFT                                   (0U)
#define CAN_PL2_PLMASK_HI_Data_byte_7(x)                                      (((uint32_t)(((uint32_t)(x)) << CAN_PL2_PLMASK_HI_Data_byte_7_SHIFT)) & CAN_PL2_PLMASK_HI_Data_byte_7_MASK)
/* CAN_WMB_WMBn_CS Register */
#define CAN_WMB_WMBn_CS_SRR_MASK                                              (0x400000U)
#define CAN_WMB_WMBn_CS_SRR_SHIFT                                             (22U)
#define CAN_WMB_WMBn_CS_SRR(x)                                                (((uint32_t)(((uint32_t)(x)) << CAN_WMB_WMBn_CS_SRR_SHIFT)) & CAN_WMB_WMBn_CS_SRR_MASK)
#define CAN_WMB_WMBn_CS_IDE_MASK                                              (0x200000U)
#define CAN_WMB_WMBn_CS_IDE_SHIFT                                             (21U)
#define CAN_WMB_WMBn_CS_IDE(x)                                                (((uint32_t)(((uint32_t)(x)) << CAN_WMB_WMBn_CS_IDE_SHIFT)) & CAN_WMB_WMBn_CS_IDE_MASK)
#define CAN_WMB_WMBn_CS_RTR_MASK                                              (0x100000U)
#define CAN_WMB_WMBn_CS_RTR_SHIFT                                             (20U)
#define CAN_WMB_WMBn_CS_RTR(x)                                                (((uint32_t)(((uint32_t)(x)) << CAN_WMB_WMBn_CS_RTR_SHIFT)) & CAN_WMB_WMBn_CS_RTR_MASK)
#define CAN_WMB_WMBn_CS_DLC_MASK                                              (0xF0000U)
#define CAN_WMB_WMBn_CS_DLC_SHIFT                                             (16U)
#define CAN_WMB_WMBn_CS_DLC(x)                                                (((uint32_t)(((uint32_t)(x)) << CAN_WMB_WMBn_CS_DLC_SHIFT)) & CAN_WMB_WMBn_CS_DLC_MASK)
/* CAN_WMB_WMBn_ID Register */
#define CAN_WMB_WMBn_ID_ID_MASK                                               (0x1FFFFFFFU)
#define CAN_WMB_WMBn_ID_ID_SHIFT                                              (0U)
#define CAN_WMB_WMBn_ID_ID(x)                                                 (((uint32_t)(((uint32_t)(x)) << CAN_WMB_WMBn_ID_ID_SHIFT)) & CAN_WMB_WMBn_ID_ID_MASK)
/* CAN_WMB_WMBn_D03 Register */
#define CAN_WMB_WMBn_D03_Data_byte_0_MASK                                     (0xFF000000U)
#define CAN_WMB_WMBn_D03_Data_byte_0_SHIFT                                    (24U)
#define CAN_WMB_WMBn_D03_Data_byte_0(x)                                       (((uint32_t)(((uint32_t)(x)) << CAN_WMB_WMBn_D03_Data_byte_0_SHIFT)) & CAN_WMB_WMBn_D03_Data_byte_0_MASK)
#define CAN_WMB_WMBn_D03_Data_byte_1_MASK                                     (0xFF0000U)
#define CAN_WMB_WMBn_D03_Data_byte_1_SHIFT                                    (16U)
#define CAN_WMB_WMBn_D03_Data_byte_1(x)                                       (((uint32_t)(((uint32_t)(x)) << CAN_WMB_WMBn_D03_Data_byte_1_SHIFT)) & CAN_WMB_WMBn_D03_Data_byte_1_MASK)
#define CAN_WMB_WMBn_D03_Data_byte_2_MASK                                     (0xFF00U)
#define CAN_WMB_WMBn_D03_Data_byte_2_SHIFT                                    (8U)
#define CAN_WMB_WMBn_D03_Data_byte_2(x)                                       (((uint32_t)(((uint32_t)(x)) << CAN_WMB_WMBn_D03_Data_byte_2_SHIFT)) & CAN_WMB_WMBn_D03_Data_byte_2_MASK)
#define CAN_WMB_WMBn_D03_Data_byte_3_MASK                                     (0xFFU)
#define CAN_WMB_WMBn_D03_Data_byte_3_SHIFT                                    (0U)
#define CAN_WMB_WMBn_D03_Data_byte_3(x)                                       (((uint32_t)(((uint32_t)(x)) << CAN_WMB_WMBn_D03_Data_byte_3_SHIFT)) & CAN_WMB_WMBn_D03_Data_byte_3_MASK)
/* CAN_WMB_WMBn_D47 Register */
#define CAN_WMB_WMBn_D47_Data_byte_4_MASK                                     (0xFF000000U)
#define CAN_WMB_WMBn_D47_Data_byte_4_SHIFT                                    (24U)
#define CAN_WMB_WMBn_D47_Data_byte_4(x)                                       (((uint32_t)(((uint32_t)(x)) << CAN_WMB_WMBn_D47_Data_byte_4_SHIFT)) & CAN_WMB_WMBn_D47_Data_byte_4_MASK)
#define CAN_WMB_WMBn_D47_Data_byte_5_MASK                                     (0xFF0000U)
#define CAN_WMB_WMBn_D47_Data_byte_5_SHIFT                                    (16U)
#define CAN_WMB_WMBn_D47_Data_byte_5(x)                                       (((uint32_t)(((uint32_t)(x)) << CAN_WMB_WMBn_D47_Data_byte_5_SHIFT)) & CAN_WMB_WMBn_D47_Data_byte_5_MASK)
#define CAN_WMB_WMBn_D47_Data_byte_6_MASK                                     (0xFF00U)
#define CAN_WMB_WMBn_D47_Data_byte_6_SHIFT                                    (8U)
#define CAN_WMB_WMBn_D47_Data_byte_6(x)                                       (((uint32_t)(((uint32_t)(x)) << CAN_WMB_WMBn_D47_Data_byte_6_SHIFT)) & CAN_WMB_WMBn_D47_Data_byte_6_MASK)
#define CAN_WMB_WMBn_D47_Data_byte_7_MASK                                     (0xFFU)
#define CAN_WMB_WMBn_D47_Data_byte_7_SHIFT                                    (0U)
#define CAN_WMB_WMBn_D47_Data_byte_7(x)                                       (((uint32_t)(((uint32_t)(x)) << CAN_WMB_WMBn_D47_Data_byte_7_SHIFT)) & CAN_WMB_WMBn_D47_Data_byte_7_MASK)

/* CAN_EPRS Register */
#define CAN_EPRS_EDPRESDIV_MASK                                               (0x3FF0000U)
#define CAN_EPRS_EDPRESDIV_SHIFT                                              (16U)
#define CAN_EPRS_EDPRESDIV(x)                                                 (((uint32_t)(((uint32_t)(x)) << CAN_EPRS_EDPRESDIV_SHIFT)) & CAN_EPRS_EDPRESDIV_MASK)
#define CAN_EPRS_ENPRESDIV_MASK                                               (0x3FFU)
#define CAN_EPRS_ENPRESDIV_SHIFT                                              (0U)
#define CAN_EPRS_ENPRESDIV(x)                                                 (((uint32_t)(((uint32_t)(x)) << CAN_EPRS_ENPRESDIV_SHIFT)) & CAN_EPRS_ENPRESDIV_MASK)
/* CAN_ENCBT Register */
#define CAN_ENCBT_NRJW_MASK                                                   (0x1FC00000U)
#define CAN_ENCBT_NRJW_SHIFT                                                  (22U)
#define CAN_ENCBT_NRJW(x)                                                     (((uint32_t)(((uint32_t)(x)) << CAN_ENCBT_NRJW_SHIFT)) & CAN_ENCBT_NRJW_MASK)
#define CAN_ENCBT_NTSEG2_MASK                                                 (0x7F000U)
#define CAN_ENCBT_NTSEG2_SHIFT                                                (12U)
#define CAN_ENCBT_NTSEG2(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_ENCBT_NTSEG2_SHIFT)) & CAN_ENCBT_NTSEG2_MASK)
#define CAN_ENCBT_NTSEG1_MASK                                                 (0xFFU)
#define CAN_ENCBT_NTSEG1_SHIFT                                                (0U)
#define CAN_ENCBT_NTSEG1(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_ENCBT_NTSEG1_SHIFT)) & CAN_ENCBT_NTSEG1_MASK)
/* CAN_EDCBT Register */
#define CAN_EDCBT_DRJW_MASK                                                   (0x3C00000U)
#define CAN_EDCBT_DRJW_SHIFT                                                  (22U)
#define CAN_EDCBT_DRJW(x)                                                     (((uint32_t)(((uint32_t)(x)) << CAN_EDCBT_DRJW_SHIFT)) & CAN_EDCBT_DRJW_MASK)
#define CAN_EDCBT_DTSEG2_MASK                                                 (0xF000U)
#define CAN_EDCBT_DTSEG2_SHIFT                                                (12U)
#define CAN_EDCBT_DTSEG2(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_EDCBT_DTSEG2_SHIFT)) & CAN_EDCBT_DTSEG2_MASK)
#define CAN_EDCBT_DTSEG1_MASK                                                 (0x1FU)
#define CAN_EDCBT_DTSEG1_SHIFT                                                (0U)
#define CAN_EDCBT_DTSEG1(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_EDCBT_DTSEG1_SHIFT)) & CAN_EDCBT_DTSEG1_MASK)
/* CAN_ETDC Register */
#define CAN_ETDC_ETDCEN_MASK                                                  (0x80000000U)
#define CAN_ETDC_ETDCEN_SHIFT                                                 (31U)
#define CAN_ETDC_ETDCEN(x)                                                    (((uint32_t)(((uint32_t)(x)) << CAN_ETDC_ETDCEN_SHIFT)) & CAN_ETDC_ETDCEN_MASK)
#define CAN_ETDC_TDMDIS_MASK                                                  (0x40000000U)
#define CAN_ETDC_TDMDIS_SHIFT                                                 (30U)
#define CAN_ETDC_TDMDIS(x)                                                    (((uint32_t)(((uint32_t)(x)) << CAN_ETDC_TDMDIS_SHIFT)) & CAN_ETDC_TDMDIS_MASK)
#define CAN_ETDC_ETDCOFF_MASK                                                 (0x7F0000U)
#define CAN_ETDC_ETDCOFF_SHIFT                                                (16U)
#define CAN_ETDC_ETDCOFF(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_ETDC_ETDCOFF_SHIFT)) & CAN_ETDC_ETDCOFF_MASK)
#define CAN_ETDC_ETDCFAIL_MASK                                                (0x8000U)
#define CAN_ETDC_ETDCFAIL_SHIFT                                               (15U)
#define CAN_ETDC_ETDCFAIL(x)                                                  (((uint32_t)(((uint32_t)(x)) << CAN_ETDC_ETDCFAIL_SHIFT)) & CAN_ETDC_ETDCFAIL_MASK)
#define CAN_ETDC_ETDCVAL_MASK                                                 (0xFFU)
#define CAN_ETDC_ETDCVAL_SHIFT                                                (0U)
#define CAN_ETDC_ETDCVAL(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_ETDC_ETDCVAL_SHIFT)) & CAN_ETDC_ETDCVAL_MASK)
/* CAN_FDCTRL Register */
#define CAN_FDCTRL_FDRATE_MASK                                                (0x80000000U)
#define CAN_FDCTRL_FDRATE_SHIFT                                               (31U)
#define CAN_FDCTRL_FDRATE(x)                                                  (((uint32_t)(((uint32_t)(x)) << CAN_FDCTRL_FDRATE_SHIFT)) & CAN_FDCTRL_FDRATE_MASK)
#define CAN_FDCTRL_MBDSR3_MASK                                                (0x6000000U)
#define CAN_FDCTRL_MBDSR3_SHIFT                                               (25U)
#define CAN_FDCTRL_MBDSR3(x)                                                  (((uint32_t)(((uint32_t)(x)) << CAN_FDCTRL_MBDSR3_SHIFT)) & CAN_FDCTRL_MBDSR3_MASK)
#define CAN_FDCTRL_MBDSR2_MASK                                                (0xC00000U)
#define CAN_FDCTRL_MBDSR2_SHIFT                                               (22U)
#define CAN_FDCTRL_MBDSR2(x)                                                  (((uint32_t)(((uint32_t)(x)) << CAN_FDCTRL_MBDSR2_SHIFT)) & CAN_FDCTRL_MBDSR2_MASK)
#define CAN_FDCTRL_MBDSR1_MASK                                                (0x180000U)
#define CAN_FDCTRL_MBDSR1_SHIFT                                               (19U)
#define CAN_FDCTRL_MBDSR1(x)                                                  (((uint32_t)(((uint32_t)(x)) << CAN_FDCTRL_MBDSR1_SHIFT)) & CAN_FDCTRL_MBDSR1_MASK)
#define CAN_FDCTRL_MBDSR0_MASK                                                (0x30000U)
#define CAN_FDCTRL_MBDSR0_SHIFT                                               (16U)
#define CAN_FDCTRL_MBDSR0(x)                                                  (((uint32_t)(((uint32_t)(x)) << CAN_FDCTRL_MBDSR0_SHIFT)) & CAN_FDCTRL_MBDSR0_MASK)
#define CAN_FDCTRL_TDCEN_MASK                                                 (0x8000U)
#define CAN_FDCTRL_TDCEN_SHIFT                                                (15U)
#define CAN_FDCTRL_TDCEN(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_FDCTRL_TDCEN_SHIFT)) & CAN_FDCTRL_TDCEN_MASK)
#define CAN_FDCTRL_TDCFAIL_MASK                                               (0x4000U)
#define CAN_FDCTRL_TDCFAIL_SHIFT                                              (14U)
#define CAN_FDCTRL_TDCFAIL(x)                                                 (((uint32_t)(((uint32_t)(x)) << CAN_FDCTRL_TDCFAIL_SHIFT)) & CAN_FDCTRL_TDCFAIL_MASK)
#define CAN_FDCTRL_TDCOFF_MASK                                                (0x1F00U)
#define CAN_FDCTRL_TDCOFF_SHIFT                                               (8U)
#define CAN_FDCTRL_TDCOFF(x)                                                  (((uint32_t)(((uint32_t)(x)) << CAN_FDCTRL_TDCOFF_SHIFT)) & CAN_FDCTRL_TDCOFF_MASK)
#define CAN_FDCTRL_TDCVAL_MASK                                                (0x3FU)
#define CAN_FDCTRL_TDCVAL_SHIFT                                               (0U)
#define CAN_FDCTRL_TDCVAL(x)                                                  (((uint32_t)(((uint32_t)(x)) << CAN_FDCTRL_TDCVAL_SHIFT)) & CAN_FDCTRL_TDCVAL_MASK)
/* CAN_FDCBT Register */
#define CAN_FDCBT_FPRESDIV_MASK                                               (0x3FF00000U)
#define CAN_FDCBT_FPRESDIV_SHIFT                                              (20U)
#define CAN_FDCBT_FPRESDIV(x)                                                 (((uint32_t)(((uint32_t)(x)) << CAN_FDCBT_FPRESDIV_SHIFT)) & CAN_FDCBT_FPRESDIV_MASK)
#define CAN_FDCBT_FRJW_MASK                                                   (0x70000U)
#define CAN_FDCBT_FRJW_SHIFT                                                  (16U)
#define CAN_FDCBT_FRJW(x)                                                     (((uint32_t)(((uint32_t)(x)) << CAN_FDCBT_FRJW_SHIFT)) & CAN_FDCBT_FRJW_MASK)
#define CAN_FDCBT_FPROPSEG_MASK                                               (0x7C00U)
#define CAN_FDCBT_FPROPSEG_SHIFT                                              (10U)
#define CAN_FDCBT_FPROPSEG(x)                                                 (((uint32_t)(((uint32_t)(x)) << CAN_FDCBT_FPROPSEG_SHIFT)) & CAN_FDCBT_FPROPSEG_MASK)
#define CAN_FDCBT_FPSEG1_MASK                                                 (0xE0U)
#define CAN_FDCBT_FPSEG1_SHIFT                                                (5U)
#define CAN_FDCBT_FPSEG1(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_FDCBT_FPSEG1_SHIFT)) & CAN_FDCBT_FPSEG1_MASK)
#define CAN_FDCBT_FPSEG2_MASK                                                 (0x7U)
#define CAN_FDCBT_FPSEG2_SHIFT                                                (0U)
#define CAN_FDCBT_FPSEG2(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_FDCBT_FPSEG2_SHIFT)) & CAN_FDCBT_FPSEG2_MASK)
/* CAN_FDCRC Register */
#define CAN_FDCRC_FD_MBCRC_MASK                                               (0x7F000000U)
#define CAN_FDCRC_FD_MBCRC_SHIFT                                              (24U)
#define CAN_FDCRC_FD_MBCRC(x)                                                 (((uint32_t)(((uint32_t)(x)) << CAN_FDCRC_FD_MBCRC_SHIFT)) & CAN_FDCRC_FD_MBCRC_MASK)
#define CAN_FDCRC_FD_TXCRC_MASK                                               (0x1FFFFFU)
#define CAN_FDCRC_FD_TXCRC_SHIFT                                              (0U)
#define CAN_FDCRC_FD_TXCRC(x)                                                 (((uint32_t)(((uint32_t)(x)) << CAN_FDCRC_FD_TXCRC_SHIFT)) & CAN_FDCRC_FD_TXCRC_MASK)
/* CAN_ERFCR Register */
#define CAN_ERFCR_ERFEN_MASK                                                  (0x80000000U)
#define CAN_ERFCR_ERFEN_SHIFT                                                 (31U)
#define CAN_ERFCR_ERFEN(x)                                                    (((uint32_t)(((uint32_t)(x)) << CAN_ERFCR_ERFEN_SHIFT)) & CAN_ERFCR_ERFEN_MASK)
#define CAN_ERFCR_DMALW_MASK                                                  (0x7C000000U)
#define CAN_ERFCR_DMALW_SHIFT                                                 (26U)
#define CAN_ERFCR_DMALW(x)                                                    (((uint32_t)(((uint32_t)(x)) << CAN_ERFCR_DMALW_SHIFT)) & CAN_ERFCR_DMALW_MASK)
#define CAN_ERFCR_NEXIF_MASK                                                  (0x7F0000U)
#define CAN_ERFCR_NEXIF_SHIFT                                                 (16U)
#define CAN_ERFCR_NEXIF(x)                                                    (((uint32_t)(((uint32_t)(x)) << CAN_ERFCR_NEXIF_SHIFT)) & CAN_ERFCR_NEXIF_MASK)
#define CAN_ERFCR_NFE_MASK                                                    (0x3F00U)
#define CAN_ERFCR_NFE_SHIFT                                                   (8U)
#define CAN_ERFCR_NFE(x)                                                      (((uint32_t)(((uint32_t)(x)) << CAN_ERFCR_NFE_SHIFT)) & CAN_ERFCR_NFE_MASK)
#define CAN_ERFCR_ERFWM_MASK                                                  (0x1FU)
#define CAN_ERFCR_ERFWM_SHIFT                                                 (0U)
#define CAN_ERFCR_ERFWM(x)                                                    (((uint32_t)(((uint32_t)(x)) << CAN_ERFCR_ERFWM_SHIFT)) & CAN_ERFCR_ERFWM_MASK)
/* CAN_ERFIER Register */
#define CAN_ERFIER_ERFUFWIE_MASK                                              (0x80000000U)
#define CAN_ERFIER_ERFUFWIE_SHIFT                                             (31U)
#define CAN_ERFIER_ERFUFWIE(x)                                                (((uint32_t)(((uint32_t)(x)) << CAN_ERFIER_ERFUFWIE_SHIFT)) & CAN_ERFIER_ERFUFWIE_MASK)
#define CAN_ERFIER_ERFOVFIE_MASK                                              (0x40000000U)
#define CAN_ERFIER_ERFOVFIE_SHIFT                                             (30U)
#define CAN_ERFIER_ERFOVFIE(x)                                                (((uint32_t)(((uint32_t)(x)) << CAN_ERFIER_ERFOVFIE_SHIFT)) & CAN_ERFIER_ERFOVFIE_MASK)
#define CAN_ERFIER_ERFWMIIE_MASK                                              (0x20000000U)
#define CAN_ERFIER_ERFWMIIE_SHIFT                                             (29U)
#define CAN_ERFIER_ERFWMIIE(x)                                                (((uint32_t)(((uint32_t)(x)) << CAN_ERFIER_ERFWMIIE_SHIFT)) & CAN_ERFIER_ERFWMIIE_MASK)
#define CAN_ERFIER_ERFDAIE_MASK                                               (0x10000000U)
#define CAN_ERFIER_ERFDAIE_SHIFT                                              (28U)
#define CAN_ERFIER_ERFDAIE(x)                                                 (((uint32_t)(((uint32_t)(x)) << CAN_ERFIER_ERFDAIE_SHIFT)) & CAN_ERFIER_ERFDAIE_MASK)
/* CAN_ERFSR Register */
#define CAN_ERFSR_ERFUFW_MASK                                                 (0x80000000U)
#define CAN_ERFSR_ERFUFW_SHIFT                                                (31U)
#define CAN_ERFSR_ERFUFW(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_ERFSR_ERFUFW_SHIFT)) & CAN_ERFSR_ERFUFW_MASK)
#define CAN_ERFSR_ERFOVF_MASK                                                 (0x40000000U)
#define CAN_ERFSR_ERFOVF_SHIFT                                                (30U)
#define CAN_ERFSR_ERFOVF(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_ERFSR_ERFOVF_SHIFT)) & CAN_ERFSR_ERFOVF_MASK)
#define CAN_ERFSR_ERFWMI_MASK                                                 (0x20000000U)
#define CAN_ERFSR_ERFWMI_SHIFT                                                (29U)
#define CAN_ERFSR_ERFWMI(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_ERFSR_ERFWMI_SHIFT)) & CAN_ERFSR_ERFWMI_MASK)
#define CAN_ERFSR_ERFDA_MASK                                                  (0x10000000U)
#define CAN_ERFSR_ERFDA_SHIFT                                                 (28U)
#define CAN_ERFSR_ERFDA(x)                                                    (((uint32_t)(((uint32_t)(x)) << CAN_ERFSR_ERFDA_SHIFT)) & CAN_ERFSR_ERFDA_MASK)
#define CAN_ERFSR_ERFCLR_MASK                                                 (0x8000000U)
#define CAN_ERFSR_ERFCLR_SHIFT                                                (27U)
#define CAN_ERFSR_ERFCLR(x)                                                   (((uint32_t)(((uint32_t)(x)) << CAN_ERFSR_ERFCLR_SHIFT)) & CAN_ERFSR_ERFCLR_MASK)
#define CAN_ERFSR_ERFE_MASK                                                   (0x20000U)
#define CAN_ERFSR_ERFE_SHIFT                                                  (17U)
#define CAN_ERFSR_ERFE(x)                                                     (((uint32_t)(((uint32_t)(x)) << CAN_ERFSR_ERFE_SHIFT)) & CAN_ERFSR_ERFE_MASK)
#define CAN_ERFSR_ERFF_MASK                                                   (0x10000U)
#define CAN_ERFSR_ERFF_SHIFT                                                  (16U)
#define CAN_ERFSR_ERFF(x)                                                     (((uint32_t)(((uint32_t)(x)) << CAN_ERFSR_ERFF_SHIFT)) & CAN_ERFSR_ERFF_MASK)
#define CAN_ERFSR_ERFEL_MASK                                                  (0x3FU)
#define CAN_ERFSR_ERFEL_SHIFT                                                 (0U)
#define CAN_ERFSR_ERFEL(x)                                                    (((uint32_t)(((uint32_t)(x)) << CAN_ERFSR_ERFEL_SHIFT)) & CAN_ERFSR_ERFEL_MASK)
/* CAN_HR_TIME_STAMPn Register */
#define CAN_HR_TIME_STAMPn_TS_MASK                                            (0xFFFFFFFFU)
#define CAN_HR_TIME_STAMPn_TS_SHIFT                                           (0U)
#define CAN_HR_TIME_STAMPn_TS(x)                                              (((uint32_t)(((uint32_t)(x)) << CAN_HR_TIME_STAMPn_TS_SHIFT)) & CAN_HR_TIME_STAMPn_TS_MASK)
/* CAN_ERFFELn Register */
#define CAN_ERFFELn_FEL_MASK                                                  (0xFFFFFFFFU)
#define CAN_ERFFELn_FEL_SHIFT                                                 (0U)
#define CAN_ERFFELn_FEL(x)                                                    (((uint32_t)(((uint32_t)(x)) << CAN_ERFFELn_FEL_SHIFT)) & CAN_ERFFELn_FEL_MASK)


/*!
 * @}
 */ /* end of group CAN_Register_Masks */


/*!
 * @}
 */ /* end of group CAN_Peripheral_Access_Layer */


/* -------------------------------------------------------------
   -- UART Peripheral Access Layer
   ------------------------------------------------------------- */

/*!
 * @addtogroup UART_Peripheral_Access_Layer UART Peripheral Access Layer
 * @{
 */


/** UART - Size of Registers Arrays */

/* UART Register Layout Typedef */
typedef struct {
    __IO uint32_t BAUD;                                                       /**< Baud Rate Configuration Register, offset: 0x0000 */
    __IO uint32_t CTRL0;                                                      /**< Control 0 Register, offset: 0x0004 */
    __IO uint32_t CTRL1;                                                      /**< Control 1 Register, offset: 0x0008 */
    __IO uint32_t INTF;                                                       /**< Interrupt Flag Register, offset: 0x000C */
    __IO uint32_t INTE;                                                       /**< Interrupt Enable Register, offset: 0x0010 */
    __IO uint32_t MATCH;                                                      /**< Match Configuration Register, offset: 0x0014 */
    __IO uint32_t DATA;                                                       /**< Data Buffer Register, offset: 0x0018 */
    __IO uint32_t TXFIFO;                                                     /**< TX FIFO Configuration Register, offset: 0x001C */
    __IO uint32_t RXFIFO;                                                     /**< RX FIFO Configuration Register, offset: 0x0020 */

} UART_Type, *UART_MemMapPtr;

/** Number of instances of the UART module. */
#define UART_INSTANCE_COUNT                                                   (3U)

/* UART0  base address */
#define UART0_BASE                                                            (0x4001B000U)
#define UART0                                                                 ((UART_Type *)(UART0_BASE))
/* UART1  base address */
#define UART1_BASE                                                            (0x4001C000U)
#define UART1                                                                 ((UART_Type *)(UART1_BASE))
/* UART2  base address */
#define UART2_BASE                                                            (0x4001D000U)
#define UART2                                                                 ((UART_Type *)(UART2_BASE))

/** Array initializer of UART peripheral base addresses */
#define UART_BASE_ADDRS                                                       { UART0_BASE,  UART1_BASE,  UART2_BASE }
/** Array initializer of UART peripheral base pointers */
#define UART_BASE_PTRS                                                        { UART0,  UART1,  UART2 }
/** Number of interrupt channels for the RX_TX type of UART. */
#define UART_RX_TX_IRQS_CH_COUNT                                              (1U)
#define UART_RX_TX_IRQS                                                       { UART0_IRQn, UART1_IRQn, UART2_IRQn }

/* UART Register Mask */
/* UART_BAUD Register */
#define UART_BAUD_OSRVAL_MASK                                                 (0x1F0000U)
#define UART_BAUD_OSRVAL_SHIFT                                                (16U)
#define UART_BAUD_OSRVAL(x)                                                   (((uint32_t)(((uint32_t)(x)) << UART_BAUD_OSRVAL_SHIFT)) & UART_BAUD_OSRVAL_MASK)
#define UART_BAUD_DIV_MASK                                                    (0x1FFFU)
#define UART_BAUD_DIV_SHIFT                                                   (0U)
#define UART_BAUD_DIV(x)                                                      (((uint32_t)(((uint32_t)(x)) << UART_BAUD_DIV_SHIFT)) & UART_BAUD_DIV_MASK)
/* UART_CTRL0 Register */
#define UART_CTRL0_TXDMAEN_MASK                                               (0x20000000U)
#define UART_CTRL0_TXDMAEN_SHIFT                                              (29U)
#define UART_CTRL0_TXDMAEN(x)                                                 (((uint32_t)(((uint32_t)(x)) << UART_CTRL0_TXDMAEN_SHIFT)) & UART_CTRL0_TXDMAEN_MASK)
#define UART_CTRL0_RXDMAEN_MASK                                               (0x10000000U)
#define UART_CTRL0_RXDMAEN_SHIFT                                              (28U)
#define UART_CTRL0_RXDMAEN(x)                                                 (((uint32_t)(((uint32_t)(x)) << UART_CTRL0_RXDMAEN_SHIFT)) & UART_CTRL0_RXDMAEN_MASK)
#define UART_CTRL0_IDLEDMAEN_MASK                                             (0x8000000U)
#define UART_CTRL0_IDLEDMAEN_SHIFT                                            (27U)
#define UART_CTRL0_IDLEDMAEN(x)                                               (((uint32_t)(((uint32_t)(x)) << UART_CTRL0_IDLEDMAEN_SHIFT)) & UART_CTRL0_IDLEDMAEN_MASK)
#define UART_CTRL0_TXCTSRC_MASK                                               (0x4000000U)
#define UART_CTRL0_TXCTSRC_SHIFT                                              (26U)
#define UART_CTRL0_TXCTSRC(x)                                                 (((uint32_t)(((uint32_t)(x)) << UART_CTRL0_TXCTSRC_SHIFT)) & UART_CTRL0_TXCTSRC_MASK)
#define UART_CTRL0_TXCTSC_MASK                                                (0x2000000U)
#define UART_CTRL0_TXCTSC_SHIFT                                               (25U)
#define UART_CTRL0_TXCTSC(x)                                                  (((uint32_t)(((uint32_t)(x)) << UART_CTRL0_TXCTSC_SHIFT)) & UART_CTRL0_TXCTSC_MASK)
#define UART_CTRL0_TXRTXPOL_MASK                                              (0x800000U)
#define UART_CTRL0_TXRTXPOL_SHIFT                                             (23U)
#define UART_CTRL0_TXRTXPOL(x)                                                (((uint32_t)(((uint32_t)(x)) << UART_CTRL0_TXRTXPOL_SHIFT)) & UART_CTRL0_TXRTXPOL_MASK)
#define UART_CTRL0_MATEN_MASK                                                 (0x200000U)
#define UART_CTRL0_MATEN_SHIFT                                                (21U)
#define UART_CTRL0_MATEN(x)                                                   (((uint32_t)(((uint32_t)(x)) << UART_CTRL0_MATEN_SHIFT)) & UART_CTRL0_MATEN_MASK)
#define UART_CTRL0_MATMOD_MASK                                                (0x30000U)
#define UART_CTRL0_MATMOD_SHIFT                                               (16U)
#define UART_CTRL0_MATMOD(x)                                                  (((uint32_t)(((uint32_t)(x)) << UART_CTRL0_MATMOD_SHIFT)) & UART_CTRL0_MATMOD_MASK)
#define UART_CTRL0_RTSWATER_MASK                                              (0x1C00U)
#define UART_CTRL0_RTSWATER_SHIFT                                             (10U)
#define UART_CTRL0_RTSWATER(x)                                                (((uint32_t)(((uint32_t)(x)) << UART_CTRL0_RTSWATER_SHIFT)) & UART_CTRL0_RTSWATER_MASK)
#define UART_CTRL0_RXRTSE_MASK                                                (0x10U)
#define UART_CTRL0_RXRTSE_SHIFT                                               (4U)
#define UART_CTRL0_RXRTSE(x)                                                  (((uint32_t)(((uint32_t)(x)) << UART_CTRL0_RXRTSE_SHIFT)) & UART_CTRL0_RXRTSE_MASK)
#define UART_CTRL0_TXRTSE_MASK                                                (0x8U)
#define UART_CTRL0_TXRTSE_SHIFT                                               (3U)
#define UART_CTRL0_TXRTSE(x)                                                  (((uint32_t)(((uint32_t)(x)) << UART_CTRL0_TXRTSE_SHIFT)) & UART_CTRL0_TXRTSE_MASK)
#define UART_CTRL0_TXCTSE_MASK                                                (0x4U)
#define UART_CTRL0_TXCTSE_SHIFT                                               (2U)
#define UART_CTRL0_TXCTSE(x)                                                  (((uint32_t)(((uint32_t)(x)) << UART_CTRL0_TXCTSE_SHIFT)) & UART_CTRL0_TXCTSE_MASK)
#define UART_CTRL0_TXEN_MASK                                                  (0x2U)
#define UART_CTRL0_TXEN_SHIFT                                                 (1U)
#define UART_CTRL0_TXEN(x)                                                    (((uint32_t)(((uint32_t)(x)) << UART_CTRL0_TXEN_SHIFT)) & UART_CTRL0_TXEN_MASK)
#define UART_CTRL0_RXEN_MASK                                                  (0x1U)
#define UART_CTRL0_RXEN_SHIFT                                                 (0U)
#define UART_CTRL0_RXEN(x)                                                    (((uint32_t)(((uint32_t)(x)) << UART_CTRL0_RXEN_SHIFT)) & UART_CTRL0_RXEN_MASK)
/* UART_CTRL1 Register */
#define UART_CTRL1_FRAMESZ_MASK                                               (0xC0000000U)
#define UART_CTRL1_FRAMESZ_SHIFT                                              (30U)
#define UART_CTRL1_FRAMESZ(x)                                                 (((uint32_t)(((uint32_t)(x)) << UART_CTRL1_FRAMESZ_SHIFT)) & UART_CTRL1_FRAMESZ_MASK)
#define UART_CTRL1_STOPSZ_MASK                                                (0x8000000U)
#define UART_CTRL1_STOPSZ_SHIFT                                               (27U)
#define UART_CTRL1_STOPSZ(x)                                                  (((uint32_t)(((uint32_t)(x)) << UART_CTRL1_STOPSZ_SHIFT)) & UART_CTRL1_STOPSZ_MASK)
#define UART_CTRL1_TXINV_MASK                                                 (0x1000000U)
#define UART_CTRL1_TXINV_SHIFT                                                (24U)
#define UART_CTRL1_TXINV(x)                                                   (((uint32_t)(((uint32_t)(x)) << UART_CTRL1_TXINV_SHIFT)) & UART_CTRL1_TXINV_MASK)
#define UART_CTRL1_RXINV_MASK                                                 (0x800000U)
#define UART_CTRL1_RXINV_SHIFT                                                (23U)
#define UART_CTRL1_RXINV(x)                                                   (((uint32_t)(((uint32_t)(x)) << UART_CTRL1_RXINV_SHIFT)) & UART_CTRL1_RXINV_MASK)
#define UART_CTRL1_TXDIR_MASK                                                 (0x400000U)
#define UART_CTRL1_TXDIR_SHIFT                                                (22U)
#define UART_CTRL1_TXDIR(x)                                                   (((uint32_t)(((uint32_t)(x)) << UART_CTRL1_TXDIR_SHIFT)) & UART_CTRL1_TXDIR_MASK)
#define UART_CTRL1_SWAP_MASK                                                  (0x200000U)
#define UART_CTRL1_SWAP_SHIFT                                                 (21U)
#define UART_CTRL1_SWAP(x)                                                    (((uint32_t)(((uint32_t)(x)) << UART_CTRL1_SWAP_SHIFT)) & UART_CTRL1_SWAP_MASK)
#define UART_CTRL1_LOOP_MASK                                                  (0x100000U)
#define UART_CTRL1_LOOP_SHIFT                                                 (20U)
#define UART_CTRL1_LOOP(x)                                                    (((uint32_t)(((uint32_t)(x)) << UART_CTRL1_LOOP_SHIFT)) & UART_CTRL1_LOOP_MASK)
#define UART_CTRL1_IDLESZ_MASK                                                (0x70000U)
#define UART_CTRL1_IDLESZ_SHIFT                                               (16U)
#define UART_CTRL1_IDLESZ(x)                                                  (((uint32_t)(((uint32_t)(x)) << UART_CTRL1_IDLESZ_SHIFT)) & UART_CTRL1_IDLESZ_MASK)
#define UART_CTRL1_BOTHEDGE_MASK                                              (0x8000U)
#define UART_CTRL1_BOTHEDGE_SHIFT                                             (15U)
#define UART_CTRL1_BOTHEDGE(x)                                                (((uint32_t)(((uint32_t)(x)) << UART_CTRL1_BOTHEDGE_SHIFT)) & UART_CTRL1_BOTHEDGE_MASK)
#define UART_CTRL1_LBKDEN_MASK                                                (0x400U)
#define UART_CTRL1_LBKDEN_SHIFT                                               (10U)
#define UART_CTRL1_LBKDEN(x)                                                  (((uint32_t)(((uint32_t)(x)) << UART_CTRL1_LBKDEN_SHIFT)) & UART_CTRL1_LBKDEN_MASK)
#define UART_CTRL1_LBKSZ_MASK                                                 (0x200U)
#define UART_CTRL1_LBKSZ_SHIFT                                                (9U)
#define UART_CTRL1_LBKSZ(x)                                                   (((uint32_t)(((uint32_t)(x)) << UART_CTRL1_LBKSZ_SHIFT)) & UART_CTRL1_LBKSZ_MASK)
#define UART_CTRL1_MSBF_MASK                                                  (0x100U)
#define UART_CTRL1_MSBF_SHIFT                                                 (8U)
#define UART_CTRL1_MSBF(x)                                                    (((uint32_t)(((uint32_t)(x)) << UART_CTRL1_MSBF_SHIFT)) & UART_CTRL1_MSBF_MASK)
#define UART_CTRL1_SBK_MASK                                                   (0x40U)
#define UART_CTRL1_SBK_SHIFT                                                  (6U)
#define UART_CTRL1_SBK(x)                                                     (((uint32_t)(((uint32_t)(x)) << UART_CTRL1_SBK_SHIFT)) & UART_CTRL1_SBK_MASK)
#define UART_CTRL1_ONEWIRE_MASK                                               (0x4U)
#define UART_CTRL1_ONEWIRE_SHIFT                                              (2U)
#define UART_CTRL1_ONEWIRE(x)                                                 (((uint32_t)(((uint32_t)(x)) << UART_CTRL1_ONEWIRE_SHIFT)) & UART_CTRL1_ONEWIRE_MASK)
#define UART_CTRL1_PAREN_MASK                                                 (0x2U)
#define UART_CTRL1_PAREN_SHIFT                                                (1U)
#define UART_CTRL1_PAREN(x)                                                   (((uint32_t)(((uint32_t)(x)) << UART_CTRL1_PAREN_SHIFT)) & UART_CTRL1_PAREN_MASK)
#define UART_CTRL1_PARSEL_MASK                                                (0x1U)
#define UART_CTRL1_PARSEL_SHIFT                                               (0U)
#define UART_CTRL1_PARSEL(x)                                                  (((uint32_t)(((uint32_t)(x)) << UART_CTRL1_PARSEL_SHIFT)) & UART_CTRL1_PARSEL_MASK)
/* UART_INTF Register */
#define UART_INTF_IDLEWAKE_MASK                                               (0x20000U)
#define UART_INTF_IDLEWAKE_SHIFT                                              (17U)
#define UART_INTF_IDLEWAKE(x)                                                 (((uint32_t)(((uint32_t)(x)) << UART_INTF_IDLEWAKE_SHIFT)) & UART_INTF_IDLEWAKE_MASK)
#define UART_INTF_RXBUSY_MASK                                                 (0x10000U)
#define UART_INTF_RXBUSY_SHIFT                                                (16U)
#define UART_INTF_RXBUSY(x)                                                   (((uint32_t)(((uint32_t)(x)) << UART_INTF_RXBUSY_SHIFT)) & UART_INTF_RXBUSY_MASK)
#define UART_INTF_RFEIF_MASK                                                  (0x2000U)
#define UART_INTF_RFEIF_SHIFT                                                 (13U)
#define UART_INTF_RFEIF(x)                                                    (((uint32_t)(((uint32_t)(x)) << UART_INTF_RFEIF_SHIFT)) & UART_INTF_RFEIF_MASK)
#define UART_INTF_TFEIF_MASK                                                  (0x1000U)
#define UART_INTF_TFEIF_SHIFT                                                 (12U)
#define UART_INTF_TFEIF(x)                                                    (((uint32_t)(((uint32_t)(x)) << UART_INTF_TFEIF_SHIFT)) & UART_INTF_TFEIF_MASK)
#define UART_INTF_LBKDIF_MASK                                                 (0x800U)
#define UART_INTF_LBKDIF_SHIFT                                                (11U)
#define UART_INTF_LBKDIF(x)                                                   (((uint32_t)(((uint32_t)(x)) << UART_INTF_LBKDIF_SHIFT)) & UART_INTF_LBKDIF_MASK)
#define UART_INTF_RXEDGEIF_MASK                                               (0x400U)
#define UART_INTF_RXEDGEIF_SHIFT                                              (10U)
#define UART_INTF_RXEDGEIF(x)                                                 (((uint32_t)(((uint32_t)(x)) << UART_INTF_RXEDGEIF_SHIFT)) & UART_INTF_RXEDGEIF_MASK)
#define UART_INTF_MATIF_MASK                                                  (0x200U)
#define UART_INTF_MATIF_SHIFT                                                 (9U)
#define UART_INTF_MATIF(x)                                                    (((uint32_t)(((uint32_t)(x)) << UART_INTF_MATIF_SHIFT)) & UART_INTF_MATIF_MASK)
#define UART_INTF_NOZIF_MASK                                                  (0x80U)
#define UART_INTF_NOZIF_SHIFT                                                 (7U)
#define UART_INTF_NOZIF(x)                                                    (((uint32_t)(((uint32_t)(x)) << UART_INTF_NOZIF_SHIFT)) & UART_INTF_NOZIF_MASK)
#define UART_INTF_OVRIF_MASK                                                  (0x40U)
#define UART_INTF_OVRIF_SHIFT                                                 (6U)
#define UART_INTF_OVRIF(x)                                                    (((uint32_t)(((uint32_t)(x)) << UART_INTF_OVRIF_SHIFT)) & UART_INTF_OVRIF_MASK)
#define UART_INTF_FEIF_MASK                                                   (0x20U)
#define UART_INTF_FEIF_SHIFT                                                  (5U)
#define UART_INTF_FEIF(x)                                                     (((uint32_t)(((uint32_t)(x)) << UART_INTF_FEIF_SHIFT)) & UART_INTF_FEIF_MASK)
#define UART_INTF_IDLEIF_MASK                                                 (0x10U)
#define UART_INTF_IDLEIF_SHIFT                                                (4U)
#define UART_INTF_IDLEIF(x)                                                   (((uint32_t)(((uint32_t)(x)) << UART_INTF_IDLEIF_SHIFT)) & UART_INTF_IDLEIF_MASK)
#define UART_INTF_PARIF_MASK                                                  (0x8U)
#define UART_INTF_PARIF_SHIFT                                                 (3U)
#define UART_INTF_PARIF(x)                                                    (((uint32_t)(((uint32_t)(x)) << UART_INTF_PARIF_SHIFT)) & UART_INTF_PARIF_MASK)
#define UART_INTF_TCIF_MASK                                                   (0x4U)
#define UART_INTF_TCIF_SHIFT                                                  (2U)
#define UART_INTF_TCIF(x)                                                     (((uint32_t)(((uint32_t)(x)) << UART_INTF_TCIF_SHIFT)) & UART_INTF_TCIF_MASK)
#define UART_INTF_TXIF_MASK                                                   (0x2U)
#define UART_INTF_TXIF_SHIFT                                                  (1U)
#define UART_INTF_TXIF(x)                                                     (((uint32_t)(((uint32_t)(x)) << UART_INTF_TXIF_SHIFT)) & UART_INTF_TXIF_MASK)
#define UART_INTF_RXIF_MASK                                                   (0x1U)
#define UART_INTF_RXIF_SHIFT                                                  (0U)
#define UART_INTF_RXIF(x)                                                     (((uint32_t)(((uint32_t)(x)) << UART_INTF_RXIF_SHIFT)) & UART_INTF_RXIF_MASK)
/* UART_INTE Register */
#define UART_INTE_RFEIE_MASK                                                  (0x2000U)
#define UART_INTE_RFEIE_SHIFT                                                 (13U)
#define UART_INTE_RFEIE(x)                                                    (((uint32_t)(((uint32_t)(x)) << UART_INTE_RFEIE_SHIFT)) & UART_INTE_RFEIE_MASK)
#define UART_INTE_TFIFOEIE_MASK                                               (0x1000U)
#define UART_INTE_TFIFOEIE_SHIFT                                              (12U)
#define UART_INTE_TFIFOEIE(x)                                                 (((uint32_t)(((uint32_t)(x)) << UART_INTE_TFIFOEIE_SHIFT)) & UART_INTE_TFIFOEIE_MASK)
#define UART_INTE_LBKDIE_MASK                                                 (0x800U)
#define UART_INTE_LBKDIE_SHIFT                                                (11U)
#define UART_INTE_LBKDIE(x)                                                   (((uint32_t)(((uint32_t)(x)) << UART_INTE_LBKDIE_SHIFT)) & UART_INTE_LBKDIE_MASK)
#define UART_INTE_RXEDGEIE_MASK                                               (0x400U)
#define UART_INTE_RXEDGEIE_SHIFT                                              (10U)
#define UART_INTE_RXEDGEIE(x)                                                 (((uint32_t)(((uint32_t)(x)) << UART_INTE_RXEDGEIE_SHIFT)) & UART_INTE_RXEDGEIE_MASK)
#define UART_INTE_MATIE_MASK                                                  (0x200U)
#define UART_INTE_MATIE_SHIFT                                                 (9U)
#define UART_INTE_MATIE(x)                                                    (((uint32_t)(((uint32_t)(x)) << UART_INTE_MATIE_SHIFT)) & UART_INTE_MATIE_MASK)
#define UART_INTE_NOZIE_MASK                                                  (0x80U)
#define UART_INTE_NOZIE_SHIFT                                                 (7U)
#define UART_INTE_NOZIE(x)                                                    (((uint32_t)(((uint32_t)(x)) << UART_INTE_NOZIE_SHIFT)) & UART_INTE_NOZIE_MASK)
#define UART_INTE_OVRIE_MASK                                                  (0x40U)
#define UART_INTE_OVRIE_SHIFT                                                 (6U)
#define UART_INTE_OVRIE(x)                                                    (((uint32_t)(((uint32_t)(x)) << UART_INTE_OVRIE_SHIFT)) & UART_INTE_OVRIE_MASK)
#define UART_INTE_FEIE_MASK                                                   (0x20U)
#define UART_INTE_FEIE_SHIFT                                                  (5U)
#define UART_INTE_FEIE(x)                                                     (((uint32_t)(((uint32_t)(x)) << UART_INTE_FEIE_SHIFT)) & UART_INTE_FEIE_MASK)
#define UART_INTE_IDLEIE_MASK                                                 (0x10U)
#define UART_INTE_IDLEIE_SHIFT                                                (4U)
#define UART_INTE_IDLEIE(x)                                                   (((uint32_t)(((uint32_t)(x)) << UART_INTE_IDLEIE_SHIFT)) & UART_INTE_IDLEIE_MASK)
#define UART_INTE_PARIE_MASK                                                  (0x8U)
#define UART_INTE_PARIE_SHIFT                                                 (3U)
#define UART_INTE_PARIE(x)                                                    (((uint32_t)(((uint32_t)(x)) << UART_INTE_PARIE_SHIFT)) & UART_INTE_PARIE_MASK)
#define UART_INTE_TCIE_MASK                                                   (0x4U)
#define UART_INTE_TCIE_SHIFT                                                  (2U)
#define UART_INTE_TCIE(x)                                                     (((uint32_t)(((uint32_t)(x)) << UART_INTE_TCIE_SHIFT)) & UART_INTE_TCIE_MASK)
#define UART_INTE_TXIE_MASK                                                   (0x2U)
#define UART_INTE_TXIE_SHIFT                                                  (1U)
#define UART_INTE_TXIE(x)                                                     (((uint32_t)(((uint32_t)(x)) << UART_INTE_TXIE_SHIFT)) & UART_INTE_TXIE_MASK)
#define UART_INTE_RXIE_MASK                                                   (0x1U)
#define UART_INTE_RXIE_SHIFT                                                  (0U)
#define UART_INTE_RXIE(x)                                                     (((uint32_t)(((uint32_t)(x)) << UART_INTE_RXIE_SHIFT)) & UART_INTE_RXIE_MASK)
/* UART_MATCH Register */
#define UART_MATCH_DATAB_MASK                                                 (0x3FF0000U)
#define UART_MATCH_DATAB_SHIFT                                                (16U)
#define UART_MATCH_DATAB(x)                                                   (((uint32_t)(((uint32_t)(x)) << UART_MATCH_DATAB_SHIFT)) & UART_MATCH_DATAB_MASK)
#define UART_MATCH_DATAA_MASK                                                 (0x3FFU)
#define UART_MATCH_DATAA_SHIFT                                                (0U)
#define UART_MATCH_DATAA(x)                                                   (((uint32_t)(((uint32_t)(x)) << UART_MATCH_DATAA_SHIFT)) & UART_MATCH_DATAA_MASK)
/* UART_DATA Register */
#define UART_DATA_FERR_MASK                                                   (0x100000U)
#define UART_DATA_FERR_SHIFT                                                  (20U)
#define UART_DATA_FERR(x)                                                     (((uint32_t)(((uint32_t)(x)) << UART_DATA_FERR_SHIFT)) & UART_DATA_FERR_MASK)
#define UART_DATA_NOISE_MASK                                                  (0x80000U)
#define UART_DATA_NOISE_SHIFT                                                 (19U)
#define UART_DATA_NOISE(x)                                                    (((uint32_t)(((uint32_t)(x)) << UART_DATA_NOISE_SHIFT)) & UART_DATA_NOISE_MASK)
#define UART_DATA_PARERR_MASK                                                 (0x40000U)
#define UART_DATA_PARERR_SHIFT                                                (18U)
#define UART_DATA_PARERR(x)                                                   (((uint32_t)(((uint32_t)(x)) << UART_DATA_PARERR_SHIFT)) & UART_DATA_PARERR_MASK)
#define UART_DATA_IDLE_MASK                                                   (0x20000U)
#define UART_DATA_IDLE_SHIFT                                                  (17U)
#define UART_DATA_IDLE(x)                                                     (((uint32_t)(((uint32_t)(x)) << UART_DATA_IDLE_SHIFT)) & UART_DATA_IDLE_MASK)
#define UART_DATA_RXEMPT_MASK                                                 (0x10000U)
#define UART_DATA_RXEMPT_SHIFT                                                (16U)
#define UART_DATA_RXEMPT(x)                                                   (((uint32_t)(((uint32_t)(x)) << UART_DATA_RXEMPT_SHIFT)) & UART_DATA_RXEMPT_MASK)
#define UART_DATA_LINBRK_MASK                                                 (0x8000U)
#define UART_DATA_LINBRK_SHIFT                                                (15U)
#define UART_DATA_LINBRK(x)                                                   (((uint32_t)(((uint32_t)(x)) << UART_DATA_LINBRK_SHIFT)) & UART_DATA_LINBRK_MASK)
#define UART_DATA_DATA_MASK                                                   (0x3FFU)
#define UART_DATA_DATA_SHIFT                                                  (0U)
#define UART_DATA_DATA(x)                                                     (((uint32_t)(((uint32_t)(x)) << UART_DATA_DATA_SHIFT)) & UART_DATA_DATA_MASK)
/* UART_TXFIFO Register */
#define UART_TXFIFO_EMPTY_MASK                                                (0x80000000U)
#define UART_TXFIFO_EMPTY_SHIFT                                               (31U)
#define UART_TXFIFO_EMPTY(x)                                                  (((uint32_t)(((uint32_t)(x)) << UART_TXFIFO_EMPTY_SHIFT)) & UART_TXFIFO_EMPTY_MASK)
#define UART_TXFIFO_ERROR_MASK                                                (0x40000000U)
#define UART_TXFIFO_ERROR_SHIFT                                               (30U)
#define UART_TXFIFO_ERROR(x)                                                  (((uint32_t)(((uint32_t)(x)) << UART_TXFIFO_ERROR_SHIFT)) & UART_TXFIFO_ERROR_MASK)
#define UART_TXFIFO_SIZE_MASK                                                 (0x7000000U)
#define UART_TXFIFO_SIZE_SHIFT                                                (24U)
#define UART_TXFIFO_SIZE(x)                                                   (((uint32_t)(((uint32_t)(x)) << UART_TXFIFO_SIZE_SHIFT)) & UART_TXFIFO_SIZE_MASK)
#define UART_TXFIFO_RESET_MASK                                                (0x8000U)
#define UART_TXFIFO_RESET_SHIFT                                               (15U)
#define UART_TXFIFO_RESET(x)                                                  (((uint32_t)(((uint32_t)(x)) << UART_TXFIFO_RESET_SHIFT)) & UART_TXFIFO_RESET_MASK)
#define UART_TXFIFO_EN_MASK                                                   (0x4000U)
#define UART_TXFIFO_EN_SHIFT                                                  (14U)
#define UART_TXFIFO_EN(x)                                                     (((uint32_t)(((uint32_t)(x)) << UART_TXFIFO_EN_SHIFT)) & UART_TXFIFO_EN_MASK)
#define UART_TXFIFO_COUNT_MASK                                                (0x700U)
#define UART_TXFIFO_COUNT_SHIFT                                               (8U)
#define UART_TXFIFO_COUNT(x)                                                  (((uint32_t)(((uint32_t)(x)) << UART_TXFIFO_COUNT_SHIFT)) & UART_TXFIFO_COUNT_MASK)
#define UART_TXFIFO_WATER_MASK                                                (0x7U)
#define UART_TXFIFO_WATER_SHIFT                                               (0U)
#define UART_TXFIFO_WATER(x)                                                  (((uint32_t)(((uint32_t)(x)) << UART_TXFIFO_WATER_SHIFT)) & UART_TXFIFO_WATER_MASK)
/* UART_RXFIFO Register */
#define UART_RXFIFO_EMPTY_MASK                                                (0x80000000U)
#define UART_RXFIFO_EMPTY_SHIFT                                               (31U)
#define UART_RXFIFO_EMPTY(x)                                                  (((uint32_t)(((uint32_t)(x)) << UART_RXFIFO_EMPTY_SHIFT)) & UART_RXFIFO_EMPTY_MASK)
#define UART_RXFIFO_ERROR_MASK                                                (0x40000000U)
#define UART_RXFIFO_ERROR_SHIFT                                               (30U)
#define UART_RXFIFO_ERROR(x)                                                  (((uint32_t)(((uint32_t)(x)) << UART_RXFIFO_ERROR_SHIFT)) & UART_RXFIFO_ERROR_MASK)
#define UART_RXFIFO_SIZE_MASK                                                 (0x7000000U)
#define UART_RXFIFO_SIZE_SHIFT                                                (24U)
#define UART_RXFIFO_SIZE(x)                                                   (((uint32_t)(((uint32_t)(x)) << UART_RXFIFO_SIZE_SHIFT)) & UART_RXFIFO_SIZE_MASK)
#define UART_RXFIFO_RESET_MASK                                                (0x8000U)
#define UART_RXFIFO_RESET_SHIFT                                               (15U)
#define UART_RXFIFO_RESET(x)                                                  (((uint32_t)(((uint32_t)(x)) << UART_RXFIFO_RESET_SHIFT)) & UART_RXFIFO_RESET_MASK)
#define UART_RXFIFO_EN_MASK                                                   (0x4000U)
#define UART_RXFIFO_EN_SHIFT                                                  (14U)
#define UART_RXFIFO_EN(x)                                                     (((uint32_t)(((uint32_t)(x)) << UART_RXFIFO_EN_SHIFT)) & UART_RXFIFO_EN_MASK)
#define UART_RXFIFO_COUNT_MASK                                                (0x700U)
#define UART_RXFIFO_COUNT_SHIFT                                               (8U)
#define UART_RXFIFO_COUNT(x)                                                  (((uint32_t)(((uint32_t)(x)) << UART_RXFIFO_COUNT_SHIFT)) & UART_RXFIFO_COUNT_MASK)
#define UART_RXFIFO_WATER_MASK                                                (0x7U)
#define UART_RXFIFO_WATER_SHIFT                                               (0U)
#define UART_RXFIFO_WATER(x)                                                  (((uint32_t)(((uint32_t)(x)) << UART_RXFIFO_WATER_SHIFT)) & UART_RXFIFO_WATER_MASK)


/*!
 * @}
 */ /* end of group UART_Register_Masks */


/*!
 * @}
 */ /* end of group UART_Peripheral_Access_Layer */


/* -------------------------------------------------------------
   -- SPI Peripheral Access Layer
   ------------------------------------------------------------- */

/*!
 * @addtogroup SPI_Peripheral_Access_Layer SPI Peripheral Access Layer
 * @{
 */


/** SPI - Size of Registers Arrays */

/* SPI Register Layout Typedef */
typedef struct {
    __IO uint32_t CTRL;                                                       /**< Control Register, offset: 0x0000 */
    __IO uint32_t TXCFG;                                                      /**< Transmit Configuration Register, offset: 0x0004 */
    __IO uint32_t CLK;                                                        /**< Clock Configuration Register, offset: 0x0008 */
    __IO uint32_t STS;                                                        /**< Status Register, offset: 0x000C */
    __IO uint32_t INTE;                                                       /**< Interrupt Enable Register, offset: 0x0010 */
    __IO uint32_t MATCH0;                                                     /**< Match Data 0 Register, offset: 0x0014 */
    __IO uint32_t MATCH1;                                                     /**< Match Data 1 Register, offset: 0x0018 */
    __IO uint32_t TXFIFO;                                                     /**< TX FIFO Configuration Register, offset: 0x001C */
    __IO uint32_t RXFIFO;                                                     /**< RX FIFO Configuration Register, offset: 0x0020 */
    __IO uint32_t DATA;                                                       /**< Data Register, offset: 0x0024 */

} SPI_Type, *SPI_MemMapPtr;

/** Number of instances of the SPI module. */
#define SPI_INSTANCE_COUNT                                                    (3U)

/* SPI0  base address */
#define SPI0_BASE                                                             (0x4002A000U)
#define SPI0                                                                  ((SPI_Type *)(SPI0_BASE))
/*-------------------------- PPU PROTECTED REGISTER --------------------------- */
#define SPI0_MIRROR_BASE                                                      (0x4002A800U)
#define SPI0_MIRROR                                                           ((SPI_Type *)(SPI0_MIRROR_BASE))

#define SPI0_PPU_BASE                                                         (0x4002A000U)
#define SPI0_PPU                                                              ((PPU_Type *)(SPI0_BASE))
/*-------------------------- PPU PROTECTED REGISTER --------------------------- */

/* SPI1  base address */
#define SPI1_BASE                                                             (0x4002B000U)
#define SPI1                                                                  ((SPI_Type *)(SPI1_BASE))
/*-------------------------- PPU PROTECTED REGISTER --------------------------- */
#define SPI1_MIRROR_BASE                                                      (0x4002B800U)
#define SPI1_MIRROR                                                           ((SPI_Type *)(SPI1_MIRROR_BASE))

#define SPI1_PPU_BASE                                                         (0x4002B000U)
#define SPI1_PPU                                                              ((PPU_Type *)(SPI1_BASE))
/*-------------------------- PPU PROTECTED REGISTER --------------------------- */

/* SPI2  base address */
#define SPI2_BASE                                                             (0x4002C000U)
#define SPI2                                                                  ((SPI_Type *)(SPI2_BASE))
/*-------------------------- PPU PROTECTED REGISTER --------------------------- */
#define SPI2_MIRROR_BASE                                                      (0x4002C800U)
#define SPI2_MIRROR                                                           ((SPI_Type *)(SPI2_MIRROR_BASE))

#define SPI2_PPU_BASE                                                         (0x4002C000U)
#define SPI2_PPU                                                              ((PPU_Type *)(SPI2_BASE))
/*-------------------------- PPU PROTECTED REGISTER --------------------------- */


/** Array initializer of SPI peripheral base addresses */
#define SPI_BASE_ADDRS                                                        { SPI0_BASE,  SPI1_BASE,  SPI2_BASE }
/** Array initializer of SPI peripheral base pointers */
#define SPI_BASE_PTRS                                                         { SPI0,  SPI1,  SPI2 }
/** Number of interrupt channels for the SPI. */
#define SPI_IRQS_CH_COUNT                                                     (1U)
/** Interrupt vectors for the SPI peripheral type */
#define SPI_IRQS                                                              { SPI0_IRQn, SPI1_IRQn, SPI2_IRQn }

/* SPI Register Mask */
/* SPI_CTRL Register */
#define SPI_CTRL_FMDMAEN_MASK                                                 (0x40000000U)
#define SPI_CTRL_FMDMAEN_SHIFT                                                (30U)
#define SPI_CTRL_FMDMAEN(x)                                                   (((uint32_t)(((uint32_t)(x)) << SPI_CTRL_FMDMAEN_SHIFT)) & SPI_CTRL_FMDMAEN_MASK)
#define SPI_CTRL_TXDMAEN_MASK                                                 (0x20000000U)
#define SPI_CTRL_TXDMAEN_SHIFT                                                (29U)
#define SPI_CTRL_TXDMAEN(x)                                                   (((uint32_t)(((uint32_t)(x)) << SPI_CTRL_TXDMAEN_SHIFT)) & SPI_CTRL_TXDMAEN_MASK)
#define SPI_CTRL_RXDMAEN_MASK                                                 (0x10000000U)
#define SPI_CTRL_RXDMAEN_SHIFT                                                (28U)
#define SPI_CTRL_RXDMAEN(x)                                                   (((uint32_t)(((uint32_t)(x)) << SPI_CTRL_RXDMAEN_SHIFT)) & SPI_CTRL_RXDMAEN_MASK)
#define SPI_CTRL_AUTOPCS_MASK                                                 (0x2000000U)
#define SPI_CTRL_AUTOPCS_SHIFT                                                (25U)
#define SPI_CTRL_AUTOPCS(x)                                                   (((uint32_t)(((uint32_t)(x)) << SPI_CTRL_AUTOPCS_SHIFT)) & SPI_CTRL_AUTOPCS_MASK)
#define SPI_CTRL_SPDEN_MASK                                                   (0x1000000U)
#define SPI_CTRL_SPDEN_SHIFT                                                  (24U)
#define SPI_CTRL_SPDEN(x)                                                     (((uint32_t)(((uint32_t)(x)) << SPI_CTRL_SPDEN_SHIFT)) & SPI_CTRL_SPDEN_MASK)
#define SPI_CTRL_MANPCSEN_MASK                                                (0x400000U)
#define SPI_CTRL_MANPCSEN_SHIFT                                               (22U)
#define SPI_CTRL_MANPCSEN(x)                                                  (((uint32_t)(((uint32_t)(x)) << SPI_CTRL_MANPCSEN_SHIFT)) & SPI_CTRL_MANPCSEN_MASK)
#define SPI_CTRL_MANPCSDAT_MASK                                               (0x200000U)
#define SPI_CTRL_MANPCSDAT_SHIFT                                              (21U)
#define SPI_CTRL_MANPCSDAT(x)                                                 (((uint32_t)(((uint32_t)(x)) << SPI_CTRL_MANPCSDAT_SHIFT)) & SPI_CTRL_MANPCSDAT_MASK)
#define SPI_CTRL_I2SMOD_MASK                                                  (0x100000U)
#define SPI_CTRL_I2SMOD_SHIFT                                                 (20U)
#define SPI_CTRL_I2SMOD(x)                                                    (((uint32_t)(((uint32_t)(x)) << SPI_CTRL_I2SMOD_SHIFT)) & SPI_CTRL_I2SMOD_MASK)
#define SPI_CTRL_MATCFG_MASK                                                  (0x70000U)
#define SPI_CTRL_MATCFG_SHIFT                                                 (16U)
#define SPI_CTRL_MATCFG(x)                                                    (((uint32_t)(((uint32_t)(x)) << SPI_CTRL_MATCFG_SHIFT)) & SPI_CTRL_MATCFG_MASK)
#define SPI_CTRL_CSPOL_MASK                                                   (0xF00U)
#define SPI_CTRL_CSPOL_SHIFT                                                  (8U)
#define SPI_CTRL_CSPOL(x)                                                     (((uint32_t)(((uint32_t)(x)) << SPI_CTRL_CSPOL_SHIFT)) & SPI_CTRL_CSPOL_MASK)
#define SPI_CTRL_CSDEN_MASK                                                   (0x80U)
#define SPI_CTRL_CSDEN_SHIFT                                                  (7U)
#define SPI_CTRL_CSDEN(x)                                                     (((uint32_t)(((uint32_t)(x)) << SPI_CTRL_CSDEN_SHIFT)) & SPI_CTRL_CSDEN_MASK)
#define SPI_CTRL_DHZEN_MASK                                                   (0x40U)
#define SPI_CTRL_DHZEN_SHIFT                                                  (6U)
#define SPI_CTRL_DHZEN(x)                                                     (((uint32_t)(((uint32_t)(x)) << SPI_CTRL_DHZEN_SHIFT)) & SPI_CTRL_DHZEN_MASK)
#define SPI_CTRL_PINCFG_MASK                                                  (0x30U)
#define SPI_CTRL_PINCFG_SHIFT                                                 (4U)
#define SPI_CTRL_PINCFG(x)                                                    (((uint32_t)(((uint32_t)(x)) << SPI_CTRL_PINCFG_SHIFT)) & SPI_CTRL_PINCFG_MASK)
#define SPI_CTRL_RXMO_MASK                                                    (0x8U)
#define SPI_CTRL_RXMO_SHIFT                                                   (3U)
#define SPI_CTRL_RXMO(x)                                                      (((uint32_t)(((uint32_t)(x)) << SPI_CTRL_RXMO_SHIFT)) & SPI_CTRL_RXMO_MASK)
#define SPI_CTRL_MODE_MASK                                                    (0x2U)
#define SPI_CTRL_MODE_SHIFT                                                   (1U)
#define SPI_CTRL_MODE(x)                                                      (((uint32_t)(((uint32_t)(x)) << SPI_CTRL_MODE_SHIFT)) & SPI_CTRL_MODE_MASK)
#define SPI_CTRL_EN_MASK                                                      (0x1U)
#define SPI_CTRL_EN_SHIFT                                                     (0U)
#define SPI_CTRL_EN(x)                                                        (((uint32_t)(((uint32_t)(x)) << SPI_CTRL_EN_SHIFT)) & SPI_CTRL_EN_MASK)
/* SPI_TXCFG Register */
#define SPI_TXCFG_CPOL_MASK                                                   (0x80000000U)
#define SPI_TXCFG_CPOL_SHIFT                                                  (31U)
#define SPI_TXCFG_CPOL(x)                                                     (((uint32_t)(((uint32_t)(x)) << SPI_TXCFG_CPOL_SHIFT)) & SPI_TXCFG_CPOL_MASK)
#define SPI_TXCFG_CPHA_MASK                                                   (0x40000000U)
#define SPI_TXCFG_CPHA_SHIFT                                                  (30U)
#define SPI_TXCFG_CPHA(x)                                                     (((uint32_t)(((uint32_t)(x)) << SPI_TXCFG_CPHA_SHIFT)) & SPI_TXCFG_CPHA_MASK)
#define SPI_TXCFG_PRESCALE_MASK                                               (0x38000000U)
#define SPI_TXCFG_PRESCALE_SHIFT                                              (27U)
#define SPI_TXCFG_PRESCALE(x)                                                 (((uint32_t)(((uint32_t)(x)) << SPI_TXCFG_PRESCALE_SHIFT)) & SPI_TXCFG_PRESCALE_MASK)
#define SPI_TXCFG_PCS_MASK                                                    (0x7000000U)
#define SPI_TXCFG_PCS_SHIFT                                                   (24U)
#define SPI_TXCFG_PCS(x)                                                      (((uint32_t)(((uint32_t)(x)) << SPI_TXCFG_PCS_SHIFT)) & SPI_TXCFG_PCS_MASK)
#define SPI_TXCFG_LSBF_MASK                                                   (0x800000U)
#define SPI_TXCFG_LSBF_SHIFT                                                  (23U)
#define SPI_TXCFG_LSBF(x)                                                     (((uint32_t)(((uint32_t)(x)) << SPI_TXCFG_LSBF_SHIFT)) & SPI_TXCFG_LSBF_MASK)
#define SPI_TXCFG_BSW_MASK                                                    (0x400000U)
#define SPI_TXCFG_BSW_SHIFT                                                   (22U)
#define SPI_TXCFG_BSW(x)                                                      (((uint32_t)(((uint32_t)(x)) << SPI_TXCFG_BSW_SHIFT)) & SPI_TXCFG_BSW_MASK)
#define SPI_TXCFG_CONT_MASK                                                   (0x200000U)
#define SPI_TXCFG_CONT_SHIFT                                                  (21U)
#define SPI_TXCFG_CONT(x)                                                     (((uint32_t)(((uint32_t)(x)) << SPI_TXCFG_CONT_SHIFT)) & SPI_TXCFG_CONT_MASK)
#define SPI_TXCFG_CONTC_MASK                                                  (0x100000U)
#define SPI_TXCFG_CONTC_SHIFT                                                 (20U)
#define SPI_TXCFG_CONTC(x)                                                    (((uint32_t)(((uint32_t)(x)) << SPI_TXCFG_CONTC_SHIFT)) & SPI_TXCFG_CONTC_MASK)
#define SPI_TXCFG_MSKRX_MASK                                                  (0x80000U)
#define SPI_TXCFG_MSKRX_SHIFT                                                 (19U)
#define SPI_TXCFG_MSKRX(x)                                                    (((uint32_t)(((uint32_t)(x)) << SPI_TXCFG_MSKRX_SHIFT)) & SPI_TXCFG_MSKRX_MASK)
#define SPI_TXCFG_MSKTX_MASK                                                  (0x40000U)
#define SPI_TXCFG_MSKTX_SHIFT                                                 (18U)
#define SPI_TXCFG_MSKTX(x)                                                    (((uint32_t)(((uint32_t)(x)) << SPI_TXCFG_MSKTX_SHIFT)) & SPI_TXCFG_MSKTX_MASK)
#define SPI_TXCFG_WIDTH_MASK                                                  (0x30000U)
#define SPI_TXCFG_WIDTH_SHIFT                                                 (16U)
#define SPI_TXCFG_WIDTH(x)                                                    (((uint32_t)(((uint32_t)(x)) << SPI_TXCFG_WIDTH_SHIFT)) & SPI_TXCFG_WIDTH_MASK)
#define SPI_TXCFG_FRAMESZ_WIDTH                                               (12)
#define SPI_TXCFG_FRAMESZ_MASK                                                (0xFFFU)
#define SPI_TXCFG_FRAMESZ_SHIFT                                               (0U)
#define SPI_TXCFG_FRAMESZ(x)                                                  (((uint32_t)(((uint32_t)(x)) << SPI_TXCFG_FRAMESZ_SHIFT)) & SPI_TXCFG_FRAMESZ_MASK)
/* SPI_CLK Register */
#define SPI_CLK_SCKPCS_WIDTH                                                  (8)
#define SPI_CLK_SCKPCS_MASK                                                   (0xFF000000U)
#define SPI_CLK_SCKPCS_SHIFT                                                  (24U)
#define SPI_CLK_SCKPCS(x)                                                     (((uint32_t)(((uint32_t)(x)) << SPI_CLK_SCKPCS_SHIFT)) & SPI_CLK_SCKPCS_MASK)
#define SPI_CLK_PCSSCK_WIDTH                                                  (8)
#define SPI_CLK_PCSSCK_MASK                                                   (0xFF0000U)
#define SPI_CLK_PCSSCK_SHIFT                                                  (16U)
#define SPI_CLK_PCSSCK(x)                                                     (((uint32_t)(((uint32_t)(x)) << SPI_CLK_PCSSCK_SHIFT)) & SPI_CLK_PCSSCK_MASK)
#define SPI_CLK_FMDLY_WIDTH                                                   (8)
#define SPI_CLK_FMDLY_MASK                                                    (0xFF00U)
#define SPI_CLK_FMDLY_SHIFT                                                   (8U)
#define SPI_CLK_FMDLY(x)                                                      (((uint32_t)(((uint32_t)(x)) << SPI_CLK_FMDLY_SHIFT)) & SPI_CLK_FMDLY_MASK)
#define SPI_CLK_DIV_MASK                                                      (0xFFU)
#define SPI_CLK_DIV_SHIFT                                                     (0U)
#define SPI_CLK_DIV(x)                                                        (((uint32_t)(((uint32_t)(x)) << SPI_CLK_DIV_SHIFT)) & SPI_CLK_DIV_MASK)
/* SPI_STS Register */
#define SPI_STS_BUSY_MASK                                                     (0x40000U)
#define SPI_STS_BUSY_SHIFT                                                    (18U)
#define SPI_STS_BUSY(x)                                                       (((uint32_t)(((uint32_t)(x)) << SPI_STS_BUSY_SHIFT)) & SPI_STS_BUSY_MASK)
#define SPI_STS_RXEPT_MASK                                                    (0x20000U)
#define SPI_STS_RXEPT_SHIFT                                                   (17U)
#define SPI_STS_RXEPT(x)                                                      (((uint32_t)(((uint32_t)(x)) << SPI_STS_RXEPT_SHIFT)) & SPI_STS_RXEPT_MASK)
#define SPI_STS_SOF_MASK                                                      (0x10000U)
#define SPI_STS_SOF_SHIFT                                                     (16U)
#define SPI_STS_SOF(x)                                                        (((uint32_t)(((uint32_t)(x)) << SPI_STS_SOF_SHIFT)) & SPI_STS_SOF_MASK)
#define SPI_STS_MATIF_MASK                                                    (0x80U)
#define SPI_STS_MATIF_SHIFT                                                   (7U)
#define SPI_STS_MATIF(x)                                                      (((uint32_t)(((uint32_t)(x)) << SPI_STS_MATIF_SHIFT)) & SPI_STS_MATIF_MASK)
#define SPI_STS_RXOVIF_MASK                                                   (0x40U)
#define SPI_STS_RXOVIF_SHIFT                                                  (6U)
#define SPI_STS_RXOVIF(x)                                                     (((uint32_t)(((uint32_t)(x)) << SPI_STS_RXOVIF_SHIFT)) & SPI_STS_RXOVIF_MASK)
#define SPI_STS_TXUNIF_MASK                                                   (0x20U)
#define SPI_STS_TXUNIF_SHIFT                                                  (5U)
#define SPI_STS_TXUNIF(x)                                                     (((uint32_t)(((uint32_t)(x)) << SPI_STS_TXUNIF_SHIFT)) & SPI_STS_TXUNIF_MASK)
#define SPI_STS_TCIF_MASK                                                     (0x10U)
#define SPI_STS_TCIF_SHIFT                                                    (4U)
#define SPI_STS_TCIF(x)                                                       (((uint32_t)(((uint32_t)(x)) << SPI_STS_TCIF_SHIFT)) & SPI_STS_TCIF_MASK)
#define SPI_STS_FCIF_MASK                                                     (0x8U)
#define SPI_STS_FCIF_SHIFT                                                    (3U)
#define SPI_STS_FCIF(x)                                                       (((uint32_t)(((uint32_t)(x)) << SPI_STS_FCIF_SHIFT)) & SPI_STS_FCIF_MASK)
#define SPI_STS_WCIF_MASK                                                     (0x4U)
#define SPI_STS_WCIF_SHIFT                                                    (2U)
#define SPI_STS_WCIF(x)                                                       (((uint32_t)(((uint32_t)(x)) << SPI_STS_WCIF_SHIFT)) & SPI_STS_WCIF_MASK)
#define SPI_STS_TXIF_MASK                                                     (0x2U)
#define SPI_STS_TXIF_SHIFT                                                    (1U)
#define SPI_STS_TXIF(x)                                                       (((uint32_t)(((uint32_t)(x)) << SPI_STS_TXIF_SHIFT)) & SPI_STS_TXIF_MASK)
#define SPI_STS_RXIF_MASK                                                     (0x1U)
#define SPI_STS_RXIF_SHIFT                                                    (0U)
#define SPI_STS_RXIF(x)                                                       (((uint32_t)(((uint32_t)(x)) << SPI_STS_RXIF_SHIFT)) & SPI_STS_RXIF_MASK)
/* SPI_INTE Register */
#define SPI_INTE_MATIE_MASK                                                   (0x80U)
#define SPI_INTE_MATIE_SHIFT                                                  (7U)
#define SPI_INTE_MATIE(x)                                                     (((uint32_t)(((uint32_t)(x)) << SPI_INTE_MATIE_SHIFT)) & SPI_INTE_MATIE_MASK)
#define SPI_INTE_RXOVIE_MASK                                                  (0x40U)
#define SPI_INTE_RXOVIE_SHIFT                                                 (6U)
#define SPI_INTE_RXOVIE(x)                                                    (((uint32_t)(((uint32_t)(x)) << SPI_INTE_RXOVIE_SHIFT)) & SPI_INTE_RXOVIE_MASK)
#define SPI_INTE_TXUNIE_MASK                                                  (0x20U)
#define SPI_INTE_TXUNIE_SHIFT                                                 (5U)
#define SPI_INTE_TXUNIE(x)                                                    (((uint32_t)(((uint32_t)(x)) << SPI_INTE_TXUNIE_SHIFT)) & SPI_INTE_TXUNIE_MASK)
#define SPI_INTE_TCIE_MASK                                                    (0x10U)
#define SPI_INTE_TCIE_SHIFT                                                   (4U)
#define SPI_INTE_TCIE(x)                                                      (((uint32_t)(((uint32_t)(x)) << SPI_INTE_TCIE_SHIFT)) & SPI_INTE_TCIE_MASK)
#define SPI_INTE_FCIE_MASK                                                    (0x8U)
#define SPI_INTE_FCIE_SHIFT                                                   (3U)
#define SPI_INTE_FCIE(x)                                                      (((uint32_t)(((uint32_t)(x)) << SPI_INTE_FCIE_SHIFT)) & SPI_INTE_FCIE_MASK)
#define SPI_INTE_WCIE_MASK                                                    (0x4U)
#define SPI_INTE_WCIE_SHIFT                                                   (2U)
#define SPI_INTE_WCIE(x)                                                      (((uint32_t)(((uint32_t)(x)) << SPI_INTE_WCIE_SHIFT)) & SPI_INTE_WCIE_MASK)
#define SPI_INTE_TXIE_MASK                                                    (0x2U)
#define SPI_INTE_TXIE_SHIFT                                                   (1U)
#define SPI_INTE_TXIE(x)                                                      (((uint32_t)(((uint32_t)(x)) << SPI_INTE_TXIE_SHIFT)) & SPI_INTE_TXIE_MASK)
#define SPI_INTE_RXIE_MASK                                                    (0x1U)
#define SPI_INTE_RXIE_SHIFT                                                   (0U)
#define SPI_INTE_RXIE(x)                                                      (((uint32_t)(((uint32_t)(x)) << SPI_INTE_RXIE_SHIFT)) & SPI_INTE_RXIE_MASK)
/* SPI_MATCH0 Register */
#define SPI_MATCH0_MATCH0_MASK                                                (0xFFFFFFFFU)
#define SPI_MATCH0_MATCH0_SHIFT                                               (0U)
#define SPI_MATCH0_MATCH0(x)                                                  (((uint32_t)(((uint32_t)(x)) << SPI_MATCH0_MATCH0_SHIFT)) & SPI_MATCH0_MATCH0_MASK)
/* SPI_MATCH1 Register */
#define SPI_MATCH1_MATCH1_MASK                                                (0xFFFFFFFFU)
#define SPI_MATCH1_MATCH1_SHIFT                                               (0U)
#define SPI_MATCH1_MATCH1(x)                                                  (((uint32_t)(((uint32_t)(x)) << SPI_MATCH1_MATCH1_SHIFT)) & SPI_MATCH1_MATCH1_MASK)
/* SPI_TXFIFO Register */
#define SPI_TXFIFO_SIZE_MASK                                                  (0xF000000U)
#define SPI_TXFIFO_SIZE_SHIFT                                                 (24U)
#define SPI_TXFIFO_SIZE(x)                                                    (((uint32_t)(((uint32_t)(x)) << SPI_TXFIFO_SIZE_SHIFT)) & SPI_TXFIFO_SIZE_MASK)
#define SPI_TXFIFO_RESET_MASK                                                 (0x8000U)
#define SPI_TXFIFO_RESET_SHIFT                                                (15U)
#define SPI_TXFIFO_RESET(x)                                                   (((uint32_t)(((uint32_t)(x)) << SPI_TXFIFO_RESET_SHIFT)) & SPI_TXFIFO_RESET_MASK)
#define SPI_TXFIFO_COUNT_MASK                                                 (0x700U)
#define SPI_TXFIFO_COUNT_SHIFT                                                (8U)
#define SPI_TXFIFO_COUNT(x)                                                   (((uint32_t)(((uint32_t)(x)) << SPI_TXFIFO_COUNT_SHIFT)) & SPI_TXFIFO_COUNT_MASK)
#define SPI_TXFIFO_WATER_MASK                                                 (0x7U)
#define SPI_TXFIFO_WATER_SHIFT                                                (0U)
#define SPI_TXFIFO_WATER(x)                                                   (((uint32_t)(((uint32_t)(x)) << SPI_TXFIFO_WATER_SHIFT)) & SPI_TXFIFO_WATER_MASK)
/* SPI_RXFIFO Register */
#define SPI_RXFIFO_SIZE_MASK                                                  (0xF000000U)
#define SPI_RXFIFO_SIZE_SHIFT                                                 (24U)
#define SPI_RXFIFO_SIZE(x)                                                    (((uint32_t)(((uint32_t)(x)) << SPI_RXFIFO_SIZE_SHIFT)) & SPI_RXFIFO_SIZE_MASK)
#define SPI_RXFIFO_RESET_MASK                                                 (0x8000U)
#define SPI_RXFIFO_RESET_SHIFT                                                (15U)
#define SPI_RXFIFO_RESET(x)                                                   (((uint32_t)(((uint32_t)(x)) << SPI_RXFIFO_RESET_SHIFT)) & SPI_RXFIFO_RESET_MASK)
#define SPI_RXFIFO_COUNT_MASK                                                 (0x700U)
#define SPI_RXFIFO_COUNT_SHIFT                                                (8U)
#define SPI_RXFIFO_COUNT(x)                                                   (((uint32_t)(((uint32_t)(x)) << SPI_RXFIFO_COUNT_SHIFT)) & SPI_RXFIFO_COUNT_MASK)
#define SPI_RXFIFO_WATER_MASK                                                 (0x7U)
#define SPI_RXFIFO_WATER_SHIFT                                                (0U)
#define SPI_RXFIFO_WATER(x)                                                   (((uint32_t)(((uint32_t)(x)) << SPI_RXFIFO_WATER_SHIFT)) & SPI_RXFIFO_WATER_MASK)
/* SPI_DATA Register */
#define SPI_DATA_DATA_MASK                                                    (0xFFFFFFFFU)
#define SPI_DATA_DATA_SHIFT                                                   (0U)
#define SPI_DATA_DATA(x)                                                      (((uint32_t)(((uint32_t)(x)) << SPI_DATA_DATA_SHIFT)) & SPI_DATA_DATA_MASK)


/*!
 * @}
 */ /* end of group SPI_Register_Masks */


/*!
 * @}
 */ /* end of group SPI_Peripheral_Access_Layer */


/* -------------------------------------------------------------
   -- I2C Peripheral Access Layer
   ------------------------------------------------------------- */

/*!
 * @addtogroup I2C_Peripheral_Access_Layer I2C Peripheral Access Layer
 * @{
 */


/** I2C - Size of Registers Arrays */

/* I2C Register Layout Typedef */
typedef struct {
    __IO uint32_t MCTRL;                                                      /**< Master Control Register, offset: 0x0000 */
    __IO uint32_t MCLKCFG;                                                    /**< Master Clock Configuration Register, offset: 0x0004 */
    __IO uint32_t MFLTCFG;                                                    /**< Master Clock Filter Configuration and Divider Register, offset: 0x0008 */
    __IO uint32_t MSTS;                                                       /**< Master Status Register, offset: 0x000C */
    __IO uint32_t MIE;                                                        /**< Master Interrupt Register, offset: 0x0010 */
    __IO uint32_t TXFIFO;                                                     /**< Master TX FIFO Control and Status Register, offset: 0x0014 */
    __IO uint32_t RXFIFO;                                                     /**< Master RX FIFO Control and Status Register, offset: 0x0018 */
    __IO uint32_t MDATA;                                                      /**< Master Data Register, offset: 0x001C */
    __IO uint32_t MATCH;                                                      /**< Data Match Register, offset: 0x0020 */
    __IO uint32_t TOCFG;                                                      /**< Timeout Configure Register, offset: 0x0024 */
    __IO uint32_t HSCLK;                                                      /**< Master Clock Configuration Under High Speed Mode Register, offset: 0x0028 */
    __IO uint8_t  RESERVED11[212];                                            /**< RESERVED11, offset: 0x002C */
    __IO uint32_t SCTRL;                                                      /**< Slave Control Register, offset: 0x0100 */
    __IO uint32_t SCLKCFG;                                                    /**< Slave Clock Configuration Register, offset: 0x0104 */
    __IO uint32_t SFLTCFG;                                                    /**< Slave Filter Configuration Register, offset: 0x0108 */
    __IO uint32_t SSTS;                                                       /**< Slave Status Register, offset: 0x010C */
    __IO uint32_t SIE;                                                        /**< Slave Interrupt Enable Register, offset: 0x0110 */
    __IO uint32_t SDATA;                                                      /**< Slave Data Register, offset: 0x0114 */
    __IO uint32_t SACK;                                                       /**< Slave Ack Register, offset: 0x0118 */
    __IO uint32_t SADDR;                                                      /**< Slave Address Match Configuration Register, offset: 0x011C */
    __I  uint32_t SRADDR;                                                     /**< Slave Received Address Register, offset: 0x0120 */

} I2C_Type, *I2C_MemMapPtr;

/** Number of instances of the I2C module. */
#define I2C_INSTANCE_COUNT                                                    (2U)

/* I2C0  base address */
#define I2C0_BASE                                                             (0x40023000U)
#define I2C0                                                                  ((I2C_Type *)(I2C0_BASE))
/* I2C1  base address */
#define I2C1_BASE                                                             (0x40024000U)
#define I2C1                                                                  ((I2C_Type *)(I2C1_BASE))

/** Array initializer of I2C peripheral base addresses */
#define I2C_BASE_ADDRS                                                        { I2C0_BASE,  I2C1_BASE }
/** Array initializer of I2C peripheral base pointers */
#define I2C_BASE_PTRS                                                         { I2C0,  I2C1 }
/** Number of interrupt channels for the MASTER type of I2C. */
#define I2C_MASTER_IRQS_CH_COUNT                                              (2U)
#define I2C_MASTER_IRQS                                                       { I2C0_Master_IRQn, I2C1_Master_IRQn }
/** Number of interrupt channels for the SLAVE type of I2C. */
#define I2C_SLAVE_IRQS_CH_COUNT                                               (1U)
#define I2C_SLAVE_IRQS                                                        { I2C0_Slave_IRQn }

/* I2C Register Mask */
/* I2C_MCTRL Register */
#define I2C_MCTRL_TXDMAEN_MASK                                                (0x20000000U)
#define I2C_MCTRL_TXDMAEN_SHIFT                                               (29U)
#define I2C_MCTRL_TXDMAEN(x)                                                  (((uint32_t)(((uint32_t)(x)) << I2C_MCTRL_TXDMAEN_SHIFT)) & I2C_MCTRL_TXDMAEN_MASK)
#define I2C_MCTRL_RXDMAEN_MASK                                                (0x10000000U)
#define I2C_MCTRL_RXDMAEN_SHIFT                                               (28U)
#define I2C_MCTRL_RXDMAEN(x)                                                  (((uint32_t)(((uint32_t)(x)) << I2C_MCTRL_RXDMAEN_SHIFT)) & I2C_MCTRL_RXDMAEN_MASK)
#define I2C_MCTRL_RDMO_MASK                                                   (0x2000000U)
#define I2C_MCTRL_RDMO_SHIFT                                                  (25U)
#define I2C_MCTRL_RDMO(x)                                                     (((uint32_t)(((uint32_t)(x)) << I2C_MCTRL_RDMO_SHIFT)) & I2C_MCTRL_RDMO_MASK)
#define I2C_MCTRL_MATCFG_MASK                                                 (0x70000U)
#define I2C_MCTRL_MATCFG_SHIFT                                                (16U)
#define I2C_MCTRL_MATCFG(x)                                                   (((uint32_t)(((uint32_t)(x)) << I2C_MCTRL_MATCFG_SHIFT)) & I2C_MCTRL_MATCFG_MASK)
#define I2C_MCTRL_IGACK_MASK                                                  (0x8000U)
#define I2C_MCTRL_IGACK_SHIFT                                                 (15U)
#define I2C_MCTRL_IGACK(x)                                                    (((uint32_t)(((uint32_t)(x)) << I2C_MCTRL_IGACK_SHIFT)) & I2C_MCTRL_IGACK_MASK)
#define I2C_MCTRL_ASTOP_MASK                                                  (0x4000U)
#define I2C_MCTRL_ASTOP_SHIFT                                                 (14U)
#define I2C_MCTRL_ASTOP(x)                                                    (((uint32_t)(((uint32_t)(x)) << I2C_MCTRL_ASTOP_SHIFT)) & I2C_MCTRL_ASTOP_MASK)
#define I2C_MCTRL_HSMOD_MASK                                                  (0x200U)
#define I2C_MCTRL_HSMOD_SHIFT                                                 (9U)
#define I2C_MCTRL_HSMOD(x)                                                    (((uint32_t)(((uint32_t)(x)) << I2C_MCTRL_HSMOD_SHIFT)) & I2C_MCTRL_HSMOD_MASK)
#define I2C_MCTRL_PPEN_MASK                                                   (0x100U)
#define I2C_MCTRL_PPEN_SHIFT                                                  (8U)
#define I2C_MCTRL_PPEN(x)                                                     (((uint32_t)(((uint32_t)(x)) << I2C_MCTRL_PPEN_SHIFT)) & I2C_MCTRL_PPEN_MASK)
#define I2C_MCTRL_MEN_MASK                                                    (0x1U)
#define I2C_MCTRL_MEN_SHIFT                                                   (0U)
#define I2C_MCTRL_MEN(x)                                                      (((uint32_t)(((uint32_t)(x)) << I2C_MCTRL_MEN_SHIFT)) & I2C_MCTRL_MEN_MASK)
/* I2C_MCLKCFG Register */
#define I2C_MCLKCFG_HIGH_WIDTH                                                (6)
#define I2C_MCLKCFG_HIGH_MASK                                                 (0x3F000000U)
#define I2C_MCLKCFG_HIGH_SHIFT                                                (24U)
#define I2C_MCLKCFG_HIGH(x)                                                   (((uint32_t)(((uint32_t)(x)) << I2C_MCLKCFG_HIGH_SHIFT)) & I2C_MCLKCFG_HIGH_MASK)
#define I2C_MCLKCFG_LOW_WIDTH                                                 (6)
#define I2C_MCLKCFG_LOW_MASK                                                  (0x3F0000U)
#define I2C_MCLKCFG_LOW_SHIFT                                                 (16U)
#define I2C_MCLKCFG_LOW(x)                                                    (((uint32_t)(((uint32_t)(x)) << I2C_MCLKCFG_LOW_SHIFT)) & I2C_MCLKCFG_LOW_MASK)
#define I2C_MCLKCFG_VALID_WIDTH                                               (6)
#define I2C_MCLKCFG_VALID_MASK                                                (0x3F00U)
#define I2C_MCLKCFG_VALID_SHIFT                                               (8U)
#define I2C_MCLKCFG_VALID(x)                                                  (((uint32_t)(((uint32_t)(x)) << I2C_MCLKCFG_VALID_SHIFT)) & I2C_MCLKCFG_VALID_MASK)
#define I2C_MCLKCFG_STHD_WIDTH                                                (6)
#define I2C_MCLKCFG_STHD_MASK                                                 (0x3FU)
#define I2C_MCLKCFG_STHD_SHIFT                                                (0U)
#define I2C_MCLKCFG_STHD(x)                                                   (((uint32_t)(((uint32_t)(x)) << I2C_MCLKCFG_STHD_SHIFT)) & I2C_MCLKCFG_STHD_MASK)
/* I2C_MFLTCFG Register */
#define I2C_MFLTCFG_FLTSCL_WIDTH                                              (4)
#define I2C_MFLTCFG_FLTSCL_MASK                                               (0xF000000U)
#define I2C_MFLTCFG_FLTSCL_SHIFT                                              (24U)
#define I2C_MFLTCFG_FLTSCL(x)                                                 (((uint32_t)(((uint32_t)(x)) << I2C_MFLTCFG_FLTSCL_SHIFT)) & I2C_MFLTCFG_FLTSCL_MASK)
#define I2C_MFLTCFG_FLTSDA_WIDTH                                              (4)
#define I2C_MFLTCFG_FLTSDA_MASK                                               (0xF0000U)
#define I2C_MFLTCFG_FLTSDA_SHIFT                                              (16U)
#define I2C_MFLTCFG_FLTSDA(x)                                                 (((uint32_t)(((uint32_t)(x)) << I2C_MFLTCFG_FLTSDA_SHIFT)) & I2C_MFLTCFG_FLTSDA_MASK)
#define I2C_MFLTCFG_DIV_WIDTH                                                 (3)
#define I2C_MFLTCFG_DIV_MASK                                                  (0x7U)
#define I2C_MFLTCFG_DIV_SHIFT                                                 (0U)
#define I2C_MFLTCFG_DIV(x)                                                    (((uint32_t)(((uint32_t)(x)) << I2C_MFLTCFG_DIV_SHIFT)) & I2C_MFLTCFG_DIV_MASK)
/* I2C_MSTS Register */
#define I2C_MSTS_ACTIVE_MASK                                                  (0x20000U)
#define I2C_MSTS_ACTIVE_SHIFT                                                 (17U)
#define I2C_MSTS_ACTIVE(x)                                                    (((uint32_t)(((uint32_t)(x)) << I2C_MSTS_ACTIVE_SHIFT)) & I2C_MSTS_ACTIVE_MASK)
#define I2C_MSTS_BUSY_MASK                                                    (0x10000U)
#define I2C_MSTS_BUSY_SHIFT                                                   (16U)
#define I2C_MSTS_BUSY(x)                                                      (((uint32_t)(((uint32_t)(x)) << I2C_MSTS_BUSY_SHIFT)) & I2C_MSTS_BUSY_MASK)
#define I2C_MSTS_OPERRIF_MASK                                                 (0x800U)
#define I2C_MSTS_OPERRIF_SHIFT                                                (11U)
#define I2C_MSTS_OPERRIF(x)                                                   (((uint32_t)(((uint32_t)(x)) << I2C_MSTS_OPERRIF_SHIFT)) & I2C_MSTS_OPERRIF_MASK)
#define I2C_MSTS_MATIF_MASK                                                   (0x200U)
#define I2C_MSTS_MATIF_SHIFT                                                  (9U)
#define I2C_MSTS_MATIF(x)                                                     (((uint32_t)(((uint32_t)(x)) << I2C_MSTS_MATIF_SHIFT)) & I2C_MSTS_MATIF_MASK)
#define I2C_MSTS_TOIF_MASK                                                    (0x100U)
#define I2C_MSTS_TOIF_SHIFT                                                   (8U)
#define I2C_MSTS_TOIF(x)                                                      (((uint32_t)(((uint32_t)(x)) << I2C_MSTS_TOIF_SHIFT)) & I2C_MSTS_TOIF_MASK)
#define I2C_MSTS_ARBIF_MASK                                                   (0x80U)
#define I2C_MSTS_ARBIF_SHIFT                                                  (7U)
#define I2C_MSTS_ARBIF(x)                                                     (((uint32_t)(((uint32_t)(x)) << I2C_MSTS_ARBIF_SHIFT)) & I2C_MSTS_ARBIF_MASK)
#define I2C_MSTS_NACKIF_MASK                                                  (0x40U)
#define I2C_MSTS_NACKIF_SHIFT                                                 (6U)
#define I2C_MSTS_NACKIF(x)                                                    (((uint32_t)(((uint32_t)(x)) << I2C_MSTS_NACKIF_SHIFT)) & I2C_MSTS_NACKIF_MASK)
#define I2C_MSTS_STOPIF_MASK                                                  (0x10U)
#define I2C_MSTS_STOPIF_SHIFT                                                 (4U)
#define I2C_MSTS_STOPIF(x)                                                    (((uint32_t)(((uint32_t)(x)) << I2C_MSTS_STOPIF_SHIFT)) & I2C_MSTS_STOPIF_MASK)
#define I2C_MSTS_TXIF_MASK                                                    (0x2U)
#define I2C_MSTS_TXIF_SHIFT                                                   (1U)
#define I2C_MSTS_TXIF(x)                                                      (((uint32_t)(((uint32_t)(x)) << I2C_MSTS_TXIF_SHIFT)) & I2C_MSTS_TXIF_MASK)
#define I2C_MSTS_RXIF_MASK                                                    (0x1U)
#define I2C_MSTS_RXIF_SHIFT                                                   (0U)
#define I2C_MSTS_RXIF(x)                                                      (((uint32_t)(((uint32_t)(x)) << I2C_MSTS_RXIF_SHIFT)) & I2C_MSTS_RXIF_MASK)
/* I2C_MIE Register */
#define I2C_MIE_OPERRIE_MASK                                                  (0x800U)
#define I2C_MIE_OPERRIE_SHIFT                                                 (11U)
#define I2C_MIE_OPERRIE(x)                                                    (((uint32_t)(((uint32_t)(x)) << I2C_MIE_OPERRIE_SHIFT)) & I2C_MIE_OPERRIE_MASK)
#define I2C_MIE_MATIE_MASK                                                    (0x200U)
#define I2C_MIE_MATIE_SHIFT                                                   (9U)
#define I2C_MIE_MATIE(x)                                                      (((uint32_t)(((uint32_t)(x)) << I2C_MIE_MATIE_SHIFT)) & I2C_MIE_MATIE_MASK)
#define I2C_MIE_TOIE_MASK                                                     (0x100U)
#define I2C_MIE_TOIE_SHIFT                                                    (8U)
#define I2C_MIE_TOIE(x)                                                       (((uint32_t)(((uint32_t)(x)) << I2C_MIE_TOIE_SHIFT)) & I2C_MIE_TOIE_MASK)
#define I2C_MIE_ARBIE_MASK                                                    (0x80U)
#define I2C_MIE_ARBIE_SHIFT                                                   (7U)
#define I2C_MIE_ARBIE(x)                                                      (((uint32_t)(((uint32_t)(x)) << I2C_MIE_ARBIE_SHIFT)) & I2C_MIE_ARBIE_MASK)
#define I2C_MIE_NACKIE_MASK                                                   (0x40U)
#define I2C_MIE_NACKIE_SHIFT                                                  (6U)
#define I2C_MIE_NACKIE(x)                                                     (((uint32_t)(((uint32_t)(x)) << I2C_MIE_NACKIE_SHIFT)) & I2C_MIE_NACKIE_MASK)
#define I2C_MIE_STOPIE_MASK                                                   (0x10U)
#define I2C_MIE_STOPIE_SHIFT                                                  (4U)
#define I2C_MIE_STOPIE(x)                                                     (((uint32_t)(((uint32_t)(x)) << I2C_MIE_STOPIE_SHIFT)) & I2C_MIE_STOPIE_MASK)
#define I2C_MIE_TXIE_MASK                                                     (0x2U)
#define I2C_MIE_TXIE_SHIFT                                                    (1U)
#define I2C_MIE_TXIE(x)                                                       (((uint32_t)(((uint32_t)(x)) << I2C_MIE_TXIE_SHIFT)) & I2C_MIE_TXIE_MASK)
#define I2C_MIE_RXIE_MASK                                                     (0x1U)
#define I2C_MIE_RXIE_SHIFT                                                    (0U)
#define I2C_MIE_RXIE(x)                                                       (((uint32_t)(((uint32_t)(x)) << I2C_MIE_RXIE_SHIFT)) & I2C_MIE_RXIE_MASK)
/* I2C_TXFIFO Register */
#define I2C_TXFIFO_SIZE_MASK                                                  (0x7000000U)
#define I2C_TXFIFO_SIZE_SHIFT                                                 (24U)
#define I2C_TXFIFO_SIZE(x)                                                    (((uint32_t)(((uint32_t)(x)) << I2C_TXFIFO_SIZE_SHIFT)) & I2C_TXFIFO_SIZE_MASK)
#define I2C_TXFIFO_RESET_MASK                                                 (0x8000U)
#define I2C_TXFIFO_RESET_SHIFT                                                (15U)
#define I2C_TXFIFO_RESET(x)                                                   (((uint32_t)(((uint32_t)(x)) << I2C_TXFIFO_RESET_SHIFT)) & I2C_TXFIFO_RESET_MASK)
#define I2C_TXFIFO_COUNT_MASK                                                 (0x700U)
#define I2C_TXFIFO_COUNT_SHIFT                                                (8U)
#define I2C_TXFIFO_COUNT(x)                                                   (((uint32_t)(((uint32_t)(x)) << I2C_TXFIFO_COUNT_SHIFT)) & I2C_TXFIFO_COUNT_MASK)
#define I2C_TXFIFO_WATER_MASK                                                 (0x7U)
#define I2C_TXFIFO_WATER_SHIFT                                                (0U)
#define I2C_TXFIFO_WATER(x)                                                   (((uint32_t)(((uint32_t)(x)) << I2C_TXFIFO_WATER_SHIFT)) & I2C_TXFIFO_WATER_MASK)
/* I2C_RXFIFO Register */
#define I2C_RXFIFO_SIZE_MASK                                                  (0x7000000U)
#define I2C_RXFIFO_SIZE_SHIFT                                                 (24U)
#define I2C_RXFIFO_SIZE(x)                                                    (((uint32_t)(((uint32_t)(x)) << I2C_RXFIFO_SIZE_SHIFT)) & I2C_RXFIFO_SIZE_MASK)
#define I2C_RXFIFO_RESET_MASK                                                 (0x8000U)
#define I2C_RXFIFO_RESET_SHIFT                                                (15U)
#define I2C_RXFIFO_RESET(x)                                                   (((uint32_t)(((uint32_t)(x)) << I2C_RXFIFO_RESET_SHIFT)) & I2C_RXFIFO_RESET_MASK)
#define I2C_RXFIFO_COUNT_MASK                                                 (0x700U)
#define I2C_RXFIFO_COUNT_SHIFT                                                (8U)
#define I2C_RXFIFO_COUNT(x)                                                   (((uint32_t)(((uint32_t)(x)) << I2C_RXFIFO_COUNT_SHIFT)) & I2C_RXFIFO_COUNT_MASK)
#define I2C_RXFIFO_WATER_MASK                                                 (0x7U)
#define I2C_RXFIFO_WATER_SHIFT                                                (0U)
#define I2C_RXFIFO_WATER(x)                                                   (((uint32_t)(((uint32_t)(x)) << I2C_RXFIFO_WATER_SHIFT)) & I2C_RXFIFO_WATER_MASK)
/* I2C_MDATA Register */
#define I2C_MDATA_EMPTY_MASK                                                  (0x10000U)
#define I2C_MDATA_EMPTY_SHIFT                                                 (16U)
#define I2C_MDATA_EMPTY(x)                                                    (((uint32_t)(((uint32_t)(x)) << I2C_MDATA_EMPTY_SHIFT)) & I2C_MDATA_EMPTY_MASK)
#define I2C_MDATA_HS_MASK                                                     (0x800U)
#define I2C_MDATA_HS_SHIFT                                                    (11U)
#define I2C_MDATA_HS(x)                                                       (((uint32_t)(((uint32_t)(x)) << I2C_MDATA_HS_SHIFT)) & I2C_MDATA_HS_MASK)
#define I2C_MDATA_REC_MASK                                                    (0x400U)
#define I2C_MDATA_REC_SHIFT                                                   (10U)
#define I2C_MDATA_REC(x)                                                      (((uint32_t)(((uint32_t)(x)) << I2C_MDATA_REC_SHIFT)) & I2C_MDATA_REC_MASK)
#define I2C_MDATA_STOP_MASK                                                   (0x200U)
#define I2C_MDATA_STOP_SHIFT                                                  (9U)
#define I2C_MDATA_STOP(x)                                                     (((uint32_t)(((uint32_t)(x)) << I2C_MDATA_STOP_SHIFT)) & I2C_MDATA_STOP_MASK)
#define I2C_MDATA_START_MASK                                                  (0x100U)
#define I2C_MDATA_START_SHIFT                                                 (8U)
#define I2C_MDATA_START(x)                                                    (((uint32_t)(((uint32_t)(x)) << I2C_MDATA_START_SHIFT)) & I2C_MDATA_START_MASK)
#define I2C_MDATA_DATA_MASK                                                   (0xFFU)
#define I2C_MDATA_DATA_SHIFT                                                  (0U)
#define I2C_MDATA_DATA(x)                                                     (((uint32_t)(((uint32_t)(x)) << I2C_MDATA_DATA_SHIFT)) & I2C_MDATA_DATA_MASK)
/* I2C_MATCH Register */
#define I2C_MATCH_MATCH1_MASK                                                 (0xFF0000U)
#define I2C_MATCH_MATCH1_SHIFT                                                (16U)
#define I2C_MATCH_MATCH1(x)                                                   (((uint32_t)(((uint32_t)(x)) << I2C_MATCH_MATCH1_SHIFT)) & I2C_MATCH_MATCH1_MASK)
#define I2C_MATCH_MATCH0_MASK                                                 (0xFFU)
#define I2C_MATCH_MATCH0_SHIFT                                                (0U)
#define I2C_MATCH_MATCH0(x)                                                   (((uint32_t)(((uint32_t)(x)) << I2C_MATCH_MATCH0_SHIFT)) & I2C_MATCH_MATCH0_MASK)
/* I2C_TOCFG Register */
#define I2C_TOCFG_IDLE_MASK                                                   (0xFFF0000U)
#define I2C_TOCFG_IDLE_SHIFT                                                  (16U)
#define I2C_TOCFG_IDLE(x)                                                     (((uint32_t)(((uint32_t)(x)) << I2C_TOCFG_IDLE_SHIFT)) & I2C_TOCFG_IDLE_MASK)
#define I2C_TOCFG_SDA_MASK                                                    (0x8000U)
#define I2C_TOCFG_SDA_SHIFT                                                   (15U)
#define I2C_TOCFG_SDA(x)                                                      (((uint32_t)(((uint32_t)(x)) << I2C_TOCFG_SDA_SHIFT)) & I2C_TOCFG_SDA_MASK)
#define I2C_TOCFG_LOW_MASK                                                    (0xFFFU)
#define I2C_TOCFG_LOW_SHIFT                                                   (0U)
#define I2C_TOCFG_LOW(x)                                                      (((uint32_t)(((uint32_t)(x)) << I2C_TOCFG_LOW_SHIFT)) & I2C_TOCFG_LOW_MASK)
/* I2C_HSCLK Register */
#define I2C_HSCLK_HIGH_WIDTH                                                  (6)
#define I2C_HSCLK_HIGH_MASK                                                   (0x3F000000U)
#define I2C_HSCLK_HIGH_SHIFT                                                  (24U)
#define I2C_HSCLK_HIGH(x)                                                     (((uint32_t)(((uint32_t)(x)) << I2C_HSCLK_HIGH_SHIFT)) & I2C_HSCLK_HIGH_MASK)
#define I2C_HSCLK_LOW_WIDTH                                                   (6)
#define I2C_HSCLK_LOW_MASK                                                    (0x3F0000U)
#define I2C_HSCLK_LOW_SHIFT                                                   (16U)
#define I2C_HSCLK_LOW(x)                                                      (((uint32_t)(((uint32_t)(x)) << I2C_HSCLK_LOW_SHIFT)) & I2C_HSCLK_LOW_MASK)
#define I2C_HSCLK_VALID_WIDTH                                                 (6)
#define I2C_HSCLK_VALID_MASK                                                  (0x3F00U)
#define I2C_HSCLK_VALID_SHIFT                                                 (8U)
#define I2C_HSCLK_VALID(x)                                                    (((uint32_t)(((uint32_t)(x)) << I2C_HSCLK_VALID_SHIFT)) & I2C_HSCLK_VALID_MASK)
#define I2C_HSCLK_STHD_WIDTH                                                  (6)
#define I2C_HSCLK_STHD_MASK                                                   (0x3FU)
#define I2C_HSCLK_STHD_SHIFT                                                  (0U)
#define I2C_HSCLK_STHD(x)                                                     (((uint32_t)(((uint32_t)(x)) << I2C_HSCLK_STHD_SHIFT)) & I2C_HSCLK_STHD_MASK)
/* I2C_SCTRL Register */
#define I2C_SCTRL_ADDRDMAEN_MASK                                              (0x40000000U)
#define I2C_SCTRL_ADDRDMAEN_SHIFT                                             (30U)
#define I2C_SCTRL_ADDRDMAEN(x)                                                (((uint32_t)(((uint32_t)(x)) << I2C_SCTRL_ADDRDMAEN_SHIFT)) & I2C_SCTRL_ADDRDMAEN_MASK)
#define I2C_SCTRL_TXDMAEN_MASK                                                (0x20000000U)
#define I2C_SCTRL_TXDMAEN_SHIFT                                               (29U)
#define I2C_SCTRL_TXDMAEN(x)                                                  (((uint32_t)(((uint32_t)(x)) << I2C_SCTRL_TXDMAEN_SHIFT)) & I2C_SCTRL_TXDMAEN_MASK)
#define I2C_SCTRL_RXDMAEN_MASK                                                (0x10000000U)
#define I2C_SCTRL_RXDMAEN_SHIFT                                               (28U)
#define I2C_SCTRL_RXDMAEN(x)                                                  (((uint32_t)(((uint32_t)(x)) << I2C_SCTRL_RXDMAEN_SHIFT)) & I2C_SCTRL_RXDMAEN_MASK)
#define I2C_SCTRL_STOPDMAEN_MASK                                              (0x8000000U)
#define I2C_SCTRL_STOPDMAEN_SHIFT                                             (27U)
#define I2C_SCTRL_STOPDMAEN(x)                                                (((uint32_t)(((uint32_t)(x)) << I2C_SCTRL_STOPDMAEN_SHIFT)) & I2C_SCTRL_STOPDMAEN_MASK)
#define I2C_SCTRL_STARTDMAEN_MASK                                             (0x4000000U)
#define I2C_SCTRL_STARTDMAEN_SHIFT                                            (26U)
#define I2C_SCTRL_STARTDMAEN(x)                                               (((uint32_t)(((uint32_t)(x)) << I2C_SCTRL_STARTDMAEN_SHIFT)) & I2C_SCTRL_STARTDMAEN_MASK)
#define I2C_SCTRL_HSEN_MASK                                                   (0x400000U)
#define I2C_SCTRL_HSEN_SHIFT                                                  (22U)
#define I2C_SCTRL_HSEN(x)                                                     (((uint32_t)(((uint32_t)(x)) << I2C_SCTRL_HSEN_SHIFT)) & I2C_SCTRL_HSEN_MASK)
#define I2C_SCTRL_RXCFG_MASK                                                  (0x100000U)
#define I2C_SCTRL_RXCFG_SHIFT                                                 (20U)
#define I2C_SCTRL_RXCFG(x)                                                    (((uint32_t)(((uint32_t)(x)) << I2C_SCTRL_RXCFG_SHIFT)) & I2C_SCTRL_RXCFG_MASK)
#define I2C_SCTRL_TXCFG_MASK                                                  (0x80000U)
#define I2C_SCTRL_TXCFG_SHIFT                                                 (19U)
#define I2C_SCTRL_TXCFG(x)                                                    (((uint32_t)(((uint32_t)(x)) << I2C_SCTRL_TXCFG_SHIFT)) & I2C_SCTRL_TXCFG_MASK)
#define I2C_SCTRL_ADDRCFG_MASK                                                (0x70000U)
#define I2C_SCTRL_ADDRCFG_SHIFT                                               (16U)
#define I2C_SCTRL_ADDRCFG(x)                                                  (((uint32_t)(((uint32_t)(x)) << I2C_SCTRL_ADDRCFG_SHIFT)) & I2C_SCTRL_ADDRCFG_MASK)
#define I2C_SCTRL_IGACK_MASK                                                  (0x8000U)
#define I2C_SCTRL_IGACK_SHIFT                                                 (15U)
#define I2C_SCTRL_IGACK(x)                                                    (((uint32_t)(((uint32_t)(x)) << I2C_SCTRL_IGACK_SHIFT)) & I2C_SCTRL_IGACK_MASK)
#define I2C_SCTRL_TXRST_MASK                                                  (0x2000U)
#define I2C_SCTRL_TXRST_SHIFT                                                 (13U)
#define I2C_SCTRL_TXRST(x)                                                    (((uint32_t)(((uint32_t)(x)) << I2C_SCTRL_TXRST_SHIFT)) & I2C_SCTRL_TXRST_MASK)
#define I2C_SCTRL_RXRST_MASK                                                  (0x1000U)
#define I2C_SCTRL_RXRST_SHIFT                                                 (12U)
#define I2C_SCTRL_RXRST(x)                                                    (((uint32_t)(((uint32_t)(x)) << I2C_SCTRL_RXRST_SHIFT)) & I2C_SCTRL_RXRST_MASK)
#define I2C_SCTRL_ALERTEN_MASK                                                (0x800U)
#define I2C_SCTRL_ALERTEN_SHIFT                                               (11U)
#define I2C_SCTRL_ALERTEN(x)                                                  (((uint32_t)(((uint32_t)(x)) << I2C_SCTRL_ALERTEN_SHIFT)) & I2C_SCTRL_ALERTEN_MASK)
#define I2C_SCTRL_GCEN_MASK                                                   (0x400U)
#define I2C_SCTRL_GCEN_SHIFT                                                  (10U)
#define I2C_SCTRL_GCEN(x)                                                     (((uint32_t)(((uint32_t)(x)) << I2C_SCTRL_GCEN_SHIFT)) & I2C_SCTRL_GCEN_MASK)
#define I2C_SCTRL_TXSTALL_MASK                                                (0x80U)
#define I2C_SCTRL_TXSTALL_SHIFT                                               (7U)
#define I2C_SCTRL_TXSTALL(x)                                                  (((uint32_t)(((uint32_t)(x)) << I2C_SCTRL_TXSTALL_SHIFT)) & I2C_SCTRL_TXSTALL_MASK)
#define I2C_SCTRL_RXSTALL_MASK                                                (0x40U)
#define I2C_SCTRL_RXSTALL_SHIFT                                               (6U)
#define I2C_SCTRL_RXSTALL(x)                                                  (((uint32_t)(((uint32_t)(x)) << I2C_SCTRL_RXSTALL_SHIFT)) & I2C_SCTRL_RXSTALL_MASK)
#define I2C_SCTRL_ACKSTALL_MASK                                               (0x20U)
#define I2C_SCTRL_ACKSTALL_SHIFT                                              (5U)
#define I2C_SCTRL_ACKSTALL(x)                                                 (((uint32_t)(((uint32_t)(x)) << I2C_SCTRL_ACKSTALL_SHIFT)) & I2C_SCTRL_ACKSTALL_MASK)
#define I2C_SCTRL_ADRSTALL_MASK                                               (0x10U)
#define I2C_SCTRL_ADRSTALL_SHIFT                                              (4U)
#define I2C_SCTRL_ADRSTALL(x)                                                 (((uint32_t)(((uint32_t)(x)) << I2C_SCTRL_ADRSTALL_SHIFT)) & I2C_SCTRL_ADRSTALL_MASK)
#define I2C_SCTRL_FILTEN_MASK                                                 (0x4U)
#define I2C_SCTRL_FILTEN_SHIFT                                                (2U)
#define I2C_SCTRL_FILTEN(x)                                                   (((uint32_t)(((uint32_t)(x)) << I2C_SCTRL_FILTEN_SHIFT)) & I2C_SCTRL_FILTEN_MASK)
#define I2C_SCTRL_SEN_MASK                                                    (0x1U)
#define I2C_SCTRL_SEN_SHIFT                                                   (0U)
#define I2C_SCTRL_SEN(x)                                                      (((uint32_t)(((uint32_t)(x)) << I2C_SCTRL_SEN_SHIFT)) & I2C_SCTRL_SEN_MASK)
/* I2C_SCLKCFG Register */
#define I2C_SCLKCFG_VALID_MASK                                                (0x3F00U)
#define I2C_SCLKCFG_VALID_SHIFT                                               (8U)
#define I2C_SCLKCFG_VALID(x)                                                  (((uint32_t)(((uint32_t)(x)) << I2C_SCLKCFG_VALID_SHIFT)) & I2C_SCLKCFG_VALID_MASK)
#define I2C_SCLKCFG_HOLD_MASK                                                 (0xFU)
#define I2C_SCLKCFG_HOLD_SHIFT                                                (0U)
#define I2C_SCLKCFG_HOLD(x)                                                   (((uint32_t)(((uint32_t)(x)) << I2C_SCLKCFG_HOLD_SHIFT)) & I2C_SCLKCFG_HOLD_MASK)
/* I2C_SFLTCFG Register */
#define I2C_SFLTCFG_FLTSCL_MASK                                               (0xF000000U)
#define I2C_SFLTCFG_FLTSCL_SHIFT                                              (24U)
#define I2C_SFLTCFG_FLTSCL(x)                                                 (((uint32_t)(((uint32_t)(x)) << I2C_SFLTCFG_FLTSCL_SHIFT)) & I2C_SFLTCFG_FLTSCL_MASK)
#define I2C_SFLTCFG_FLTSDA_MASK                                               (0xF0000U)
#define I2C_SFLTCFG_FLTSDA_SHIFT                                              (16U)
#define I2C_SFLTCFG_FLTSDA(x)                                                 (((uint32_t)(((uint32_t)(x)) << I2C_SFLTCFG_FLTSDA_SHIFT)) & I2C_SFLTCFG_FLTSDA_MASK)
/* I2C_SSTS Register */
#define I2C_SSTS_ACTIVE_MASK                                                  (0x20000U)
#define I2C_SSTS_ACTIVE_SHIFT                                                 (17U)
#define I2C_SSTS_ACTIVE(x)                                                    (((uint32_t)(((uint32_t)(x)) << I2C_SSTS_ACTIVE_SHIFT)) & I2C_SSTS_ACTIVE_MASK)
#define I2C_SSTS_BUSY_MASK                                                    (0x10000U)
#define I2C_SSTS_BUSY_SHIFT                                                   (16U)
#define I2C_SSTS_BUSY(x)                                                      (((uint32_t)(((uint32_t)(x)) << I2C_SSTS_BUSY_SHIFT)) & I2C_SSTS_BUSY_MASK)
#define I2C_SSTS_OPERRIF_MASK                                                 (0x2000U)
#define I2C_SSTS_OPERRIF_SHIFT                                                (13U)
#define I2C_SSTS_OPERRIF(x)                                                   (((uint32_t)(((uint32_t)(x)) << I2C_SSTS_OPERRIF_SHIFT)) & I2C_SSTS_OPERRIF_MASK)
#define I2C_SSTS_ALERTIF_MASK                                                 (0x1000U)
#define I2C_SSTS_ALERTIF_SHIFT                                                (12U)
#define I2C_SSTS_ALERTIF(x)                                                   (((uint32_t)(((uint32_t)(x)) << I2C_SSTS_ALERTIF_SHIFT)) & I2C_SSTS_ALERTIF_MASK)
#define I2C_SSTS_GCIF_MASK                                                    (0x800U)
#define I2C_SSTS_GCIF_SHIFT                                                   (11U)
#define I2C_SSTS_GCIF(x)                                                      (((uint32_t)(((uint32_t)(x)) << I2C_SSTS_GCIF_SHIFT)) & I2C_SSTS_GCIF_MASK)
#define I2C_SSTS_ADRBIF_MASK                                                  (0x400U)
#define I2C_SSTS_ADRBIF_SHIFT                                                 (10U)
#define I2C_SSTS_ADRBIF(x)                                                    (((uint32_t)(((uint32_t)(x)) << I2C_SSTS_ADRBIF_SHIFT)) & I2C_SSTS_ADRBIF_MASK)
#define I2C_SSTS_MATCHAIF_MASK                                                (0x200U)
#define I2C_SSTS_MATCHAIF_SHIFT                                               (9U)
#define I2C_SSTS_MATCHAIF(x)                                                  (((uint32_t)(((uint32_t)(x)) << I2C_SSTS_MATCHAIF_SHIFT)) & I2C_SSTS_MATCHAIF_MASK)
#define I2C_SSTS_ARBIF_MASK                                                   (0x80U)
#define I2C_SSTS_ARBIF_SHIFT                                                  (7U)
#define I2C_SSTS_ARBIF(x)                                                     (((uint32_t)(((uint32_t)(x)) << I2C_SSTS_ARBIF_SHIFT)) & I2C_SSTS_ARBIF_MASK)
#define I2C_SSTS_RSIF_MASK                                                    (0x20U)
#define I2C_SSTS_RSIF_SHIFT                                                   (5U)
#define I2C_SSTS_RSIF(x)                                                      (((uint32_t)(((uint32_t)(x)) << I2C_SSTS_RSIF_SHIFT)) & I2C_SSTS_RSIF_MASK)
#define I2C_SSTS_STOPIF_MASK                                                  (0x10U)
#define I2C_SSTS_STOPIF_SHIFT                                                 (4U)
#define I2C_SSTS_STOPIF(x)                                                    (((uint32_t)(((uint32_t)(x)) << I2C_SSTS_STOPIF_SHIFT)) & I2C_SSTS_STOPIF_MASK)
#define I2C_SSTS_ADDRIF_MASK                                                  (0x8U)
#define I2C_SSTS_ADDRIF_SHIFT                                                 (3U)
#define I2C_SSTS_ADDRIF(x)                                                    (((uint32_t)(((uint32_t)(x)) << I2C_SSTS_ADDRIF_SHIFT)) & I2C_SSTS_ADDRIF_MASK)
#define I2C_SSTS_ACKIF_MASK                                                   (0x4U)
#define I2C_SSTS_ACKIF_SHIFT                                                  (2U)
#define I2C_SSTS_ACKIF(x)                                                     (((uint32_t)(((uint32_t)(x)) << I2C_SSTS_ACKIF_SHIFT)) & I2C_SSTS_ACKIF_MASK)
#define I2C_SSTS_TXIF_MASK                                                    (0x2U)
#define I2C_SSTS_TXIF_SHIFT                                                   (1U)
#define I2C_SSTS_TXIF(x)                                                      (((uint32_t)(((uint32_t)(x)) << I2C_SSTS_TXIF_SHIFT)) & I2C_SSTS_TXIF_MASK)
#define I2C_SSTS_RXIF_MASK                                                    (0x1U)
#define I2C_SSTS_RXIF_SHIFT                                                   (0U)
#define I2C_SSTS_RXIF(x)                                                      (((uint32_t)(((uint32_t)(x)) << I2C_SSTS_RXIF_SHIFT)) & I2C_SSTS_RXIF_MASK)
/* I2C_SIE Register */
#define I2C_SIE_OPERRIE_MASK                                                  (0x2000U)
#define I2C_SIE_OPERRIE_SHIFT                                                 (13U)
#define I2C_SIE_OPERRIE(x)                                                    (((uint32_t)(((uint32_t)(x)) << I2C_SIE_OPERRIE_SHIFT)) & I2C_SIE_OPERRIE_MASK)
#define I2C_SIE_ALERTIE_MASK                                                  (0x1000U)
#define I2C_SIE_ALERTIE_SHIFT                                                 (12U)
#define I2C_SIE_ALERTIE(x)                                                    (((uint32_t)(((uint32_t)(x)) << I2C_SIE_ALERTIE_SHIFT)) & I2C_SIE_ALERTIE_MASK)
#define I2C_SIE_GCIE_MASK                                                     (0x800U)
#define I2C_SIE_GCIE_SHIFT                                                    (11U)
#define I2C_SIE_GCIE(x)                                                       (((uint32_t)(((uint32_t)(x)) << I2C_SIE_GCIE_SHIFT)) & I2C_SIE_GCIE_MASK)
#define I2C_SIE_MAT1IE_MASK                                                   (0x400U)
#define I2C_SIE_MAT1IE_SHIFT                                                  (10U)
#define I2C_SIE_MAT1IE(x)                                                     (((uint32_t)(((uint32_t)(x)) << I2C_SIE_MAT1IE_SHIFT)) & I2C_SIE_MAT1IE_MASK)
#define I2C_SIE_MAT0IE_MASK                                                   (0x200U)
#define I2C_SIE_MAT0IE_SHIFT                                                  (9U)
#define I2C_SIE_MAT0IE(x)                                                     (((uint32_t)(((uint32_t)(x)) << I2C_SIE_MAT0IE_SHIFT)) & I2C_SIE_MAT0IE_MASK)
#define I2C_SIE_ARBIE_MASK                                                    (0x80U)
#define I2C_SIE_ARBIE_SHIFT                                                   (7U)
#define I2C_SIE_ARBIE(x)                                                      (((uint32_t)(((uint32_t)(x)) << I2C_SIE_ARBIE_SHIFT)) & I2C_SIE_ARBIE_MASK)
#define I2C_SIE_RSIE_MASK                                                     (0x20U)
#define I2C_SIE_RSIE_SHIFT                                                    (5U)
#define I2C_SIE_RSIE(x)                                                       (((uint32_t)(((uint32_t)(x)) << I2C_SIE_RSIE_SHIFT)) & I2C_SIE_RSIE_MASK)
#define I2C_SIE_STOPIE_MASK                                                   (0x10U)
#define I2C_SIE_STOPIE_SHIFT                                                  (4U)
#define I2C_SIE_STOPIE(x)                                                     (((uint32_t)(((uint32_t)(x)) << I2C_SIE_STOPIE_SHIFT)) & I2C_SIE_STOPIE_MASK)
#define I2C_SIE_ADDRIE_MASK                                                   (0x8U)
#define I2C_SIE_ADDRIE_SHIFT                                                  (3U)
#define I2C_SIE_ADDRIE(x)                                                     (((uint32_t)(((uint32_t)(x)) << I2C_SIE_ADDRIE_SHIFT)) & I2C_SIE_ADDRIE_MASK)
#define I2C_SIE_ACKIE_MASK                                                    (0x4U)
#define I2C_SIE_ACKIE_SHIFT                                                   (2U)
#define I2C_SIE_ACKIE(x)                                                      (((uint32_t)(((uint32_t)(x)) << I2C_SIE_ACKIE_SHIFT)) & I2C_SIE_ACKIE_MASK)
#define I2C_SIE_TXIE_MASK                                                     (0x2U)
#define I2C_SIE_TXIE_SHIFT                                                    (1U)
#define I2C_SIE_TXIE(x)                                                       (((uint32_t)(((uint32_t)(x)) << I2C_SIE_TXIE_SHIFT)) & I2C_SIE_TXIE_MASK)
#define I2C_SIE_RXIE_MASK                                                     (0x1U)
#define I2C_SIE_RXIE_SHIFT                                                    (0U)
#define I2C_SIE_RXIE(x)                                                       (((uint32_t)(((uint32_t)(x)) << I2C_SIE_RXIE_SHIFT)) & I2C_SIE_RXIE_MASK)
/* I2C_SDATA Register */
#define I2C_SDATA_EMPTY_MASK                                                  (0x10000U)
#define I2C_SDATA_EMPTY_SHIFT                                                 (16U)
#define I2C_SDATA_EMPTY(x)                                                    (((uint32_t)(((uint32_t)(x)) << I2C_SDATA_EMPTY_SHIFT)) & I2C_SDATA_EMPTY_MASK)
#define I2C_SDATA_DATA_MASK                                                   (0xFFU)
#define I2C_SDATA_DATA_SHIFT                                                  (0U)
#define I2C_SDATA_DATA(x)                                                     (((uint32_t)(((uint32_t)(x)) << I2C_SDATA_DATA_SHIFT)) & I2C_SDATA_DATA_MASK)
/* I2C_SACK Register */
#define I2C_SACK_NACK_MASK                                                    (0x1U)
#define I2C_SACK_NACK_SHIFT                                                   (0U)
#define I2C_SACK_NACK(x)                                                      (((uint32_t)(((uint32_t)(x)) << I2C_SACK_NACK_SHIFT)) & I2C_SACK_NACK_MASK)
/* I2C_SADDR Register */
#define I2C_SADDR_ADDRB_MASK                                                  (0x7FF0000U)
#define I2C_SADDR_ADDRB_SHIFT                                                 (16U)
#define I2C_SADDR_ADDRB(x)                                                    (((uint32_t)(((uint32_t)(x)) << I2C_SADDR_ADDRB_SHIFT)) & I2C_SADDR_ADDRB_MASK)
#define I2C_SADDR_ADDRA_MASK                                                  (0x7FFU)
#define I2C_SADDR_ADDRA_SHIFT                                                 (0U)
#define I2C_SADDR_ADDRA(x)                                                    (((uint32_t)(((uint32_t)(x)) << I2C_SADDR_ADDRA_SHIFT)) & I2C_SADDR_ADDRA_MASK)
/* I2C_SRADDR Register */
#define I2C_SRADDR_VALID_MASK                                                 (0x8000U)
#define I2C_SRADDR_VALID_SHIFT                                                (15U)
#define I2C_SRADDR_VALID(x)                                                   (((uint32_t)(((uint32_t)(x)) << I2C_SRADDR_VALID_SHIFT)) & I2C_SRADDR_VALID_MASK)
#define I2C_SRADDR_ADDR_MASK                                                  (0x7FFU)
#define I2C_SRADDR_ADDR_SHIFT                                                 (0U)
#define I2C_SRADDR_ADDR(x)                                                    (((uint32_t)(((uint32_t)(x)) << I2C_SRADDR_ADDR_SHIFT)) & I2C_SRADDR_ADDR_MASK)


/*!
 * @}
 */ /* end of group I2C_Register_Masks */


/*!
 * @}
 */ /* end of group I2C_Peripheral_Access_Layer */


/* -------------------------------------------------------------
   -- pTMR Peripheral Access Layer
   ------------------------------------------------------------- */

/*!
 * @addtogroup pTMR_Peripheral_Access_Layer pTMR Peripheral Access Layer
 * @{
 */


/** pTMR - Size of Registers Arrays */
#define pTMR_CH_COUNT                                                         (4U)

/* pTMR Register Layout Typedef */
typedef struct {
    __IO uint32_t MCR;                                                        /**< Module Control Register, offset: 0x0000 */
    __IO uint8_t  RESERVED1[252];                                             /**< RESERVED1, offset: 0x0004 */
    struct {
        __IO uint32_t TSV;                                                    /**< Timer Start Value Register, offset: 0x0100 */
        __IO uint32_t TCV;                                                    /**< Timer Current Value Register, offset: 0x0104 */
        __IO uint32_t TCR;                                                    /**< Timer Control Register, offset: 0x0108 */
        __IO uint32_t TFR;                                                    /**< Timer Flag Register, offset: 0x010C */

    } CH[4];

} pTMR_Type, *pTMR_MemMapPtr;

/** Number of instances of the pTMR module. */
#define pTMR_INSTANCE_COUNT                                                   (1U)

/* pTMR0  base address */
#define pTMR0_BASE                                                            (0x4005A000U)
#define pTMR0                                                                 ((pTMR_Type *)(pTMR0_BASE))

/** Array initializer of pTMR peripheral base addresses */
#define pTMR_BASE_ADDRS                                                       { pTMR0_BASE }
/** Array initializer of pTMR peripheral base pointers */
#define pTMR_BASE_PTRS                                                        { pTMR0 }
/** Number of interrupt channels for the pTMR. */
#define pTMR_IRQS_CH_COUNT                                                    (4U)
/** Interrupt vectors for the pTMR peripheral type */
#define pTMR_IRQS                                                             { {pTMR0_Ch0_IRQn, pTMR0_Ch1_IRQn, pTMR0_Ch2_IRQn, pTMR0_Ch3_IRQn} }

/* pTMR Register Mask */
/* pTMR_MCR Register */
#define pTMR_MCR_FRZ_MASK                                                     (0x2U)
#define pTMR_MCR_FRZ_SHIFT                                                    (1U)
#define pTMR_MCR_FRZ(x)                                                       (((uint32_t)(((uint32_t)(x)) << pTMR_MCR_FRZ_SHIFT)) & pTMR_MCR_FRZ_MASK)
#define pTMR_MCR_EN_MASK                                                      (0x1U)
#define pTMR_MCR_EN_SHIFT                                                     (0U)
#define pTMR_MCR_EN(x)                                                        (((uint32_t)(((uint32_t)(x)) << pTMR_MCR_EN_SHIFT)) & pTMR_MCR_EN_MASK)
/* pTMR_CH_TSV Register */
#define pTMR_CH_TSV_STVAL_MASK                                                (0xFFFFFFFFU)
#define pTMR_CH_TSV_STVAL_SHIFT                                               (0U)
#define pTMR_CH_TSV_STVAL(x)                                                  (((uint32_t)(((uint32_t)(x)) << pTMR_CH_TSV_STVAL_SHIFT)) & pTMR_CH_TSV_STVAL_MASK)
/* pTMR_CH_TCV Register */
#define pTMR_CH_TCV_CVAL_MASK                                                 (0xFFFFFFFFU)
#define pTMR_CH_TCV_CVAL_SHIFT                                                (0U)
#define pTMR_CH_TCV_CVAL(x)                                                   (((uint32_t)(((uint32_t)(x)) << pTMR_CH_TCV_CVAL_SHIFT)) & pTMR_CH_TCV_CVAL_MASK)
/* pTMR_CH_TCR Register */
#define pTMR_CH_TCR_CHAIN_MASK                                                (0x4U)
#define pTMR_CH_TCR_CHAIN_SHIFT                                               (2U)
#define pTMR_CH_TCR_CHAIN(x)                                                  (((uint32_t)(((uint32_t)(x)) << pTMR_CH_TCR_CHAIN_SHIFT)) & pTMR_CH_TCR_CHAIN_MASK)
#define pTMR_CH_TCR_TIE_MASK                                                  (0x2U)
#define pTMR_CH_TCR_TIE_SHIFT                                                 (1U)
#define pTMR_CH_TCR_TIE(x)                                                    (((uint32_t)(((uint32_t)(x)) << pTMR_CH_TCR_TIE_SHIFT)) & pTMR_CH_TCR_TIE_MASK)
#define pTMR_CH_TCR_TEN_MASK                                                  (0x1U)
#define pTMR_CH_TCR_TEN_SHIFT                                                 (0U)
#define pTMR_CH_TCR_TEN(x)                                                    (((uint32_t)(((uint32_t)(x)) << pTMR_CH_TCR_TEN_SHIFT)) & pTMR_CH_TCR_TEN_MASK)
/* pTMR_CH_TFR Register */
#define pTMR_CH_TFR_TIF_MASK                                                  (0x1U)
#define pTMR_CH_TFR_TIF_SHIFT                                                 (0U)
#define pTMR_CH_TFR_TIF(x)                                                    (((uint32_t)(((uint32_t)(x)) << pTMR_CH_TFR_TIF_SHIFT)) & pTMR_CH_TFR_TIF_MASK)



/*!
 * @}
 */ /* end of group pTMR_Register_Masks */


/*!
 * @}
 */ /* end of group pTMR_Peripheral_Access_Layer */


/* -------------------------------------------------------------
   -- lpTMR Peripheral Access Layer
   ------------------------------------------------------------- */

/*!
 * @addtogroup lpTMR_Peripheral_Access_Layer lpTMR Peripheral Access Layer
 * @{
 */


/** lpTMR - Size of Registers Arrays */

/* lpTMR Register Layout Typedef */
typedef struct {
    __IO uint32_t CTRL;                                                       /**< Control Register, offset: 0x0000 */
    __IO uint32_t PRS;                                                        /**< Prescaler Register, offset: 0x0004 */
    __IO uint32_t DIE;                                                        /**< DMA and Interrupt Enable Register, offset: 0x0008 */
    __IO uint32_t STS;                                                        /**< Status Register, offset: 0x000C */
    __IO uint32_t CMP;                                                        /**< Compare Value Register, offset: 0x0010 */
    __O  uint32_t LCNT;                                                       /**< Latch Timer Counter Register, offset: 0x0014 */
    __IO uint32_t CNT;                                                        /**< Timer Current Counter Register, offset: 0x0018 */

} lpTMR_Type, *lpTMR_MemMapPtr;

/** Number of instances of the lpTMR module. */
#define lpTMR_INSTANCE_COUNT                                                  (1U)

/* lpTMR0  base address */
#define lpTMR0_BASE                                                           (0x4005D000U)
#define lpTMR0                                                                ((lpTMR_Type *)(lpTMR0_BASE))

/** Array initializer of lpTMR peripheral base addresses */
#define lpTMR_BASE_ADDRS                                                      { lpTMR0_BASE }
/** Array initializer of lpTMR peripheral base pointers */
#define lpTMR_BASE_PTRS                                                       { lpTMR0 }
/** Number of interrupt channels for the lpTMR. */
#define lpTMR_IRQS_CH_COUNT                                                   (1U)
/** Interrupt vectors for the lpTMR peripheral type */
#define lpTMR_IRQS                                                            { lpTMR0_IRQn }

/* lpTMR Register Mask */
/* lpTMR_CTRL Register */
#define lpTMR_CTRL_PINSEL_MASK                                                (0x30U)
#define lpTMR_CTRL_PINSEL_SHIFT                                               (4U)
#define lpTMR_CTRL_PINSEL(x)                                                  (((uint32_t)(((uint32_t)(x)) << lpTMR_CTRL_PINSEL_SHIFT)) & lpTMR_CTRL_PINSEL_MASK)
#define lpTMR_CTRL_PINPOL_MASK                                                (0x8U)
#define lpTMR_CTRL_PINPOL_SHIFT                                               (3U)
#define lpTMR_CTRL_PINPOL(x)                                                  (((uint32_t)(((uint32_t)(x)) << lpTMR_CTRL_PINPOL_SHIFT)) & lpTMR_CTRL_PINPOL_MASK)
#define lpTMR_CTRL_TMODE_MASK                                                 (0x4U)
#define lpTMR_CTRL_TMODE_SHIFT                                                (2U)
#define lpTMR_CTRL_TMODE(x)                                                   (((uint32_t)(((uint32_t)(x)) << lpTMR_CTRL_TMODE_SHIFT)) & lpTMR_CTRL_TMODE_MASK)
#define lpTMR_CTRL_MODE_MASK                                                  (0x2U)
#define lpTMR_CTRL_MODE_SHIFT                                                 (1U)
#define lpTMR_CTRL_MODE(x)                                                    (((uint32_t)(((uint32_t)(x)) << lpTMR_CTRL_MODE_SHIFT)) & lpTMR_CTRL_MODE_MASK)
#define lpTMR_CTRL_EN_MASK                                                    (0x1U)
#define lpTMR_CTRL_EN_SHIFT                                                   (0U)
#define lpTMR_CTRL_EN(x)                                                      (((uint32_t)(((uint32_t)(x)) << lpTMR_CTRL_EN_SHIFT)) & lpTMR_CTRL_EN_MASK)
/* lpTMR_PRS Register */
#define lpTMR_PRS_PRES_WIDTH                                                  (4)
#define lpTMR_PRS_PRES_MASK                                                   (0x78U)
#define lpTMR_PRS_PRES_SHIFT                                                  (3U)
#define lpTMR_PRS_PRES(x)                                                     (((uint32_t)(((uint32_t)(x)) << lpTMR_PRS_PRES_SHIFT)) & lpTMR_PRS_PRES_MASK)
#define lpTMR_PRS_BYPASS_MASK                                                 (0x4U)
#define lpTMR_PRS_BYPASS_SHIFT                                                (2U)
#define lpTMR_PRS_BYPASS(x)                                                   (((uint32_t)(((uint32_t)(x)) << lpTMR_PRS_BYPASS_SHIFT)) & lpTMR_PRS_BYPASS_MASK)
#define lpTMR_PRS_CLKSEL_MASK                                                 (0x3U)
#define lpTMR_PRS_CLKSEL_SHIFT                                                (0U)
#define lpTMR_PRS_CLKSEL(x)                                                   (((uint32_t)(((uint32_t)(x)) << lpTMR_PRS_CLKSEL_SHIFT)) & lpTMR_PRS_CLKSEL_MASK)
/* lpTMR_DIE Register */
#define lpTMR_DIE_DMAEN_MASK                                                  (0x2U)
#define lpTMR_DIE_DMAEN_SHIFT                                                 (1U)
#define lpTMR_DIE_DMAEN(x)                                                    (((uint32_t)(((uint32_t)(x)) << lpTMR_DIE_DMAEN_SHIFT)) & lpTMR_DIE_DMAEN_MASK)
#define lpTMR_DIE_IE_MASK                                                     (0x1U)
#define lpTMR_DIE_IE_SHIFT                                                    (0U)
#define lpTMR_DIE_IE(x)                                                       (((uint32_t)(((uint32_t)(x)) << lpTMR_DIE_IE_SHIFT)) & lpTMR_DIE_IE_MASK)
/* lpTMR_STS Register */
#define lpTMR_STS_CCF_MASK                                                    (0x1U)
#define lpTMR_STS_CCF_SHIFT                                                   (0U)
#define lpTMR_STS_CCF(x)                                                      (((uint32_t)(((uint32_t)(x)) << lpTMR_STS_CCF_SHIFT)) & lpTMR_STS_CCF_MASK)
/* lpTMR_CMP Register */
#define lpTMR_CMP_CMP_MASK                                                    (0xFFFFU)
#define lpTMR_CMP_CMP_SHIFT                                                   (0U)
#define lpTMR_CMP_CMP(x)                                                      (((uint32_t)(((uint32_t)(x)) << lpTMR_CMP_CMP_SHIFT)) & lpTMR_CMP_CMP_MASK)
/* lpTMR_LCNT Register */
#define lpTMR_LCNT_LCVAL_MASK                                                 (0xFFFFU)
#define lpTMR_LCNT_LCVAL_SHIFT                                                (0U)
#define lpTMR_LCNT_LCVAL(x)                                                   (((uint32_t)(((uint32_t)(x)) << lpTMR_LCNT_LCVAL_SHIFT)) & lpTMR_LCNT_LCVAL_MASK)
/* lpTMR_CNT Register */
#define lpTMR_CNT_CVAL_WIDTH                                                  (16)
#define lpTMR_CNT_CVAL_MASK                                                   (0xFFFFU)
#define lpTMR_CNT_CVAL_SHIFT                                                  (0U)
#define lpTMR_CNT_CVAL(x)                                                     (((uint32_t)(((uint32_t)(x)) << lpTMR_CNT_CVAL_SHIFT)) & lpTMR_CNT_CVAL_MASK)


/*!
 * @}
 */ /* end of group lpTMR_Register_Masks */


/*!
 * @}
 */ /* end of group lpTMR_Peripheral_Access_Layer */


/* -------------------------------------------------------------
   -- eTMR Peripheral Access Layer
   ------------------------------------------------------------- */

/*!
 * @addtogroup eTMR_Peripheral_Access_Layer eTMR Peripheral Access Layer
 * @{
 */


/** eTMR - Size of Registers Arrays */
#define eTMR_CH_COUNT                                                         (8U)

/* eTMR Register Layout Typedef */
typedef struct {
    __IO uint32_t STS;                                                        /**< Status Register, offset: 0x0000 */
    __IO uint32_t IOSTS;                                                      /**< Input/Output Status Register, offset: 0x0004 */
    __IO uint32_t CNT;                                                        /**< Counter Value Register, offset: 0x0008 */
    __IO uint32_t QDCNT;                                                      /**< Quadrature Decoder Counter Value Register, offset: 0x000C */
    __IO uint32_t CTRL;                                                       /**< Control Register, offset: 0x0010 */
    __IO uint32_t SYNC;                                                       /**< Synchronous Register, offset: 0x0014 */
    __IO uint32_t OTRIG;                                                      /**< Output Trigger Register, offset: 0x0018 */
    __IO uint32_t INTE;                                                       /**< Interrupt Enable Register, offset: 0x001C */
    __IO uint32_t CHINIT;                                                     /**< Channel Output Initial Value Register, offset: 0x0020 */
    __IO uint32_t CHSWV;                                                      /**< Channel Software Value Register, offset: 0x0024 */
    __IO uint32_t CHMASK;                                                     /**< Channel Output Mask Value Register, offset: 0x0028 */
    __IO uint32_t CHFV;                                                       /**< Channel Fault Value Register, offset: 0x002C */
    __IO uint32_t MOD;                                                        /**< Counter Modulus Value Register, offset: 0x0030 */
    __IO uint32_t DT;                                                         /**< Deadtime Register, offset: 0x0034 */
    __IO uint32_t QDCTRL;                                                     /**< Quadrature Decoder Control Register, offset: 0x0038 */
    __IO uint32_t FAULT;                                                      /**< Fault Control Register, offset: 0x003C */
    __IO uint8_t  RESERVED16[16];                                             /**< RESERVED16, offset: 0x0040 */
    struct {
        __IO uint32_t CTRL;                                                   /**< Channel Control Register, offset: 0x0050 */
        __IO uint32_t VAL0;                                                   /**< Value 0 Register, offset: 0x0054 */
        __IO uint32_t VAL1;                                                   /**< Value 1 Register, offset: 0x0058 */
        __IO uint32_t CVAL;                                                   /**< Capture Value Register, offset: 0x005C */
        __IO uint32_t RESERVEDS174[ 4];                                       /**< RESERVEDS174, offset: 0x0060 */

    } CH[8];

} eTMR_Type, *eTMR_MemMapPtr;

/** Number of instances of the eTMR module. */
#define eTMR_INSTANCE_COUNT                                                   (2U)

/* eTMR0  base address */
#define eTMR0_BASE                                                            (0x40049000U)
#define eTMR0                                                                 ((eTMR_Type *)(eTMR0_BASE))
/* eTMR1  base address */
#define eTMR1_BASE                                                            (0x4004A000U)
#define eTMR1                                                                 ((eTMR_Type *)(eTMR1_BASE))

/** Array initializer of eTMR peripheral base addresses */
#define eTMR_BASE_ADDRS                                                       { eTMR0_BASE,  eTMR1_BASE }
/** Array initializer of eTMR peripheral base pointers */
#define eTMR_BASE_PTRS                                                        { eTMR0,  eTMR1 }
/** Number of interrupt channels for the capture and compare interrupt type of eTMR. */
#define eTMR_CHN_IRQS_CH_COUNT                                                (8U)
#define eTMR_CHN_IRQS                                                         { {eTMR0_Ch0_Ch1_IRQn, eTMR0_Ch0_Ch1_IRQn, eTMR0_Ch2_Ch3_IRQn, eTMR0_Ch2_Ch3_IRQn, eTMR0_Ch4_Ch5_IRQn, eTMR0_Ch4_Ch5_IRQn, eTMR0_Ch6_Ch7_IRQn, eTMR0_Ch6_Ch7_IRQn}, {eTMR1_Ch0_Ch1_IRQn, eTMR1_Ch0_Ch1_IRQn, eTMR1_Ch2_Ch3_IRQn, eTMR1_Ch2_Ch3_IRQn, eTMR1_Ch4_Ch5_IRQn, eTMR1_Ch4_Ch5_IRQn, eTMR1_Ch6_Ch7_IRQn, eTMR1_Ch6_Ch7_IRQn} }
/** Number of interrupt channels for the fault interrupt type of eTMR. */
#define eTMR_FAULT_IRQS_CH_COUNT                                              (2U)
#define eTMR_FAULT_IRQS                                                       { eTMR0_Fault_IRQn, eTMR1_Fault_IRQn }
/** Number of interrupt channels for the counter overflow interrupt type of eTMR. */
#define eTMR_OVF_IRQS_CH_COUNT                                                (2U)
#define eTMR_OVF_IRQS                                                         { eTMR0_Ovf_IRQn, eTMR1_Ovf_IRQn }

/* eTMR Register Mask */
/* eTMR_STS Register */
#define eTMR_STS_QDDIR_MASK                                                   (0x8000U)
#define eTMR_STS_QDDIR_SHIFT                                                  (15U)
#define eTMR_STS_QDDIR(x)                                                     (((uint32_t)(((uint32_t)(x)) << eTMR_STS_QDDIR_SHIFT)) & eTMR_STS_QDDIR_MASK)
#define eTMR_STS_QDTOF_MASK                                                   (0x4000U)
#define eTMR_STS_QDTOF_SHIFT                                                  (14U)
#define eTMR_STS_QDTOF(x)                                                     (((uint32_t)(((uint32_t)(x)) << eTMR_STS_QDTOF_SHIFT)) & eTMR_STS_QDTOF_MASK)
#define eTMR_STS_TOF_MASK                                                     (0x2000U)
#define eTMR_STS_TOF_SHIFT                                                    (13U)
#define eTMR_STS_TOF(x)                                                       (((uint32_t)(((uint32_t)(x)) << eTMR_STS_TOF_SHIFT)) & eTMR_STS_TOF_MASK)
#define eTMR_STS_RF_MASK                                                      (0x1000U)
#define eTMR_STS_RF_SHIFT                                                     (12U)
#define eTMR_STS_RF(x)                                                        (((uint32_t)(((uint32_t)(x)) << eTMR_STS_RF_SHIFT)) & eTMR_STS_RF_MASK)
#define eTMR_STS_F3F_MASK                                                     (0x800U)
#define eTMR_STS_F3F_SHIFT                                                    (11U)
#define eTMR_STS_F3F(x)                                                       (((uint32_t)(((uint32_t)(x)) << eTMR_STS_F3F_SHIFT)) & eTMR_STS_F3F_MASK)
#define eTMR_STS_F2F_MASK                                                     (0x400U)
#define eTMR_STS_F2F_SHIFT                                                    (10U)
#define eTMR_STS_F2F(x)                                                       (((uint32_t)(((uint32_t)(x)) << eTMR_STS_F2F_SHIFT)) & eTMR_STS_F2F_MASK)
#define eTMR_STS_F1F_MASK                                                     (0x200U)
#define eTMR_STS_F1F_SHIFT                                                    (9U)
#define eTMR_STS_F1F(x)                                                       (((uint32_t)(((uint32_t)(x)) << eTMR_STS_F1F_SHIFT)) & eTMR_STS_F1F_MASK)
#define eTMR_STS_F0F_MASK                                                     (0x100U)
#define eTMR_STS_F0F_SHIFT                                                    (8U)
#define eTMR_STS_F0F(x)                                                       (((uint32_t)(((uint32_t)(x)) << eTMR_STS_F0F_SHIFT)) & eTMR_STS_F0F_MASK)
#define eTMR_STS_CH7F_MASK                                                    (0x80U)
#define eTMR_STS_CH7F_SHIFT                                                   (7U)
#define eTMR_STS_CH7F(x)                                                      (((uint32_t)(((uint32_t)(x)) << eTMR_STS_CH7F_SHIFT)) & eTMR_STS_CH7F_MASK)
#define eTMR_STS_CH6F_MASK                                                    (0x40U)
#define eTMR_STS_CH6F_SHIFT                                                   (6U)
#define eTMR_STS_CH6F(x)                                                      (((uint32_t)(((uint32_t)(x)) << eTMR_STS_CH6F_SHIFT)) & eTMR_STS_CH6F_MASK)
#define eTMR_STS_CH5F_MASK                                                    (0x20U)
#define eTMR_STS_CH5F_SHIFT                                                   (5U)
#define eTMR_STS_CH5F(x)                                                      (((uint32_t)(((uint32_t)(x)) << eTMR_STS_CH5F_SHIFT)) & eTMR_STS_CH5F_MASK)
#define eTMR_STS_CH4F_MASK                                                    (0x10U)
#define eTMR_STS_CH4F_SHIFT                                                   (4U)
#define eTMR_STS_CH4F(x)                                                      (((uint32_t)(((uint32_t)(x)) << eTMR_STS_CH4F_SHIFT)) & eTMR_STS_CH4F_MASK)
#define eTMR_STS_CH3F_MASK                                                    (0x8U)
#define eTMR_STS_CH3F_SHIFT                                                   (3U)
#define eTMR_STS_CH3F(x)                                                      (((uint32_t)(((uint32_t)(x)) << eTMR_STS_CH3F_SHIFT)) & eTMR_STS_CH3F_MASK)
#define eTMR_STS_CH2F_MASK                                                    (0x4U)
#define eTMR_STS_CH2F_SHIFT                                                   (2U)
#define eTMR_STS_CH2F(x)                                                      (((uint32_t)(((uint32_t)(x)) << eTMR_STS_CH2F_SHIFT)) & eTMR_STS_CH2F_MASK)
#define eTMR_STS_CH1F_MASK                                                    (0x2U)
#define eTMR_STS_CH1F_SHIFT                                                   (1U)
#define eTMR_STS_CH1F(x)                                                      (((uint32_t)(((uint32_t)(x)) << eTMR_STS_CH1F_SHIFT)) & eTMR_STS_CH1F_MASK)
#define eTMR_STS_CH0F_MASK                                                    (0x1U)
#define eTMR_STS_CH0F_SHIFT                                                   (0U)
#define eTMR_STS_CH0F(x)                                                      (((uint32_t)(((uint32_t)(x)) << eTMR_STS_CH0F_SHIFT)) & eTMR_STS_CH0F_MASK)
/* eTMR_IOSTS Register */
#define eTMR_IOSTS_PHB_MASK                                                   (0x2000000U)
#define eTMR_IOSTS_PHB_SHIFT                                                  (25U)
#define eTMR_IOSTS_PHB(x)                                                     (((uint32_t)(((uint32_t)(x)) << eTMR_IOSTS_PHB_SHIFT)) & eTMR_IOSTS_PHB_MASK)
#define eTMR_IOSTS_PHA_MASK                                                   (0x1000000U)
#define eTMR_IOSTS_PHA_SHIFT                                                  (24U)
#define eTMR_IOSTS_PHA(x)                                                     (((uint32_t)(((uint32_t)(x)) << eTMR_IOSTS_PHA_SHIFT)) & eTMR_IOSTS_PHA_MASK)
#define eTMR_IOSTS_F3_MASK                                                    (0x80000U)
#define eTMR_IOSTS_F3_SHIFT                                                   (19U)
#define eTMR_IOSTS_F3(x)                                                      (((uint32_t)(((uint32_t)(x)) << eTMR_IOSTS_F3_SHIFT)) & eTMR_IOSTS_F3_MASK)
#define eTMR_IOSTS_F2_MASK                                                    (0x40000U)
#define eTMR_IOSTS_F2_SHIFT                                                   (18U)
#define eTMR_IOSTS_F2(x)                                                      (((uint32_t)(((uint32_t)(x)) << eTMR_IOSTS_F2_SHIFT)) & eTMR_IOSTS_F2_MASK)
#define eTMR_IOSTS_F1_MASK                                                    (0x20000U)
#define eTMR_IOSTS_F1_SHIFT                                                   (17U)
#define eTMR_IOSTS_F1(x)                                                      (((uint32_t)(((uint32_t)(x)) << eTMR_IOSTS_F1_SHIFT)) & eTMR_IOSTS_F1_MASK)
#define eTMR_IOSTS_F0_MASK                                                    (0x10000U)
#define eTMR_IOSTS_F0_SHIFT                                                   (16U)
#define eTMR_IOSTS_F0(x)                                                      (((uint32_t)(((uint32_t)(x)) << eTMR_IOSTS_F0_SHIFT)) & eTMR_IOSTS_F0_MASK)
#define eTMR_IOSTS_CH7IO_MASK                                                 (0x80U)
#define eTMR_IOSTS_CH7IO_SHIFT                                                (7U)
#define eTMR_IOSTS_CH7IO(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_IOSTS_CH7IO_SHIFT)) & eTMR_IOSTS_CH7IO_MASK)
#define eTMR_IOSTS_CH6IO_MASK                                                 (0x40U)
#define eTMR_IOSTS_CH6IO_SHIFT                                                (6U)
#define eTMR_IOSTS_CH6IO(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_IOSTS_CH6IO_SHIFT)) & eTMR_IOSTS_CH6IO_MASK)
#define eTMR_IOSTS_CH5IO_MASK                                                 (0x20U)
#define eTMR_IOSTS_CH5IO_SHIFT                                                (5U)
#define eTMR_IOSTS_CH5IO(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_IOSTS_CH5IO_SHIFT)) & eTMR_IOSTS_CH5IO_MASK)
#define eTMR_IOSTS_CH4IO_MASK                                                 (0x10U)
#define eTMR_IOSTS_CH4IO_SHIFT                                                (4U)
#define eTMR_IOSTS_CH4IO(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_IOSTS_CH4IO_SHIFT)) & eTMR_IOSTS_CH4IO_MASK)
#define eTMR_IOSTS_CH3IO_MASK                                                 (0x8U)
#define eTMR_IOSTS_CH3IO_SHIFT                                                (3U)
#define eTMR_IOSTS_CH3IO(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_IOSTS_CH3IO_SHIFT)) & eTMR_IOSTS_CH3IO_MASK)
#define eTMR_IOSTS_CH2IO_MASK                                                 (0x4U)
#define eTMR_IOSTS_CH2IO_SHIFT                                                (2U)
#define eTMR_IOSTS_CH2IO(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_IOSTS_CH2IO_SHIFT)) & eTMR_IOSTS_CH2IO_MASK)
#define eTMR_IOSTS_CH1IO_MASK                                                 (0x2U)
#define eTMR_IOSTS_CH1IO_SHIFT                                                (1U)
#define eTMR_IOSTS_CH1IO(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_IOSTS_CH1IO_SHIFT)) & eTMR_IOSTS_CH1IO_MASK)
#define eTMR_IOSTS_CH0IO_MASK                                                 (0x1U)
#define eTMR_IOSTS_CH0IO_SHIFT                                                (0U)
#define eTMR_IOSTS_CH0IO(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_IOSTS_CH0IO_SHIFT)) & eTMR_IOSTS_CH0IO_MASK)
/* eTMR_CNT Register */
#define eTMR_CNT_CNT_MASK                                                     (0xFFFFU)
#define eTMR_CNT_CNT_SHIFT                                                    (0U)
#define eTMR_CNT_CNT(x)                                                       (((uint32_t)(((uint32_t)(x)) << eTMR_CNT_CNT_SHIFT)) & eTMR_CNT_CNT_MASK)
/* eTMR_QDCNT Register */
#define eTMR_QDCNT_QDCNT_MASK                                                 (0xFFFFU)
#define eTMR_QDCNT_QDCNT_SHIFT                                                (0U)
#define eTMR_QDCNT_QDCNT(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_QDCNT_QDCNT_SHIFT)) & eTMR_QDCNT_QDCNT_MASK)
/* eTMR_CTRL Register */
#define eTMR_CTRL_CAPTST_MASK                                                 (0x80000000U)
#define eTMR_CTRL_CAPTST_SHIFT                                                (31U)
#define eTMR_CTRL_CAPTST(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_CTRL_CAPTST_SHIFT)) & eTMR_CTRL_CAPTST_MASK)
#define eTMR_CTRL_DTPRS_WIDTH                                                 (2)
#define eTMR_CTRL_DTPRS_MASK                                                  (0x30000000U)
#define eTMR_CTRL_DTPRS_SHIFT                                                 (28U)
#define eTMR_CTRL_DTPRS(x)                                                    (((uint32_t)(((uint32_t)(x)) << eTMR_CTRL_DTPRS_SHIFT)) & eTMR_CTRL_DTPRS_MASK)
#define eTMR_CTRL_COMPL67_MASK                                                (0x8000000U)
#define eTMR_CTRL_COMPL67_SHIFT                                               (27U)
#define eTMR_CTRL_COMPL67(x)                                                  (((uint32_t)(((uint32_t)(x)) << eTMR_CTRL_COMPL67_SHIFT)) & eTMR_CTRL_COMPL67_MASK)
#define eTMR_CTRL_COMPL45_MASK                                                (0x4000000U)
#define eTMR_CTRL_COMPL45_SHIFT                                               (26U)
#define eTMR_CTRL_COMPL45(x)                                                  (((uint32_t)(((uint32_t)(x)) << eTMR_CTRL_COMPL45_SHIFT)) & eTMR_CTRL_COMPL45_MASK)
#define eTMR_CTRL_COMPL23_MASK                                                (0x2000000U)
#define eTMR_CTRL_COMPL23_SHIFT                                               (25U)
#define eTMR_CTRL_COMPL23(x)                                                  (((uint32_t)(((uint32_t)(x)) << eTMR_CTRL_COMPL23_SHIFT)) & eTMR_CTRL_COMPL23_MASK)
#define eTMR_CTRL_COMPL01_MASK                                                (0x1000000U)
#define eTMR_CTRL_COMPL01_SHIFT                                               (24U)
#define eTMR_CTRL_COMPL01(x)                                                  (((uint32_t)(((uint32_t)(x)) << eTMR_CTRL_COMPL01_SHIFT)) & eTMR_CTRL_COMPL01_MASK)
#define eTMR_CTRL_COMB67_MASK                                                 (0x800000U)
#define eTMR_CTRL_COMB67_SHIFT                                                (23U)
#define eTMR_CTRL_COMB67(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_CTRL_COMB67_SHIFT)) & eTMR_CTRL_COMB67_MASK)
#define eTMR_CTRL_COMB45_MASK                                                 (0x400000U)
#define eTMR_CTRL_COMB45_SHIFT                                                (22U)
#define eTMR_CTRL_COMB45(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_CTRL_COMB45_SHIFT)) & eTMR_CTRL_COMB45_MASK)
#define eTMR_CTRL_COMB23_MASK                                                 (0x200000U)
#define eTMR_CTRL_COMB23_SHIFT                                                (21U)
#define eTMR_CTRL_COMB23(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_CTRL_COMB23_SHIFT)) & eTMR_CTRL_COMB23_MASK)
#define eTMR_CTRL_COMB01_MASK                                                 (0x100000U)
#define eTMR_CTRL_COMB01_SHIFT                                                (20U)
#define eTMR_CTRL_COMB01(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_CTRL_COMB01_SHIFT)) & eTMR_CTRL_COMB01_MASK)
#define eTMR_CTRL_COMBSRC67_MASK                                              (0x80000U)
#define eTMR_CTRL_COMBSRC67_SHIFT                                             (19U)
#define eTMR_CTRL_COMBSRC67(x)                                                (((uint32_t)(((uint32_t)(x)) << eTMR_CTRL_COMBSRC67_SHIFT)) & eTMR_CTRL_COMBSRC67_MASK)
#define eTMR_CTRL_COMBSRC45_MASK                                              (0x40000U)
#define eTMR_CTRL_COMBSRC45_SHIFT                                             (18U)
#define eTMR_CTRL_COMBSRC45(x)                                                (((uint32_t)(((uint32_t)(x)) << eTMR_CTRL_COMBSRC45_SHIFT)) & eTMR_CTRL_COMBSRC45_MASK)
#define eTMR_CTRL_COMBSRC23_MASK                                              (0x20000U)
#define eTMR_CTRL_COMBSRC23_SHIFT                                             (17U)
#define eTMR_CTRL_COMBSRC23(x)                                                (((uint32_t)(((uint32_t)(x)) << eTMR_CTRL_COMBSRC23_SHIFT)) & eTMR_CTRL_COMBSRC23_MASK)
#define eTMR_CTRL_COMBSRC01_MASK                                              (0x10000U)
#define eTMR_CTRL_COMBSRC01_SHIFT                                             (16U)
#define eTMR_CTRL_COMBSRC01(x)                                                (((uint32_t)(((uint32_t)(x)) << eTMR_CTRL_COMBSRC01_SHIFT)) & eTMR_CTRL_COMBSRC01_MASK)
#define eTMR_CTRL_DBGDIS_MASK                                                 (0x8000U)
#define eTMR_CTRL_DBGDIS_SHIFT                                                (15U)
#define eTMR_CTRL_DBGDIS(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_CTRL_DBGDIS_SHIFT)) & eTMR_CTRL_DBGDIS_MASK)
#define eTMR_CTRL_CLKPRS_WIDTH                                                (7)
#define eTMR_CTRL_CLKPRS_MASK                                                 (0x7F00U)
#define eTMR_CTRL_CLKPRS_SHIFT                                                (8U)
#define eTMR_CTRL_CLKPRS(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_CTRL_CLKPRS_SHIFT)) & eTMR_CTRL_CLKPRS_MASK)
#define eTMR_CTRL_CLKSRC_MASK                                                 (0x10U)
#define eTMR_CTRL_CLKSRC_SHIFT                                                (4U)
#define eTMR_CTRL_CLKSRC(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_CTRL_CLKSRC_SHIFT)) & eTMR_CTRL_CLKSRC_MASK)
#define eTMR_CTRL_GLOBEN_MASK                                                 (0x2U)
#define eTMR_CTRL_GLOBEN_SHIFT                                                (1U)
#define eTMR_CTRL_GLOBEN(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_CTRL_GLOBEN_SHIFT)) & eTMR_CTRL_GLOBEN_MASK)
#define eTMR_CTRL_EN_MASK                                                     (0x1U)
#define eTMR_CTRL_EN_SHIFT                                                    (0U)
#define eTMR_CTRL_EN(x)                                                       (((uint32_t)(((uint32_t)(x)) << eTMR_CTRL_EN_SHIFT)) & eTMR_CTRL_EN_MASK)
/* eTMR_SYNC Register */
#define eTMR_SYNC_GLOB_MASK                                                   (0x80000000U)
#define eTMR_SYNC_GLOB_SHIFT                                                  (31U)
#define eTMR_SYNC_GLOB(x)                                                     (((uint32_t)(((uint32_t)(x)) << eTMR_SYNC_GLOB_SHIFT)) & eTMR_SYNC_GLOB_MASK)
#define eTMR_SYNC_LDFRQ_WIDTH                                                 (4)
#define eTMR_SYNC_LDFRQ_MASK                                                  (0xF000000U)
#define eTMR_SYNC_LDFRQ_SHIFT                                                 (24U)
#define eTMR_SYNC_LDFRQ(x)                                                    (((uint32_t)(((uint32_t)(x)) << eTMR_SYNC_LDFRQ_SHIFT)) & eTMR_SYNC_LDFRQ_MASK)
#define eTMR_SYNC_TRIG2EN_MASK                                                (0x80000U)
#define eTMR_SYNC_TRIG2EN_SHIFT                                               (19U)
#define eTMR_SYNC_TRIG2EN(x)                                                  (((uint32_t)(((uint32_t)(x)) << eTMR_SYNC_TRIG2EN_SHIFT)) & eTMR_SYNC_TRIG2EN_MASK)
#define eTMR_SYNC_TRIG1EN_MASK                                                (0x40000U)
#define eTMR_SYNC_TRIG1EN_SHIFT                                               (18U)
#define eTMR_SYNC_TRIG1EN(x)                                                  (((uint32_t)(((uint32_t)(x)) << eTMR_SYNC_TRIG1EN_SHIFT)) & eTMR_SYNC_TRIG1EN_MASK)
#define eTMR_SYNC_TRIG0EN_MASK                                                (0x20000U)
#define eTMR_SYNC_TRIG0EN_SHIFT                                               (17U)
#define eTMR_SYNC_TRIG0EN(x)                                                  (((uint32_t)(((uint32_t)(x)) << eTMR_SYNC_TRIG0EN_SHIFT)) & eTMR_SYNC_TRIG0EN_MASK)
#define eTMR_SYNC_SWTRIG_MASK                                                 (0x10000U)
#define eTMR_SYNC_SWTRIG_SHIFT                                                (16U)
#define eTMR_SYNC_SWTRIG(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_SYNC_SWTRIG_SHIFT)) & eTMR_SYNC_SWTRIG_MASK)
#define eTMR_SYNC_MASKLD_WIDTH                                                (2)
#define eTMR_SYNC_MASKLD_MASK                                                 (0xC000U)
#define eTMR_SYNC_MASKLD_SHIFT                                                (14U)
#define eTMR_SYNC_MASKLD(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_SYNC_MASKLD_SHIFT)) & eTMR_SYNC_MASKLD_MASK)
#define eTMR_SYNC_TRIGS2_WIDTH                                                (2)
#define eTMR_SYNC_TRIGS2_MASK                                                 (0x3000U)
#define eTMR_SYNC_TRIGS2_SHIFT                                                (12U)
#define eTMR_SYNC_TRIGS2(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_SYNC_TRIGS2_SHIFT)) & eTMR_SYNC_TRIGS2_MASK)
#define eTMR_SYNC_CNTLD_WIDTH                                                 (2)
#define eTMR_SYNC_CNTLD_MASK                                                  (0xC00U)
#define eTMR_SYNC_CNTLD_SHIFT                                                 (10U)
#define eTMR_SYNC_CNTLD(x)                                                    (((uint32_t)(((uint32_t)(x)) << eTMR_SYNC_CNTLD_SHIFT)) & eTMR_SYNC_CNTLD_MASK)
#define eTMR_SYNC_TRIGS1_WIDTH                                                (2)
#define eTMR_SYNC_TRIGS1_MASK                                                 (0x300U)
#define eTMR_SYNC_TRIGS1_SHIFT                                                (8U)
#define eTMR_SYNC_TRIGS1(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_SYNC_TRIGS1_SHIFT)) & eTMR_SYNC_TRIGS1_MASK)
#define eTMR_SYNC_REGLD_WIDTH                                                 (2)
#define eTMR_SYNC_REGLD_MASK                                                  (0xC0U)
#define eTMR_SYNC_REGLD_SHIFT                                                 (6U)
#define eTMR_SYNC_REGLD(x)                                                    (((uint32_t)(((uint32_t)(x)) << eTMR_SYNC_REGLD_SHIFT)) & eTMR_SYNC_REGLD_MASK)
#define eTMR_SYNC_TRIGS0_WIDTH                                                (2)
#define eTMR_SYNC_TRIGS0_MASK                                                 (0x30U)
#define eTMR_SYNC_TRIGS0_SHIFT                                                (4U)
#define eTMR_SYNC_TRIGS0(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_SYNC_TRIGS0_SHIFT)) & eTMR_SYNC_TRIGS0_MASK)
#define eTMR_SYNC_CLDOK_MASK                                                  (0x2U)
#define eTMR_SYNC_CLDOK_SHIFT                                                 (1U)
#define eTMR_SYNC_CLDOK(x)                                                    (((uint32_t)(((uint32_t)(x)) << eTMR_SYNC_CLDOK_SHIFT)) & eTMR_SYNC_CLDOK_MASK)
#define eTMR_SYNC_LDOK_MASK                                                   (0x1U)
#define eTMR_SYNC_LDOK_SHIFT                                                  (0U)
#define eTMR_SYNC_LDOK(x)                                                     (((uint32_t)(((uint32_t)(x)) << eTMR_SYNC_LDOK_SHIFT)) & eTMR_SYNC_LDOK_MASK)
/* eTMR_OTRIG Register */
#define eTMR_OTRIG_TFRQ_WIDTH                                                 (4)
#define eTMR_OTRIG_TFRQ_MASK                                                  (0xF000U)
#define eTMR_OTRIG_TFRQ_SHIFT                                                 (12U)
#define eTMR_OTRIG_TFRQ(x)                                                    (((uint32_t)(((uint32_t)(x)) << eTMR_OTRIG_TFRQ_SHIFT)) & eTMR_OTRIG_TFRQ_MASK)
#define eTMR_OTRIG_PULSESRC_WIDTH                                             (3)
#define eTMR_OTRIG_PULSESRC_MASK                                              (0x700U)
#define eTMR_OTRIG_PULSESRC_SHIFT                                             (8U)
#define eTMR_OTRIG_PULSESRC(x)                                                (((uint32_t)(((uint32_t)(x)) << eTMR_OTRIG_PULSESRC_SHIFT)) & eTMR_OTRIG_PULSESRC_MASK)
#define eTMR_OTRIG_TSRC_MASK                                                  (0x10U)
#define eTMR_OTRIG_TSRC_SHIFT                                                 (4U)
#define eTMR_OTRIG_TSRC(x)                                                    (((uint32_t)(((uint32_t)(x)) << eTMR_OTRIG_TSRC_SHIFT)) & eTMR_OTRIG_TSRC_MASK)
#define eTMR_OTRIG_MODTEN_MASK                                                (0x4U)
#define eTMR_OTRIG_MODTEN_SHIFT                                               (2U)
#define eTMR_OTRIG_MODTEN(x)                                                  (((uint32_t)(((uint32_t)(x)) << eTMR_OTRIG_MODTEN_SHIFT)) & eTMR_OTRIG_MODTEN_MASK)
/* eTMR_INTE Register */
#define eTMR_INTE_QDTOIE_MASK                                                 (0x4000U)
#define eTMR_INTE_QDTOIE_SHIFT                                                (14U)
#define eTMR_INTE_QDTOIE(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_INTE_QDTOIE_SHIFT)) & eTMR_INTE_QDTOIE_MASK)
#define eTMR_INTE_TOIE_MASK                                                   (0x2000U)
#define eTMR_INTE_TOIE_SHIFT                                                  (13U)
#define eTMR_INTE_TOIE(x)                                                     (((uint32_t)(((uint32_t)(x)) << eTMR_INTE_TOIE_SHIFT)) & eTMR_INTE_TOIE_MASK)
#define eTMR_INTE_FIE_MASK                                                    (0x100U)
#define eTMR_INTE_FIE_SHIFT                                                   (8U)
#define eTMR_INTE_FIE(x)                                                      (((uint32_t)(((uint32_t)(x)) << eTMR_INTE_FIE_SHIFT)) & eTMR_INTE_FIE_MASK)
#define eTMR_INTE_CH7IE_MASK                                                  (0x80U)
#define eTMR_INTE_CH7IE_SHIFT                                                 (7U)
#define eTMR_INTE_CH7IE(x)                                                    (((uint32_t)(((uint32_t)(x)) << eTMR_INTE_CH7IE_SHIFT)) & eTMR_INTE_CH7IE_MASK)
#define eTMR_INTE_CH6IE_MASK                                                  (0x40U)
#define eTMR_INTE_CH6IE_SHIFT                                                 (6U)
#define eTMR_INTE_CH6IE(x)                                                    (((uint32_t)(((uint32_t)(x)) << eTMR_INTE_CH6IE_SHIFT)) & eTMR_INTE_CH6IE_MASK)
#define eTMR_INTE_CH5IE_MASK                                                  (0x20U)
#define eTMR_INTE_CH5IE_SHIFT                                                 (5U)
#define eTMR_INTE_CH5IE(x)                                                    (((uint32_t)(((uint32_t)(x)) << eTMR_INTE_CH5IE_SHIFT)) & eTMR_INTE_CH5IE_MASK)
#define eTMR_INTE_CH4IE_MASK                                                  (0x10U)
#define eTMR_INTE_CH4IE_SHIFT                                                 (4U)
#define eTMR_INTE_CH4IE(x)                                                    (((uint32_t)(((uint32_t)(x)) << eTMR_INTE_CH4IE_SHIFT)) & eTMR_INTE_CH4IE_MASK)
#define eTMR_INTE_CH3IE_MASK                                                  (0x8U)
#define eTMR_INTE_CH3IE_SHIFT                                                 (3U)
#define eTMR_INTE_CH3IE(x)                                                    (((uint32_t)(((uint32_t)(x)) << eTMR_INTE_CH3IE_SHIFT)) & eTMR_INTE_CH3IE_MASK)
#define eTMR_INTE_CH2IE_MASK                                                  (0x4U)
#define eTMR_INTE_CH2IE_SHIFT                                                 (2U)
#define eTMR_INTE_CH2IE(x)                                                    (((uint32_t)(((uint32_t)(x)) << eTMR_INTE_CH2IE_SHIFT)) & eTMR_INTE_CH2IE_MASK)
#define eTMR_INTE_CH1IE_MASK                                                  (0x2U)
#define eTMR_INTE_CH1IE_SHIFT                                                 (1U)
#define eTMR_INTE_CH1IE(x)                                                    (((uint32_t)(((uint32_t)(x)) << eTMR_INTE_CH1IE_SHIFT)) & eTMR_INTE_CH1IE_MASK)
#define eTMR_INTE_CH0IE_MASK                                                  (0x1U)
#define eTMR_INTE_CH0IE_SHIFT                                                 (0U)
#define eTMR_INTE_CH0IE(x)                                                    (((uint32_t)(((uint32_t)(x)) << eTMR_INTE_CH0IE_SHIFT)) & eTMR_INTE_CH0IE_MASK)
/* eTMR_CHINIT Register */
#define eTMR_CHINIT_CH7INITV_MASK                                             (0x800000U)
#define eTMR_CHINIT_CH7INITV_SHIFT                                            (23U)
#define eTMR_CHINIT_CH7INITV(x)                                               (((uint32_t)(((uint32_t)(x)) << eTMR_CHINIT_CH7INITV_SHIFT)) & eTMR_CHINIT_CH7INITV_MASK)
#define eTMR_CHINIT_CH6INITV_MASK                                             (0x400000U)
#define eTMR_CHINIT_CH6INITV_SHIFT                                            (22U)
#define eTMR_CHINIT_CH6INITV(x)                                               (((uint32_t)(((uint32_t)(x)) << eTMR_CHINIT_CH6INITV_SHIFT)) & eTMR_CHINIT_CH6INITV_MASK)
#define eTMR_CHINIT_CH5INITV_MASK                                             (0x200000U)
#define eTMR_CHINIT_CH5INITV_SHIFT                                            (21U)
#define eTMR_CHINIT_CH5INITV(x)                                               (((uint32_t)(((uint32_t)(x)) << eTMR_CHINIT_CH5INITV_SHIFT)) & eTMR_CHINIT_CH5INITV_MASK)
#define eTMR_CHINIT_CH4INITV_MASK                                             (0x100000U)
#define eTMR_CHINIT_CH4INITV_SHIFT                                            (20U)
#define eTMR_CHINIT_CH4INITV(x)                                               (((uint32_t)(((uint32_t)(x)) << eTMR_CHINIT_CH4INITV_SHIFT)) & eTMR_CHINIT_CH4INITV_MASK)
#define eTMR_CHINIT_CH3INITV_MASK                                             (0x80000U)
#define eTMR_CHINIT_CH3INITV_SHIFT                                            (19U)
#define eTMR_CHINIT_CH3INITV(x)                                               (((uint32_t)(((uint32_t)(x)) << eTMR_CHINIT_CH3INITV_SHIFT)) & eTMR_CHINIT_CH3INITV_MASK)
#define eTMR_CHINIT_CH2INITV_MASK                                             (0x40000U)
#define eTMR_CHINIT_CH2INITV_SHIFT                                            (18U)
#define eTMR_CHINIT_CH2INITV(x)                                               (((uint32_t)(((uint32_t)(x)) << eTMR_CHINIT_CH2INITV_SHIFT)) & eTMR_CHINIT_CH2INITV_MASK)
#define eTMR_CHINIT_CH1INITV_MASK                                             (0x20000U)
#define eTMR_CHINIT_CH1INITV_SHIFT                                            (17U)
#define eTMR_CHINIT_CH1INITV(x)                                               (((uint32_t)(((uint32_t)(x)) << eTMR_CHINIT_CH1INITV_SHIFT)) & eTMR_CHINIT_CH1INITV_MASK)
#define eTMR_CHINIT_CH0INITV_MASK                                             (0x10000U)
#define eTMR_CHINIT_CH0INITV_SHIFT                                            (16U)
#define eTMR_CHINIT_CH0INITV(x)                                               (((uint32_t)(((uint32_t)(x)) << eTMR_CHINIT_CH0INITV_SHIFT)) & eTMR_CHINIT_CH0INITV_MASK)
#define eTMR_CHINIT_CH7INIT_MASK                                              (0x80U)
#define eTMR_CHINIT_CH7INIT_SHIFT                                             (7U)
#define eTMR_CHINIT_CH7INIT(x)                                                (((uint32_t)(((uint32_t)(x)) << eTMR_CHINIT_CH7INIT_SHIFT)) & eTMR_CHINIT_CH7INIT_MASK)
#define eTMR_CHINIT_CH6INIT_MASK                                              (0x40U)
#define eTMR_CHINIT_CH6INIT_SHIFT                                             (6U)
#define eTMR_CHINIT_CH6INIT(x)                                                (((uint32_t)(((uint32_t)(x)) << eTMR_CHINIT_CH6INIT_SHIFT)) & eTMR_CHINIT_CH6INIT_MASK)
#define eTMR_CHINIT_CH5INIT_MASK                                              (0x20U)
#define eTMR_CHINIT_CH5INIT_SHIFT                                             (5U)
#define eTMR_CHINIT_CH5INIT(x)                                                (((uint32_t)(((uint32_t)(x)) << eTMR_CHINIT_CH5INIT_SHIFT)) & eTMR_CHINIT_CH5INIT_MASK)
#define eTMR_CHINIT_CH4INIT_MASK                                              (0x10U)
#define eTMR_CHINIT_CH4INIT_SHIFT                                             (4U)
#define eTMR_CHINIT_CH4INIT(x)                                                (((uint32_t)(((uint32_t)(x)) << eTMR_CHINIT_CH4INIT_SHIFT)) & eTMR_CHINIT_CH4INIT_MASK)
#define eTMR_CHINIT_CH3INIT_MASK                                              (0x8U)
#define eTMR_CHINIT_CH3INIT_SHIFT                                             (3U)
#define eTMR_CHINIT_CH3INIT(x)                                                (((uint32_t)(((uint32_t)(x)) << eTMR_CHINIT_CH3INIT_SHIFT)) & eTMR_CHINIT_CH3INIT_MASK)
#define eTMR_CHINIT_CH2INIT_MASK                                              (0x4U)
#define eTMR_CHINIT_CH2INIT_SHIFT                                             (2U)
#define eTMR_CHINIT_CH2INIT(x)                                                (((uint32_t)(((uint32_t)(x)) << eTMR_CHINIT_CH2INIT_SHIFT)) & eTMR_CHINIT_CH2INIT_MASK)
#define eTMR_CHINIT_CH1INIT_MASK                                              (0x2U)
#define eTMR_CHINIT_CH1INIT_SHIFT                                             (1U)
#define eTMR_CHINIT_CH1INIT(x)                                                (((uint32_t)(((uint32_t)(x)) << eTMR_CHINIT_CH1INIT_SHIFT)) & eTMR_CHINIT_CH1INIT_MASK)
#define eTMR_CHINIT_CH0INIT_MASK                                              (0x1U)
#define eTMR_CHINIT_CH0INIT_SHIFT                                             (0U)
#define eTMR_CHINIT_CH0INIT(x)                                                (((uint32_t)(((uint32_t)(x)) << eTMR_CHINIT_CH0INIT_SHIFT)) & eTMR_CHINIT_CH0INIT_MASK)
/* eTMR_CHSWV Register */
#define eTMR_CHSWV_CH7SWV_MASK                                                (0x80U)
#define eTMR_CHSWV_CH7SWV_SHIFT                                               (7U)
#define eTMR_CHSWV_CH7SWV(x)                                                  (((uint32_t)(((uint32_t)(x)) << eTMR_CHSWV_CH7SWV_SHIFT)) & eTMR_CHSWV_CH7SWV_MASK)
#define eTMR_CHSWV_CH6SWV_MASK                                                (0x40U)
#define eTMR_CHSWV_CH6SWV_SHIFT                                               (6U)
#define eTMR_CHSWV_CH6SWV(x)                                                  (((uint32_t)(((uint32_t)(x)) << eTMR_CHSWV_CH6SWV_SHIFT)) & eTMR_CHSWV_CH6SWV_MASK)
#define eTMR_CHSWV_CH5SWV_MASK                                                (0x20U)
#define eTMR_CHSWV_CH5SWV_SHIFT                                               (5U)
#define eTMR_CHSWV_CH5SWV(x)                                                  (((uint32_t)(((uint32_t)(x)) << eTMR_CHSWV_CH5SWV_SHIFT)) & eTMR_CHSWV_CH5SWV_MASK)
#define eTMR_CHSWV_CH4SWV_MASK                                                (0x10U)
#define eTMR_CHSWV_CH4SWV_SHIFT                                               (4U)
#define eTMR_CHSWV_CH4SWV(x)                                                  (((uint32_t)(((uint32_t)(x)) << eTMR_CHSWV_CH4SWV_SHIFT)) & eTMR_CHSWV_CH4SWV_MASK)
#define eTMR_CHSWV_CH3SWV_MASK                                                (0x8U)
#define eTMR_CHSWV_CH3SWV_SHIFT                                               (3U)
#define eTMR_CHSWV_CH3SWV(x)                                                  (((uint32_t)(((uint32_t)(x)) << eTMR_CHSWV_CH3SWV_SHIFT)) & eTMR_CHSWV_CH3SWV_MASK)
#define eTMR_CHSWV_CH2SWV_MASK                                                (0x4U)
#define eTMR_CHSWV_CH2SWV_SHIFT                                               (2U)
#define eTMR_CHSWV_CH2SWV(x)                                                  (((uint32_t)(((uint32_t)(x)) << eTMR_CHSWV_CH2SWV_SHIFT)) & eTMR_CHSWV_CH2SWV_MASK)
#define eTMR_CHSWV_CH1SWV_MASK                                                (0x2U)
#define eTMR_CHSWV_CH1SWV_SHIFT                                               (1U)
#define eTMR_CHSWV_CH1SWV(x)                                                  (((uint32_t)(((uint32_t)(x)) << eTMR_CHSWV_CH1SWV_SHIFT)) & eTMR_CHSWV_CH1SWV_MASK)
#define eTMR_CHSWV_CH0SWV_MASK                                                (0x1U)
#define eTMR_CHSWV_CH0SWV_SHIFT                                               (0U)
#define eTMR_CHSWV_CH0SWV(x)                                                  (((uint32_t)(((uint32_t)(x)) << eTMR_CHSWV_CH0SWV_SHIFT)) & eTMR_CHSWV_CH0SWV_MASK)
/* eTMR_CHMASK Register */
#define eTMR_CHMASK_CH7MV_WIDTH                                               (2)
#define eTMR_CHMASK_CH7MV_MASK                                                (0xC0000000U)
#define eTMR_CHMASK_CH7MV_SHIFT                                               (30U)
#define eTMR_CHMASK_CH7MV(x)                                                  (((uint32_t)(((uint32_t)(x)) << eTMR_CHMASK_CH7MV_SHIFT)) & eTMR_CHMASK_CH7MV_MASK)
#define eTMR_CHMASK_CH6MV_WIDTH                                               (2)
#define eTMR_CHMASK_CH6MV_MASK                                                (0x30000000U)
#define eTMR_CHMASK_CH6MV_SHIFT                                               (28U)
#define eTMR_CHMASK_CH6MV(x)                                                  (((uint32_t)(((uint32_t)(x)) << eTMR_CHMASK_CH6MV_SHIFT)) & eTMR_CHMASK_CH6MV_MASK)
#define eTMR_CHMASK_CH5MV_WIDTH                                               (2)
#define eTMR_CHMASK_CH5MV_MASK                                                (0xC000000U)
#define eTMR_CHMASK_CH5MV_SHIFT                                               (26U)
#define eTMR_CHMASK_CH5MV(x)                                                  (((uint32_t)(((uint32_t)(x)) << eTMR_CHMASK_CH5MV_SHIFT)) & eTMR_CHMASK_CH5MV_MASK)
#define eTMR_CHMASK_CH4MV_WIDTH                                               (2)
#define eTMR_CHMASK_CH4MV_MASK                                                (0x3000000U)
#define eTMR_CHMASK_CH4MV_SHIFT                                               (24U)
#define eTMR_CHMASK_CH4MV(x)                                                  (((uint32_t)(((uint32_t)(x)) << eTMR_CHMASK_CH4MV_SHIFT)) & eTMR_CHMASK_CH4MV_MASK)
#define eTMR_CHMASK_CH3MV_WIDTH                                               (2)
#define eTMR_CHMASK_CH3MV_MASK                                                (0xC00000U)
#define eTMR_CHMASK_CH3MV_SHIFT                                               (22U)
#define eTMR_CHMASK_CH3MV(x)                                                  (((uint32_t)(((uint32_t)(x)) << eTMR_CHMASK_CH3MV_SHIFT)) & eTMR_CHMASK_CH3MV_MASK)
#define eTMR_CHMASK_CH2MV_WIDTH                                               (2)
#define eTMR_CHMASK_CH2MV_MASK                                                (0x300000U)
#define eTMR_CHMASK_CH2MV_SHIFT                                               (20U)
#define eTMR_CHMASK_CH2MV(x)                                                  (((uint32_t)(((uint32_t)(x)) << eTMR_CHMASK_CH2MV_SHIFT)) & eTMR_CHMASK_CH2MV_MASK)
#define eTMR_CHMASK_CH1MV_WIDTH                                               (2)
#define eTMR_CHMASK_CH1MV_MASK                                                (0xC0000U)
#define eTMR_CHMASK_CH1MV_SHIFT                                               (18U)
#define eTMR_CHMASK_CH1MV(x)                                                  (((uint32_t)(((uint32_t)(x)) << eTMR_CHMASK_CH1MV_SHIFT)) & eTMR_CHMASK_CH1MV_MASK)
#define eTMR_CHMASK_CH0MV_WIDTH                                               (2)
#define eTMR_CHMASK_CH0MV_MASK                                                (0x30000U)
#define eTMR_CHMASK_CH0MV_SHIFT                                               (16U)
#define eTMR_CHMASK_CH0MV(x)                                                  (((uint32_t)(((uint32_t)(x)) << eTMR_CHMASK_CH0MV_SHIFT)) & eTMR_CHMASK_CH0MV_MASK)
#define eTMR_CHMASK_CH7M_MASK                                                 (0x80U)
#define eTMR_CHMASK_CH7M_SHIFT                                                (7U)
#define eTMR_CHMASK_CH7M(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_CHMASK_CH7M_SHIFT)) & eTMR_CHMASK_CH7M_MASK)
#define eTMR_CHMASK_CH6M_MASK                                                 (0x40U)
#define eTMR_CHMASK_CH6M_SHIFT                                                (6U)
#define eTMR_CHMASK_CH6M(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_CHMASK_CH6M_SHIFT)) & eTMR_CHMASK_CH6M_MASK)
#define eTMR_CHMASK_CH5M_MASK                                                 (0x20U)
#define eTMR_CHMASK_CH5M_SHIFT                                                (5U)
#define eTMR_CHMASK_CH5M(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_CHMASK_CH5M_SHIFT)) & eTMR_CHMASK_CH5M_MASK)
#define eTMR_CHMASK_CH4M_MASK                                                 (0x10U)
#define eTMR_CHMASK_CH4M_SHIFT                                                (4U)
#define eTMR_CHMASK_CH4M(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_CHMASK_CH4M_SHIFT)) & eTMR_CHMASK_CH4M_MASK)
#define eTMR_CHMASK_CH3M_MASK                                                 (0x8U)
#define eTMR_CHMASK_CH3M_SHIFT                                                (3U)
#define eTMR_CHMASK_CH3M(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_CHMASK_CH3M_SHIFT)) & eTMR_CHMASK_CH3M_MASK)
#define eTMR_CHMASK_CH2M_MASK                                                 (0x4U)
#define eTMR_CHMASK_CH2M_SHIFT                                                (2U)
#define eTMR_CHMASK_CH2M(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_CHMASK_CH2M_SHIFT)) & eTMR_CHMASK_CH2M_MASK)
#define eTMR_CHMASK_CH1M_MASK                                                 (0x2U)
#define eTMR_CHMASK_CH1M_SHIFT                                                (1U)
#define eTMR_CHMASK_CH1M(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_CHMASK_CH1M_SHIFT)) & eTMR_CHMASK_CH1M_MASK)
#define eTMR_CHMASK_CH0M_MASK                                                 (0x1U)
#define eTMR_CHMASK_CH0M_SHIFT                                                (0U)
#define eTMR_CHMASK_CH0M(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_CHMASK_CH0M_SHIFT)) & eTMR_CHMASK_CH0M_MASK)
/* eTMR_CHFV Register */
#define eTMR_CHFV_CH7FV_WIDTH                                                 (2)
#define eTMR_CHFV_CH7FV_MASK                                                  (0xC000U)
#define eTMR_CHFV_CH7FV_SHIFT                                                 (14U)
#define eTMR_CHFV_CH7FV(x)                                                    (((uint32_t)(((uint32_t)(x)) << eTMR_CHFV_CH7FV_SHIFT)) & eTMR_CHFV_CH7FV_MASK)
#define eTMR_CHFV_CH6FV_WIDTH                                                 (2)
#define eTMR_CHFV_CH6FV_MASK                                                  (0x3000U)
#define eTMR_CHFV_CH6FV_SHIFT                                                 (12U)
#define eTMR_CHFV_CH6FV(x)                                                    (((uint32_t)(((uint32_t)(x)) << eTMR_CHFV_CH6FV_SHIFT)) & eTMR_CHFV_CH6FV_MASK)
#define eTMR_CHFV_CH5FV_WIDTH                                                 (2)
#define eTMR_CHFV_CH5FV_MASK                                                  (0xC00U)
#define eTMR_CHFV_CH5FV_SHIFT                                                 (10U)
#define eTMR_CHFV_CH5FV(x)                                                    (((uint32_t)(((uint32_t)(x)) << eTMR_CHFV_CH5FV_SHIFT)) & eTMR_CHFV_CH5FV_MASK)
#define eTMR_CHFV_CH4FV_WIDTH                                                 (2)
#define eTMR_CHFV_CH4FV_MASK                                                  (0x300U)
#define eTMR_CHFV_CH4FV_SHIFT                                                 (8U)
#define eTMR_CHFV_CH4FV(x)                                                    (((uint32_t)(((uint32_t)(x)) << eTMR_CHFV_CH4FV_SHIFT)) & eTMR_CHFV_CH4FV_MASK)
#define eTMR_CHFV_CH3FV_WIDTH                                                 (2)
#define eTMR_CHFV_CH3FV_MASK                                                  (0xC0U)
#define eTMR_CHFV_CH3FV_SHIFT                                                 (6U)
#define eTMR_CHFV_CH3FV(x)                                                    (((uint32_t)(((uint32_t)(x)) << eTMR_CHFV_CH3FV_SHIFT)) & eTMR_CHFV_CH3FV_MASK)
#define eTMR_CHFV_CH2FV_WIDTH                                                 (2)
#define eTMR_CHFV_CH2FV_MASK                                                  (0x30U)
#define eTMR_CHFV_CH2FV_SHIFT                                                 (4U)
#define eTMR_CHFV_CH2FV(x)                                                    (((uint32_t)(((uint32_t)(x)) << eTMR_CHFV_CH2FV_SHIFT)) & eTMR_CHFV_CH2FV_MASK)
#define eTMR_CHFV_CH1FV_WIDTH                                                 (2)
#define eTMR_CHFV_CH1FV_MASK                                                  (0xCU)
#define eTMR_CHFV_CH1FV_SHIFT                                                 (2U)
#define eTMR_CHFV_CH1FV(x)                                                    (((uint32_t)(((uint32_t)(x)) << eTMR_CHFV_CH1FV_SHIFT)) & eTMR_CHFV_CH1FV_MASK)
#define eTMR_CHFV_CH0FV_WIDTH                                                 (2)
#define eTMR_CHFV_CH0FV_MASK                                                  (0x3U)
#define eTMR_CHFV_CH0FV_SHIFT                                                 (0U)
#define eTMR_CHFV_CH0FV(x)                                                    (((uint32_t)(((uint32_t)(x)) << eTMR_CHFV_CH0FV_SHIFT)) & eTMR_CHFV_CH0FV_MASK)
/* eTMR_MOD Register */
#define eTMR_MOD_MOD_MASK                                                     (0xFFFFU)
#define eTMR_MOD_MOD_SHIFT                                                    (0U)
#define eTMR_MOD_MOD(x)                                                       (((uint32_t)(((uint32_t)(x)) << eTMR_MOD_MOD_SHIFT)) & eTMR_MOD_MOD_MASK)
/* eTMR_DT Register */
#define eTMR_DT_DTODD_WIDTH                                                   (8)
#define eTMR_DT_DTODD_MASK                                                    (0xFF0000U)
#define eTMR_DT_DTODD_SHIFT                                                   (16U)
#define eTMR_DT_DTODD(x)                                                      (((uint32_t)(((uint32_t)(x)) << eTMR_DT_DTODD_SHIFT)) & eTMR_DT_DTODD_MASK)
#define eTMR_DT_DTEVEN_WIDTH                                                  (8)
#define eTMR_DT_DTEVEN_MASK                                                   (0xFFU)
#define eTMR_DT_DTEVEN_SHIFT                                                  (0U)
#define eTMR_DT_DTEVEN(x)                                                     (((uint32_t)(((uint32_t)(x)) << eTMR_DT_DTEVEN_SHIFT)) & eTMR_DT_DTEVEN_MASK)
/* eTMR_QDCTRL Register */
#define eTMR_QDCTRL_QDFCNT_WIDTH                                              (4)
#define eTMR_QDCTRL_QDFCNT_MASK                                               (0xF0000000U)
#define eTMR_QDCTRL_QDFCNT_SHIFT                                              (28U)
#define eTMR_QDCTRL_QDFCNT(x)                                                 (((uint32_t)(((uint32_t)(x)) << eTMR_QDCTRL_QDFCNT_SHIFT)) & eTMR_QDCTRL_QDFCNT_MASK)
#define eTMR_QDCTRL_QDFPER_WIDTH                                              (4)
#define eTMR_QDCTRL_QDFPER_MASK                                               (0xF000000U)
#define eTMR_QDCTRL_QDFPER_SHIFT                                              (24U)
#define eTMR_QDCTRL_QDFPER(x)                                                 (((uint32_t)(((uint32_t)(x)) << eTMR_QDCTRL_QDFPER_SHIFT)) & eTMR_QDCTRL_QDFPER_MASK)
#define eTMR_QDCTRL_PHBPOL_MASK                                               (0x20000U)
#define eTMR_QDCTRL_PHBPOL_SHIFT                                              (17U)
#define eTMR_QDCTRL_PHBPOL(x)                                                 (((uint32_t)(((uint32_t)(x)) << eTMR_QDCTRL_PHBPOL_SHIFT)) & eTMR_QDCTRL_PHBPOL_MASK)
#define eTMR_QDCTRL_PHAPOL_MASK                                               (0x10000U)
#define eTMR_QDCTRL_PHAPOL_SHIFT                                              (16U)
#define eTMR_QDCTRL_PHAPOL(x)                                                 (((uint32_t)(((uint32_t)(x)) << eTMR_QDCTRL_PHAPOL_SHIFT)) & eTMR_QDCTRL_PHAPOL_MASK)
#define eTMR_QDCTRL_QDCLKPRS_WIDTH                                            (3)
#define eTMR_QDCTRL_QDCLKPRS_MASK                                             (0x7000U)
#define eTMR_QDCTRL_QDCLKPRS_SHIFT                                            (12U)
#define eTMR_QDCTRL_QDCLKPRS(x)                                               (((uint32_t)(((uint32_t)(x)) << eTMR_QDCTRL_QDCLKPRS_SHIFT)) & eTMR_QDCTRL_QDCLKPRS_MASK)
#define eTMR_QDCTRL_QDMODE_WIDTH                                              (2)
#define eTMR_QDCTRL_QDMODE_MASK                                               (0x30U)
#define eTMR_QDCTRL_QDMODE_SHIFT                                              (4U)
#define eTMR_QDCTRL_QDMODE(x)                                                 (((uint32_t)(((uint32_t)(x)) << eTMR_QDCTRL_QDMODE_SHIFT)) & eTMR_QDCTRL_QDMODE_MASK)
#define eTMR_QDCTRL_QDEN_MASK                                                 (0x1U)
#define eTMR_QDCTRL_QDEN_SHIFT                                                (0U)
#define eTMR_QDCTRL_QDEN(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_QDCTRL_QDEN_SHIFT)) & eTMR_QDCTRL_QDEN_MASK)
/* eTMR_FAULT Register */
#define eTMR_FAULT_FFCNT_WIDTH                                                (4)
#define eTMR_FAULT_FFCNT_MASK                                                 (0xF0000000U)
#define eTMR_FAULT_FFCNT_SHIFT                                                (28U)
#define eTMR_FAULT_FFCNT(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_FAULT_FFCNT_SHIFT)) & eTMR_FAULT_FFCNT_MASK)
#define eTMR_FAULT_FFPER_WIDTH                                                (4)
#define eTMR_FAULT_FFPER_MASK                                                 (0xF000000U)
#define eTMR_FAULT_FFPER_SHIFT                                                (24U)
#define eTMR_FAULT_FFPER(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_FAULT_FFPER_SHIFT)) & eTMR_FAULT_FFPER_MASK)
#define eTMR_FAULT_STRETCH_MASK                                               (0x200000U)
#define eTMR_FAULT_STRETCH_SHIFT                                              (21U)
#define eTMR_FAULT_STRETCH(x)                                                 (((uint32_t)(((uint32_t)(x)) << eTMR_FAULT_STRETCH_SHIFT)) & eTMR_FAULT_STRETCH_MASK)
#define eTMR_FAULT_FCOMB_MASK                                                 (0x100000U)
#define eTMR_FAULT_FCOMB_SHIFT                                                (20U)
#define eTMR_FAULT_FCOMB(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_FAULT_FCOMB_SHIFT)) & eTMR_FAULT_FCOMB_MASK)
#define eTMR_FAULT_RCVRY_WIDTH                                                (2)
#define eTMR_FAULT_RCVRY_MASK                                                 (0xC0000U)
#define eTMR_FAULT_RCVRY_SHIFT                                                (18U)
#define eTMR_FAULT_RCVRY(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_FAULT_RCVRY_SHIFT)) & eTMR_FAULT_RCVRY_MASK)
#define eTMR_FAULT_FAUTO_MASK                                                 (0x30000U)
#define eTMR_FAULT_FAUTO_SHIFT                                                (16U)
#define eTMR_FAULT_FAUTO(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_FAULT_FAUTO_SHIFT)) & eTMR_FAULT_FAUTO_MASK)
#define eTMR_FAULT_F3POL_MASK                                                 (0x800U)
#define eTMR_FAULT_F3POL_SHIFT                                                (11U)
#define eTMR_FAULT_F3POL(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_FAULT_F3POL_SHIFT)) & eTMR_FAULT_F3POL_MASK)
#define eTMR_FAULT_F2POL_MASK                                                 (0x400U)
#define eTMR_FAULT_F2POL_SHIFT                                                (10U)
#define eTMR_FAULT_F2POL(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_FAULT_F2POL_SHIFT)) & eTMR_FAULT_F2POL_MASK)
#define eTMR_FAULT_F1POL_MASK                                                 (0x200U)
#define eTMR_FAULT_F1POL_SHIFT                                                (9U)
#define eTMR_FAULT_F1POL(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_FAULT_F1POL_SHIFT)) & eTMR_FAULT_F1POL_MASK)
#define eTMR_FAULT_F0POL_MASK                                                 (0x100U)
#define eTMR_FAULT_F0POL_SHIFT                                                (8U)
#define eTMR_FAULT_F0POL(x)                                                   (((uint32_t)(((uint32_t)(x)) << eTMR_FAULT_F0POL_SHIFT)) & eTMR_FAULT_F0POL_MASK)
#define eTMR_FAULT_F3EN_MASK                                                  (0x8U)
#define eTMR_FAULT_F3EN_SHIFT                                                 (3U)
#define eTMR_FAULT_F3EN(x)                                                    (((uint32_t)(((uint32_t)(x)) << eTMR_FAULT_F3EN_SHIFT)) & eTMR_FAULT_F3EN_MASK)
#define eTMR_FAULT_F2EN_MASK                                                  (0x4U)
#define eTMR_FAULT_F2EN_SHIFT                                                 (2U)
#define eTMR_FAULT_F2EN(x)                                                    (((uint32_t)(((uint32_t)(x)) << eTMR_FAULT_F2EN_SHIFT)) & eTMR_FAULT_F2EN_MASK)
#define eTMR_FAULT_F1EN_MASK                                                  (0x2U)
#define eTMR_FAULT_F1EN_SHIFT                                                 (1U)
#define eTMR_FAULT_F1EN(x)                                                    (((uint32_t)(((uint32_t)(x)) << eTMR_FAULT_F1EN_SHIFT)) & eTMR_FAULT_F1EN_MASK)
#define eTMR_FAULT_F0EN_MASK                                                  (0x1U)
#define eTMR_FAULT_F0EN_SHIFT                                                 (0U)
#define eTMR_FAULT_F0EN(x)                                                    (((uint32_t)(((uint32_t)(x)) << eTMR_FAULT_F0EN_SHIFT)) & eTMR_FAULT_F0EN_MASK)
/* eTMR_CH_CTRL Register */
#define eTMR_CH_CTRL_DMAEN_MASK                                               (0x40000000U)
#define eTMR_CH_CTRL_DMAEN_SHIFT                                              (30U)
#define eTMR_CH_CTRL_DMAEN(x)                                                 (((uint32_t)(((uint32_t)(x)) << eTMR_CH_CTRL_DMAEN_SHIFT)) & eTMR_CH_CTRL_DMAEN_MASK)
#define eTMR_CH_CTRL_LDEN_MASK                                                (0x10000000U)
#define eTMR_CH_CTRL_LDEN_SHIFT                                               (28U)
#define eTMR_CH_CTRL_LDEN(x)                                                  (((uint32_t)(((uint32_t)(x)) << eTMR_CH_CTRL_LDEN_SHIFT)) & eTMR_CH_CTRL_LDEN_MASK)
#define eTMR_CH_CTRL_CAPHOLD_MASK                                             (0x8000000U)
#define eTMR_CH_CTRL_CAPHOLD_SHIFT                                            (27U)
#define eTMR_CH_CTRL_CAPHOLD(x)                                               (((uint32_t)(((uint32_t)(x)) << eTMR_CH_CTRL_CAPHOLD_SHIFT)) & eTMR_CH_CTRL_CAPHOLD_MASK)
#define eTMR_CH_CTRL_CAPEDGE_WIDTH                                            (2)
#define eTMR_CH_CTRL_CAPEDGE_MASK                                             (0x3000000U)
#define eTMR_CH_CTRL_CAPEDGE_SHIFT                                            (24U)
#define eTMR_CH_CTRL_CAPEDGE(x)                                               (((uint32_t)(((uint32_t)(x)) << eTMR_CH_CTRL_CAPEDGE_SHIFT)) & eTMR_CH_CTRL_CAPEDGE_MASK)
#define eTMR_CH_CTRL_CFCNT_WIDTH                                              (4)
#define eTMR_CH_CTRL_CFCNT_MASK                                               (0xF00000U)
#define eTMR_CH_CTRL_CFCNT_SHIFT                                              (20U)
#define eTMR_CH_CTRL_CFCNT(x)                                                 (((uint32_t)(((uint32_t)(x)) << eTMR_CH_CTRL_CFCNT_SHIFT)) & eTMR_CH_CTRL_CFCNT_MASK)
#define eTMR_CH_CTRL_CFPER_WIDTH                                              (4)
#define eTMR_CH_CTRL_CFPER_MASK                                               (0xF0000U)
#define eTMR_CH_CTRL_CFPER_SHIFT                                              (16U)
#define eTMR_CH_CTRL_CFPER(x)                                                 (((uint32_t)(((uint32_t)(x)) << eTMR_CH_CTRL_CFPER_SHIFT)) & eTMR_CH_CTRL_CFPER_MASK)
#define eTMR_CH_CTRL_VAL1CMP_WIDTH                                            (2)
#define eTMR_CH_CTRL_VAL1CMP_MASK                                             (0xC000U)
#define eTMR_CH_CTRL_VAL1CMP_SHIFT                                            (14U)
#define eTMR_CH_CTRL_VAL1CMP(x)                                               (((uint32_t)(((uint32_t)(x)) << eTMR_CH_CTRL_VAL1CMP_SHIFT)) & eTMR_CH_CTRL_VAL1CMP_MASK)
#define eTMR_CH_CTRL_VAL0CMP_WIDTH                                            (2)
#define eTMR_CH_CTRL_VAL0CMP_MASK                                             (0x3000U)
#define eTMR_CH_CTRL_VAL0CMP_SHIFT                                            (12U)
#define eTMR_CH_CTRL_VAL0CMP(x)                                               (((uint32_t)(((uint32_t)(x)) << eTMR_CH_CTRL_VAL0CMP_SHIFT)) & eTMR_CH_CTRL_VAL0CMP_MASK)
#define eTMR_CH_CTRL_PWMSRC_MASK                                              (0x300U)
#define eTMR_CH_CTRL_PWMSRC_SHIFT                                             (8U)
#define eTMR_CH_CTRL_PWMSRC(x)                                                (((uint32_t)(((uint32_t)(x)) << eTMR_CH_CTRL_PWMSRC_SHIFT)) & eTMR_CH_CTRL_PWMSRC_MASK)
#define eTMR_CH_CTRL_VAL1TEN_MASK                                             (0x80U)
#define eTMR_CH_CTRL_VAL1TEN_SHIFT                                            (7U)
#define eTMR_CH_CTRL_VAL1TEN(x)                                               (((uint32_t)(((uint32_t)(x)) << eTMR_CH_CTRL_VAL1TEN_SHIFT)) & eTMR_CH_CTRL_VAL1TEN_MASK)
#define eTMR_CH_CTRL_VAL0TEN_MASK                                             (0x40U)
#define eTMR_CH_CTRL_VAL0TEN_SHIFT                                            (6U)
#define eTMR_CH_CTRL_VAL0TEN(x)                                               (((uint32_t)(((uint32_t)(x)) << eTMR_CH_CTRL_VAL0TEN_SHIFT)) & eTMR_CH_CTRL_VAL0TEN_MASK)
#define eTMR_CH_CTRL_CHPOL_MASK                                               (0x20U)
#define eTMR_CH_CTRL_CHPOL_SHIFT                                              (5U)
#define eTMR_CH_CTRL_CHPOL(x)                                                 (((uint32_t)(((uint32_t)(x)) << eTMR_CH_CTRL_CHPOL_SHIFT)) & eTMR_CH_CTRL_CHPOL_MASK)
#define eTMR_CH_CTRL_CHMODE_WIDTH                                             (2)
#define eTMR_CH_CTRL_CHMODE_MASK                                              (0x3U)
#define eTMR_CH_CTRL_CHMODE_SHIFT                                             (0U)
#define eTMR_CH_CTRL_CHMODE(x)                                                (((uint32_t)(((uint32_t)(x)) << eTMR_CH_CTRL_CHMODE_SHIFT)) & eTMR_CH_CTRL_CHMODE_MASK)
/* eTMR_CH_VAL0 Register */
#define eTMR_CH_VAL0_VAL0_MASK                                                (0xFFFFU)
#define eTMR_CH_VAL0_VAL0_SHIFT                                               (0U)
#define eTMR_CH_VAL0_VAL0(x)                                                  (((uint32_t)(((uint32_t)(x)) << eTMR_CH_VAL0_VAL0_SHIFT)) & eTMR_CH_VAL0_VAL0_MASK)
/* eTMR_CH_VAL1 Register */
#define eTMR_CH_VAL1_VAL1_MASK                                                (0xFFFFU)
#define eTMR_CH_VAL1_VAL1_SHIFT                                               (0U)
#define eTMR_CH_VAL1_VAL1(x)                                                  (((uint32_t)(((uint32_t)(x)) << eTMR_CH_VAL1_VAL1_SHIFT)) & eTMR_CH_VAL1_VAL1_MASK)
/* eTMR_CH_CVAL Register */
#define eTMR_CH_CVAL_CVAL_MASK                                                (0xFFFFU)
#define eTMR_CH_CVAL_CVAL_SHIFT                                               (0U)
#define eTMR_CH_CVAL_CVAL(x)                                                  (((uint32_t)(((uint32_t)(x)) << eTMR_CH_CVAL_CVAL_SHIFT)) & eTMR_CH_CVAL_CVAL_MASK)



/*!
 * @}
 */ /* end of group eTMR_Register_Masks */


/*!
 * @}
 */ /* end of group eTMR_Peripheral_Access_Layer */


/* -------------------------------------------------------------
   -- MPWM Peripheral Access Layer
   ------------------------------------------------------------- */

/*!
 * @addtogroup MPWM_Peripheral_Access_Layer MPWM Peripheral Access Layer
 * @{
 */


/** MPWM - Size of Registers Arrays */
#define MPWM_CH_COUNT                                                         (8U)

/* MPWM Register Layout Typedef */
typedef struct {
    __I  uint32_t IOSTS;                                                      /**< IO Status Register, offset: 0x0000 */
    __IO uint32_t STS0;                                                       /**< Status 0 Register, offset: 0x0004 */
    __IO uint32_t STS1;                                                       /**< Status 1 Register, offset: 0x0008 */
    __IO uint32_t INTE0;                                                      /**< Interrupt Enable 0 Register, offset: 0x000C */
    __IO uint32_t INTE1;                                                      /**< Interrupt Enable 1 Register, offset: 0x0010 */
    __IO uint32_t CTRL;                                                       /**< Control Register, offset: 0x0014 */
    __O  uint32_t SWT;                                                        /**< Software Trigger Register, offset: 0x0018 */
    __IO uint32_t INIT;                                                       /**< Channel Initial Register, offset: 0x001C */
    __IO uint32_t POL;                                                        /**< Channel Polarity Register, offset: 0x0020 */
    __IO uint32_t LDOK;                                                       /**< Load OK Register, offset: 0x0024 */
    __IO uint32_t CLDOK;                                                      /**< Clear Load OK Register, offset: 0x0028 */
    __IO uint32_t COMBCMP;                                                    /**< Combination Capture Register, offset: 0x002C */
    __IO uint8_t  RESERVED12[16];                                             /**< RESERVED12, offset: 0x0030 */
    struct {
        __IO uint32_t CFG;                                                    /**< Channel Configuration Register, offset: 0x0040 */
        __I  uint32_t CNT;                                                    /**< Counter Status Register, offset: 0x0044 */
        __I  uint32_t CAP;                                                    /**< Capture Status Register, offset: 0x0048 */
        __IO uint32_t RESERVEDS133[ 1];                                       /**< RESERVEDS133, offset: 0x004C */
        __IO uint32_t PERIOD;                                                 /**< Period Register, offset: 0x0050 */
        __IO uint32_t CMP;                                                    /**< Compare Register, offset: 0x0054 */
        __IO uint32_t RESERVEDS136[ 2];                                       /**< RESERVEDS136, offset: 0x0058 */

    } CH[8];

} MPWM_Type, *MPWM_MemMapPtr;

/** Number of instances of the MPWM module. */
#define MPWM_INSTANCE_COUNT                                                   (1U)

/* MPWM0  base address */
#define MPWM0_BASE                                                            (0x4004F000U)
#define MPWM0                                                                 ((MPWM_Type *)(MPWM0_BASE))

/** Array initializer of MPWM peripheral base addresses */
#define MPWM_BASE_ADDRS                                                       { MPWM0_BASE }
/** Array initializer of MPWM peripheral base pointers */
#define MPWM_BASE_PTRS                                                        { MPWM0 }
/** Number of interrupt channels for the capture and compare interrupt type of MPWM. */
#define MPWM_CHN_IRQS_CH_COUNT                                                (8U)
#define MPWM_CHN_IRQS                                                         { {MPWM0_Ch0_IRQn, MPWM0_Ch1_IRQn, MPWM0_Ch2_IRQn, MPWM0_Ch3_IRQn, MPWM0_Ch4_IRQn, MPWM0_Ch5_IRQn, MPWM0_Ch6_IRQn, MPWM0_Ch7_IRQn} }
/** Number of interrupt channels for the counter overflow interrupt type of MPWM. */
#define MPWM_OVF_IRQS_CH_COUNT                                                (8U)
#define MPWM_OVF_IRQS                                                         { {MPWM0_Ch0_Ovf_IRQn, MPWM0_Ch1_Ovf_IRQn, MPWM0_Ch2_Ovf_IRQn, MPWM0_Ch3_Ovf_IRQn, MPWM0_Ch4_Ovf_IRQn, MPWM0_Ch5_Ovf_IRQn, MPWM0_Ch6_Ovf_IRQn, MPWM0_Ch7_Ovf_IRQn} }

/* MPWM Register Mask */
/* MPWM_IOSTS Register */
#define MPWM_IOSTS_CH7IO_MASK                                                 (0x80U)
#define MPWM_IOSTS_CH7IO_SHIFT                                                (7U)
#define MPWM_IOSTS_CH7IO(x)                                                   (((uint32_t)(((uint32_t)(x)) << MPWM_IOSTS_CH7IO_SHIFT)) & MPWM_IOSTS_CH7IO_MASK)
#define MPWM_IOSTS_CH6IO_MASK                                                 (0x40U)
#define MPWM_IOSTS_CH6IO_SHIFT                                                (6U)
#define MPWM_IOSTS_CH6IO(x)                                                   (((uint32_t)(((uint32_t)(x)) << MPWM_IOSTS_CH6IO_SHIFT)) & MPWM_IOSTS_CH6IO_MASK)
#define MPWM_IOSTS_CH5IO_MASK                                                 (0x20U)
#define MPWM_IOSTS_CH5IO_SHIFT                                                (5U)
#define MPWM_IOSTS_CH5IO(x)                                                   (((uint32_t)(((uint32_t)(x)) << MPWM_IOSTS_CH5IO_SHIFT)) & MPWM_IOSTS_CH5IO_MASK)
#define MPWM_IOSTS_CH4IO_MASK                                                 (0x10U)
#define MPWM_IOSTS_CH4IO_SHIFT                                                (4U)
#define MPWM_IOSTS_CH4IO(x)                                                   (((uint32_t)(((uint32_t)(x)) << MPWM_IOSTS_CH4IO_SHIFT)) & MPWM_IOSTS_CH4IO_MASK)
#define MPWM_IOSTS_CH3IO_MASK                                                 (0x8U)
#define MPWM_IOSTS_CH3IO_SHIFT                                                (3U)
#define MPWM_IOSTS_CH3IO(x)                                                   (((uint32_t)(((uint32_t)(x)) << MPWM_IOSTS_CH3IO_SHIFT)) & MPWM_IOSTS_CH3IO_MASK)
#define MPWM_IOSTS_CH2IO_MASK                                                 (0x4U)
#define MPWM_IOSTS_CH2IO_SHIFT                                                (2U)
#define MPWM_IOSTS_CH2IO(x)                                                   (((uint32_t)(((uint32_t)(x)) << MPWM_IOSTS_CH2IO_SHIFT)) & MPWM_IOSTS_CH2IO_MASK)
#define MPWM_IOSTS_CH1IO_MASK                                                 (0x2U)
#define MPWM_IOSTS_CH1IO_SHIFT                                                (1U)
#define MPWM_IOSTS_CH1IO(x)                                                   (((uint32_t)(((uint32_t)(x)) << MPWM_IOSTS_CH1IO_SHIFT)) & MPWM_IOSTS_CH1IO_MASK)
#define MPWM_IOSTS_CH0IO_MASK                                                 (0x1U)
#define MPWM_IOSTS_CH0IO_SHIFT                                                (0U)
#define MPWM_IOSTS_CH0IO(x)                                                   (((uint32_t)(((uint32_t)(x)) << MPWM_IOSTS_CH0IO_SHIFT)) & MPWM_IOSTS_CH0IO_MASK)
/* MPWM_STS0 Register */
#define MPWM_STS0_CH7TOF_MASK                                                 (0x80U)
#define MPWM_STS0_CH7TOF_SHIFT                                                (7U)
#define MPWM_STS0_CH7TOF(x)                                                   (((uint32_t)(((uint32_t)(x)) << MPWM_STS0_CH7TOF_SHIFT)) & MPWM_STS0_CH7TOF_MASK)
#define MPWM_STS0_CH6TOF_MASK                                                 (0x40U)
#define MPWM_STS0_CH6TOF_SHIFT                                                (6U)
#define MPWM_STS0_CH6TOF(x)                                                   (((uint32_t)(((uint32_t)(x)) << MPWM_STS0_CH6TOF_SHIFT)) & MPWM_STS0_CH6TOF_MASK)
#define MPWM_STS0_CH5TOF_MASK                                                 (0x20U)
#define MPWM_STS0_CH5TOF_SHIFT                                                (5U)
#define MPWM_STS0_CH5TOF(x)                                                   (((uint32_t)(((uint32_t)(x)) << MPWM_STS0_CH5TOF_SHIFT)) & MPWM_STS0_CH5TOF_MASK)
#define MPWM_STS0_CH4TOF_MASK                                                 (0x10U)
#define MPWM_STS0_CH4TOF_SHIFT                                                (4U)
#define MPWM_STS0_CH4TOF(x)                                                   (((uint32_t)(((uint32_t)(x)) << MPWM_STS0_CH4TOF_SHIFT)) & MPWM_STS0_CH4TOF_MASK)
#define MPWM_STS0_CH3TOF_MASK                                                 (0x8U)
#define MPWM_STS0_CH3TOF_SHIFT                                                (3U)
#define MPWM_STS0_CH3TOF(x)                                                   (((uint32_t)(((uint32_t)(x)) << MPWM_STS0_CH3TOF_SHIFT)) & MPWM_STS0_CH3TOF_MASK)
#define MPWM_STS0_CH2TOF_MASK                                                 (0x4U)
#define MPWM_STS0_CH2TOF_SHIFT                                                (2U)
#define MPWM_STS0_CH2TOF(x)                                                   (((uint32_t)(((uint32_t)(x)) << MPWM_STS0_CH2TOF_SHIFT)) & MPWM_STS0_CH2TOF_MASK)
#define MPWM_STS0_CH1TOF_MASK                                                 (0x2U)
#define MPWM_STS0_CH1TOF_SHIFT                                                (1U)
#define MPWM_STS0_CH1TOF(x)                                                   (((uint32_t)(((uint32_t)(x)) << MPWM_STS0_CH1TOF_SHIFT)) & MPWM_STS0_CH1TOF_MASK)
#define MPWM_STS0_CH0TOF_MASK                                                 (0x1U)
#define MPWM_STS0_CH0TOF_SHIFT                                                (0U)
#define MPWM_STS0_CH0TOF(x)                                                   (((uint32_t)(((uint32_t)(x)) << MPWM_STS0_CH0TOF_SHIFT)) & MPWM_STS0_CH0TOF_MASK)
/* MPWM_STS1 Register */
#define MPWM_STS1_CH7F_MASK                                                   (0x80U)
#define MPWM_STS1_CH7F_SHIFT                                                  (7U)
#define MPWM_STS1_CH7F(x)                                                     (((uint32_t)(((uint32_t)(x)) << MPWM_STS1_CH7F_SHIFT)) & MPWM_STS1_CH7F_MASK)
#define MPWM_STS1_CH6F_MASK                                                   (0x40U)
#define MPWM_STS1_CH6F_SHIFT                                                  (6U)
#define MPWM_STS1_CH6F(x)                                                     (((uint32_t)(((uint32_t)(x)) << MPWM_STS1_CH6F_SHIFT)) & MPWM_STS1_CH6F_MASK)
#define MPWM_STS1_CH5F_MASK                                                   (0x20U)
#define MPWM_STS1_CH5F_SHIFT                                                  (5U)
#define MPWM_STS1_CH5F(x)                                                     (((uint32_t)(((uint32_t)(x)) << MPWM_STS1_CH5F_SHIFT)) & MPWM_STS1_CH5F_MASK)
#define MPWM_STS1_CH4F_MASK                                                   (0x10U)
#define MPWM_STS1_CH4F_SHIFT                                                  (4U)
#define MPWM_STS1_CH4F(x)                                                     (((uint32_t)(((uint32_t)(x)) << MPWM_STS1_CH4F_SHIFT)) & MPWM_STS1_CH4F_MASK)
#define MPWM_STS1_CH3F_MASK                                                   (0x8U)
#define MPWM_STS1_CH3F_SHIFT                                                  (3U)
#define MPWM_STS1_CH3F(x)                                                     (((uint32_t)(((uint32_t)(x)) << MPWM_STS1_CH3F_SHIFT)) & MPWM_STS1_CH3F_MASK)
#define MPWM_STS1_CH2F_MASK                                                   (0x4U)
#define MPWM_STS1_CH2F_SHIFT                                                  (2U)
#define MPWM_STS1_CH2F(x)                                                     (((uint32_t)(((uint32_t)(x)) << MPWM_STS1_CH2F_SHIFT)) & MPWM_STS1_CH2F_MASK)
#define MPWM_STS1_CH1F_MASK                                                   (0x2U)
#define MPWM_STS1_CH1F_SHIFT                                                  (1U)
#define MPWM_STS1_CH1F(x)                                                     (((uint32_t)(((uint32_t)(x)) << MPWM_STS1_CH1F_SHIFT)) & MPWM_STS1_CH1F_MASK)
#define MPWM_STS1_CH0F_MASK                                                   (0x1U)
#define MPWM_STS1_CH0F_SHIFT                                                  (0U)
#define MPWM_STS1_CH0F(x)                                                     (((uint32_t)(((uint32_t)(x)) << MPWM_STS1_CH0F_SHIFT)) & MPWM_STS1_CH0F_MASK)
/* MPWM_INTE0 Register */
#define MPWM_INTE0_CH7TOIE_MASK                                               (0x80U)
#define MPWM_INTE0_CH7TOIE_SHIFT                                              (7U)
#define MPWM_INTE0_CH7TOIE(x)                                                 (((uint32_t)(((uint32_t)(x)) << MPWM_INTE0_CH7TOIE_SHIFT)) & MPWM_INTE0_CH7TOIE_MASK)
#define MPWM_INTE0_CH6TOIE_MASK                                               (0x40U)
#define MPWM_INTE0_CH6TOIE_SHIFT                                              (6U)
#define MPWM_INTE0_CH6TOIE(x)                                                 (((uint32_t)(((uint32_t)(x)) << MPWM_INTE0_CH6TOIE_SHIFT)) & MPWM_INTE0_CH6TOIE_MASK)
#define MPWM_INTE0_CH5TOIE_MASK                                               (0x20U)
#define MPWM_INTE0_CH5TOIE_SHIFT                                              (5U)
#define MPWM_INTE0_CH5TOIE(x)                                                 (((uint32_t)(((uint32_t)(x)) << MPWM_INTE0_CH5TOIE_SHIFT)) & MPWM_INTE0_CH5TOIE_MASK)
#define MPWM_INTE0_CH4TOIE_MASK                                               (0x10U)
#define MPWM_INTE0_CH4TOIE_SHIFT                                              (4U)
#define MPWM_INTE0_CH4TOIE(x)                                                 (((uint32_t)(((uint32_t)(x)) << MPWM_INTE0_CH4TOIE_SHIFT)) & MPWM_INTE0_CH4TOIE_MASK)
#define MPWM_INTE0_CH3TOIE_MASK                                               (0x8U)
#define MPWM_INTE0_CH3TOIE_SHIFT                                              (3U)
#define MPWM_INTE0_CH3TOIE(x)                                                 (((uint32_t)(((uint32_t)(x)) << MPWM_INTE0_CH3TOIE_SHIFT)) & MPWM_INTE0_CH3TOIE_MASK)
#define MPWM_INTE0_CH2TOIE_MASK                                               (0x4U)
#define MPWM_INTE0_CH2TOIE_SHIFT                                              (2U)
#define MPWM_INTE0_CH2TOIE(x)                                                 (((uint32_t)(((uint32_t)(x)) << MPWM_INTE0_CH2TOIE_SHIFT)) & MPWM_INTE0_CH2TOIE_MASK)
#define MPWM_INTE0_CH1TOIE_MASK                                               (0x2U)
#define MPWM_INTE0_CH1TOIE_SHIFT                                              (1U)
#define MPWM_INTE0_CH1TOIE(x)                                                 (((uint32_t)(((uint32_t)(x)) << MPWM_INTE0_CH1TOIE_SHIFT)) & MPWM_INTE0_CH1TOIE_MASK)
#define MPWM_INTE0_CH0TOIE_MASK                                               (0x1U)
#define MPWM_INTE0_CH0TOIE_SHIFT                                              (0U)
#define MPWM_INTE0_CH0TOIE(x)                                                 (((uint32_t)(((uint32_t)(x)) << MPWM_INTE0_CH0TOIE_SHIFT)) & MPWM_INTE0_CH0TOIE_MASK)
/* MPWM_INTE1 Register */
#define MPWM_INTE1_CH7IE_MASK                                                 (0x80U)
#define MPWM_INTE1_CH7IE_SHIFT                                                (7U)
#define MPWM_INTE1_CH7IE(x)                                                   (((uint32_t)(((uint32_t)(x)) << MPWM_INTE1_CH7IE_SHIFT)) & MPWM_INTE1_CH7IE_MASK)
#define MPWM_INTE1_CH6IE_MASK                                                 (0x40U)
#define MPWM_INTE1_CH6IE_SHIFT                                                (6U)
#define MPWM_INTE1_CH6IE(x)                                                   (((uint32_t)(((uint32_t)(x)) << MPWM_INTE1_CH6IE_SHIFT)) & MPWM_INTE1_CH6IE_MASK)
#define MPWM_INTE1_CH5IE_MASK                                                 (0x20U)
#define MPWM_INTE1_CH5IE_SHIFT                                                (5U)
#define MPWM_INTE1_CH5IE(x)                                                   (((uint32_t)(((uint32_t)(x)) << MPWM_INTE1_CH5IE_SHIFT)) & MPWM_INTE1_CH5IE_MASK)
#define MPWM_INTE1_CH4IE_MASK                                                 (0x10U)
#define MPWM_INTE1_CH4IE_SHIFT                                                (4U)
#define MPWM_INTE1_CH4IE(x)                                                   (((uint32_t)(((uint32_t)(x)) << MPWM_INTE1_CH4IE_SHIFT)) & MPWM_INTE1_CH4IE_MASK)
#define MPWM_INTE1_CH3IE_MASK                                                 (0x8U)
#define MPWM_INTE1_CH3IE_SHIFT                                                (3U)
#define MPWM_INTE1_CH3IE(x)                                                   (((uint32_t)(((uint32_t)(x)) << MPWM_INTE1_CH3IE_SHIFT)) & MPWM_INTE1_CH3IE_MASK)
#define MPWM_INTE1_CH2IE_MASK                                                 (0x4U)
#define MPWM_INTE1_CH2IE_SHIFT                                                (2U)
#define MPWM_INTE1_CH2IE(x)                                                   (((uint32_t)(((uint32_t)(x)) << MPWM_INTE1_CH2IE_SHIFT)) & MPWM_INTE1_CH2IE_MASK)
#define MPWM_INTE1_CH1IE_MASK                                                 (0x2U)
#define MPWM_INTE1_CH1IE_SHIFT                                                (1U)
#define MPWM_INTE1_CH1IE(x)                                                   (((uint32_t)(((uint32_t)(x)) << MPWM_INTE1_CH1IE_SHIFT)) & MPWM_INTE1_CH1IE_MASK)
#define MPWM_INTE1_CH0IE_MASK                                                 (0x1U)
#define MPWM_INTE1_CH0IE_SHIFT                                                (0U)
#define MPWM_INTE1_CH0IE(x)                                                   (((uint32_t)(((uint32_t)(x)) << MPWM_INTE1_CH0IE_SHIFT)) & MPWM_INTE1_CH0IE_MASK)
/* MPWM_CTRL Register */
#define MPWM_CTRL_CH7EN_MASK                                                  (0x80U)
#define MPWM_CTRL_CH7EN_SHIFT                                                 (7U)
#define MPWM_CTRL_CH7EN(x)                                                    (((uint32_t)(((uint32_t)(x)) << MPWM_CTRL_CH7EN_SHIFT)) & MPWM_CTRL_CH7EN_MASK)
#define MPWM_CTRL_CH6EN_MASK                                                  (0x40U)
#define MPWM_CTRL_CH6EN_SHIFT                                                 (6U)
#define MPWM_CTRL_CH6EN(x)                                                    (((uint32_t)(((uint32_t)(x)) << MPWM_CTRL_CH6EN_SHIFT)) & MPWM_CTRL_CH6EN_MASK)
#define MPWM_CTRL_CH5EN_MASK                                                  (0x20U)
#define MPWM_CTRL_CH5EN_SHIFT                                                 (5U)
#define MPWM_CTRL_CH5EN(x)                                                    (((uint32_t)(((uint32_t)(x)) << MPWM_CTRL_CH5EN_SHIFT)) & MPWM_CTRL_CH5EN_MASK)
#define MPWM_CTRL_CH4EN_MASK                                                  (0x10U)
#define MPWM_CTRL_CH4EN_SHIFT                                                 (4U)
#define MPWM_CTRL_CH4EN(x)                                                    (((uint32_t)(((uint32_t)(x)) << MPWM_CTRL_CH4EN_SHIFT)) & MPWM_CTRL_CH4EN_MASK)
#define MPWM_CTRL_CH3EN_MASK                                                  (0x8U)
#define MPWM_CTRL_CH3EN_SHIFT                                                 (3U)
#define MPWM_CTRL_CH3EN(x)                                                    (((uint32_t)(((uint32_t)(x)) << MPWM_CTRL_CH3EN_SHIFT)) & MPWM_CTRL_CH3EN_MASK)
#define MPWM_CTRL_CH2EN_MASK                                                  (0x4U)
#define MPWM_CTRL_CH2EN_SHIFT                                                 (2U)
#define MPWM_CTRL_CH2EN(x)                                                    (((uint32_t)(((uint32_t)(x)) << MPWM_CTRL_CH2EN_SHIFT)) & MPWM_CTRL_CH2EN_MASK)
#define MPWM_CTRL_CH1EN_MASK                                                  (0x2U)
#define MPWM_CTRL_CH1EN_SHIFT                                                 (1U)
#define MPWM_CTRL_CH1EN(x)                                                    (((uint32_t)(((uint32_t)(x)) << MPWM_CTRL_CH1EN_SHIFT)) & MPWM_CTRL_CH1EN_MASK)
#define MPWM_CTRL_CH0EN_MASK                                                  (0x1U)
#define MPWM_CTRL_CH0EN_SHIFT                                                 (0U)
#define MPWM_CTRL_CH0EN(x)                                                    (((uint32_t)(((uint32_t)(x)) << MPWM_CTRL_CH0EN_SHIFT)) & MPWM_CTRL_CH0EN_MASK)
/* MPWM_SWT Register */
#define MPWM_SWT_CH7SWT_MASK                                                  (0x80U)
#define MPWM_SWT_CH7SWT_SHIFT                                                 (7U)
#define MPWM_SWT_CH7SWT(x)                                                    (((uint32_t)(((uint32_t)(x)) << MPWM_SWT_CH7SWT_SHIFT)) & MPWM_SWT_CH7SWT_MASK)
#define MPWM_SWT_CH6SWT_MASK                                                  (0x40U)
#define MPWM_SWT_CH6SWT_SHIFT                                                 (6U)
#define MPWM_SWT_CH6SWT(x)                                                    (((uint32_t)(((uint32_t)(x)) << MPWM_SWT_CH6SWT_SHIFT)) & MPWM_SWT_CH6SWT_MASK)
#define MPWM_SWT_CH5SWT_MASK                                                  (0x20U)
#define MPWM_SWT_CH5SWT_SHIFT                                                 (5U)
#define MPWM_SWT_CH5SWT(x)                                                    (((uint32_t)(((uint32_t)(x)) << MPWM_SWT_CH5SWT_SHIFT)) & MPWM_SWT_CH5SWT_MASK)
#define MPWM_SWT_CH4SWT_MASK                                                  (0x10U)
#define MPWM_SWT_CH4SWT_SHIFT                                                 (4U)
#define MPWM_SWT_CH4SWT(x)                                                    (((uint32_t)(((uint32_t)(x)) << MPWM_SWT_CH4SWT_SHIFT)) & MPWM_SWT_CH4SWT_MASK)
#define MPWM_SWT_CH3SWT_MASK                                                  (0x8U)
#define MPWM_SWT_CH3SWT_SHIFT                                                 (3U)
#define MPWM_SWT_CH3SWT(x)                                                    (((uint32_t)(((uint32_t)(x)) << MPWM_SWT_CH3SWT_SHIFT)) & MPWM_SWT_CH3SWT_MASK)
#define MPWM_SWT_CH2SWT_MASK                                                  (0x4U)
#define MPWM_SWT_CH2SWT_SHIFT                                                 (2U)
#define MPWM_SWT_CH2SWT(x)                                                    (((uint32_t)(((uint32_t)(x)) << MPWM_SWT_CH2SWT_SHIFT)) & MPWM_SWT_CH2SWT_MASK)
#define MPWM_SWT_CH1SWT_MASK                                                  (0x2U)
#define MPWM_SWT_CH1SWT_SHIFT                                                 (1U)
#define MPWM_SWT_CH1SWT(x)                                                    (((uint32_t)(((uint32_t)(x)) << MPWM_SWT_CH1SWT_SHIFT)) & MPWM_SWT_CH1SWT_MASK)
#define MPWM_SWT_CH0SWT_MASK                                                  (0x1U)
#define MPWM_SWT_CH0SWT_SHIFT                                                 (0U)
#define MPWM_SWT_CH0SWT(x)                                                    (((uint32_t)(((uint32_t)(x)) << MPWM_SWT_CH0SWT_SHIFT)) & MPWM_SWT_CH0SWT_MASK)
/* MPWM_INIT Register */
#define MPWM_INIT_CH7INIT_MASK                                                (0x80U)
#define MPWM_INIT_CH7INIT_SHIFT                                               (7U)
#define MPWM_INIT_CH7INIT(x)                                                  (((uint32_t)(((uint32_t)(x)) << MPWM_INIT_CH7INIT_SHIFT)) & MPWM_INIT_CH7INIT_MASK)
#define MPWM_INIT_CH6INIT_MASK                                                (0x40U)
#define MPWM_INIT_CH6INIT_SHIFT                                               (6U)
#define MPWM_INIT_CH6INIT(x)                                                  (((uint32_t)(((uint32_t)(x)) << MPWM_INIT_CH6INIT_SHIFT)) & MPWM_INIT_CH6INIT_MASK)
#define MPWM_INIT_CH5INIT_MASK                                                (0x20U)
#define MPWM_INIT_CH5INIT_SHIFT                                               (5U)
#define MPWM_INIT_CH5INIT(x)                                                  (((uint32_t)(((uint32_t)(x)) << MPWM_INIT_CH5INIT_SHIFT)) & MPWM_INIT_CH5INIT_MASK)
#define MPWM_INIT_CH4INIT_MASK                                                (0x10U)
#define MPWM_INIT_CH4INIT_SHIFT                                               (4U)
#define MPWM_INIT_CH4INIT(x)                                                  (((uint32_t)(((uint32_t)(x)) << MPWM_INIT_CH4INIT_SHIFT)) & MPWM_INIT_CH4INIT_MASK)
#define MPWM_INIT_CH3INIT_MASK                                                (0x8U)
#define MPWM_INIT_CH3INIT_SHIFT                                               (3U)
#define MPWM_INIT_CH3INIT(x)                                                  (((uint32_t)(((uint32_t)(x)) << MPWM_INIT_CH3INIT_SHIFT)) & MPWM_INIT_CH3INIT_MASK)
#define MPWM_INIT_CH2INIT_MASK                                                (0x4U)
#define MPWM_INIT_CH2INIT_SHIFT                                               (2U)
#define MPWM_INIT_CH2INIT(x)                                                  (((uint32_t)(((uint32_t)(x)) << MPWM_INIT_CH2INIT_SHIFT)) & MPWM_INIT_CH2INIT_MASK)
#define MPWM_INIT_CH1INIT_MASK                                                (0x2U)
#define MPWM_INIT_CH1INIT_SHIFT                                               (1U)
#define MPWM_INIT_CH1INIT(x)                                                  (((uint32_t)(((uint32_t)(x)) << MPWM_INIT_CH1INIT_SHIFT)) & MPWM_INIT_CH1INIT_MASK)
#define MPWM_INIT_CH0INIT_MASK                                                (0x1U)
#define MPWM_INIT_CH0INIT_SHIFT                                               (0U)
#define MPWM_INIT_CH0INIT(x)                                                  (((uint32_t)(((uint32_t)(x)) << MPWM_INIT_CH0INIT_SHIFT)) & MPWM_INIT_CH0INIT_MASK)
/* MPWM_POL Register */
#define MPWM_POL_CH7POL_MASK                                                  (0x80U)
#define MPWM_POL_CH7POL_SHIFT                                                 (7U)
#define MPWM_POL_CH7POL(x)                                                    (((uint32_t)(((uint32_t)(x)) << MPWM_POL_CH7POL_SHIFT)) & MPWM_POL_CH7POL_MASK)
#define MPWM_POL_CH6POL_MASK                                                  (0x40U)
#define MPWM_POL_CH6POL_SHIFT                                                 (6U)
#define MPWM_POL_CH6POL(x)                                                    (((uint32_t)(((uint32_t)(x)) << MPWM_POL_CH6POL_SHIFT)) & MPWM_POL_CH6POL_MASK)
#define MPWM_POL_CH5POL_MASK                                                  (0x20U)
#define MPWM_POL_CH5POL_SHIFT                                                 (5U)
#define MPWM_POL_CH5POL(x)                                                    (((uint32_t)(((uint32_t)(x)) << MPWM_POL_CH5POL_SHIFT)) & MPWM_POL_CH5POL_MASK)
#define MPWM_POL_CH4POL_MASK                                                  (0x10U)
#define MPWM_POL_CH4POL_SHIFT                                                 (4U)
#define MPWM_POL_CH4POL(x)                                                    (((uint32_t)(((uint32_t)(x)) << MPWM_POL_CH4POL_SHIFT)) & MPWM_POL_CH4POL_MASK)
#define MPWM_POL_CH3POL_MASK                                                  (0x8U)
#define MPWM_POL_CH3POL_SHIFT                                                 (3U)
#define MPWM_POL_CH3POL(x)                                                    (((uint32_t)(((uint32_t)(x)) << MPWM_POL_CH3POL_SHIFT)) & MPWM_POL_CH3POL_MASK)
#define MPWM_POL_CH2POL_MASK                                                  (0x4U)
#define MPWM_POL_CH2POL_SHIFT                                                 (2U)
#define MPWM_POL_CH2POL(x)                                                    (((uint32_t)(((uint32_t)(x)) << MPWM_POL_CH2POL_SHIFT)) & MPWM_POL_CH2POL_MASK)
#define MPWM_POL_CH1POL_MASK                                                  (0x2U)
#define MPWM_POL_CH1POL_SHIFT                                                 (1U)
#define MPWM_POL_CH1POL(x)                                                    (((uint32_t)(((uint32_t)(x)) << MPWM_POL_CH1POL_SHIFT)) & MPWM_POL_CH1POL_MASK)
#define MPWM_POL_CH0POL_MASK                                                  (0x1U)
#define MPWM_POL_CH0POL_SHIFT                                                 (0U)
#define MPWM_POL_CH0POL(x)                                                    (((uint32_t)(((uint32_t)(x)) << MPWM_POL_CH0POL_SHIFT)) & MPWM_POL_CH0POL_MASK)
/* MPWM_LDOK Register */
#define MPWM_LDOK_CH7LDOK_MASK                                                (0x80U)
#define MPWM_LDOK_CH7LDOK_SHIFT                                               (7U)
#define MPWM_LDOK_CH7LDOK(x)                                                  (((uint32_t)(((uint32_t)(x)) << MPWM_LDOK_CH7LDOK_SHIFT)) & MPWM_LDOK_CH7LDOK_MASK)
#define MPWM_LDOK_CH6LDOK_MASK                                                (0x40U)
#define MPWM_LDOK_CH6LDOK_SHIFT                                               (6U)
#define MPWM_LDOK_CH6LDOK(x)                                                  (((uint32_t)(((uint32_t)(x)) << MPWM_LDOK_CH6LDOK_SHIFT)) & MPWM_LDOK_CH6LDOK_MASK)
#define MPWM_LDOK_CH5LDOK_MASK                                                (0x20U)
#define MPWM_LDOK_CH5LDOK_SHIFT                                               (5U)
#define MPWM_LDOK_CH5LDOK(x)                                                  (((uint32_t)(((uint32_t)(x)) << MPWM_LDOK_CH5LDOK_SHIFT)) & MPWM_LDOK_CH5LDOK_MASK)
#define MPWM_LDOK_CH4LDOK_MASK                                                (0x10U)
#define MPWM_LDOK_CH4LDOK_SHIFT                                               (4U)
#define MPWM_LDOK_CH4LDOK(x)                                                  (((uint32_t)(((uint32_t)(x)) << MPWM_LDOK_CH4LDOK_SHIFT)) & MPWM_LDOK_CH4LDOK_MASK)
#define MPWM_LDOK_CH3LDOK_MASK                                                (0x8U)
#define MPWM_LDOK_CH3LDOK_SHIFT                                               (3U)
#define MPWM_LDOK_CH3LDOK(x)                                                  (((uint32_t)(((uint32_t)(x)) << MPWM_LDOK_CH3LDOK_SHIFT)) & MPWM_LDOK_CH3LDOK_MASK)
#define MPWM_LDOK_CH2LDOK_MASK                                                (0x4U)
#define MPWM_LDOK_CH2LDOK_SHIFT                                               (2U)
#define MPWM_LDOK_CH2LDOK(x)                                                  (((uint32_t)(((uint32_t)(x)) << MPWM_LDOK_CH2LDOK_SHIFT)) & MPWM_LDOK_CH2LDOK_MASK)
#define MPWM_LDOK_CH1LDOK_MASK                                                (0x2U)
#define MPWM_LDOK_CH1LDOK_SHIFT                                               (1U)
#define MPWM_LDOK_CH1LDOK(x)                                                  (((uint32_t)(((uint32_t)(x)) << MPWM_LDOK_CH1LDOK_SHIFT)) & MPWM_LDOK_CH1LDOK_MASK)
#define MPWM_LDOK_CH0LDOK_MASK                                                (0x1U)
#define MPWM_LDOK_CH0LDOK_SHIFT                                               (0U)
#define MPWM_LDOK_CH0LDOK(x)                                                  (((uint32_t)(((uint32_t)(x)) << MPWM_LDOK_CH0LDOK_SHIFT)) & MPWM_LDOK_CH0LDOK_MASK)
/* MPWM_CLDOK Register */
#define MPWM_CLDOK_CH7CLDOK_MASK                                              (0x80U)
#define MPWM_CLDOK_CH7CLDOK_SHIFT                                             (7U)
#define MPWM_CLDOK_CH7CLDOK(x)                                                (((uint32_t)(((uint32_t)(x)) << MPWM_CLDOK_CH7CLDOK_SHIFT)) & MPWM_CLDOK_CH7CLDOK_MASK)
#define MPWM_CLDOK_CH6CLDOK_MASK                                              (0x40U)
#define MPWM_CLDOK_CH6CLDOK_SHIFT                                             (6U)
#define MPWM_CLDOK_CH6CLDOK(x)                                                (((uint32_t)(((uint32_t)(x)) << MPWM_CLDOK_CH6CLDOK_SHIFT)) & MPWM_CLDOK_CH6CLDOK_MASK)
#define MPWM_CLDOK_CH5CLDOK_MASK                                              (0x20U)
#define MPWM_CLDOK_CH5CLDOK_SHIFT                                             (5U)
#define MPWM_CLDOK_CH5CLDOK(x)                                                (((uint32_t)(((uint32_t)(x)) << MPWM_CLDOK_CH5CLDOK_SHIFT)) & MPWM_CLDOK_CH5CLDOK_MASK)
#define MPWM_CLDOK_CH4CLDOK_MASK                                              (0x10U)
#define MPWM_CLDOK_CH4CLDOK_SHIFT                                             (4U)
#define MPWM_CLDOK_CH4CLDOK(x)                                                (((uint32_t)(((uint32_t)(x)) << MPWM_CLDOK_CH4CLDOK_SHIFT)) & MPWM_CLDOK_CH4CLDOK_MASK)
#define MPWM_CLDOK_CH3CLDOK_MASK                                              (0x8U)
#define MPWM_CLDOK_CH3CLDOK_SHIFT                                             (3U)
#define MPWM_CLDOK_CH3CLDOK(x)                                                (((uint32_t)(((uint32_t)(x)) << MPWM_CLDOK_CH3CLDOK_SHIFT)) & MPWM_CLDOK_CH3CLDOK_MASK)
#define MPWM_CLDOK_CH2CLDOK_MASK                                              (0x4U)
#define MPWM_CLDOK_CH2CLDOK_SHIFT                                             (2U)
#define MPWM_CLDOK_CH2CLDOK(x)                                                (((uint32_t)(((uint32_t)(x)) << MPWM_CLDOK_CH2CLDOK_SHIFT)) & MPWM_CLDOK_CH2CLDOK_MASK)
#define MPWM_CLDOK_CH1CLDOK_MASK                                              (0x2U)
#define MPWM_CLDOK_CH1CLDOK_SHIFT                                             (1U)
#define MPWM_CLDOK_CH1CLDOK(x)                                                (((uint32_t)(((uint32_t)(x)) << MPWM_CLDOK_CH1CLDOK_SHIFT)) & MPWM_CLDOK_CH1CLDOK_MASK)
#define MPWM_CLDOK_CH0CLDOK_MASK                                              (0x1U)
#define MPWM_CLDOK_CH0CLDOK_SHIFT                                             (0U)
#define MPWM_CLDOK_CH0CLDOK(x)                                                (((uint32_t)(((uint32_t)(x)) << MPWM_CLDOK_CH0CLDOK_SHIFT)) & MPWM_CLDOK_CH0CLDOK_MASK)
/* MPWM_COMBCMP Register */
#define MPWM_COMBCMP_CH67COMBSRC_MASK                                         (0x80000U)
#define MPWM_COMBCMP_CH67COMBSRC_SHIFT                                        (19U)
#define MPWM_COMBCMP_CH67COMBSRC(x)                                           (((uint32_t)(((uint32_t)(x)) << MPWM_COMBCMP_CH67COMBSRC_SHIFT)) & MPWM_COMBCMP_CH67COMBSRC_MASK)
#define MPWM_COMBCMP_CH45COMBSRC_MASK                                         (0x40000U)
#define MPWM_COMBCMP_CH45COMBSRC_SHIFT                                        (18U)
#define MPWM_COMBCMP_CH45COMBSRC(x)                                           (((uint32_t)(((uint32_t)(x)) << MPWM_COMBCMP_CH45COMBSRC_SHIFT)) & MPWM_COMBCMP_CH45COMBSRC_MASK)
#define MPWM_COMBCMP_CH23COMBSRC_MASK                                         (0x20000U)
#define MPWM_COMBCMP_CH23COMBSRC_SHIFT                                        (17U)
#define MPWM_COMBCMP_CH23COMBSRC(x)                                           (((uint32_t)(((uint32_t)(x)) << MPWM_COMBCMP_CH23COMBSRC_SHIFT)) & MPWM_COMBCMP_CH23COMBSRC_MASK)
#define MPWM_COMBCMP_CH01COMBSRC_MASK                                         (0x10000U)
#define MPWM_COMBCMP_CH01COMBSRC_SHIFT                                        (16U)
#define MPWM_COMBCMP_CH01COMBSRC(x)                                           (((uint32_t)(((uint32_t)(x)) << MPWM_COMBCMP_CH01COMBSRC_SHIFT)) & MPWM_COMBCMP_CH01COMBSRC_MASK)
#define MPWM_COMBCMP_CH67COMBEN_MASK                                          (0x8U)
#define MPWM_COMBCMP_CH67COMBEN_SHIFT                                         (3U)
#define MPWM_COMBCMP_CH67COMBEN(x)                                            (((uint32_t)(((uint32_t)(x)) << MPWM_COMBCMP_CH67COMBEN_SHIFT)) & MPWM_COMBCMP_CH67COMBEN_MASK)
#define MPWM_COMBCMP_CH45COMBEN_MASK                                          (0x4U)
#define MPWM_COMBCMP_CH45COMBEN_SHIFT                                         (2U)
#define MPWM_COMBCMP_CH45COMBEN(x)                                            (((uint32_t)(((uint32_t)(x)) << MPWM_COMBCMP_CH45COMBEN_SHIFT)) & MPWM_COMBCMP_CH45COMBEN_MASK)
#define MPWM_COMBCMP_CH23COMBEN_MASK                                          (0x2U)
#define MPWM_COMBCMP_CH23COMBEN_SHIFT                                         (1U)
#define MPWM_COMBCMP_CH23COMBEN(x)                                            (((uint32_t)(((uint32_t)(x)) << MPWM_COMBCMP_CH23COMBEN_SHIFT)) & MPWM_COMBCMP_CH23COMBEN_MASK)
#define MPWM_COMBCMP_CH01COMBEN_MASK                                          (0x1U)
#define MPWM_COMBCMP_CH01COMBEN_SHIFT                                         (0U)
#define MPWM_COMBCMP_CH01COMBEN(x)                                            (((uint32_t)(((uint32_t)(x)) << MPWM_COMBCMP_CH01COMBEN_SHIFT)) & MPWM_COMBCMP_CH01COMBEN_MASK)
/* MPWM_CH_CFG Register */
#define MPWM_CH_CFG_ACLR_MASK                                                 (0x8000000U)
#define MPWM_CH_CFG_ACLR_SHIFT                                                (27U)
#define MPWM_CH_CFG_ACLR(x)                                                   (((uint32_t)(((uint32_t)(x)) << MPWM_CH_CFG_ACLR_SHIFT)) & MPWM_CH_CFG_ACLR_MASK)
#define MPWM_CH_CFG_OVWR_MASK                                                 (0x4000000U)
#define MPWM_CH_CFG_OVWR_SHIFT                                                (26U)
#define MPWM_CH_CFG_OVWR(x)                                                   (((uint32_t)(((uint32_t)(x)) << MPWM_CH_CFG_OVWR_SHIFT)) & MPWM_CH_CFG_OVWR_MASK)
#define MPWM_CH_CFG_CAPEDGE_WIDTH                                             (2)
#define MPWM_CH_CFG_CAPEDGE_MASK                                              (0x3000000U)
#define MPWM_CH_CFG_CAPEDGE_SHIFT                                             (24U)
#define MPWM_CH_CFG_CAPEDGE(x)                                                (((uint32_t)(((uint32_t)(x)) << MPWM_CH_CFG_CAPEDGE_SHIFT)) & MPWM_CH_CFG_CAPEDGE_MASK)
#define MPWM_CH_CFG_CMPOUT_WIDTH                                              (2)
#define MPWM_CH_CFG_CMPOUT_MASK                                               (0x300000U)
#define MPWM_CH_CFG_CMPOUT_SHIFT                                              (20U)
#define MPWM_CH_CFG_CMPOUT(x)                                                 (((uint32_t)(((uint32_t)(x)) << MPWM_CH_CFG_CMPOUT_SHIFT)) & MPWM_CH_CFG_CMPOUT_MASK)
#define MPWM_CH_CFG_RLDSRC_WIDTH                                              (2)
#define MPWM_CH_CFG_RLDSRC_MASK                                               (0x30000U)
#define MPWM_CH_CFG_RLDSRC_SHIFT                                              (16U)
#define MPWM_CH_CFG_RLDSRC(x)                                                 (((uint32_t)(((uint32_t)(x)) << MPWM_CH_CFG_RLDSRC_SHIFT)) & MPWM_CH_CFG_RLDSRC_MASK)
#define MPWM_CH_CFG_DMAEN_MASK                                                (0x8000U)
#define MPWM_CH_CFG_DMAEN_SHIFT                                               (15U)
#define MPWM_CH_CFG_DMAEN(x)                                                  (((uint32_t)(((uint32_t)(x)) << MPWM_CH_CFG_DMAEN_SHIFT)) & MPWM_CH_CFG_DMAEN_MASK)
#define MPWM_CH_CFG_DBGDIS_MASK                                               (0x4000U)
#define MPWM_CH_CFG_DBGDIS_SHIFT                                              (14U)
#define MPWM_CH_CFG_DBGDIS(x)                                                 (((uint32_t)(((uint32_t)(x)) << MPWM_CH_CFG_DBGDIS_SHIFT)) & MPWM_CH_CFG_DBGDIS_MASK)
#define MPWM_CH_CFG_HWTRIG_MASK                                               (0x400U)
#define MPWM_CH_CFG_HWTRIG_SHIFT                                              (10U)
#define MPWM_CH_CFG_HWTRIG(x)                                                 (((uint32_t)(((uint32_t)(x)) << MPWM_CH_CFG_HWTRIG_SHIFT)) & MPWM_CH_CFG_HWTRIG_MASK)
#define MPWM_CH_CFG_CMPTOEN_MASK                                              (0x200U)
#define MPWM_CH_CFG_CMPTOEN_SHIFT                                             (9U)
#define MPWM_CH_CFG_CMPTOEN(x)                                                (((uint32_t)(((uint32_t)(x)) << MPWM_CH_CFG_CMPTOEN_SHIFT)) & MPWM_CH_CFG_CMPTOEN_MASK)
#define MPWM_CH_CFG_PERTOEN_MASK                                              (0x100U)
#define MPWM_CH_CFG_PERTOEN_SHIFT                                             (8U)
#define MPWM_CH_CFG_PERTOEN(x)                                                (((uint32_t)(((uint32_t)(x)) << MPWM_CH_CFG_PERTOEN_SHIFT)) & MPWM_CH_CFG_PERTOEN_MASK)
#define MPWM_CH_CFG_CLKPRS_WIDTH                                              (3)
#define MPWM_CH_CFG_CLKPRS_MASK                                               (0x70U)
#define MPWM_CH_CFG_CLKPRS_SHIFT                                              (4U)
#define MPWM_CH_CFG_CLKPRS(x)                                                 (((uint32_t)(((uint32_t)(x)) << MPWM_CH_CFG_CLKPRS_SHIFT)) & MPWM_CH_CFG_CLKPRS_MASK)
#define MPWM_CH_CFG_ONESHOT_MASK                                              (0x8U)
#define MPWM_CH_CFG_ONESHOT_SHIFT                                             (3U)
#define MPWM_CH_CFG_ONESHOT(x)                                                (((uint32_t)(((uint32_t)(x)) << MPWM_CH_CFG_ONESHOT_SHIFT)) & MPWM_CH_CFG_ONESHOT_MASK)
#define MPWM_CH_CFG_CHMODE_WIDTH                                              (3)
#define MPWM_CH_CFG_CHMODE_MASK                                               (0x7U)
#define MPWM_CH_CFG_CHMODE_SHIFT                                              (0U)
#define MPWM_CH_CFG_CHMODE(x)                                                 (((uint32_t)(((uint32_t)(x)) << MPWM_CH_CFG_CHMODE_SHIFT)) & MPWM_CH_CFG_CHMODE_MASK)
/* MPWM_CH_CNT Register */
#define MPWM_CH_CNT_CNT_MASK                                                  (0xFFFFU)
#define MPWM_CH_CNT_CNT_SHIFT                                                 (0U)
#define MPWM_CH_CNT_CNT(x)                                                    (((uint32_t)(((uint32_t)(x)) << MPWM_CH_CNT_CNT_SHIFT)) & MPWM_CH_CNT_CNT_MASK)
/* MPWM_CH_CAP Register */
#define MPWM_CH_CAP_CAP_MASK                                                  (0xFFFFU)
#define MPWM_CH_CAP_CAP_SHIFT                                                 (0U)
#define MPWM_CH_CAP_CAP(x)                                                    (((uint32_t)(((uint32_t)(x)) << MPWM_CH_CAP_CAP_SHIFT)) & MPWM_CH_CAP_CAP_MASK)
/* MPWM_CH_PERIOD Register */
#define MPWM_CH_PERIOD_PERIOD_MASK                                            (0xFFFFU)
#define MPWM_CH_PERIOD_PERIOD_SHIFT                                           (0U)
#define MPWM_CH_PERIOD_PERIOD(x)                                              (((uint32_t)(((uint32_t)(x)) << MPWM_CH_PERIOD_PERIOD_SHIFT)) & MPWM_CH_PERIOD_PERIOD_MASK)
/* MPWM_CH_CMP Register */
#define MPWM_CH_CMP_CMP_MASK                                                  (0xFFFFU)
#define MPWM_CH_CMP_CMP_SHIFT                                                 (0U)
#define MPWM_CH_CMP_CMP(x)                                                    (((uint32_t)(((uint32_t)(x)) << MPWM_CH_CMP_CMP_SHIFT)) & MPWM_CH_CMP_CMP_MASK)



/*!
 * @}
 */ /* end of group MPWM_Register_Masks */


/*!
 * @}
 */ /* end of group MPWM_Peripheral_Access_Layer */


/* -------------------------------------------------------------
   -- ADC Peripheral Access Layer
   ------------------------------------------------------------- */

/*!
 * @addtogroup ADC_Peripheral_Access_Layer ADC Peripheral Access Layer
 * @{
 */


/** ADC - Size of Registers Arrays */
#define ADC_CHSEL_COUNT                                                       (8U)

/* ADC Register Layout Typedef */
typedef struct {
    __IO uint32_t STS;                                                        /**< Status register, offset: 0x0000 */
    __IO uint32_t INTE;                                                       /**< Interrupt Enable Register, offset: 0x0004 */
    __IO uint32_t CTRL;                                                       /**< Control Register, offset: 0x0008 */
    __IO uint32_t CFG0;                                                       /**< Configuration 0 Register, offset: 0x000C */
    __IO uint32_t CFG1;                                                       /**< Configuration 1 Register, offset: 0x0010 */
    __IO uint32_t SMP;                                                        /**< Sampling Register, offset: 0x0014 */
    __IO uint32_t WDCTRL;                                                     /**< Watchdog Control Register, offset: 0x0018 */
    __IO uint32_t WDTH;                                                       /**< Watchdog Threshold Register, offset: 0x001C */
    __IO uint8_t  RESERVED8[12];                                              /**< RESERVED8, offset: 0x0020 */
    __IO uint8_t  CHSEL[ 8];                                                  /**< Channel Configure Register, offset: 0x002C */
    __IO uint8_t  RESERVED10[24];                                             /**< RESERVED10, offset: 0x0034 */
    __I  uint32_t FIFO;                                                       /**< FIFO Access Register, offset: 0x004C */
    __IO uint32_t RESERVED12;                                                 /**< RESERVED12, offset: 0x0050 */
    __IO uint32_t TSC;                                                        /**< Temperature Sensor Coefficient, offset: 0x0054 */

} ADC_Type, *ADC_MemMapPtr;

/** Number of instances of the ADC module. */
#define ADC_INSTANCE_COUNT                                                    (1U)

/* ADC0  base address */
#define ADC0_BASE                                                             (0x40040000U)
#define ADC0                                                                  ((ADC_Type *)(ADC0_BASE))

/** Array initializer of ADC peripheral base addresses */
#define ADC_BASE_ADDRS                                                        { ADC0_BASE }
/** Array initializer of ADC peripheral base pointers */
#define ADC_BASE_PTRS                                                         { ADC0 }
/** Number of interrupt channels for the ADC. */
#define ADC_IRQS_CH_COUNT                                                     (1U)
/** Interrupt vectors for the ADC peripheral type */
#define ADC_IRQS                                                              { ADC0_IRQn }

/* ADC Register Mask */
/* ADC_STS Register */
#define ADC_STS_ERR_MASK                                                      (0x100U)
#define ADC_STS_ERR_SHIFT                                                     (8U)
#define ADC_STS_ERR(x)                                                        (((uint32_t)(((uint32_t)(x)) << ADC_STS_ERR_SHIFT)) & ADC_STS_ERR_MASK)
#define ADC_STS_WD_MASK                                                       (0x80U)
#define ADC_STS_WD_SHIFT                                                      (7U)
#define ADC_STS_WD(x)                                                         (((uint32_t)(((uint32_t)(x)) << ADC_STS_WD_SHIFT)) & ADC_STS_WD_MASK)
#define ADC_STS_EMPTY_MASK                                                    (0x40U)
#define ADC_STS_EMPTY_SHIFT                                                   (6U)
#define ADC_STS_EMPTY(x)                                                      (((uint32_t)(((uint32_t)(x)) << ADC_STS_EMPTY_SHIFT)) & ADC_STS_EMPTY_MASK)
#define ADC_STS_FULL_MASK                                                     (0x20U)
#define ADC_STS_FULL_SHIFT                                                    (5U)
#define ADC_STS_FULL(x)                                                       (((uint32_t)(((uint32_t)(x)) << ADC_STS_FULL_SHIFT)) & ADC_STS_FULL_MASK)
#define ADC_STS_OVR_MASK                                                      (0x10U)
#define ADC_STS_OVR_SHIFT                                                     (4U)
#define ADC_STS_OVR(x)                                                        (((uint32_t)(((uint32_t)(x)) << ADC_STS_OVR_SHIFT)) & ADC_STS_OVR_MASK)
#define ADC_STS_EOSEQ_MASK                                                    (0x8U)
#define ADC_STS_EOSEQ_SHIFT                                                   (3U)
#define ADC_STS_EOSEQ(x)                                                      (((uint32_t)(((uint32_t)(x)) << ADC_STS_EOSEQ_SHIFT)) & ADC_STS_EOSEQ_MASK)
#define ADC_STS_EOC_MASK                                                      (0x4U)
#define ADC_STS_EOC_SHIFT                                                     (2U)
#define ADC_STS_EOC(x)                                                        (((uint32_t)(((uint32_t)(x)) << ADC_STS_EOC_SHIFT)) & ADC_STS_EOC_MASK)
#define ADC_STS_EOSMP_MASK                                                    (0x2U)
#define ADC_STS_EOSMP_SHIFT                                                   (1U)
#define ADC_STS_EOSMP(x)                                                      (((uint32_t)(((uint32_t)(x)) << ADC_STS_EOSMP_SHIFT)) & ADC_STS_EOSMP_MASK)
#define ADC_STS_ADRDY_MASK                                                    (0x1U)
#define ADC_STS_ADRDY_SHIFT                                                   (0U)
#define ADC_STS_ADRDY(x)                                                      (((uint32_t)(((uint32_t)(x)) << ADC_STS_ADRDY_SHIFT)) & ADC_STS_ADRDY_MASK)
/* ADC_INTE Register */
#define ADC_INTE_ERRIE_MASK                                                   (0x100U)
#define ADC_INTE_ERRIE_SHIFT                                                  (8U)
#define ADC_INTE_ERRIE(x)                                                     (((uint32_t)(((uint32_t)(x)) << ADC_INTE_ERRIE_SHIFT)) & ADC_INTE_ERRIE_MASK)
#define ADC_INTE_WDIE_MASK                                                    (0x80U)
#define ADC_INTE_WDIE_SHIFT                                                   (7U)
#define ADC_INTE_WDIE(x)                                                      (((uint32_t)(((uint32_t)(x)) << ADC_INTE_WDIE_SHIFT)) & ADC_INTE_WDIE_MASK)
#define ADC_INTE_OVRIE_MASK                                                   (0x10U)
#define ADC_INTE_OVRIE_SHIFT                                                  (4U)
#define ADC_INTE_OVRIE(x)                                                     (((uint32_t)(((uint32_t)(x)) << ADC_INTE_OVRIE_SHIFT)) & ADC_INTE_OVRIE_MASK)
#define ADC_INTE_EOSEQIE_MASK                                                 (0x8U)
#define ADC_INTE_EOSEQIE_SHIFT                                                (3U)
#define ADC_INTE_EOSEQIE(x)                                                   (((uint32_t)(((uint32_t)(x)) << ADC_INTE_EOSEQIE_SHIFT)) & ADC_INTE_EOSEQIE_MASK)
#define ADC_INTE_EOCIE_MASK                                                   (0x4U)
#define ADC_INTE_EOCIE_SHIFT                                                  (2U)
#define ADC_INTE_EOCIE(x)                                                     (((uint32_t)(((uint32_t)(x)) << ADC_INTE_EOCIE_SHIFT)) & ADC_INTE_EOCIE_MASK)
#define ADC_INTE_EOSMPIE_MASK                                                 (0x2U)
#define ADC_INTE_EOSMPIE_SHIFT                                                (1U)
#define ADC_INTE_EOSMPIE(x)                                                   (((uint32_t)(((uint32_t)(x)) << ADC_INTE_EOSMPIE_SHIFT)) & ADC_INTE_EOSMPIE_MASK)
#define ADC_INTE_ADRDYIE_MASK                                                 (0x1U)
#define ADC_INTE_ADRDYIE_SHIFT                                                (0U)
#define ADC_INTE_ADRDYIE(x)                                                   (((uint32_t)(((uint32_t)(x)) << ADC_INTE_ADRDYIE_SHIFT)) & ADC_INTE_ADRDYIE_MASK)
/* ADC_CTRL Register */
#define ADC_CTRL_TSEN_MASK                                                    (0x10U)
#define ADC_CTRL_TSEN_SHIFT                                                   (4U)
#define ADC_CTRL_TSEN(x)                                                      (((uint32_t)(((uint32_t)(x)) << ADC_CTRL_TSEN_SHIFT)) & ADC_CTRL_TSEN_MASK)
#define ADC_CTRL_ADSTOP_MASK                                                  (0x8U)
#define ADC_CTRL_ADSTOP_SHIFT                                                 (3U)
#define ADC_CTRL_ADSTOP(x)                                                    (((uint32_t)(((uint32_t)(x)) << ADC_CTRL_ADSTOP_SHIFT)) & ADC_CTRL_ADSTOP_MASK)
#define ADC_CTRL_ADSTART_MASK                                                 (0x4U)
#define ADC_CTRL_ADSTART_SHIFT                                                (2U)
#define ADC_CTRL_ADSTART(x)                                                   (((uint32_t)(((uint32_t)(x)) << ADC_CTRL_ADSTART_SHIFT)) & ADC_CTRL_ADSTART_MASK)
#define ADC_CTRL_ADDIS_MASK                                                   (0x2U)
#define ADC_CTRL_ADDIS_SHIFT                                                  (1U)
#define ADC_CTRL_ADDIS(x)                                                     (((uint32_t)(((uint32_t)(x)) << ADC_CTRL_ADDIS_SHIFT)) & ADC_CTRL_ADDIS_MASK)
#define ADC_CTRL_ADEN_MASK                                                    (0x1U)
#define ADC_CTRL_ADEN_SHIFT                                                   (0U)
#define ADC_CTRL_ADEN(x)                                                      (((uint32_t)(((uint32_t)(x)) << ADC_CTRL_ADEN_SHIFT)) & ADC_CTRL_ADEN_MASK)
/* ADC_CFG0 Register */
#define ADC_CFG0_SEQLEN_MASK                                                  (0x7000000U)
#define ADC_CFG0_SEQLEN_SHIFT                                                 (24U)
#define ADC_CFG0_SEQLEN(x)                                                    (((uint32_t)(((uint32_t)(x)) << ADC_CFG0_SEQLEN_SHIFT)) & ADC_CFG0_SEQLEN_MASK)
#define ADC_CFG0_OVRMD_MASK                                                   (0x800000U)
#define ADC_CFG0_OVRMD_SHIFT                                                  (23U)
#define ADC_CFG0_OVRMD(x)                                                     (((uint32_t)(((uint32_t)(x)) << ADC_CFG0_OVRMD_SHIFT)) & ADC_CFG0_OVRMD_MASK)
#define ADC_CFG0_LPEN_MASK                                                    (0x400000U)
#define ADC_CFG0_LPEN_SHIFT                                                   (22U)
#define ADC_CFG0_LPEN(x)                                                      (((uint32_t)(((uint32_t)(x)) << ADC_CFG0_LPEN_SHIFT)) & ADC_CFG0_LPEN_MASK)
#define ADC_CFG0_AUTOOFF_MASK                                                 (0x200000U)
#define ADC_CFG0_AUTOOFF_SHIFT                                                (21U)
#define ADC_CFG0_AUTOOFF(x)                                                   (((uint32_t)(((uint32_t)(x)) << ADC_CFG0_AUTOOFF_SHIFT)) & ADC_CFG0_AUTOOFF_MASK)
#define ADC_CFG0_WAIT_MASK                                                    (0x100000U)
#define ADC_CFG0_WAIT_SHIFT                                                   (20U)
#define ADC_CFG0_WAIT(x)                                                      (((uint32_t)(((uint32_t)(x)) << ADC_CFG0_WAIT_SHIFT)) & ADC_CFG0_WAIT_MASK)
#define ADC_CFG0_ALIGN_MASK                                                   (0x80000U)
#define ADC_CFG0_ALIGN_SHIFT                                                  (19U)
#define ADC_CFG0_ALIGN(x)                                                     (((uint32_t)(((uint32_t)(x)) << ADC_CFG0_ALIGN_SHIFT)) & ADC_CFG0_ALIGN_MASK)
#define ADC_CFG0_MODE_MASK                                                    (0x30000U)
#define ADC_CFG0_MODE_SHIFT                                                   (16U)
#define ADC_CFG0_MODE(x)                                                      (((uint32_t)(((uint32_t)(x)) << ADC_CFG0_MODE_SHIFT)) & ADC_CFG0_MODE_MASK)
#define ADC_CFG0_TRIGMD_MASK                                                  (0x800U)
#define ADC_CFG0_TRIGMD_SHIFT                                                 (11U)
#define ADC_CFG0_TRIGMD(x)                                                    (((uint32_t)(((uint32_t)(x)) << ADC_CFG0_TRIGMD_SHIFT)) & ADC_CFG0_TRIGMD_MASK)
#define ADC_CFG0_RES_MASK                                                     (0x300U)
#define ADC_CFG0_RES_SHIFT                                                    (8U)
#define ADC_CFG0_RES(x)                                                       (((uint32_t)(((uint32_t)(x)) << ADC_CFG0_RES_SHIFT)) & ADC_CFG0_RES_MASK)
#define ADC_CFG0_WM_MASK                                                      (0xF0U)
#define ADC_CFG0_WM_SHIFT                                                     (4U)
#define ADC_CFG0_WM(x)                                                        (((uint32_t)(((uint32_t)(x)) << ADC_CFG0_WM_SHIFT)) & ADC_CFG0_WM_MASK)
#define ADC_CFG0_DMAEN_MASK                                                   (0x1U)
#define ADC_CFG0_DMAEN_SHIFT                                                  (0U)
#define ADC_CFG0_DMAEN(x)                                                     (((uint32_t)(((uint32_t)(x)) << ADC_CFG0_DMAEN_SHIFT)) & ADC_CFG0_DMAEN_MASK)
/* ADC_CFG1 Register */
#define ADC_CFG1_PRS_MASK                                                     (0xF00U)
#define ADC_CFG1_PRS_SHIFT                                                    (8U)
#define ADC_CFG1_PRS(x)                                                       (((uint32_t)(((uint32_t)(x)) << ADC_CFG1_PRS_SHIFT)) & ADC_CFG1_PRS_MASK)
#define ADC_CFG1_STCNT_MASK                                                   (0xFFU)
#define ADC_CFG1_STCNT_SHIFT                                                  (0U)
#define ADC_CFG1_STCNT(x)                                                     (((uint32_t)(((uint32_t)(x)) << ADC_CFG1_STCNT_SHIFT)) & ADC_CFG1_STCNT_MASK)
/* ADC_SMP Register */
#define ADC_SMP_SMP_MASK                                                      (0xFFU)
#define ADC_SMP_SMP_SHIFT                                                     (0U)
#define ADC_SMP_SMP(x)                                                        (((uint32_t)(((uint32_t)(x)) << ADC_SMP_SMP_SHIFT)) & ADC_SMP_SMP_MASK)
/* ADC_WDCTRL Register */
#define ADC_WDCTRL_WDEN_MASK                                                  (0x80U)
#define ADC_WDCTRL_WDEN_SHIFT                                                 (7U)
#define ADC_WDCTRL_WDEN(x)                                                    (((uint32_t)(((uint32_t)(x)) << ADC_WDCTRL_WDEN_SHIFT)) & ADC_WDCTRL_WDEN_MASK)
#define ADC_WDCTRL_WDSGL_MASK                                                 (0x40U)
#define ADC_WDCTRL_WDSGL_SHIFT                                                (6U)
#define ADC_WDCTRL_WDSGL(x)                                                   (((uint32_t)(((uint32_t)(x)) << ADC_WDCTRL_WDSGL_SHIFT)) & ADC_WDCTRL_WDSGL_MASK)
#define ADC_WDCTRL_WDCHSEL_MASK                                               (0x3FU)
#define ADC_WDCTRL_WDCHSEL_SHIFT                                              (0U)
#define ADC_WDCTRL_WDCHSEL(x)                                                 (((uint32_t)(((uint32_t)(x)) << ADC_WDCTRL_WDCHSEL_SHIFT)) & ADC_WDCTRL_WDCHSEL_MASK)
/* ADC_WDTH Register */
#define ADC_WDTH_THMD_MASK                                                    (0x80000000U)
#define ADC_WDTH_THMD_SHIFT                                                   (31U)
#define ADC_WDTH_THMD(x)                                                      (((uint32_t)(((uint32_t)(x)) << ADC_WDTH_THMD_SHIFT)) & ADC_WDTH_THMD_MASK)
#define ADC_WDTH_HIGH_MASK                                                    (0xFFF0000U)
#define ADC_WDTH_HIGH_SHIFT                                                   (16U)
#define ADC_WDTH_HIGH(x)                                                      (((uint32_t)(((uint32_t)(x)) << ADC_WDTH_HIGH_SHIFT)) & ADC_WDTH_HIGH_MASK)
#define ADC_WDTH_LOW_MASK                                                     (0xFFFU)
#define ADC_WDTH_LOW_SHIFT                                                    (0U)
#define ADC_WDTH_LOW(x)                                                       (((uint32_t)(((uint32_t)(x)) << ADC_WDTH_LOW_SHIFT)) & ADC_WDTH_LOW_MASK)
/* ADC_CHSEL Register */
#define ADC_CHSEL_CHSEL_MASK                                                  (0x3FU)
#define ADC_CHSEL_CHSEL_SHIFT                                                 (0U)
#define ADC_CHSEL_CHSEL(x)                                                    (((uint32_t)(((uint32_t)(x)) << ADC_CHSEL_CHSEL_SHIFT)) & ADC_CHSEL_CHSEL_MASK)
/* ADC_FIFO Register */
#define ADC_FIFO_CHID_MASK                                                    (0x3F0000U)
#define ADC_FIFO_CHID_SHIFT                                                   (16U)
#define ADC_FIFO_CHID(x)                                                      (((uint32_t)(((uint32_t)(x)) << ADC_FIFO_CHID_SHIFT)) & ADC_FIFO_CHID_MASK)
#define ADC_FIFO_DATA_MASK                                                    (0xFFFFU)
#define ADC_FIFO_DATA_SHIFT                                                   (0U)
#define ADC_FIFO_DATA(x)                                                      (((uint32_t)(((uint32_t)(x)) << ADC_FIFO_DATA_SHIFT)) & ADC_FIFO_DATA_MASK)
/* ADC_TSC Register */
#define ADC_TSC_COEFF_MASK                                                    (0xFFFU)
#define ADC_TSC_COEFF_SHIFT                                                   (0U)
#define ADC_TSC_COEFF(x)                                                      (((uint32_t)(((uint32_t)(x)) << ADC_TSC_COEFF_SHIFT)) & ADC_TSC_COEFF_MASK)


/*!
 * @}
 */ /* end of group ADC_Register_Masks */


/*!
 * @}
 */ /* end of group ADC_Peripheral_Access_Layer */


/* -------------------------------------------------------------
   -- ACMP Peripheral Access Layer
   ------------------------------------------------------------- */

/*!
 * @addtogroup ACMP_Peripheral_Access_Layer ACMP Peripheral Access Layer
 * @{
 */


/** ACMP - Size of Registers Arrays */

/* ACMP Register Layout Typedef */
typedef struct {
    __IO uint32_t STS;                                                        /**< Status Register, offset: 0x0000 */
    __IO uint32_t INTE;                                                       /**< Interrupt Enable Register, offset: 0x0004 */
    __IO uint32_t DAC;                                                        /**< DAC Register, offset: 0x0008 */
    __IO uint32_t MUX;                                                        /**< Channels Multiplexer, offset: 0x000C */
    __IO uint32_t FILT;                                                       /**< Filter Register, offset: 0x0010 */
    __IO uint32_t CONT;                                                       /**< Continuous Mode Configuration Register, offset: 0x0014 */
    __IO uint32_t EXP;                                                        /**< Expectation Register, offset: 0x0018 */
    __IO uint32_t CTRL;                                                       /**< Control Register, offset: 0x001C */
    __IO uint32_t EN;                                                         /**< Module Enable Register, offset: 0x0020 */
    __IO uint32_t DMACR;                                                      /**< Direct Memory Access Register, offset: 0x0024 */
    __IO uint8_t  RESERVED10[ 8];                                             /**< RESERVED10, offset: 0x0028 */
    __IO uint32_t RESERVED11;                                                 /**< RESERVED11, offset: 0x0030 */

} ACMP_Type, *ACMP_MemMapPtr;

/** Number of instances of the ACMP module. */
#define ACMP_INSTANCE_COUNT                                                   (1U)

/* ACMP0  base address */
#define ACMP0_BASE                                                            (0x40043000U)
#define ACMP0                                                                 ((ACMP_Type *)(ACMP0_BASE))

/** Array initializer of ACMP peripheral base addresses */
#define ACMP_BASE_ADDRS                                                       { ACMP0_BASE }
/** Array initializer of ACMP peripheral base pointers */
#define ACMP_BASE_PTRS                                                        { ACMP0 }
/** Number of interrupt channels for the ACMP. */
#define ACMP_IRQS_CH_COUNT                                                    (1U)
/** Interrupt vectors for the ACMP peripheral type */
#define ACMP_IRQS                                                             { ACMP0_IRQn }

/* ACMP Register Mask */
/* ACMP_STS Register */
#define ACMP_STS_CH7OUT_MASK                                                  (0x80000000U)
#define ACMP_STS_CH7OUT_SHIFT                                                 (31U)
#define ACMP_STS_CH7OUT(x)                                                    (((uint32_t)(((uint32_t)(x)) << ACMP_STS_CH7OUT_SHIFT)) & ACMP_STS_CH7OUT_MASK)
#define ACMP_STS_CH6OUT_MASK                                                  (0x40000000U)
#define ACMP_STS_CH6OUT_SHIFT                                                 (30U)
#define ACMP_STS_CH6OUT(x)                                                    (((uint32_t)(((uint32_t)(x)) << ACMP_STS_CH6OUT_SHIFT)) & ACMP_STS_CH6OUT_MASK)
#define ACMP_STS_CH5OUT_MASK                                                  (0x20000000U)
#define ACMP_STS_CH5OUT_SHIFT                                                 (29U)
#define ACMP_STS_CH5OUT(x)                                                    (((uint32_t)(((uint32_t)(x)) << ACMP_STS_CH5OUT_SHIFT)) & ACMP_STS_CH5OUT_MASK)
#define ACMP_STS_CH4OUT_MASK                                                  (0x10000000U)
#define ACMP_STS_CH4OUT_SHIFT                                                 (28U)
#define ACMP_STS_CH4OUT(x)                                                    (((uint32_t)(((uint32_t)(x)) << ACMP_STS_CH4OUT_SHIFT)) & ACMP_STS_CH4OUT_MASK)
#define ACMP_STS_CH3OUT_MASK                                                  (0x8000000U)
#define ACMP_STS_CH3OUT_SHIFT                                                 (27U)
#define ACMP_STS_CH3OUT(x)                                                    (((uint32_t)(((uint32_t)(x)) << ACMP_STS_CH3OUT_SHIFT)) & ACMP_STS_CH3OUT_MASK)
#define ACMP_STS_CH2OUT_MASK                                                  (0x4000000U)
#define ACMP_STS_CH2OUT_SHIFT                                                 (26U)
#define ACMP_STS_CH2OUT(x)                                                    (((uint32_t)(((uint32_t)(x)) << ACMP_STS_CH2OUT_SHIFT)) & ACMP_STS_CH2OUT_MASK)
#define ACMP_STS_CH1OUT_MASK                                                  (0x2000000U)
#define ACMP_STS_CH1OUT_SHIFT                                                 (25U)
#define ACMP_STS_CH1OUT(x)                                                    (((uint32_t)(((uint32_t)(x)) << ACMP_STS_CH1OUT_SHIFT)) & ACMP_STS_CH1OUT_MASK)
#define ACMP_STS_CH0OUT_MASK                                                  (0x1000000U)
#define ACMP_STS_CH0OUT_SHIFT                                                 (24U)
#define ACMP_STS_CH0OUT(x)                                                    (((uint32_t)(((uint32_t)(x)) << ACMP_STS_CH0OUT_SHIFT)) & ACMP_STS_CH0OUT_MASK)
#define ACMP_STS_CH7F_MASK                                                    (0x800000U)
#define ACMP_STS_CH7F_SHIFT                                                   (23U)
#define ACMP_STS_CH7F(x)                                                      (((uint32_t)(((uint32_t)(x)) << ACMP_STS_CH7F_SHIFT)) & ACMP_STS_CH7F_MASK)
#define ACMP_STS_CH6F_MASK                                                    (0x400000U)
#define ACMP_STS_CH6F_SHIFT                                                   (22U)
#define ACMP_STS_CH6F(x)                                                      (((uint32_t)(((uint32_t)(x)) << ACMP_STS_CH6F_SHIFT)) & ACMP_STS_CH6F_MASK)
#define ACMP_STS_CH5F_MASK                                                    (0x200000U)
#define ACMP_STS_CH5F_SHIFT                                                   (21U)
#define ACMP_STS_CH5F(x)                                                      (((uint32_t)(((uint32_t)(x)) << ACMP_STS_CH5F_SHIFT)) & ACMP_STS_CH5F_MASK)
#define ACMP_STS_CH4F_MASK                                                    (0x100000U)
#define ACMP_STS_CH4F_SHIFT                                                   (20U)
#define ACMP_STS_CH4F(x)                                                      (((uint32_t)(((uint32_t)(x)) << ACMP_STS_CH4F_SHIFT)) & ACMP_STS_CH4F_MASK)
#define ACMP_STS_CH3F_MASK                                                    (0x80000U)
#define ACMP_STS_CH3F_SHIFT                                                   (19U)
#define ACMP_STS_CH3F(x)                                                      (((uint32_t)(((uint32_t)(x)) << ACMP_STS_CH3F_SHIFT)) & ACMP_STS_CH3F_MASK)
#define ACMP_STS_CH2F_MASK                                                    (0x40000U)
#define ACMP_STS_CH2F_SHIFT                                                   (18U)
#define ACMP_STS_CH2F(x)                                                      (((uint32_t)(((uint32_t)(x)) << ACMP_STS_CH2F_SHIFT)) & ACMP_STS_CH2F_MASK)
#define ACMP_STS_CH1F_MASK                                                    (0x20000U)
#define ACMP_STS_CH1F_SHIFT                                                   (17U)
#define ACMP_STS_CH1F(x)                                                      (((uint32_t)(((uint32_t)(x)) << ACMP_STS_CH1F_SHIFT)) & ACMP_STS_CH1F_MASK)
#define ACMP_STS_CH0F_MASK                                                    (0x10000U)
#define ACMP_STS_CH0F_SHIFT                                                   (16U)
#define ACMP_STS_CH0F(x)                                                      (((uint32_t)(((uint32_t)(x)) << ACMP_STS_CH0F_SHIFT)) & ACMP_STS_CH0F_MASK)
#define ACMP_STS_RDY_MASK                                                     (0x1000U)
#define ACMP_STS_RDY_SHIFT                                                    (12U)
#define ACMP_STS_RDY(x)                                                       (((uint32_t)(((uint32_t)(x)) << ACMP_STS_RDY_SHIFT)) & ACMP_STS_RDY_MASK)
#define ACMP_STS_CHID_MASK                                                    (0x700U)
#define ACMP_STS_CHID_SHIFT                                                   (8U)
#define ACMP_STS_CHID(x)                                                      (((uint32_t)(((uint32_t)(x)) << ACMP_STS_CHID_SHIFT)) & ACMP_STS_CHID_MASK)
#define ACMP_STS_OUTLF_MASK                                                   (0x10U)
#define ACMP_STS_OUTLF_SHIFT                                                  (4U)
#define ACMP_STS_OUTLF(x)                                                     (((uint32_t)(((uint32_t)(x)) << ACMP_STS_OUTLF_SHIFT)) & ACMP_STS_OUTLF_MASK)
#define ACMP_STS_OUTHF_MASK                                                   (0x8U)
#define ACMP_STS_OUTHF_SHIFT                                                  (3U)
#define ACMP_STS_OUTHF(x)                                                     (((uint32_t)(((uint32_t)(x)) << ACMP_STS_OUTHF_SHIFT)) & ACMP_STS_OUTHF_MASK)
#define ACMP_STS_OUTNF_MASK                                                   (0x4U)
#define ACMP_STS_OUTNF_SHIFT                                                  (2U)
#define ACMP_STS_OUTNF(x)                                                     (((uint32_t)(((uint32_t)(x)) << ACMP_STS_OUTNF_SHIFT)) & ACMP_STS_OUTNF_MASK)
#define ACMP_STS_OUTPF_MASK                                                   (0x2U)
#define ACMP_STS_OUTPF_SHIFT                                                  (1U)
#define ACMP_STS_OUTPF(x)                                                     (((uint32_t)(((uint32_t)(x)) << ACMP_STS_OUTPF_SHIFT)) & ACMP_STS_OUTPF_MASK)
#define ACMP_STS_OUT_MASK                                                     (0x1U)
#define ACMP_STS_OUT_SHIFT                                                    (0U)
#define ACMP_STS_OUT(x)                                                       (((uint32_t)(((uint32_t)(x)) << ACMP_STS_OUT_SHIFT)) & ACMP_STS_OUT_MASK)
/* ACMP_INTE Register */
#define ACMP_INTE_CONTIE_MASK                                                 (0x10000U)
#define ACMP_INTE_CONTIE_SHIFT                                                (16U)
#define ACMP_INTE_CONTIE(x)                                                   (((uint32_t)(((uint32_t)(x)) << ACMP_INTE_CONTIE_SHIFT)) & ACMP_INTE_CONTIE_MASK)
#define ACMP_INTE_IE_MASK                                                     (0x1U)
#define ACMP_INTE_IE_SHIFT                                                    (0U)
#define ACMP_INTE_IE(x)                                                       (((uint32_t)(((uint32_t)(x)) << ACMP_INTE_IE_SHIFT)) & ACMP_INTE_IE_MASK)
/* ACMP_DAC Register */
#define ACMP_DAC_VAL_MASK                                                     (0xFF00U)
#define ACMP_DAC_VAL_SHIFT                                                    (8U)
#define ACMP_DAC_VAL(x)                                                       (((uint32_t)(((uint32_t)(x)) << ACMP_DAC_VAL_SHIFT)) & ACMP_DAC_VAL_MASK)
#define ACMP_DAC_OUTEN_MASK                                                   (0x10U)
#define ACMP_DAC_OUTEN_SHIFT                                                  (4U)
#define ACMP_DAC_OUTEN(x)                                                     (((uint32_t)(((uint32_t)(x)) << ACMP_DAC_OUTEN_SHIFT)) & ACMP_DAC_OUTEN_MASK)
#define ACMP_DAC_EN_MASK                                                      (0x1U)
#define ACMP_DAC_EN_SHIFT                                                     (0U)
#define ACMP_DAC_EN(x)                                                        (((uint32_t)(((uint32_t)(x)) << ACMP_DAC_EN_SHIFT)) & ACMP_DAC_EN_MASK)
/* ACMP_MUX Register */
#define ACMP_MUX_CHNSEL_MASK                                                  (0x7000U)
#define ACMP_MUX_CHNSEL_SHIFT                                                 (12U)
#define ACMP_MUX_CHNSEL(x)                                                    (((uint32_t)(((uint32_t)(x)) << ACMP_MUX_CHNSEL_SHIFT)) & ACMP_MUX_CHNSEL_MASK)
#define ACMP_MUX_CHPSEL_MASK                                                  (0x700U)
#define ACMP_MUX_CHPSEL_SHIFT                                                 (8U)
#define ACMP_MUX_CHPSEL(x)                                                    (((uint32_t)(((uint32_t)(x)) << ACMP_MUX_CHPSEL_SHIFT)) & ACMP_MUX_CHPSEL_MASK)
#define ACMP_MUX_INNSEL_MASK                                                  (0x30U)
#define ACMP_MUX_INNSEL_SHIFT                                                 (4U)
#define ACMP_MUX_INNSEL(x)                                                    (((uint32_t)(((uint32_t)(x)) << ACMP_MUX_INNSEL_SHIFT)) & ACMP_MUX_INNSEL_MASK)
#define ACMP_MUX_INPSEL_MASK                                                  (0x3U)
#define ACMP_MUX_INPSEL_SHIFT                                                 (0U)
#define ACMP_MUX_INPSEL(x)                                                    (((uint32_t)(((uint32_t)(x)) << ACMP_MUX_INPSEL_SHIFT)) & ACMP_MUX_INPSEL_MASK)
/* ACMP_FILT Register */
#define ACMP_FILT_BP_MASK                                                     (0x10000U)
#define ACMP_FILT_BP_SHIFT                                                    (16U)
#define ACMP_FILT_BP(x)                                                       (((uint32_t)(((uint32_t)(x)) << ACMP_FILT_BP_SHIFT)) & ACMP_FILT_BP_MASK)
#define ACMP_FILT_CNT_WIDTH                                                   (3)
#define ACMP_FILT_CNT_MASK                                                    (0x700U)
#define ACMP_FILT_CNT_SHIFT                                                   (8U)
#define ACMP_FILT_CNT(x)                                                      (((uint32_t)(((uint32_t)(x)) << ACMP_FILT_CNT_SHIFT)) & ACMP_FILT_CNT_MASK)
#define ACMP_FILT_PER_WIDTH                                                   (8)
#define ACMP_FILT_PER_MASK                                                    (0xFFU)
#define ACMP_FILT_PER_SHIFT                                                   (0U)
#define ACMP_FILT_PER(x)                                                      (((uint32_t)(((uint32_t)(x)) << ACMP_FILT_PER_SHIFT)) & ACMP_FILT_PER_MASK)
/* ACMP_CONT Register */
#define ACMP_CONT_SMP_WIDTH                                                   (8)
#define ACMP_CONT_SMP_MASK                                                    (0xFF000000U)
#define ACMP_CONT_SMP_SHIFT                                                   (24U)
#define ACMP_CONT_SMP(x)                                                      (((uint32_t)(((uint32_t)(x)) << ACMP_CONT_SMP_SHIFT)) & ACMP_CONT_SMP_MASK)
#define ACMP_CONT_PER_WIDTH                                                   (8)
#define ACMP_CONT_PER_MASK                                                    (0xFF0000U)
#define ACMP_CONT_PER_SHIFT                                                   (16U)
#define ACMP_CONT_PER(x)                                                      (((uint32_t)(((uint32_t)(x)) << ACMP_CONT_PER_SHIFT)) & ACMP_CONT_PER_MASK)
#define ACMP_CONT_CH7EN_MASK                                                  (0x8000U)
#define ACMP_CONT_CH7EN_SHIFT                                                 (15U)
#define ACMP_CONT_CH7EN(x)                                                    (((uint32_t)(((uint32_t)(x)) << ACMP_CONT_CH7EN_SHIFT)) & ACMP_CONT_CH7EN_MASK)
#define ACMP_CONT_CH6EN_MASK                                                  (0x4000U)
#define ACMP_CONT_CH6EN_SHIFT                                                 (14U)
#define ACMP_CONT_CH6EN(x)                                                    (((uint32_t)(((uint32_t)(x)) << ACMP_CONT_CH6EN_SHIFT)) & ACMP_CONT_CH6EN_MASK)
#define ACMP_CONT_CH5EN_MASK                                                  (0x2000U)
#define ACMP_CONT_CH5EN_SHIFT                                                 (13U)
#define ACMP_CONT_CH5EN(x)                                                    (((uint32_t)(((uint32_t)(x)) << ACMP_CONT_CH5EN_SHIFT)) & ACMP_CONT_CH5EN_MASK)
#define ACMP_CONT_CH4EN_MASK                                                  (0x1000U)
#define ACMP_CONT_CH4EN_SHIFT                                                 (12U)
#define ACMP_CONT_CH4EN(x)                                                    (((uint32_t)(((uint32_t)(x)) << ACMP_CONT_CH4EN_SHIFT)) & ACMP_CONT_CH4EN_MASK)
#define ACMP_CONT_CH3EN_MASK                                                  (0x800U)
#define ACMP_CONT_CH3EN_SHIFT                                                 (11U)
#define ACMP_CONT_CH3EN(x)                                                    (((uint32_t)(((uint32_t)(x)) << ACMP_CONT_CH3EN_SHIFT)) & ACMP_CONT_CH3EN_MASK)
#define ACMP_CONT_CH2EN_MASK                                                  (0x400U)
#define ACMP_CONT_CH2EN_SHIFT                                                 (10U)
#define ACMP_CONT_CH2EN(x)                                                    (((uint32_t)(((uint32_t)(x)) << ACMP_CONT_CH2EN_SHIFT)) & ACMP_CONT_CH2EN_MASK)
#define ACMP_CONT_CH1EN_MASK                                                  (0x200U)
#define ACMP_CONT_CH1EN_SHIFT                                                 (9U)
#define ACMP_CONT_CH1EN(x)                                                    (((uint32_t)(((uint32_t)(x)) << ACMP_CONT_CH1EN_SHIFT)) & ACMP_CONT_CH1EN_MASK)
#define ACMP_CONT_CH0EN_MASK                                                  (0x100U)
#define ACMP_CONT_CH0EN_SHIFT                                                 (8U)
#define ACMP_CONT_CH0EN(x)                                                    (((uint32_t)(((uint32_t)(x)) << ACMP_CONT_CH0EN_SHIFT)) & ACMP_CONT_CH0EN_MASK)
#define ACMP_CONT_TRIGMD_MASK                                                 (0x10U)
#define ACMP_CONT_TRIGMD_SHIFT                                                (4U)
#define ACMP_CONT_TRIGMD(x)                                                   (((uint32_t)(((uint32_t)(x)) << ACMP_CONT_TRIGMD_SHIFT)) & ACMP_CONT_TRIGMD_MASK)
#define ACMP_CONT_MODE_MASK                                                   (0x2U)
#define ACMP_CONT_MODE_SHIFT                                                  (1U)
#define ACMP_CONT_MODE(x)                                                     (((uint32_t)(((uint32_t)(x)) << ACMP_CONT_MODE_SHIFT)) & ACMP_CONT_MODE_MASK)
#define ACMP_CONT_CHFIX_MASK                                                  (0x1U)
#define ACMP_CONT_CHFIX_SHIFT                                                 (0U)
#define ACMP_CONT_CHFIX(x)                                                    (((uint32_t)(((uint32_t)(x)) << ACMP_CONT_CHFIX_SHIFT)) & ACMP_CONT_CHFIX_MASK)
/* ACMP_EXP Register */
#define ACMP_EXP_CH7EXP_MASK                                                  (0x80U)
#define ACMP_EXP_CH7EXP_SHIFT                                                 (7U)
#define ACMP_EXP_CH7EXP(x)                                                    (((uint32_t)(((uint32_t)(x)) << ACMP_EXP_CH7EXP_SHIFT)) & ACMP_EXP_CH7EXP_MASK)
#define ACMP_EXP_CH6EXP_MASK                                                  (0x40U)
#define ACMP_EXP_CH6EXP_SHIFT                                                 (6U)
#define ACMP_EXP_CH6EXP(x)                                                    (((uint32_t)(((uint32_t)(x)) << ACMP_EXP_CH6EXP_SHIFT)) & ACMP_EXP_CH6EXP_MASK)
#define ACMP_EXP_CH5EXP_MASK                                                  (0x20U)
#define ACMP_EXP_CH5EXP_SHIFT                                                 (5U)
#define ACMP_EXP_CH5EXP(x)                                                    (((uint32_t)(((uint32_t)(x)) << ACMP_EXP_CH5EXP_SHIFT)) & ACMP_EXP_CH5EXP_MASK)
#define ACMP_EXP_CH4EXP_MASK                                                  (0x10U)
#define ACMP_EXP_CH4EXP_SHIFT                                                 (4U)
#define ACMP_EXP_CH4EXP(x)                                                    (((uint32_t)(((uint32_t)(x)) << ACMP_EXP_CH4EXP_SHIFT)) & ACMP_EXP_CH4EXP_MASK)
#define ACMP_EXP_CH3EXP_MASK                                                  (0x8U)
#define ACMP_EXP_CH3EXP_SHIFT                                                 (3U)
#define ACMP_EXP_CH3EXP(x)                                                    (((uint32_t)(((uint32_t)(x)) << ACMP_EXP_CH3EXP_SHIFT)) & ACMP_EXP_CH3EXP_MASK)
#define ACMP_EXP_CH2EXP_MASK                                                  (0x4U)
#define ACMP_EXP_CH2EXP_SHIFT                                                 (2U)
#define ACMP_EXP_CH2EXP(x)                                                    (((uint32_t)(((uint32_t)(x)) << ACMP_EXP_CH2EXP_SHIFT)) & ACMP_EXP_CH2EXP_MASK)
#define ACMP_EXP_CH1EXP_MASK                                                  (0x2U)
#define ACMP_EXP_CH1EXP_SHIFT                                                 (1U)
#define ACMP_EXP_CH1EXP(x)                                                    (((uint32_t)(((uint32_t)(x)) << ACMP_EXP_CH1EXP_SHIFT)) & ACMP_EXP_CH1EXP_MASK)
#define ACMP_EXP_CH0EXP_MASK                                                  (0x1U)
#define ACMP_EXP_CH0EXP_SHIFT                                                 (0U)
#define ACMP_EXP_CH0EXP(x)                                                    (((uint32_t)(((uint32_t)(x)) << ACMP_EXP_CH0EXP_SHIFT)) & ACMP_EXP_CH0EXP_MASK)
/* ACMP_CTRL Register */
#define ACMP_CTRL_EDGESEL_WIDTH                                               (3)
#define ACMP_CTRL_EDGESEL_MASK                                                (0x70000000U)
#define ACMP_CTRL_EDGESEL_SHIFT                                               (28U)
#define ACMP_CTRL_EDGESEL(x)                                                  (((uint32_t)(((uint32_t)(x)) << ACMP_CTRL_EDGESEL_SHIFT)) & ACMP_CTRL_EDGESEL_MASK)
#define ACMP_CTRL_HYSTVAL_WIDTH                                               (2)
#define ACMP_CTRL_HYSTVAL_MASK                                                (0x300000U)
#define ACMP_CTRL_HYSTVAL_SHIFT                                               (20U)
#define ACMP_CTRL_HYSTVAL(x)                                                  (((uint32_t)(((uint32_t)(x)) << ACMP_CTRL_HYSTVAL_SHIFT)) & ACMP_CTRL_HYSTVAL_MASK)
#define ACMP_CTRL_OUTSEL_MASK                                                 (0x20000U)
#define ACMP_CTRL_OUTSEL_SHIFT                                                (17U)
#define ACMP_CTRL_OUTSEL(x)                                                   (((uint32_t)(((uint32_t)(x)) << ACMP_CTRL_OUTSEL_SHIFT)) & ACMP_CTRL_OUTSEL_MASK)
#define ACMP_CTRL_POL_MASK                                                    (0x10000U)
#define ACMP_CTRL_POL_SHIFT                                                   (16U)
#define ACMP_CTRL_POL(x)                                                      (((uint32_t)(((uint32_t)(x)) << ACMP_CTRL_POL_SHIFT)) & ACMP_CTRL_POL_MASK)
#define ACMP_CTRL_FILTCLK_MASK                                                (0x1000U)
#define ACMP_CTRL_FILTCLK_SHIFT                                               (12U)
#define ACMP_CTRL_FILTCLK(x)                                                  (((uint32_t)(((uint32_t)(x)) << ACMP_CTRL_FILTCLK_SHIFT)) & ACMP_CTRL_FILTCLK_MASK)
#define ACMP_CTRL_PWRMD_MASK                                                  (0x10U)
#define ACMP_CTRL_PWRMD_SHIFT                                                 (4U)
#define ACMP_CTRL_PWRMD(x)                                                    (((uint32_t)(((uint32_t)(x)) << ACMP_CTRL_PWRMD_SHIFT)) & ACMP_CTRL_PWRMD_MASK)
#define ACMP_CTRL_MODE_WIDTH                                                  (2)
#define ACMP_CTRL_MODE_MASK                                                   (0x3U)
#define ACMP_CTRL_MODE_SHIFT                                                  (0U)
#define ACMP_CTRL_MODE(x)                                                     (((uint32_t)(((uint32_t)(x)) << ACMP_CTRL_MODE_SHIFT)) & ACMP_CTRL_MODE_MASK)
/* ACMP_EN Register */
#define ACMP_EN_EN_MASK                                                       (0x1U)
#define ACMP_EN_EN_SHIFT                                                      (0U)
#define ACMP_EN_EN(x)                                                         (((uint32_t)(((uint32_t)(x)) << ACMP_EN_EN_SHIFT)) & ACMP_EN_EN_MASK)
/* ACMP_DMACR Register */
#define ACMP_DMACR_EN_MASK                                                    (0x1U)
#define ACMP_DMACR_EN_SHIFT                                                   (0U)
#define ACMP_DMACR_EN(x)                                                      (((uint32_t)(((uint32_t)(x)) << ACMP_DMACR_EN_SHIFT)) & ACMP_DMACR_EN_MASK)


/*!
 * @}
 */ /* end of group ACMP_Register_Masks */


/*!
 * @}
 */ /* end of group ACMP_Peripheral_Access_Layer */


/* -------------------------------------------------------------
   -- CRC Peripheral Access Layer
   ------------------------------------------------------------- */

/*!
 * @addtogroup CRC_Peripheral_Access_Layer CRC Peripheral Access Layer
 * @{
 */


/** CRC - Size of Registers Arrays */

/* CRC Register Layout Typedef */
typedef struct {
    __IO uint32_t CTRL;                                                       /**< Control Register, offset: 0x0000 */
    __IO uint32_t INIT;                                                       /**< Initial Seed Register, offset: 0x0004 */
    union {                                                                   /*PRQA S 0750*/
        __O  uint32_t IN32;                                                   /**< Data in 32 bits, offset: 0x0008 */
        __O  uint16_t IN16;                                                   /**< Data in 16 bits, offset: 0x0008 */
        __O  uint8_t  IN8;                                                    /**< Data in 8 Bits, offset: 0x0008 */

    } DATA;
    __I  uint32_t RESULT;                                                     /**< Result Register, offset: 0x000C */

} CRC_Type, *CRC_MemMapPtr;

/** Number of instances of the CRC module. */
#define CRC_INSTANCE_COUNT                                                    (1U)

/* CRC base address */
#define CRC_BASE                                                              (0x40065000U)
#define CRC                                                                   ((CRC_Type *)(CRC_BASE))

/** Array initializer of CRC peripheral base addresses */
#define CRC_BASE_ADDRS                                                        { CRC_BASE }
/** Array initializer of CRC peripheral base pointers */
#define CRC_BASE_PTRS                                                         { CRC }

/* CRC Register Mask */
/* CRC_CTRL Register */
#define CRC_CTRL_INV_OUT_MASK                                                 (0x40U)
#define CRC_CTRL_INV_OUT_SHIFT                                                (6U)
#define CRC_CTRL_INV_OUT(x)                                                   (((uint32_t)(((uint32_t)(x)) << CRC_CTRL_INV_OUT_SHIFT)) & CRC_CTRL_INV_OUT_MASK)
#define CRC_CTRL_SWAP_OUT_MASK                                                (0x20U)
#define CRC_CTRL_SWAP_OUT_SHIFT                                               (5U)
#define CRC_CTRL_SWAP_OUT(x)                                                  (((uint32_t)(((uint32_t)(x)) << CRC_CTRL_SWAP_OUT_SHIFT)) & CRC_CTRL_SWAP_OUT_MASK)
#define CRC_CTRL_SWAP_IN_MASK                                                 (0x10U)
#define CRC_CTRL_SWAP_IN_SHIFT                                                (4U)
#define CRC_CTRL_SWAP_IN(x)                                                   (((uint32_t)(((uint32_t)(x)) << CRC_CTRL_SWAP_IN_SHIFT)) & CRC_CTRL_SWAP_IN_MASK)
#define CRC_CTRL_MODE_MASK                                                    (0x3U)
#define CRC_CTRL_MODE_SHIFT                                                   (0U)
#define CRC_CTRL_MODE(x)                                                      (((uint32_t)(((uint32_t)(x)) << CRC_CTRL_MODE_SHIFT)) & CRC_CTRL_MODE_MASK)
/* CRC_INIT Register */
#define CRC_INIT_SEED_MASK                                                    (0xFFFFFFFFU)
#define CRC_INIT_SEED_SHIFT                                                   (0U)
#define CRC_INIT_SEED(x)                                                      (((uint32_t)(((uint32_t)(x)) << CRC_INIT_SEED_SHIFT)) & CRC_INIT_SEED_MASK)
/* CRC_DATA_IN32 Register */
#define CRC_DATA_IN32_DATA_MASK                                               (0xFFFFFFFFU)
#define CRC_DATA_IN32_DATA_SHIFT                                              (0U)
#define CRC_DATA_IN32_DATA(x)                                                 (((uint32_t)(((uint32_t)(x)) << CRC_DATA_IN32_DATA_SHIFT)) & CRC_DATA_IN32_DATA_MASK)
/* CRC_DATA_IN16 Register */
#define CRC_DATA_IN16_DATA_MASK                                               (0xFFFFU)
#define CRC_DATA_IN16_DATA_SHIFT                                              (0U)
#define CRC_DATA_IN16_DATA(x)                                                 (((uint32_t)(((uint32_t)(x)) << CRC_DATA_IN16_DATA_SHIFT)) & CRC_DATA_IN16_DATA_MASK)
/* CRC_DATA_IN8 Register */
#define CRC_DATA_IN8_DATA_MASK                                                (0xFFU)
#define CRC_DATA_IN8_DATA_SHIFT                                               (0U)
#define CRC_DATA_IN8_DATA(x)                                                  (((uint32_t)(((uint32_t)(x)) << CRC_DATA_IN8_DATA_SHIFT)) & CRC_DATA_IN8_DATA_MASK)

/* CRC_RESULT Register */
#define CRC_RESULT_RESULT_MASK                                                (0xFFFFFFFFU)
#define CRC_RESULT_RESULT_SHIFT                                               (0U)
#define CRC_RESULT_RESULT(x)                                                  (((uint32_t)(((uint32_t)(x)) << CRC_RESULT_RESULT_SHIFT)) & CRC_RESULT_RESULT_MASK)


/*!
 * @}
 */ /* end of group CRC_Register_Masks */


/*!
 * @}
 */ /* end of group CRC_Peripheral_Access_Layer */


/* -------------------------------------------------------------
   -- HCU Peripheral Access Layer
   ------------------------------------------------------------- */

/*!
 * @addtogroup HCU_Peripheral_Access_Layer HCU Peripheral Access Layer
 * @{
 */


/** HCU - Size of Registers Arrays */
#define HCU_KEY_COUNT                                                         (4U)
#define HCU_CONTEXT_4_7_COUNT                                                 (4U)
#define HCU_CONTEXT_8_11_COUNT                                                (4U)

/* HCU Register Layout Typedef */
typedef struct {
    __IO uint32_t GO;                                                         /**< Go Register, offset: 0x0000 */
    __IO uint32_t CR;                                                         /**< Configuration Register, offset: 0x0004 */
    __IO uint8_t  RESERVED2[ 4];                                              /**< RESERVED2, offset: 0x0008 */
    __IO uint32_t MSGCUR;                                                     /**< Message Current number Register, offset: 0x000C */
    __IO uint8_t  RESERVED4[ 8];                                              /**< RESERVED4, offset: 0x0010 */
    __IO uint32_t INTE;                                                       /**< Interrupt Enable Register, offset: 0x0018 */
    __IO uint32_t SR;                                                         /**< Status Register, offset: 0x001C */
    __O  uint32_t KEY[ 4];                                                    /**< Key Register, offset: 0x0020 */
    __IO uint8_t  RESERVED8[48];                                              /**< RESERVED8, offset: 0x0030 */
    __IO uint32_t CONTEXT_4_7[ 4];                                            /**< Context data 4~7 Register, offset: 0x0060 */
    __IO uint32_t CONTEXT_8_11[ 4];                                           /**< Context data 8~11 Register, offset: 0x0070 */
    __IO uint8_t  RESERVED11[48];                                             /**< RESERVED11, offset: 0x0080 */
    __O  uint32_t INMSG;                                                      /**< Input Message Register, offset: 0x00B0 */
    __I  uint32_t OUTMSG;                                                     /**< Output Message Register, offset: 0x00B4 */

} HCU_Type, *HCU_MemMapPtr;

/** Number of instances of the HCU module. */
#define HCU_INSTANCE_COUNT                                                    (1U)

/* HCU base address */
#define HCU_BASE                                                              (0x40067000U)
#define HCU                                                                   ((HCU_Type *)(HCU_BASE))

/** Array initializer of HCU peripheral base addresses */
#define HCU_BASE_ADDRS                                                        { HCU_BASE }
/** Array initializer of HCU peripheral base pointers */
#define HCU_BASE_PTRS                                                         { HCU }
/** Number of interrupt channels for the HCU. */
#define HCU_IRQS_CH_COUNT                                                     (1U)
/** Interrupt vectors for the HCU peripheral type */
#define HCU_IRQS                                                              { HCU_IRQn }

/* HCU Register Mask */
/* HCU_GO Register */
#define HCU_GO_GO_MASK                                                        (0x1U)
#define HCU_GO_GO_SHIFT                                                       (0U)
#define HCU_GO_GO(x)                                                          (((uint32_t)(((uint32_t)(x)) << HCU_GO_GO_SHIFT)) & HCU_GO_GO_MASK)
/* HCU_CR Register */
#define HCU_CR_OUTDMAE_MASK                                                   (0x20000000U)
#define HCU_CR_OUTDMAE_SHIFT                                                  (29U)
#define HCU_CR_OUTDMAE(x)                                                     (((uint32_t)(((uint32_t)(x)) << HCU_CR_OUTDMAE_SHIFT)) & HCU_CR_OUTDMAE_MASK)
#define HCU_CR_INDMAE_MASK                                                    (0x10000000U)
#define HCU_CR_INDMAE_SHIFT                                                   (28U)
#define HCU_CR_INDMAE(x)                                                      (((uint32_t)(((uint32_t)(x)) << HCU_CR_INDMAE_SHIFT)) & HCU_CR_INDMAE_MASK)
#define HCU_CR_MACLEN_MASK                                                    (0xF000000U)
#define HCU_CR_MACLEN_SHIFT                                                   (24U)
#define HCU_CR_MACLEN(x)                                                      (((uint32_t)(((uint32_t)(x)) << HCU_CR_MACLEN_SHIFT)) & HCU_CR_MACLEN_MASK)
#define HCU_CR_CS_MASK                                                        (0x40000U)
#define HCU_CR_CS_SHIFT                                                       (18U)
#define HCU_CR_CS(x)                                                          (((uint32_t)(((uint32_t)(x)) << HCU_CR_CS_SHIFT)) & HCU_CR_CS_MASK)
#define HCU_CR_MSGB_MASK                                                      (0x20000U)
#define HCU_CR_MSGB_SHIFT                                                     (17U)
#define HCU_CR_MSGB(x)                                                        (((uint32_t)(((uint32_t)(x)) << HCU_CR_MSGB_SHIFT)) & HCU_CR_MSGB_MASK)
#define HCU_CR_MSGE_MASK                                                      (0x10000U)
#define HCU_CR_MSGE_SHIFT                                                     (16U)
#define HCU_CR_MSGE(x)                                                        (((uint32_t)(((uint32_t)(x)) << HCU_CR_MSGE_SHIFT)) & HCU_CR_MSGE_MASK)
#define HCU_CR_ALGSEL_MASK                                                    (0xF000U)
#define HCU_CR_ALGSEL_SHIFT                                                   (12U)
#define HCU_CR_ALGSEL(x)                                                      (((uint32_t)(((uint32_t)(x)) << HCU_CR_ALGSEL_SHIFT)) & HCU_CR_ALGSEL_MASK)
#define HCU_CR_ENGSEL_MASK                                                    (0xF0U)
#define HCU_CR_ENGSEL_SHIFT                                                   (4U)
#define HCU_CR_ENGSEL(x)                                                      (((uint32_t)(((uint32_t)(x)) << HCU_CR_ENGSEL_SHIFT)) & HCU_CR_ENGSEL_MASK)
#define HCU_CR_DATSWP_MASK                                                    (0xCU)
#define HCU_CR_DATSWP_SHIFT                                                   (2U)
#define HCU_CR_DATSWP(x)                                                      (((uint32_t)(((uint32_t)(x)) << HCU_CR_DATSWP_SHIFT)) & HCU_CR_DATSWP_MASK)
#define HCU_CR_ENC_MASK                                                       (0x2U)
#define HCU_CR_ENC_SHIFT                                                      (1U)
#define HCU_CR_ENC(x)                                                         (((uint32_t)(((uint32_t)(x)) << HCU_CR_ENC_SHIFT)) & HCU_CR_ENC_MASK)
/* HCU_MSGCUR Register */
#define HCU_MSGCUR_MSGLEN_MASK                                                (0xFFFFU)
#define HCU_MSGCUR_MSGLEN_SHIFT                                               (0U)
#define HCU_MSGCUR_MSGLEN(x)                                                  (((uint32_t)(((uint32_t)(x)) << HCU_MSGCUR_MSGLEN_SHIFT)) & HCU_MSGCUR_MSGLEN_MASK)
/* HCU_INTE Register */
#define HCU_INTE_INMSGWIE_MASK                                                (0x80000000U)
#define HCU_INTE_INMSGWIE_SHIFT                                               (31U)
#define HCU_INTE_INMSGWIE(x)                                                  (((uint32_t)(((uint32_t)(x)) << HCU_INTE_INMSGWIE_SHIFT)) & HCU_INTE_INMSGWIE_MASK)
#define HCU_INTE_OUTMSGRIE_MASK                                               (0x40000000U)
#define HCU_INTE_OUTMSGRIE_SHIFT                                              (30U)
#define HCU_INTE_OUTMSGRIE(x)                                                 (((uint32_t)(((uint32_t)(x)) << HCU_INTE_OUTMSGRIE_SHIFT)) & HCU_INTE_OUTMSGRIE_MASK)
#define HCU_INTE_KEYWIE_MASK                                                  (0x20000000U)
#define HCU_INTE_KEYWIE_SHIFT                                                 (29U)
#define HCU_INTE_KEYWIE(x)                                                    (((uint32_t)(((uint32_t)(x)) << HCU_INTE_KEYWIE_SHIFT)) & HCU_INTE_KEYWIE_MASK)
#define HCU_INTE_AESIVWIE_MASK                                                (0x10000000U)
#define HCU_INTE_AESIVWIE_SHIFT                                               (28U)
#define HCU_INTE_AESIVWIE(x)                                                  (((uint32_t)(((uint32_t)(x)) << HCU_INTE_AESIVWIE_SHIFT)) & HCU_INTE_AESIVWIE_MASK)
#define HCU_INTE_AESMACRIE_MASK                                               (0x8000000U)
#define HCU_INTE_AESMACRIE_SHIFT                                              (27U)
#define HCU_INTE_AESMACRIE(x)                                                 (((uint32_t)(((uint32_t)(x)) << HCU_INTE_AESMACRIE_SHIFT)) & HCU_INTE_AESMACRIE_MASK)
#define HCU_INTE_AESMACWIE_MASK                                               (0x4000000U)
#define HCU_INTE_AESMACWIE_SHIFT                                              (26U)
#define HCU_INTE_AESMACWIE(x)                                                 (((uint32_t)(((uint32_t)(x)) << HCU_INTE_AESMACWIE_SHIFT)) & HCU_INTE_AESMACWIE_MASK)
#define HCU_INTE_AESIVRIE_MASK                                                (0x800000U)
#define HCU_INTE_AESIVRIE_SHIFT                                               (23U)
#define HCU_INTE_AESIVRIE(x)                                                  (((uint32_t)(((uint32_t)(x)) << HCU_INTE_AESIVRIE_SHIFT)) & HCU_INTE_AESIVRIE_MASK)
#define HCU_INTE_ODIE_MASK                                                    (0x1U)
#define HCU_INTE_ODIE_SHIFT                                                   (0U)
#define HCU_INTE_ODIE(x)                                                      (((uint32_t)(((uint32_t)(x)) << HCU_INTE_ODIE_SHIFT)) & HCU_INTE_ODIE_MASK)
/* HCU_SR Register */
#define HCU_SR_INMSGW_MASK                                                    (0x80000000U)
#define HCU_SR_INMSGW_SHIFT                                                   (31U)
#define HCU_SR_INMSGW(x)                                                      (((uint32_t)(((uint32_t)(x)) << HCU_SR_INMSGW_SHIFT)) & HCU_SR_INMSGW_MASK)
#define HCU_SR_OUTMSGR_MASK                                                   (0x40000000U)
#define HCU_SR_OUTMSGR_SHIFT                                                  (30U)
#define HCU_SR_OUTMSGR(x)                                                     (((uint32_t)(((uint32_t)(x)) << HCU_SR_OUTMSGR_SHIFT)) & HCU_SR_OUTMSGR_MASK)
#define HCU_SR_KEYW_MASK                                                      (0x20000000U)
#define HCU_SR_KEYW_SHIFT                                                     (29U)
#define HCU_SR_KEYW(x)                                                        (((uint32_t)(((uint32_t)(x)) << HCU_SR_KEYW_SHIFT)) & HCU_SR_KEYW_MASK)
#define HCU_SR_AESIVW_MASK                                                    (0x10000000U)
#define HCU_SR_AESIVW_SHIFT                                                   (28U)
#define HCU_SR_AESIVW(x)                                                      (((uint32_t)(((uint32_t)(x)) << HCU_SR_AESIVW_SHIFT)) & HCU_SR_AESIVW_MASK)
#define HCU_SR_AESMACR_MASK                                                   (0x8000000U)
#define HCU_SR_AESMACR_SHIFT                                                  (27U)
#define HCU_SR_AESMACR(x)                                                     (((uint32_t)(((uint32_t)(x)) << HCU_SR_AESMACR_SHIFT)) & HCU_SR_AESMACR_MASK)
#define HCU_SR_AESMACW_MASK                                                   (0x4000000U)
#define HCU_SR_AESMACW_SHIFT                                                  (26U)
#define HCU_SR_AESMACW(x)                                                     (((uint32_t)(((uint32_t)(x)) << HCU_SR_AESMACW_SHIFT)) & HCU_SR_AESMACW_MASK)
#define HCU_SR_AESIVR_MASK                                                    (0x800000U)
#define HCU_SR_AESIVR_SHIFT                                                   (23U)
#define HCU_SR_AESIVR(x)                                                      (((uint32_t)(((uint32_t)(x)) << HCU_SR_AESIVR_SHIFT)) & HCU_SR_AESIVR_MASK)
#define HCU_SR_AESVAL_MASK                                                    (0x2U)
#define HCU_SR_AESVAL_SHIFT                                                   (1U)
#define HCU_SR_AESVAL(x)                                                      (((uint32_t)(((uint32_t)(x)) << HCU_SR_AESVAL_SHIFT)) & HCU_SR_AESVAL_MASK)
#define HCU_SR_OD_MASK                                                        (0x1U)
#define HCU_SR_OD_SHIFT                                                       (0U)
#define HCU_SR_OD(x)                                                          (((uint32_t)(((uint32_t)(x)) << HCU_SR_OD_SHIFT)) & HCU_SR_OD_MASK)
/* HCU_KEY Register */
#define HCU_KEY_KEY_MASK                                                      (0xFFFFFFFFU)
#define HCU_KEY_KEY_SHIFT                                                     (0U)
#define HCU_KEY_KEY(x)                                                        (((uint32_t)(((uint32_t)(x)) << HCU_KEY_KEY_SHIFT)) & HCU_KEY_KEY_MASK)
/* HCU_CONTEXT_4_7 Register */
#define HCU_CONTEXT_4_7_CONTEXT_MASK                                          (0xFFFFFFFFU)
#define HCU_CONTEXT_4_7_CONTEXT_SHIFT                                         (0U)
#define HCU_CONTEXT_4_7_CONTEXT(x)                                            (((uint32_t)(((uint32_t)(x)) << HCU_CONTEXT_4_7_CONTEXT_SHIFT)) & HCU_CONTEXT_4_7_CONTEXT_MASK)
/* HCU_CONTEXT_8_11 Register */
#define HCU_CONTEXT_8_11_CONTEXT_MASK                                         (0xFFFFFFFFU)
#define HCU_CONTEXT_8_11_CONTEXT_SHIFT                                        (0U)
#define HCU_CONTEXT_8_11_CONTEXT(x)                                           (((uint32_t)(((uint32_t)(x)) << HCU_CONTEXT_8_11_CONTEXT_SHIFT)) & HCU_CONTEXT_8_11_CONTEXT_MASK)
/* HCU_INMSG Register */
#define HCU_INMSG_INMSG_MASK                                                  (0xFFFFFFFFU)
#define HCU_INMSG_INMSG_SHIFT                                                 (0U)
#define HCU_INMSG_INMSG(x)                                                    (((uint32_t)(((uint32_t)(x)) << HCU_INMSG_INMSG_SHIFT)) & HCU_INMSG_INMSG_MASK)
/* HCU_OUTMSG Register */
#define HCU_OUTMSG_OUTMSG_MASK                                                (0xFFFFFFFFU)
#define HCU_OUTMSG_OUTMSG_SHIFT                                               (0U)
#define HCU_OUTMSG_OUTMSG(x)                                                  (((uint32_t)(((uint32_t)(x)) << HCU_OUTMSG_OUTMSG_SHIFT)) & HCU_OUTMSG_OUTMSG_MASK)


/*!
 * @}
 */ /* end of group HCU_Register_Masks */


/*!
 * @}
 */ /* end of group HCU_Peripheral_Access_Layer */


/* -------------------------------------------------------------
   -- TRNG Peripheral Access Layer
   ------------------------------------------------------------- */

/*!
 * @addtogroup TRNG_Peripheral_Access_Layer TRNG Peripheral Access Layer
 * @{
 */


/** TRNG - Size of Registers Arrays */
#define TRNG_ENT_COUNT                                                        (4U)

/* TRNG Register Layout Typedef */
typedef struct {
    __IO uint32_t CTRL;                                                       /**< Control Register, offset: 0x0000 */
    __IO uint32_t STAT;                                                       /**< Status Register, offset: 0x0004 */
    __IO uint32_t SDCTL;                                                      /**< Seed Control Register, offset: 0x0008 */
    __IO uint32_t FRQMIN;                                                     /**< Frequency Minimum Limit Register, offset: 0x000C */
    __IO uint32_t FRQMAX;                                                     /**< Frequency Maximum Limit Register, offset: 0x0010 */
    __IO uint32_t FRQCNT;                                                     /**< Frequency Count Register, offset: 0x0014 */
    __IO uint32_t SCML;                                                       /**< Statistical Check Monobit Limit Register, offset: 0x0018 */
    __IO uint32_t SCMC;                                                       /**< Statistical Check Monobit Count Register, offset: 0x001C */
    __IO uint8_t  RESERVED8[32];                                              /**< RESERVED8, offset: 0x0020 */
    __IO uint32_t ENT[ 4];                                                    /**< Entropy Read Register, offset: 0x0040 */

} TRNG_Type, *TRNG_MemMapPtr;

/** Number of instances of the TRNG module. */
#define TRNG_INSTANCE_COUNT                                                   (1U)

/* TRNG base address */
#define TRNG_BASE                                                             (0x40066000U)
#define TRNG                                                                  ((TRNG_Type *)(TRNG_BASE))

/** Array initializer of TRNG peripheral base addresses */
#define TRNG_BASE_ADDRS                                                       { TRNG_BASE }
/** Array initializer of TRNG peripheral base pointers */
#define TRNG_BASE_PTRS                                                        { TRNG }
/** Number of interrupt channels for the TRNG. */
#define TRNG_IRQS_CH_COUNT                                                    (1U)
/** Interrupt vectors for the TRNG peripheral type */
#define TRNG_IRQS                                                             { TRNG_IRQn }

/* TRNG Register Mask */
/* TRNG_CTRL Register */
#define TRNG_CTRL_RTY_CT_MASK                                                 (0xF000000U)
#define TRNG_CTRL_RTY_CT_SHIFT                                                (24U)
#define TRNG_CTRL_RTY_CT(x)                                                   (((uint32_t)(((uint32_t)(x)) << TRNG_CTRL_RTY_CT_SHIFT)) & TRNG_CTRL_RTY_CT_MASK)
#define TRNG_CTRL_LRUN_MAX_MASK                                               (0xFF0000U)
#define TRNG_CTRL_LRUN_MAX_SHIFT                                              (16U)
#define TRNG_CTRL_LRUN_MAX(x)                                                 (((uint32_t)(((uint32_t)(x)) << TRNG_CTRL_LRUN_MAX_SHIFT)) & TRNG_CTRL_LRUN_MAX_MASK)
#define TRNG_CTRL_FRQ_CT_IE_MASK                                              (0x400U)
#define TRNG_CTRL_FRQ_CT_IE_SHIFT                                             (10U)
#define TRNG_CTRL_FRQ_CT_IE(x)                                                (((uint32_t)(((uint32_t)(x)) << TRNG_CTRL_FRQ_CT_IE_SHIFT)) & TRNG_CTRL_FRQ_CT_IE_MASK)
#define TRNG_CTRL_ENT_VAL_IE_MASK                                             (0x200U)
#define TRNG_CTRL_ENT_VAL_IE_SHIFT                                            (9U)
#define TRNG_CTRL_ENT_VAL_IE(x)                                               (((uint32_t)(((uint32_t)(x)) << TRNG_CTRL_ENT_VAL_IE_SHIFT)) & TRNG_CTRL_ENT_VAL_IE_MASK)
#define TRNG_CTRL_HW_ERR_IE_MASK                                              (0x100U)
#define TRNG_CTRL_HW_ERR_IE_SHIFT                                             (8U)
#define TRNG_CTRL_HW_ERR_IE(x)                                                (((uint32_t)(((uint32_t)(x)) << TRNG_CTRL_HW_ERR_IE_SHIFT)) & TRNG_CTRL_HW_ERR_IE_MASK)
#define TRNG_CTRL_FOR_SCLK_MASK                                               (0x80U)
#define TRNG_CTRL_FOR_SCLK_SHIFT                                              (7U)
#define TRNG_CTRL_FOR_SCLK(x)                                                 (((uint32_t)(((uint32_t)(x)) << TRNG_CTRL_FOR_SCLK_SHIFT)) & TRNG_CTRL_FOR_SCLK_MASK)
#define TRNG_CTRL_UNLOCK_MASK                                                 (0x20U)
#define TRNG_CTRL_UNLOCK_SHIFT                                                (5U)
#define TRNG_CTRL_UNLOCK(x)                                                   (((uint32_t)(((uint32_t)(x)) << TRNG_CTRL_UNLOCK_SHIFT)) & TRNG_CTRL_UNLOCK_MASK)
#define TRNG_CTRL_CLK_OUT_MASK                                                (0x10U)
#define TRNG_CTRL_CLK_OUT_SHIFT                                               (4U)
#define TRNG_CTRL_CLK_OUT(x)                                                  (((uint32_t)(((uint32_t)(x)) << TRNG_CTRL_CLK_OUT_SHIFT)) & TRNG_CTRL_CLK_OUT_MASK)
#define TRNG_CTRL_OSC_DIV_MASK                                                (0xCU)
#define TRNG_CTRL_OSC_DIV_SHIFT                                               (2U)
#define TRNG_CTRL_OSC_DIV(x)                                                  (((uint32_t)(((uint32_t)(x)) << TRNG_CTRL_OSC_DIV_SHIFT)) & TRNG_CTRL_OSC_DIV_MASK)
#define TRNG_CTRL_TRNG_EN_MASK                                                (0x2U)
#define TRNG_CTRL_TRNG_EN_SHIFT                                               (1U)
#define TRNG_CTRL_TRNG_EN(x)                                                  (((uint32_t)(((uint32_t)(x)) << TRNG_CTRL_TRNG_EN_SHIFT)) & TRNG_CTRL_TRNG_EN_MASK)
#define TRNG_CTRL_SWR_MASK                                                    (0x1U)
#define TRNG_CTRL_SWR_SHIFT                                                   (0U)
#define TRNG_CTRL_SWR(x)                                                      (((uint32_t)(((uint32_t)(x)) << TRNG_CTRL_SWR_SHIFT)) & TRNG_CTRL_SWR_MASK)
/* TRNG_STAT Register */
#define TRNG_STAT_FRQ_CT_IF_MASK                                              (0x400U)
#define TRNG_STAT_FRQ_CT_IF_SHIFT                                             (10U)
#define TRNG_STAT_FRQ_CT_IF(x)                                                (((uint32_t)(((uint32_t)(x)) << TRNG_STAT_FRQ_CT_IF_SHIFT)) & TRNG_STAT_FRQ_CT_IF_MASK)
#define TRNG_STAT_ENT_VAL_IF_MASK                                             (0x200U)
#define TRNG_STAT_ENT_VAL_IF_SHIFT                                            (9U)
#define TRNG_STAT_ENT_VAL_IF(x)                                               (((uint32_t)(((uint32_t)(x)) << TRNG_STAT_ENT_VAL_IF_SHIFT)) & TRNG_STAT_ENT_VAL_IF_MASK)
#define TRNG_STAT_HW_ERR_IF_MASK                                              (0x100U)
#define TRNG_STAT_HW_ERR_IF_SHIFT                                             (8U)
#define TRNG_STAT_HW_ERR_IF(x)                                                (((uint32_t)(((uint32_t)(x)) << TRNG_STAT_HW_ERR_IF_SHIFT)) & TRNG_STAT_HW_ERR_IF_MASK)
#define TRNG_STAT_IDLE_MASK                                                   (0x20U)
#define TRNG_STAT_IDLE_SHIFT                                                  (5U)
#define TRNG_STAT_IDLE(x)                                                     (((uint32_t)(((uint32_t)(x)) << TRNG_STAT_IDLE_SHIFT)) & TRNG_STAT_IDLE_MASK)
#define TRNG_STAT_FCT_VAL_MASK                                                (0x2U)
#define TRNG_STAT_FCT_VAL_SHIFT                                               (1U)
#define TRNG_STAT_FCT_VAL(x)                                                  (((uint32_t)(((uint32_t)(x)) << TRNG_STAT_FCT_VAL_SHIFT)) & TRNG_STAT_FCT_VAL_MASK)
/* TRNG_SDCTL Register */
#define TRNG_SDCTL_ENT_DLY_MASK                                               (0xFFFF0000U)
#define TRNG_SDCTL_ENT_DLY_SHIFT                                              (16U)
#define TRNG_SDCTL_ENT_DLY(x)                                                 (((uint32_t)(((uint32_t)(x)) << TRNG_SDCTL_ENT_DLY_SHIFT)) & TRNG_SDCTL_ENT_DLY_MASK)
#define TRNG_SDCTL_SAMP_SIZE_MASK                                             (0xFFFFU)
#define TRNG_SDCTL_SAMP_SIZE_SHIFT                                            (0U)
#define TRNG_SDCTL_SAMP_SIZE(x)                                               (((uint32_t)(((uint32_t)(x)) << TRNG_SDCTL_SAMP_SIZE_SHIFT)) & TRNG_SDCTL_SAMP_SIZE_MASK)
/* TRNG_FRQMIN Register */
#define TRNG_FRQMIN_FRQ_MIN_MASK                                              (0x3FFFFFU)
#define TRNG_FRQMIN_FRQ_MIN_SHIFT                                             (0U)
#define TRNG_FRQMIN_FRQ_MIN(x)                                                (((uint32_t)(((uint32_t)(x)) << TRNG_FRQMIN_FRQ_MIN_SHIFT)) & TRNG_FRQMIN_FRQ_MIN_MASK)
/* TRNG_FRQMAX Register */
#define TRNG_FRQMAX_FRQ_MAX_MASK                                              (0x3FFFFFU)
#define TRNG_FRQMAX_FRQ_MAX_SHIFT                                             (0U)
#define TRNG_FRQMAX_FRQ_MAX(x)                                                (((uint32_t)(((uint32_t)(x)) << TRNG_FRQMAX_FRQ_MAX_SHIFT)) & TRNG_FRQMAX_FRQ_MAX_MASK)
/* TRNG_FRQCNT Register */
#define TRNG_FRQCNT_FRQ_CT_MASK                                               (0x3FFFFFU)
#define TRNG_FRQCNT_FRQ_CT_SHIFT                                              (0U)
#define TRNG_FRQCNT_FRQ_CT(x)                                                 (((uint32_t)(((uint32_t)(x)) << TRNG_FRQCNT_FRQ_CT_SHIFT)) & TRNG_FRQCNT_FRQ_CT_MASK)
/* TRNG_SCML Register */
#define TRNG_SCML_MONO_RNG_MASK                                               (0xFFFF0000U)
#define TRNG_SCML_MONO_RNG_SHIFT                                              (16U)
#define TRNG_SCML_MONO_RNG(x)                                                 (((uint32_t)(((uint32_t)(x)) << TRNG_SCML_MONO_RNG_SHIFT)) & TRNG_SCML_MONO_RNG_MASK)
#define TRNG_SCML_MONO_MAX_MASK                                               (0xFFFFU)
#define TRNG_SCML_MONO_MAX_SHIFT                                              (0U)
#define TRNG_SCML_MONO_MAX(x)                                                 (((uint32_t)(((uint32_t)(x)) << TRNG_SCML_MONO_MAX_SHIFT)) & TRNG_SCML_MONO_MAX_MASK)
/* TRNG_SCMC Register */
#define TRNG_SCMC_MONO_CT_MASK                                                (0xFFFFU)
#define TRNG_SCMC_MONO_CT_SHIFT                                               (0U)
#define TRNG_SCMC_MONO_CT(x)                                                  (((uint32_t)(((uint32_t)(x)) << TRNG_SCMC_MONO_CT_SHIFT)) & TRNG_SCMC_MONO_CT_MASK)
/* TRNG_ENT Register */
#define TRNG_ENT_ENT_MASK                                                     (0xFFFFFFFFU)
#define TRNG_ENT_ENT_SHIFT                                                    (0U)
#define TRNG_ENT_ENT(x)                                                       (((uint32_t)(((uint32_t)(x)) << TRNG_ENT_ENT_SHIFT)) & TRNG_ENT_ENT_MASK)


/*!
 * @}
 */ /* end of group TRNG_Register_Masks */


/*!
 * @}
 */ /* end of group TRNG_Peripheral_Access_Layer */


/* -------------------------------------------------------------
   -- EMU Peripheral Access Layer
   ------------------------------------------------------------- */

/*!
 * @addtogroup EMU_Peripheral_Access_Layer EMU Peripheral Access Layer
 * @{
 */


/** EMU - Size of Registers Arrays */
#define EMU_EICHD_COUNT                                                       (1U)
#define EMU_ERINFO_COUNT                                                      (1U)

/* EMU Register Layout Typedef */
typedef struct {
    __IO uint32_t EIGCR;                                                      /**< Error Injection Global Control Register, offset: 0x0000 */
    __IO uint32_t EICHCR;                                                     /**< Error Injection Channel Control Register, offset: 0x0004 */
    __IO uint8_t  RESERVED2[ 8];                                              /**< RESERVED2, offset: 0x0008 */
    struct {
        __IO uint32_t ADDR;                                                   /**< Channel Error Injection Address Register, offset: 0x0010 */
        __IO uint32_t CHKBIT;                                                 /**< Error Injection CheckBit Mask Register, offset: 0x0014 */
        __IO uint32_t DATA;                                                   /**< Error Injection DATA Mask Register, offset: 0x0018 */
        __IO uint32_t RESERVEDS33;                                            /**< RESERVEDS33, offset: 0x001C */

    } EICHD[1];
    __IO uint8_t  RESERVED4[224];                                             /**< RESERVED4, offset: 0x0020 */
    __IO uint32_t ERIE;                                                       /**< Error Report Interrupt Enable Register, offset: 0x0100 */
    __IO uint32_t ERIF;                                                       /**< Error Report Interrupt Flag Register, offset: 0x0104 */
    __IO uint8_t  RESERVED7[ 8];                                              /**< RESERVED7, offset: 0x0108 */
    struct {
        __I  uint32_t ADDR;                                                   /**< Error Report Address Register, offset: 0x0110 */
        __I  uint32_t SYND;                                                   /**< Error Report Syndrome Register, offset: 0x0114 */
        __IO uint32_t CNT;                                                    /**< Correctable Error Count Register, offset: 0x0118 */
        __IO uint32_t RESERVEDS83;                                            /**< RESERVEDS83, offset: 0x011C */

    } ERINFO[1];                                                              /*PRQA S 1039*/

} EMU_Type, *EMU_MemMapPtr;

/** Number of instances of the EMU module. */
#define EMU_INSTANCE_COUNT                                                    (1U)

/* EMU0  base address */
#define EMU0_BASE                                                             (0x40072000U)
#define EMU0                                                                  ((EMU_Type *)(EMU0_BASE))

/** Array initializer of EMU peripheral base addresses */
#define EMU_BASE_ADDRS                                                        { EMU0_BASE }
/** Array initializer of EMU peripheral base pointers */
#define EMU_BASE_PTRS                                                         { EMU0 }
/** Number of interrupt channels for the  type of EMU. */
#define EMU_SB_IRQS_CH_COUNT                                                  (1U)
#define EMU_SB_IRQS                                                           { EMU0_SB_IRQn }
/** Number of interrupt channels for the  type of EMU. */
#define EMU_DB_IRQS_CH_COUNT                                                  (1U)
#define EMU_DB_IRQS                                                           { EMU0_DB_IRQn }

/* EMU Register Mask */
/* EMU_EIGCR Register */
#define EMU_EIGCR_GEN_MASK                                                    (0xFFFFFFFFU)
#define EMU_EIGCR_GEN_SHIFT                                                   (0U)
#define EMU_EIGCR_GEN(x)                                                      (((uint32_t)(((uint32_t)(x)) << EMU_EIGCR_GEN_SHIFT)) & EMU_EIGCR_GEN_MASK)
/* EMU_EICHCR Register */
#define EMU_EICHCR_CHEN_MASK                                                  (0x1U)
#define EMU_EICHCR_CHEN_SHIFT                                                 (0U)
#define EMU_EICHCR_CHEN(x)                                                    (((uint32_t)(((uint32_t)(x)) << EMU_EICHCR_CHEN_SHIFT)) & EMU_EICHCR_CHEN_MASK)
/* EMU_EICHD_ADDR Register */
#define EMU_EICHD_ADDR_ADDR_MASK                                              (0xFFFFFFFFU)
#define EMU_EICHD_ADDR_ADDR_SHIFT                                             (0U)
#define EMU_EICHD_ADDR_ADDR(x)                                                (((uint32_t)(((uint32_t)(x)) << EMU_EICHD_ADDR_ADDR_SHIFT)) & EMU_EICHD_ADDR_ADDR_MASK)
/* EMU_EICHD_CHKBIT Register */
#define EMU_EICHD_CHKBIT_CHKBIT_MASK                                          (0x7FU)
#define EMU_EICHD_CHKBIT_CHKBIT_SHIFT                                         (0U)
#define EMU_EICHD_CHKBIT_CHKBIT(x)                                            (((uint32_t)(((uint32_t)(x)) << EMU_EICHD_CHKBIT_CHKBIT_SHIFT)) & EMU_EICHD_CHKBIT_CHKBIT_MASK)
/* EMU_EICHD_DATA Register */
#define EMU_EICHD_DATA_DATA_MASK                                              (0xFFFFFFFFU)
#define EMU_EICHD_DATA_DATA_SHIFT                                             (0U)
#define EMU_EICHD_DATA_DATA(x)                                                (((uint32_t)(((uint32_t)(x)) << EMU_EICHD_DATA_DATA_SHIFT)) & EMU_EICHD_DATA_DATA_MASK)

/* EMU_ERIE Register */
#define EMU_ERIE_NCIE_WIDTH                                                   (1)
#define EMU_ERIE_NCIE_MASK                                                    (0x10000U)
#define EMU_ERIE_NCIE_SHIFT                                                   (16U)
#define EMU_ERIE_NCIE(x)                                                      (((uint32_t)(((uint32_t)(x)) << EMU_ERIE_NCIE_SHIFT)) & EMU_ERIE_NCIE_MASK)
#define EMU_ERIE_SCIE_WIDTH                                                   (1)
#define EMU_ERIE_SCIE_MASK                                                    (0x1U)
#define EMU_ERIE_SCIE_SHIFT                                                   (0U)
#define EMU_ERIE_SCIE(x)                                                      (((uint32_t)(((uint32_t)(x)) << EMU_ERIE_SCIE_SHIFT)) & EMU_ERIE_SCIE_MASK)
/* EMU_ERIF Register */
#define EMU_ERIF_NCIF_WIDTH                                                   (1)
#define EMU_ERIF_NCIF_MASK                                                    (0x10000U)
#define EMU_ERIF_NCIF_SHIFT                                                   (16U)
#define EMU_ERIF_NCIF(x)                                                      (((uint32_t)(((uint32_t)(x)) << EMU_ERIF_NCIF_SHIFT)) & EMU_ERIF_NCIF_MASK)
#define EMU_ERIF_SCIF_WIDTH                                                   (1)
#define EMU_ERIF_SCIF_MASK                                                    (0x1U)
#define EMU_ERIF_SCIF_SHIFT                                                   (0U)
#define EMU_ERIF_SCIF(x)                                                      (((uint32_t)(((uint32_t)(x)) << EMU_ERIF_SCIF_SHIFT)) & EMU_ERIF_SCIF_MASK)
/* EMU_ERINFO_ADDR Register */
#define EMU_ERINFO_ADDR_ADDR_MASK                                             (0xFFFFFFFFU)
#define EMU_ERINFO_ADDR_ADDR_SHIFT                                            (0U)
#define EMU_ERINFO_ADDR_ADDR(x)                                               (((uint32_t)(((uint32_t)(x)) << EMU_ERINFO_ADDR_ADDR_SHIFT)) & EMU_ERINFO_ADDR_ADDR_MASK)
/* EMU_ERINFO_SYND Register */
#define EMU_ERINFO_SYND_SYNDROME_MASK                                         (0x7FU)
#define EMU_ERINFO_SYND_SYNDROME_SHIFT                                        (0U)
#define EMU_ERINFO_SYND_SYNDROME(x)                                           (((uint32_t)(((uint32_t)(x)) << EMU_ERINFO_SYND_SYNDROME_SHIFT)) & EMU_ERINFO_SYND_SYNDROME_MASK)
/* EMU_ERINFO_CNT Register */
#define EMU_ERINFO_CNT_CNT_MASK                                               (0xFFU)
#define EMU_ERINFO_CNT_CNT_SHIFT                                              (0U)
#define EMU_ERINFO_CNT_CNT(x)                                                 (((uint32_t)(((uint32_t)(x)) << EMU_ERINFO_CNT_CNT_SHIFT)) & EMU_ERINFO_CNT_CNT_MASK)



/*!
 * @}
 */ /* end of group EMU_Register_Masks */


/*!
 * @}
 */ /* end of group EMU_Peripheral_Access_Layer */


/* -------------------------------------------------------------
   -- WDG Peripheral Access Layer
   ------------------------------------------------------------- */

/*!
 * @addtogroup WDG_Peripheral_Access_Layer WDG Peripheral Access Layer
 * @{
 */


/** WDG - Size of Registers Arrays */

/* WDG Register Layout Typedef */
typedef struct {
    __O  uint32_t SVCR;                                                       /**< Service Code Register, offset: 0x0000 */
    __IO uint32_t CR;                                                         /**< Control Register, offset: 0x0004 */
    __IO uint32_t LR;                                                         /**< Lock Register, offset: 0x0008 */
    __IO uint32_t TOVR;                                                       /**< Timer Overflow Value, offset: 0x000C */
    __IO uint32_t WVR;                                                        /**< Window Value Register, offset: 0x0010 */
    __I  uint32_t CNTCVR;                                                     /**< Counter Current Value Register, offset: 0x0014 */
    __IO uint32_t INTF;                                                       /**< Interrupt Flag register, offset: 0x0018 */

} WDG_Type, *WDG_MemMapPtr;

/** Number of instances of the WDG module. */
#define WDG_INSTANCE_COUNT                                                    (1U)

/* WDG0  base address */
#define WDG0_BASE                                                             (0x4006A000U)
#define WDG0                                                                  ((WDG_Type *)(WDG0_BASE))

/** Array initializer of WDG peripheral base addresses */
#define WDG_BASE_ADDRS                                                        { WDG0_BASE }
/** Array initializer of WDG peripheral base pointers */
#define WDG_BASE_PTRS                                                         { WDG0 }
/** Number of interrupt channels for the WDG. */
#define WDG_IRQS_CH_COUNT                                                     (1U)
/** Interrupt vectors for the WDG peripheral type */
#define WDG_IRQS                                                              { WDG0_IRQn }

/* WDG Register Mask */
/* WDG_SVCR Register */
#define WDG_SVCR_SVCODE_MASK                                                  (0xFFFFU)
#define WDG_SVCR_SVCODE_SHIFT                                                 (0U)
#define WDG_SVCR_SVCODE(x)                                                    (((uint32_t)(((uint32_t)(x)) << WDG_SVCR_SVCODE_SHIFT)) & WDG_SVCR_SVCODE_MASK)
/* WDG_CR Register */
#define WDG_CR_RIA_MASK                                                       (0x80U)
#define WDG_CR_RIA_SHIFT                                                      (7U)
#define WDG_CR_RIA(x)                                                         (((uint32_t)(((uint32_t)(x)) << WDG_CR_RIA_SHIFT)) & WDG_CR_RIA_MASK)
#define WDG_CR_WIN_MASK                                                       (0x40U)
#define WDG_CR_WIN_SHIFT                                                      (6U)
#define WDG_CR_WIN(x)                                                         (((uint32_t)(((uint32_t)(x)) << WDG_CR_WIN_SHIFT)) & WDG_CR_WIN_MASK)
#define WDG_CR_IBR_MASK                                                       (0x20U)
#define WDG_CR_IBR_SHIFT                                                      (5U)
#define WDG_CR_IBR(x)                                                         (((uint32_t)(((uint32_t)(x)) << WDG_CR_IBR_SHIFT)) & WDG_CR_IBR_MASK)
#define WDG_CR_CLKSRC_MASK                                                    (0x18U)
#define WDG_CR_CLKSRC_SHIFT                                                   (3U)
#define WDG_CR_CLKSRC(x)                                                      (((uint32_t)(((uint32_t)(x)) << WDG_CR_CLKSRC_SHIFT)) & WDG_CR_CLKSRC_MASK)
#define WDG_CR_DSDIS_MASK                                                     (0x4U)
#define WDG_CR_DSDIS_SHIFT                                                    (2U)
#define WDG_CR_DSDIS(x)                                                       (((uint32_t)(((uint32_t)(x)) << WDG_CR_DSDIS_SHIFT)) & WDG_CR_DSDIS_MASK)
#define WDG_CR_DBGDIS_MASK                                                    (0x2U)
#define WDG_CR_DBGDIS_SHIFT                                                   (1U)
#define WDG_CR_DBGDIS(x)                                                      (((uint32_t)(((uint32_t)(x)) << WDG_CR_DBGDIS_SHIFT)) & WDG_CR_DBGDIS_MASK)
#define WDG_CR_EN_MASK                                                        (0x1U)
#define WDG_CR_EN_SHIFT                                                       (0U)
#define WDG_CR_EN(x)                                                          (((uint32_t)(((uint32_t)(x)) << WDG_CR_EN_SHIFT)) & WDG_CR_EN_MASK)
/* WDG_LR Register */
#define WDG_LR_HL_MASK                                                        (0x2U)
#define WDG_LR_HL_SHIFT                                                       (1U)
#define WDG_LR_HL(x)                                                          (((uint32_t)(((uint32_t)(x)) << WDG_LR_HL_SHIFT)) & WDG_LR_HL_MASK)
#define WDG_LR_SL_MASK                                                        (0x1U)
#define WDG_LR_SL_SHIFT                                                       (0U)
#define WDG_LR_SL(x)                                                          (((uint32_t)(((uint32_t)(x)) << WDG_LR_SL_SHIFT)) & WDG_LR_SL_MASK)
/* WDG_TOVR Register */
#define WDG_TOVR_TO_MASK                                                      (0xFFFFU)
#define WDG_TOVR_TO_SHIFT                                                     (0U)
#define WDG_TOVR_TO(x)                                                        (((uint32_t)(((uint32_t)(x)) << WDG_TOVR_TO_SHIFT)) & WDG_TOVR_TO_MASK)
/* WDG_WVR Register */
#define WDG_WVR_WIN_MASK                                                      (0xFFFFU)
#define WDG_WVR_WIN_SHIFT                                                     (0U)
#define WDG_WVR_WIN(x)                                                        (((uint32_t)(((uint32_t)(x)) << WDG_WVR_WIN_SHIFT)) & WDG_WVR_WIN_MASK)
/* WDG_CNTCVR Register */
#define WDG_CNTCVR_CVAL_MASK                                                  (0xFFFFU)
#define WDG_CNTCVR_CVAL_SHIFT                                                 (0U)
#define WDG_CNTCVR_CVAL(x)                                                    (((uint32_t)(((uint32_t)(x)) << WDG_CNTCVR_CVAL_SHIFT)) & WDG_CNTCVR_CVAL_MASK)
/* WDG_INTF Register */
#define WDG_INTF_IF_MASK                                                      (0x1U)
#define WDG_INTF_IF_SHIFT                                                     (0U)
#define WDG_INTF_IF(x)                                                        (((uint32_t)(((uint32_t)(x)) << WDG_INTF_IF_SHIFT)) & WDG_INTF_IF_MASK)


/*!
 * @}
 */ /* end of group WDG_Register_Masks */


/*!
 * @}
 */ /* end of group WDG_Peripheral_Access_Layer */


/* -------------------------------------------------------------
   -- EWDG Peripheral Access Layer
   ------------------------------------------------------------- */

/*!
 * @addtogroup EWDG_Peripheral_Access_Layer EWDG Peripheral Access Layer
 * @{
 */


/** EWDG - Size of Registers Arrays */

/* EWDG Register Layout Typedef */
typedef struct {
    __IO uint32_t SERV;                                                       /**< Service Code Register, offset: 0x0000 */
    __IO uint32_t CMPL;                                                       /**< Compare Low Register, offset: 0x0004 */
    __IO uint32_t CMPH;                                                       /**< Compare High Register, offset: 0x0008 */
    __IO uint32_t CTRL;                                                       /**< Control Register, offset: 0x000C */
    __IO uint32_t CLK_CTRL;                                                   /**< Clock Control Register, offset: 0x0010 */

} EWDG_Type, *EWDG_MemMapPtr;

/** Number of instances of the EWDG module. */
#define EWDG_INSTANCE_COUNT                                                   (1U)

/* EWDG0  base address */
#define EWDG0_BASE                                                            (0x4006B000U)
#define EWDG0                                                                 ((EWDG_Type *)(EWDG0_BASE))

/** Array initializer of EWDG peripheral base addresses */
#define EWDG_BASE_ADDRS                                                       { EWDG0_BASE }
/** Array initializer of EWDG peripheral base pointers */
#define EWDG_BASE_PTRS                                                        { EWDG0 }
/** Number of interrupt channels for the EWDG. */
#define EWDG_IRQS_CH_COUNT                                                    (1U)
/** Interrupt vectors for the EWDG peripheral type */
#define EWDG_IRQS                                                             { EWDG0_IRQn }

/* EWDG Register Mask */
/* EWDG_SERV Register */
#define EWDG_SERV_SERVICE_MASK                                                (0xFFU)
#define EWDG_SERV_SERVICE_SHIFT                                               (0U)
#define EWDG_SERV_SERVICE(x)                                                  (((uint32_t)(((uint32_t)(x)) << EWDG_SERV_SERVICE_SHIFT)) & EWDG_SERV_SERVICE_MASK)
/* EWDG_CMPL Register */
#define EWDG_CMPL_COMPAREL_MASK                                               (0xFFFFU)
#define EWDG_CMPL_COMPAREL_SHIFT                                              (0U)
#define EWDG_CMPL_COMPAREL(x)                                                 (((uint32_t)(((uint32_t)(x)) << EWDG_CMPL_COMPAREL_SHIFT)) & EWDG_CMPL_COMPAREL_MASK)
/* EWDG_CMPH Register */
#define EWDG_CMPH_COMPAREH_MASK                                               (0xFFFFU)
#define EWDG_CMPH_COMPAREH_SHIFT                                              (0U)
#define EWDG_CMPH_COMPAREH(x)                                                 (((uint32_t)(((uint32_t)(x)) << EWDG_CMPH_COMPAREH_SHIFT)) & EWDG_CMPH_COMPAREH_MASK)
/* EWDG_CTRL Register */
#define EWDG_CTRL_INTEN_MASK                                                  (0x8U)
#define EWDG_CTRL_INTEN_SHIFT                                                 (3U)
#define EWDG_CTRL_INTEN(x)                                                    (((uint32_t)(((uint32_t)(x)) << EWDG_CTRL_INTEN_SHIFT)) & EWDG_CTRL_INTEN_MASK)
#define EWDG_CTRL_INEN_MASK                                                   (0x4U)
#define EWDG_CTRL_INEN_SHIFT                                                  (2U)
#define EWDG_CTRL_INEN(x)                                                     (((uint32_t)(((uint32_t)(x)) << EWDG_CTRL_INEN_SHIFT)) & EWDG_CTRL_INEN_MASK)
#define EWDG_CTRL_POLARITY_MASK                                               (0x2U)
#define EWDG_CTRL_POLARITY_SHIFT                                              (1U)
#define EWDG_CTRL_POLARITY(x)                                                 (((uint32_t)(((uint32_t)(x)) << EWDG_CTRL_POLARITY_SHIFT)) & EWDG_CTRL_POLARITY_MASK)
#define EWDG_CTRL_EN_MASK                                                     (0x1U)
#define EWDG_CTRL_EN_SHIFT                                                    (0U)
#define EWDG_CTRL_EN(x)                                                       (((uint32_t)(((uint32_t)(x)) << EWDG_CTRL_EN_SHIFT)) & EWDG_CTRL_EN_MASK)
/* EWDG_CLK_CTRL Register */
#define EWDG_CLK_CTRL_CLKSEL_MASK                                             (0x10000U)
#define EWDG_CLK_CTRL_CLKSEL_SHIFT                                            (16U)
#define EWDG_CLK_CTRL_CLKSEL(x)                                               (((uint32_t)(((uint32_t)(x)) << EWDG_CLK_CTRL_CLKSEL_SHIFT)) & EWDG_CLK_CTRL_CLKSEL_MASK)
#define EWDG_CLK_CTRL_PRESCALER_MASK                                          (0xFFU)
#define EWDG_CLK_CTRL_PRESCALER_SHIFT                                         (0U)
#define EWDG_CLK_CTRL_PRESCALER(x)                                            (((uint32_t)(((uint32_t)(x)) << EWDG_CLK_CTRL_PRESCALER_SHIFT)) & EWDG_CLK_CTRL_PRESCALER_MASK)


/*!
 * @}
 */ /* end of group EWDG_Register_Masks */


/*!
 * @}
 */ /* end of group EWDG_Peripheral_Access_Layer */




/* ----------------------------------------------------------------------------
   -- Device Peripheral Access Layer for YTM32B1MC0
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup Peripheral_access_layer_YTM32B1MC0 Device Peripheral Access Layer for YTM32B1MC0
 * @{
 */

/* @brief This module covers memory mapped registers available on SoC */

/* ----------------------------------------------------------------------------
   -- Backward Compatibility for YTM32B1MC0
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup Backward_Compatibility_Symbols_YTM32B1MC0 Backward Compatibility for YTM32B1MC0
 * @{
 */

/* No backward compatibility issues. */

/*!
 * @}
 */ /* end of group Backward_Compatibility_Symbols_YTM32B1MC0 */


#endif  /* YTM32B1MC0_H */

/* YTM32B1MC0.h, eof. */
/*******************************************************************************
* EOF
*******************************************************************************/
