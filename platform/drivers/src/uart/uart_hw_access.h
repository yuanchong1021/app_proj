/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file uart_hw_access.h
 * @version 1.4.0
 */

#ifndef UART_HW_ACCESS_H__
#define UART_HW_ACCESS_H__

#include "uart_driver.h"


/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define UART_SHIFT          (16U)
#define UART_BAUD_REG_ID    (1U)
#define UART_CTRL0_REG_ID   (2U)
#define UART_CTRL1_REG_ID   (3U)
#define UART_STATUTS_REG_ID (4U)
#define UART_INTE_REG_ID      (5U)
#define UART_MATCH_REG_ID   (6U)
#define UART_DATA_REG_ID    (7U)
#define UART_TXFIFO_REG_ID  (8U)
#define UART_RXFIFO_REG_ID  (9U)

/*! @brief UART wakeup from standby method constants
 */
typedef enum
{
    UART_IDLE_LINE_WAKE = 0x0U, /*!< Idle-line wakes the UART receiver from standby. */
    UART_ADDR_MARK_WAKE = 0x1U  /*!< Addr-mark wakes UART receiver from standby.*/
} uart_wakeup_method_t;

/*!
 * @brief UART break character length settings for transmit/detect.
 *
 * The actual maximum bit times may vary depending on the UART instance.
 */
typedef enum
{
    UART_BREAK_CHAR_10_BIT_MINIMUM = 0x0U, /*!< UART break char length 10 bit times (if M = 0, SBNS = 0)
                                                  or 11 (if M = 1, SBNS = 0 or M = 0, SBNS = 1) or 12 (if M = 1,
                                                  SBNS = 1 or M10 = 1, SNBS = 0) or 13 (if M10 = 1, SNBS = 1) */
    UART_BREAK_CHAR_13_BIT_MINIMUM = 0x1U  /*!< UART break char length 13 bit times (if M = 0, SBNS = 0
                                                  or M10 = 0, SBNS = 1) or 14 (if M = 1, SBNS = 0 or M = 1,
                                                  SBNS = 1) or 15 (if M10 = 1, SBNS = 1 or M10 = 1, SNBS = 0) */
} uart_break_char_length_t;

/*!
 * @brief UART status flags.
 *
 * This provides constants for the UART status flags for use in the UART functions.
 */
typedef enum
{
    UART_TX_DATA_REG_EMPTY          = UART_INTF_TXIF_MASK,       /*!< Tx data register empty flag, sets when Tx buffer is empty */
    UART_TX_COMPLETE                = UART_INTF_TCIF_MASK,       /*!< Transmission complete flag, sets when transmission activity complete */
    UART_RX_DATA_REG_FULL           = UART_INTF_RXIF_MASK,       /*!< Rx data register full flag, sets when the receive data buffer is full */
    UART_IDLE_LINE_DETECT           = UART_INTF_IDLEIF_MASK,     /*!< Idle line detect flag, sets when idle line detected */
    UART_RX_OVERRUN                 = UART_INTF_OVRIF_MASK,      /*!< Rx Overrun sets if new data is received before data is read */
    UART_NOISE_DETECT               = UART_INTF_NOZIF_MASK,      /*!< Rx takes 3 samples of each received bit. If these differ, the flag sets */
    UART_FRAME_ERR                  = UART_INTF_FEIF_MASK,       /*!< Frame error flag, sets if logic 0 was detected where stop bit expected */
    UART_PARITY_ERR                 = UART_INTF_PARIF_MASK,      /*!< If parity enabled, sets upon parity error detection */
    UART_LIN_BREAK_DETECT           = UART_INTF_LBKDIF_MASK,     /*!< LIN break detect interrupt flag, sets when LIN break char detected */
    UART_RX_ACTIVE_EDGE_DETECT      = UART_INTF_RXEDGEIF_MASK,   /*!< Rx pin active edge interrupt flag, sets when active edge detected */
#if FEATURE_UART_HAS_ADDRESS_MATCHING
    UART_MATCH_ADDR_ONE             = UART_INTF_MATIF_MASK,      /*!< Address one match flag */
#endif
#if FEATURE_UART_FIFO_SIZE > 0U
    UART_FIFO_TX_OF                 = UART_INTF_TFEIF_MASK,      /*!< Transmitter FIFO buffer overflow */
    UART_FIFO_RX_UF                 = UART_INTF_RFEIF_MASK,      /*!< Receiver FIFO buffer underflow */
#endif
} uart_status_flag_t;

/*! @brief UART interrupt configuration structure, default settings are 0 (disabled) */
typedef enum
{
    UART_INT_LIN_BREAK_DETECT  = UART_INTE_LBKDIE_MASK,               /*!< LIN break detect. */
    UART_INT_RX_ACTIVE_EDGE    = UART_INTE_RXEDGEIE_MASK,             /*!< RX Active Edge. */
    UART_INT_TX_DATA_REG_EMPTY = UART_INTE_TXIE_MASK,                 /*!< Transmit data register empty. */
    UART_INT_TX_COMPLETE       = UART_INTE_TCIE_MASK,                 /*!< Transmission complete. */
    UART_INT_RX_DATA_REG_FULL  = UART_INTE_RXIE_MASK,                 /*!< Receiver data register full. */
    UART_INT_IDLE_LINE         = UART_INTE_IDLEIE_MASK,               /*!< Idle line. */
    UART_INT_RX_OVERRUN        = UART_INTE_OVRIE_MASK,                /*!< Receiver Overrun. */
    UART_INT_NOISE_ERR_FLAG    = UART_INTE_NOZIE_MASK,                /*!< Noise error flag. */
    UART_INT_FRAME_ERR_FLAG    = UART_INTE_FEIE_MASK,                 /*!< Framing error flag. */
    UART_INT_PARITY_ERR_FLAG   = UART_INTE_PARIE_MASK,                /*!< Parity error flag. */
#if FEATURE_UART_HAS_ADDRESS_MATCHING
    UART_INT_MATCH_ADDR_ONE    = UART_INTE_MATIE_MASK,                /*!< Match address one flag. */
#endif
} uart_interrupt_t;


/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name UART Common Configurations
 * @{
 */

/*!
 * @brief Initializes the UART controller.
 *
 * This function Initializes the UART controller to known state.
 *
 *
 * @param base UART base pointer.
 */
void UART_Init(UART_Type * base);

/*!
 * @brief Enable/Disable the UART transmitter.
 *
 * This function enables or disables the UART transmitter, based on the
 * parameter received.
 *
 *
 * @param base UART base pointer.
 * @param enable Enable(true) or disable(false) transmitter.
 */
static inline void UART_SetTransmitterCmd(UART_Type * base, bool enable)
{
    SDK_ENTER_CRITICAL();
    base->CTRL0 = (base->CTRL0 & ~UART_CTRL0_TXEN_MASK) | ((enable ? 1UL : 0UL) << UART_CTRL0_TXEN_SHIFT);
    /* Wait for the register write operation to complete */
    while((bool)((base->CTRL0 & UART_CTRL0_TXEN_MASK) != 0U) != enable) {}
    SDK_EXIT_CRITICAL();
}

/*!
 * @brief Enable/Disable the UART receiver.
 *
 * This function enables or disables the UART receiver, based on the
 * parameter received.
 *
 *
 * @param base UART base pointer
 * @param enable Enable(true) or disable(false) receiver.
 */
static inline void UART_SetReceiverCmd(UART_Type * base, bool enable)
{
    SDK_ENTER_CRITICAL();
    base->CTRL0 = (base->CTRL0 & ~UART_CTRL0_RXEN_MASK) | ((enable ? 1UL : 0UL) << UART_CTRL0_RXEN_SHIFT);
    /* Wait for the register write operation to complete */
    while((bool)((base->CTRL0 & UART_CTRL0_RXEN_MASK) != 0U) != enable) {}
    SDK_EXIT_CRITICAL();
}

/*!
 * @brief Sets the UART baud rate modulo divisor.
 *
 * This function sets the UART baud rate modulo divisor.
 *
 *
 * @param base UART base pointer.
 * @param baudRateDivisor The baud rate modulo division "SBR"
 */
static inline void UART_SetBaudRateDivisor(UART_Type * base, uint32_t baudRateDivisor)
{
    DEV_ASSERT((baudRateDivisor <= 0x1FFFU) && (baudRateDivisor >= 1U));
    uint32_t baudRegValTemp;

    baudRegValTemp = base->BAUD;
    baudRegValTemp &= ~(UART_BAUD_DIV_MASK);
    /* Removed the shift operation as the SBR field position is zero; shifting with 0 violates MISRA */
    baudRegValTemp |= baudRateDivisor & UART_BAUD_DIV_MASK;
    base->BAUD = baudRegValTemp;
}

/*!
 * @brief Gets the UART baud rate modulo divisor.
 *
 * This function gets the UART baud rate modulo divisor.
 *
 *
 * @param base UART base pointer.
 * @return The baud rate modulo division "SBR"
 */
static inline uint16_t UART_GetBaudRateDivisor(const UART_Type * base)
{
    return ((uint16_t)((base->BAUD & UART_BAUD_DIV_MASK) >> UART_BAUD_DIV_SHIFT));
}

#if FEATURE_UART_HAS_BAUD_RATE_OVER_SAMPLING_SUPPORT
/*!
 * @brief Sets the UART baud rate oversampling ratio
 *
 * This function sets the UART baud rate oversampling ratio.
 * (Note: Feature available on select UART instances used together with baud rate programming)
 * The oversampling ratio should be set between 4x (00011) and 32x (11111). Writing
 * an invalid oversampling ratio results in an error and is set to a default
 * 16x (01111) oversampling ratio.
 * Disable the transmitter/receiver before calling this function.
 *
 *
 * @param base UART base pointer.
 * @param overSamplingRatio The oversampling ratio "OSR"
 */
static inline void UART_SetOversamplingRatio(UART_Type * base, uint32_t overSamplingRatio)
{
    DEV_ASSERT(overSamplingRatio <= 0x1FU);
    uint32_t baudRegValTemp;

    baudRegValTemp = base->BAUD;
    baudRegValTemp &= ~(UART_BAUD_OSRVAL_MASK);
    baudRegValTemp |= UART_BAUD_OSRVAL(overSamplingRatio);
    base->BAUD = baudRegValTemp;
}

/*!
 * @brief Gets the UART baud rate oversampling ratio
 *
 * This function gets the UART baud rate oversampling ratio.
 * (Note: Feature available on select UART instances used together with baud rate programming)
 *
 *
 * @param base UART base pointer.
 * @return The oversampling ratio "OSR"
 */
static inline uint8_t UART_GetOversamplingRatio(const UART_Type * base)
{
    return ((uint8_t)((base->BAUD & UART_BAUD_OSRVAL_MASK) >> UART_BAUD_OSRVAL_SHIFT));
}
#endif

#if FEATURE_UART_HAS_BOTH_EDGE_SAMPLING_SUPPORT
/*!
 * @brief Configures the UART baud rate both edge sampling
 *
 * This function configures the UART baud rate both edge sampling.
 * (Note: Feature available on select UART instances used with baud rate programming)
 * When enabled, the received data is sampled on both edges of the baud rate clock.
 * This must be set when the oversampling ratio is between 4x and 7x.
 * This function should only be called when the receiver is disabled.
 *
 *
 * @param base UART base pointer.
 * @param enable   Enable (1) or Disable (0) Both Edge Sampling
 */
static inline void UART_EnableBothEdgeSamplingCmd(UART_Type * base)
{
    base->CTRL1 |= UART_CTRL1_BOTHEDGE_MASK;
}
#endif

/*!
 * @brief Configures the number of bits per character in the UART controller.
 *
 * This function configures the number of bits per character in the UART controller.
 * In some UART instances, the user should disable the transmitter/receiver
 * before calling this function.
 * Generally, this may be applied to all UARTs to ensure safe operation.
 *
 * @param base UART base pointer.
 * @param bitCountPerChar  Number of bits per char (8, 9, or 10, depending on the UART instance)
 * @param parity  Specifies whether parity bit is enabled
 */
void UART_SetBitCountPerChar(UART_Type * base, uart_bit_count_per_char_t bitCountPerChar, bool parity);

/*!
 * @brief Configures parity mode in the UART controller.
 *
 * This function configures parity mode in the UART controller.
 * In some UART instances, the user should disable the transmitter/receiver
 * before calling this function.
 * Generally, this may be applied to all UARTs to ensure safe operation.
 *
 * @param base UART base pointer.
 * @param parityModeType  Parity mode (enabled, disable, odd, even - see parity_mode_t struct)
 */
void UART_SetParityMode(UART_Type * base, uart_parity_mode_t parityModeType);

/*!
 * @brief Configures the number of stop bits in the UART controller.
 *
 * This function configures the number of stop bits in the UART controller.
 * In some UART instances, the user should disable the transmitter/receiver
 * before calling this function.
 * Generally, this may be applied to all UARTs to ensure safe operation.
 *
 * @param base UART base pointer.
 * @param stopBitCount Number of stop bits (1 or 2 - see uart_stop_bit_count_t struct)
 */
static inline void UART_SetStopBitCount(UART_Type * base, uart_stop_bit_count_t stopBitCount)
{
    base->CTRL1 = (base->CTRL1 & ~UART_CTRL1_STOPSZ_MASK) | ((uint32_t)stopBitCount << UART_CTRL1_STOPSZ_SHIFT);
}

/*@}*/

/*!
 * @name UART Interrupts and DMA
 * @{
 */

/*!
 * @brief Configures the UART module interrupts.
 *
 * This function configures the UART module interrupts to enable/disable various interrupt sources.
 *
 *
 * @param   base UART module base pointer.
 * @param   intSrc UART interrupt configuration data.
 * @param   enable   true: enable, false: disable.
 */
void UART_SetIntMode(UART_Type * base, uart_interrupt_t intSrc, bool enable);

/*!
 * @brief Returns UART module interrupts state.
 *
 * This function returns whether a certain UART module interrupt is enabled or disabled.
 *
 *
 * @param   base UART module base pointer.
 * @param   intSrc UART interrupt configuration data.
 * @return  true: enable, false: disable.
 */
bool UART_GetIntMode(const UART_Type * base, uart_interrupt_t intSrc);

#if FEATURE_UART_HAS_DMA_ENABLE
/*!
 * @brief Configures DMA requests.
 *
 * This function configures DMA requests for UART Transmitter.
 *
 *
 * @param base UART base pointer
 * @param enable Transmit DMA request configuration (enable:1 /disable: 0)
 */
static inline void UART_SetTxDmaCmd(UART_Type * base, bool enable)
{
    uint32_t tempReg;
    SDK_ENTER_CRITICAL();
    tempReg = base->CTRL0;
    tempReg &= ~UART_CTRL0_TXDMAEN_MASK;
    tempReg |= UART_CTRL0_TXDMAEN(enable ? 1U : 0U);
    base->CTRL0 = tempReg;
    SDK_EXIT_CRITICAL();
}

/*!
 * @brief Configures DMA requests.
 *
 * This function configures DMA requests for UART Receiver.
 *
 *
 * @param base UART base pointer
 * @param enable Receive DMA request configuration (enable: 1/disable: 0)
 */
static inline void UART_SetRxDmaCmd(UART_Type * base, bool enable)
{
    uint32_t tempReg;
    SDK_ENTER_CRITICAL();
    tempReg = base->CTRL0;
    tempReg &= ~UART_CTRL0_RXDMAEN_MASK;
    tempReg |= UART_CTRL0_RXDMAEN(enable ? 1U : 0U);
    base->CTRL0 = tempReg;
    SDK_EXIT_CRITICAL();
}
#endif

/*@}*/

/*!
 * @name UART Transfer Functions
 * @{
 */

/*!
 * @brief Sends the UART 8-bit character.
 *
 * This functions sends an 8-bit character.
 *
 *
 * @param base UART Instance
 * @param data     data to send (8-bit)
 */
static inline void UART_Putchar(UART_Type * base, uint8_t data)
{
    base->DATA = data;
}

/*!
 * @brief Sends the UART 9-bit character.
 *
 * This functions sends a 9-bit character.
 *
 *
 * @param base UART Instance
 * @param data     data to send (9-bit)
 */
void UART_Putchar9(UART_Type * base, uint16_t data);

/*!
 * @brief Sends the UART 10-bit character (Note: Feature available on select UART instances).
 *
 * This functions sends a 10-bit character.
 *
 *
 * @param base UART Instance
 * @param data   data to send (10-bit)
 */
void UART_Putchar10(UART_Type * base, uint16_t data);

/*!
 * @brief Gets the UART 7-bit character.
 *
 * This functions receives an 7-bit character.
 *
 *
 * @param base UART base pointer
 * @param readData Data read from receive (7-bit)
 */
void UART_Getchar7(const UART_Type * base, uint8_t *readData);

/*!
 * @brief Gets the UART 8-bit character.
 *
 * This functions receives an 8-bit character.
 *
 *
 * @param base UART base pointer
 * @param readData Data read from receive (8-bit)
 */
void UART_Getchar8(const UART_Type * base, uint8_t *readData);

/*!
 * @brief Gets the UART 9-bit character.
 *
 * This functions receives a 9-bit character.
 *
 *
 * @param base UART base pointer
 * @param readData Data read from receive (9-bit)
 */
void UART_Getchar9(const UART_Type * base, uint16_t *readData);

/*!
 * @brief Gets the UART 10-bit character.
 *
 * This functions receives a 10-bit character.
 *
 *
 * @param base UART base pointer
 * @param readData Data read from receive (10-bit)
 */
void UART_Getchar10(const UART_Type * base, uint16_t *readData);

/*@}*/

/*!
 * @name UART Status Flags
 * @{
 */

/*!
 * @brief  UART get status flag
 *
 * This function returns the state of a status flag.
 *
 *
 * @param base UART base pointer
 * @param statusFlag  The status flag to query
 * @return Whether the current status flag is set(true) or not(false).
 */
bool UART_GetStatusFlag(const UART_Type * base, uart_status_flag_t statusFlag);

/*!
 * @brief UART clears an individual status flag.
 *
 * This function clears an individual status flag (see uart_status_flag_t for list of status bits).
 *
 *
 * @param base UART base pointer
 * @param statusFlag  Desired UART status flag to clear
 * @return STATUS_SUCCESS if successful or STATUS_ERROR if an error occured
 */
status_t UART_ClearStatusFlag(UART_Type * base, uart_status_flag_t statusFlag);

/*@}*/

/*!
 * @name UART Special Feature Configurations
 * @{
 */

/*!
 * @brief Enable or disable the UART error interrupts.
 *
 * This function configures the error interrupts (parity, noise, overrun, framing).
 *
 *
 * @param base UART base pointer
 * @param enable true - enable, false - disable error interrupts
 */
void UART_SetErrorInterrupts(UART_Type * base, bool enable);

/*!
 * @brief  Clears the error flags treated by the driver
 *
 * This function clears the error flags treated by the driver.
 * *
 * @param base UART base pointer
 */
static inline void UART_DRV_ClearErrorFlags(UART_Type * base)
{
    uint32_t mask = UART_INTF_OVRIF_MASK | \
                    UART_INTF_NOZIF_MASK | \
                    UART_INTF_FEIF_MASK  | \
                    UART_INTF_IDLEIF_MASK| \
                    UART_INTF_PARIF_MASK;

    base->INTF = (base->INTF & (~FEATURE_UART_INTF_REG_FLAGS_MASK)) | mask;
}

/*!
 * @brief  Set line idle detect size
 *
 * This function sets line idle detect size
 * *
 * @param base UART base pointer
 * @param size Idle size
 */
static inline void UART_SetLineIdleSize(UART_Type * base, uint8_t size)
{
    base->CTRL1 &= ~(UART_CTRL1_IDLESZ_MASK);
    base->CTRL1 |= UART_CTRL1_IDLESZ(size);
}
#if !defined(FEATURE_UART_LITE_VERSION)
/*!
 * @brief  Get tx fifo size
 *
 * This function gets tx fifo size
 * *
 * @param base UART base pointer
 * @return Tx fifo size
 */
static inline uint32_t UART_GetTxFifoSize(UART_Type * base)
{
    return ((uint32_t)0x01U << ((base->TXFIFO & UART_TXFIFO_SIZE_MASK) >> UART_TXFIFO_SIZE_SHIFT));
}

/*!
 * @brief  Get rx fifo size
 *
 * This function gets rx fifo size
 * *
 * @param base UART base pointer
 * @return Rx fifo size
 */
static inline uint32_t UART_GetRxFifoSize(UART_Type * base)
{
    return ((uint32_t)0x01U << ((base->RXFIFO & UART_RXFIFO_SIZE_MASK) >> UART_RXFIFO_SIZE_SHIFT));
}

/*!
 * @brief  Reset tx fifo
 *
 * This function resets tx fifo
 * *
 * @param base UART base pointer
 */
static inline void UART_ResetTxFifo(UART_Type * base)
{
    base->TXFIFO = UART_TXFIFO_RESET_MASK;
}

/*!
 * @brief  Reset rx fifo
 *
 * This function resets rx fifo
 * *
 * @param base UART base pointer
 */
static inline void UART_ResetRxFifo(UART_Type * base)
{
    base->RXFIFO = UART_RXFIFO_RESET_MASK;
}

/*!
 * @brief  Enable tx fifo
 *
 * This function Enable tx fifo
 * *
 * @param base UART base pointer
 * @param isEnable Enable or Disable tx fifo
 */
static inline void UART_EnableTxFifo(UART_Type * base, bool isEnable)
{
    if(isEnable)
    {
        base->TXFIFO |= UART_TXFIFO_EN(1);
    }
    else 
    {
        base->TXFIFO &= ~UART_TXFIFO_EN_MASK;
    }
}

/*!
 * @brief  Enable rx fifo
 *
 * This function Enable rx fifo
 * *
 * @param base UART base pointer
 * @param isEnable Enable or Disable rx fifo
 */
static inline void UART_EnableRxFifo(UART_Type * base, bool isEnable)
{
    if(isEnable)
    {
        base->RXFIFO |= UART_RXFIFO_EN(1);
    }
    else 
    {
        base->RXFIFO &= ~UART_RXFIFO_EN_MASK;
    }
}

/*!
 * @brief  Get word count in tx fifo
 *
 * This function gets word count in tx fifo
 * *
 * @param base UART base pointer
 * @return Word count in tx fifo
 */
static inline uint32_t UART_GetWordCountInTxFifo(UART_Type * base)
{
    return ((base->TXFIFO & UART_TXFIFO_COUNT_MASK) >> UART_TXFIFO_COUNT_SHIFT);
}

/*!
 * @brief  Get word count in rx fifo
 *
 * This function gets word count in rx fifo
 * *
 * @param base UART base pointer
 * @return Word count in rx fifo
 */
static inline uint32_t UART_GetWordCountInRxFifo(UART_Type * base)
{
    return ((base->RXFIFO & UART_RXFIFO_COUNT_MASK) >> UART_RXFIFO_COUNT_SHIFT);
}

/*!
 * @brief  Get Set tx fifo watermark
 *
 * This function gets tx fifo watermark
 * *
 * @param base UART base pointer
 * @return Tx fifo watermark
 */
static inline uint32_t UART_GetTxFifoWatermark(UART_Type * base)
{
    return ((base->TXFIFO & UART_TXFIFO_WATER_MASK) >> UART_TXFIFO_WATER_SHIFT);
}

/*!
 * @brief  Get Set rx fifo watermark
 *
 * This function gets rx fifo watermark
 * *
 * @param base UART base pointer
 * @return Rx fifo watermark
 */
static inline uint32_t UART_GetRxFifoWatermark(UART_Type * base)
{
    return ((base->RXFIFO & UART_RXFIFO_WATER_MASK) >> UART_RXFIFO_WATER_SHIFT);
}

/*!
 * @brief  Set tx fifo watermark
 *
 * This function sets tx fifo watermark
 * *
 * @param base UART base pointer
 */
static inline void UART_SetTxFifoWatermark(UART_Type * base, uint32_t watermarkValue)
{
    base->TXFIFO &= ~UART_TXFIFO_WATER_MASK;
    base->TXFIFO |= UART_TXFIFO_WATER(watermarkValue);
}

/*!
 * @brief  Set rx fifo watermark
 *
 * This function sets rx fifo watermark
 * *
 * @param base UART base pointer
 */
static inline void UART_SetRxFifoWatermark(UART_Type * base, uint32_t watermarkValue)
{
    base->RXFIFO &= ~UART_RXFIFO_WATER_MASK;
    base->RXFIFO |= UART_RXFIFO_WATER(watermarkValue);
}
#endif /* !defined(FEATURE_UART_LITE_VERSION) */

#if FEATURE_UART_HAS_MODEM_SUPPORT
/*!
 * @brief  Enable/Disable tx cts
 *
 * This function enables/disables tx cts
 * *
 * @param base UART base pointer
 * @param enable Enable/Disable tx cts
 */
static inline void UART_EnableTxCts(UART_Type * base, bool enable)
{
    SDK_ENTER_CRITICAL();
    base->CTRL0 &= ~UART_CTRL0_TXCTSE_MASK;
    base->CTRL0 |= UART_CTRL0_TXCTSE(enable ? 1U : 0U);
    SDK_EXIT_CRITICAL();
}

/*!
 * @brief  Set tx cts source
 *
 * This function sets tx cts source
 * *
 * @param base UART base pointer
 * @param source Select tx cts source
 */
static inline void UART_SetTxCtsSource(UART_Type * base, uart_tx_cts_source_t source)
{
    SDK_ENTER_CRITICAL();
    base->CTRL0 &= ~UART_CTRL0_TXCTSRC_MASK;
    base->CTRL0 |= UART_CTRL0_TXCTSRC(source);
    SDK_EXIT_CRITICAL();
}

/*!
 * @brief  Set tx cts detection time
 *
 * This function sets tx cts detection time
 * *
 * @param base UART base pointer
 * @param source Select tx cts detection time
 */
static inline void UART_SetTxCtsDetectionTime(UART_Type * base, uart_tx_cts_detection_time_t tx_cts_det_time)
{
    SDK_ENTER_CRITICAL();
    base->CTRL0 &= ~UART_CTRL0_TXCTSC_MASK;
    base->CTRL0 |= UART_CTRL0_TXCTSC(tx_cts_det_time);
    SDK_EXIT_CRITICAL();
}

/*!
 * @brief  Enable/Disable rx rts
 *
 * This function enables/disables rx rts
 * *
 * @param base UART base pointer
 * @param enable Enable/Disable rx rts
 */
static inline void UART_EnableRxRts(UART_Type * base, bool enable)
{
    SDK_ENTER_CRITICAL();
    base->CTRL0 &= ~UART_CTRL0_RXRTSE_MASK;
    base->CTRL0 |= UART_CTRL0_RXRTSE(enable ? 1U : 0U);
    SDK_EXIT_CRITICAL();
}

/*!
 * @brief  check if rx rts is enable
 *
 * This function checks Whether rx rts is enable
 * *
 * @param base UART base pointer
 * @return Whether rx rts is enable
 */
static inline bool UART_IsEnableRxRts(UART_Type * base)
{
    return (((base->CTRL0 & UART_CTRL0_RXRTSE_MASK) >> UART_CTRL0_RXRTSE_SHIFT) != 0U);
}

/*!
 * @brief  Set rx rts watermark
 *
 * This function sets rx rts watermark
 * *
 * @param base UART base pointer
 * @param enable rx rts watermark
 */
static inline void UART_SetRxRtsWatermark(UART_Type * base, uart_rx_rts_watermark_t watermark)
{
    SDK_ENTER_CRITICAL();
    base->CTRL0 &= ~UART_CTRL0_RTSWATER_MASK;
    base->CTRL0 |= UART_CTRL0_RTSWATER(watermark);
    SDK_EXIT_CRITICAL();
}

/*!
 * @brief  Enable/Disable tx rts
 *
 * This function enables/disables tx rts
 * *
 * @param base UART base pointer
 * @param enable Enable/Disable tx rts
 */
static inline void UART_EnableTxRts(UART_Type * base, bool enable)
{
    SDK_ENTER_CRITICAL();
    base->CTRL0 &= ~UART_CTRL0_TXRTSE_MASK;
    base->CTRL0 |= UART_CTRL0_TXRTSE(enable ? 1U : 0U);
    SDK_EXIT_CRITICAL();
}

/*!
 * @brief  Check if tx rts is enable
 *
 * This function checks Whether tx rts is enable
 * *
 * @param base UART base pointer
 * @return Whether tx rts is enable
 */
static inline bool UART_IsEnableTxRts(UART_Type * base)
{
    return (((base->CTRL0 & UART_CTRL0_TXRTSE_MASK) >> UART_CTRL0_TXRTSE_SHIFT) != 0U);
}

/*!
 * @brief  Set tx rts polarity
 *
 * This function sets tx rts polarity
 * *
 * @param base UART base pointer
 * @param polarity tx rts polarity
 */
static inline void UART_SetTxRtsPolarity(UART_Type * base, uart_tx_rts_polarity_t polarity)
{
    SDK_ENTER_CRITICAL();
    base->CTRL0 &= ~UART_CTRL0_TXRTXPOL_MASK;
    base->CTRL0 |= UART_CTRL0_TXRTXPOL(polarity);
    SDK_EXIT_CRITICAL();
}
#endif

#if defined(FEATURE_UART_HAS_ONE_WIRE_MODE_SUPPORT) && (FEATURE_UART_HAS_ONE_WIRE_MODE_SUPPORT == 1U) 
/*!
 * @brief  Enable/Disable loop mode
 *
 * This function enables/disables loop mode
 * *
 * @param base UART base pointer
 * @param enable Enable/Disable loop mode
 */
static inline void UART_EnableLoopMode(UART_Type * base, bool enable)
{
    base->CTRL1 &= ~UART_CTRL1_LOOP_MASK;
    base->CTRL1 |= UART_CTRL1_LOOP(enable ? 1U : 0U);
}

/*!
 * @brief  Enable/Disable one wire mode
 *
 * This function enables/disables one wire mode
 * *
 * @param base UART base pointer
 * @param enable Enable/Disable one wire mode
 */
static inline void UART_EnableOneWireMode(UART_Type * base, bool enable)
{
    base->CTRL1 &= ~UART_CTRL1_ONEWIRE_MASK;
    base->CTRL1 |= UART_CTRL1_ONEWIRE(enable ? 1U : 0U);
}

/*!
 * @brief  Set tx line direction in one wire mode
 *
 * This function sets tx line direction in one wire mode
 * *
 * @param base UART base pointer
 * @param direction tx line direction in one wire mode
 */
static inline void UART_SetTxLineDirection(UART_Type * base, uart_tx_line_derection_t direction)
{
    base->CTRL1 &= ~UART_CTRL1_TXDIR_MASK;
    base->CTRL1 |= UART_CTRL1_TXDIR(direction);
}
#endif
/*@}*/

#if defined(__cplusplus)
}
#endif

#endif /* UART_HW_ACCESS_H__ */
/*******************************************************************************
 * EOF
 ******************************************************************************/
