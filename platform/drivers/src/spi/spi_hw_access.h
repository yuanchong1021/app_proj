/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file spi_hw_access.h
 * @version 1.4.0
 */

#ifndef SPI_HW_ACCESS_H
#define SPI_HW_ACCESS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "status.h"
#include "device_registers.h"
#include "spi_shared_function.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/


/*! @brief Prescaler values for SPI clock source.
 */
typedef enum
{
    SPI_DIV_1 = 0U,
    SPI_DIV_2 = 1U,
    SPI_DIV_4 = 2U,
    SPI_DIV_8 = 3U,
    SPI_DIV_16 = 4U,
    SPI_DIV_32 = 5U,
    SPI_DIV_64 = 6U,
    SPI_DIV_128 = 7U,
} spi_prescaler_t;

/*! @brief SPI status flags.
 */
typedef enum
{
    SPI_RX_DATA_FLAG = 0x0,             /*!< RX data flag */
    SPI_TX_DATA_FLAG = 0x1,             /*!< TX data flag */
    SPI_WORD_COMPLETE = 0x2,             /*!< Word Complete flag */
    SPI_FRAME_COMPLETE = 0x3,             /*!< Frame Complete flag */
    SPI_TRANSFER_COMPLETE = 0x4,             /*!< Transfer Complete flag */
    SPI_TRANSMIT_ERROR = 0x5,             /*!< Transmit Error flag (FIFO underrun) */
    SPI_RECEIVE_ERROR = 0x6,             /*!< Receive Error flag (FIFO overrun) */
    SPI_DATA_MATCH = 0x7,             /*!< Data Match flag */
    SPI_MODULE_BUSY = 0x12,            /*!< Module Busy flag */
    SPI_ALL_STATUS = 0x000000FC       /*!< Used for clearing all w1c status flags */
} spi_status_flag_t;

/*! @brief SPI master or slave configuration.
 */
typedef enum
{
    SPI_MASTER = 1U,     /*!< SPI peripheral operates in master mode. */
    SPI_SLAVE = 0U      /*!< SPI peripheral operates in slave mode. */
} spi_master_slave_mode_t;

/*! @brief SPI pin (SDO and SDI) configuration.
 */
typedef enum
{
    SPI_SDI_IN_SDO_OUT = 0U,     /*!< SPI SDI input, SDO output. */
    SPI_SDI_IN_OUT = 1U,     /*!< SDI is used for both input and output data. */
    SPI_SDO_IN_OUT = 2U,     /*!< SDO is used for both input and output data. */
    SPI_SDI_OUT_SDO_IN = 3U      /*!< SPI SDO input, SDI output. */
} spi_pin_config_t;

/*! @brief SPI data output configuration.
 */
typedef enum
{
    SPI_DATA_OUT_RETAINED = 0U, /*!< Data out retains last value when chip select de-asserted */
    SPI_DATA_OUT_TRISTATE = 1U  /*!< Data out is tri-stated when chip select de-asserted */
} spi_data_out_config_t;

/*! @brief SPI Transmit Command Register configuration structure.
 *
 * This structure contains the Transmit Command Register (TCR) settings. Any writes
 * to the TCR will cause the entire TCR contents to be pushed to the TX FIFO.
 * Therefore any updates to the TCR should include updates to all of the register
 * bit fields to form a 32-bit write to the TCR.
 */
typedef struct
{
    uint32_t frameSize;              /*!< Number of bits/frame, minimum is 8-bits. */
    spi_transfer_width_t width;    /*!< Transfer width, single, 2-bit, or 4-bit transfer. */
    bool txMask;                     /*!< Option to mask the transmit data (won't load to FIFO). */
    bool rxMask;                     /*!< Option to mask the receive data (won't store in FIFO). */
    bool contCmd;                    /*!< Master option to change cmd word within cont transfer. */
    bool contTransfer;               /*!< Master option for continuous transfer. */
    bool byteSwap;                   /*!< Option to invoke the byte swap option in the FIFOs. */
    bool lsbFirst;                   /*!< Option to transmit LSB first. */
    spi_which_pcs_t whichPcs;      /*!< Selects which PCS to use. */
    uint32_t preDiv;                /*!< Selects the baud rate prescaler divider TCR bit setting. */
    spi_clock_phase_t clkPhase;    /*!< Selects which phase of clock to capture data. */
    spi_sck_polarity_t clkPolarity; /*!< Selects clock polarity. */
} spi_tx_cmd_config_t;

/*! @brief SPI initialization configuration structure.
 *
 * This structure contains parameters for the user to fill in to configure the SPI.
 * The user passes this structure into the SPI init function to configure it to
 * their desired parameters.
 * Example user code for:
    - 60MHz assumed, check ref manual for exact value
    - baudrate 500KHz
    - master mode
    - PCS is active low
   @code
    spi_init_config_t spiCfg;
    spiCfg.spiSrcClk = 60000000;
    spiCfg.baudRate = 500000;
    spiCfg.spiMode = SPI_MASTER;
    spiCfg.pcsPol = SPI_ACTIVE_LOW;
   @endcode
 */
typedef struct
{
    uint32_t spiSrcClk;                /*!< SPI module clock */
    uint32_t baudRate;                   /*!< SPI baudrate */
    spi_master_slave_mode_t spiMode; /*!< SPI master/slave mode */
    spi_signal_polarity_t pcsPol;      /*!< SPI PCS polarity */
} spi_init_config_t;

/*! @brief SPI delay type selection
 */
typedef enum
{
    SPI_SCK_TO_PCS = SPI_CLK_SCKPCS_SHIFT,     /*!< SCK to PCS Delay */
    SPI_PCS_TO_SCK = SPI_CLK_PCSSCK_SHIFT,     /*!< PCS to SCK Delay */
    SPI_BETWEEN_TRANSFER = SPI_CLK_FMDLY_SHIFT  /*!< Delay between transfers */
} spi_delay_type_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* Defines constant value arrays for the baud rate pre-scalar values.*/
static const uint32_t s_baudratePrescaler[] = {1, 2, 4, 8, 16, 32, 64, 128};

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name Configuration
 * @{
 */

/*!
 * @brief Resets the SPI internal logic and registers to their default settings.
 *
 * This function first performs a software reset of the SPI module which resets the
 * internal SPI logic and most registers, then proceeds to manually reset all of the
 * SPI registers to their default setting to ensuring these registers at programmed to
 * their default value which includes disabling the module.
 *
 * @param Module base pointer of type SPI_Type.
 */
void SPI_Init(SPI_Type *base);

/*!
 * @brief Enables the SPI module.
 *
 * @param base Module base pointer of type SPI_Type.
 */
static inline void SPI_Enable(SPI_Type *base)
{
    (base->CTRL) |= SPI_CTRL_EN_MASK;
}

/*!
 * @brief Disables the SPI module.
 *
 * @param base Module base pointer of type SPI_Type.
 * @return This function returns STATUS_BUSY if it is detected that the Module Busy Flag
 *         (MBF) is set, otherwise, if success, it returns STATUS_SUCCESS.
 */
status_t SPI_Disable(SPI_Type *base);

/*!
 * @brief Configures the SPI for master or slave.
 *
 * Note that the SPI module must first be disabled before configuring this.
 *
 * @param base Module base pointer of type SPI_Type.
 * @param mode Mode setting (master or slave) of type spi_master_slave_mode_t
 * @return This function returns the error condition STATUS_ERROR if the module is not
 *         disabled else it returns STATUS_SUCCESS.
 */
status_t SPI_SetMasterSlaveMode(SPI_Type *base, spi_master_slave_mode_t mode);

/*!
 * @brief Returns whether the SPI module is in master mode.
 *
 * @param base Module base pointer of type SPI_Type.
 * @return Returns true if SPI in master mode or false if in slave mode.
 */
static inline bool SPI_IsMaster(const SPI_Type *base)
{
    return (((base->CTRL >> SPI_CTRL_MODE_SHIFT) & 1U) != 0U);
}
#if !defined(FEATURE_SPI_LITE_VERSION)
/*!
 * @brief Gets FIFO sizes of the SPI module.
 *
 * @ param base Module base pointer of type SPI_Type.
 * @ param fifoSize The FIFO size passed back to the user
 */
static inline void SPI_GetFifoSizes(const SPI_Type *base, uint8_t *fifoSize)
{
    if (fifoSize != NULL)
    {
        *fifoSize = (uint8_t) (1U << ((base->TXFIFO & SPI_TXFIFO_SIZE_MASK) >> SPI_TXFIFO_SIZE_SHIFT));
    }
}

/*!
 * @brief Flushes the SPI FIFOs.
 *
 * @param base Module base pointer of type SPI_Type.
 * @param flushTxFifo Flushes (true) the Tx FIFO, else do not flush (false) the Tx FIFO
 * @param flushRxFifo Flushes (true) the Rx FIFO, else do not flush (false) the Rx FIFO
 */
void SPI_SetFlushFifoCmd(SPI_Type *base, bool flushTxFifo, bool flushRxFifo);

/*!
 * @brief Sets the RX FIFO watermark values.
 *
 * This function allows the user to set the RX FIFO watermarks.
 *
 * @param base Module base pointer of type SPI_Type.
 * @param rxWater The RX FIFO watermark value
 */
static inline void SPI_SetRxWatermarks(SPI_Type *base, uint32_t rxWater)
{
    uint32_t spi_tmp = base->RXFIFO;
    spi_tmp &= ~(SPI_RXFIFO_WATER_MASK);
    spi_tmp |= (rxWater << SPI_RXFIFO_WATER_SHIFT);
    base->RXFIFO = spi_tmp;
}

/*!
 * @brief Sets the TX FIFO watermark values.
 *
 * This function allows the user to set the TX FIFO watermarks.
 *
 * @param base Module base pointer of type SPI_Type.
 * @param txWater The TX FIFO watermark value
 */
static inline void SPI_SetTxWatermarks(SPI_Type *base, uint32_t txWater)
{
    uint32_t spi_tmp = base->TXFIFO;
    spi_tmp &= ~(SPI_TXFIFO_WATER_MASK);
    spi_tmp |= (txWater << SPI_TXFIFO_WATER_SHIFT);
    base->TXFIFO = spi_tmp;
}
#endif /* !defined(FEATURE_SPI_LITE_VERSION) */
/*@}*/

/*!
 * @name Status flags and Interrupt configuration
 * @{
 */

/*!
 * @brief Gets the SPI status flag state.
 *
 * This function returns the state of one of the SPI status flags as requested by
 * the user.
 *
 * @param base Module base pointer of type SPI_Type.
 * @param statusFlag The status flag, of type spi_status_flag_t
 * @return State of the status flag: asserted (true) or not-asserted (false)
 */
static inline bool SPI_GetStatusFlag(const SPI_Type *base,
                                     spi_status_flag_t statusFlag)
{
    return ((((base->STS) >> (uint8_t) statusFlag) & 1U) != 0U);
}

/*!
 * @brief Clears the SPI status flag.
 *
 * This function clears the state of one of the SPI status flags as requested by
 * the user. Note, the flag must be w1c capable, if not the function returns an error.
 * w1c capable flags are:
 *   SPI_WORD_COMPLETE
 *   SPI_FRAME_COMPLETE
 *   SPI_TRANSFER_COMPLETE
 *   SPI_TRANSMIT_ERROR
 *   SPI_RECEIVE_ERROR
 *   SPI_DATA_MATCH
 *
 * @param base Module base pointer of type SPI_Type.
 * @param statusFlag The status flag, of type spi_status_flag_t
 * @return STATUS_SUCCESS or SPI_STS_INVALID_PARAMETER
 */
status_t SPI_ClearStatusFlag(SPI_Type *base, spi_status_flag_t statusFlag);

/*!
 * @brief Configures the SPI interrupts.
 *
 * @param base Module base pointer of type SPI_Type.
 * @param interruptSrc The interrupt source, of type spi_status_flag_t
 * @param enable Enable (true) or disable (false) the interrupt source
 */
static inline void SPI_SetIntMode(SPI_Type *base,
                                  spi_status_flag_t interruptSrc, bool enable)
{
    if (enable == true)
    {
        base->INTE |= (uint32_t) 1U << (uint8_t) interruptSrc;
    } else
    {
        base->INTE &= ~((uint32_t) 1U << (uint8_t) interruptSrc);
    }
}

/*!
 * @brief Returns if the SPI interrupt request is enabled or disabled.
 *
 * @param base Module base pointer of type SPI_Type.
 * @param interruptSrc The interrupt source, of type spi_status_flag_t
 * @return Returns if the interrupt source is enabled (true) or disabled (false)
 */
static inline bool SPI_GetIntMode(const SPI_Type *base,
                                  spi_status_flag_t interruptSrc)
{
    return ((((base->INTE) >> (uint8_t) interruptSrc) & 1U) != 0U);
}

/*@}*/

/*!
 * @name DMA configuration
 * @{
 */
#if !defined(FEATURE_SPI_LITE_VERSION)
/*!
 * @brief Sets the SPI Transmit Data DMA configuration (enable or disable).
 *
 * @param base Module base pointer of type SPI_Type.
 * @param enable Enable (true) or disable (false) the TX DMA request
 */
static inline void SPI_SetTxDmaCmd(SPI_Type *base, bool enable)
{
    base->CTRL = (base->CTRL & (~SPI_CTRL_TXDMAEN_MASK)) | (enable ? SPI_CTRL_TXDMAEN_MASK : 0U);
}

/*!
 * @brief Sets the SPI Receive Data DMA configuration (enable or disable).
 *
 * @param base Module base pointer of type SPI_Type.
 * @param enable Enable (true) or disable (false) the RX DMA request
 */
static inline void SPI_SetRxDmaCmd(SPI_Type *base, bool enable)
{
    (base->CTRL) = (base->CTRL & (~SPI_CTRL_RXDMAEN_MASK)) | (enable ? SPI_CTRL_RXDMAEN_MASK : 0U);
}
#endif /* !defined(FEATURE_SPI_LITE_VERSION) */

/*!
 * @brief Manually configures a specific SPI delay parameter (module must be disabled to
 *        change the delay values).
 *
 * This function configures the:
 * SCK to PCS delay, or
 * PCS to SCK delay, or
 * Between transfer delay.
 *
 * These delay names are available in type spi_delay_type_t.
 *
 * The user passes which delay they want to configure along with the delay value.
 * This allows the user to directly set the delay values if they have
 * pre-calculated them or if they simply wish to manually increment the value.
 *
 * Note that the SPI module must first be disabled before configuring this.
 * Note that the SPI module must be configure for master mode before configuring this.
 *
 * @param base Module base pointer of type SPI_Type.
 * @param whichDelay The desired delay to configure, must be of type spi_delay_type_t
 * @param delay The 8-bit delay value 0x00 to 0xFF (255). The delay is equal to:
 *             -delay + 1 cycles of the SPI baud rate clock (SCK to PCS and PCS to SCK)
 *             -delay + 2 cycles of the SPI baud rate clock (Between transfer delay)
 * @return Either STATUS_SUCCESS, SPI_STS_OUT_OF_RANGE, or STATUS_ERROR if
 *         SPI is not disabled or if is not set for master mode.
 */
static inline status_t SPI_SetDelay(SPI_Type *base, spi_delay_type_t whichDelay, uint32_t delay)
{
    uint32_t ccrValue = 0;

    ccrValue = base->CLK & ~(0xFFUL << (uint32_t) whichDelay);
    ccrValue |= delay << (uint32_t) whichDelay;
    base->CLK = ccrValue;
    return STATUS_SUCCESS;
}

/*@}*/

/*!
 * @name SPI Bus Configuration
 * @{
 */

/*!
 * @brief Configures the desired SPI PCS polarity.
 *
 * This function allows the user to configure the polarity of a particular PCS signal.
 * Note that the SPI module must first be disabled before configuring this.
 *
 * @param base Module base pointer of type SPI_Type.
 * @param whichPcs Select which PCS to program, of type spi_which_pcs_t
 * @param pcsPolarity Set PCS as active high or low, of type spi_signal_polarity_t
 * @return This function returns the error condition STATUS_ERROR if the module is not
 *         disabled else it returns STATUS_SUCCESS.
 */
status_t SPI_SetPcsPolarityMode(SPI_Type *base, spi_which_pcs_t whichPcs,
                                spi_signal_polarity_t pcsPolarity);

/*!
 * @brief Configures the SPI SDO/SDI pin configuration mode.
 *
 * This function configures the pin mode of the SPI.
 * For the SDI and SDO pins, the user can configure these pins as follows:
 *  SDI is used for input data and SDO for output data.
 *  SDO is used for input data and SDO for output data.
 *  SDI is used for input data and SDI for output data.
 *  SDO is used for input data and SDI for output data.
 *
 * The user has the option to configure the output data as:
 *  Output data retains last value when chip select is de-asserted (default setting).
 *  Output data is tristated when chip select is de-asserted.
 *
 * Finally, the user has the option to configure the PCS[3:2] pins as:
 *  Enabled for PCS operation (default setting).
 *  Disabled - this is need if the user wishes to configure the SPI mode for 4-bit transfers
 *             where these pins will be used as I/O data pins.
 *
 * Note that the SPI module must first be disabled before configuring this.
 *
 * @param base Module base pointer of type SPI_Type.
 * @param pinCfg Select configuration for the SDO/SDI pins (see spi_pin_config_t)
 * @param dataOutConfig Select data output config after chip select de-assertion
 * @param pcs3and2Enable Enable or disable PCS[3:2]
 * @return This function returns the error condition STATUS_ERROR if the module is not
 *         disabled else it returns STATUS_SUCCESS.
 */
status_t SPI_SetPinConfigMode(SPI_Type *base,
                              spi_pin_config_t pinCfg,
                              spi_data_out_config_t dataOutConfig,
                              bool pcs3and2Enable);

/*!
 * @brief Sets the SPI baud rate in bits per second.
 *
 * This function takes in the desired bitsPerSec (baud rate) and calculates the nearest
 * possible baud rate without exceeding the desired baud rate, and returns the
 * calculated baud rate in bits-per-second. It requires that the caller also provide
 * the frequency of the module source clock (in Hertz). Also note that the baud rate
 * does not take into affect until the Transmit Control Register (TCR) is programmed
 * with the PRESCALE value. Hence, this function returns the PRESCALE tcrPrescaleValue
 * parameter for later programming in the TCR.  It is up to the higher level
 * peripheral driver to alert the user of an out of range baud rate input.
 *
 * Note that the SPI module must first be disabled before configuring this.
 * Note that the SPI module must be configure for master mode before configuring this.
 *
 * @param base Module base pointer of type SPI_Type.
 * @param bitsPerSec The desired baud rate in bits per second
 * @param sourceClockInHz Module source input clock in Hertz
 * @param tcrPrescaleValue The TCR PRESCALE value, needed by user to program the TCR
 * @return  The actual calculated baud rate. This function may also return a "0" if the
 *          SPI is not configued for master mode or if the SPI module is not disabled.
 */
uint32_t SPI_SetBaudRate(SPI_Type *base, uint32_t bitsPerSec,
                         uint32_t sourceClockInHz, uint32_t *tcrPrescaleValue);

/*!
 * @brief Configures the baud rate divisor manually (only the SPI_CCR[SCKDIV]).
 *
 * This function allows the caller to manually set the baud rate divisor in the event
 * that this divider is known and the caller does not wish to call the
 * SPI_SetBaudRate function. Note that this only affects the SPI_CCR[SCKDIV]).
 * The Transmit Control Register (TCR) is programmed separately with the PRESCALE value.
 * The valid range is 0x00 to 0xFF, if the user inputs outside of this range, an error
 * is returned.
 *
 * Note that the SPI module must first be disabled before configuring this.
 * Note that the SPI module must be configure for master mode before configuring this.
 *
 * @param base Module base pointer of type SPI_Type.
 * @param divisor Desired baud rate divisor setting (0x00 to 0xFF)
 * @return STATUS_SUCCESS or SPI_STS_OUT_OF_RANGE if divisor > 0xFF
 */
status_t SPI_SetBaudRateDivisor(SPI_Type *base, uint32_t divisor);

/*!
 * @brief Sets the PCS flag to the value of the whichPcs parameter.
 *
 * @param base Module base pointer of type SPI_Type.
 * @param whichPcs Desired chip
 */
void SPI_SetPcs(SPI_Type *base, spi_which_pcs_t whichPcs);

/*@}*/

/*!
 * @name Data transfer
 * @{
 */

/*!
 * @brief Sets the Transmit Command Register (TCR) parameters.
 *
 * The Transmit Command Register (TCR) contains multiple parameters that affect
 * the transmission of data, such as clock phase and polarity, which PCS to use,
 * whether or not the PCS remains asserted at the completion of a frame, etc.
 * Any writes to this register results in an immediate push of the entire register
 * and its contents to the TX FIFO.  Hence, writes to this register should include
 * all of the desired parameters written to the register at once. Hence, the user
 * should fill in the members of the spi_tx_cmd_config_t data structure and pass
 * this to the function.
 *
 * @param base Module base pointer of type SPI_Type.
 * @param txCmdCfgSet Structure that contains the Transmit Command Register (TCR)
 *                    settings of type spi_tx_cmd_config_t
 */
void SPI_SetTxCommandReg(SPI_Type *base, const spi_tx_cmd_config_t *txCmdCfgSet);

/*!
 * @brief Writes data into the TX data buffer.
 *
 * This function writes data passed in by the user to the Transmit Data Register (TDR).
 * The user can pass up to 32-bits of data to load into the TDR. If the frame size exceeds 32-bits,
 * the user will have to manage sending the data one 32-bit word at a time.
 * Any writes to the TDR will result in an immediate push to the TX FIFO.
 * This function can be used for either master or slave mode.
 *
 * @param base Module base pointer of type SPI_Type.
 * @param data The data word to be sent
 */
static inline void SPI_WriteData(SPI_Type *base, uint32_t data)
{
    base->DATA = data;
}

/*!
 * @brief Reads data from the data buffer.
 *
 * This function reads the data from the Receive Data Register (RDR).
 * This function can be used for either master or slave mode.
 *
 * @param base Module base pointer of type SPI_Type.
 * @return The data read from the data buffer
 */
static inline uint32_t SPI_ReadData(const SPI_Type *base)
{
    return (uint32_t) base->DATA;
}

#if !defined(FEATURE_SPI_LITE_VERSION)
/*!
 * @brief Reads TX COUNT form the FIFO Status Register.
 *
 * This function reads the TX COUNT field  from the FIFO Status Register (FSR).
 *
 * @param base Module base pointer of type SPI_Type.
 * @return The data read from the FIFO Status Register
 */
static inline uint32_t SPI_ReadTxCount(const SPI_Type *base)
{
    return (uint32_t) (((uint32_t) (base->TXFIFO & SPI_TXFIFO_COUNT_MASK)) >> SPI_TXFIFO_COUNT_SHIFT);
}

/*!
 * @brief Reads RX COUNT form the FIFO Status Register.
 *
 * This function reads the RX COUNT field  from the FIFO Status Register (FSR).
 *
 * @param base Module base pointer of type SPI_Type.
 * @return The data read from the FIFO Status Register
 */
static inline uint32_t SPI_ReadRxCount(const SPI_Type *base)
{
    return (uint32_t) ((((uint32_t) base->RXFIFO & (uint32_t) SPI_RXFIFO_COUNT_MASK))
        >> (uint32_t) SPI_RXFIFO_COUNT_SHIFT);
}
#endif /* !defined(FEATURE_SPI_LITE_VERSION) */

/*!
 * @brief Clear RXMSK bit form TCR Register.
 *
 * This function clears the RXMSK bit from the Transmit Command Register (TCR).
 *
 * @param base Module base pointer of type SPI_Type.
 */
static inline void SPI_ClearRxmaskBit(SPI_Type *base)
{
    (base->TXCFG) = ((base->TXCFG) & (~SPI_TXCFG_MSKRX_MASK));
}

/*!
 * @brief Set RXMSK bit form TCR Register.
 *
 * This function set the RXMSK bit from the Transmit Command Register (TCR).
 *
 * @param base Module base pointer of type SPI_Type.
 */
static inline void SPI_SetRxmskBit(SPI_Type *base)
{
    (base->TXCFG) = ((base->TXCFG) | (SPI_TXCFG_MSKRX_MASK));
}

/*!
 * @brief Clear TXMSK bit form TCR Register.
 *
 * This function clears the TXMSK bit from the Transmit Command Register (TCR).
 *
 * @param base Module base pointer of type SPI_Type.
 */
static inline void SPI_ClearTxmaskBit(SPI_Type *base)
{
    (base->TXCFG) = ((base->TXCFG) & (~SPI_TXCFG_MSKTX_MASK));
}

/*!
 * @brief Set TXMSK bit form TCR Register.
 *
 * This function set the TXMSK bit from the Transmit Command Register (TCR).
 *
 * @param base Module base pointer of type SPI_Type.
 */
static inline void SPI_SetTxmskBit(SPI_Type *base)
{
    (base->TXCFG) = ((base->TXCFG) | (SPI_TXCFG_MSKTX_MASK));
}

/*!
 * @brief Clear CONT bit form continues mode.
 *
 * This function clears the CONT bit from the continues mode.
 *
 * @param base Module base pointer of type SPI_Type.
 */
static inline void SPI_ClearContBit(SPI_Type *base)
{
    (base->TXCFG) = ((base->TXCFG) & (~SPI_TXCFG_CONT_MASK));
}

/*!
 * @brief Set CONT bit form TCR Register.
 *
 * This function sets the CONT bit from the Transmit Command Register (TCR).
 *
 * @param base Module base pointer of type SPI_Type.
 */
static inline void SPI_SetContBit(SPI_Type *base)
{
    (base->TXCFG) = ((base->TXCFG) | (SPI_TXCFG_CONT_MASK));
}

#if !defined(FEATURE_SPI_LITE_VERSION)
/*!
 * @brief Clear CONTC bit form TCR Register.
 *
 * This function clears the CONTC bit from the Transmit Command Register (TCR).
 *
 * @param base Module base pointer of type SPI_Type.
 */
static inline void SPI_ClearContCBit(SPI_Type *base)
{
    (base->TXCFG) = ((base->TXCFG) & (~SPI_TXCFG_CONTC_MASK));
}

/*!
 * @brief Set CONTC bit form TCR Register.
 *
 * This function set the CONTC bit from the Transmit Command Register (TCR).
 *
 * @param base Module base pointer of type SPI_Type.
 */
static inline void SPI_SetContCBit(SPI_Type *base)
{
    (base->TXCFG) = ((base->TXCFG) | (SPI_TXCFG_CONTC_MASK));
}

/*!
 * @brief Configures the clock prescaler used for all SPI master logic.
 *
 * @param base Module base pointer of type SPI_Type.
 * @param prescaler Prescaler value for master logic.
 */
static inline void SPI_SetClockPrescaler(SPI_Type *base, spi_prescaler_t prescaler)
{
    uint32_t spi_tmp = base->TXCFG;
    spi_tmp &= ~(SPI_TXCFG_PRESCALE_MASK);
    spi_tmp |= ((uint32_t) prescaler << SPI_TXCFG_PRESCALE_SHIFT);
    base->TXCFG = spi_tmp;
}

/*!
 * @brief Get the clock prescaler used for all SPI master logic.
 *
 * @param base Module base pointer of type SPI_Type.
 * @return Prescaler value for master logic.
 */
static inline spi_prescaler_t SPI_GetClockPrescaler(const SPI_Type *base)
{
    uint32_t prescalerValue = (((base->TXCFG) & SPI_TXCFG_PRESCALE_MASK) >> SPI_TXCFG_PRESCALE_SHIFT);
    spi_prescaler_t prescaler = (spi_prescaler_t)prescalerValue;
    return prescaler;
}

#endif /* !FEATURE_SPI_LITE_VERSION */

/*!
 * @brief Configures if the sample point for master devices is delayed.
 *
 * @param base Module base pointer of type SPI_Type.
 * @param isSamplingPointDelayed Configure if the sampling point is delayed for master devices
 */
static inline void SPI_SetSamplingPoint(SPI_Type *base, bool isSamplingPointDelayed)
{
    uint32_t spi_tmp = base->CTRL;
    spi_tmp &= ~(SPI_CTRL_SPDEN_MASK);
    spi_tmp |= (isSamplingPointDelayed ? SPI_CTRL_SPDEN(1U) : 0U);
    base->CTRL = spi_tmp;
}

/*@}*/

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* SPI_HW_ACCESS_H*/

/*******************************************************************************
 * EOF
 ******************************************************************************/
