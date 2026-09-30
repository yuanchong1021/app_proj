/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file spi_slave_driver.h
 * @version 1.4.0
 */

/*!
 * @page misra_violations MISRA-C:2012 violations list
 *
 * PRQA S 3449 Rule 8.5: Multiple declarations of external object or function.Declarations.
 *
 */

#ifndef SPI_SLAVE_DRIVER_H
#define SPI_SLAVE_DRIVER_H

#include "spi_shared_function.h"

/*!
 * @addtogroup spi_driver
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*!
 * @brief User configuration structure for the SPI slave driver.
 * Implements : spi_slave_config_t_Class
 */
typedef struct
{
    spi_signal_polarity_t pcsPolarity;      /*!< PCS polarity */
    uint16_t bitcount;                      /*!< Number of bits/frame, minimum is 8-bits */
    spi_clock_phase_t clkPhase;             /*!< Selects which phase of clock to capture data */
    spi_which_pcs_t whichPcs;               /*!< Selects which pcs */
    spi_sck_polarity_t clkPolarity;         /*!< Selects clock polarity */
    bool lsbFirst;                          /*!< Option to transmit LSB first */
    spi_transfer_type transferType;         /*!< Type of SPI transfer */
    uint8_t rxDMAChannel;                   /*!< Channel number for DMA rx channel. If DMA mode isn't used this field will be ignored. */
    uint8_t txDMAChannel;                   /*!< Channel number for DMA tx channel. If DMA mode isn't used this field will be ignored. */
    spi_callback_t callback;                /*!< Select the callback to transfer complete */
    void *callbackParam;                    /*!< Select additional callback parameters if it's necessary */
    spi_transfer_width_t width;             /*!< Transfer width, single, 2-bit, or 4-bit transfer. */
} spi_slave_config_t;


/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Return default configuration for SPI master.
 *
 * Initializes a structured provided by user with the configuration
 * of an interrupt based SPI transfer. 
 *
 */

void SPI_DRV_SlaveGetDefaultConfig(spi_slave_config_t *spiConfig);

/*!
 * @brief Initializes a SPI instance for a slave mode operation, using interrupt mechanism.
 *
 * This function un-gates the clock to the SPI module, initializes the SPI for
 * slave mode. After it is  initialized, the SPI module is configured in slave mode and the
 * user can start transmitting and receiving data by calling send, receive, and transfer functions.
 * This function indicates SPI slave uses an interrupt mechanism.
 *
 * @param[in] instance The instance number of the SPI peripheral.
 * @param[in] spiState The pointer to the SPI slave driver state structure.
 * @param[in] slaveConfig The configuration structure spi_slave_user_config_t which
 *      configures the data bus format.
 *
 * @return operation status
 *         - An error code :        Operation failure was occurred.
 *         - STATUS_SUCCESS:        Operation was successful.
 */
status_t SPI_DRV_SlaveInit(uint32_t instance,
                           spi_state_t *spiState,
                           const spi_slave_config_t *slaveConfig);

/*!
 * @brief Shuts down an SPI instance interrupt mechanism.
 *
 * Disables the SPI module, gates its clock, and changes the SPI slave driver state to NonInit for the
 * SPI slave module which is initialized with interrupt mechanism. After de-initialization, the
 * user can re-initialize the SPI slave module with other mechanisms.
 *
 * @param[in] instance The instance number of the SPI peripheral.
 * @return operation status
 *         - STATUS_SUCCESS:     If driver starts to send/receive data successfully.
 *         - STATUS_ERROR:       If driver is error and needs to clean error.
 *         - STATUS_BUSY:        If a transfer is in progress
 */
status_t SPI_DRV_SlaveDeinit(uint32_t instance);

/*!
 * @brief Transfers data on SPI bus using a blocking call.
 *
 * This function checks the driver status and mechanism and transmits/receives data through the SPI
 * bus. If the sendBuffer is NULL, the transmit process is ignored. If the receiveBuffer is NULL, the
 * receive process is ignored. If both the receiveBuffer and the sendBuffer are available, the transmit and the
 * receive progress is processed. If only the receiveBuffer is available, the receive is
 * processed. Otherwise, the transmit is processed. This function only returns when the
 * processes are completed. This function uses an interrupt mechanism.
 * Depending on frame size sendBuffer and receiveBuffer must be aligned like this:
 * -1 byte if frame size <= 8 bits 
 * -2 bytes if 8 bits < frame size <= 16 bits 
 * -4 bytes if 16 bits < frame size   
 *
 * @param[in] instance The instance number of SPI peripheral
 * @param[in] sendBuffer The pointer to data that user wants to transmit.
 * @param[in] receiveBuffer The pointer to data that user wants to store received data.
 * @param[in] transferByteCount The number of bytes to send and receive which is equal to size of send or receive buffers
 * @param[in] timeout The maximum number of milliseconds that function waits before
 *              timed out reached.
 *
 * @return  operation status
 *          - STATUS_SUCCESS:    If driver starts to send/receive data successfully.
 *          - STATUS_ERROR:      If driver is error and needs to clean error.
 *          - STATUS_BUSY:       If a transfer is in progress
 *          - STATUS_TIMEOUT:    If time out reached while transferring is in progress.
 */
status_t SPI_DRV_SlaveTransferBlocking(uint32_t instance,
                                       const uint8_t *sendBuffer,
                                       uint8_t *receiveBuffer,
                                       uint16_t transferByteCount,
                                       uint32_t timeout);

/*!
 * @brief Starts the transfer data on SPI bus using a non-blocking call.
 *
 * This function checks the driver status and mechanism and transmits/receives data through the SPI
 * bus. If the sendBuffer is NULL, the transmit process is ignored. If the receiveBuffer is NULL, the
 * receive process is ignored. If both the receiveBuffer and the sendBuffer are available, the transmit and the
 * receive progress is processed. If only the receiveBuffer is available, the receive is
 * processed. Otherwise, the transmit is processed. This function only returns when the
 * processes are completed. This function uses an interrupt mechanism.
 * Depending on frame size sendBuffer and receiveBuffer must be aligned like this:
 * -1 byte if frame size <= 8 bits 
 * -2 bytes if 8 bits < frame size <= 16 bits 
 * -4 bytes if 16 bits < frame size
 *
 * @param[in] instance The instance number of SPI peripheral
 * @param[in] sendBuffer The pointer to data that user wants to transmit.
 * @param[in] receiveBuffer The pointer to data that user wants to store received data.
 * @param[in] transferByteCount The number of bytes to send and receive which is equal to size of send or receive buffers
 *
 * @return  operation status
 *          - STATUS_SUCCESS:   If driver starts to send/receive data successfully.
 *          - STATUS_ERROR:     If driver is error and needs to clean error.
 *          - STATUS_BUSY:      If a transfer is in progress
 */
status_t SPI_DRV_SlaveTransfer(uint32_t instance,
                               const uint8_t *sendBuffer,
                               uint8_t *receiveBuffer,
                               uint16_t transferByteCount);

/*!
 * @brief Aborts the transfer that started by a non-blocking call transfer function.
 *
 * This function stops the transfer which was started by the calling the SPI_DRV_SlaveTransfer() function.
 *
 * @param[in] instance The instance number of SPI peripheral
 *
 * @return  operation status
 *          - STATUS_SUCCESS:   If everything is OK.
 *
 */
status_t SPI_DRV_SlaveAbortTransfer(uint32_t instance);

/*!
 * @brief Returns whether the previous transfer is finished.
 *
 * When performing an a-sync transfer, the user can call this function to ascertain
 * the state of the current transfer: in progress (or busy) or complete (success).
 * In addition, if the transfer is still in progress, the user can get the number
 * of bytes that have been transferred up to now.
 *
 * @param[in] instance The instance number of the SPI peripheral.
 * @param[in] bytesRemained Pointer to value that is filled in with the number of
 *  frames that have been sent in the active transfer. A frame is defined as the
 *  number of bits per frame.
 *
 * @return operation status
 *         - STATUS_SUCCESS:    The transfer has completed successfully.
 *         - STATUS_BUSY:       The transfer is still in progress.
 *         - STATUS_ERROR:      If driver is error and needs to clean error.
 */
status_t SPI_DRV_SlaveGetTransferStatus(uint32_t instance,
                                        uint32_t *bytesRemained);

#if defined(__cplusplus)
}
#endif


/*! @} */


#endif /* __SPI_SLAVE_DRIVER_H__ */


