/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file spi_shared_function.h
 * @version 1.4.0
 */

/*!
 * @page misra_violations MISRA-C:2012 violations list
 *
 * PRQA S 3449 Rule 8.5: Multiple declarations of external object or function.
 *
 */

#ifndef SPI_SHARED_FUNCTION_H
#define SPI_SHARED_FUNCTION_H

#include <stdbool.h>
#include "clock_manager.h"
#include "osif.h"
#include "status.h"
#include "callbacks.h"
#if defined(FEATURE_SPI_HAS_DMA_ENABLE) && (FEATURE_SPI_HAS_DMA_ENABLE > 0U)
#include "dma_driver.h"
#endif /* FEATURE_SPI_HAS_DMA_ENABLE */

/*!
 * @addtogroup spi_driver
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*!
 * @brief Extern for the SPI master driver's interrupt handler.
 *
 */
extern void SPI_DRV_MasterIRQHandler(uint32_t instance);

/*!
 * @brief Extern for the SPI slave driver's interrupt handler.
 *
 */
extern void SPI_DRV_SlaveIRQHandler(uint32_t instance);

/*! 
 * @brief SPI Peripheral Chip Select (PCS) configuration (which PCS to configure).
 * Implements : spi_which_pcs_t_Class
 */
typedef enum
{
    SPI_PCS0 = 0U, /*!< PCS[0] */
    SPI_PCS1 = 1U, /*!< PCS[1] */
    SPI_PCS2 = 2U, /*!< PCS[2] */
    SPI_PCS3 = 3U, /*!< PCS[3] */
#if defined(FEATURE_SPI_HAS_EXTERNAL_DEVICES_SELECTION) && (FEATURE_SPI_HAS_EXTERNAL_DEVICES_SELECTION > 4)
    SPI_PCS4 = 4U, /*!< PCS[4] */
    SPI_PCS5 = 5U, /*!< PCS[5] */
    SPI_PCS6 = 6U, /*!< PCS[6] */
    SPI_PCS7 = 7U, /*!< PCS[7] */
#endif
} spi_which_pcs_t;

/*! 
 * @brief SPI Signal (PCS and Host Request) Polarity configuration.
 * Implements : spi_signal_polarity_t_Class
 */
typedef enum
{
    SPI_ACTIVE_HIGH = 1U, /*!< Signal is Active High (idles low). */
    SPI_ACTIVE_LOW = 0U   /*!< Signal is Active Low (idles high). */
} spi_signal_polarity_t;

/*! 
 * @brief SPI clock phase configuration.
 * Implements : spi_clock_phase_t_Class
 */
typedef enum
{
    SPI_CLOCK_PHASE_1ST_EDGE = 0U, /*!< Data captured on SCK 1st edge, changed on 2nd. */
    SPI_CLOCK_PHASE_2ND_EDGE = 1U  /*!< Data changed on SCK 1st edge, captured on 2nd. */
} spi_clock_phase_t;

/*!
 * @brief SPI Clock Signal (SCK) Polarity configuration.
 * Implements : spi_sck_polarity_t_Class
 */
typedef enum
{
    SPI_SCK_ACTIVE_HIGH = 0U, /*!< Signal is Active High (idles low). */
    SPI_SCK_ACTIVE_LOW = 1U   /*!< Signal is Active Low (idles high). */
} spi_sck_polarity_t;

/*!
  * @brief Type of SPI transfer (based on interrupts or DMA).
  * Implements : spi_transfer_type_Class
  */
typedef enum
{
    SPI_USING_DMA = 0,    /*!< The driver will use DMA to perform SPI transfer */
    SPI_USING_INTERRUPTS, /*!< The driver will use interrupts to perform SPI transfer */
} spi_transfer_type;

/*!
  * @brief Type of error reported by SPI
  */
typedef enum
{
    SPI_TRANSFER_OK = 0U, /*!< Transfer OK */
    SPI_TRANSMIT_FAIL,    /*!< Error during transmission */
    SPI_RECEIVE_FAIL      /*!< Error during reception */
} transfer_status_t;

/*!
 * @brief SPI transfer width configuration.
 */
typedef enum
{
    SPI_SINGLE_BIT_XFER = 0U,           /*!< 1-bit shift at a time, data out on SDO, in on SDI (normal mode) */
    SPI_TWO_BIT_XFER = 1U,              /*!< 2-bits shift out on SDO/SDI and in on SDO/SDI */
    SPI_FOUR_BIT_XFER = 2U              /*!< 4-bits shift out on SDO/SDI/PCS[3:2] and in on SDO/SDI/PCS[3:2] */
} spi_transfer_width_t;

/*!
 * @brief Runtime state structure for the SPI master driver.
 *
 * This structure holds data that is used by the SPI peripheral driver to
 * communicate between the transfer function and the interrupt handler. The
 * interrupt handler also uses this information to keep track of its progress.
 * The user must pass  the memory for this run-time state structure. The
 * SPI master driver populates the members.
 * Implements : spi_state_t_Class
 */
typedef struct
{
    uint16_t bitsPerFrame;              /*!< Number of bits per frame: 8- to 4096-bits; needed for
                                              TCR programming */
    uint16_t bytesPerFrame;             /*!< Number of bytes per frame: 1- to 512-bytes */
    bool isPcsContinuous;               /*!< Option to keep chip select asserted until transfer
                                              complete; needed for TCR programming */
    bool isBlocking;                    /*!< Save the transfer type */
    uint32_t spiSrcClk;                 /*!< Module source clock */
    volatile bool isTransferInProgress; /*!< True if there is an active transfer */
    const uint8_t *txBuff;              /*!< The buffer from which transmitted bytes are taken */
    uint8_t *rxBuff;                    /*!< The buffer into which received bytes are placed */
    volatile uint16_t txCount;          /*!< Number of bytes remaining to send  */
    volatile uint16_t rxCount;          /*!< Number of bytes remaining to receive */
    volatile uint16_t txFrameCnt;       /*!< Number of bytes from current frame which were already sent */
    volatile uint16_t rxFrameCnt;       /*!< Number of bytes from current frame which were already received */
    volatile bool lsb;                  /*!< True if first bit is LSB and false if first bit is MSB */
    uint8_t fifoSize;                   /*!< RX/TX fifo size */
#if FEATURE_SPI_HAS_DMA_ENABLE
    uint8_t rxDMAChannel;               /*!< Channel number for DMA rx channel */
    uint8_t txDMAChannel;               /*!< Channel number for DMA tx channel */
#endif
    spi_transfer_type transferType;     /*!< Type of SPI transfer */
    semaphore_t spiSemaphore;           /*!< The semaphore used for blocking transfers */
    transfer_status_t status;           /*!< The status of the current */
    spi_callback_t callback;            /*!< Select the callback to transfer complete */
    void *callbackParam;                /*!< Select additional callback parameters if it's necessary */
    uint32_t dummy;                     /*!< This field is used for the cases when TX is NULL and SPI is in DMA mode */
} spi_state_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*! 
 * @brief Table of base pointers for SPI instances. 
 */
extern SPI_Type *g_spiBase[SPI_INSTANCE_COUNT];

/*! 
 * @brief Table to save SPI clock names as defined in clock manager. 
 */
extern const clock_names_t s_spiClkNames[SPI_INSTANCE_COUNT];

/*!
 * @brief Table to save SPI IRQ enumeration numbers defined in the CMSIS header file.
 */
extern IRQn_Type g_spiIrqId[SPI_INSTANCE_COUNT];

/*!
 * @brief Pointer to runtime state structure.
 */
extern spi_state_t *g_spiStatePtr[SPI_INSTANCE_COUNT];

/*******************************************************************************
 * Function Prototypes
 ******************************************************************************/

/*!
 * @brief The function SPI_DRV_IRQHandler passes IRQ control to either the master or
 * slave driver.
 *
 * The address of the IRQ handlers are checked to make sure they are non-zero before
 * they are called. If the IRQ handler's address is zero, it means that driver was
 * not present in the link (because the IRQ handlers are marked as weak). This would
 * actually be a program error, because it means the master/slave config for the IRQ
 * was set incorrectly.
 */
void SPI_DRV_IRQHandler(uint32_t instance);

/*!
 * @brief The function SPI_DRV_FillupTxBuffer writes data in TX hardware buffer
 * depending on driver state and number of bytes remained to send.
 */
void SPI_DRV_FillupTxBuffer(uint32_t instance);

/*!
 * @brief The function SPI_DRV_ReadRXBuffer reads data from RX hardware buffer and
 * writes this data in RX software buffer.
 */
void SPI_DRV_ReadRXBuffer(uint32_t instance);

/*!
 * @brief Disable the TEIE interrupts at the end of a transfer.
 * Disable the interrupts and clear the status for transmit/receive errors.
 */
void SPI_DRV_DisableTEIEInterrupts(uint32_t instance);
/*! @} */

#endif /* __SPI_SHARED_FUNCTION_H__*/
/*******************************************************************************
 * EOF
 ******************************************************************************/
