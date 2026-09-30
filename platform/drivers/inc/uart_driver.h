/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file uart_driver.h
 * @version 1.4.0
 */

#ifndef UART_DRIVER_H__
#define UART_DRIVER_H__

#include <stddef.h>
#include "device_registers.h"
#include "status.h"
#include "interrupt_manager.h"
#include "osif.h"

#if FEATURE_UART_HAS_DMA_ENABLE
#include "dma_driver.h"
#endif

#include "callbacks.h"

/*!
 * @addtogroup uart_driver
 * @{
 */


/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief Type of UART transfer (based on interrupts or DMA).
 *
 * Implements : uart_transfer_type_t_Class
 */
typedef enum
{
    UART_USING_DMA = 0,    /*!< The driver will use DMA to perform UART transfer */
    UART_USING_INTERRUPTS          /*!< The driver will use interrupts to perform UART transfer */
} uart_transfer_type_t;

/*! @brief UART number of bits in a character
 *
 * Implements : uart_bit_count_per_char_t_Class
 */
typedef enum
{
    UART_7_BITS_PER_CHAR = 0x0U, /*!< 7-bit data characters */
    UART_8_BITS_PER_CHAR = 0x1U, /*!< 8-bit data characters */
    UART_9_BITS_PER_CHAR = 0x2U, /*!< 9-bit data characters */
    UART_10_BITS_PER_CHAR = 0x3U  /*!< 10-bit data characters */
} uart_bit_count_per_char_t;

/*! @brief UART line idle detect size
 *
 * Implements : uart_line_idle_size_t_Class
 */
typedef enum
{
    UART_LINE_IDLE_SIZE_1_CHAR = 0x0U, /*!< idles after 1 char length */
    UART_LINE_IDLE_SIZE_2_CHAR = 0x1U, /*!< idles after 2 char length */
    UART_LINE_IDLE_SIZE_4_CHAR = 0x2U, /*!< idles after 4 char length */
    UART_LINE_IDLE_SIZE_8_CHAR = 0x3U, /*!< idles after 8 char length */
    UART_LINE_IDLE_SIZE_16_CHAR = 0x4U, /*!< idles after 16 char length */
    UART_LINE_IDLE_SIZE_32_CHAR = 0x5U, /*!< idles after 32 char length */
    UART_LINE_IDLE_SIZE_64_CHAR = 0x6U, /*!< idles after 64 char length */
    UART_LINE_IDLE_SIZE_128_CHAR = 0x7U, /*!< idles after 128 char length */
} uart_line_size_t;

/*! @brief UART parity mode
 *
 * Implements : uart_parity_mode_t_Class
 */
typedef enum
{
    UART_PARITY_DISABLED = 0x0U, /*!< parity disabled */
    UART_PARITY_EVEN = 0x2U, /*!< parity enabled, type even, bit setting: PE|PT = 10 */
    UART_PARITY_ODD = 0x3U  /*!< parity enabled, type odd,  bit setting: PE|PT = 11 */
} uart_parity_mode_t;

/*! @brief UART number of stop bits
 *
 * Implements : uart_stop_bit_count_t_Class
 */
typedef enum
{
    UART_ONE_STOP_BIT = 0x0U, /*!< one stop bit */
    UART_TWO_STOP_BIT = 0x1U  /*!< two stop bits */
} uart_stop_bit_count_t;

#if FEATURE_UART_HAS_MODEM_SUPPORT
/*! @brief UART transmit CTS source
 *
 * Implements : uart_tx_cts_source_t_Class
 */
typedef enum
{
    UART_TX_CTS_SOURCE_FROM_PIN           = 0x0U, /*!< cts source from pin */
    UART_TX_CTS_SOURCE_FROM_ADDRESS_MATCH = 0x1U  /*!< cts source from match result */
} uart_tx_cts_source_t;

/*! @brief UART transmit CTS detection time
 *
 * Implements : uart_tx_cts_detection_time_t_Class
 */
typedef enum
{
    UART_TX_CTS_DETECTION_TIME_START_BIT          = 0x0U, /*!< cts detection time at the start of each character */
    UART_TX_CTS_DETECTION_TIME_TRANSMITTER_IDLE   = 0x1U  /*!< cts detection time when the transmitter is idle */
} uart_tx_cts_detection_time_t;

/*! @brief UART receive RTS watermark
 *
 * Implements : uart_rx_rts_watermark_t_Class
 */
typedef enum
{
    UART_RX_RTS_WATERMARK_0 = 0x0U, /*!< rts watermark = 0 */
    UART_RX_RTS_WATERMARK_1 = 0x1U, /*!< rts watermark = 1 */
    UART_RX_RTS_WATERMARK_2 = 0x2U, /*!< rts watermark = 2 */
    UART_RX_RTS_WATERMARK_3 = 0x3U, /*!< rts watermark = 3 */
    UART_RX_RTS_WATERMARK_4 = 0x4U, /*!< rts watermark = 4 */
} uart_rx_rts_watermark_t;

/*! @brief UART transmit RTS polarity
 *
 * Implements : uart_tx_rts_polarity_t_Class
 */
typedef enum
{
    UART_TX_RTS_POLARITY_LOW = 0x0U, /*!< tx rts polarity low */
    UART_TX_RTS_POLARITY_HIGH = 0x1U, /*!< tx rts polarity high */
}uart_tx_rts_polarity_t;
#endif

#if defined(FEATURE_UART_HAS_ONE_WIRE_MODE_SUPPORT) && (FEATURE_UART_HAS_ONE_WIRE_MODE_SUPPORT == 1U) 
/*! @brief UART tx line direction in one wire
 *
 * Implements : uart_tx_line_derection_t_Class
 */
typedef enum
{
    UART_TX_LINE_DIRECTION_INPUT = 0x0U,  /*!< Tx line direction is input in one wire mode */
    UART_TX_LINE_DIRECTION_OUTPUT = 0x1U, /*!< Tx line direction is output in one wire mode */
}uart_tx_line_derection_t;
#endif

/*!
 * @brief Runtime state of the UART driver.
 *
 * Note that the caller provides memory for the driver state structures during
 * initialization because the driver does not statically allocate memory.
 *
 * Implements : uart_state_t_Class
 */
typedef struct
{
    const uint8_t *txBuff;              /*!< The buffer of data being sent.*/
    uint8_t *rxBuff;                    /*!< The buffer of received data.*/
    volatile uint32_t txSize;            /*!< The remaining number of bytes to be transmitted. */
    volatile uint32_t rxSize;            /*!< The remaining number of bytes to be received. */
    volatile bool isTxBusy;              /*!< True if there is an active transmit.*/
    volatile bool isRxBusy;              /*!< True if there is an active receive.*/
    volatile bool isTxBlocking;          /*!< True if transmit is blocking transaction. */
    volatile bool isRxBlocking;          /*!< True if receive is blocking transaction. */
    uart_bit_count_per_char_t bitCountPerChar; /*!< number of bits in a char (8/9/10) */
    bool idleErrorIntEnable;                    /*!< enable/disable idle interrupt. */
    uart_callback_t rxCallback;          /*!< Callback to invoke for data receive
                                              Note: when the transmission is interrupt based, the callback
                                              is being called upon receiving a byte; when DMA transmission
                                              is used, the bytes are copied to the rx buffer by the DMA engine
                                              and the callback is called when all the bytes have been transferred. */
    void *rxCallbackParam;              /*!< Receive callback parameter pointer.*/
    uart_callback_t txCallback;          /*!< Callback to invoke for data send
                                              Note: when the transmission is interrupt based, the callback
                                              is being called upon sending a byte; when DMA transmission
                                              is used, the bytes are copied to the tx buffer by the DMA engine
                                              and the callback is called when all the bytes have been transferred. */
    void *txCallbackParam;              /*!< Transmit callback parameter pointer.*/
    uart_transfer_type_t transferType; /*!< Type of UART transfer (interrupt/dma based) */
#if FEATURE_UART_HAS_DMA_ENABLE
    uint8_t rxDMAChannel;                /*!< DMA channel number for DMA-based rx. */
    uint8_t txDMAChannel;                /*!< DMA channel number for DMA-based tx. */
#endif
    semaphore_t rxComplete;              /*!< Synchronization object for blocking Rx timeout condition */
    semaphore_t txComplete;              /*!< Synchronization object for blocking Tx timeout condition */
    volatile status_t transmitStatus;    /*!< Status of last driver transmit operation */
    volatile status_t receiveStatus;     /*!< Status of last driver receive operation */
} uart_state_t;

/*! @brief UART configuration structure
 *
 * Implements : uart_user_config_t_Class
 */
typedef struct
{
    uint32_t baudRate;                           /*!< UART baud rate */
    uart_parity_mode_t parityMode;             /*!< parity mode, disabled (default), even, odd */
    uart_stop_bit_count_t stopBitCount;        /*!< number of stop bits, 1 stop bit (default) or 2 stop bits */
    uart_bit_count_per_char_t bitCountPerChar; /*!< number of bits in a character (8-default, 9 or 10);
                                                      for 9/10 bits chars, users must provide appropriate buffers
                                                      to the send/receive functions (bits 8/9 in subsequent bytes);
                                                      for DMA transmission only 8-bit char is supported. */
    uart_transfer_type_t transferType;         /*!< Type of UART transfer (interrupt/dma based) */
    uint8_t rxDMAChannel;                        /*!< Channel number for DMA rx channel.
                                                      If DMA mode isn't used this field will be ignored. */
    uint8_t txDMAChannel;                        /*!< Channel number for DMA tx channel.
                                                      If DMA mode isn't used this field will be ignored. */
    bool idleErrorIntEnable;                    /*!< enable/disable idle interrupt. */
} uart_user_config_t;

#if FEATURE_UART_HAS_MODEM_SUPPORT
/*! @brief UART transmit CTS configuration structure
 *
 * Implements : uart_tx_cts_config_t_Class
 */
typedef struct
{
    bool txCtsEnable;
    uart_tx_cts_source_t txCtsSrc;                  /*!< set cts source, pin or the receiver address match result */
    uart_tx_cts_detection_time_t txCtsDetTime;      /*!< set cts detection time, at the start of each character or
                                                        when the transmitter is idle*/
} uart_tx_cts_config_t;

/*! @brief UART receive RTS configuration structure
 *
 * Implements : uart_rx_rts_config_t_Class
 */
typedef struct
{
    bool rxRtsEnbale;
    uart_rx_rts_watermark_t rxRtsWatermark;
}uart_rx_rts_config_t;

/*! @brief UART receive RTS configuration structure
 *
 * Implements : uart_rx_rts_config_t_Class
 */
typedef struct
{
    bool txRtsEnable;
    uart_tx_rts_polarity_t txRtsPolarity;
}uart_tx_rts_config_t;
#endif

#if defined(FEATURE_UART_HAS_ONE_WIRE_MODE_SUPPORT) && (FEATURE_UART_HAS_ONE_WIRE_MODE_SUPPORT == 1U) 
/*! @brief UART one wire mode configuration structure
 *
 * Implements : uart_one_wire_mode_config_t_Class
 */
typedef struct
{
    bool oneWireModeEnable;
    uart_tx_line_derection_t txLineDirection;
}uart_one_wire_mode_config_t;
#endif
/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name UART Driver
 * @{
 */

/*!
* @brief Initializes the UART configuration structure with default values.
*
* This function initializes a configuration structure received from the application
* with default values.
*
* @param uartUserConfig user configuration structure of type #uart_user_config_t
*/
void UART_DRV_GetDefaultConfig(uart_user_config_t *uartUserConfig);

/*!
 * @brief Initializes an UART operation instance.
 *
 * The caller provides memory for the driver state structures during initialization.
 * The user must select the UART clock source in the application to initialize the UART.
 *
 * @param instance  UART instance number
 * @param uartUserConfig user configuration structure of type #uart_user_config_t
 * @param uartStatePtr pointer to the UART driver state structure
 * @return STATUS_SUCCESS if successful;
 *         STATUS_ERROR if an error occurred
 */
status_t UART_DRV_Init(uint32_t instance, uart_state_t *uartStatePtr,
                       const uart_user_config_t *uartUserConfig);

/*!
 * @brief Shuts down the UART by disabling interrupts and transmitter/receiver.
 *
 * @param instance  UART instance number
 * @return STATUS_SUCCESS if successful;
 *         STATUS_ERROR if an error occurred
 */
status_t UART_DRV_Deinit(uint32_t instance);

/*!
 * @brief Installs callback function for the UART receive.
 *
 * @note After a callback is installed, it bypasses part of the UART IRQHandler logic.
 * Therefore, the callback needs to handle the indexes of txBuff and txSize.
 *
 * @param instance The UART instance number.
 * @param function The UART receive callback function.
 * @param rxBuff The receive buffer used inside IRQHandler. This buffer must be kept as long as the callback is alive.
 * @param callbackParam The UART receive callback parameter pointer.
 * @return Former UART receive callback function pointer.
 */
uart_callback_t UART_DRV_InstallRxCallback(uint32_t instance,
                                           uart_callback_t function,
                                           void *callbackParam);

/*!
 * @brief Installs callback function for the UART transmit.
 *
 * @note After a callback is installed, it bypasses part of the UART IRQHandler logic.
 * Therefore, the callback needs to handle the indexes of txBuff and txSize.
 *
 * @param instance The UART instance number.
 * @param function The UART transmit callback function.
 * @param txBuff The transmit buffer used inside IRQHandler. This buffer must be kept as long as the callback is alive.
 * @param callbackParam The UART transmit callback parameter pointer.
 * @return Former UART transmit callback function pointer.
 */
uart_callback_t UART_DRV_InstallTxCallback(uint32_t instance,
                                           uart_callback_t function,
                                           void *callbackParam);

/*!
 * @brief Sends data out through the UART module using a blocking method.
 *
 *  Blocking means that the function does not return until the transmission is complete.
 *
 * @param instance  UART instance number
 * @param txBuff  source buffer containing 8-bit data chars to send
 * @param txSize the number of bytes to send
 * @param timeout timeout value in milliseconds
 * @return STATUS_SUCCESS if successful;
 *         STATUS_TIMEOUT if the timeout was reached;
 *         STATUS_BUSY if the resource is busy;
 *         STATUS_ERROR if an error occurred
 */
status_t UART_DRV_SendDataBlocking(uint32_t instance,
                                   const uint8_t *txBuff,
                                   uint32_t txSize,
                                   uint32_t timeout);

/*!
 * @brief Send out multiple bytes of data using polling method.
 *
 * @param   instance  UART instance number.
 * @param   txBuff The buffer pointer which saves the data to be sent.
 * @param   txSize Size of data to be sent in unit of byte.
 * @return  STATUS_SUCCESS if successful;
 *          STATUS_BUSY if the resource is busy;
 */
status_t UART_DRV_SendDataPolling(uint32_t instance, const uint8_t *txBuff, uint32_t txSize);

/*!
 * @brief Sends data out through the UART module using a non-blocking method.
 *  This enables an a-sync method for transmitting data. When used with
 *  a non-blocking receive, the UART can perform a full duplex operation.
 *  Non-blocking  means that the function returns immediately.
 *  The application has to get the transmit status to know when the transmit is complete.
 *
 * @param instance  UART instance number
 * @param txBuff  source buffer containing 8-bit data chars to send
 * @param txSize  the number of bytes to send
 * @return STATUS_SUCCESS if successful;
 *         STATUS_BUSY if the resource is busy;
 */
status_t UART_DRV_SendData(uint32_t instance,
                           const uint8_t *txBuff,
                           uint32_t txSize);

/*!
 * @brief Returns whether the previous transmit is complete.
 *
 * @param instance  UART instance number
 * @param bytesRemaining Pointer to value that is populated with the number of bytes that
 *        have been sent in the active transfer
 *        @note In DMA mode, this parameter may not be accurate, in case the transfer completes
 *              right after calling this function; in this edge-case, the parameter will reflect
 *              the initial transfer size, due to automatic reloading of the trigger loop count
 *              in the DMA transfer descriptor.
 * @return The transmit status.
 * @retval STATUS_SUCCESS The transmit has completed successfully.
 * @retval STATUS_BUSY The transmit is still in progress. @a bytesRemaining will be
 *         filled with the number of bytes that are yet to be transmitted.
 * @retval STATUS_UART_ABORTED The transmit was aborted.
 * @retval STATUS_TIMEOUT A timeout was reached.
 * @retval STATUS_ERROR An error occurred.
 */
status_t UART_DRV_GetTransmitStatus(uint32_t instance, uint32_t *bytesRemaining);

/*!
 * @brief Terminates a non-blocking transmission early.
 *
 * @param instance  UART instance number
 * @return Whether the aborting is successful or not.
 */
status_t UART_DRV_AbortSendingData(uint32_t instance);

/*!
 * @brief Gets data from the UART module by using a blocking method.
 *  Blocking means that the function does not return until the
 *  receive is complete.
 *
 * @param instance  UART instance number
 * @param rxBuff  buffer containing 8-bit read data chars received
 * @param rxSize the number of bytes to receive
 * @param timeout timeout value in milliseconds
 * @return STATUS_SUCCESS if successful;
 *         STATUS_TIMEOUT if the timeout was reached;
 *         STATUS_BUSY if the resource is busy;
 *         STATUS_UART_FRAMING_ERROR if a framing error occurred;
 *         STATUS_UART_NOISE_ERROR if a noise error occurred;
 *         STATUS_UART_PARITY_ERROR if a parity error occurred;
 *         STATUS_UART_RX_OVERRUN if an overrun error occurred;
 *         STATUS_ERROR if a DMA error occurred;
 */
status_t UART_DRV_ReceiveDataBlocking(uint32_t instance,
                                      uint8_t *rxBuff,
                                      uint32_t rxSize,
                                      uint32_t timeout);

/*!
 * @brief Receive multiple bytes of data using polling method.
 *
 *
 * @param   instance  UART instance number.
 * @param   rxBuff The buffer pointer which saves the data to be received.
 * @param   rxSize Size of data need to be received in unit of byte.
 * @return  STATUS_SUCCESS if the transaction is successful;
 *          STATUS_BUSY if the resource is busy;
 *          STATUS_UART_RX_OVERRUN if an overrun error occurred.
 */
status_t UART_DRV_ReceiveDataPolling(uint32_t instance, uint8_t *rxBuff, uint32_t rxSize);

/*!
 * @brief Gets data from the UART module by using a non-blocking method.
 *  This enables an a-sync method for receiving data. When used with
 *  a non-blocking transmission, the UART can perform a full duplex operation.
 *  Non-blocking means that the function returns immediately.
 *  The application has to get the receive status to know when the receive is complete.
 *
 * @param instance  UART instance number
 * @param rxBuff  buffer containing 8-bit read data chars received
 * @param rxSize  the number of bytes to receive
 * @return STATUS_SUCCESS if successful;
 *         STATUS_BUSY if the resource is busy
 */
status_t UART_DRV_ReceiveData(uint32_t instance,
                              uint8_t *rxBuff,
                              uint32_t rxSize);

/*!
 * @brief Returns whether the previous receive is complete.
 *
 * @param instance  UART instance number
 * @param bytesRemaining pointer to value that is filled  with the number of bytes that
 *        still need to be received in the active transfer.
 *        @note In DMA mode, this parameter may not be accurate, in case the transfer completes
 *              right after calling this function; in this edge-case, the parameter will reflect
 *              the initial transfer size, due to automatic reloading of the trigger loop count
 *              in the DMA transfer descriptor.
 * @return The receive status.
 * @retval STATUS_SUCCESS the receive has completed successfully.
 * @retval STATUS_BUSY the receive is still in progress. @a bytesReceived will be
 *     filled with the number of bytes that have been received so far.
 * @retval STATUS_UART_ABORTED The receive was aborted.
 * @retval STATUS_TIMEOUT A timeout was reached.
 * @retval STATUS_UART_RX_OVERRUN, STATUS_UART_FRAMING_ERROR, STATUS_UART_PARITY_ERROR,
           or STATUS_UART_NOISE_ERROR, STATUS_ERROR An error occurred during reception.
 */
status_t UART_DRV_GetReceiveStatus(uint32_t instance, uint32_t *bytesRemaining);

/*!
 * @brief Terminates a non-blocking receive early.
 *
 * @param instance  UART instance number
 *
 * @return Whether the receiving was successful or not.
 */
status_t UART_DRV_AbortReceivingData(uint32_t instance);

/*!
 * @brief Configures the UART baud rate.
 *
 * This function configures the UART baud rate.
 * In some UART instances the user must disable the transmitter/receiver
 * before calling this function.
 * Generally, this may be applied to all UARTs to ensure safe operation.
 *
 * @param instance  UART instance number.
 * @param desiredBaudRate UART desired baud rate.
 * @return STATUS_BUSY if called during an on-going transfer, STATUS_SUCCESS otherwise
 */
status_t UART_DRV_SetBaudRate(uint32_t instance, uint32_t desiredBaudRate);

/*!
 * @brief Returns the UART baud rate.
 *
 * This function returns the UART configured baud rate.
 *
 * @param instance  UART instance number.
 * @param[out] configuredBaudRate UART configured baud rate.
 */
void UART_DRV_GetBaudRate(uint32_t instance, uint32_t *configuredBaudRate);

/*!
 * @brief Sets the internal driver reference to the tx buffer.
 *
 * This function can be called from the tx callback to provide the driver
 * with a new buffer, for continuous transmission.
 *
 * @param instance  UART instance number
 * @param txBuff  source buffer containing 8-bit data chars to send
 * @param txSize  the number of bytes to send
 * @return STATUS_SUCCESS
 */
status_t UART_DRV_SetTxBuffer(uint32_t instance,
                              const uint8_t *txBuff,
                              uint32_t txSize);

/*!
 * @brief Sets the internal driver reference to the rx buffer.
 *
 * This function can be called from the rx callback to provide the driver
 * with a new buffer, for continuous reception.
 *
 * @param instance  UART instance number
 * @param rxBuff  destination buffer containing 8-bit data chars to receive
 * @param rxSize  the number of bytes to receive
 * @return STATUS_SUCCESS
 */
status_t UART_DRV_SetRxBuffer(uint32_t instance,
                              uint8_t *rxBuff,
                              uint32_t rxSize);

/*!
 * @brief Sets the UART line idle detect size and interrupt enable
 *
 *
 * @param instance  UART instance number
 * @param idleSize  IDLE size
 * @param intEnable  UART line idle detect interrupt enable
 * @return STATUS_SUCCESS
 */
status_t UART_DRV_SetLineIdleDetect(uint32_t instance,
                                    uart_line_size_t idleSize,
                                    bool intEnable);

#if FEATURE_UART_HAS_MODEM_SUPPORT
/*!
 * @brief Configure transmit CTS
 *
 *
 * @param instance  UART instance number
 * @param uart_tx_cts_config_t configuration structure of type #uart_tx_cts_config_t
 */
void UART_DRV_SetTransmitCTS(uint32_t instance,
                            uart_tx_cts_config_t *uartTxCtsConfig);
/*@}*/

/*!
 * @brief Configure receive RTS
 *
 *
 * @param instance  UART instance number
 * @param uart_rx_rts_config_t configuration structure of type #uart_rx_rts_config_t
 */
void UART_DRV_SetReceiveRTS(uint32_t instance, 
                            uart_rx_rts_config_t *uartRxRtsConfig);

/*!
 * @brief Configure Transmit RTS
 *
 *
 * @param instance  UART instance number
 * @param uart_tx_rts_config_t configuration structure of type #uart_tx_rts_config_t
 */
void UART_DRV_SetTransmitRTS(uint32_t instance,
                            uart_tx_rts_config_t *uartTxRtsConfig);
                            
#endif

#if defined(FEATURE_UART_HAS_ONE_WIRE_MODE_SUPPORT) && (FEATURE_UART_HAS_ONE_WIRE_MODE_SUPPORT == 1U) 
/*!
 * @brief Configure one wire mode
 *
 *
 * @param instance  UART instance number
 * @param uart_one_wire_mode_config_t configuration structure of type #uart_one_wire_mode_config_t
 */
void UART_DRV_SetOneWireMode(uint32_t instance,
                            uart_one_wire_mode_config_t *oneWireModeConfig);
/*!
 * @brief Configure tx line derection
 *
 *
 * @param instance  UART instance number
 * @param uart_tx_line_derection_t configuration structure of type #uart_tx_line_derection_t
 */

void UART_DRV_SetTxLineDirection(uint32_t instance,
                                uart_tx_line_derection_t txLinDerection);
#endif
/*@}*/
#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* UART_DRIVER_H__ */
/******************************************************************************/
/* EOF */
/******************************************************************************/
