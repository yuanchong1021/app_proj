/*
 * Copyright (c) 2021-2025 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file YTM32B1MC0_features.h
 * @version 1.4.0
 * @brief Chip specific module features
 */

#ifndef YTM32B1MC0_FEATURES_H
#define YTM32B1MC0_FEATURES_H

/* ERRATA sections*/

/* @brief Number of cores. */
#define NUMBER_OF_CORES                     (1u)

/* SOC module features */

/* @brief PORT availability on the SoC. */
#define FEATURE_SOC_PORT_COUNT              (5)

#define FEATURE_SOC_IPC_COUNT               (1)

/* @brief Power down mode availability on the SoC. */
#define FEATURE_SOC_SUPPORT_POWERDOWN_MODE  (0)
#define FEATURE_SOC_SUPPORT_DEEPPOWERDOWN_MODE  (0)
#define FEATURE_SCU_SUPPORT_PLL             (0)
#define FEATURE_SOC_HAS_SEPARATE_CMU_AND_CLKOUT (0)

/* @brief Slow IRC clock frequency(2MHz). */
#define FEATURE_SCU_SIRC_FREQ               (2000000U)
/* @brief Fast IRC trimmed clock frequency(80MHz). */
#define FEATURE_SCU_FIRC_FREQ               (80000000U)
/* @brief FXOSC clock frequency(24MHz). */
#define FEATURE_SCU_FXOSC_FREQ              (24000000U)
/* @brief SCU support SXOSC 32.768K clock. */
#define FEATURE_SCU_SUPPORT_SXOSC           (0)
/* @brief SCU FXOSC support GAIN configuration. */
#define FEATURE_SCU_FXOSC_SUPPORT_GAIN_SEL  (1)
/* @brief SCU support PLL clock. */
#define FEATURE_SCU_SUPPORT_PLL             (0)
/* @brief SCU support CMU reference clock selection. */
#define FEATURE_SCU_SUPPORT_CMU_REFCLK_SEL  (0)
#if FEATURE_SCU_SUPPORT_SXOSC
/* @brief SXOSC clock frequency(32.768K). */
#define FEATURE_SCU_SXOSC_FREQ              (32768U)
#endif /* FEATURE_SCU_SUPPORT_SXOSC */
#define FEATURE_SCU_SUPPORT_LPO             (1)
#if FEATURE_SCU_SUPPORT_LPO
#define FEATURE_SCU_LPO_FREQ                (32000U)
#endif
/* @brief Clock switch PLL lock timeout loop count. */
#define FEATURE_CLOCK_TIMEOUT_LOOP_CNT      (0x1FFFFU)

/* FLASH module features */

/* @brief Flash operation clock source, used for PRESCALER */
#define FEATURE_EFM_OPERATION_CLOCK_SOURCE   (SLOW_BUS_CLK)
/* @brief Flash timing max value */
#define FEATURE_EFM_TIMING_MAX               (0x1U)
#define FEATURE_EFM_TIMING1_MAX              (0x0000000CU)
#define FEATURE_EFM_TIMING2_MAX              (0x000016A8U)
/* @brief Flash timing of Erase pulse times */
#define FEATURE_EFM_HAS_ERASE_TIMING_UNION    (1u)
#define FEATURE_EFM_SECTOR_ERASE_RETRY_TIMING (400u)
#if defined (FEATURE_EFM_TIMING_MAX) && (FEATURE_EFM_TIMING_MAX == 0x1U)
#define FEATURE_EFM_SECTOR_ERASE_TIMING       (5800u)
#define FEATURE_EFM_BLOCK_ERASE_TIMING        (0xFFFFu)
#else
#define FEATURE_EFM_SECTOR_ERASE_TIMING       (4000u)
#define FEATURE_EFM_BLOCK_ERASE_TIMING        (60000u)
#endif /* FEATURE_EFM_TIMING_MAX && FEATURE_EFM_TIMING_MAX */
/* @brief Flash block count. */
#define FEATURE_EFM_BLOCK_COUNT              (2u)
/* @brief Flash program flash start address. */
#define FEATURE_EFM_MAIN_ARRAY_START_ADDRESS (0x0U)
/* @brief Flash program flash end address. */
#define FEATURE_EFM_MAIN_ARRAY_END_ADDRESS   (0x3FFFFU)
/* @brief Flash sector size. */
#define FEATURE_EFM_MAIN_ARRAY_SECTOR_SIZE   (0x200u)
/* @brief Flash has data flash. */
#define FEATURE_EFM_HAS_DATA_FLASH           (1)
#if FEATURE_EFM_HAS_DATA_FLASH
/* @brief Flash data flash start address. */
#define FEATURE_EFM_DATA_ARRAY0_START_ADDRESS (0x10001000U)
/* @brief Flash data flash end address. */
#define FEATURE_EFM_DATA_ARRAY0_END_ADDRESS   (0x100037FFU)
/* @brief Flash data flash start address. */
#define FEATURE_EFM_DATA_ARRAY1_START_ADDRESS (0x10011000U)
/* @brief Flash data flash end address. */
#define FEATURE_EFM_DATA_ARRAY1_END_ADDRESS   (0x100137FFU)
/* @brief Flash data flash sector size. */
#define FEATURE_EFM_DATA_ARRAY_SECTOR_SIZE   (0x200u)
#endif /* FEATURE_EFM_HAS_DATA_FLASH */
#define FEATURE_EFM_HAS_NVR_FLASH (1)
#if FEATURE_EFM_HAS_NVR_FLASH
/* @brief Flash NVR start address. */
#define FEATURE_EFM_NVR_ARRAY_START_ADDRESS (0x10000000U)
/* @brief Flash NVR end address. */
#define FEATURE_EFM_NVR_ARRAY_END_ADDRESS   (0x10013FFFU)
/* @brief Flash NVR sector size. */
#define FEATURE_EFM_NVR_ARRAY_SECTOR_SIZE   (0x200u)
#endif /* FEATURE_EFM_HAS_NVR_FLASH */

/* @brief Flash sector minimum size. */
#define FEATURE_EFM_FLASH_MIN_SECTOR_SIZE    (0x200u)

/* @brief Flash write unit size. */
#define FEATURE_EFM_WRITE_UNIT_SIZE                 (8u)
/* @brief Flash write unit size in words. */
#define FEATURE_EFM_WRITE_UNIT_WORD_SIZE            (FEATURE_EFM_WRITE_UNIT_SIZE >> 2)
/* @brief Flash Program unit command. */
#define FEATURE_EFM_PROGRAM_CMD_CODE                (0x01u)
/* @brief Flash Erase Sector command. */
#define FEATURE_EFM_ERASE_SECTOR_CMD_CODE           (0x10u)
/* @brief Erase Sector Retry Flash command (Soft). */
#define FEATURE_EFM_ERASE_SECTOR_RETRY_CMD_CODE     (0xFEu)
/* @brief Erase Flash Block command. */
#define FEATURE_EFM_ERASE_BLOCK_CMD_CODE            (0x11u)
/* @brief Flash Load AES KEY command. */
#define FEATURE_EFM_LOAD_AES_KEY_CMD_CODE           (0x20u)
/* @brief Flash Boot Swap command. */
#define FEATURE_EFM_BOOT_SWAP_CMD_CODE              (0x30u)
/* @brief Flash Boot Swap tag address. */
#define FEATURE_EFM_BOOT_SWAP_TAG_ADDR              (0x10013A00u)
/* @brief EFM AES key size (bytes)*/
#define FEATURE_EFM_AES_KEY_SIZE                    (16U)
#define FEATURE_EFM_AES_KEY_COUNT_MAX               (32U)
/* @brief Flash Status Code to Clear Mask before write command. */
#define FEATURE_EFM_CMD_STS_CLEAR_MASK              (EFM_STS_DONE_MASK | EFM_STS_CMD_FAIL_MASK | EFM_STS_ACCERR_MASK | EFM_STS_UNRECOVERR_MASK | EFM_STS_RECOVERR_MASK)
/* @brief Flash Status Error Code Mask. */
#define FEATURE_EFM_CMD_ERROR_MASK                  (EFM_STS_CMD_FAIL_MASK | EFM_STS_ACCERR_MASK | EFM_STS_UNRECOVERR_MASK)
/* @brief Flash Unlock command register. */
#define EFM_UNLOCK_CMD_REGISTER(base)        \
    do                                       \
    {                                        \
        (base)->CMD_UNLOCK = 0xfd9573f5U;    \
    }                                        \
    while (0)
/* @brief Flash enable flash write commands. */
#define EFM_ENABLE_WE_COMMAND(base)          \
    do                                       \
    {                                        \
        (base)->CTRL |= EFM_CTRL_WE_MASK;    \
        __ISB();                             \
        __DSB();                             \
    }                                        \
    while (0)
/* @brief Flash disable flash write commands. */
#define EFM_DISABLE_WE_COMMAND(base)         \
    do                                       \
    {                                        \
        __ISB();                             \
        __DSB();                             \
        (base)->CTRL &= ~EFM_CTRL_WE_MASK;   \
    }                                        \
    while (0)

/* RCU module features */
/* @brief Reset pin output feature */
#define FEATURE_RCU_HAS_PIN_OUTPUT                  (1U)

/* WDG module features */

/*!
 * @brief WDG clock source.
 * Implements : wdg_clock_source_t_Class
 */
typedef enum
{
    WDG_LPO_CLOCK = 0x00U, /*!< LPO clock */
    WDG_SIRC_CLOCK = 0x01U, /*!< SIRC clock */
    WDG_IPC_CLOCK = 0x02U, /*!< IPC clock */
} wdg_clock_source_t;
/* @brief The 32-bit value used for unlocking the WDG. */
#define FEATURE_WDG_UNLOCK_VALUE_1                      (0xB631)
#define FEATURE_WDG_UNLOCK_VALUE_2                      (0xC278)
/* @brief The 32-bit value used for resetting the WDG counter. */
#define FEATURE_WDG_TRIGGER_VALUE_1                     (0xA518)
#define FEATURE_WDG_TRIGGER_VALUE_2                     (0xD826)
/* @brief The reset value of the timeout register. */
#define FEATURE_WDG_TOVR_RESET_VALUE                    (0xC00U)
/* @brief The value minimum of the timeout register. */
#define FEATURE_WDG_MINIMUM_TIMEOUT_VALUE               (0x100U)
/* @brief The reset value of the window register. */
#define FEATURE_WDG_WVR_RESET_VALUE                     (0x0U)
/* @brief The mask of the reserved bit in the CS register. */
#define FEATURE_WDG_CS_RESERVED_MASK                    (0x2000U)
/* @brief The value used to set WDG source clock from LPO. */
#define FEATURE_WDG_CLK_FROM_LPO                        (0x1UL)

/* @brief Default reset value of the CS register. */
#define FEATURE_WDG_CR_RESET_VALUE                      (0x82U)

/* EWDG module features */
/* @brief First byte of the EWM Service key        */
#define FEATURE_EWDG_KEY_FIRST_BYTE                      (0xA8U)
/* @brief Second byte of the EWM Service key       */
#define FEATURE_EWDG_KEY_SECOND_BYTE                     (0x6CU)
/* @brief EWDG maximum compare high value. */
#define FEATURE_EWDG_CMPH_MAX_VALUE                      (0xFFFEU)
/* @brief EWDG minimum compare low value. */
#define FEATURE_EWDG_CMPL_MIN_VALUE                      (0x00U)
/* @brief EWDG support LPO clock */
#define FEATURE_EWDG_SUPPORT_LPO_CLOCK                   (1U)


/* Interrupt module features */

/* @brief Lowest interrupt request number. */
#define FEATURE_INTERRUPT_IRQ_MIN                       (NMI_IRQn)
/* @brief Highest interrupt request number. */
#define FEATURE_INTERRUPT_IRQ_MAX                       (HCU_IRQn)
/**< Number of priority bits implemented in the NVIC */
#define FEATURE_NVIC_PRIO_BITS                          (3U)
/* @brief Has software interrupt. */
#define FEATURE_INTERRUPT_HAS_SOFTWARE_IRQ              (0u)
/* @brief Has pending interrupt state. */
#define FEATURE_INTERRUPT_HAS_PENDING_STATE             (1u)
/* @brief Has active interrupt state. */
#define FEATURE_INTERRUPT_HAS_ACTIVE_STATE              (0u)
/* @brief Multicore support for interrupts */
#define FEATURE_INTERRUPT_MULTICORE_SUPPORT             (0u)
/* @brief Registers in which the start of interrupt vector table needs to be configured */
#define FEATURE_INTERRUPT_INT_VECTORS                   {&SCB->VTOR}

/* eTMR module features */

/* @brief Number of PWM channels */
#define FEATURE_eTMR_CHANNEL_COUNTS                     {8U, 8U}
#define FEATURE_eTMR_CHANNEL_INTERRUPT_COUNTS           (8U)
#define FEATURE_eTMR_CHANNEL_MAX_COUNT                  (8U)
/* @brief Number of fault channels */
#define FEATURE_eTMR_FAULT_CHANNELS                     (4U)
/* @brief Output channel offset */
#define FEATURE_eTMR_OUTPUT_CHANNEL_OFFSET              (16U)
/* @brief Max counter value */
#define FEATURE_eTMR_CNT_MAX_VALUE_U32                  (0xFFFFFFFFU)
/* @brief Input capture for single shot */
#define FEATURE_eTMR_INPUT_CAPTURE_SINGLE_SHOT          (2U)
/* @brief Number of interrupt vector for channels of the eTMR module. */
#define FEATURE_eTMR_HAS_NUM_IRQS_CHANS                 (4U)
/* @brief Deadtime prescaler */
#define FEATURE_eTMR_HAS_DEAD_TIME_PRESCALER            (1U)
/* @brief Double switch */
#define FEATURE_eTMR_HAS_DOUBLE_SWITCH                  (0U)
/* @brief Combination capture mode */
#define FEATURE_eTMR_HAS_COMBINATION_CAPTURE            (1U)
/* @brief Hardware capture method */
#define FEATURE_eTMR_HAS_HARDWARE_CAPTURE               (0U)
/* @brief Capture hold */
#define FEATURE_eTMR_HAS_CAPTURE_HOLD                   (1U)
/* @brief Output trigger width */
#define FEATURE_eTMR_HAS_OUTPUT_TRIGGER_WIDTH           (0U)
/* @brief MID feature */
#define FEATURE_eTMR_HAS_MID                            (0U)
/* @brief configure counter initial value source */
#define FEATURE_eTMR_HAS_CNT_INIT_SRC                   (0U)
/* @brief Counter initial value configurable */
#define FEATURE_eTMR_HAS_INIT_REG                       (0U)
/* @brief CHINIT bit is in Channel CTRL register */
#define FEATURE_eTMR_HAS_CHINIT_BIT                     (0U)
/* @brief Quadrature decoder mod register */
#define FEATURE_eTMR_HAS_QUAD_DECODER_MOD               (0U)
/* @brief Support configure dead time alone */
#define FEATURE_eTMR_DEADTIME_CONFIG_EACH_CHANNEL       (0U)
/* @brief Counter count from 1 */
#define FEATURE_eTMR_COUNT_FROM_INIT_PLUS_ONE           (1U)
/* @brief eTMR support ipc clock source */
#define FEATURE_eTMR_HAS_IPC_CLOCK_SOURCE               (1U)
/* @brief eTMR configure ipc clock source*/
#define eTMR_DRV_ConfigIpcClock(n)                      (CIM->ETMROPT0 |= ((uint32_t)0x3U << ((uint8_t)(n) * 2U)))
/* @brief eTMR internal clock source */
#define eTMR_INTERNAL_CLOCK                             (FAST_BUS_CLK)
/* @brief eTMR external ipc clock source */
#define eTMR_IPC_CLK                                    {eTMR0_CLK, eTMR1_CLK}
/* @brief eTMR external clock source frequency */
#define FEATURE_eTMR_EXTERNAL_CLOCK_FREQ                (1000000U)
/* @brief eTMR Dither  */
#define FEATURE_eTMR_HAS_DITHERING                      (0U)
/* @brief eTMR support center align calculation by hardware */
#define FEATURE_eTMR_HAS_HW_CENTER_ALIGN_CALCULATION    (0U)
/* @brief eTMR support channel val0, val1 match interrupt in PWM mode */
#define FEATURE_eTMR_HAS_CHANNEL_VALUE_MATCH_INTERRUPT_IN_PWM_MODE  (0U)

/* MPWM module features */

/* @brief MPWM internal clock source */
#define MPWM_CLK                                        {FAST_BUS_CLK}
/* @brief MPWM clock array */
#define MPWM_IPC_CLK                                    {MPWM0_CLK}
/* @brief MPWM support combination capture */
#define FEATURE_MPWM_HAS_COMBINATION_CAPTURE            (1U)
/* @brief MPWM support ldok/cldok simultaneously */
#define FEATURE_MPWM_HAS_SIMULTANEOUS_UPDATE            (1U)

/* I2C module features */

/* @brief DMA requests for I2C module. */
#define I2C_DMA_REQ                                     {{(uint8_t)DMA_REQ_I2C0_TX, (uint8_t)DMA_REQ_I2C0_RX}, \
                                                            {(uint8_t)DMA_REQ_I2C1_TX, (uint8_t)DMA_REQ_I2C1_RX}}
/* @brief IPC clocks for I2C module. */
#define I2C_CLOCK_NAMES                                 {I2C0_CLK, I2C1_CLK}
/* @brief IPC index for I2C module */
#define I2C_IPC_INDEX                                   {IPC_I2C0_INDEX, IPC_I2C1_INDEX}
/* @brief I2C state initialization */
#define I2C_STATE_PTR                                   {NULL, NULL}
/* @brief Slave mode support I2C module. */
#define I2C_SLAVE_SUPPORT                               {true, false}
/* @brief I2C module support TXCFG. */
#define I2C_SUPPORT_TXCFG                               (1)
/* @brief Disable high-speed and ultra-fast operating modes */
#define I2C_HAS_FAST_PLUS_MODE                          (1U)
#define I2C_HAS_HIGH_SPEED_MODE                         (1U)
#define I2C_HAS_ULTRA_FAST_MODE                         (1U)

/* CRC module features */

/* @brief CRC module */
#define FEATURE_CRC_DRIVER_SOFT_POLYNOMIAL
/* @brief Default CRC bit width */
#define FEATURE_CRC_DEFAULT_WIDTH                       CRC_BITS_16
/* @brief CRC support CRC 4 mode*/
#define FEATURE_CRC_SUPPORT_CRC4                        (1)
/* @brief CRC support CRC 8 mode*/
#define FEATURE_CRC_SUPPORT_CRC8                        (1)
/* @brief Default CRC read transpose */
#define FEATURE_CRC_DEFAULT_READ_TRANSPOSE              CRC_TRANSPOSE_NONE
/* @brief Default CRC write transpose */
#define FEATURE_CRC_DEFAULT_WRITE_TRANSPOSE             CRC_TRANSPOSE_NONE
/* @brief Default polynomial 0x1021U */
#define FEATURE_CRC_DEFAULT_POLYNOMIAL                  (0x1021U)
/* @brief Default seed value is 0xFFFFU */
#define FEATURE_CRC_DEFAULT_SEED                        (0xFFFFU)

/* PCTRL module features */
/* @brief Has control lock. */
#define FEATURE_PCTRL_HAS_PIN_CONTROL_LOCK              (1)
/* @brief Has open drain control. */
#define FEATURE_PINS_HAS_OPEN_DRAIN                     (0)
/* @brief Has digital filter. */
#define FEATURE_PINS_HAS_DIGITAL_FILTER                 (0)
/* @brief Has digital filter clock selection. */
#define FEATURE_PINS_HAS_FILTER_CLOCK_SEL               (0)
/* @brief Has digital filter. */
#define FEATURE_PCTRL_HAS_DIGITAL_FILTER                (1)
/* @brief Has trigger output to trigger other peripherals. */
#define FEATURE_PINS_HAS_TRIGGER_OUT                    (0)
/* @brief Has setting flag only. */
#define FEATURE_PINS_HAS_FLAG_SET_ONLY                  (0)
/* @brief Has pull resistor selection available. */
#define FEATURE_PINS_HAS_PULL_SELECTION                 (1)
/* @brief Has slew rate control. */
#define FEATURE_PINS_HAS_SLEW_RATE                      (0)
/* @brief Has filter clock selection control. */
#define FEATURE_PINS_HAS_FILTER_CLOCK_SELECTION         (0)
/* @brief Has passive filter. */
#define FEATURE_PINS_HAS_PASSIVE_FILTER                 (1)
/* @brief Has drive strength. */
#define FEATURE_PINS_HAS_DRIVE_STRENGTH                 (1)
/* @brief Has drive strength control bits*/
#define FEATURE_PINS_HAS_DRIVE_STRENGTH_CONTROL         (1)
/* @brief Has port input disable control bits*/
#define FEATURE_PINS_HAS_INPUT_DISABLE                  (1)


/* ACMP module features */

/* @brief Comparator has internal DAC module */
#define FEATURE_ACMP_HAS_DAC                            (1U)
/* @brief Comparator has power mode control */
#define FEATURE_ACMP_HAS_POWER_MODE                     (1U)
/* @brief Comparator has pin output enable */
#define FEATURE_ACMP_HAS_PINOUT_ENABLE                  (0U)
/* @brief Comparator has mode config */
#define FEATURE_ACMP_HAS_EN_REG                         (1U)
/* @brief Continuous mode is in the CTRL register */
#define FEATURE_ACMP_HAS_CONTINUOUS_MODE_IN_CTRL_REG    (1U)
/* @brief ACMP Function Clock Selection */
#define FEATURE_ACMP_HAS_CLK_SRC_SEL                    (0U)
/* @brief ACMP has filter bypass control */
#define FEATURE_ACMP_HAS_FILTER_BYPASS                  (1U)
/* @brief ACMP filter clock source is in the CTRL register */
#define FEATURE_ACMP_HAS_FILTER_CLK_SRC_IN_CTRL_REG     (1U)
/* @brief ACMP positive/negative input source selected in MUX register */
#define FEATURE_ACMP_HAS_INPUT_SRC_SEL_IN_MUX_REG       (1U)
/* @brief ACMP DAC reference voltage selection */
#define FEATURE_ACMP_HAS_DAC_VOLTAGE_REF_SRC            (0U)
/* @brief ACMP DAC output enable */
#define FEATURE_ACMP_HAS_DAC_OUTPUT                     (1U)
/* @brief ACMP Auto disable hard block */
#define FEATURE_ACMP_HAS_AUTODIS                        (0U)
/* @brief ACMP comparator offset control */
#define FEATURE_ACMP_HAS_OFFSET_CONTROL                 (0U)
/* @brief ACMP support sxosc clock */
#define FEATURE_ACMP_SUPPORT_SXOSC_CLK                  (0U)
/* @brief ACMP has EXP register */
#define FEATURE_ACMP_HAS_EXP_REG                        (1U)
/* @brief ACMP has trigger mode gate */
#define FEATURE_ACMP_HAS_TRIG_MODE_GATE                 (1U)
/* @brief ACMP supports high/low level detection */
#define FEATURE_ACMP_SUPPORT_LEVEL_DETECTION            (1U)

#define FEATURE_ACMP_CTRL_RESET_VALUE                   (0x00000000)
#define FEATURE_ACMP_STS_CLEAR_MASK                     (0x00FF0000)
#define FEATURE_ACMP_STS_CH_OUT_MASK                    (0xFF000000)
#define FEATURE_ACMP_STS_CH_OUT_SHIFT                   (24)
#define FEATURE_ACMP_STS_CH_FLAG_MASK                   (0x00FF0000)
#define FEATURE_ACMP_STS_CH_FLAG_SHIFT                  (16)
#define FEATURE_ACMP_DEFAULT_CONT_PERIOD                (0x30)
#define FEATURE_ACMP_DEFAULT_CONT_POS                   (0x18)

#if FEATURE_ACMP_HAS_DAC
/* @brief Comparator fix DAC input to mux side */
#define FEATURE_ACMP_DAC_FIX_SELECTION                  (0U)
#define ACMP_DAC_RESOLUTION                             (255U)
#endif /* FEATURE_ACMP_HAS_DAC */

/* IPC module features */
/* @brief IPC support FAST BUS clock */
#define FEATURE_IPC_SUPPORT_FAST_BUS_CLK                (1)

/*! @brief Clock names. */
typedef enum {
    /* IPC clocks */
    DMA_CLK                  =  0U, /*!< DMA        clock source */
    GPIO_CLK                 =  1U, /*!< GPIO       clock source */
    PCTRLA_CLK               =  2U, /*!< PCTRLA     clock source */
    PCTRLB_CLK               =  3U, /*!< PCTRLB     clock source */
    PCTRLC_CLK               =  4U, /*!< PCTRLC     clock source */
    PCTRLD_CLK               =  5U, /*!< PCTRLD     clock source */
    PCTRLE_CLK               =  6U, /*!< PCTRLE     clock source */
    UART0_CLK                =  7U, /*!< UART0      clock source */
    UART1_CLK                =  8U, /*!< UART1      clock source */
    UART2_CLK                =  9U, /*!< UART2      clock source */
    I2C0_CLK                 = 10U, /*!< I2C0       clock source */
    I2C1_CLK                 = 11U, /*!< I2C1       clock source */
    SPI0_CLK                 = 12U, /*!< SPI0       clock source */
    SPI1_CLK                 = 13U, /*!< SPI1       clock source */
    SPI2_CLK                 = 14U, /*!< SPI2       clock source */
    FlexCAN0_CLK             = 15U, /*!< FlexCAN0   clock source */
    FlexCAN1_CLK             = 16U, /*!< FlexCAN1   clock source */
    ADC0_CLK                 = 17U, /*!< ADC0       clock source */
    ACMP0_CLK                = 18U, /*!< ACMP0      clock source */
    TMU_CLK                  = 19U, /*!< TMU        clock source */
    eTMR0_CLK                = 20U, /*!< eTMR0      clock source */
    eTMR1_CLK                = 21U, /*!< eTMR1      clock source */
    MPWM0_CLK                = 22U, /*!< MPWM0      clock source */
    pTMR0_CLK                = 23U, /*!< pTMR0      clock source */
    lpTMR0_CLK               = 24U, /*!< lpTMR0     clock source */
    CRC_CLK                  = 25U, /*!< CRC        clock source */
    TRNG_CLK                 = 26U, /*!< TRNG       clock source */
    HCU_CLK                  = 27U, /*!< HCU        clock source */
    WDG0_CLK                 = 28U, /*!< WDG0       clock source */
    EWDG0_CLK                = 29U, /*!< EWDG0      clock source */
    EMU0_CLK                 = 30U, /*!< EMU0       clock source */
    STU_CLK                  = 31U, /*!< STU        clock source */
    CIM_CLK                  = 32U, /*!< CIM        clock source */
    SCU_CLK                  = 33U, /*!< SCU        clock source */

    IPC_PERI_END_OF_CLOCK    = 34U, /*!< IPC_PERI_END_OF_CLOCK */
    IPC_SIRC_CLK             = 35U, /*!< IPC_SIRC   clock source */
    IPC_FIRC_CLK             = 36U, /*!< IPC_FIRC   clock source */
    IPC_FXOSC_CLK            = 37U, /*!< IPC_FXOSC  clock source */
    IPC_LPO_CLK              = 38U, /*!< IPC_LPO    clock source */

    IPC_END_OF_CLOCK         = 39U, /*!< IPC_END_OF_CLOCK */
    CORE_CLK                 = 40U, /*!< CORE       clock source */
    FAST_BUS_CLK             = 41U, /*!< FAST_BUS   clock source */
    SLOW_BUS_CLK             = 42U, /*!< SLOW_BUS   clock source */

    CLOCK_NAME_COUNT         = 43U, /*!< CLOCK_NAME_COUNT */
} clock_names_t;

/*! @brief SCU clock output selection */
typedef enum
{
    SCU_CLKOUT_SEL_DISABLE      = 0U,     /*!< SCU CLKOUT Disable               */
    SCU_CLKOUT_SEL_FIRC_CLK     = 1U,     /*!< SCU CLKOUT Select FIRC CLK       */
    SCU_CLKOUT_SEL_SIRC_CLK     = 2U,     /*!< SCU CLKOUT Select SIRC CLK       */
    SCU_CLKOUT_SEL_FXOSC_CLK    = 3U,     /*!< SCU CLKOUT Select FXOSC CLK      */
    SCU_CLKOUT_SEL_LPO_CLK      = 5U,     /*!< SCU CLKOUT Select LPO CLK        */
    SCU_CLKOUT_SEL_TRNG_CLK     = 6U,     /*!< SCU CLKOUT Select TRNG OSC CLK   */
    SCU_CLKOUT_SEL_CORE_CLK     = 7U,     /*!< SCU CLKOUT Select CORE CLK       */
} scu_clkout_src_t;

/*! @brief IPC clock name mappings
 *  Mappings between clock names and peripheral clock control indexes.
 *  If there is no peripheral clock control index for a clock name,
 *  then the corresponding value is IPC_INVALID_INDEX.
 */
#define IPC_DMA_INDEX          (  8U)
#define IPC_GPIO_INDEX         ( 17U)
#define IPC_PCTRLA_INDEX       ( 18U)
#define IPC_PCTRLB_INDEX       ( 19U)
#define IPC_PCTRLC_INDEX       ( 20U)
#define IPC_PCTRLD_INDEX       ( 21U)
#define IPC_PCTRLE_INDEX       ( 22U)
#define IPC_UART0_INDEX        ( 27U)
#define IPC_UART1_INDEX        ( 28U)
#define IPC_UART2_INDEX        ( 29U)
#define IPC_I2C0_INDEX         ( 35U)
#define IPC_I2C1_INDEX         ( 36U)
#define IPC_SPI0_INDEX         ( 42U)
#define IPC_SPI1_INDEX         ( 43U)
#define IPC_SPI2_INDEX         ( 44U)
#define IPC_FlexCAN0_INDEX     ( 48U)
#define IPC_FlexCAN1_INDEX     ( 52U)
#define IPC_ADC0_INDEX         ( 64U)
#define IPC_ACMP0_INDEX        ( 67U)
#define IPC_TMU_INDEX          ( 72U)
#define IPC_eTMR0_INDEX        ( 73U)
#define IPC_eTMR1_INDEX        ( 74U)
#define IPC_MPWM0_INDEX        ( 79U)
#define IPC_pTMR0_INDEX        ( 90U)
#define IPC_lpTMR0_INDEX       ( 93U)
#define IPC_CRC_INDEX          (101U)
#define IPC_TRNG_INDEX         (102U)
#define IPC_HCU_INDEX          (103U)
#define IPC_WDG0_INDEX         (106U)
#define IPC_EWDG0_INDEX        (107U)
#define IPC_EMU0_INDEX         (114U)
#define IPC_STU_INDEX          (119U)
#define IPC_CIM_INDEX          (123U)
#define IPC_SCU_INDEX          (124U)
#define IPC_PERI_END_OF_CLOCK_INDEX   (125U)
#define IPC_SIRC_INDEX         (126U)
#define IPC_FIRC_INDEX         (127U)
#define IPC_FXOSC_INDEX        (128U)
#define IPC_LPO_INDEX          (129U)
#define IPC_END_OF_CLOCK_INDEX   (130U)
#define IPC_CORE_INDEX         (131U)
#define IPC_FAST_BUS_INDEX     (132U)
#define IPC_SLOW_BUS_INDEX     (133U)

#define  IPC_CLOCK_NAME_MAPPINGS \
{                                \
    IPC_DMA_INDEX,            /*!< DMA        clock source    8u */ \
    IPC_GPIO_INDEX,           /*!< GPIO       clock source   17u */ \
    IPC_PCTRLA_INDEX,         /*!< PCTRLA     clock source   18u */ \
    IPC_PCTRLB_INDEX,         /*!< PCTRLB     clock source   19u */ \
    IPC_PCTRLC_INDEX,         /*!< PCTRLC     clock source   20u */ \
    IPC_PCTRLD_INDEX,         /*!< PCTRLD     clock source   21u */ \
    IPC_PCTRLE_INDEX,         /*!< PCTRLE     clock source   22u */ \
    IPC_UART0_INDEX,          /*!< UART0      clock source   27u */ \
    IPC_UART1_INDEX,          /*!< UART1      clock source   28u */ \
    IPC_UART2_INDEX,          /*!< UART2      clock source   29u */ \
    IPC_I2C0_INDEX,           /*!< I2C0       clock source   35u */ \
    IPC_I2C1_INDEX,           /*!< I2C1       clock source   36u */ \
    IPC_SPI0_INDEX,           /*!< SPI0       clock source   42u */ \
    IPC_SPI1_INDEX,           /*!< SPI1       clock source   43u */ \
    IPC_SPI2_INDEX,           /*!< SPI2       clock source   44u */ \
    IPC_FlexCAN0_INDEX,       /*!< FlexCAN0   clock source   48u */ \
    IPC_FlexCAN1_INDEX,       /*!< FlexCAN1   clock source   52u */ \
    IPC_ADC0_INDEX,           /*!< ADC0       clock source   64u */ \
    IPC_ACMP0_INDEX,          /*!< ACMP0      clock source   67u */ \
    IPC_TMU_INDEX,            /*!< TMU        clock source   72u */ \
    IPC_eTMR0_INDEX,          /*!< eTMR0      clock source   73u */ \
    IPC_eTMR1_INDEX,          /*!< eTMR1      clock source   74u */ \
    IPC_MPWM0_INDEX,          /*!< MPWM0      clock source   79u */ \
    IPC_pTMR0_INDEX,          /*!< pTMR0      clock source   90u */ \
    IPC_lpTMR0_INDEX,         /*!< lpTMR0     clock source   93u */ \
    IPC_CRC_INDEX,            /*!< CRC        clock source  101u */ \
    IPC_TRNG_INDEX,           /*!< TRNG       clock source  102u */ \
    IPC_HCU_INDEX,            /*!< HCU        clock source  103u */ \
    IPC_WDG0_INDEX,           /*!< WDG0       clock source  106u */ \
    IPC_EWDG0_INDEX,          /*!< EWDG0      clock source  107u */ \
    IPC_EMU0_INDEX,           /*!< EMU0       clock source  114u */ \
    IPC_STU_INDEX,            /*!< STU        clock source  119u */ \
    IPC_CIM_INDEX,            /*!< CIM        clock source  123u */ \
    IPC_SCU_INDEX,            /*!< SCU        clock source  124u */ \
    IPC_PERI_END_OF_CLOCK_INDEX,     /*!< PERI_END_OF_CLOCK clock source  125u */ \
    IPC_SIRC_INDEX,           /*!< SIRC       clock source  126u */ \
    IPC_FIRC_INDEX,           /*!< FIRC       clock source  127u */ \
    IPC_FXOSC_INDEX,          /*!< FXOSC      clock source  128u */ \
    IPC_LPO_INDEX,            /*!< LPO        clock source  129u */ \
    IPC_END_OF_CLOCK_INDEX,     /*!< END_OF_CLOCK clock source  130u */ \
    IPC_CORE_INDEX,           /*!< CORE       clock source  131u */ \
    IPC_FAST_BUS_INDEX,       /*!< FAST_BUS   clock source  132u */ \
    IPC_SLOW_BUS_INDEX,       /*!< SLOW_BUS   clock source  133u */ \
}
/*! @brief IPC clock source select
 *  Implements peripheral_clock_source_t_Class
 */
typedef enum
{
    CLK_SRC_DISABLED        = 0x00U,            /*!< Disabled clock */
    CLK_SRC_FIRC            = 0x01U,            /*!< Fast IRC Clock */
    CLK_SRC_SIRC            = 0x02U,            /*!< Slow IRC Clock */
    CLK_SRC_FXOSC           = 0x03U,            /*!< External Fast OSC Clock */
    CLK_SRC_LPO             = 0x04U,            /*!< Internal LPO Clock */
    CLK_SRC_FAST_BUS        = 0x07U,            /*!< Internal Fast Bus Clock */
} peripheral_clock_source_t;

/* DMA module features */

/* @brief Number of DMA channels. */
#define FEATURE_DMA_CHANNELS (4U)
/* @brief Number of DMA virtual channels. */
#define FEATURE_DMA_VIRTUAL_CHANNELS (FEATURE_DMA_CHANNELS * DMA_INSTANCE_COUNT)
/* @brief Number of DMA interrupt lines. */
#define FEATURE_DMA_CHANNELS_INTERRUPT_LINES (4U)
/* @brief Number of DMA virtual interrupt lines. */
#define FEATURE_DMA_VIRTUAL_CHANNELS_INTERRUPT_LINES        (FEATURE_DMA_CHANNELS_INTERRUPT_LINES * DMA_INSTANCE_COUNT)
/* @brief Number of DMA error interrupt lines. */
#define FEATURE_DMA_ERROR_INTERRUPT_LINES (1U)
/* @brief Number of DMA virtual error interrupt lines. */
#define FEATURE_DMA_VIRTUAL_ERROR_INTERRUPT_LINES           (FEATURE_DMA_ERROR_INTERRUPT_LINES * DMA_INSTANCE_COUNT)
/* @brief DMA module has error interrupt. */
#define FEATURE_DMA_HAS_ERROR_IRQ
/* @brief DMA module separate interrupt lines for each channel */
#define FEATURE_DMA_SEPARATE_IRQ_LINES_PER_CHN
/* @brief Clock name for DMA */
#define FEATURE_DMA_CLOCK_NAMES {DMA_CLK}
/* @brief DMA channel width based on number of CTSs: 2^N, N=4,5,... */
#define FEATURE_DMA_CH_WIDTH (2U)
/* @brief DMA channel to instance */
#define FEATURE_DMA_VCH_TO_INSTANCE(x) 	((x) >> (uint32_t)FEATURE_DMA_CH_WIDTH)
/* @brief DMA virtual channel to channel */
#define FEATURE_DMA_VCH_TO_CH(x)        ((x) & ((uint32_t)FEATURE_DMA_CHANNELS - 1U))
/* @brief DMA supports bus bandwidth control. */
#define FEATURE_DMA_ENGINE_STALL

/* DMAMUX module features */

/* @brief DMAMUX peripheral is available in silicon. */
#define FEATURE_DMAMUX_AVAILABLE
/* @brief Number of DMA channels. */
#define FEATURE_DMAMUX_CHANNELS (16U)
/* @brief Conversion from request source to the actual DMAMUX channel */
#define FEATURE_DMAMUX_REQ_SRC_TO_CH(x) (x)
/* @brief Mapping between request source and DMAMUX instance */
#define FEATURE_DMAMUX_REQ_SRC_TO_INSTANCE(x) (0U)
/* @brief Conversion from DMA channel index to DMAMUX channel. */
#define FEATURE_DMAMUX_DMA_CH_TO_CH(x) (x)
/* @brief Conversion from DMAMUX channel DMAMUX register index. */
#define FEATURE_DMAMUX_CHN_REG_INDEX(x) (x)

/*!
 * @brief Structure for the DMA hardware request
 *
 * Defines the structure for the DMA hardware request collections. The user can configure the
 * hardware request into DMAMUX to trigger the DMA transfer accordingly. The index
 * of the hardware request varies according  to the to SoC.
 */
typedef enum {
    DMA_REQ_DISABLED         =   0U,     /*!< Disabled         dma request    0u */ \
    DMA_REQ_UART0_RX         =   2U,     /*!< UART0_RX         dma request    2u */ \
    DMA_REQ_UART0_TX         =   3U,     /*!< UART0_TX         dma request    3u */ \
    DMA_REQ_UART1_RX         =   4U,     /*!< UART1_RX         dma request    4u */ \
    DMA_REQ_UART1_TX         =   5U,     /*!< UART1_TX         dma request    5u */ \
    DMA_REQ_UART2_RX         =   6U,     /*!< UART2_RX         dma request    6u */ \
    DMA_REQ_UART2_TX         =   7U,     /*!< UART2_TX         dma request    7u */ \
    DMA_REQ_I2C1_RX          =   8U,     /*!< I2C1_RX          dma request    8u */ \
    DMA_REQ_I2C1_TX          =   9U,     /*!< I2C1_TX          dma request    9u */ \
    DMA_REQ_SPI0_RX          =  14U,     /*!< SPI0_RX          dma request   14u */ \
    DMA_REQ_SPI0_TX          =  15U,     /*!< SPI0_TX          dma request   15u */ \
    DMA_REQ_SPI1_RX          =  16U,     /*!< SPI1_RX          dma request   16u */ \
    DMA_REQ_SPI1_TX          =  17U,     /*!< SPI1_TX          dma request   17u */ \
    DMA_REQ_SPI2_RX          =  18U,     /*!< SPI2_RX          dma request   18u */ \
    DMA_REQ_SPI2_TX          =  19U,     /*!< SPI2_TX          dma request   19u */ \
    DMA_REQ_eTMR1_CH0        =  20U,     /*!< eTMR1_CH0        dma request   20u */ \
    DMA_REQ_eTMR1_CH1        =  21U,     /*!< eTMR1_CH1        dma request   21u */ \
    DMA_REQ_eTMR1_CH2        =  22U,     /*!< eTMR1_CH2        dma request   22u */ \
    DMA_REQ_eTMR1_CH3        =  23U,     /*!< eTMR1_CH3        dma request   23u */ \
    DMA_REQ_eTMR1_CH4        =  24U,     /*!< eTMR1_CH4        dma request   24u */ \
    DMA_REQ_eTMR1_CH5        =  25U,     /*!< eTMR1_CH5        dma request   25u */ \
    DMA_REQ_eTMR1_CH6        =  26U,     /*!< eTMR1_CH6        dma request   26u */ \
    DMA_REQ_eTMR1_CH7        =  27U,     /*!< eTMR1_CH7        dma request   27u */ \
    DMA_REQ_MPWM0_CH0        =  28U,     /*!< MPWM0_CH0        dma request   28u */ \
    DMA_REQ_MPWM0_CH1        =  29U,     /*!< MPWM0_CH1        dma request   29u */ \
    DMA_REQ_MPWM0_CH2        =  30U,     /*!< MPWM0_CH2        dma request   30u */ \
    DMA_REQ_MPWM0_CH3        =  31U,     /*!< MPWM0_CH3        dma request   31u */ \
    DMA_REQ_MPWM0_CH4        =  32U,     /*!< MPWM0_CH4        dma request   32u */ \
    DMA_REQ_MPWM0_CH5        =  33U,     /*!< MPWM0_CH5        dma request   33u */ \
    DMA_REQ_MPWM0_CH6        =  34U,     /*!< MPWM0_CH6        dma request   34u */ \
    DMA_REQ_MPWM0_CH7        =  35U,     /*!< MPWM0_CH7        dma request   35u */ \
    DMA_REQ_eTMR0            =  36U,     /*!< eTMR0            dma request   36u */ \
    DMA_REQ_HCU_INGRESS      =  40U,     /*!< HCU_INGRESS      dma request   40u */ \
    DMA_REQ_HCU_EGRESS       =  41U,     /*!< HCU_EGRESS       dma request   41u */ \
    DMA_REQ_ADC0             =  42U,     /*!< ADC0             dma request   42u */ \
    DMA_REQ_I2C0_RX          =  44U,     /*!< I2C0_RX          dma request   44u */ \
    DMA_REQ_I2C0_TX          =  45U,     /*!< I2C0_TX          dma request   45u */ \
    DMA_REQ_ACMP0            =  48U,     /*!< ACMP0            dma request   48u */ \
    DMA_REQ_GPIOA            =  49U,     /*!< GPIOA            dma request   49u */ \
    DMA_REQ_GPIOB            =  50U,     /*!< GPIOB            dma request   50u */ \
    DMA_REQ_GPIOC            =  51U,     /*!< GPIOC            dma request   51u */ \
    DMA_REQ_GPIOD            =  52U,     /*!< GPIOD            dma request   52u */ \
    DMA_REQ_GPIOE            =  53U,     /*!< GPIOE            dma request   53u */ \
    DMA_REQ_FLEXCAN0         =  54U,     /*!< FLEXCAN0         dma request   54u */ \
    DMA_REQ_FLEXCAN1         =  55U,     /*!< FLEXCAN1         dma request   55u */ \
    DMA_REQ_lpTMR0           =  59U,     /*!< lpTMR0           dma request   59u */ \
    DMA_REQ_Always_On0       =  62U,     /*!< Always_On0       dma request   62u */ \
    DMA_REQ_Always_On1       =  63U,     /*!< Always_On1       dma request   63u */ \
} dma_request_source_t;

/* TMU module features */
/*!
 * @brief Enumeration for trigger source module of TMU
 *
 * Describes all possible inputs (trigger sources) of the TMU IP
 * This enumeration depends on the supported instances in device
 */
enum tmu_trigger_source_e
{
    TMU_TRIG_SOURCE_DISABLED                       = 0U,
    TMU_TRIG_SOURCE_VDD                            = 1U,
    TMU_TRIG_SOURCE_TMU_IN0                        = 2U,
    TMU_TRIG_SOURCE_TMU_IN1                        = 3U,
    TMU_TRIG_SOURCE_TMU_IN2                        = 4U,
    TMU_TRIG_SOURCE_TMU_IN3                        = 5U,
    TMU_TRIG_SOURCE_TMU_IN4                        = 6U,
    TMU_TRIG_SOURCE_TMU_IN5                        = 7U,
    TMU_TRIG_SOURCE_TMU_IN6                        = 8U,
    TMU_TRIG_SOURCE_TMU_IN7                        = 9U,
    TMU_TRIG_SOURCE_TMU_IN8                        = 10U,
    TMU_TRIG_SOURCE_TMU_IN9                        = 11U,
    TMU_TRIG_SOURCE_ACMP0_OUT                      = 14U,
    TMU_TRIG_SOURCE_MPWM0_CH0                      = 15U,
    TMU_TRIG_SOURCE_MPWM0_CH1                      = 16U,
    TMU_TRIG_SOURCE_pTMR_CH0                       = 17U,
    TMU_TRIG_SOURCE_pTMR_CH1                       = 18U,
    TMU_TRIG_SOURCE_pTMR_CH2                       = 19U,
    TMU_TRIG_SOURCE_pTMR_CH3                       = 20U,
    TMU_TRIG_SOURCE_lpTMR0                         = 21U,
    TMU_TRIG_SOURCE_eTMR0_INIT_TRIG                = 22U,
    TMU_TRIG_SOURCE_eTMR0_EXT_TRIG                 = 23U,
    TMU_TRIG_SOURCE_eTMR1_EXT_TRIG                 = 25U,
    TMU_TRIG_SOURCE_MPWM0_CH2                      = 28U,
    TMU_TRIG_SOURCE_MPWM0_CH3                      = 29U,
    TMU_TRIG_SOURCE_ADC0_COCO                      = 30U,
    TMU_TRIG_SOURCE_ADC0_AWD                       = 31U,
    TMU_TRIG_SOURCE_MPWM0_CH4                      = 32U,
    TMU_TRIG_SOURCE_MPWM0_CH5                      = 33U,
    TMU_TRIG_SOURCE_MPWM0_CH6                      = 34U,
    TMU_TRIG_SOURCE_MPWM0_CH7                      = 35U,
    TMU_TRIG_SOURCE_UART2_RX_data                  = 40U,
    TMU_TRIG_SOURCE_UART2_TX_data                  = 41U,
    TMU_TRIG_SOURCE_UART2_RX_idle                  = 42U,
    TMU_TRIG_SOURCE_SPI2_Frame                     = 45U,
    TMU_TRIG_SOURCE_SPI2_RX_data                   = 46U,
    TMU_TRIG_SOURCE_UART0_RX_data                  = 49U,
    TMU_TRIG_SOURCE_UART0_TX_data                  = 50U,
    TMU_TRIG_SOURCE_UART0_RX_idle                  = 51U,
    TMU_TRIG_SOURCE_UART1_RX_data                  = 52U,
    TMU_TRIG_SOURCE_UART1_TX_data                  = 53U,
    TMU_TRIG_SOURCE_UART1_RX_idle                  = 54U,
    TMU_TRIG_SOURCE_I2C0_MASTER_TRIG               = 55U,
    TMU_TRIG_SOURCE_I2C0_SLAVE_TRIG                = 56U,
    TMU_TRIG_SOURCE_I2C1_MASTER_TRIG               = 57U,
    TMU_TRIG_SOURCE_SPI0_FRAME                     = 59U,
    TMU_TRIG_SOURCE_SPI0_RX_DATA                   = 60U,
    TMU_TRIG_SOURCE_SPI1_FRAME                     = 61U,
    TMU_TRIG_SOURCE_SPI1_RX_DATA                   = 62U,
    TMU_TRIG_SOURCE_SIM_SW_TRIG                    = 63U,
};

/*!
 * @brief Enumeration for target module of TMU
 *
 * Describes all possible outputs (target modules) of the TMU IP
 * This enumeration depends on the supported instances in device
 */
enum tmu_target_module_e
{
    TMU_TARGET_MODULE_TMU_OUT0                       = 4U,
    TMU_TARGET_MODULE_TMU_OUT1                       = 5U,
    TMU_TARGET_MODULE_TMU_OUT2                       = 6U,
    TMU_TARGET_MODULE_TMU_OUT3                       = 7U,
    TMU_TARGET_MODULE_TMU_OUT4                       = 8U,
    TMU_TARGET_MODULE_TMU_OUT5                       = 9U,
    TMU_TARGET_MODULE_ADC0_EXT_TRIG                  = 12U,
    TMU_TARGET_MODULE_MPWM0_TRIG0                    = 16U,
    TMU_TARGET_MODULE_MPWM0_TRIG1                    = 17U,
    TMU_TARGET_MODULE_MPWM0_TRIG2                    = 18U,
    TMU_TARGET_MODULE_MPWM0_TRIG3                    = 19U,
    TMU_TARGET_MODULE_lpTMR0_ALT0                    = 21U,
    TMU_TARGET_MODULE_MPWM0_TRIG4                    = 24U,
    TMU_TARGET_MODULE_MPWM0_TRIG5                    = 25U,
    TMU_TARGET_MODULE_MPWM0_TRIG6                    = 26U,
    TMU_TARGET_MODULE_MPWM0_TRIG7                    = 27U,
    TMU_TARGET_MODULE_ACMP0_SAMPLE_INPUT             = 28U,
    TMU_TARGET_MODULE_eTMR0_HWTRIG0                  = 40U,
    TMU_TARGET_MODULE_eTMR0_FAULT0                   = 41U,
    TMU_TARGET_MODULE_eTMR0_FAULT1                   = 42U,
    TMU_TARGET_MODULE_eTMR0_FAULT2                   = 43U,
    TMU_TARGET_MODULE_eTMR1_HWTRIG0                  = 44U,
    TMU_TARGET_MODULE_eTMR1_FAULT0                   = 45U,
    TMU_TARGET_MODULE_eTMR1_FAULT1                   = 46U,
    TMU_TARGET_MODULE_eTMR1_FAULT2                   = 47U,
    TMU_TARGET_MODULE_I2C1_TRIG                      = 57U,
    TMU_TARGET_MODULE_I2C2_TRIG                      = 58U,
};

/* @brief Constant array storing the value of all TMU output(target module) identifiers */
#define FEATURE_TMU_TARGET_MODULE                     \
    {                                                 \
    TMU_TARGET_MODULE_TMU_OUT0,                       \
    TMU_TARGET_MODULE_TMU_OUT1,                       \
    TMU_TARGET_MODULE_TMU_OUT2,                       \
    TMU_TARGET_MODULE_TMU_OUT3,                       \
    TMU_TARGET_MODULE_TMU_OUT4,                       \
    TMU_TARGET_MODULE_TMU_OUT5,                       \
    TMU_TARGET_MODULE_ADC0_EXT_TRIG,                  \
    TMU_TARGET_MODULE_MPWM0_TRIG0,                    \
    TMU_TARGET_MODULE_MPWM0_TRIG1,                    \
    TMU_TARGET_MODULE_MPWM0_TRIG2,                    \
    TMU_TARGET_MODULE_MPWM0_TRIG3,                    \
    TMU_TARGET_MODULE_lpTMR0_ALT0,                    \
    TMU_TARGET_MODULE_MPWM0_TRIG4,                    \
    TMU_TARGET_MODULE_MPWM0_TRIG5,                    \
    TMU_TARGET_MODULE_MPWM0_TRIG6,                    \
    TMU_TARGET_MODULE_MPWM0_TRIG7,                    \
    TMU_TARGET_MODULE_ACMP0_SAMPLE_INPUT,             \
    TMU_TARGET_MODULE_eTMR0_HWTRIG0,                  \
    TMU_TARGET_MODULE_eTMR0_FAULT0,                   \
    TMU_TARGET_MODULE_eTMR0_FAULT1,                   \
    TMU_TARGET_MODULE_eTMR0_FAULT2,                   \
    TMU_TARGET_MODULE_eTMR1_HWTRIG0,                  \
    TMU_TARGET_MODULE_eTMR1_FAULT0,                   \
    TMU_TARGET_MODULE_eTMR1_FAULT1,                   \
    TMU_TARGET_MODULE_eTMR1_FAULT2,                   \
    TMU_TARGET_MODULE_I2C1_TRIG,                      \
    TMU_TARGET_MODULE_I2C2_TRIG,                      \
    }

/* SPI module features */
/* @brief Initial value for state structure */
#define FEATURE_SPI_STATE_STRUCTURES_NULL {NULL, NULL, NULL}
/* @brief Has transmitter/receiver DMA enable bits. */
#define FEATURE_SPI_HAS_DMA_ENABLE (1)
/* @brief Clock indexes for SPI clock */
#define FEATURE_SPI_CLOCKS_NAMES {SPI0_CLK, SPI1_CLK, SPI2_CLK}

/* ADC module features */

/*! @brief ADC feature flag for extended number of SC1 and R registers,
 * generically named 'alias registers' */
#define FEATURE_ADC_HAS_EXTRA_NUM_REGS                    (0)
/* @brief ADC Support keep working in lp mode */
#define FEATURE_ADC_SUPPORT_LOW_POWER_KEEP                (1U)

#define NUMBER_OF_ALT_CLOCKS  ADC_CLK_ALT_1
/*! @brief ADC feature flag for defining number of external ADC channels.
 * If each ADC instance has different number of external channels, then
 * this define is set with the maximum value. */
#define FEATURE_ADC_MAX_NUM_EXT_CHANS                     (33)
#define ADC_CLOCKS                                        {ADC0_CLK}

/*! @brief ADC default clock divider from RM */
#define ADC_DEFAULT_CLOCK_DIVIDER                         (0x02U)
/*! @brief ADC default Sample Time from RM */
#define ADC_DEFAULT_SAMPLE_TIME                           (0x0CU)
/*! @brief ADC default Start Time from RM */
#define ADC_DEFAULT_START_TIME                            (0x20U)
/* @brief Max of adc clock frequency */
#define ADC_CLOCK_FREQ_MAX_RUNTIME                        (32000000u)
/* @brief Min of adc clock frequency */
#define ADC_CLOCK_FREQ_MIN_RUNTIME                        (1000u)

/* CAN module features */

/* @brief FlexCAN state ptr initialization */
#define FEATURE_FLEXCAN_STATE_PTR           { NULL, NULL }
/* @brief Frames available in Rx FIFO flag shift */
#define FEATURE_CAN_RXFIFO_FRAME_AVAILABLE  (5U)
/* @brief Rx FIFO warning flag shift */
#define FEATURE_CAN_RXFIFO_WARNING          (6U)
/* @brief Rx FIFO overflow flag shift */
#define FEATURE_CAN_RXFIFO_OVERFLOW         (7U)
/* @brief The list contains definitions of the FD feature support on all instances */
#define FEATURE_CAN_INSTANCES_HAS_FD        {true, true}

/* @brief Maximum number of Message Buffers supported for payload size 8 for CAN0 */
#define FEATURE_CAN0_MAX_MB_NUM             (64U)
/* @brief Maximum number of Message Buffers supported for payload size 8 for CAN1 */
#define FEATURE_CAN1_MAX_MB_NUM             (64U)

/* @brief Has PE clock source select (bit field CAN_CTRL1[CLKSRC]). */
#define FEATURE_CAN_HAS_PE_CLKSRC_SELECT    (1)
/* @brief Has DMA enable (bit field MCR[DMA]). */
#define FEATURE_CAN_HAS_DMA_ENABLE          (1)

/* @brief Maximum number of Message Buffers supported for payload size 8 for any of the CAN instances */
#define FEATURE_CAN_MAX_MB_NUM              (64U)
/* @brief Maximum number of Message Buffers supported for payload size 8 for any of the CAN instances */
#define FEATURE_CAN_MAX_MB_NUM_ARRAY        { FEATURE_CAN0_MAX_MB_NUM, \
                                              FEATURE_CAN1_MAX_MB_NUM}
/* @brief Has Pretending Networking mode */
#define FEATURE_CAN_HAS_PRETENDED_NETWORKING    (0)
/* @brief Has Stuff Bit Count Enable Bit */
#define FEATURE_CAN_HAS_STFCNTEN_ENABLE         (0)
/* @brief Has ISO CAN FD Enable Bit */
#define FEATURE_CAN_HAS_ISOCANFDEN_ENABLE       (1)
/* @brief Has Message Buffer Data Size Region 1 */
#define FEATURE_CAN_HAS_MBDSR1                  (1)
/* @brief Has Message Buffer Data Size Region 2 */
#define FEATURE_CAN_HAS_MBDSR2                  (0)
/* @brief DMA hardware requests for all FlexCAN instances */
#define FEATURE_CAN_DMA_REQUESTS              { DMA_REQ_FLEXCAN0, \
                                                DMA_REQ_FLEXCAN1}

/* @brief Maximum number of Message Buffers IRQs */
#define FEATURE_CAN_MB_IRQS_MAX_COUNT       (4U)
/* @brief Message Buffers IRQs */
#define FEATURE_CAN_MB_IRQS                 { CAN_ORed_0_15_MB_IRQS, CAN_ORed_16_31_MB_IRQS,  CAN_ORed_32_47_MB_IRQS,  CAN_ORed_48_63_MB_IRQS }
/* @brief Has Wake Up Irq channels (CAN_Wake_Up_IRQS_CH_COUNT > 0u) */
#define FEATURE_CAN_HAS_WAKE_UP_IRQ         (1)
/* @brief Has Self Wake Up mode */
#define FEATURE_CAN_HAS_SELF_WAKE_UP        (1)
/* @brief Has Flexible Data Rate */
#define FEATURE_CAN_HAS_FD                  (1)
/* @brief Has RAM ECC */
#define FEATURE_CAN_HAS_RAM_ECC             (0)
/* @brief Clock name for the PE oscillator clock source */
#define FEATURE_CAN_PE_OSC_CLK_NAME         IPC_FXOSC_CLK
/* @bried FlexCAN has Detection And Correction of Memory Errors */
#define FEATURE_CAN_HAS_MEM_ERR_DET			(1)

#define FEATURE_CAN_RAM_COUNT               (256)
#define FEATURE_CAN_RXIMR_COUNT             (64)
/* @brief Has enhanced rx fifo feature for CAN0 */
#define FEATURE_CAN0_HAS_ENHANCE_RX_FIFO (0)
/* @brief Has enhanced rx fifo feature for CAN1 */
#define FEATURE_CAN1_HAS_ENHANCE_RX_FIFO (0)

/* @brief Has enhanced rx fifo feature for all CAN */
#define FEATURE_CAN_HAS_ENHANCE_RX_FIFO  (0)

/* @brief The list contains definitions of the rx-fifo feature support on all instances */
#define FEATURE_CAN_INSTANCES_HAS_ENHANCE_RX_FIFO        { FEATURE_CAN0_HAS_ENHANCE_RX_FIFO, \
                                                           FEATURE_CAN1_HAS_ENHANCE_RX_FIFO }
#define FEATURE_CAN_ENHANCE_RX_FIFO_COUNT (20U)

/* @brief High-resolution time stamp feature for CAN0 */
#define FEATURE_CAN0_HAS_HR_TIMESTAMP (0U)
/* @brief High-resolution time stamp feature for CAN1 */
#define FEATURE_CAN1_HAS_HR_TIMESTAMP (0U)
/* @brief High-resolution time stamp feature for all CAN */
#define FEATURE_CAN_HAS_HR_TIMESTAMP (0U)
/* @brief The list contains definitions of the High-resolution time stamp feature support on all instances */
#define FEATURE_CAN_INSTANCES_HAS_HR_TIMESTAMP           { FEATURE_CAN0_HAS_HR_TIMESTAMP, \
                                                           FEATURE_CAN1_HAS_HR_TIMESTAMP }
/* pTMR module features */

/* @brief Clock names for pTMR */
#define pTMR_CLOCK_NAMES {SLOW_BUS_CLK}

/* @brief pTMR instance list */
typedef enum {
    pTMR0_INST = 0,
} ptmr_instance_t;

/* @brief pTMR channels list for each instance */
typedef enum {
    pTMR0_CH0 = 0U,
    pTMR0_CH1 = 1U,
    pTMR0_CH2 = 2U,
    pTMR0_CH3 = 3U,
} ptmr_channel_t;

/* TMR module features */
#define TMR_CLOCK_NAMES {SLOW_BUS_CLK}

/* lpTMR module features */

/* @brief lpTMR pulse counter input options */
#define FEATURE_LPTMR_HAS_INPUT_ALT1_SELECTION  (1U)
/* @brief Clock names for lpTMR. */
#define lpTMR_CLOCK_NAMES {lpTMR0_CLK }
/* @brief alt1 input for lpTMR. */
#define FEATURE_lpTMR_HAS_INPUT_ALT1_SELECTION  (1U)
/* @brief lpTMR ip clock selection */
#define FEATURE_lpTMR_HAS_CLOCK_SELECTION       (1U)
/* @brief lpTMR clock source support ipc */
#define FEATURE_lpTMR_CLKSRC_SUPPORT_IPC        (0U)
/* @brief lpTMR clock source support firc */
#define FEATURE_lpTMR_CLKSRC_SUPPORT_FIRC       (1U)
/* @brief lpTMR clock source support sxosc */
#define FEATURE_lpTMR_CLKSRC_SUPPORT_FXOSC      (1U)
/* @brief lpTMR clock source support lpo */
#define FEATURE_lpTMR_CLKSRC_SUPPORT_LPO        (1U)
/* @brief lpTMR clock source support sirc */
#define FEATURE_lpTMR_CLKSRC_SUPPORT_SIRC       (1U)

/* HCU module features */
#define FEATURE_HCU_AES_ENGINE                           (1U)
#define FEATURE_HCU_SM4_ENGINE                           (0U)
#define FEATURE_HCU_SHA_ENGINE                           (0U)
#define FEATURE_HCU_ONE_LOOP_DATA_SIZE                   (4U)
#define FEATURE_HCU_ONE_LOOP_INPUT_WATERMARK             (4U)
#define FEATURE_HCU_HAS_FIXED_DMA                        (1U)
#define FEATURE_HCU_HAS_SHA_AUTHORIZE                    (0U)
#define FEATURE_HCU_AES_CTR_ENGINE                       (0U)
#define FEATURE_HCU_AES_CCM_ENGINE                       (0U)
#define FEATURE_HCU_HMAC_ENGINE                          (0U)

/* @brief Reduce HCU area */
#define FEATURE_HCU_HAS_AREA_REDUCTION                   (1U)
/* @brief HCU key size fixed */
#define FEATURE_HCU_KEY_SIZE_UNFIXED                     (0U)

#define FEATURE_HCU_FIFO_SIZE_IN_WORDS                   (8U)

/* @brief EFM AES area base address */
#define FEATURE_EFM_AES_KEY_BASE_ADDR                    (0x10003800U)
/* @brief EFM AES key size (bytes)*/
#define FEATURE_HCU_GET_KEY_ADDR(key_id)                 (FEATURE_EFM_AES_KEY_BASE_ADDR + ((key_id) * FEATURE_EFM_AES_KEY_SIZE))


/* UART module features */

/* @brief One wire mode is supported. */
#define FEATURE_UART_HAS_ONE_WIRE_MODE_SUPPORT (1)
/* @brief Hardware flow control (RTS, CTS) is supported. */
#define FEATURE_UART_HAS_MODEM_SUPPORT (1)
/* @brief Baud rate oversampling is available. */
#define FEATURE_UART_HAS_BAUD_RATE_OVER_SAMPLING_SUPPORT (1)
/* @brief Baud rate oversampling is available. */
#define FEATURE_UART_HAS_BOTH_EDGE_SAMPLING_SUPPORT (1)
/* @brief Capacity (number of entries) of the transmit/receive FIFO (or zero if no FIFO is available). */
#define FEATURE_UART_FIFO_SIZE (4U)
/* @brief Supports two match addresses to filter incoming frames. */
#define FEATURE_UART_HAS_ADDRESS_MATCHING (1)
/* @brief Has transmitter/receiver DMA enable bits. */
#define FEATURE_UART_HAS_DMA_ENABLE (1)
/* @brief Flag clearance mask for STAT register. */
#define FEATURE_UART_INTF_REG_FLAGS_MASK (0x0000CEFFU)
/* @brief Flag clearance mask for FIFO register. */
#define FEATURE_UART_FIFO_REG_FLAGS_MASK (0x00030000U)
/* @brief Reset mask for FIFO register. */
#define FEATURE_UART_FIFO_RESET_MASK (0x00008000U)
/* @brief Default oversampling ratio. */
#define FEATURE_UART_DEFAULT_OSR (0x0FUL)
/* @brief Default baud rate divisor. */
#define FEATURE_UART_DEFAULT_DIV (0x04UL)
/* @brief Clock names for UART. */
#define UART_CLOCK_NAMES                \
    {                                   \
        UART0_CLK, UART1_CLK, UART2_CLK \
    }
/* @brief IPC index for UART module */
#define UART_IPC_INDEX                                      \
    {                                                       \
        IPC_UART0_INDEX, IPC_UART1_INDEX, IPC_UART2_INDEX   \
    }
/* @brief UART state ptr initialization */
#define FEATURE_UART_STATE_PTR  { NULL, NULL, NULL }

/* TRNG module features */

/* @brief Entropy Value count optimize */
#define FEATURE_TRNG_ENTROPY_COUNT_OPTIMIZE         (1U)

/* OSIF module features */

#define FEATURE_OSIF_USE_SYSTICK                         (1)
#define FEATURE_OSIF_FREERTOS_ISR_CONTEXT_METHOD         (1) /* Cortex M device */


/* PTU module features */

/* @brief PTU has instance back to back mode between PTU0 CH0 and PTU1 CH0 pre-triggers */
#define FEATURE_PTU_HAS_INSTANCE_BACKTOBACK     (1)

/* @brief PTU has inter-channel back to back mode between PTUx CH0 and PTUx CH1 pre-triggers */
#define FEATURE_PTU_HAS_INTERCHANNEL_BACKTOBACK (1)

/*! @brief EMU Global Enable KEY */
#define FEATURE_EMU_GLOBAL_ENABLE_KEY       (1U)


#if defined(USER_DEVICE_FEATURES_OVERRIDE)
#include "user_device_features_override.h"
#endif


#endif /* YTM32B1MD0_FEATURES_H */

/*******************************************************************************
 * EOF
 ******************************************************************************/
