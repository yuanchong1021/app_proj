/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file i2c_hw_access.h
 * @version 1.4.0
 */

/*!
 * @page misra_violations MISRA-C:2012 violations list
 *
 * PRQA S 0779 Rule 5.2: dentifier does not differ from other identifier(s)
 *
 */

#ifndef I2C_HW_ACCESS_H
#define I2C_HW_ACCESS_H

#include <stdbool.h>
#include "i2c_driver.h"
#include "device_registers.h"


/*******************************************************************************
 * Definitions
 ******************************************************************************/


/*!
 * I2C master interrupts
 */

#define   I2C_MASTER_DATA_MATCH_INT             (I2C_MIE_MATIE_MASK)    /*!< Data Match Interrupt       */
#define   I2C_MASTER_PIN_LOW_TIMEOUT_INT        (I2C_MIE_TOIE_MASK)     /*!< Pin Low Timeout Interrupt  */
#define   I2C_MASTER_FIFO_ERROR_INT             (I2C_MIE_OPERRIE_MASK)  /*!< FIFO Error Interrupt       */
#define   I2C_MASTER_ARBITRATION_LOST_INT       (I2C_MIE_ARBIE_MASK)    /*!< Arbitration Lost Interrupt */
#define   I2C_MASTER_NACK_DETECT_INT            (I2C_MIE_NACKIE_MASK)   /*!< NACK Detect Interrupt      */
#define   I2C_MASTER_STOP_DETECT_INT            (I2C_MIE_STOPIE_MASK)   /*!< STOP Detect Interrupt      */
#define   I2C_MASTER_END_PACKET_INT             (I2C_MIE_EPIE_MASK)     /*!< End Packet Interrupt       */
#define   I2C_MASTER_RECEIVE_DATA_INT           (I2C_MIE_RXIE_MASK)     /*!< Receive Data Interrupt     */
#define   I2C_MASTER_TRANSMIT_DATA_INT          (I2C_MIE_TXIE_MASK)     /*!< Transmit Data Interrupt    */

/*!
 * I2C slave interrupts
 */
#define   I2C_SLAVE_SMBUS_ALERT_RESPONSE_INT    (I2C_SIE_ALERTIE_MASK)  /*!< SMBus Alert Response Interrupt */
#define   I2C_SLAVE_GENERAL_CALL_INT            (I2C_SIE_GCIE_MASK)     /*!< General Call Interrupt         */
#define   I2C_SLAVE_ADDRESS_MATCH_1_INT         (I2C_SIE_MAT1IE_MASK)   /*!< Address Match 1 Interrupt      */
#define   I2C_SLAVE_ADDRESS_MATCH_0_INT         (I2C_SIE_MAT0IE_MASK)   /*!< Address Match 0 Interrupt      */
#define   I2C_SLAVE_FIFO_ERROR_INT              (I2C_SIE_OPERRIE_MASK)  /*!< FIFO Error Interrupt           */
#define   I2C_SLAVE_BIT_ERROR_INT               (I2C_SIE_ARBIE_MASK)    /*!< Bit Error Interrupt            */
#define   I2C_SLAVE_STOP_DETECT_INT             (I2C_SIE_STOPIE_MASK)   /*!< STOP Detect Interrupt          */
#define   I2C_SLAVE_REPEATED_START_INT          (I2C_SIE_RSIE_MASK)     /*!< Repeated Start Interrupt       */
#define   I2C_SLAVE_TRANSMIT_ACK_INT            (I2C_SIE_ACKIE_MASK)    /*!< Transmit ACK Interrupt         */
#define   I2C_SLAVE_ADDRESS_VALID_INT           (I2C_SIE_ADDRIE_MASK)   /*!< Address Valid Interrupt        */
#define   I2C_SLAVE_RECEIVE_DATA_INT            (I2C_SIE_RXIE_MASK)     /*!< Receive Data Interrupt         */
#define   I2C_SLAVE_TRANSMIT_DATA_INT           (I2C_SIE_TXIE_MASK)     /*!< Transmit Data Interrupt        */

/*! @brief Pin configuration selection
 */
typedef enum
{
    I2C_CFG_2PIN_OPEN_DRAIN = 0U,  /*!< 2-pin open drain mode */
    I2C_CFG_2PIN_PUSH_PULL = 1U,  /*!< 2-pin push-pull mode */
    I2C_CFG_2PIN_OUTPUT_ONLY = 3U,  /*!< 2-pin output only mode (ultra-fast mode) */
} i2c_pin_config_t;

/*! @brief Master NACK reaction configuration
 */
typedef enum
{
    I2C_NACK_RECEIVE = 0U,  /*!< Receive ACK and NACK normally */
    I2C_NACK_IGNORE = 1U,  /*!< Treat a received NACK as if it was an ACK */
} i2c_nack_config_t;

/*! @brief I2C master prescaler options
 */
typedef enum
{
    I2C_MASTER_PRESC_DIV_1 = 0U,  /*!< Divide by 1   */
    I2C_MASTER_PRESC_DIV_2 = 1U,  /*!< Divide by 2   */
    I2C_MASTER_PRESC_DIV_4 = 2U,  /*!< Divide by 4   */
    I2C_MASTER_PRESC_DIV_8 = 3U,  /*!< Divide by 8   */
    I2C_MASTER_PRESC_DIV_16 = 4U,  /*!< Divide by 16  */
    I2C_MASTER_PRESC_DIV_32 = 5U,  /*!< Divide by 32  */
    I2C_MASTER_PRESC_DIV_64 = 6U,  /*!< Divide by 64  */
    I2C_MASTER_PRESC_DIV_128 = 7U,  /*!< Divide by 128 */
} i2c_master_prescaler_t;

/*! @brief Slave address configuration
 */
typedef enum
{
    I2C_SLAVE_ADDR_MATCH_0_7BIT = 0U,  /*!< Address match 0 (7-bit) */
    I2C_SLAVE_ADDR_MATCH_0_10BIT = 1U,  /*!< Address match 0 (10-bit) */
    I2C_SLAVE_ADDR_MATCH_0_7BIT_OR_1_7BIT = 2U,  /*!< Address match 0 (7-bit) or Address match 1 (7-bit) */
    I2C_SLAVE_ADDR_MATCH_0_10BIT_OR_1_10BIT = 3U,  /*!< Address match 0 (10-bit) or Address match 1 (10-bit) */
    I2C_SLAVE_ADDR_MATCH_0_7BIT_OR_1_10BIT = 4U,  /*!< Address match 0 (7-bit) or Address match 1 (10-bit) */     /* PRQA S 0779 */
    I2C_SLAVE_ADDR_MATCH_0_10BIT_OR_1_7BIT = 5U,  /*!< Address match 0 (10-bit) or Address match 1 (7-bit) */     /* PRQA S 0779 */
    I2C_SLAVE_ADDR_MATCH_RANGE_7BIT = 6U,  /*!< From Address match 0 (7-bit) to Address match 1 (7-bit) */
    I2C_SLAVE_ADDR_MATCH_RANGE_10BIT = 7U,  /*!< From Address match 0 (10-bit) to Address match 1 (10-bit) */
} i2c_slave_addr_config_t;

/*! @brief Slave NACK reaction configuration
 */
typedef enum
{
    I2C_SLAVE_NACK_END_TRANSFER = 0U,  /*!< Slave will end transfer when NACK detected */
    I2C_SLAVE_NACK_CONTINUE_TRANSFER = 1U,  /*!< Slave will not end transfer when NACK detected */
} i2c_slave_nack_config_t;

/*! @brief Slave ACK transmission options
 */
typedef enum
{
    I2C_SLAVE_TRANSMIT_ACK = 0U,  /*!< Transmit ACK for received word  */
    I2C_SLAVE_TRANSMIT_NACK = 1U,  /*!< Transmit NACK for received word */
} i2c_slave_nack_transmit_t;

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
 * @brief Get the size of the Master Receive FIFO
 * 
 * This function returns the size of the Master Receive FIFO, always a power of 2.
 * 
 * @param baseAddr  base address of the I2C module
 * @return  Master Receive FIFO Size
 */
static inline uint16_t I2C_Get_MasterRxFIFOSize(const I2C_Type *baseAddr)
{
    uint32_t tmp = baseAddr->RXFIFO;
    tmp = (tmp & I2C_RXFIFO_SIZE_MASK) >> I2C_RXFIFO_SIZE_SHIFT;
    tmp = 1UL << tmp;     /* RX FIFO size = 2^MRXFIFO */
    return (uint16_t) tmp;
}


/*!
 * @brief Get the size of the Master Transmit FIFO
 * 
 * This function returns the size of the Master Transmit FIFO, always a power of 2.
 * 
 * @param baseAddr  base address of the I2C module
 * @return  Master Transmit FIFO Size
 */
static inline uint16_t I2C_Get_MasterTxFIFOSize(const I2C_Type *baseAddr)
{
    uint32_t tmp = baseAddr->TXFIFO;
    tmp = (tmp & I2C_TXFIFO_SIZE_MASK) >> I2C_TXFIFO_SIZE_SHIFT;
    tmp = 1UL << tmp;      /* TX FIFO size = 2^MTXFIFO */
    return (uint16_t) tmp;
}


/*!
 * @brief Reset the master receive FIFO
 * 
 * This function empties the receive FIFO of the I2C master.
 * 
 * @param baseAddr  base address of the I2C module
 */
static inline void I2C_Reset_MasterRxFIFOCmd(I2C_Type *baseAddr)
{
    baseAddr->RXFIFO |= I2C_RXFIFO_RESET_MASK;
}


/*!
 * @brief Reset the master transmit FIFO
 * 
 * This function empties the transmit FIFO of the I2C master. 
 * 
 * @param baseAddr  base address of the I2C module
 */
static inline void I2C_Reset_MasterTxFIFOCmd(I2C_Type *baseAddr)
{
    baseAddr->TXFIFO |= I2C_TXFIFO_RESET_MASK;
}


/*!
 * @brief Enable or disable the I2C master
 * 
 * This function enables or disables the I2C module in master mode. If the module 
 * is enabled, the transmit FIFO  is not empty and the I2C bus is idle, then 
 * the I2C master will immediately initiate a transfer on the I2C bus.
 * 
 * @param baseAddr  base address of the I2C module
 * @param enable  specifies whether to enable or disable the I2C master
 */
static inline void I2C_Set_MasterEnable(I2C_Type *baseAddr, bool enable)
{
    uint32_t regValue = (uint32_t) baseAddr->MCTRL;
    regValue &= (uint32_t) (~(I2C_MCTRL_MEN_MASK));
    regValue |= (uint32_t)(enable ? 1U : 0U) << I2C_MCTRL_MEN_SHIFT;
    baseAddr->MCTRL = (uint32_t) regValue;
}

/*!
 * @brief Indicate the availability of receive data
 * 
 * This function returns true when the number of words in the receive FIFO is greater 
 * than the receive FIFO watermark. See function I2C_MasterSetRxFIFOWatermark()
 * for configuring the receive FIFO watermark.
 * 
 * @param baseAddr  base address of the I2C module
 * @return  receive data ready/not ready
 */
static inline bool I2C_Get_MasterReceiveDataReadyEvent(const I2C_Type *baseAddr)
{
    uint32_t regValue = baseAddr->MSTS;
    return (((regValue & I2C_MSTS_RXIF_MASK) >> I2C_MSTS_RXIF_SHIFT) != 0U);
}

/*!
 * @brief Indicate if the I2C master requests more data
 * 
 * This function returns true when the number of words in the transmit FIFO is equal 
 * or less than the transmit FIFO watermark. See function I2C_Set_MasterTxFIFOWatermark()
 * for configuring the transmit FIFO watermark.
 *
 * @param baseAddr  base address of the I2C module
 * @return  transmit data requested/not requested
 */
static inline bool I2C_Get_MasterTransmitDataRequestEvent(const I2C_Type *baseAddr)
{
    uint32_t regValue = baseAddr->MSTS;
    return (((regValue & I2C_MSTS_TXIF_MASK) >> I2C_MSTS_TXIF_SHIFT) != 0U);
}

/*!
 * @brief Check the occurrence of a FIFO error event
 * 
 * This function returns true if the I2C master detects an attempt to send or 
 * receive data without first generating a (repeated) START condition. This can 
 * occur if the transmit FIFO underflows when the AUTOSTOP bit is set. When this 
 * flag is set, the I2C master will send a STOP condition (if busy) and will 
 * not initiate a new START condition until this flag has been cleared.
 * 
 * @param baseAddr  base address of the I2C module
 * @return  indication of a FIFO error event
 */
static inline bool I2C_Get_MasterFIFOErrorEvent(const I2C_Type *baseAddr)
{
    uint32_t regValue = baseAddr->MSTS;
    regValue = (regValue & I2C_MSTS_OPERRIF_MASK) >> I2C_MSTS_OPERRIF_SHIFT;
    return (bool) ((regValue != 0U) ? true : false);
}

/*!
 * @brief Get line low detection event.
 *
 * @param baseAddr  base address of the I2C module
 */
static inline bool I2C_Get_MasterLineLowTimeoutEvent(I2C_Type *baseAddr)
{
    return (bool) ((((baseAddr->MSTS & I2C_MSTS_TOIF_MASK) >> I2C_MSTS_TOIF_SHIFT) != 0U) ? true : false);
}

/*!
 * @brief Check the occurrence of an arbitration lost event
 * 
 * This function returns true if the I2C master detects an arbitration lost
 * condition, as defined by the I2C standard. When this flag sets, the I2C 
 * master will release the bus (go idle) and will not initiate a new START 
 * condition until this flag has been cleared.
 * 
 * @param baseAddr  base address of the I2C module
 * @return  indication of an arbitration lost event
 */
static inline bool I2C_Get_MasterArbitrationLostEvent(const I2C_Type *baseAddr)
{
    uint32_t regValue = baseAddr->MSTS;
    regValue = (regValue & I2C_MSTS_ARBIF_MASK) >> I2C_MSTS_ARBIF_SHIFT;
    return (bool) ((regValue != 0U) ? true : false);
}

/*!
 * @brief Check the occurrence of an unexpected NACK event
 * 
 * This function returns true if the I2C master detects a NACK when 
 * transmitting an address or data. If a NACK is expected for a given address 
 * (as configured by the command word) then the flag will set if a NACK is not
 * generated. When set, the master will transmit a STOP condition and will not 
 * initiate a new START condition until this flag has been cleared.
 * 
 * @param baseAddr  base address of the I2C module
 * @return  indication of an unexpected NACK event
 */
static inline bool I2C_Get_MasterNACKDetectEvent(const I2C_Type *baseAddr)
{
    uint32_t regValue = baseAddr->MSTS;
    regValue = (regValue & I2C_MSTS_NACKIF_MASK) >> I2C_MSTS_NACKIF_SHIFT;
    return (bool) ((regValue != 0U) ? true : false);
}

/*!
 * @brief Check the bus busy condition event
 * 
 * This function returns true if the I2C bus is busy.
 * 
 * @param baseAddr  base address of the I2C module
 * @return  indication of a bus busy event
 */
static inline bool I2C_Get_MasterBusBusyEvent(const I2C_Type *baseAddr)
{
    uint32_t regValue = baseAddr->MSTS;
    regValue = (regValue & I2C_MSTS_BUSY_MASK) >> I2C_MSTS_BUSY_SHIFT;
    return (bool) ((regValue != 0U) ? true : false);
}

/*!
 * @brief Clear the FIFO error event flag
 * 
 * This function clears the FIFO error event. This event must be cleared before 
 * the I2C master can initiate a START condition.
 * 
 * @param baseAddr  base address of the I2C module
 */
static inline void I2C_Clear_MasterFIFOErrorEvent(I2C_Type *baseAddr)
{
    baseAddr->MSTS = ((uint32_t) 1U << I2C_MSTS_OPERRIF_SHIFT);
}

/*!
 * @brief Clear the line low event flag
 * 
 * This function clears the line low event. This event must be cleared 
 * before the I2C master can initiate a START condition.
 * 
 * @param baseAddr  base address of the I2C module
 */
static inline void I2C_Clear_MasterLineLowTimeoutEvent(I2C_Type *baseAddr)
{
    baseAddr->MSTS = ((uint32_t) 1U << I2C_MSTS_TOIF_SHIFT);
}

/*!
 * @brief Clear the arbitration lost event flag
 * 
 * This function clears the arbitration lost event. This event must be cleared 
 * before the I2C master can initiate a START condition.
 * 
 * @param baseAddr  base address of the I2C module
 */
static inline void I2C_Clear_MasterArbitrationLostEvent(I2C_Type *baseAddr)
{
    baseAddr->MSTS = ((uint32_t) 1U << I2C_MSTS_ARBIF_SHIFT);
}

/*!
 * @brief Clear the unexpected NACK event flag
 * 
 * This function clears the unexpected NACK event. This event must be cleared 
 * before the I2C master can initiate a START condition.
 * 
 * @param baseAddr  base address of the I2C module
 */
static inline void I2C_Clear_MasterNACKDetectEvent(I2C_Type *baseAddr)
{
    baseAddr->MSTS = ((uint32_t) 1U << I2C_MSTS_NACKIF_SHIFT);
}

/*!
 * @brief Enable/disable receive data DMA requests
 * 
 * This function enables or disables generation of Rx DMA requests when data
 * can be read from the receive FIFO, as configured by the receive FIFO watermark.
 * 
 * @param baseAddr  base address of the I2C module
 * @param enable  specifies whether to enable or disable DMA requests
 */
static inline void I2C_Set_MasterRxDMA(I2C_Type *baseAddr, bool enable)
{
    uint32_t regValue = (uint32_t) baseAddr->MCTRL;
    regValue &= (uint32_t) (~(I2C_MCTRL_RXDMAEN_MASK));
    regValue |= (uint32_t)(enable ? 1U : 0U) << I2C_MCTRL_RXDMAEN_SHIFT;
    baseAddr->MCTRL = (uint32_t) regValue;
}

/*!
 * @brief Enable/disable transmit data DMA requests
 * 
 * This function enables or disables generation of Tx DMA requests when data
 * can be written to the transmit FIFO, as configured by the transmit FIFO watermark.
 * 
 * @param baseAddr  base address of the I2C module
 * @param enable  specifies whether to enable or disable DMA requests
 */
static inline void I2C_Set_MasterTxDMA(I2C_Type *baseAddr, bool enable)
{
    uint32_t regValue = (uint32_t) baseAddr->MCTRL;
    regValue &= (uint32_t) (~(I2C_MCTRL_TXDMAEN_MASK));
    regValue |= (uint32_t)(enable ? 1U : 0U) << I2C_MCTRL_TXDMAEN_SHIFT;
    baseAddr->MCTRL = (uint32_t) regValue;
}

/*!
 * @brief Enable or disable specified I2C master interrupts
 * 
 * This function can enable or disable one or more master interrupt sources 
 * specified by the interrupts parameter.
 * 
 * @param baseAddr  base address of the I2C module
 * @param interrupts  interrupts to be enabled or disabled; 
 *  must be a bitwise or between one or more of the following constants: 
 *  - I2C_MASTER_DATA_MATCH_INT          - Data Match Interrupt
 *  - I2C_MASTER_PIN_LOW_TIMEOUT_INT     - Pin Low Timeout Interrupt
 *  - I2C_MASTER_FIFO_ERROR_INT          - FIFO Error Interrupt
 *  - I2C_MASTER_ARBITRATION_LOST_INT    - Arbitration Lost Interrupt
 *  - I2C_MASTER_NACK_DETECT_INT         - NACK Detect Interrupt
 *  - I2C_MASTER_STOP_DETECT_INT         - STOP Detect Interrupt
 *  - I2C_MASTER_END_PACKET_INT          - End Packet Interrupt
 *  - I2C_MASTER_RECEIVE_DATA_INT        - Receive Data Interrupt
 *  - I2C_MASTER_TRANSMIT_DATA_INT       - Transmit Data Interrupt
 * @param enable  specifies whether to enable or disable specified interrupts
 */
static inline void I2C_Set_MasterInt(I2C_Type *baseAddr, uint32_t interrupts, bool enable)
{
    uint32_t tmp = baseAddr->MIE;

    if (enable == true)
    {
        tmp |= interrupts;
    } else
    {
        tmp &= ~interrupts;
    }
    baseAddr->MIE = tmp;
}

/*!
 * @brief Set the pin mode of the module
 * 
 * This function sets the pin mode of the module. See type i2c_pin_config_t for 
 * a description of available modes.
 * 
 * @param baseAddr  base address of the I2C module
 * @param configuration  pin mode of the module
 */
static inline void I2C_Set_MasterPinConfig(I2C_Type *baseAddr, i2c_pin_config_t configuration)
{
    uint32_t tmp = baseAddr->MCTRL;
    tmp &= ~(I2C_MCTRL_PPEN_MASK | I2C_MCTRL_HSMOD_MASK);
    tmp |= ((uint32_t)configuration) << I2C_MCTRL_PPEN_SHIFT;
    baseAddr->MCTRL = tmp;
}

/*!
 * @brief Configure the reaction of the module on NACK reception
 * 
 * This function configures how the I2C master reacts when receiving a NACK. NACK responses can 
 * be treated normally or ignored. In Ultra-Fast mode it is necessary to configure the module to 
 * ignore NACK responses.
 * 
 * @param baseAddr  base address of the I2C module
 * @param configuration  set reaction of the module on NACK reception
 */
static inline void I2C_Set_MasterNACKConfig(I2C_Type *baseAddr, i2c_nack_config_t configuration)
{
    uint32_t regValue = (uint32_t) baseAddr->MCTRL;
    regValue &= (uint32_t) (~(I2C_MCTRL_IGACK_MASK));
    regValue |= I2C_MCTRL_IGACK(configuration);
    baseAddr->MCTRL = (uint32_t) regValue;
}

/*!
 * @brief Configure the I2C master prescaler
 * 
 * This function configures the clock prescaler used for all I2C master logic, 
 * except the digital glitch filters.
 * 
 * @param baseAddr  base address of the I2C module
 * @param prescaler  I2C master prescaler
 */
static inline void I2C_Set_MasterPrescaler(I2C_Type *baseAddr, i2c_master_prescaler_t prescaler)
{
    uint32_t tmp = baseAddr->MFLTCFG;
    tmp &= ~(I2C_MFLTCFG_DIV_MASK);
    tmp |= I2C_MFLTCFG_DIV(prescaler);
    baseAddr->MFLTCFG = tmp;
}

/*!
 * @brief Return the I2C master prescaler
 * 
 * This function returns the currently configured clock prescaler.
 * 
 * @param baseAddr  base address of the I2C module
 * @return  I2C master prescaler
 */
static inline i2c_master_prescaler_t I2C_Get_MasterPrescaler(const I2C_Type *baseAddr)
{
    uint32_t tmp = baseAddr->MFLTCFG;
    tmp = (tmp & I2C_MFLTCFG_DIV_MASK) >> I2C_MFLTCFG_DIV_SHIFT;
    return (i2c_master_prescaler_t) tmp;
}

/*!
 * @brief Return the configured minimum clock high period
 * 
 * This function returns the currently configured value for clock high period.
 * 
 * @param baseAddr  base address of the I2C module
 * @return  minimum clock high period
 */
static inline uint8_t I2C_Get_MasterClockHighPeriod(const I2C_Type *baseAddr)
{
    uint32_t tmp = baseAddr->MCLKCFG;
    tmp = (tmp & I2C_MCLKCFG_HIGH_MASK) >> I2C_MCLKCFG_HIGH_SHIFT;
    return (uint8_t) tmp;
}

/*!
 * @brief Set the minimum clock high period
 * 
 * This function configures the minimum number of cycles (minus one) that the 
 * SCL clock is driven high by the master. The SCL high time is extended by the 
 * time it takes to detect a rising edge on the external SCL pin. Ignoring any 
 * additional board delay due to external loading, this is equal to 
 * (2 + FILTSCL) / 2^PRESCALE cycles.
 * 
 * @param baseAddr  base address of the I2C module
 * @param value  minimum clock high period
 */
static inline void I2C_Set_MasterClockHighPeriod(I2C_Type *baseAddr, uint8_t value)
{
    uint32_t tmp = baseAddr->MCLKCFG;
    tmp &= ~(I2C_MCLKCFG_HIGH_MASK);
    tmp |= I2C_MCLKCFG_HIGH(value);
    baseAddr->MCLKCFG = tmp;
}

/*!
 * @brief Set the data hold time for SDA
 * 
 * This function sets the minimum number of cycles (minus one) that is used as the 
 * data hold time for SDA. Must be configured less than the minimum SCL low period.
 * 
 * @param baseAddr  base address of the I2C module
 * @param value  value of the data hold time for SDA
 */
static inline void I2C_Set_MasterDataValidDelay(I2C_Type *baseAddr, uint8_t value)
{
    uint32_t tmp = baseAddr->MCLKCFG;
    tmp &= ~(I2C_MCLKCFG_VALID_MASK);
    tmp |= I2C_MCLKCFG_VALID(value);
    baseAddr->MCLKCFG = tmp;
}

/*!
 * @brief Set the setup and hold delay for a START / STOP condition
 * 
 * This function configures the Minimum number of cycles (minus one) that is used 
 * by the master as the setup and hold time for a (repeated) START condition and setup 
 * time for a STOP condition. The setup time is extended by the time it takes to detect 
 * a rising edge on the external SCL pin. Ignoring any additional board delay due to 
 * external loading, this is equal to (2 + FILTSCL) / 2^PRESCALE cycles.
 * 
 * @param baseAddr  base address of the I2C module
 * @param value  setup and hold time for a START / STOP condition
 */
static inline void I2C_Set_MasterSetupHoldDelay(I2C_Type *baseAddr, uint8_t value)
{
    uint32_t tmp = baseAddr->MCLKCFG;
    tmp &= ~(I2C_MCLKCFG_STHD_MASK);
    tmp |= I2C_MCLKCFG_STHD(value);
    baseAddr->MCLKCFG = tmp;
}

/*!
 * @brief Set the minimum clock low period
 * 
 * This function configures the minimum number of cycles (minus one) that the 
 * SCL clock is driven low by the master. This value is also used for the 
 * minimum bus free time between a STOP and a START condition.
 * 
 * @param baseAddr  base address of the I2C module
 * @param value  minimum clock low period
 */
static inline void I2C_Set_MasterClockLowPeriod(I2C_Type *baseAddr, uint8_t value)
{
    uint32_t tmp = baseAddr->MCLKCFG;
    tmp &= ~(I2C_MCLKCFG_LOW_MASK);
    tmp |= I2C_MCLKCFG_LOW(value);
    baseAddr->MCLKCFG = tmp;
}

/*!
 * @brief Return the configured minimum clock low period
 * 
 * This function returns the currently configured value for clock low period.
 * 
 * @param baseAddr  base address of the I2C module
 * @return  minimum clock low period
 */
static inline uint8_t I2C_Get_MasterClockLowPeriod(const I2C_Type *baseAddr)
{
    uint32_t tmp = baseAddr->MCLKCFG;
    tmp = (tmp & I2C_MCLKCFG_LOW_MASK) >> I2C_MCLKCFG_LOW_SHIFT;
    return (uint8_t) tmp;
}

#if(I2C_HAS_HIGH_SPEED_MODE)

/*!
 * @brief Set the data hold time for SDA in high-speed mode
 * 
 * This function sets the minimum number of cycles (minus one) that is used as the 
 * data hold time for SDA in High-Speed mode. Must be configured less than the 
 * minimum SCL low period.
 * This setting only has effect during High-Speed mode transfers.
 * 
 * @param baseAddr  base address of the I2C module
 * @param value  value of the data hold time for SDA
 */
static inline void I2C_Set_MasterDataValidDelayHS(I2C_Type *baseAddr, uint8_t value)
{
    uint32_t tmp = baseAddr->HSCLK;
    tmp &= ~(I2C_HSCLK_VALID_MASK);
    tmp |= I2C_HSCLK_VALID(value);
    baseAddr->HSCLK = tmp;
}

#endif

#if(I2C_HAS_HIGH_SPEED_MODE)

/*!
 * @brief Set the setup and hold time for a START / STOP condition in high-speed mode
 * 
 * This function configures the Minimum number of cycles (minus one) that is used 
 * by the master as the setup and hold time for a (repeated) START condition and setup 
 * time for a STOP condition. The setup time is extended by the time it takes to detect 
 * a rising edge on the external SCL pin. Ignoring any additional board delay due to 
 * external loading, this is equal to (2 + FILTSCL) / 2^PRESCALE cycles.
 * This setting only has effect during High-Speed mode transfers.
 * 
 * @param baseAddr  base address of the I2C module
 * @param value  setup and hold time for a START / STOP condition
 */
static inline void I2C_Set_MasterSetupHoldDelayHS(I2C_Type *baseAddr, uint8_t value)
{
    uint32_t tmp = baseAddr->HSCLK;
    tmp &= ~(I2C_HSCLK_STHD_MASK);
    tmp |= I2C_HSCLK_STHD(value);
    baseAddr->HSCLK = tmp;
}

#endif

#if(I2C_HAS_HIGH_SPEED_MODE)

/*!
 * @brief Set the minimum clock high period in high-speed mode
 * 
 * This function configures the minimum number of cycles (minus one) that the 
 * SCL clock is driven high by the master. The SCL high time is extended by the 
 * time it takes to detect a rising edge on the external SCL pin. Ignoring any 
 * additional board delay due to external loading, this is equal to 
 * (2 + FILTSCL) / 2^PRESCALE cycles.
 * This setting only has effect during High-Speed mode transfers.
 * 
 * @param baseAddr  base address of the I2C module
 * @param value  minimum clock high period
 */
static inline void I2C_Set_MasterClockHighPeriodHS(I2C_Type *baseAddr, uint8_t value)
{
    uint32_t tmp = baseAddr->HSCLK;
    tmp &= ~(I2C_HSCLK_HIGH_MASK);
    tmp |= I2C_HSCLK_HIGH(value);
    baseAddr->HSCLK = tmp;
}

#endif

#if(I2C_HAS_HIGH_SPEED_MODE)

/*!
 * @brief Return the configured minimum clock high period in high-speed mode
 * 
 * This function returns the currently configured value for clock high period
 * in high-speed mode.
 * 
 * @param baseAddr  base address of the I2C module
 * @return  minimum clock high period
 */
static inline uint8_t I2C_Get_MasterClockHighPeriodHS(const I2C_Type *baseAddr)
{
    uint32_t tmp = baseAddr->HSCLK;
    tmp = (tmp & I2C_HSCLK_HIGH_MASK) >> I2C_HSCLK_HIGH_SHIFT;
    return (uint8_t) tmp;
}

#endif

#if(I2C_HAS_HIGH_SPEED_MODE)

/*!
 * @brief Set the minimum clock low period in high-speed mode
 * 
 * This function configures the minimum number of cycles (minus one) that the 
 * SCL clock is driven low by the master. This value is also used for the 
 * minimum bus free time between a STOP and a START condition.
 * This setting only has effect during High-Speed mode transfers.
 * 
 * @param baseAddr  base address of the I2C module
 * @param value  minimum clock low period
 */
static inline void I2C_Set_MasterClockLowPeriodHS(I2C_Type *baseAddr, uint8_t value)
{
    uint32_t tmp = baseAddr->HSCLK;
    tmp &= ~(I2C_HSCLK_LOW_MASK);
    tmp |= I2C_HSCLK_LOW(value);
    baseAddr->HSCLK = tmp;
}

#endif

#if(I2C_HAS_HIGH_SPEED_MODE)

/*!
 * @brief Return the configured minimum clock low period in high-speed mode
 * 
 * This function returns the currently configured value for clock low period
 * in high-speed mode.
 * 
 * @param baseAddr  base address of the I2C module
 * @return  minimum clock low period
 */
static inline uint8_t I2C_Get_MasterClockLowPeriodHS(const I2C_Type *baseAddr)
{
    uint32_t tmp = baseAddr->HSCLK;
    tmp = (tmp & I2C_HSCLK_LOW_MASK) >> I2C_HSCLK_LOW_SHIFT;
    return (uint8_t) tmp;
}

#endif

/*!
 * @brief Set the receive FIFO watermark
 * 
 * This function configures the receive FIFO watermark. Whenever the number of words in the receive 
 * FIFO is greater than the receive FIFO watermark, a receive data ready event is generated.
 * Writing a value equal or greater than the FIFO size will be truncated.
 * 
 * @param baseAddr  base address of the I2C module
 * @param value  number of words in the receive FIFO that will cause the receive data flag to be set
 */
static inline void I2C_Set_MasterRxFIFOWatermark(I2C_Type *baseAddr, uint16_t value)
{
    uint32_t tmp = baseAddr->RXFIFO;
    tmp &= ~(I2C_RXFIFO_WATER_MASK);
    tmp |= I2C_RXFIFO_WATER(value);
    baseAddr->RXFIFO = tmp;
}

/*!
 * @brief Return the configured receive FIFO watermark
 * 
 * This function returns the currently configured value for receive FIFO watermark
 * 
 * @param baseAddr  base address of the I2C module
 * @return  number of words in the receive FIFO that will cause the receive data flag to be set
 */
static inline uint16_t I2C_Get_MasterRxFIFOWatermark(const I2C_Type *baseAddr)
{
    uint32_t tmp = baseAddr->RXFIFO;
    tmp = (tmp & I2C_RXFIFO_WATER_MASK) >> I2C_RXFIFO_WATER_SHIFT;
    return (uint16_t) tmp;
}

/*!
 * @brief Set the transmit FIFO watermark
 * 
 * This function configures the transmit FIFO watermark. Whenever the number of words in the transmit 
 * FIFO is greater than the transmit FIFO watermark, a transmit data request event is generated.
 * Writing a value equal or greater than the FIFO size will be truncated.
 * 
 * @param baseAddr  base address of the I2C module
 * @param value  number of words in the transmit FIFO that will cause the transmit data flag to be set
 */
static inline void I2C_Set_MasterTxFIFOWatermark(I2C_Type *baseAddr, uint16_t value)
{
    uint32_t tmp = baseAddr->TXFIFO;
    tmp &= ~(I2C_TXFIFO_WATER_MASK);
    tmp |= I2C_TXFIFO_WATER(value);
    baseAddr->TXFIFO = tmp;
}

/*!
 * @brief Return the number of words in the receive FIFO
 * 
 * This function returns the number of words currently available in the receive FIFO.
 * 
 * @param baseAddr  base address of the I2C module
 * @return  the number of words in the receive FIFO
 */
static inline uint16_t I2C_Get_MasterRxFIFOCount(const I2C_Type *baseAddr)
{
    uint32_t tmp = baseAddr->RXFIFO;
    tmp = (tmp & I2C_RXFIFO_COUNT_MASK) >> I2C_RXFIFO_COUNT_SHIFT;
    return (uint16_t) tmp;
}

/*!
 * @brief Return the number of words in the transmit FIFO
 * 
 * This function returns the number of words currently available in the transmit FIFO.
 * 
 * @param baseAddr  base address of the I2C module
 * @return  the number of words in the transmit FIFO
 */
static inline uint16_t I2C_Get_MasterTxFIFOCount(const I2C_Type *baseAddr)
{
    uint32_t tmp = baseAddr->TXFIFO;
    tmp = (tmp & I2C_TXFIFO_COUNT_MASK) >> I2C_TXFIFO_COUNT_SHIFT;
    return (uint16_t) tmp;
}

/*!
 * @brief Provide commands and data for the I2C master
 * 
 * This function stores commands and data in the transmit FIFO and increments the FIFO 
 * write pointer.
 * 
 * @param baseAddr  base address of the I2C module
 * @param cmd  command for the I2C master
 * @param data  data for the I2C master
 */
static inline void I2C_Cmd_MasterTransmit(I2C_Type *baseAddr, i2c_master_command_t cmd, uint8_t data)
{
    baseAddr->MDATA = ((uint32_t) cmd << 8U) + (uint32_t) data;
}

/*!
 * @brief Return the received data
 * 
 * This function returns data received by the I2C master that has not been discarded 
 * due to data match settings or active command, and increments the FIFO read pointer.
 * 
 * @param baseAddr  base address of the I2C module
 * @return  data received by the I2C master
 */
static inline uint8_t I2C_Get_MasterRxData(const I2C_Type *baseAddr)
{
    uint32_t tmp = baseAddr->MDATA;
    tmp = (tmp & I2C_MDATA_DATA_MASK) >> I2C_MDATA_DATA_SHIFT;
    return (uint8_t) tmp;
}

/*!
 * @brief Enable or disable the I2C slave
 * 
 * This function enables or disables the I2C module in slave mode.
 * 
 * @param baseAddr  base address of the I2C module
 * @param enable  specifies whether to enable or disable the I2C slave
 */
static inline void I2C_Set_SlaveEnable(I2C_Type *baseAddr, bool enable)
{
    uint32_t regValue = (uint32_t) baseAddr->SCTRL;
    regValue &= (uint32_t) (~(I2C_SCTRL_SEN_MASK));
    regValue |= (uint32_t)(enable ? 1U : 0U) << I2C_SCTRL_SEN_SHIFT;
    baseAddr->SCTRL = (uint32_t) regValue;
}


#if(I2C_HAS_ULTRA_FAST_MODE)

/*!
 * @brief Check the detection of a FIFO overflow or underflow
 * 
 * This function checks for the occurrence of a slave FIFO overflow or underflow. 
 * This event can only occur if clock stretching is disabled.
 * 
 * @param baseAddr  base address of the I2C module
 * @return  indication of a FIFO overflow or underflow
 */
static inline bool I2C_Get_SlaveFIFOErrorEvent(const I2C_Type *baseAddr)
{
    uint32_t regValue = (uint32_t) baseAddr->SSTS;
    regValue = (regValue & I2C_SSTS_OPERRIF_MASK) >> I2C_SSTS_OPERRIF_SHIFT;
    return (bool) ((regValue != 0U) ? true : false);
}

#endif

/*!
 * @brief Check the detection of a bit error
 * 
 * This function checks for the occurrence of a bit error event. This event occurs
 * if the I2C slave transmits a logic one and detects a logic zero on the I2C bus. The
 * slave will ignore the rest of the transfer until the next (repeated) START condition.
 * 
 * @param baseAddr  base address of the I2C module
 * @return  indication of a bit error
 */
static inline bool I2C_Get_SlaveBitErrorEvent(const I2C_Type *baseAddr)
{
    uint32_t regValue = (uint32_t) baseAddr->SSTS;
    regValue = (regValue & I2C_SSTS_ARBIF_MASK) >> I2C_SSTS_ARBIF_SHIFT;
    return (bool) ((regValue != 0U) ? true : false);
}

/*!
 * @brief Check the detection of a STOP condition
 * 
 * This function checks for the detection of a STOP condition, after the I2C slave 
 * matched the last address byte.
 * 
 * @param baseAddr  base address of the I2C module
 * @return  indication of a STOP condition
 */
static inline bool I2C_Get_SlaveSTOPDetectEvent(const I2C_Type *baseAddr)
{
    uint32_t regValue = (uint32_t) baseAddr->SSTS;
    regValue = (regValue & I2C_SSTS_STOPIF_MASK) >> I2C_SSTS_STOPIF_SHIFT;
    return (bool) ((regValue != 0U) ? true : false);
}

/*!
 * @brief Check the detection of a repeated START condition
 * 
 * This function checks for the detection of a repeated START condition, after 
 * the I2C slave matched the last address byte. This event does not occur
 * when the slave first detects a START condition.
 * 
 * @param baseAddr  base address of the I2C module
 * @return  indication of a repeated START condition
 */
static inline bool I2C_Get_SlaveRepeatedStartEvent(const I2C_Type *baseAddr)
{
    uint32_t regValue = (uint32_t) baseAddr->SSTS;
    regValue = (regValue & I2C_SSTS_RSIF_MASK) >> I2C_SSTS_RSIF_SHIFT;
    return (bool) ((regValue != 0U) ? true : false);
}

/*!
 * @brief Check the validity of the Address Status Register
 * 
 * This function checks for the detection of a valid address. The event is 
 * cleared by reading the address - see function I2C_Get_SlaveReceivedAddr().
 * It can also be cleared by reading the data register, when data register has 
 * been configured to allow address reads.
 * 
 * @param baseAddr  base address of the I2C module
 * @return  indication of the validity of the Address Status Register
 */
static inline bool I2C_Get_SlaveAddressValidEvent(const I2C_Type *baseAddr)
{
    uint32_t regValue = (uint32_t) baseAddr->SSTS;
    regValue = (regValue & I2C_SSTS_ADDRIF_MASK) >> I2C_SSTS_ADDRIF_SHIFT;
    return (bool) ((regValue != 0U) ? true : false);
}

/*!
 * @brief Check the availability of receive data
 * 
 * This function checks for the availability of data received by the I2C slave.
 * The event is cleared by reading the received data - see function 
 * I2C_Get_SlaveData(). The event is not cleared by calling
 * I2C_Get_SlaveData() if the data register is configured to allow address
 * reads and an address valid event is active.
 * 
 * @param baseAddr  base address of the I2C module
 * @return  indication of receive data availability
 */
static inline bool I2C_Get_SlaveReceiveDataEvent(const I2C_Type *baseAddr)
{
    uint32_t regValue = (uint32_t) baseAddr->SSTS;
    regValue = (regValue & I2C_SSTS_RXIF_MASK) >> I2C_SSTS_RXIF_SHIFT;
    return (bool) ((regValue != 0U) ? true : false);
}

/*!
 * @brief Check if transmit data is requested
 * 
 * This function checks if the I2C slave requests data to transmit. The
 * event is cleared by providing transmit data - see function 
 * I2C_Transmit_SlaveData(). The event can also be automatically cleared
 * if the I2C module detects a NACK or a repeated START or STOP condition
 * 
 * @param baseAddr  base address of the I2C module
 * @return  indication of a transmit data request
 */
static inline bool I2C_Get_SlaveTransmitDataEvent(const I2C_Type *baseAddr)
{
    uint32_t regValue = (uint32_t) baseAddr->SSTS;
    regValue = (regValue & I2C_SSTS_TXIF_MASK) >> I2C_SSTS_TXIF_SHIFT;
    return (bool) ((regValue != 0U) ? true : false);
}

#if(I2C_HAS_ULTRA_FAST_MODE)

/*!
 * @brief Clear the FIFO overflow or underflow flag
 * 
 * This function clears the FIFO overflow or underflow event.
 * 
 * @param baseAddr  base address of the I2C module
 */
static inline void I2C_Clear_SlaveFIFOErrorEvent(I2C_Type *baseAddr)
{
    baseAddr->SSTS = ((uint32_t) 1U << I2C_SSTS_OPERRIF_SHIFT);
}

#endif

/*!
 * @brief Clear bit error flag
 * 
 * This function clears the bit error event.
 * 
 * @param baseAddr  base address of the I2C module
 */
static inline void I2C_Clear_SlaveBitErrorEvent(I2C_Type *baseAddr)
{
    baseAddr->SSTS = ((uint32_t) 1U << I2C_SSTS_ARBIF_SHIFT);
}

/*!
 * @brief Clear the STOP detect flag
 * 
 * This function clears the STOP detect event.
 * 
 * @param baseAddr  base address of the I2C module
 */
static inline void I2C_Clear_SlaveSTOPDetectEvent(I2C_Type *baseAddr)
{
    baseAddr->SSTS = ((uint32_t) 1U << I2C_SSTS_STOPIF_SHIFT);
}

/*!
 * @brief Clear the repeated START detect flag
 * 
 * This function clears the repeated START detect event.
 * 
 * @param baseAddr  base address of the I2C module
 */
static inline void I2C_Clear_SlaveRepeatedStartEvent(I2C_Type *baseAddr)
{
    baseAddr->SSTS = ((uint32_t) 1U << I2C_SSTS_RSIF_SHIFT);
}

/*!
 * @brief Enable or disable specified I2C slave interrupts
 * 
 * This function can enable or disable one or more slave interrupt sources 
 * specified by the interrupts parameter.
 * 
 * @param baseAddr  base address of the I2C module
 * @param interrupts  interrupts to be enabled or disabled; 
 *  must be a bitwise or between one or more of the following constants: 
 *  - I2C_SLAVE_SMBUS_ALERT_RESPONSE  - SMBus Alert Response Interrupt
 *  - I2C_SLAVE_GENERAL_CALL          - General Call Interrupt
 *  - I2C_SLAVE_ADDRESS_MATCH_1       - Address Match 1 Interrupt
 *  - I2C_SLAVE_ADDRESS_MATCH_0       - Address Match 0 Interrupt
 *  - I2C_SLAVE_FIFO_ERROR            - FIFO Error Interrupt
 *  - I2C_SLAVE_BIT_ERROR             - Bit Error Interrupt
 *  - I2C_SLAVE_STOP_DETECT           - STOP Detect Interrupt
 *  - I2C_SLAVE_REPEATED_START        - Repeated Start Interrupt
 *  - I2C_SLAVE_TRANSMIT_ACK          - Transmit ACK Interrupt
 *  - I2C_SLAVE_ADDRESS_VALID         - Address Valid Interrupt
 *  - I2C_SLAVE_RECEIVE_DATA          - Receive Data Interrupt
 *  - I2C_SLAVE_TRANSMIT_DATA         - Transmit Data Interrupt
 * @param enable  specifies whether to enable or disable specified interrupts
 */
static inline void I2C_Set_SlaveInt(I2C_Type *baseAddr, uint32_t interrupts, bool enable)
{
    uint32_t tmp = baseAddr->SIE;

    if (enable == true)
    {
        tmp |= interrupts;
    } else
    {
        tmp &= ~interrupts;
    }
    baseAddr->SIE = tmp;
}

/*!
 * @brief Return the state of the specified I2C slave interrupt
 * 
 * This function returns the enabled/disabled state of the slave interrupt 
 * source specified by the interrupt parameter.
 * 
 * @param baseAddr  base address of the I2C module
 * @param interrupts  interrupt for which the check is made; 
 *  must be one of the following constants:
 *  - I2C_SLAVE_SMBUS_ALERT_RESPONSE  - SMBus Alert Response Interrupt
 *  - I2C_SLAVE_GENERAL_CALL          - General Call Interrupt
 *  - I2C_SLAVE_ADDRESS_MATCH_1       - Address Match 1 Interrupt
 *  - I2C_SLAVE_ADDRESS_MATCH_0       - Address Match 0 Interrupt
 *  - I2C_SLAVE_FIFO_ERROR            - FIFO Error Interrupt
 *  - I2C_SLAVE_BIT_ERROR             - Bit Error Interrupt
 *  - I2C_SLAVE_STOP_DETECT           - STOP Detect Interrupt
 *  - I2C_SLAVE_REPEATED_START        - Repeated Start Interrupt
 *  - I2C_SLAVE_TRANSMIT_ACK          - Transmit ACK Interrupt
 *  - I2C_SLAVE_ADDRESS_VALID         - Address Valid Interrupt
 *  - I2C_SLAVE_RECEIVE_DATA          - Receive Data Interrupt
 *  - I2C_SLAVE_TRANSMIT_DATA         - Transmit Data Interrupt
 * @return  enable/disable state of specified interrupt
 */
static inline bool I2C_Get_SlaveInt(const I2C_Type *baseAddr, uint32_t interrupts)
{
    uint32_t tmp = baseAddr->SIE;
    bool hasInterrupts = false;

    if ((tmp & interrupts) != (uint32_t) 0U)
    {
        hasInterrupts = true;
    }

    return hasInterrupts;
}

/*!
 * @brief Enable/disable slave receive data DMA requests
 * 
 * This function enables or disables generation of Rx DMA requests when received
 * data is available.
 * 
 * @param baseAddr  base address of the I2C module
 * @param enable  specifies whether to enable or disable receive data DMA requests
 */
static inline void I2C_Set_SlaveRxDMA(I2C_Type *baseAddr, bool enable)
{
    uint32_t regValue = (uint32_t) baseAddr->SCTRL;
    regValue &= (uint32_t) (~(I2C_SCTRL_RXDMAEN_MASK));
    regValue |= (uint32_t)(enable ? 1U : 0U) << I2C_SCTRL_RXDMAEN_SHIFT;
    baseAddr->SCTRL = (uint32_t) regValue;
}

/*!
 * @brief Enable/disable slave transmit data DMA requests
 * 
 * This function enables or disables generation of Tx DMA requests when the module
 * requires more data to transmit.
 * 
 * @param baseAddr  base address of the I2C module
 * @param enable  specifies whether to enable or disable transmit data DMA requests
 */
static inline void I2C_Set_SlaveTxDMA(I2C_Type *baseAddr, bool enable)
{
    uint32_t regValue = (uint32_t) baseAddr->SCTRL;
    regValue &= (uint32_t) (~(I2C_SCTRL_TXDMAEN_MASK));
    regValue |= (uint32_t)(enable ? 1U : 0U) << I2C_SCTRL_TXDMAEN_SHIFT;
    baseAddr->SCTRL = (uint32_t) regValue;
}

/*!
 * @brief Control address match configuration
 * 
 * This function configures the condition that will cause an address match to 
 * occur. See type i2c_slave_addr_config_t for a description of available options.
 * 
 * @param baseAddr  base address of the I2C module
 * @param configuration  configures the condition that will cause an address to match
 */
static inline void I2C_Set_SlaveAddrConfig(I2C_Type *baseAddr, i2c_slave_addr_config_t configuration)
{
    uint32_t tmp = baseAddr->SCTRL;
    tmp &= ~(I2C_SCTRL_ADDRCFG_MASK);
    tmp |= I2C_SCTRL_ADDRCFG(configuration);
    baseAddr->SCTRL = tmp;
}

/*!
 * @brief Control detection of the High-speed Mode master code
 * 
 * This function enables or disables the detection of the High-speed Mode 
 * master code of slave address 0000_1XX, but does not cause an address match 
 * on this code. When set and any Hs-mode master code is detected, the slave 
 * filter and ACK stalls are disabled until the next STOP condition is detected.
 * 
 * @param baseAddr  base address of the I2C module
 * @param enable  enable/disable the detection of the High-speed Mode master code
 */
static inline void I2C_Set_SlaveHighSpeedModeDetect(I2C_Type *baseAddr, bool enable)
{
    uint32_t regValue = (uint32_t) baseAddr->SCTRL;
    regValue &= (uint32_t) (~(I2C_SCTRL_HSEN_MASK));
    regValue |= (uint32_t)(enable ? 1U : 0U) << I2C_SCTRL_HSEN_SHIFT;
    baseAddr->SCTRL = (uint32_t) regValue;
}

/*!
 * @brief Control slave behaviour when NACK is detected
 * 
 * This function controls the option to ignore received NACKs. When enabled, the 
 * I2C slave will continue transfers after a NACK is detected. This option is needed
 * for Ultra-Fast mode.
 * 
 * @param baseAddr  base address of the I2C module
 * @param nack_config  slave behaviour when NACK is detected
 */
static inline void I2C_Set_SlaveIgnoreNACK(I2C_Type *baseAddr, i2c_slave_nack_config_t nack_config)
{
    uint32_t regValue = (uint32_t) baseAddr->SCTRL;
    regValue &= (uint32_t) (~(I2C_SCTRL_IGACK_MASK));
    regValue |= ((uint32_t)nack_config) << I2C_SCTRL_IGACK_SHIFT;
    baseAddr->SCTRL = (uint32_t) regValue;
}

/*!
 * @brief Enable or disable clock stretching for the sending of the ACK bit
 * 
 * This function enables or disables SCL clock stretching during slave-transmit address 
 * byte(s) and slave-receiver address and data byte(s) to allow software to write the 
 * Transmit ACK Register before the ACK or NACK is transmitted. Clock stretching occurs 
 * when transmitting the 9th bit and is therefore not compatible with high speed mode.
 * 
 * @param baseAddr  base address of the I2C module
 * @param enable  enable or disable clock stretching
 */
static inline void I2C_Set_SlaveACKStall(I2C_Type *baseAddr, bool enable)
{
    uint32_t regValue = (uint32_t) baseAddr->SCTRL;
    regValue &= (uint32_t) (~(I2C_SCTRL_ACKSTALL_MASK));
    regValue |= (uint32_t)(enable ? 1U : 0U) << I2C_SCTRL_ACKSTALL_SHIFT;
    baseAddr->SCTRL = (uint32_t) regValue;
}

/*!
 * @brief Enable or disable clock stretching for data transmission
 * 
 * This function enables or disables SCL clock stretching when the transmit data 
 * flag is set during a slave-transmit transfer. Clock stretching occurs following 
 * the 9th bit and is therefore compatible with high speed mode.
 * 
 * @param baseAddr  base address of the I2C module
 * @param enable  enable or disable clock stretching
 */
static inline void I2C_Set_SlaveTXDStall(I2C_Type *baseAddr, bool enable)
{
    uint32_t regValue = (uint32_t) baseAddr->SCTRL;
    regValue &= (uint32_t) (~(I2C_SCTRL_TXSTALL_MASK));
    regValue |= (uint32_t)(enable ? 1U : 0U) << I2C_SCTRL_TXSTALL_SHIFT;
    baseAddr->SCTRL = (uint32_t) regValue;
}

/*!
 * @brief Enable or disable clock stretching for data reception
 * 
 * This function enables or disables SCL clock stretching when receive data flag 
 * is set during a slave-receive transfer. Clock stretching occurs following the 9th
 * bit and is therefore compatible with high speed mode.
 * 
 * @param baseAddr  base address of the I2C module
 * @param enable  enable or disable clock stretching
 */
static inline void I2C_Set_SlaveRXStall(I2C_Type *baseAddr, bool enable)
{
    uint32_t regValue = (uint32_t) baseAddr->SCTRL;
    regValue &= (uint32_t) (~(I2C_SCTRL_RXSTALL_MASK));
    regValue |= (uint32_t)(enable ? 1U : 0U) << I2C_SCTRL_RXSTALL_SHIFT;
    baseAddr->SCTRL = (uint32_t) regValue;
}

/*!
 * @brief Enable or disable clock stretching for valid address reception
 * 
 * This function enables or disables SCL clock stretching when the address valid 
 * flag is asserted. Clock stretching only occurs following the 9th bit and is 
 * therefore compatible with high speed mode.
 * 
 * @param baseAddr  base address of the I2C module
 * @param enable  enable or disable clock stretching
 */
static inline void I2C_Set_SlaveAddrStall(I2C_Type *baseAddr, bool enable)
{
    uint32_t regValue = (uint32_t) baseAddr->SCTRL;
    regValue &= (uint32_t) (~(I2C_SCTRL_ADRSTALL_MASK));
    regValue |= (uint32_t)(enable ? 1U : 0U) << I2C_SCTRL_ADRSTALL_SHIFT;
    baseAddr->SCTRL = (uint32_t) regValue;
}

/*!
 * @brief Configure the ADDR0 address for slave address match
 * 
 * This function configures the ADDR0 value which is used to validate the received 
 * slave address. In 10-bit mode, the first address byte is compared to 
 * { 11110, ADDR0[10:9] } and the second address byte is compared to ADDR0[8:1]. 
 * In 7-bit mode, the address is compared to ADDR0[7:1]
 * The formula used for address validation is configured with function 
 * I2C_Set_SlaveAddrConfig().
 * 
 * @param baseAddr  base address of the I2C module
 * @param addr  ADDR0 address for slave address match
 */
static inline void I2C_Set_SlaveAddr0(I2C_Type *baseAddr, uint16_t addr)
{
    uint32_t tmp = baseAddr->SADDR;
    tmp &= ~(I2C_SADDR_ADDRA_MASK);
    tmp |= I2C_SADDR_ADDRA(addr);
    baseAddr->SADDR = tmp;
}

/*!
 * @brief Return the received slave address
 * 
 * This function returns the received slave address. Reading the address clears 
 * the address valid event. The address can be 7-bit or 10-bit (10-bit addresses 
 * are prefixed by 11110) and includes the R/W bit in the least significant position.
 * 
 * @param baseAddr  base address of the I2C module
 * @return  received address
 */
static inline uint16_t I2C_Get_SlaveReceivedAddr(const I2C_Type *baseAddr)
{
    uint32_t tmp = baseAddr->SRADDR;
    tmp = (tmp & I2C_SRADDR_ADDR_MASK) >> I2C_SRADDR_ADDR_SHIFT;
    return (uint16_t) tmp;
}

/*!
 * @brief Configure the ACK/NACK transmission after a received byte
 * 
 * This function can be used to instruct the I2C slave whether to send an ACK or 
 * a NACK after receiving a byte. When ACK stall is enabled this function must be 
 * called after each matching address and after each received data byte. It can also 
 * be called when I2C Slave is disabled or idle to configure the default ACK/NACK.
 * 
 * @param baseAddr  base address of the I2C module
 * @param nack  specifies whether to transmit ACK or NACK
 */
static inline void I2C_Set_SlaveTransmitNACK(I2C_Type *baseAddr, i2c_slave_nack_transmit_t nack)
{
    uint32_t regValue = (uint32_t) baseAddr->SACK;
    regValue &= (uint32_t) (~(I2C_SACK_NACK_MASK));
    regValue |= ((uint32_t)nack) << I2C_SACK_NACK_SHIFT;
    baseAddr->SACK = (uint32_t) regValue;
}

/*!
 * @brief Provide data for the I2C slave transmitter
 * 
 * This function provides one byte of data for the I2C slave to transmit. 
 * Calling this function clears the transmit data event.
 * 
 * @param baseAddr  base address of the I2C module
 * @param data  data for the I2C slave transmitter
 */
static inline void I2C_Transmit_SlaveData(I2C_Type *baseAddr, uint8_t data)
{
    baseAddr->SDATA = (uint32_t) data;
}

/*!
 * @brief Return the data received by the I2C slave receiver
 * 
 * This function returns the data received by the I2C slave.
 * Calling this function clears the receive data event.
 * 
 * @param baseAddr  base address of the I2C module
 * @return  data received by the I2C slave receiver
 */
static inline uint8_t I2C_Get_SlaveData(const I2C_Type *baseAddr)
{
    uint32_t tmp = baseAddr->SDATA;
    tmp = (tmp & I2C_SDATA_DATA_MASK) >> I2C_SDATA_DATA_SHIFT;
    return (uint8_t) tmp;
}

/*!
 * @brief Set the idle timeout period
 * 
 * This function is to configure the number of cycles that the line idle flag is 
 * set after detecting that SDA and SCL are high.
 * 
 * @param baseAddr  base address of the I2C module
 * @param value  timeout period
 */
static inline void I2C_Set_MasterTimeoutPeriod(I2C_Type *baseAddr, uint16_t value)
{
    uint32_t tmp = baseAddr->TOCFG;
    tmp &= ~(I2C_TOCFG_IDLE_MASK);
    tmp |= I2C_TOCFG_IDLE(value);
    baseAddr->TOCFG = tmp;
}

/*!
 * @brief Get the idle timeout period
 * 
 * This function is to Get the number of cycles that the line idle flag is 
 * set after detecting that SDA and SCL are high.
 * 
 * @param baseAddr  base address of the I2C module
 */
static inline uint32_t I2C_Get_MasterTimeoutPeriod(I2C_Type *baseAddr)
{
    uint32_t tmp = baseAddr->TOCFG;
    tmp &= (I2C_TOCFG_IDLE_MASK);
    tmp >>= I2C_TOCFG_IDLE_SHIFT;
    return tmp;
}

/*!
 * @brief Set the pin low timeout period
 * 
 * This function is to configure the cycles of line low timeout.
 * 
 * @param baseAddr  base address of the I2C module
 * @param value  timeout period
 */
static inline void I2C_Set_MasterLineLowTimeoutPeriod(I2C_Type *baseAddr, uint16_t value)
{
    baseAddr->TOCFG &= ~(I2C_TOCFG_LOW_MASK);
    baseAddr->TOCFG |= I2C_TOCFG_LOW(value);
}

/*!
 * @brief Set SDA and SCL line low detection.
 *
 * @param baseAddr  base address of the I2C module
 */
static inline void I2C_Set_MasterLineLowDetect(I2C_Type *baseAddr)
{
    baseAddr->TOCFG &= ~(I2C_TOCFG_SDA_MASK);
    baseAddr->TOCFG |= I2C_TOCFG_SDA_MASK;
}




/*! @}*/

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* __I2C_HW_ACCESS_H__*/
/*******************************************************************************
 * EOF
 ******************************************************************************/

