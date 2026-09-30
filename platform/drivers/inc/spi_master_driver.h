/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file spi_master_driver.h
 * @version 1.4.0
 */

#ifndef SPI_MASTER_DRIVER_H
#define SPI_MASTER_DRIVER_H

#include "spi_shared_function.h"

/*!
 * @defgroup spi_driver SPI driver
 * @ingroup spi
 * @brief Serial Peripheral Interface Peripheral Driver
 * @{
 */


/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*!
 * @brief Data structure containing information about a device on the SPI bus.
 *
 * The user must populate these members to set up the SPI master and
 * properly communicate with the SPI device.
 * Implements : spi_master_config_t_Class
 */
typedef struct
{
    uint32_t bitsPerSec;                 /*!< Baud rate in bits per second*/
    spi_which_pcs_t whichPcs;            /*!< Selects which PCS to use */
    spi_signal_polarity_t pcsPolarity;   /*!< PCS polarity */
    bool isPcsContinuous;                /*!< Keeps PCS asserted until transfer complete */
    uint16_t bitcount;                   /*!< Number of bits/frame, minimum is 8-bits */
    spi_clock_phase_t clkPhase;          /*!< Selects which phase of clock to capture data */
    spi_sck_polarity_t clkPolarity;      /*!< Selects clock polarity */
    bool lsbFirst;                       /*!< Option to transmit LSB first */
    spi_transfer_type transferType;      /*!< Type of SPI transfer */
#if FEATURE_SPI_HAS_DMA_ENABLE
    uint8_t rxDMAChannel;                /*!< Channel number for DMA rx channel. If DMA mode isn't used this field will be ignored. */
    uint8_t txDMAChannel;                /*!< Channel number for DMA tx channel. If DMA mode isn't used this field will be ignored. */
#endif
    spi_callback_t callback;             /*!< Select the callback to transfer complete */
    void *callbackParam;                 /*!< Select additional callback parameters if it's necessary */
    spi_transfer_width_t width;          /*!< Transfer width, single, 2-bit, or 4-bit transfer. */
} spi_master_config_t;


/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name Initialization and shutdown
 * @{
 */

/*!
 * @brief Return default configuration for SPI master
 *
 * Initializes a structured provided by user with the configuration
 * of an interrupt based SPI transfer. Source clock for SPI is configured to 
 * 8MHz. If the applications uses other frequency is necessary to update spiSrcClk field.
 *
 @param spiConfig Pointer to configuration structure which is filled with default configuration 
 */

void SPI_DRV_MasterGetDefaultConfig(spi_master_config_t *spiConfig);

/*!
 * @brief Initializes a SPI instance for interrupt driven master mode operation.
 *
 * This function uses an interrupt-driven method for transferring data.
 * In this function, the term "spiConfig" is used to indicate the SPI device for which the SPI
 * master is communicating.
 * This function initializes the run-time state structure to track the ongoing
 * transfers, un-gates the clock to the SPI module, resets the SPI module,
 * configures the IRQ state structure, enables the module-level interrupt to the core, and
 * enables the SPI module.
 * This is an example to set up the spi_master_state_t and call the
 * SPI_DRV_MasterInit function by passing in these parameters:
   @code
    spi_master_state_t spiMasterState;  <- the user  allocates memory for this structure
    spi_master_config_t spiConfig;  Can declare more configs for use in transfer functions
    spiConfig.bitsPerSec = 500000;
    spiConfig.whichPcs = SPI_PCS0;
    spiConfig.pcsPolarity = SPI_ACTIVE_LOW;
    spiConfig.isPcsContinuous = false;
    spiConfig.bitCount = 16;
    spiConfig.clkPhase = SPI_CLOCK_PHASE_1ST_EDGE;
    spiConfig.clkPolarity = SPI_ACTIVE_HIGH;
    spiConfig.lsbFirst= false;
    spiConfig.transferType = SPI_USING_INTERRUPTS;
    SPI_DRV_MasterInit(masterInstance, &spiMasterState, &spiConfig);
   @endcode
 *
 * @param[in] instance The instance number of the SPI peripheral.
 * @param[in] spiState The pointer to the SPI master driver state structure. The user
 *  passes the memory for this run-time state structure. The SPI master driver
 *  populates the members. This run-time state structure keeps track of the
 *  transfer in progress.
 * @param[in] spiConfig The data structure containing information about a device on the SPI bus
 * @return operation status
 *        - An error code :   Operation failure was occurred.
 *        - STATUS_SUCCESS:   Operation was successful.
 */
status_t SPI_DRV_MasterInit(uint32_t instance, spi_state_t *spiState,
                            const spi_master_config_t *spiConfig);

/*!
 * @brief Shuts down a SPI instance.
 *
 * This function resets the SPI peripheral, gates its clock, and disables the interrupt to
 * the core.  It first checks to see if a transfer is in progress and if so returns an error
 * status.
 *
 * @param[in] instance The instance number of the SPI peripheral.
 * @return operation status
 *         - STATUS_SUCCESS:   The transfer has completed successfully.
 *         - STATUS_BUSY:      The transfer is still in progress.
 *         - STATUS_ERROR:     If driver is error and needs to clean error.
 */
status_t SPI_DRV_MasterDeinit(uint32_t instance);

/*!
 * @brief Configures the SPI master mode bus timing delay options.
 *
 * This function involves the SPI module's delay options to
 * "fine tune" some of the signal timings and match the timing needs of a slower peripheral device.
 * This is an optional function that can be called after the SPI module has been initialized for
 * master mode. The timings are adjusted in terms of cycles of the baud rate clock.
 * The bus timing delays that can be adjusted are listed below:
 *
 * SCK to PCS Delay: Adjustable delay option between the last edge of SCK to the de-assertion
 *                   of the PCS signal.
 *
 * PCS to SCK Delay: Adjustable delay option between the assertion of the PCS signal to the
 *                   first SCK edge.
 *
 * Delay between Transfers: Adjustable delay option between the de-assertion of the PCS signal for
 *                          a frame to the assertion of the PCS signal for the next frame.
 *
 * @param[in] instance The instance number of the SPI peripheral.
 * @param[in] delayBetwenTransfers Minimum delay between 2 transfers in clock cycles
 * @param[in] delaySCKtoPCS Minimum delay between SCK and PCS in clock cycles
 * @param[in] delayPCStoSCK Minimum delay between PCS and SCK in clock cycles
 * @return operation status
 *         - STATUS_SUCCESS:   The transfer has completed successfully.
 *         - STATUS_ERROR:     If driver is error and needs to clean error.
 */
status_t SPI_DRV_MasterSetDelay(uint32_t instance, uint32_t delayBetwenTransfers,
                                uint32_t delaySCKtoPCS, uint32_t delayPCStoSCK);


/*@}*/

/*!
 * @name Bus configuration
 * @{
 */

/*!
 * @brief Configures the SPI port physical parameters to access a device on the bus when the LSPI
 *        instance is configured for interrupt operation.
 *
 * In this function, the term "spiConfig" is used to indicate the SPI device for which the SPI
 * master is communicating. This is an optional function as the spiConfig parameters are
 * normally configured in the initialization function or the transfer functions, where these various
 * functions would call the configure bus function.
 * This is an example to set up the spi_master_config_t structure
 * to call the SPI_DRV_MasterConfigureBus function by passing in these parameters:
   @code
    spi_master_config_t spiConfig1;   You can also declare spiConfig2, spiConfig3, etc
    spiConfig1.bitsPerSec = 500000;
    spiConfig1.whichPcs = SPI_PCS0;
    spiConfig1.pcsPolarity = SPI_ACTIVE_LOW;
    spiConfig1.isPcsContinuous = false;
    spiConfig1.bitCount = 16;
    spiConfig1.clkPhase = SPI_CLOCK_PHASE_1ST_EDGE;
    spiConfig1.clkPolarity = SPI_ACTIVE_HIGH;
    spiConfig1.lsbFirst= false;
    spiConfig.transferType = SPI_USING_INTERRUPTS;
   @endcode
 *
 * @param[in] instance The instance number of the SPI peripheral.
 * @param[in] spiConfig Pointer to the spiConfig structure. This structure contains the settings
 *  for the SPI bus configuration.  The SPI device parameters are the desired baud rate (in
 *  bits-per-sec), bits-per-frame, chip select attributes, clock attributes, and data shift
 *  direction.
 * @param[in] calculatedBaudRate The calculated baud rate passed back to the user to determine
 *  if the calculated baud rate is close enough to meet the needs. The baud rate never exceeds
 *  the desired baud rate.
 * @return operation status
 *         - STATUS_SUCCESS:   The transfer has completed successfully.
 *         - STATUS_ERROR:     If driver is error and needs to clean error.
 */
status_t SPI_DRV_MasterConfigureBus(uint32_t instance,
                                    const spi_master_config_t *spiConfig,
                                    uint32_t *calculatedBaudRate);

/*!
* @brief Select the chip to communicate with.
*
* The main purpose of this function is to set the PCS and the appropriate polarity.
* @param[in] instance SPI instance
* @param[in] whichPcs selected chip
* @param[in] polarity chip select line polarity
* @return operation status
*         - STATUS_SUCCESS:  The transfer has completed successfully.
*         - STATUS_ERROR:    If driver is error and needs to clean error.
*/
status_t SPI_DRV_SetPcs(uint32_t instance, spi_which_pcs_t whichPcs, spi_signal_polarity_t polarity);

/*@}*/

/*!
 * @name Polling transfers
 * @{
 */

/*!
 * @brief Performs an interrupt driven Polling SPI master mode transfer.
 *
 * This function simultaneously sends and receives data on the SPI bus, as SPI is naturally
 * a full-duplex bus. The function does not return until the transfer is complete.
 * This function allows the user to optionally pass in a SPI configuration structure which
 * allows the user to change the SPI bus attributes in conjunction with initiating a SPI transfer.
 * The difference between passing in the SPI configuration structure here as opposed to the
 * configure bus function is that the configure bus function returns the calculated baud rate where
 * this function does not. The user can also call the configure bus function prior to the transfer
 * in which case the user would simply pass in a NULL to the transfer function's device structure
 * parameter.
 * Depending on frame size sendBuffer and receiveBuffer must be aligned like this:
 * -1 byte if frame size <= 8 bits 
 * -2 bytes if 8 bits < frame size <= 16 bits 
 * -4 bytes if 16 bits < frame size   
 *
 * @param[in] instance The instance number of the SPI peripheral.
 * @param[in] sendBuffer The pointer to the data buffer of the data to send. You may pass NULL for this
 *  parameter and  bytes with a value of 0 (zero) is sent.
 * @param[in] receiveBuffer Pointer to the buffer where the received bytes are stored. If you pass NULL
 *  for this parameter, the received bytes are ignored.
 * @param[in] transferByteCount The number of bytes to send and receive which is equal to size of send or receive buffers
 * @return operation status
 *         - STATUS_SUCCESS:   The transfer was successful.
 *         - STATUS_BUSY:      Cannot perform transfer because a transfer is already in progress.
 */
status_t SPI_DRV_MasterTransferPolling(uint32_t instance,
                                       const uint8_t *sendBuffer,
                                       uint8_t *receiveBuffer,
                                       uint16_t transferByteCount);
/*!
 * @name Blocking transfers
 * @{
 */

/*!
 * @brief Performs an interrupt driven blocking SPI master mode transfer.
 *
 * This function simultaneously sends and receives data on the SPI bus, as SPI is naturally
 * a full-duplex bus. The function does not return until the transfer is complete.
 * This function allows the user to optionally pass in a SPI configuration structure which
 * allows the user to change the SPI bus attributes in conjunction with initiating a SPI transfer.
 * The difference between passing in the SPI configuration structure here as opposed to the
 * configure bus function is that the configure bus function returns the calculated baud rate where
 * this function does not. The user can also call the configure bus function prior to the transfer
 * in which case the user would simply pass in a NULL to the transfer function's device structure
 * parameter.
 * Depending on frame size sendBuffer and receiveBuffer must be aligned like this:
 * -1 byte if frame size <= 8 bits 
 * -2 bytes if 8 bits < frame size <= 16 bits 
 * -4 bytes if 16 bits < frame size   
 *
 * @param[in] instance The instance number of the SPI peripheral.
 * @param[in] sendBuffer The pointer to the data buffer of the data to send. You may pass NULL for this
 *  parameter and  bytes with a value of 0 (zero) is sent.
 * @param[in] receiveBuffer Pointer to the buffer where the received bytes are stored. If you pass NULL
 *  for this parameter, the received bytes are ignored.
 * @param[in] transferByteCount The number of bytes to send and receive which is equal to size of send or receive buffers
 * @param[in] timeout A timeout for the transfer in milliseconds. If the transfer takes longer than
 *  this amount of time, the transfer is aborted and a STATUS_TIMEOUT error
 *  returned.
 * @return operation status
 *         - STATUS_SUCCESS:   The transfer was successful.
 *         - STATUS_BUSY:      Cannot perform transfer because a transfer is already in progress.
 *         - STATUS_TIMEOUT:   The transfer timed out and was aborted.
 */
status_t SPI_DRV_MasterTransferBlocking(uint32_t instance,
                                        const uint8_t *sendBuffer,
                                        uint8_t *receiveBuffer,
                                        uint16_t transferByteCount,
                                        uint32_t timeout);

/*@}*/

/*!
 * @name Non-blocking transfers
 * @{
 */

/*!
 * @brief Performs an interrupt driven non-blocking SPI master mode transfer.
 *
 * This function simultaneously sends and receives data on the SPI bus, as SPI is naturally
 * a full-duplex bus. The function returns immediately after initiating the transfer. The user
 * needs to check whether the transfer is complete using the SPI_DRV_MasterGetTransferStatus
 * function.
 * This function allows the user to optionally pass in a SPI configuration structure which
 * allows the user to change the SPI bus attributes in conjunction with initiating a SPI transfer.
 * The difference between passing in the SPI configuration structure here as opposed to the
 * configure bus function is that the configure bus function returns the calculated baud rate where
 * this function does not. The user can also call the configure bus function prior to the transfer
 * in which case the user would simply pass in a NULL to the transfer function's device structure
 * parameter.
 * Depending on frame size sendBuffer and receiveBuffer must be aligned like this:
 * -1 byte if frame size <= 8 bits 
 * -2 bytes if 8 bits < frame size <= 16 bits 
 * -4 bytes if 16 bits < frame size
 *
 * @param[in] instance The instance number of the SPI peripheral.
 * @param[in] sendBuffer The pointer to the data buffer of the data to send. You may pass NULL for this
 *  parameter and  bytes with a value of 0 (zero) is sent.
 * @param[in] receiveBuffer Pointer to the buffer where the received bytes are stored. If you pass NULL
 *  for this parameter, the received bytes are ignored.
 * @param[in] transferByteCount The number of bytes to send and receive which is equal to size of send or receive buffers
 * @return operation status
 *         - STATUS_SUCCESS:     The transfer was successful.
 *         - STATUS_BUSY         Cannot perform transfer because a transfer is already in progress
 */
status_t SPI_DRV_MasterTransfer(uint32_t instance,
                                const uint8_t *sendBuffer,
                                uint8_t *receiveBuffer,
                                uint16_t transferByteCount);

/*!
 * @brief Returns whether the previous interrupt driven transfer is completed.
 *
 * When performing an a-sync (non-blocking) transfer, the user can call this function to ascertain
 * the state of the current transfer: in progress (or busy) or complete (success).
 * In addition, if the transfer is still in progress, the user can get the number of words that
 * have been transferred up to now.
 *
 * @param[in] instance The instance number of the SPI peripheral.
 * @param[in] bytesRemained Pointer to a value that is filled in with the number of bytes that must be received.
 * @return operation status
 *         - STATUS_SUCCESS       The transfer has completed successfully.
 *         - STATUS_BUSY          The transfer is still in progress. framesTransferred is filled
 *         with the number of words that have been transferred so far.
 */
status_t SPI_DRV_MasterGetTransferStatus(uint32_t instance, uint32_t *bytesRemained);

/*!
 * @brief Terminates an interrupt driven asynchronous transfer early.
 *
 * During an a-sync (non-blocking) transfer, the user has the option to terminate the transfer early
 * if the transfer is still in progress.
 *
 * @param[in] instance The instance number of the SPI peripheral.
 * @return operation status
 *         - STATUS_SUCCESS                       The transfer was successful.
 *         - SPI_STATUS_NO_TRANSFER_IN_PROGRESS   No transfer is currently in progress.
 */
status_t SPI_DRV_MasterAbortTransfer(uint32_t instance);

/* @}*/

#if defined(__cplusplus)
}
#endif

/*! @}*/


#endif /* __SPI_MASTER_DRIVER_H__*/
/*******************************************************************************
 * EOF
 ******************************************************************************/
