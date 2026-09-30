/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file i2c_driver.c
 * @version 1.4.0
 */

/*!
* @page misra_violations MISRA-C:2012 violations list
*
* PRQA S 0311 Rule 11.8: Dangerous pointer cast results in loss of const qualification.
*
* PRQA S 0326 Rule 11.6: Cast between a pointer to void and an integral type.
*
*/

#include "i2c_driver.h"
#include "interrupt_manager.h"
#include "i2c_hw_access.h"
#include "clock_manager.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*! @cond DRIVER_INTERNAL_USE_ONLY */

/* Constraints used for baud rate computation */
#define CLKHI_MIN_VALUE 1U
#define CLKLO_MIN_VALUE 3U
#define CLKHI_MAX_VALUE ((1U << I2C_MCLKCFG_HIGH_WIDTH) - 1U)
#define CLKLO_MAX_VALUE CLKHI_MAX_VALUE
#define DATAVD_MIN_VALUE 1U
#define SETHOLD_MIN_VALUE 2U

/* Table of base addresses for I2C instances. */
static I2C_Type *const g_i2cBase[I2C_INSTANCE_COUNT] = I2C_BASE_PTRS;

/* I2C DMA request sources */
static const uint8_t g_i2cDMASrc[I2C_INSTANCE_COUNT][2] = I2C_DMA_REQ;
/* I2C Slave support */
static const bool g_i2cSupportSlave[I2C_INSTANCE_COUNT] = I2C_SLAVE_SUPPORT;

/* Pointer to runtime state structure.*/
static i2c_master_state_t *g_i2cMasterStatePtr[I2C_INSTANCE_COUNT] = I2C_STATE_PTR;
static i2c_slave_state_t *g_i2cSlaveStatePtr[I2C_INSTANCE_COUNT] = I2C_STATE_PTR;

/* Table for i2c IRQ numbers */
static const IRQn_Type g_i2cMasterIrqId[I2C_MASTER_IRQS_CH_COUNT] = I2C_MASTER_IRQS;
static const IRQn_Type g_i2cSlaveIrqId[I2C_SLAVE_IRQS_CH_COUNT] = I2C_SLAVE_IRQS;

/* PCC clock sources, for getting the input clock frequency */
static const clock_names_t g_i2cClock[I2C_INSTANCE_COUNT] = I2C_CLOCK_NAMES;

/*******************************************************************************
 * Private Functions
 ******************************************************************************/
/* Callback for master DMA transfer done.*/
static void I2C_DRV_MasterCompletDMATransfer(void *parameter, dma_chn_status_t status);
/* Static function for I2C initialization .*/
static void I2C_Init(uint32_t instance);


/*! @brief Direction of a I2C transfer - transmit or receive. */
typedef enum
{
    I2C_TX_REQ = 0,    /*!< The driver will perform an I2C transmit transfer */
    I2C_RX_REQ = 1,    /*!< The driver will perform an I2C receive transfer */
} i2c_transfer_direction_t;

/*!
 * @brief DMA internal parameters structure
 *
 * This structure is used in DMA transfers. It contains different
 * variables required for setting and maintaining a DMA transfer.
 */
typedef struct
{
    /*! @cond DRIVER_INTERNAL_USE_ONLY */
    uint8_t dmaChannel;                             /* Channel number for the DMA channel */
    dma_transfer_type_t dmaTransferType;           /* Type for the DMA transfer */
    uint32_t i2cDataRegAddr;                        /* An i2c data register address */
    uint8_t *bufferTransfer;                        /* Buffer used for transfer */
    uint32_t transferSize;                          /* Size of the data to be transferred */
    i2c_transfer_direction_t transferDirection;   /* Tells if the driver will make a reception or transmit DMA transfer */
    /*! @endcond */
} i2c_dma_transfer_params_t;


/*******************************************************************************
 * Private Functions
 ******************************************************************************/


/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_MasterCmdQueueEmpty
 * Description   : checks if there are any commands in the master software queue
 *
 *END**************************************************************************/
static inline bool I2C_DRV_MasterCmdQueueEmpty(const i2c_master_state_t *master)
{
    DEV_ASSERT(master != NULL);

    return (master->cmdQueue.writeIdx == master->cmdQueue.readIdx);
}


/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_MasterResetQueue
 * Description   : resets the master software queue
 *
 *END**************************************************************************/
static inline void I2C_DRV_MasterResetQueue(i2c_master_state_t *master)
{
    DEV_ASSERT(master != NULL);

    master->cmdQueue.readIdx = 0U;
    master->cmdQueue.writeIdx = 0U;
}


/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_MasterQueueCmd
 * Description   : queues a command in the hardware FIFO or in the master software queue
 *
 *END**************************************************************************/
static inline void I2C_DRV_MasterQueueCmd(I2C_Type *baseAddr,
                                          i2c_master_state_t *master,
                                          i2c_master_command_t cmd,
                                          uint8_t data)
{
    DEV_ASSERT(master != NULL);
    DEV_ASSERT(baseAddr != NULL);

    uint16_t txFIFOCount = I2C_Get_MasterTxFIFOCount(baseAddr);
    uint16_t txFIFOSize = I2C_Get_MasterTxFIFOSize(baseAddr);

    /* Check if there is room in the hardware FIFO */
    if (txFIFOCount < txFIFOSize)
    {
        I2C_Cmd_MasterTransmit(baseAddr, cmd, data);
    } else
    {
        /* Hardware FIFO full, use software FIFO */
        DEV_ASSERT(master->cmdQueue.writeIdx < I2C_MASTER_CMD_QUEUE_SIZE);

        master->cmdQueue.cmd[master->cmdQueue.writeIdx] = cmd;
        master->cmdQueue.data[master->cmdQueue.writeIdx] = data;
        master->cmdQueue.writeIdx++;
    }
}


/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_MasterSendQueuedCmd
 * Description   : transfers commands from the master software queue to the hardware FIFO
 *
 *END**************************************************************************/
static inline void I2C_DRV_MasterSendQueuedCmd(I2C_Type *baseAddr, i2c_master_state_t *master)
{
    DEV_ASSERT(master != NULL);
    DEV_ASSERT(baseAddr != NULL);

    uint16_t txFIFOCount = I2C_Get_MasterTxFIFOCount(baseAddr);
    uint16_t txFifoSize = I2C_Get_MasterTxFIFOSize(baseAddr);

    while ((!I2C_DRV_MasterCmdQueueEmpty(master)) && (txFIFOCount < txFifoSize))
    {
        I2C_Cmd_MasterTransmit(baseAddr,
                               master->cmdQueue.cmd[master->cmdQueue.readIdx],
                               master->cmdQueue.data[master->cmdQueue.readIdx]);
        master->cmdQueue.readIdx++;

        txFIFOCount = I2C_Get_MasterTxFIFOCount(baseAddr);
    }

    if (I2C_DRV_MasterCmdQueueEmpty(master))
    {
        /* Reset queue */
        I2C_DRV_MasterResetQueue(master);
    }
}


/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_MasterSendAddress
 * Description   : send start event and slave address
 *                 parameter receive specifies the direction of the transfer
 *
 *END**************************************************************************/
static void I2C_DRV_MasterSendAddress(I2C_Type *baseAddr,
                                      i2c_master_state_t *master,
                                      bool receive)
{
    uint8_t addrByte;
    i2c_master_command_t startCommand;

    DEV_ASSERT(master != NULL);
    DEV_ASSERT(baseAddr != NULL);

#if(I2C_HAS_HIGH_SPEED_MODE)
    if ((master->operatingMode == I2C_HIGHSPEED_MODE) && (master->highSpeedInProgress == false))
    {
        /* Initiating High-speed mode - send master code first */
        I2C_DRV_MasterQueueCmd(baseAddr, master, I2C_MASTER_COMMAND_START_NACK, master->masterCode);
        master->highSpeedInProgress = true;
    }

    if (master->highSpeedInProgress == true)
    {
        /* Use high-speed settings after start event in High Speed mode */
        startCommand = I2C_MASTER_COMMAND_START_HS;
    } 
    else
#endif
    {
        /* Normal START command */
        startCommand = I2C_MASTER_COMMAND_START;
    }

    if (master->is10bitAddr)
    {
        /* 10-bit addressing */
        /* First address byte: 1111 0XXD, where XX are bits 10 and 9 of address, and D = 0(transmit) */
        addrByte = (uint8_t) (0xF0U + ((master->slaveAddress >> 7U) & 0x6U));
        I2C_DRV_MasterQueueCmd(baseAddr, master, startCommand, addrByte);
        /* Second address byte: Remaining 8 bits of address */
        addrByte = (uint8_t) (master->slaveAddress & 0xFFU);
        I2C_DRV_MasterQueueCmd(baseAddr, master, I2C_MASTER_COMMAND_TRANSMIT, addrByte);
        if (receive == true)
        {
            /* Receiving from 10-bit slave - must send repeated start and resend first address byte */
            /* First address byte: 1111 0XXD, where XX are bits 10 and 9 of address, and D = 1 (receive) */
            addrByte = (uint8_t) (0xF0U + ((master->slaveAddress >> 7U) & 0x6U) + (uint8_t) 1U);
            I2C_DRV_MasterQueueCmd(baseAddr, master, startCommand, addrByte);
        }
    } else
    {
        /* 7-bit addressing */
        /* Address byte: slave 7-bit address + D = 0(transmit) or 1 (receive) */
        addrByte = (uint8_t) ((master->slaveAddress << 1U) + (receive ? 1U : 0U));
        I2C_DRV_MasterQueueCmd(baseAddr, master, startCommand, addrByte);
    }
}


/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_MasterQueueData
 * Description   : queues transmit data in the I2C tx fifo until it is full
 *
 *END**************************************************************************/
static void I2C_DRV_MasterQueueData(I2C_Type *baseAddr,
                                    i2c_master_state_t *master)
{
    DEV_ASSERT(master != NULL);
    DEV_ASSERT(baseAddr != NULL);

    uint16_t txFIFOCount = I2C_Get_MasterTxFIFOCount(baseAddr);
    uint16_t txFifoSize = I2C_Get_MasterTxFIFOSize(baseAddr);

    /* Don't queue any data if there are commands in the software queue */
    if (I2C_DRV_MasterCmdQueueEmpty(master))
    {
        while ((master->txSize > 0U) && (txFIFOCount < txFifoSize))
        {
            I2C_Cmd_MasterTransmit(baseAddr, I2C_MASTER_COMMAND_TRANSMIT, master->txBuff[0U]);
            master->txBuff++;
            master->txSize--;

            txFIFOCount = I2C_Get_MasterTxFIFOCount(baseAddr);
        }
    }
}


/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_MasterEndTransfer
 * Description   : ends current transmission or reception
 *
 *END**************************************************************************/
static void I2C_DRV_MasterEndTransfer(I2C_Type *baseAddr,
                                      i2c_master_state_t *master,
                                      bool sendStop,
                                      bool resetFIFO)
{
    DEV_ASSERT(master != NULL);
    DEV_ASSERT(baseAddr != NULL);

    /* Disable all events */
    I2C_Set_MasterInt(baseAddr, I2C_MASTER_FIFO_ERROR_INT |
                                I2C_MASTER_PIN_LOW_TIMEOUT_INT |
                                I2C_MASTER_ARBITRATION_LOST_INT |
                                I2C_MASTER_NACK_DETECT_INT |
                                I2C_MASTER_TRANSMIT_DATA_INT |
                                I2C_MASTER_RECEIVE_DATA_INT,
                      false);

    if (master->transferType == I2C_USING_DMA)
    {
        /* Stop DMA channel */
        (void) DMA_DRV_StopChannel(master->dmaChannel);

        /* Disable I2C DMA request. */
        if (master->rxSize != (uint16_t) 0)
        {
            (void) I2C_Set_MasterRxDMA(baseAddr, false);
        } else
        {
            (void) I2C_Set_MasterTxDMA(baseAddr, false);
        }
    }

    if (resetFIFO == true)
    {
        /* Reset FIFOs if requested */
        I2C_Reset_MasterTxFIFOCmd(baseAddr);
        I2C_Reset_MasterRxFIFOCmd(baseAddr);
    }

    /* Queue STOP command if requested */
    if (sendStop == true)
    {
        I2C_Cmd_MasterTransmit(baseAddr, I2C_MASTER_COMMAND_STOP, 0U);
#if(I2C_HAS_HIGH_SPEED_MODE)
        master->highSpeedInProgress = false; /* High-speed transfers end at STOP condition */
#endif
    }

    master->txBuff = NULL;
    master->txSize = 0;
    master->rxBuff = NULL;
    master->rxSize = 0;
    master->i2cIdle = true;
}


/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_SlaveEndTransfer
 * Description   : ends current transmission or reception
 *
 *END**************************************************************************/
static void I2C_DRV_SlaveEndTransfer(I2C_Type *baseAddr,
                                     i2c_slave_state_t *slave)
{
    DEV_ASSERT(slave != NULL);
    DEV_ASSERT(baseAddr != NULL);

    /* Deactivate events */

    I2C_Set_SlaveInt(baseAddr, I2C_SLAVE_BIT_ERROR_INT |
                               I2C_SLAVE_FIFO_ERROR_INT |
                               I2C_SLAVE_STOP_DETECT_INT |
                               I2C_SLAVE_REPEATED_START_INT |
                               I2C_SLAVE_ADDRESS_VALID_INT |
                               I2C_SLAVE_RECEIVE_DATA_INT |
                               I2C_SLAVE_TRANSMIT_DATA_INT,
                     false);

    /* For DMA we must disable the DMA request. */
    if (slave->transferType == I2C_USING_DMA)
    {
        if (slave->rxSize != (uint16_t) 0)
        {
            (void) I2C_Set_SlaveRxDMA(baseAddr, false);
        } else
        {
            (void) I2C_Set_SlaveTxDMA(baseAddr, false);
        }
    }

    /* Disable I2C slave */
    I2C_Set_SlaveEnable(baseAddr, false);

    slave->isTransferInProgress = false;
    slave->rxBuff = NULL;
    slave->rxSize = 0U;
    slave->txBuff = NULL;
    slave->txSize = 0U;
    slave->repeatedStarts = 0U;
}


/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_MasterSetOperatingMode
 * Description   : sets the operating mode of the I2C master
 *
 *END**************************************************************************/
static void I2C_DRV_MasterSetOperatingMode(uint32_t instance, i2c_mode_t operatingMode)
{
    I2C_Type *baseAddr;
    i2c_master_state_t *master;

    baseAddr = g_i2cBase[instance];
    master = g_i2cMasterStatePtr[instance];
    DEV_ASSERT(master != NULL);

#if(I2C_HAS_ULTRA_FAST_MODE)
    if (operatingMode == I2C_ULTRAFAST_MODE)
    {
        I2C_Set_MasterPinConfig(baseAddr, I2C_CFG_2PIN_OUTPUT_ONLY);
        I2C_Set_MasterNACKConfig(baseAddr, I2C_NACK_IGNORE);
    } else
#endif
    {
        I2C_Set_MasterPinConfig(baseAddr, I2C_CFG_2PIN_OPEN_DRAIN);
        I2C_Set_MasterNACKConfig(baseAddr, I2C_NACK_RECEIVE);
    }

    master->operatingMode = operatingMode;
}


/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_SlaveSetOperatingMode
 * Description   : sets the operating mode of the I2C slave
 *
 *END**************************************************************************/
static void I2C_DRV_SlaveSetOperatingMode(uint32_t instance, i2c_mode_t operatingMode)
{
    I2C_Type *baseAddr;
    i2c_slave_state_t *slave;

    baseAddr = g_i2cBase[instance];
    slave = g_i2cSlaveStatePtr[instance];
    DEV_ASSERT(slave != NULL);

#if(I2C_HAS_ULTRA_FAST_MODE)
    if (operatingMode == I2C_ULTRAFAST_MODE)
    {
        I2C_Set_SlaveIgnoreNACK(baseAddr, I2C_SLAVE_NACK_CONTINUE_TRANSFER);
        I2C_Set_SlaveTransmitNACK(baseAddr, I2C_SLAVE_TRANSMIT_NACK);
        /* Disable all clock stretching in ultra-fast mode */
        I2C_Set_SlaveACKStall(baseAddr, false);
        I2C_Set_SlaveTXDStall(baseAddr, false);
        I2C_Set_SlaveRXStall(baseAddr, false);
        I2C_Set_SlaveAddrStall(baseAddr, false);
    } else
#endif
    {
        I2C_Set_SlaveIgnoreNACK(baseAddr, I2C_SLAVE_NACK_END_TRANSFER);
        I2C_Set_SlaveTransmitNACK(baseAddr, I2C_SLAVE_TRANSMIT_ACK);
        /* Enable clock stretching except ACKSTALL (we don't need to send ACK/NACK manually) */
        I2C_Set_SlaveACKStall(baseAddr, false);
        I2C_Set_SlaveTXDStall(baseAddr, true);
        I2C_Set_SlaveRXStall(baseAddr, true);
        I2C_Set_SlaveAddrStall(baseAddr, true);
    }

#if(I2C_HAS_HIGH_SPEED_MODE)
    if (operatingMode == I2C_HIGHSPEED_MODE)
    {
        /* Enable detection of the High-speed Mode master code */
        I2C_Set_SlaveHighSpeedModeDetect(baseAddr, true);
    } else
#endif
    {
        /* Disable detection of the High-speed Mode master code */
        I2C_Set_SlaveHighSpeedModeDetect(baseAddr, false);
    }

    slave->operatingMode = operatingMode;
}


/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_ConfigureDmaTransfer
 * Description   : configures the DMA transfer
 *
 *END**************************************************************************/
static void I2C_DRV_ConfigureDmaTransfer(uint32_t instance, const i2c_dma_transfer_params_t *dmaTransParams)
{
    /* Configure DMA channel */
    if (dmaTransParams->transferDirection == I2C_TX_REQ)
    {
        (void) DMA_DRV_SetChannelRequestAndTrigger(dmaTransParams->dmaChannel, g_i2cDMASrc[instance][I2C_TX_REQ]);
        (void) DMA_DRV_ConfigMultiBlockTransfer(dmaTransParams->dmaChannel, dmaTransParams->dmaTransferType,
                                                (uint32_t) dmaTransParams->bufferTransfer,
                                                (uint32_t) dmaTransParams->i2cDataRegAddr, DMA_TRANSFER_SIZE_1B,
                                                (uint32_t) 1U,
                                                (uint32_t) dmaTransParams->transferSize, false);
    } else
    {
        (void) DMA_DRV_SetChannelRequestAndTrigger(dmaTransParams->dmaChannel, g_i2cDMASrc[instance][I2C_RX_REQ]);
        (void) DMA_DRV_ConfigMultiBlockTransfer(dmaTransParams->dmaChannel, dmaTransParams->dmaTransferType,
                                                (uint32_t) dmaTransParams->i2cDataRegAddr,
                                                (uint32_t) dmaTransParams->bufferTransfer, DMA_TRANSFER_SIZE_1B,
                                                (uint32_t) 1U,
                                                (uint32_t) dmaTransParams->transferSize, false);
    }
}


/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_MasterStartDmaTransfer
 * Description   : starts the DMA transfer for master
 *
 *END**************************************************************************/
static void I2C_DRV_MasterStartDmaTransfer(uint32_t instance)
{
    I2C_Type *baseAddr = g_i2cBase[instance];
    i2c_master_state_t *master = g_i2cMasterStatePtr[instance];
    i2c_dma_transfer_params_t dmaTransParams;
    bool receive = false;

    dmaTransParams.dmaChannel = master->dmaChannel;
    if (master->txSize > 0U)
    {
        /* Configure watermarks for transmit DMA for master */
        uint32_t txBytes = I2C_Get_MasterTxFIFOSize(baseAddr);
        if (txBytes > master->txSize)
        {
            txBytes = master->txSize;
        }
        I2C_Set_MasterTxFIFOWatermark(baseAddr, (uint16_t) (txBytes - 1U));

        dmaTransParams.dmaTransferType = DMA_TRANSFER_MEM2PERIPH;
        dmaTransParams.i2cDataRegAddr = (uint32_t) (&(baseAddr->MDATA));
        dmaTransParams.bufferTransfer = (uint8_t *) master->txBuff; /* PRQA S 0311 */
        dmaTransParams.transferDirection = I2C_TX_REQ;
        dmaTransParams.transferSize = master->txSize;

    } else
    {
        /* Configure watermarks for receive DMA for master */
        I2C_Set_MasterRxFIFOWatermark(baseAddr, 0U);

        receive = true;

        dmaTransParams.dmaTransferType = DMA_TRANSFER_PERIPH2MEM;
        dmaTransParams.i2cDataRegAddr = (uint32_t) (&(baseAddr->MDATA));
        dmaTransParams.bufferTransfer = master->rxBuff;
        dmaTransParams.transferDirection = I2C_RX_REQ;
        dmaTransParams.transferSize = master->rxSize;
    }

    (void) I2C_DRV_ConfigureDmaTransfer(instance, &dmaTransParams);

    /* Disable DMA requests for channel when transfer is done */
    DMA_DRV_DisableRequestsOnTransferComplete(dmaTransParams.dmaChannel, true);

    /* Call callback function when all the bytes were transfered. */
    (void) DMA_DRV_InstallCallback(dmaTransParams.dmaChannel, (I2C_DRV_MasterCompletDMATransfer), (void *) (instance)); /* PRQA S 0326 */

    /* Start channel */
    (void) DMA_DRV_StartChannel(dmaTransParams.dmaChannel);

    I2C_DRV_MasterSendAddress(baseAddr, master, receive);

    /* Enable transmit/receive DMA requests */
    if (master->txSize > (uint32_t) 0U)
    {
        (void) I2C_Set_MasterTxDMA(baseAddr, true);
    } else
    {
        I2C_DRV_MasterQueueCmd(baseAddr, master, I2C_MASTER_COMMAND_RECEIVE, (uint8_t) (master->rxSize - 1U));
        (void) I2C_Set_MasterRxDMA(baseAddr, true);
    }
}


/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_SlaveStartDmaTransfer
 * Description   : starts the DMA transfer for slave
 *
 *END**************************************************************************/
static void I2C_DRV_SlaveStartDmaTransfer(uint32_t instance)
{
    I2C_Type *baseAddr = g_i2cBase[instance];
    const i2c_slave_state_t *slave = g_i2cSlaveStatePtr[instance];
    i2c_dma_transfer_params_t dmaTransParams;

    if (slave->txSize > (uint32_t) 0U)
    {
        dmaTransParams.dmaChannel = slave->dmaChannel;
        dmaTransParams.dmaTransferType = DMA_TRANSFER_MEM2PERIPH;
        dmaTransParams.i2cDataRegAddr = (uint32_t) (&(baseAddr->SDATA));
        dmaTransParams.bufferTransfer = (uint8_t *) slave->txBuff; /* PRQA S 0311 */
        dmaTransParams.transferDirection = I2C_TX_REQ;
        dmaTransParams.transferSize = slave->txSize;
    } else
    {
        dmaTransParams.dmaChannel = slave->dmaChannel;
        dmaTransParams.dmaTransferType = DMA_TRANSFER_PERIPH2MEM;
        dmaTransParams.i2cDataRegAddr = (uint32_t) (&(baseAddr->SDATA));
        dmaTransParams.bufferTransfer = slave->rxBuff;
        dmaTransParams.transferDirection = I2C_RX_REQ;
        dmaTransParams.transferSize = slave->rxSize;
    }

    (void) I2C_DRV_ConfigureDmaTransfer(instance, &dmaTransParams);
    /* Adjustment added to source address at the beginning of TX buffer */
    DMA_DRV_SetSrcLastAddrAdjustment(dmaTransParams.dmaChannel, -(int32_t) (slave->txSize));

    /* Start channel */
    (void) DMA_DRV_StartChannel(dmaTransParams.dmaChannel);

    /* Enable transmit/receive DMA requests */
    if (slave->txSize > (uint32_t) 0U)
    {
        (void) I2C_Set_SlaveTxDMA(baseAddr, true);
    } else
    {
        (void) I2C_Set_SlaveRxDMA(baseAddr, true);
    }
}


/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_MasterCompletDMATransfer
 * Description   : Finish up a transfer DMA for master. The main purpose of
 *                 this function is to create a function compatible with DMA
 *                 callback type
 *
 *END**************************************************************************/
static void I2C_DRV_MasterCompletDMATransfer(void *parameter, dma_chn_status_t status)
{
    I2C_Type *baseAddr;
    i2c_master_state_t *master;

    uint32_t instance = (uint32_t) parameter; /* PRQA S 0326 */

    baseAddr = g_i2cBase[instance];
    master = g_i2cMasterStatePtr[instance];

    if ((master->txSize > 0U) && (status != DMA_CHN_ERROR))
    {
        master->txSize = 0U;

        I2C_Set_MasterTxFIFOWatermark(baseAddr, 0U);

        /* Disable transmit data DMA requests */
        (void) I2C_Set_MasterTxDMA(baseAddr, false);

        /* Activate transmit data events */
        I2C_Set_MasterInt(baseAddr, (uint32_t) I2C_MASTER_TRANSMIT_DATA_INT, true);
    } else
    {
        /* Signal transfer end for blocking transfers */
        I2C_DRV_MasterEndTransfer(baseAddr, master, master->sendStop, false);

        if (master->blocking == true)
        {
            (void) OSIF_SemaPost(&(master->idleSemaphore));
        }

        /* Report status error if an error occurred in DMA channel */
        if (status == DMA_CHN_ERROR)
        {
            master->status = STATUS_ERROR;
        } else
        {
            master->status = STATUS_SUCCESS;
        }

        if (master->masterCallback != NULL)
        {
            master->masterCallback(I2C_MASTER_EVENT_END_TRANSFER, master->callbackParam);
        }
    }
}
/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_Init
 * Description   : This function initializes the I2C module to a known
 * state (use software reset bit to reset the module).
 *
 * Implements : I2C_Init_Activity
 *END**************************************************************************/
static void I2C_Init(uint32_t instance)
{
    DEV_ASSERT(instance < I2C_INSTANCE_COUNT);
    /* ipc software reset */
    CLOCK_DRV_ResetModule(g_i2cClock[instance]);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_MasterReinit
 * Description   : re-initialize the I2C master
 *
 *END**************************************************************************/
static status_t I2C_DRV_MasterReinit(uint32_t instance, i2c_master_state_t *master)
{
    I2C_Type *baseAddr;
    i2c_baud_rate_params_t baudRate;

    baseAddr = g_i2cBase[instance];
    g_i2cMasterStatePtr[instance] = master;

    /* Re-initialize driver status structure */
    master->txBuff = NULL;
    master->txSize = 0;
    master->rxBuff = NULL;
    master->rxSize = 0;
    master->i2cIdle = true;

    I2C_DRV_MasterResetQueue(master);

    /* Reset the i2c module by calling I2C_Init() */
    I2C_Init(instance);

    /* Set baud rate */
    baudRate.baudRate = master->baudrate;
#if(I2C_HAS_HIGH_SPEED_MODE)
    baudRate.baudRateHS = master->baudRateHS;
#endif
    (void) I2C_DRV_MasterSetBaudRate(instance, master->operatingMode, baudRate);

    /* Set slave address */
    I2C_DRV_MasterSetSlaveAddr(instance, master->slaveAddress, master->is10bitAddr);

    /* Enable I2C master */
    I2C_Set_MasterEnable(baseAddr, true);

    return STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_MasterWaitTransferEnd
 * Description   : waits for the end of a blocking transfer
 *
 *END**************************************************************************/
static status_t I2C_DRV_MasterWaitTransferEnd(uint32_t instance, uint32_t timeout)
{
    I2C_Type *baseAddr;
    status_t osifError = STATUS_SUCCESS;
    i2c_master_state_t *master;
    uint16_t rxFifoFill = 0;

    baseAddr = g_i2cBase[instance];
    master = g_i2cMasterStatePtr[instance];

    /* Wait for transfer to be completed by the IRQ */
    osifError = OSIF_SemaWait(&(master->idleSemaphore), timeout);

    if (osifError == STATUS_TIMEOUT)
    {
        /* If master is sending data transfer is aborted now in case timeout occurred */
        if (master->txSize != 0U)
        {
            I2C_DRV_MasterEndTransfer(baseAddr, master, false, true);

            master->status = STATUS_TIMEOUT;
        } else
        {
            if (master->transferType == I2C_USING_DMA)
            {
                /* Stop DMA channel and activate interrupts */
                (void) DMA_DRV_StopChannel(master->dmaChannel);
            }

            /* Disable interrupts to check number of bytes in rx fifo */
            I2C_Set_MasterInt(baseAddr, (uint32_t) I2C_MASTER_RECEIVE_DATA_INT, false);

            /* Check number of bytes in rx fifo */
            rxFifoFill = I2C_Get_MasterRxFIFOCount(baseAddr);

            /* In case both rx size and number of bytes in rx fifo is 0, then the transfer ended successfully */
            if ((rxFifoFill == master->rxSize) && (master->rxSize == 0U))
            {
                master->status = STATUS_SUCCESS;

            }else {

                /* Set watermark to rxFifoFill in case the rx size is grater than the number of bytes in the rx  fifo */
                if (rxFifoFill < master->rxSize)
                {
                    master->abortedTransfer = true;
                    if(rxFifoFill > 0U)
                    {
                        I2C_Set_MasterRxFIFOWatermark(baseAddr, rxFifoFill-1U);
                    } 
                    else 
                    {
                        (void) I2C_DRV_MasterReinit(instance, master);
                        (void) OSIF_SemaPost(&(master->idleSemaphore));
                    }
                    master->status = STATUS_TIMEOUT;
                }
    
                I2C_Set_MasterInt(baseAddr, (uint32_t) I2C_MASTER_RECEIVE_DATA_INT, true);
    
                osifError = OSIF_SemaWait(&(master->idleSemaphore), timeout);
                if (osifError == STATUS_TIMEOUT)
                {
                    (void) I2C_DRV_MasterReinit(instance, master);
                    master->status = STATUS_TIMEOUT;
                }
    
                master->abortedTransfer = false;
            }
        }
    }

    /* Blocking transfer is over */
    master->blocking = false;
    return master->status;
}


/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_SlaveWaitTransferEnd
 * Description   : waits for the end of a blocking transfer
 *
 *END**************************************************************************/
static status_t I2C_DRV_SlaveWaitTransferEnd(uint32_t instance, uint32_t timeout)
{
    status_t osifError = STATUS_SUCCESS;
    I2C_Type *baseAddr;
    i2c_slave_state_t *slave;

    baseAddr = g_i2cBase[instance];
    slave = g_i2cSlaveStatePtr[instance];

    /* Wait for transfer to be completed by the IRQ */
    osifError = OSIF_SemaWait(&(slave->idleSemaphore), timeout);

    if (osifError == STATUS_TIMEOUT)
    {
        I2C_DRV_SlaveEndTransfer(baseAddr, slave);
        slave->status = STATUS_TIMEOUT;
    }

    /* Blocking transfer is over */
    slave->blocking = false;
    return slave->status;
}


/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_MasterHandleTransmitDataRequest
 * Description   : handle a transmit request for master
 *
 *END**************************************************************************/
static void I2C_DRV_MasterHandleTransmitDataRequest(I2C_Type *baseAddr, i2c_master_state_t *master)
{
    /* More data needed for transmission */
    if (!I2C_DRV_MasterCmdQueueEmpty(master))
    {
        /* If there are queued commands, send them */
        I2C_DRV_MasterSendQueuedCmd(baseAddr, master);
    } else if (master->txBuff != NULL)
    {
        /* A transmission is in progress */
        if (master->txSize == 0U)
        {
            /* There is no more data in buffer, the transmission is over */
            I2C_DRV_MasterEndTransfer(baseAddr, master, master->sendStop, false);

            /* Signal transfer end for blocking transfers */
            if (master->blocking == true)
            {
                (void) OSIF_SemaPost(&(master->idleSemaphore));
            }

            master->status = STATUS_SUCCESS;

            if (master->masterCallback != NULL)
            {
                master->masterCallback(I2C_MASTER_EVENT_TX_END, master->callbackParam);
            }
        } else
        {
            /* Queue data bytes to fill tx fifo */
            I2C_DRV_MasterQueueData(baseAddr, master);
        }
    } else
    {
        /* No more commands and no transmission in progress - disable tx event */
        I2C_Set_MasterInt(baseAddr, (uint32_t) I2C_MASTER_TRANSMIT_DATA_INT, false);
    }
}


/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_MasterHandleReceiveDataRequest
 * Description   : handle a receive request for master
 *
 *END**************************************************************************/
static void I2C_DRV_MasterHandleReceiveDataReadyEvent(I2C_Type *baseAddr, i2c_master_state_t *master)
{
    /* Received data ready */
    DEV_ASSERT(master->rxBuff != NULL);

    /* Transfer received data to user buffer */
    while ((I2C_Get_MasterRxFIFOCount(baseAddr) > 0U) && (master->rxSize > 0U))
    {
        master->rxBuff[0U] = I2C_Get_MasterRxData(baseAddr);
        master->rxBuff++;
        master->rxSize--;
    }
    if (master->rxSize == 0U)
    {
        /* Done receiving */
        I2C_DRV_MasterEndTransfer(baseAddr, master, master->sendStop, false);

        /* Signal transfer end for blocking transfers */
        if (master->blocking == true)
        {
            (void) OSIF_SemaPost(&(master->idleSemaphore));
        }

        master->status = STATUS_SUCCESS;

        if (master->masterCallback != NULL)
        {
            master->masterCallback(I2C_MASTER_EVENT_RX_END, master->callbackParam);
        }
    } else if (master->rxSize <= I2C_Get_MasterRxFIFOWatermark(baseAddr))
    {
        /* Reduce rx watermark to receive the last few bytes */
        I2C_Set_MasterRxFIFOWatermark(baseAddr, (uint16_t) (master->rxSize - 1U));
    } else
    {
        /* Continue reception */
    }
}


/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_SlaveHandleAddressValidEvent
 * Description   : handle an address valid event for slave
 *
 *END**************************************************************************/
static void I2C_DRV_SlaveHandleAddressValidEvent(uint32_t instance, const I2C_Type *baseAddr, i2c_slave_state_t *slave)
{
    uint16_t receivedAddr;

    receivedAddr = I2C_Get_SlaveReceivedAddr(baseAddr);
    if ((receivedAddr & 1U) != (uint16_t) 0U)
    {
        /* Request from master to transmit data */
        if ((slave->slaveCallback != NULL) && slave->slaveListening)
        {
            slave->slaveCallback(I2C_SLAVE_EVENT_TX_REQ, slave->callbackParam);
        }

#if defined(I2C_SUPPORT_TXCFG) && (I2C_SUPPORT_TXCFG == 1)
        if (slave->transferType == I2C_USING_INTERRUPTS) {
            /* Enable interrupt for transmitting data */
            I2C_Set_SlaveInt(g_i2cBase[instance], (uint32_t)I2C_SLAVE_TRANSMIT_DATA_INT, true);
        }
#endif

        slave->txUnderrunWarning = false;

        if ((slave->transferType == I2C_USING_DMA) && slave->slaveListening)
        {
            (void) I2C_DRV_SlaveStartDmaTransfer(instance);
        }

    } else
    {
        /* Request from master to receive data */
        if ((slave->slaveCallback != NULL) && slave->slaveListening)
        {
            slave->slaveCallback(I2C_SLAVE_EVENT_RX_REQ, slave->callbackParam);
        }

        if ((slave->transferType == I2C_USING_DMA) && slave->slaveListening)
        {
            (void) I2C_DRV_SlaveStartDmaTransfer(instance);
        }
    }

    slave->status = STATUS_BUSY;
}


/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_SlaveHandleTransmitDataEvent
 * Description   : handle a transmit data event for slave
 *
 *END**************************************************************************/
static void I2C_DRV_SlaveHandleTransmitDataEvent(I2C_Type *baseAddr, i2c_slave_state_t *slave)
{
    if (slave->txUnderrunWarning == true)
    {
        /* Another Tx event after underflow warning means the dummy char was sent */
        slave->status = STATUS_I2C_TX_UNDERRUN;
    }

    if (slave->txSize == 0U)
    {
        /* Out of data, call callback to allow user to provide a new buffer */
        if (slave->slaveCallback != NULL)
        {
            slave->slaveCallback(I2C_SLAVE_EVENT_TX_EMPTY, slave->callbackParam);
        }
    }

    if (slave->txSize == 0U)
    {
        /*
         * Still no data, record tx underflow event and send dummy char.
         * Special case after the last tx byte: the device will ask for more data
         * but the dummy char will not be sent if NACK and then STOP condition are
         * received from master. So only record a "warning" for now.
         */
        slave->txUnderrunWarning = true;
        I2C_Transmit_SlaveData(baseAddr, (uint8_t) 0xFFU);
    } else
    {
        I2C_Transmit_SlaveData(baseAddr, slave->txBuff[0U]);
        slave->txBuff++;
        slave->txSize--;
    }
}


/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_SlaveHandleReceiveDataEvent
 * Description   : handle a receive data event for slave
 *
 *END**************************************************************************/
static void I2C_DRV_SlaveHandleReceiveDataEvent(const I2C_Type *baseAddr, i2c_slave_state_t *slave)
{
    if (slave->rxSize == 0U)
    {
        /* No more room for data, call callback to allow user to provide a new buffer */
        if (slave->slaveCallback != NULL)
        {
            slave->slaveCallback(I2C_SLAVE_EVENT_RX_FULL, slave->callbackParam);
        }
    }

    if (slave->rxSize == 0U)
    {
        /* Still no room for data, record rx overrun event and dummy read data */
        slave->status = STATUS_I2C_RX_OVERRUN;
        (void) I2C_Get_SlaveData(baseAddr);
    } else
    {
        slave->rxBuff[0U] = I2C_Get_SlaveData(baseAddr);
        slave->rxBuff++;
        slave->rxSize--;
    }
}

/*! @endcond */

/*******************************************************************************
 * Code
 ******************************************************************************/

/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_MasterInit
 * Description   : initialize the I2C master mode driver
 *
 * Implements : I2C_DRV_MasterInit_Activity
 *END**************************************************************************/
status_t I2C_DRV_MasterInit(uint32_t instance,
                            const i2c_master_user_config_t *userConfigPtr,
                            i2c_master_state_t *master)
{
    I2C_Type *baseAddr;
    status_t retVal;
    uint32_t inputClock;
    uint32_t coreClock;
    i2c_baud_rate_params_t baudRate;
    uint32_t idleAttempts = 0;

    DEV_ASSERT(master != NULL);
    DEV_ASSERT(userConfigPtr != NULL);
    DEV_ASSERT(instance < I2C_INSTANCE_COUNT);

    /* Check to see if the I2C master instance is already initialized */
    DEV_ASSERT(g_i2cMasterStatePtr[instance] == NULL);

    /* Check the protocol clock frequency */
    (void)CLOCK_SYS_GetFreq(g_i2cClock[instance], &inputClock);
    DEV_ASSERT(inputClock > 0U);
    (void)CLOCK_SYS_GetFreq(CORE_CLK, &coreClock);
    DEV_ASSERT(coreClock > 0U);

    baseAddr = g_i2cBase[instance];
    g_i2cMasterStatePtr[instance] = master;

    /* Initialize driver status structure */
    master->rxBuff = NULL;
    master->rxSize = 0U;
    master->txBuff = NULL;
    master->txSize = 0U;
    master->status = STATUS_SUCCESS;
    master->i2cIdle = true;
    master->slaveAddress = userConfigPtr->slaveAddress;
    master->is10bitAddr = userConfigPtr->is10bitAddr;
    master->transferType = userConfigPtr->transferType;
    /* Store DMA channel number used in transfer */
    master->dmaChannel = userConfigPtr->dmaChannel;
    master->masterCallback = userConfigPtr->masterCallback;
    master->callbackParam = userConfigPtr->callbackParam;
#if(I2C_HAS_HIGH_SPEED_MODE)
    master->masterCode = userConfigPtr->masterCode;
    master->highSpeedInProgress = false;
#endif
    master->blocking = false;
    master->baudrate = userConfigPtr->baudRate;
    master->abortedTransfer = false;

    /* Initialize the semaphore */
    retVal = OSIF_SemaCreate(&(master->idleSemaphore), 0);
    DEV_ASSERT(retVal == STATUS_SUCCESS);

    I2C_DRV_MasterResetQueue(master);

    /* Enable i2c interrupt */
    INT_SYS_EnableIRQ(g_i2cMasterIrqId[instance]);

    /* Reset the i2c module by calling I2C_Init() */
    I2C_Init(instance);

    /* Set line low timeout value */
    I2C_Set_MasterLineLowTimeoutPeriod(baseAddr, 0xFFFU);
    /* Set SDA and SCL line low detect enable */
    I2C_Set_MasterLineLowDetect(baseAddr);

    /* Set baud rate */
    baudRate.baudRate = userConfigPtr->baudRate;
#if(I2C_HAS_HIGH_SPEED_MODE)
    master->baudRateHS = userConfigPtr->baudRateHS;
    baudRate.baudRateHS = userConfigPtr->baudRateHS;
#endif
    (void) I2C_DRV_MasterSetBaudRate(instance, userConfigPtr->operatingMode, baudRate);

    /* Set slave address */
    I2C_DRV_MasterSetSlaveAddr(instance, userConfigPtr->slaveAddress, userConfigPtr->is10bitAddr);

    /* Enable I2C master */
    I2C_Set_MasterEnable(baseAddr, true);

    /* 
        Here, it is necessary to incorporate a timeout mechanism to determine if the bus is idle. 
        This is crucial because the time required for transitioning from the activation of the I2C 
        master to the recognition of the bus as idle is IDLE*divided function clock cycles.
    */
    idleAttempts = (1UL<<(uint32_t)I2C_Get_MasterPrescaler(baseAddr)) * I2C_Get_MasterTimeoutPeriod(baseAddr) \
                    * (coreClock / inputClock);
    for(uint32_t i=0; i<idleAttempts; i++)
    {
        /* Check bus busy condition */
        if (!I2C_Get_MasterBusBusyEvent(baseAddr))
        {
            retVal = STATUS_SUCCESS;
            break;
        }

        retVal = STATUS_I2C_BUS_BUSY;
    }

    return retVal;
}


/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_MasterDeinit
 * Description   : deinitialize the I2C master mode driver
 *
 * Implements : I2C_DRV_MasterDeinit_Activity
 *END**************************************************************************/
status_t I2C_DRV_MasterDeinit(uint32_t instance)
{
    const i2c_master_state_t *master;

    DEV_ASSERT(instance < I2C_INSTANCE_COUNT);

    master = g_i2cMasterStatePtr[instance];
    DEV_ASSERT(master != NULL);

    /* Destroy the semaphore */
    (void) OSIF_SemaDestroy(&(master->idleSemaphore));

    g_i2cMasterStatePtr[instance] = NULL;

    /* Reset the i2c module by calling I2C_Init() */
    I2C_Init(instance);

    /* Disable i2c interrupt */
    INT_SYS_DisableIRQ(g_i2cMasterIrqId[instance]);

    return STATUS_SUCCESS;
}


/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_MasterGetBaudRate
 * Description   : returns the currently configured baud rate
 *
 * Implements : I2C_DRV_MasterGetBaudRate_Activity
 *END**************************************************************************/
void I2C_DRV_MasterGetBaudRate(uint32_t instance, i2c_baud_rate_params_t *baudRate)
{
    const I2C_Type *baseAddr;
    const i2c_master_state_t *master;
    status_t retVal;
    uint32_t prescaler;
    uint32_t clkLo;
    uint32_t clkHi;
    uint32_t inputClock;

    DEV_ASSERT(instance < I2C_INSTANCE_COUNT);

    baseAddr = g_i2cBase[instance];
    master = g_i2cMasterStatePtr[instance];
    DEV_ASSERT(master != NULL);

    /* Get the protocol clock frequency */
    retVal = CLOCK_SYS_GetFreq(g_i2cClock[instance], &inputClock);
    DEV_ASSERT(retVal == STATUS_SUCCESS);
    DEV_ASSERT(inputClock > 0U);

    /* Ignoring the glitch filter, the baud rate formula is:
            SCL_freq = Input_freq / (2^PRESCALER * (CLKLO + CLKHI + 2))
    */
    prescaler = (uint32_t) I2C_Get_MasterPrescaler(baseAddr);
    clkHi = (uint32_t) I2C_Get_MasterClockHighPeriod(baseAddr);
    clkLo = (uint32_t) I2C_Get_MasterClockLowPeriod(baseAddr);

    baudRate->baudRate = inputClock / (((uint32_t) 1U << prescaler) * (clkLo + clkHi + (uint32_t) 2U));

#if(I2C_HAS_HIGH_SPEED_MODE)
    if (master->operatingMode == I2C_HIGHSPEED_MODE)
    {
        clkHi = I2C_Get_MasterClockHighPeriodHS(baseAddr);
        clkLo = I2C_Get_MasterClockLowPeriodHS(baseAddr);

        baudRate->baudRateHS = inputClock / (((uint32_t) 1U << prescaler) * (clkLo + clkHi + (uint32_t) 2U));
    }
#endif

    (void) retVal;
    (void) master;
}


/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_MasterSetBaudRate
 * Description   : set the baud rate for any subsequent I2C communication
 *
 * Implements : I2C_DRV_MasterSetBaudRate_Activity
 *END**************************************************************************/
status_t I2C_DRV_MasterSetBaudRate(uint32_t instance,
                                   const i2c_mode_t operatingMode,
                                   const i2c_baud_rate_params_t baudRate)
{
    I2C_Type *baseAddr;
    const i2c_master_state_t *master;
    status_t retVal;
    uint32_t inputClock;
    uint32_t minPrescaler = 0U;
    uint32_t prescaler;
    uint32_t clkTotal;
    uint32_t clkLo;
    uint32_t clkHi;
    uint32_t setHold;
    uint32_t dataVd;

    DEV_ASSERT(instance < I2C_INSTANCE_COUNT);

    baseAddr = g_i2cBase[instance];
    master = g_i2cMasterStatePtr[instance];
    DEV_ASSERT(master != NULL);

    /* Check if driver is busy */
    if (!master->i2cIdle)
    {
        retVal = STATUS_BUSY;
    }else {
        
        /* Get the protocol clock frequency */
        retVal = CLOCK_SYS_GetFreq(g_i2cClock[instance], &inputClock);
        DEV_ASSERT(retVal == STATUS_SUCCESS);
        DEV_ASSERT(inputClock > 0U);
    
        /* Disable master */
        I2C_Set_MasterEnable(baseAddr, false);
    
        /* Ignoring the glitch filter, the baud rate formula is:
                SCL_freq = Input_freq / (2^PRESCALER * (CLKLO + CLKHI + 2 + (2 + 0)/(1<<prescaler)))
                Assume CLKLO = CLKHI, SETHOLD = CLKHI, DATAVD = CLKHI/2
        */
    
        if (baudRate.baudRate != 0U)
        {
            /* Compute minimum prescaler for which CLKLO and CLKHI values are in valid range. Always round up. */
            minPrescaler =
                ((inputClock - 1U) / ((baudRate.baudRate) * (CLKHI_MAX_VALUE + CLKLO_MAX_VALUE + 2U))) + (uint32_t) 1U;
            for (prescaler = 0U; prescaler < 7U; prescaler++)
            {
                if (((uint32_t) 1U << prescaler) >= minPrescaler)
                {
                    break;
                }
            }
    
            /* Compute CLKLO and CLKHI values for this prescaler. Round to nearest integer. */
            clkTotal = (inputClock + ((baudRate.baudRate << prescaler) >> 1U)) / (baudRate.baudRate << prescaler);
        } else
        {
            prescaler = 7U;
            clkTotal = (CLKHI_MAX_VALUE + CLKLO_MAX_VALUE + 2U);
        }
    
        if (clkTotal > (CLKHI_MAX_VALUE + CLKLO_MAX_VALUE + 2U))
        {
            clkTotal = (CLKHI_MAX_VALUE + CLKLO_MAX_VALUE + 2U);
        }
    
        /*
         * If we try to compute clk high and low values using clkTotal equal with 0
         * (this is the case when the baudrate is 0), we will get negative values for
         * them, so we set them to 0 for this case.
         */
        if (clkTotal <= 1U)
        {
            clkHi = 0U;
            clkLo = 0U;
        } else
        {
            clkHi = (clkTotal - 2U) / 2U;
            clkLo = (clkTotal - clkHi - 2U) - (uint32_t)((2U + 0U)/(1UL<<prescaler));
        }
    
        if (clkHi < CLKHI_MIN_VALUE)
        {
            clkHi = CLKHI_MIN_VALUE;
        }
        if (clkLo < CLKLO_MIN_VALUE)
        {
            clkLo = CLKLO_MIN_VALUE;
        }
    
        /* Compute DATAVD and SETHOLD */
        setHold = clkHi;
        dataVd = clkHi >> 1U;
        if (setHold < SETHOLD_MIN_VALUE)
        {
            setHold = SETHOLD_MIN_VALUE;
        }
        if (dataVd < DATAVD_MIN_VALUE)
        {
            dataVd = DATAVD_MIN_VALUE;
        }
    
        /* Apply settings */
        I2C_Set_MasterPrescaler(baseAddr, (i2c_master_prescaler_t) prescaler);
        I2C_Set_MasterDataValidDelay(baseAddr, (uint8_t) dataVd);
        I2C_Set_MasterSetupHoldDelay(baseAddr, (uint8_t) setHold);
        I2C_Set_MasterClockHighPeriod(baseAddr, (uint8_t) clkHi);
        I2C_Set_MasterClockLowPeriod(baseAddr, (uint8_t) clkLo);
    
#if(I2C_HAS_HIGH_SPEED_MODE)
        if (operatingMode == I2C_HIGHSPEED_MODE)
        {
            /* Compute settings for High-speed baud rate */
            /* Compute High-speed CLKLO and CLKHI values for the same prescaler. Round to nearest integer. */
            clkTotal = (inputClock + ((baudRate.baudRateHS << prescaler) >> 1U)) / (baudRate.baudRateHS << prescaler);
            if (clkTotal > (CLKHI_MAX_VALUE + CLKLO_MAX_VALUE + 2U))
            {
                clkTotal = (CLKHI_MAX_VALUE + CLKLO_MAX_VALUE + 2U);
            }
    
            clkHi = (clkTotal - 2U) / 3U;
            clkLo = clkTotal - 2U - clkHi;
            if (clkHi < CLKHI_MIN_VALUE)
            {
                clkHi = CLKHI_MIN_VALUE;
            }
            if (clkLo < CLKLO_MIN_VALUE)
            {
                clkLo = CLKLO_MIN_VALUE;
            }
    
            /* Compute High-speed DATAVD and SETHOLD */
            setHold = clkHi;
            dataVd = clkHi >> 1U;
            if (setHold < SETHOLD_MIN_VALUE)
            {
                setHold = SETHOLD_MIN_VALUE;
            }
            if (dataVd < DATAVD_MIN_VALUE)
            {
                dataVd = DATAVD_MIN_VALUE;
            }
    
            /* Apply High-speed settings */
            I2C_Set_MasterDataValidDelayHS(baseAddr, (uint8_t) dataVd);
            I2C_Set_MasterSetupHoldDelayHS(baseAddr, (uint8_t) setHold);
            I2C_Set_MasterClockHighPeriodHS(baseAddr, (uint8_t) clkHi);
            I2C_Set_MasterClockLowPeriodHS(baseAddr, (uint8_t) clkLo);
        }
#endif
    
        /* Perform other settings related to the chosen operating mode */
        I2C_DRV_MasterSetOperatingMode(instance, operatingMode);
    
        /* configure idle timeout */
        I2C_Set_MasterTimeoutPeriod(baseAddr, (uint16_t)(clkHi + 6U));
    
        /* Re-enable master */
        I2C_Set_MasterEnable(baseAddr, true);
    
        (void) minPrescaler;
        (void) master;
    }

    return retVal;
}


/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_MasterSetSlaveAddr
 * Description   : set the slave address for any subsequent I2C communication
 *
 * Implements : I2C_DRV_MasterSetSlaveAddr_Activity
 *END**************************************************************************/
void I2C_DRV_MasterSetSlaveAddr(uint32_t instance, const uint16_t address, const bool is10bitAddr)
{
    i2c_master_state_t *master;

    DEV_ASSERT(instance < I2C_INSTANCE_COUNT);

    master = g_i2cMasterStatePtr[instance];
    DEV_ASSERT(master != NULL);

    master->slaveAddress = address;
    master->is10bitAddr = is10bitAddr;
}


/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_MasterSendData
 * Description   : perform a non-blocking send transaction on the I2C bus
 *
 * Implements : I2C_DRV_MasterSendData_Activity
 *END**************************************************************************/
status_t I2C_DRV_MasterSendData(uint32_t instance,
                                const uint8_t *txBuff,
                                uint32_t txSize,
                                bool sendStop)
{
    I2C_Type *baseAddr;
    i2c_master_state_t *master;

    DEV_ASSERT(instance < I2C_INSTANCE_COUNT);
    DEV_ASSERT(txBuff != NULL);
    DEV_ASSERT(txSize > 0U);

    status_t status = STATUS_SUCCESS;
    baseAddr = g_i2cBase[instance];
    master = g_i2cMasterStatePtr[instance];
    DEV_ASSERT(master != NULL);

    /* Check if driver is busy */
    if (!master->i2cIdle)
    {
        status = STATUS_BUSY;
    }else {
        
        /* Copy parameters to driver state structure */
        master->txBuff = txBuff;
        master->txSize = txSize;
        master->sendStop = sendStop;
        master->i2cIdle = false;
        master->status = STATUS_BUSY;
    
        if (master->operatingMode == I2C_ULTRAFAST_MODE)
        {
            /* Clear NACK flag */
            I2C_Clear_MasterNACKDetectEvent(baseAddr);
        } 
    
        if (master->transferType == I2C_USING_DMA)
        {
            /* Enable relevant events */
#if(I2C_HAS_ULTRA_FAST_MODE)
            if (master->operatingMode == I2C_ULTRAFAST_MODE)
            {
                /* Do not enable NACK event reporting in ultra-fast mode */
                I2C_Set_MasterInt(baseAddr, I2C_MASTER_FIFO_ERROR_INT |
                                            I2C_MASTER_PIN_LOW_TIMEOUT_INT |
                                            I2C_MASTER_ARBITRATION_LOST_INT,
                                true);
            } else
#endif
            {
                I2C_Set_MasterInt(baseAddr, I2C_MASTER_FIFO_ERROR_INT |
                                            I2C_MASTER_PIN_LOW_TIMEOUT_INT |
                                            I2C_MASTER_ARBITRATION_LOST_INT |
                                            I2C_MASTER_NACK_DETECT_INT,
                                true);
            }
    
            I2C_DRV_MasterStartDmaTransfer(instance);
        } else
        {
            /* Initiate communication */
            I2C_DRV_MasterSendAddress(baseAddr, master, false);
    
            /* Queue data bytes to fill tx fifo */
            I2C_DRV_MasterQueueData(baseAddr, master);
    
            /* Set tx FIFO watermark */
            I2C_Set_MasterTxFIFOWatermark(baseAddr, 0U);
    
            /* Enable relevant events */
#if(I2C_HAS_ULTRA_FAST_MODE)
            if (master->operatingMode == I2C_ULTRAFAST_MODE)
            {
                /* Do not enable NACK event reporting in ultra-fast mode */
                I2C_Set_MasterInt(baseAddr, I2C_MASTER_FIFO_ERROR_INT |
                                            I2C_MASTER_PIN_LOW_TIMEOUT_INT |
                                            I2C_MASTER_ARBITRATION_LOST_INT |
                                            I2C_MASTER_TRANSMIT_DATA_INT,
                                  true);
            } else
#endif
            {
                I2C_Set_MasterInt(baseAddr, I2C_MASTER_FIFO_ERROR_INT |
                                            I2C_MASTER_PIN_LOW_TIMEOUT_INT |
                                            I2C_MASTER_ARBITRATION_LOST_INT |
                                            I2C_MASTER_NACK_DETECT_INT |
                                            I2C_MASTER_TRANSMIT_DATA_INT,
                                  true);
            }
        }
    }

    return status;
}


/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_MasterSendDataBlocking
 * Description   : perform a blocking send transaction on the I2C bus
 *
 * Implements : I2C_DRV_MasterSendDataBlocking_Activity
 *END**************************************************************************/
status_t I2C_DRV_MasterSendDataBlocking(uint32_t instance,
                                        const uint8_t *txBuff,
                                        uint32_t txSize,
                                        bool sendStop,
                                        uint32_t timeout)
{

    DEV_ASSERT(instance < I2C_INSTANCE_COUNT);
    DEV_ASSERT(txBuff != NULL);
    DEV_ASSERT(txSize > 0U);

    status_t status = STATUS_SUCCESS;
    i2c_master_state_t *master = g_i2cMasterStatePtr[instance];
    DEV_ASSERT(master != NULL);

    /* Check if driver is busy */
    if (!master->i2cIdle)
    {
        status = STATUS_BUSY; 
    }else {
        
        /* mark transfer as blocking */
        master->blocking = true;
    
        /* Dummy wait to ensure the semaphore is 0, no need to check result */
        (void) OSIF_SemaWait(&(master->idleSemaphore), 0);
    
        (void) I2C_DRV_MasterSendData(instance, txBuff, txSize, sendStop);
    
        /* Wait for transfer to end */
        status = I2C_DRV_MasterWaitTransferEnd(instance, timeout);
    }

    return status;
}


/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_MasterAbortTransferData
 * Description   : abort a non-blocking I2C Master transmission or reception
 *
 * Implements : I2C_DRV_MasterAbortTransferData_Activity
 *END**************************************************************************/
status_t I2C_DRV_MasterAbortTransferData(uint32_t instance)
{
    status_t retVal = STATUS_SUCCESS;
    I2C_Type *baseAddr;
    i2c_master_state_t *master;

    DEV_ASSERT(instance < I2C_INSTANCE_COUNT);

    baseAddr = g_i2cBase[instance];
    master = g_i2cMasterStatePtr[instance];
    DEV_ASSERT(master != NULL);

    if (master->rxBuff != NULL)
    {
        /* Aborting a reception not supported because hardware will continue the
           current command even if the FIFO is reset and this could last indefinitely */
        retVal = STATUS_UNSUPPORTED;
    }else {

        /* End transfer: force stop generation, reset FIFOs */
        master->status = STATUS_I2C_ABORTED;
        I2C_DRV_MasterEndTransfer(baseAddr, master, true, true);
    }

    return retVal;
}


/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_MasterReceiveData
 * Description   : perform a non-blocking receive transaction on the I2C bus
 *
 * Implements : I2C_DRV_MasterReceiveData_Activity
 *END**************************************************************************/
status_t I2C_DRV_MasterReceiveData(uint32_t instance,
                                   uint8_t *rxBuff,
                                   uint32_t rxSize,
                                   bool sendStop)
{
    status_t retVal = STATUS_SUCCESS;
    I2C_Type *baseAddr;
    i2c_master_state_t *master;
    uint16_t rxBytes;

    DEV_ASSERT(instance < I2C_INSTANCE_COUNT);
    DEV_ASSERT(rxBuff != NULL);
    DEV_ASSERT(rxSize > 0U);

    DEV_ASSERT(rxSize <= 256U);

    baseAddr = g_i2cBase[instance];
    master = g_i2cMasterStatePtr[instance];
    DEV_ASSERT(master != NULL);

    /* Check if driver is busy */
    if (!master->i2cIdle)
    {
        retVal = STATUS_BUSY;
    }else {
        
#if(I2C_HAS_ULTRA_FAST_MODE)
        if (master->operatingMode == I2C_ULTRAFAST_MODE)
        {
            /* No reception possible in ultra-fast mode */
            retVal = STATUS_ERROR;
        }else
#endif
        {
            /* Copy parameters to driver state structure */
            master->rxSize = rxSize;
            master->i2cIdle = false;
            master->sendStop = sendStop;
            master->rxBuff = rxBuff;
            master->status = STATUS_BUSY;
        
            if (master->transferType == I2C_USING_DMA)
            {
                I2C_Set_MasterInt(baseAddr, I2C_MASTER_FIFO_ERROR_INT |
                                            I2C_MASTER_PIN_LOW_TIMEOUT_INT |
                                            I2C_MASTER_ARBITRATION_LOST_INT |
                                            I2C_MASTER_NACK_DETECT_INT,
                                  true);
        
                I2C_DRV_MasterStartDmaTransfer(instance);
            } else
            {
                /* Initiate communication */
                I2C_DRV_MasterSendAddress(baseAddr, master, true);
                /* Queue receive command for rxSize bytes */
                I2C_DRV_MasterQueueCmd(baseAddr, master, I2C_MASTER_COMMAND_RECEIVE, (uint8_t) (rxSize - 1U));
        
                /* Set rx FIFO watermark */
                rxBytes = I2C_Get_MasterRxFIFOSize(baseAddr);
                if (rxBytes > rxSize)
                {
                    rxBytes = (uint8_t) rxSize;
                }
                I2C_Set_MasterRxFIFOWatermark(baseAddr, (uint16_t) (rxBytes - 1U));
        
                /* Enable relevant events */
                if (!I2C_DRV_MasterCmdQueueEmpty(master))
                {
                    /* Enable tx event too if there are commands in the software FIFO */
                    I2C_Set_MasterInt(baseAddr, I2C_MASTER_FIFO_ERROR_INT |
                                                I2C_MASTER_PIN_LOW_TIMEOUT_INT |
                                                I2C_MASTER_ARBITRATION_LOST_INT |
                                                I2C_MASTER_NACK_DETECT_INT |
                                                I2C_MASTER_TRANSMIT_DATA_INT |
                                                I2C_MASTER_RECEIVE_DATA_INT,
                                      true);
                } else
                {
                    I2C_Set_MasterInt(baseAddr, I2C_MASTER_FIFO_ERROR_INT |
                                                I2C_MASTER_PIN_LOW_TIMEOUT_INT |
                                                I2C_MASTER_ARBITRATION_LOST_INT |
                                                I2C_MASTER_NACK_DETECT_INT |
                                                I2C_MASTER_RECEIVE_DATA_INT,
                                      true);
                }
            }
        }
    }

    return retVal;
}


/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_MasterReceiveDataBlocking
 * Description   : perform a blocking receive transaction on the I2C bus
 *
 * Implements : I2C_DRV_MasterReceiveDataBlocking_Activity
 *END**************************************************************************/
status_t I2C_DRV_MasterReceiveDataBlocking(uint32_t instance,
                                           uint8_t *rxBuff,
                                           uint32_t rxSize,
                                           bool sendStop,
                                           uint32_t timeout)
{
    status_t retVal = STATUS_SUCCESS;

    DEV_ASSERT(instance < I2C_INSTANCE_COUNT);
    DEV_ASSERT(rxBuff != NULL);
    DEV_ASSERT(rxSize > 0U);

    i2c_master_state_t *master = g_i2cMasterStatePtr[instance];
    DEV_ASSERT(master != NULL);

    /* Check if driver is busy */
    if (!master->i2cIdle)
    {
        retVal = STATUS_BUSY;
    }else {

        /* mark transfer as blocking */
        master->blocking = true;
    
        /* Dummy wait to ensure the semaphore is 0, no need to check result */
        (void) OSIF_SemaWait(&(master->idleSemaphore), 0);
    
        retVal = I2C_DRV_MasterReceiveData(instance, rxBuff, rxSize, sendStop);
    
#if(I2C_HAS_ULTRA_FAST_MODE)
        if (retVal != STATUS_SUCCESS)
        {
            master->blocking = false;
        }else
#endif
        {
            /* Wait for transfer to end */
            retVal = I2C_DRV_MasterWaitTransferEnd(instance, timeout);
        }
    }

    return retVal;
}


/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_MasterGetTransferStatus
 * Description   : return the current status of the I2C master transfer
 *
 * When performing an a-sync (non-blocking) transfer, the user can call this function
 * to ascertain the state of the current transfer. In addition, if the transfer is still
 * in progress, the user can get the number of words that should be receive.
 *
 * Implements : I2C_DRV_MasterGetTransferStatus_Activity
 *END**************************************************************************/
status_t I2C_DRV_MasterGetTransferStatus(uint32_t instance,
                                         uint32_t *bytesRemaining)
{
    const I2C_Type *baseAddr;
    const i2c_master_state_t *master;

    DEV_ASSERT(instance < I2C_INSTANCE_COUNT);

    baseAddr = g_i2cBase[instance];
    master = g_i2cMasterStatePtr[instance];
    DEV_ASSERT(master != NULL);

    if ((bytesRemaining != NULL) && (master->transferType == I2C_USING_INTERRUPTS))
    {
        if (master->txSize > 0U)
        {
            /* Send data */
            /* Remaining bytes = bytes in buffer + bytes in tx FIFO */
            *bytesRemaining = master->txSize + (uint32_t) I2C_Get_MasterTxFIFOCount(baseAddr);
        } else if (master->rxSize > 0U)
        {
            /* Receive data */
            /* Remaining bytes = free space in buffer - bytes in rx FIFO */
            *bytesRemaining = master->rxSize - (uint32_t) I2C_Get_MasterRxFIFOCount(baseAddr);
        } else
        {
            *bytesRemaining = 0U;
        }
    }

    return master->status;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_MasterGetDefaultConfig
 * Description   : Gets the default configuration structure for master
 *
 * Implements : I2C_DRV_MasterGetDefaultConfig_Activity
 *END**************************************************************************/
void I2C_DRV_MasterGetDefaultConfig(i2c_master_user_config_t *config)
{
    config->slaveAddress = 32U;
    config->is10bitAddr = false;
    config->operatingMode = I2C_STANDARD_MODE;
    config->baudRate = 100000U;
    config->transferType = I2C_USING_INTERRUPTS;
    config->dmaChannel = 0U;
    config->masterCallback = NULL;
    config->callbackParam = NULL;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_MasterIRQHandler
 * Description   : handle non-blocking master operation when I2C interrupt occurs
 *
 *END**************************************************************************/
void I2C_DRV_MasterIRQHandler(uint32_t instance)
{
    I2C_Type *baseAddr;
    i2c_master_state_t *master;

    DEV_ASSERT(instance < I2C_INSTANCE_COUNT);

    baseAddr = g_i2cBase[instance];
    master = g_i2cMasterStatePtr[instance];
    DEV_ASSERT(master != NULL);

    /* Check which event caused the interrupt */
    if (I2C_Get_MasterTransmitDataRequestEvent(baseAddr))
    {
        I2C_DRV_MasterHandleTransmitDataRequest(baseAddr, master);
    }

    if ((I2C_Get_MasterReceiveDataReadyEvent(baseAddr)))
    {
        if (master->abortedTransfer)
        {
            /* Signal transfer end for blocking transfers */
            if (master->blocking == true)
            {
                (void) OSIF_SemaPost(&(master->idleSemaphore));
            }

            master->status = STATUS_TIMEOUT;

            /* Get the remaining data of rx fifo to user buff */
            while ((I2C_Get_MasterRxFIFOCount(baseAddr) > 0U) && (master->rxSize > 0U))
            {
                master->rxBuff[0U] = I2C_Get_MasterRxData(baseAddr);
                master->rxBuff++;
                master->rxSize--;
            }
            
            (void) I2C_DRV_MasterReinit(instance, master);

        } else
        {
            I2C_DRV_MasterHandleReceiveDataReadyEvent(baseAddr, master);
        }

    }

    if (I2C_Get_MasterFIFOErrorEvent(baseAddr))
    {
        /* FIFO error */
        I2C_Clear_MasterFIFOErrorEvent(baseAddr);

#if(I2C_HAS_HIGH_SPEED_MODE)
        /* High-speed transfers end at STOP condition */
        master->highSpeedInProgress = false;
#endif
        master->status = STATUS_ERROR;

        /* End transfer: no stop generation (the module will handle that by itself
           if needed), reset FIFOs */
        I2C_DRV_MasterEndTransfer(baseAddr, master, false, true);

        /* Signal transfer end for blocking transfers */
        if (master->blocking == true)
        {
            (void) OSIF_SemaPost(&(master->idleSemaphore));
        }

        if (master->masterCallback != NULL)
        {
            master->masterCallback(I2C_MASTER_EVENT_FIFO_ERROR_REQ, master->callbackParam);
        }
    }

    if (I2C_Get_MasterArbitrationLostEvent(baseAddr))
    {
        /* Arbitration lost */
        I2C_Clear_MasterArbitrationLostEvent(baseAddr);

        /* End transfer: no stop generation (the module will handle that by itself
           if needed), reset FIFOs */
        I2C_DRV_MasterEndTransfer(baseAddr, master, false, true);

        /* Signal transfer end for blocking transfers */
        if (master->blocking == true)
        {
            (void) OSIF_SemaPost(&(master->idleSemaphore));
        }

        master->status = STATUS_I2C_ARBITRATION_LOST;

        if (master->masterCallback != NULL)
        {
            master->masterCallback(I2C_MASTER_EVENT_ARBITRATION_LOST_ERROR, master->callbackParam);
        }
    }

    if (I2C_Get_MasterNACKDetectEvent(baseAddr))
    {
        /* Received NACK */

#if(I2C_HAS_ULTRA_FAST_MODE)
        /* Ignore NACK in Ultra Fast mode */
        if (master->operatingMode != I2C_ULTRAFAST_MODE)
        {
#endif
            /* Signal transfer end for blocking transfers */
            if (master->blocking == true)
            {
                (void) OSIF_SemaPost(&(master->idleSemaphore));
            }

#if(I2C_HAS_HIGH_SPEED_MODE)
            /* High-speed transfers end at STOP condition */
            master->highSpeedInProgress = false;
#endif
            master->status = STATUS_I2C_RECEIVED_NACK;

            /* End transfer: no stop generation (the module will handle that by itself
               if needed), reset FIFOs */
            I2C_DRV_MasterEndTransfer(baseAddr, master, false, true);

            if (master->masterCallback != NULL)
            {
                master->masterCallback(I2C_MASTER_EVENT_NACK_DETECT, master->callbackParam);
            }

            /* Clear NACK flag */
            I2C_Clear_MasterNACKDetectEvent(baseAddr);
#if(I2C_HAS_ULTRA_FAST_MODE)
        }
#endif
    }

    if (I2C_Get_MasterLineLowTimeoutEvent(baseAddr))
    {
        if (master->masterCallback != NULL)
        {
            master->masterCallback(I2C_MASTER_EVENT_LINE_LOW_TIMEOUT, master->callbackParam);
        }

        /* End transfer: no stop generation (the module will handle that by itself
           if needed), reset FIFOs */
        I2C_DRV_MasterEndTransfer(baseAddr, master, false, true);

        /* Signal transfer end for blocking transfers */
        if (master->blocking == true)
        {
            (void) OSIF_SemaPost(&(master->idleSemaphore));
        }

        master->status = STATUS_I2C_LINE_LOW_TIMEOUT;

        /* Clear line low timeout event */
        I2C_Clear_MasterLineLowTimeoutEvent(baseAddr);
    }
}


/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_SlaveInit
 * Description   : initialize the I2C slave mode driver
 *
 * Implements : I2C_DRV_SlaveInit_Activity
 *END**************************************************************************/
status_t I2C_DRV_SlaveInit(uint32_t instance,
                           const i2c_slave_user_config_t *userConfigPtr,
                           i2c_slave_state_t *slave)
{
    I2C_Type *baseAddr;
    status_t retVal;
    uint32_t inputClock;

    DEV_ASSERT(slave != NULL);
    DEV_ASSERT(userConfigPtr != NULL);
    DEV_ASSERT(instance < I2C_INSTANCE_COUNT);

    DEV_ASSERT(g_i2cSlaveStatePtr[instance] == NULL);

    /* Check if slave mode is supported */
    if (!g_i2cSupportSlave[instance])
    {
        retVal = STATUS_UNSUPPORTED;
    }else {
        
        /*
         * Check the protocol clock frequency.
         * I2C slave remains operational, even when the I2C functional
         * clock is disabled, so we don't need to check if inputClock is 0.
         */
        (void)CLOCK_SYS_GetFreq(g_i2cClock[instance], &inputClock);
    
        baseAddr = g_i2cBase[instance];
        g_i2cSlaveStatePtr[instance] = slave;
    
        /* Initialize driver status structure */
        slave->status = STATUS_SUCCESS;
        slave->slaveListening = userConfigPtr->slaveListening;
        slave->slaveCallback = userConfigPtr->slaveCallback;
        slave->callbackParam = userConfigPtr->callbackParam;
        slave->txBuff = NULL;
        slave->rxBuff = NULL;
        slave->txSize = 0U;
        slave->rxSize = 0U;
        slave->transferType = userConfigPtr->transferType;
        /* Store DMA channel number used in transfer */
        slave->dmaChannel = userConfigPtr->dmaChannel;
        slave->isTransferInProgress = false;
        slave->blocking = false;
        slave->is10bitAddress = userConfigPtr->is10bitAddr;
        slave->repeatedStarts = 0U;
    
        /* Initialize the semaphore */
        retVal = OSIF_SemaCreate(&(slave->idleSemaphore), 0);
        DEV_ASSERT(retVal == STATUS_SUCCESS);
    
        /* Enable i2c interrupt */
        INT_SYS_EnableIRQ(g_i2cSlaveIrqId[instance]);
    
        /* Reset the i2c module by calling I2C_Init() */
        I2C_Init(instance);
    
        /* Configure slave address */
        I2C_Set_SlaveAddr0(baseAddr, userConfigPtr->slaveAddress << 1);
        if (userConfigPtr->is10bitAddr)
        {
            I2C_Set_SlaveAddrConfig(baseAddr, I2C_SLAVE_ADDR_MATCH_0_10BIT);
        } else
        {
            I2C_Set_SlaveAddrConfig(baseAddr, I2C_SLAVE_ADDR_MATCH_0_7BIT);
        }
    
        /* Configure operating mode */
        I2C_DRV_SlaveSetOperatingMode(instance, userConfigPtr->operatingMode);
    
        if (userConfigPtr->slaveListening)
        {
            if (slave->transferType == I2C_USING_DMA)
            {
                /* Activate events */
                I2C_Set_SlaveInt(baseAddr, I2C_SLAVE_BIT_ERROR_INT |
                                           I2C_SLAVE_FIFO_ERROR_INT |
                                           I2C_SLAVE_STOP_DETECT_INT |
                                           I2C_SLAVE_REPEATED_START_INT |
                                           I2C_SLAVE_ADDRESS_VALID_INT,
                                 true);
            }
            if (slave->transferType == I2C_USING_INTERRUPTS)
            {
                /* Activate events */
#if defined(I2C_SUPPORT_TXCFG) && (I2C_SUPPORT_TXCFG == 1)
                I2C_Set_SlaveInt(baseAddr, I2C_SLAVE_BIT_ERROR_INT |
                                                I2C_SLAVE_FIFO_ERROR_INT |
                                                I2C_SLAVE_STOP_DETECT_INT |
                                                I2C_SLAVE_REPEATED_START_INT |
                                                I2C_SLAVE_ADDRESS_VALID_INT |
                                                I2C_SLAVE_RECEIVE_DATA_INT,
                                       true);
    
#else
                I2C_Set_SlaveInt(baseAddr, I2C_SLAVE_BIT_ERROR_INT |
                                           I2C_SLAVE_FIFO_ERROR_INT |
                                           I2C_SLAVE_STOP_DETECT_INT |
                                           I2C_SLAVE_REPEATED_START_INT |
                                           I2C_SLAVE_ADDRESS_VALID_INT |
                                           I2C_SLAVE_RECEIVE_DATA_INT |
                                           I2C_SLAVE_TRANSMIT_DATA_INT,
                                 true);
    
#endif
    
            }
    
            /* Enable I2C slave */
            I2C_Set_SlaveEnable(baseAddr, true);
        }

    }

    return retVal;
}


/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_SlaveDeinit
 * Description   : de-initialize the I2C slave mode driver
 *
 * Implements : I2C_DRV_SlaveDeinit_Activity
 *END**************************************************************************/
status_t I2C_DRV_SlaveDeinit(uint32_t instance)
{
    DEV_ASSERT(instance < I2C_INSTANCE_COUNT);

    const i2c_slave_state_t *slave = g_i2cSlaveStatePtr[instance];
    DEV_ASSERT(slave != NULL);

    /* Destroy the semaphore */
    (void) OSIF_SemaDestroy(&(slave->idleSemaphore));

    g_i2cSlaveStatePtr[instance] = NULL;

    /* Reset the i2c module by calling I2C_Init() */
    I2C_Init(instance);

    /* Disable i2c interrupt */
    INT_SYS_DisableIRQ(g_i2cSlaveIrqId[instance]);

    return STATUS_SUCCESS;
}


/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_SlaveSetTxBuffer
 * Description   : Provide a buffer for transmitting data.
 *
 * Implements : I2C_DRV_SlaveSetTxBuffer_Activity
 *END**************************************************************************/
status_t I2C_DRV_SlaveSetTxBuffer(uint32_t instance,
                                  const uint8_t *txBuff,
                                  uint32_t txSize)
{
    i2c_slave_state_t *slave;

    DEV_ASSERT(instance < I2C_INSTANCE_COUNT);
    DEV_ASSERT(txBuff != NULL);
    DEV_ASSERT(txSize > 0U);

    slave = g_i2cSlaveStatePtr[instance];
    DEV_ASSERT(slave != NULL);

    slave->txBuff = txBuff;
    slave->txSize = txSize;

    return STATUS_SUCCESS;
}


/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_SlaveSetRxBuffer
 * Description   : Provide a buffer for receiving data.
 *
 * Implements : I2C_DRV_SlaveSetRxBuffer_Activity
 *END**************************************************************************/
status_t I2C_DRV_SlaveSetRxBuffer(uint32_t instance,
                                  uint8_t *rxBuff,
                                  uint32_t rxSize)
{
    i2c_slave_state_t *slave;

    DEV_ASSERT(instance < I2C_INSTANCE_COUNT);
    DEV_ASSERT(rxBuff != NULL);
    DEV_ASSERT(rxSize > 0U);

    slave = g_i2cSlaveStatePtr[instance];
    DEV_ASSERT(slave != NULL);

    slave->rxBuff = rxBuff;
    slave->rxSize = rxSize;

    return STATUS_SUCCESS;
}


/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_SlaveSendData
 * Description   : perform a non-blocking send transaction on the I2C bus
 *
 * Implements : I2C_DRV_SlaveSendData_Activity
 *END**************************************************************************/
status_t I2C_DRV_SlaveSendData(uint32_t instance,
                               const uint8_t *txBuff,
                               uint32_t txSize)
{
    status_t retVal = STATUS_SUCCESS;
    I2C_Type *baseAddr;
    i2c_slave_state_t *slave;

    DEV_ASSERT(instance < I2C_INSTANCE_COUNT);
    DEV_ASSERT(txBuff != NULL);
    DEV_ASSERT(txSize > 0U);


    baseAddr = g_i2cBase[instance];
    slave = g_i2cSlaveStatePtr[instance];
    DEV_ASSERT(slave != NULL);

    /* If the slave is in listening mode the user should not use this function or the blocking counterpart. */
    DEV_ASSERT(slave->slaveListening == false);

    /* Check if slave is busy */
    if (slave->isTransferInProgress)
    {
        retVal = STATUS_BUSY;
    }else {
        
        slave->txBuff = txBuff;
        slave->txSize = txSize;
        slave->status = STATUS_BUSY;
    
        if (slave->transferType == I2C_USING_DMA)
        {
            /* Activate events */
            I2C_Set_SlaveInt(baseAddr, I2C_SLAVE_BIT_ERROR_INT |
                                       I2C_SLAVE_FIFO_ERROR_INT |
                                       I2C_SLAVE_STOP_DETECT_INT |
                                       I2C_SLAVE_REPEATED_START_INT |
                                       I2C_SLAVE_ADDRESS_VALID_INT,
                             true);
    
            /* Enable I2C slave */
            I2C_Set_SlaveEnable(baseAddr, true);
    
            slave->isTransferInProgress = true;
    
            I2C_DRV_SlaveStartDmaTransfer(instance);
        } else
        {
            /* Activate events */
    #if defined(I2C_SUPPORT_TXCFG) && (I2C_SUPPORT_TXCFG == 1)
    
            I2C_Set_SlaveInt(baseAddr, I2C_SLAVE_BIT_ERROR_INT |
                                            I2C_SLAVE_FIFO_ERROR_INT |
                                            I2C_SLAVE_STOP_DETECT_INT |
                                            I2C_SLAVE_REPEATED_START_INT |
                                            I2C_SLAVE_ADDRESS_VALID_INT,
                                  true);
    
    #else
            I2C_Set_SlaveInt(baseAddr, I2C_SLAVE_BIT_ERROR_INT |
                                       I2C_SLAVE_FIFO_ERROR_INT |
                                       I2C_SLAVE_STOP_DETECT_INT |
                                       I2C_SLAVE_REPEATED_START_INT |
                                       I2C_SLAVE_ADDRESS_VALID_INT |
                                       I2C_SLAVE_TRANSMIT_DATA_INT,
                             true);
    #endif
    
    
            /* Enable I2C slave */
            I2C_Set_SlaveEnable(baseAddr, true);
    
            slave->isTransferInProgress = true;
        }
    }


    return retVal;
}


/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_SlaveSendDataBlocking
 * Description   : perform a blocking send transaction on the I2C bus
 *
 * Implements : I2C_DRV_SlaveSendDataBlocking_Activity
 *END**************************************************************************/
status_t I2C_DRV_SlaveSendDataBlocking(uint32_t instance,
                                       const uint8_t *txBuff,
                                       uint32_t txSize,
                                       uint32_t timeout)
{
    status_t retVal = STATUS_SUCCESS;
    DEV_ASSERT(instance < I2C_INSTANCE_COUNT);
    DEV_ASSERT(txBuff != NULL);
    DEV_ASSERT(txSize > 0U);

    i2c_slave_state_t *slave = g_i2cSlaveStatePtr[instance];
    DEV_ASSERT(slave != NULL);

    /* Check if slave is busy */
    if (slave->isTransferInProgress)
    {
        retVal = STATUS_BUSY;
    }else {

        /* mark transfer as blocking */
        slave->blocking = true;
    
        /* Dummy wait to ensure the semaphore is 0, no need to check result */
        (void) OSIF_SemaWait(&(slave->idleSemaphore), 0);
    
        (void) I2C_DRV_SlaveSendData(instance, txBuff, txSize);
    
        /* Wait for transfer to end */
        retVal = I2C_DRV_SlaveWaitTransferEnd(instance, timeout);
    }

    return retVal;
}


/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_SlaveReceiveData
 * Description   : perform a non-blocking receive transaction on the I2C bus
 *
 * Implements : I2C_DRV_SlaveReceiveData_Activity
 *END**************************************************************************/
status_t I2C_DRV_SlaveReceiveData(uint32_t instance,
                                  uint8_t *rxBuff,
                                  uint32_t rxSize)
{
    I2C_Type *baseAddr;
    i2c_slave_state_t *slave;

    DEV_ASSERT(instance < I2C_INSTANCE_COUNT);
    DEV_ASSERT(rxBuff != NULL);
    DEV_ASSERT(rxSize > 0U);

    status_t status = STATUS_SUCCESS;
    baseAddr = g_i2cBase[instance];
    slave = g_i2cSlaveStatePtr[instance];
    DEV_ASSERT(slave != NULL);

    /* If the slave is in listening mode the user should not use this function or the blocking counterpart. */
    DEV_ASSERT(slave->slaveListening == false);

    /* Check if slave is busy */
    if (slave->isTransferInProgress)
    {
        status = STATUS_BUSY;
    }else {

        slave->rxBuff = rxBuff;
        slave->rxSize = rxSize;
        slave->status = STATUS_BUSY;
    
        if (slave->transferType == I2C_USING_DMA)
        {
            /* Activate events */
            I2C_Set_SlaveInt(baseAddr, I2C_SLAVE_BIT_ERROR_INT |
                                       I2C_SLAVE_FIFO_ERROR_INT |
                                       I2C_SLAVE_STOP_DETECT_INT |
                                       I2C_SLAVE_REPEATED_START_INT |
                                       I2C_SLAVE_ADDRESS_VALID_INT,
                             true);
    
            /* Enable I2C slave */
            I2C_Set_SlaveEnable(baseAddr, true);
    
            slave->isTransferInProgress = true;
    
            I2C_DRV_SlaveStartDmaTransfer(instance);
        } else
        {
            slave->isTransferInProgress = true;
    
            /* Activate events */
            I2C_Set_SlaveInt(baseAddr, I2C_SLAVE_BIT_ERROR_INT |
                                       I2C_SLAVE_FIFO_ERROR_INT |
                                       I2C_SLAVE_STOP_DETECT_INT |
                                       I2C_SLAVE_REPEATED_START_INT |
                                       I2C_SLAVE_ADDRESS_VALID_INT |
                                       I2C_SLAVE_RECEIVE_DATA_INT,
                             true);
    
            /* Enable I2C slave */
            I2C_Set_SlaveEnable(baseAddr, true);
        }
    }

    return status;
}


/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_SlaveReceiveDataBlocking
 * Description   : perform a blocking receive transaction on the I2C bus
 *
 * Implements : I2C_DRV_SlaveReceiveDataBlocking_Activity
 *END**************************************************************************/
status_t I2C_DRV_SlaveReceiveDataBlocking(uint32_t instance,
                                          uint8_t *rxBuff,
                                          uint32_t rxSize,
                                          uint32_t timeout)
{
    DEV_ASSERT(instance < I2C_INSTANCE_COUNT);
    DEV_ASSERT(rxBuff != NULL);
    DEV_ASSERT(rxSize > 0U);

    status_t status = STATUS_SUCCESS;
    i2c_slave_state_t *slave = g_i2cSlaveStatePtr[instance];
    DEV_ASSERT(slave != NULL);

    /* Check if slave is busy */
    if (slave->isTransferInProgress)
    {
        status = STATUS_BUSY;
    }else {
        
        /* mark transfer as blocking */
        slave->blocking = true;
    
        /* Dummy wait to ensure the semaphore is 0, no need to check result */
        (void) OSIF_SemaWait(&(slave->idleSemaphore), 0);
    
        (void) I2C_DRV_SlaveReceiveData(instance, rxBuff, rxSize);
    
        /* Wait for transfer to end */
        status = I2C_DRV_SlaveWaitTransferEnd(instance, timeout);
    }

    return status;
}


/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_SlaveGetTransferStatus
 * Description   : return the current status of the I2C slave transfer
 *
 * When performing an a-sync (non-blocking) transfer, the user can call this function
 * to ascertain the state of the current transfer. In addition, if the transfer is still
 * in progress, the user can get the number of words that should be receive.
 *
 * Implements : I2C_DRV_SlaveGetTransferStatus_Activity
 *END**************************************************************************/
status_t I2C_DRV_SlaveGetTransferStatus(uint32_t instance,
                                        uint32_t *bytesRemaining)
{
    const i2c_slave_state_t *slave;

    DEV_ASSERT(instance < I2C_INSTANCE_COUNT);

    slave = g_i2cSlaveStatePtr[instance];
    DEV_ASSERT(slave != NULL);

    if ((bytesRemaining != NULL) && (slave->transferType == I2C_USING_INTERRUPTS))
    {
        if (slave->txSize > 0U)
        {
            /* Send data */
            *bytesRemaining = slave->txSize;
        } else if (slave->rxSize > 0U)
        {
            /* Receive data */
            *bytesRemaining = slave->rxSize;
        } else
        {
            *bytesRemaining = 0U;
        }
    }

    return slave->status;
}


/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_SlaveAbortTransferData
 * Description   : abort a non-blocking I2C Master transmission or reception
 *
 * Implements : I2C_DRV_SlaveAbortTransferData_Activity
 *END**************************************************************************/
status_t I2C_DRV_SlaveAbortTransferData(uint32_t instance)
{
    i2c_slave_state_t *slave;
    I2C_Type *baseAddr;

    DEV_ASSERT(instance < I2C_INSTANCE_COUNT);

    baseAddr = g_i2cBase[instance];
    slave = g_i2cSlaveStatePtr[instance];
    DEV_ASSERT(slave != NULL);

    if (!slave->slaveListening)
    {
        slave->status = STATUS_I2C_ABORTED;
        I2C_DRV_SlaveEndTransfer(baseAddr, slave);
    }

    return STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_SlaveGetDefaultConfig
 * Description   : Gets the default configuration structure for slave
 *
 * Implements : I2C_DRV_SlaveGetDefaultConfig_Activity
 *END**************************************************************************/
void I2C_DRV_SlaveGetDefaultConfig(i2c_slave_user_config_t *config)
{
    config->slaveAddress = 32U;
    config->is10bitAddr = false;
    config->slaveListening = true;
    config->operatingMode = I2C_STANDARD_MODE;
    config->transferType = I2C_USING_INTERRUPTS;
    config->dmaChannel = 0U;
    config->slaveCallback = NULL;
    config->callbackParam = NULL;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_SlaveEndTransferHandler
 * Description   : handle slave end transfer operations
 *
 *END**************************************************************************/
static void I2C_DRV_SlaveEndTransferHandler(i2c_slave_state_t *slave, I2C_Type *baseAddr)
{
    /* Check slave state */
    DEV_ASSERT(slave != NULL);

    /* Stop DMA channel if slave is transferring data in DMA mode */
    if (slave->transferType == I2C_USING_DMA)
    {
        (void) DMA_DRV_StopChannel(slave->dmaChannel);
    }

    if (!slave->slaveListening)
    {

        I2C_DRV_SlaveEndTransfer(baseAddr, slave);

        /* Signal transfer end for blocking transfers */
        if (slave->blocking == true)
        {
            (void) OSIF_SemaPost(&(slave->idleSemaphore));
        }
    }

    if (slave->slaveCallback != NULL)
    {
        slave->slaveCallback(I2C_SLAVE_EVENT_STOP, slave->callbackParam);
    }

}

/*FUNCTION**********************************************************************
 *
 * Function Name : I2C_DRV_SlaveIRQHandler
 * Description   : handle non-blocking slave operation when I2C interrupt occurs
 *
 *END**************************************************************************/
void I2C_DRV_SlaveIRQHandler(uint32_t instance)
{
    I2C_Type *baseAddr;
    i2c_slave_state_t *slave;
    bool stopDetect = false, repeatStartDetect = false;

    DEV_ASSERT(instance < I2C_INSTANCE_COUNT);

    baseAddr = g_i2cBase[instance];
    slave = g_i2cSlaveStatePtr[instance];
    DEV_ASSERT(slave != NULL);

    /* Check which event caused the interrupt */
    repeatStartDetect = I2C_Get_SlaveRepeatedStartEvent(baseAddr);
    if (repeatStartDetect)
    {
        I2C_Clear_SlaveRepeatedStartEvent(baseAddr);

        slave->repeatedStarts++;

        if ((slave->repeatedStarts == 1U) && (slave->is10bitAddress))
        {
            repeatStartDetect = false;
            (void)repeatStartDetect;
        }
        else 
        {
#if defined(I2C_SUPPORT_TXCFG) && (I2C_SUPPORT_TXCFG == 1)
            /* Deactivate interrupts for transmitting data */
            I2C_Set_SlaveInt(baseAddr, (uint32_t)I2C_SLAVE_TRANSMIT_DATA_INT, false);
#endif
            if (slave->status == STATUS_BUSY)
            {
                /* Report success if no error was recorded */
                slave->status = STATUS_SUCCESS;
            }

            I2C_DRV_SlaveEndTransferHandler(slave, baseAddr);
        }
    }

    if (I2C_Get_SlaveAddressValidEvent(baseAddr))
    {

        I2C_DRV_SlaveHandleAddressValidEvent(instance, baseAddr, slave);
    }

    if (I2C_Get_SlaveTransmitDataEvent(baseAddr))
    {
        if (I2C_Get_SlaveInt(baseAddr, (uint32_t) I2C_SLAVE_TRANSMIT_DATA_INT))
        {
            I2C_DRV_SlaveHandleTransmitDataEvent(baseAddr, slave);
        }
    }

    if (I2C_Get_SlaveReceiveDataEvent(baseAddr))
    {
        if (I2C_Get_SlaveInt(baseAddr, (uint32_t) I2C_SLAVE_RECEIVE_DATA_INT))
        {
            I2C_DRV_SlaveHandleReceiveDataEvent(baseAddr, slave);
        }
    }

    stopDetect = I2C_Get_SlaveSTOPDetectEvent(baseAddr);

    if (stopDetect == true)
    {
        I2C_Clear_SlaveSTOPDetectEvent(baseAddr);

#if defined(I2C_SUPPORT_TXCFG) && (I2C_SUPPORT_TXCFG == 1)
        /* Deactivate interrupts for transmitting data */
        I2C_Set_SlaveInt(baseAddr, (uint32_t)I2C_SLAVE_TRANSMIT_DATA_INT, false);
#endif

        if (slave->status == STATUS_BUSY)
        {
            /* Report success if no error was recorded */
            slave->status = STATUS_SUCCESS;
        }

        I2C_DRV_SlaveEndTransferHandler(slave, baseAddr);
    }

    if (I2C_Get_SlaveBitErrorEvent(baseAddr))
    {
        slave->status = STATUS_ERROR;
        I2C_Clear_SlaveBitErrorEvent(baseAddr);

#if defined(I2C_SUPPORT_TXCFG) && (I2C_SUPPORT_TXCFG == 1)
        /* Deactivate interrupts for transmitting data */
        I2C_Set_SlaveInt(baseAddr, (uint32_t)I2C_SLAVE_TRANSMIT_DATA_INT, false);
#endif

        I2C_DRV_SlaveEndTransferHandler(slave, baseAddr);
    }

#if(I2C_HAS_ULTRA_FAST_MODE)
    if (I2C_Get_SlaveFIFOErrorEvent(baseAddr))
    {
        /* In Ultra-Fast mode clock stretching is disabled, so it is possible to get
           this event if the slave can't keep up */
        slave->status = STATUS_I2C_RX_OVERRUN;
        I2C_Clear_SlaveFIFOErrorEvent(baseAddr);

#if defined(I2C_SUPPORT_TXCFG) && (I2C_SUPPORT_TXCFG == 1)
        /* Deactivate interrupts for transmitting data */
        I2C_Set_SlaveInt(baseAddr, I2C_SLAVE_TRANSMIT_DATA_INT, false);
#endif

        I2C_DRV_SlaveEndTransferHandler(slave, baseAddr);
    }
#endif

}

#if defined(YTM32B1L_SERIES) || defined (YTM32B1H_SERIES)
void I2C_DRV_ModuleIRQHandler(uint32_t instance)
{
    DEV_ASSERT(instance < I2C_INSTANCE_COUNT);

    /* Check if module is master or slave */
    if (g_i2cSlaveStatePtr[instance] == NULL)
    {
        I2C_DRV_MasterIRQHandler(instance);
    }
    else
    {
        I2C_DRV_SlaveIRQHandler(instance);
    }

}
#endif

/*******************************************************************************
 * EOF
 ******************************************************************************/
