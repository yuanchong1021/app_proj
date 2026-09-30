/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file spi_slave_driver.c
 * @version 1.4.0
 */

/*!
 * @page misra_violations MISRA-C:2012 violations list
 *
 * PRQA S 0326 Rule 11.6: Cast between a pointer to void and an integral type.
 *
 */

#include "spi_slave_driver.h"
#include "interrupt_manager.h"
#include "spi_hw_access.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#if defined(FEATURE_SPI_HAS_DMA_ENABLE) && (FEATURE_SPI_HAS_DMA_ENABLE > 0U)

/* Callback for DMA transfer done.*/
static void SPI_DRV_SlaveCompleteDMATransfer(void *parameter, dma_chn_status_t status);

#endif /* FEATURE_SPI_HAS_DMA_ENABLE */

/*******************************************************************************
 * Code
 ******************************************************************************/
/*FUNCTION**********************************************************************
*
* Function Name : SPI_DRV_SlaveGetDefaultConfig
* Description   : Initializes a structured provided by user with the configuration
* of an interrupt based SPI transfer.
* Implements : SPI_DRV_SlaveGetDefaultConfig_Activity
*
*END**************************************************************************/
void SPI_DRV_SlaveGetDefaultConfig(spi_slave_config_t *spiConfig)
{
    DEV_ASSERT(spiConfig != NULL);

    spiConfig->whichPcs = SPI_PCS0;
    spiConfig->pcsPolarity = SPI_ACTIVE_LOW;
    spiConfig->bitcount = 8U;
    spiConfig->clkPhase = SPI_CLOCK_PHASE_1ST_EDGE;
    spiConfig->clkPolarity = SPI_SCK_ACTIVE_HIGH;
    spiConfig->lsbFirst = false;
    spiConfig->transferType = SPI_USING_INTERRUPTS;
#if FEATURE_SPI_HAS_DMA_ENABLE
    spiConfig->rxDMAChannel = 0;
    spiConfig->txDMAChannel = 0;
#endif
    spiConfig->callback = NULL;
    spiConfig->callbackParam = NULL;
}

/*
 * Implements : SPI_DRV_SlaveInit_Activity
 */
status_t SPI_DRV_SlaveInit(uint32_t instance, spi_state_t *spiState, const spi_slave_config_t *slaveConfig)
{
    DEV_ASSERT(instance < SPI_INSTANCE_COUNT);
    DEV_ASSERT(spiState != NULL);
    DEV_ASSERT(slaveConfig != NULL);
    SPI_Type *base = g_spiBase[instance];
    status_t errorCode = STATUS_SUCCESS;

    spiState->lsb = slaveConfig->lsbFirst;
    spiState->bitsPerFrame = slaveConfig->bitcount;
    spiState->transferType = slaveConfig->transferType;
    spiState->isBlocking = false;
#if FEATURE_SPI_HAS_DMA_ENABLE
    /* Store DMA channels numbers used for DMA transfer */
    spiState->rxDMAChannel = slaveConfig->rxDMAChannel;
    spiState->txDMAChannel = slaveConfig->txDMAChannel;
#endif
    /* Store callback */
    spiState->callback = slaveConfig->callback;
    spiState->callbackParam = slaveConfig->callbackParam;
    /* Calculate the bytes/frame for spiState->bytesPerFrame. */
    spiState->bytesPerFrame = (uint16_t)((spiState->bitsPerFrame + 7U) / 8U);
    /* Due to DMA limitations frames of 3 bytes/frame will be internally handled as 4 bytes/frame. */
    if (spiState->bytesPerFrame == 3U)
    {
        spiState->bytesPerFrame = 4U;
    }
    /* Due to some limitations all frames bigger than 4 bytes/frame must be composed only from 4 bytes chunks. */
    if (spiState->bytesPerFrame > 4U)
    {
        spiState->bytesPerFrame = (((spiState->bytesPerFrame - 1U) / 4U) + 1U) * 4U;
    }
    spiState->isTransferInProgress = false;
    /* Initialize the semaphore */
    errorCode = OSIF_SemaCreate(&(spiState->spiSemaphore), 0);
    DEV_ASSERT(errorCode == STATUS_SUCCESS);
    g_spiStatePtr[instance] = spiState;

    /* Configure registers */
    SPI_Init(base);

    /* Configure spi to slave mode */
    (void)SPI_SetMasterSlaveMode(base, SPI_SLAVE);
#if !defined(FEATURE_SPI_LITE_VERSION)
    /* Set Pin settings */
    (void)SPI_SetPinConfigMode(base, SPI_SDI_IN_SDO_OUT, SPI_DATA_OUT_TRISTATE, true);
#else
    /* Set Pin settings */
    (void)SPI_SetPinConfigMode(base, SPI_SDI_IN_SDO_OUT, SPI_DATA_OUT_TRISTATE, false);
#endif /* FEATURE_SPI_LITE_VERSION */
#if !defined(FEATURE_SPI_LITE_VERSION)
    /* Calculate the FIFO size for the SPI */
    SPI_GetFifoSizes(base, &(spiState->fifoSize));
#endif /* FEATURE_SPI_LITE_VERSION */

    /* Set polarity */
    (void)SPI_SetPcsPolarityMode(base, slaveConfig->whichPcs, slaveConfig->pcsPolarity);

    /* Write the TCR for this transfer */
    spi_tx_cmd_config_t txCmdCfg = { .frameSize = spiState->bitsPerFrame,
                                     .width = slaveConfig->width,
                                     .txMask = false,
                                     .rxMask = false,
                                     .byteSwap = false,
                                     .lsbFirst = slaveConfig->lsbFirst,
                                     .clkPhase = slaveConfig->clkPhase,
                                     .clkPolarity = slaveConfig->clkPolarity,
                                     .whichPcs = slaveConfig->whichPcs };

    /* Write to the TX CMD register */
    SPI_SetTxCommandReg(base, &txCmdCfg);
    SPI_Enable(base);
    /* Enable the interrupt source */
    INT_SYS_EnableIRQ(g_spiIrqId[instance]);

    return errorCode;
}

/*
 * Implements : SPI_DRV_SlaveDeinit_Activity
 */
status_t SPI_DRV_SlaveDeinit(uint32_t instance)
{
    DEV_ASSERT(instance < SPI_INSTANCE_COUNT);
    DEV_ASSERT(g_spiStatePtr[instance] != NULL);
    /* Instantiate local variable of type spi_master_state_t and point to global state */
    const spi_state_t *spiState = (spi_state_t *)g_spiStatePtr[instance];
    SPI_Type *base = g_spiBase[instance];
    status_t errorCode = STATUS_SUCCESS;

    /* Check if a transfer is still in progress */
    DEV_ASSERT(spiState->isTransferInProgress == false);
    /* Destroy the semaphore */
    errorCode = OSIF_SemaDestroy(&(spiState->spiSemaphore));
    DEV_ASSERT(errorCode == STATUS_SUCCESS);
    /* Reset the SPI registers to their default state, including disabling the SPI */
    SPI_Init(base);

    /* Disable the interrupt*/
    INT_SYS_DisableIRQ(g_spiIrqId[instance]);

    /* Clear the state pointer. */
    g_spiStatePtr[instance] = NULL;

    return errorCode;
}

/*
 * Implements : SPI_DRV_SlaveTransferBlocking_Activity
 */
status_t SPI_DRV_SlaveTransferBlocking(uint32_t instance,
                                       const uint8_t *sendBuffer,
                                       uint8_t *receiveBuffer,
                                       uint16_t transferByteCount,
                                       uint32_t timeout)
{
    DEV_ASSERT(instance < SPI_INSTANCE_COUNT);
    DEV_ASSERT(g_spiStatePtr[instance] != NULL);
    spi_state_t *state = (spi_state_t *)g_spiStatePtr[instance];
    status_t status;

    /* Check if SPI module isn't busy */
    if (state->isTransferInProgress == true)
    {
        status = STATUS_BUSY;
    }else{
        
        /* Dummy wait to ensure the semaphore is 0, no need to check result */
        (void)OSIF_SemaWait(&(state->spiSemaphore), 0);
        state->isBlocking = true;
    
        status = SPI_DRV_SlaveTransfer(instance, sendBuffer, receiveBuffer, transferByteCount);
        if (status != STATUS_SUCCESS)
        {
            state->isBlocking = false;
            SPI_DRV_DisableTEIEInterrupts(instance);
        }else {

            /* As this is a synchronous transfer, wait until the transfer is complete.*/
            status = OSIF_SemaWait(&(state->spiSemaphore), timeout);
        
            if (status == STATUS_TIMEOUT)
            {
                /* Set isBlocking variable to false to avoid dummy semaphore post. */
                state->isBlocking = false;
                /* Complete transfer. */
                (void)SPI_DRV_SlaveAbortTransfer(instance);
                status = STATUS_TIMEOUT;
            }else {
                SPI_DRV_DisableTEIEInterrupts(instance);
            }
        }
    }

    return status;
}

/*
 * Implements : SPI_DRV_SlaveTransfer_Activity
 */
status_t
SPI_DRV_SlaveTransfer(uint32_t instance, const uint8_t *sendBuffer, uint8_t *receiveBuffer, uint16_t transferByteCount)
{
    DEV_ASSERT(instance < SPI_INSTANCE_COUNT);
    DEV_ASSERT(g_spiStatePtr[instance] != NULL);
    DEV_ASSERT(!((sendBuffer == NULL) && (receiveBuffer == NULL)));
    status_t status = STATUS_SUCCESS;
    SPI_Type *base = g_spiBase[instance];
    spi_state_t *state = (spi_state_t *)g_spiStatePtr[instance];
    state->dummy = 0xFFU;
#if FEATURE_SPI_HAS_DMA_ENABLE
    dma_transfer_size_t dmaTransferSize = DMA_TRANSFER_SIZE_1B;
    const uint8_t *buffer;
#endif

    /* The number of transferred bytes should be divisible by frame size */
    if ((uint16_t)(transferByteCount % state->bytesPerFrame) != (uint16_t)0)
    {
        status = STATUS_ERROR;
    }else {

        /* Check if SPI module isn't busy */
        if (state->isTransferInProgress == true)
        {
            status = STATUS_BUSY;
        }else {

            /* Initialize the status of the current transfer */
            state->status = SPI_TRANSFER_OK;
#if !defined(FEATURE_SPI_LITE_VERSION)
            /* Clean RX and TX buffers */
            SPI_SetFlushFifoCmd(base, true, true);
            /* The second flush command is used to avoid the case when one word is still in shifter. */
            SPI_SetFlushFifoCmd(base, true, true);
#endif /* FEATURE_SPI_LITE_VERSION */
            /* Clear all interrupts sources */
            (void)SPI_ClearStatusFlag(base, SPI_ALL_STATUS);
            /* Enable fault interrupts sources */
            SPI_SetIntMode(base, SPI_TRANSMIT_ERROR, true);
            SPI_SetIntMode(base, SPI_RECEIVE_ERROR, true);
            /* Fill out the other members of the run-time state structure. */
            state->txBuff = (const uint8_t *)sendBuffer;
            state->rxBuff = (uint8_t *)receiveBuffer;
            if (state->transferType == SPI_USING_INTERRUPTS)
            {
                if (state->txBuff != NULL)
                {
                    state->txCount = transferByteCount;
                    SPI_ClearTxmaskBit(base);
                }
                else
                {
                    state->txCount = 0;
                    SPI_SetTxmskBit(base);
                }
        
                if (state->rxBuff != NULL)
                {
                    state->rxCount = transferByteCount;
                    SPI_ClearRxmaskBit(base);
                }
                else
                {
                    state->rxCount = 0;
                    SPI_SetRxmskBit(base);
                }
        
                state->txFrameCnt = 0;
                state->rxFrameCnt = 0;
                state->isPcsContinuous = false;
#if !defined(FEATURE_SPI_LITE_VERSION)
                /* Configure watermarks */
                SPI_SetRxWatermarks(base, 0U);
                SPI_SetTxWatermarks(base, 2U);
#endif /* FEATURE_SPI_LITE_VERSION */
        
                state->isTransferInProgress = true;
                /* Enable interrupts for RX and TX only if it's necessary */
                if (state->txBuff != NULL)
                {
                    SPI_SetIntMode(base, SPI_TX_DATA_FLAG, true);
                }
                if (state->rxBuff != NULL)
                {
                    SPI_SetIntMode(base, SPI_RX_DATA_FLAG, true);
                }
            }
#if FEATURE_SPI_HAS_DMA_ENABLE
            else
            {
                /* Configure watermarks */
                SPI_SetRxWatermarks(base, 0U);
                SPI_SetTxWatermarks(base, 3U);
                /* When SPI use DMA frames with 3 bytes size are not accepted. */
                switch (state->bytesPerFrame)
                {
                    case 1:
                        dmaTransferSize = DMA_TRANSFER_SIZE_1B;
                        break;
                    case 2:
                        dmaTransferSize = DMA_TRANSFER_SIZE_2B;
                        break;
                    default:
                        /* Frame size is 4 */
                        dmaTransferSize = DMA_TRANSFER_SIZE_4B;
                        break;
                }
        
                if (receiveBuffer != NULL)
                {
                    state->rxCount = transferByteCount;
                    buffer = receiveBuffer;
                }
                else
                {
                    state->rxCount = 0U;
                    /* if there is no data to receive, use dummy data as destination for DMA transfer */
                    buffer = (uint8_t *)(&(state->dummy));
                }
                (void)DMA_DRV_ConfigMultiBlockTransfer(state->rxDMAChannel,
                                                       DMA_TRANSFER_PERIPH2MEM,
                                                       (uint32_t)(&(base->DATA)),
                                                       (uint32_t)buffer,
                                                       dmaTransferSize,
                                                       (uint32_t)1U << (uint8_t)(dmaTransferSize),
                                                       (uint32_t)transferByteCount /
                                                           (uint32_t)((uint32_t)1U << (uint8_t)(dmaTransferSize)),
                                                       true);
                if (receiveBuffer == NULL)
                {
                    /* if there is no data to receive, don't increment destination offset */
                    DMA_DRV_SetDestOffset(state->rxDMAChannel, 0);
                }
        
                if (sendBuffer != NULL)
                {
                    state->txCount = transferByteCount;
                    buffer = sendBuffer;
                }
                else
                {
                    state->txCount = 0U;
                    /* if there is no data to send, use dummy data as source for DMA transfer */
                    buffer = (uint8_t *)(&(state->dummy));
                }
                (void)DMA_DRV_ConfigMultiBlockTransfer(state->txDMAChannel,
                                                       DMA_TRANSFER_MEM2PERIPH,
                                                       (uint32_t)buffer,
                                                       (uint32_t)(&(base->DATA)),
                                                       dmaTransferSize,
                                                       (uint32_t)1U << (uint8_t)(dmaTransferSize),
                                                       (uint32_t)transferByteCount /
                                                           (uint32_t)((uint32_t)1U << (uint8_t)(dmaTransferSize)),
                                                       true);
                if (sendBuffer == NULL)
                {
                    /* if there is no data to transmit, don't increment source offset */
                    DMA_DRV_SetSrcOffset(state->txDMAChannel, 0);
                }
        
                (void)DMA_DRV_InstallCallback(state->rxDMAChannel, (SPI_DRV_SlaveCompleteDMATransfer), (void *)(instance)); /* PRQA S 0326 */
        
                state->isTransferInProgress = true;
        
                /* Start RX channel */
                (void)DMA_DRV_StartChannel(state->rxDMAChannel);
                /* Start TX channel */
                (void)DMA_DRV_StartChannel(state->txDMAChannel);
                /* Enable SPI DMA requests */
                SPI_SetRxDmaCmd(base, true);
                SPI_SetTxDmaCmd(base, true);
            }
#endif /* FEATURE_SPI_HAS_DMA_ENABLE */
        }
    }
    return status;
}

void SPI_DRV_SlaveIRQHandler(uint32_t instance)
{
    SPI_Type *base = g_spiBase[instance];
    spi_state_t *spiState = (spi_state_t *)g_spiStatePtr[instance];

    /* If an error is detected the transfer will be aborted */
    if ((bool)SPI_GetStatusFlag(base, SPI_TRANSMIT_ERROR) && (spiState->txBuff != NULL))
    {
        (void)SPI_DRV_SlaveAbortTransfer(instance);
        spiState->status = SPI_TRANSMIT_FAIL;
    }else {

        if (SPI_GetStatusFlag(base, SPI_RECEIVE_ERROR) && (spiState->rxBuff != NULL))
        {
            (void)SPI_DRV_SlaveAbortTransfer(instance);
            spiState->status = SPI_RECEIVE_FAIL;
        }else {

            /* Receive data */
            if (SPI_GetStatusFlag(base, SPI_RX_DATA_FLAG))
            {
                if ((spiState->rxCount != (uint8_t)0))
                {
                    SPI_DRV_ReadRXBuffer(instance);
                }
            }
            /* Transmit data */
            if (SPI_GetStatusFlag(base, SPI_TX_DATA_FLAG))
            {
                if ((spiState->txCount != (uint8_t)0))
                {
                    SPI_DRV_FillupTxBuffer(instance);
                }
            }
            /* If all bytes are sent disable interrupt TDF */
            if (spiState->txCount == (uint8_t)0)
            {
                SPI_SetIntMode(base, SPI_TX_DATA_FLAG, false);
            }
            /* If all bytes are received disable interrupt RDF */
            if (spiState->rxCount == (uint8_t)0)
            {
                SPI_SetIntMode(base, SPI_RX_DATA_FLAG, false);
            }
            if (spiState->rxCount == (uint8_t)0)
            {
                if (spiState->txCount == (uint8_t)0)
                {
                    /* Disable fault interrupts sources */
                    SPI_SetIntMode(base, SPI_TRANSMIT_ERROR, false);
                    SPI_SetIntMode(base, SPI_RECEIVE_ERROR, false);
        
                    /* Call the callback if it is defined */
                    if (spiState->callback != NULL)
                    {
                        spiState->callback(spiState, SPI_EVENT_END_TRANSFER, spiState->callbackParam);
                    }
        
                    /* If the transfer is blocking post the semaphore */
                    if (spiState->isBlocking == true)
                    {
                        (void)OSIF_SemaPost(&(spiState->spiSemaphore));
                        spiState->isBlocking = false;
                    }
        
                    /* Update internal state of the transfer */
                    spiState->isTransferInProgress = false;
                }
            }
        }
    }
}

/*
 * Implements : SPI_DRV_SlaveAbortTransfer_Activity
 */
status_t SPI_DRV_SlaveAbortTransfer(uint32_t instance)
{
    DEV_ASSERT(instance < SPI_INSTANCE_COUNT);
    DEV_ASSERT(g_spiStatePtr[instance] != NULL);
    SPI_Type *base = g_spiBase[instance];
    spi_state_t *state = (spi_state_t *)g_spiStatePtr[instance];

    if (state->transferType == SPI_USING_INTERRUPTS)
    {
        /* Disable interrupts */
        SPI_SetIntMode(base, SPI_TX_DATA_FLAG, false);
        SPI_SetIntMode(base, SPI_RX_DATA_FLAG, false);
    }
    else
    {
#if !defined(FEATURE_SPI_LITE_VERSION)
        /* Disable SPI DMA request */
        SPI_SetRxDmaCmd(base, false);
        SPI_SetTxDmaCmd(base, false);
#endif /* FEATURE_SPI_LITE_VERSION */
    }

    SPI_DRV_DisableTEIEInterrupts(instance);

    state->isTransferInProgress = false;
#if !defined(FEATURE_SPI_LITE_VERSION)
    /* Clean RX and TX buffers */
    SPI_SetFlushFifoCmd(base, true, true);
    /* The second flush command is used to avoid the case when one word is still in shifter. */
    SPI_SetFlushFifoCmd(base, true, true);
#endif /* FEATURE_SPI_LITE_VERSION */
    if (state->isBlocking == true)
    {
        (void)OSIF_SemaPost(&(state->spiSemaphore));
        state->isBlocking = false;
    }
    return STATUS_SUCCESS;
}

/*
 * Implements : SPI_DRV_SlaveGetTransferStatus_Activity
 */
status_t SPI_DRV_SlaveGetTransferStatus(uint32_t instance, uint32_t *bytesRemained)
{
    DEV_ASSERT(instance < SPI_INSTANCE_COUNT);
    DEV_ASSERT(g_spiStatePtr[instance] != NULL);
    status_t status = STATUS_SUCCESS;
    const spi_state_t *spiState = (spi_state_t *)g_spiStatePtr[instance];

    /* Fill in the bytes transferred.*/
    if (bytesRemained != NULL)
    {
        *bytesRemained = spiState->txCount;
    }
    if (spiState->status == SPI_TRANSFER_OK)
    {
        status = (status_t)(spiState->isTransferInProgress ? STATUS_BUSY : STATUS_SUCCESS);
    }
    else
    {
        status = STATUS_ERROR;
    }

    return status;
}

#if FEATURE_SPI_HAS_DMA_ENABLE

/*!
 * @brief Finish up a transfer DMA.
 * The main purpose of this function is to create a function compatible with DMA callback type
 */
static void SPI_DRV_SlaveCompleteDMATransfer(void *parameter, dma_chn_status_t status)
{
    uint32_t instance = (uint32_t)parameter; /* PRQA S 0326 */
    spi_state_t *spiState = (spi_state_t *)g_spiStatePtr[instance];

    (void)status;
    (void)SPI_DRV_SlaveAbortTransfer(instance);

    /* Check RX and TX DMA channels. */
    if (DMA_DRV_GetChannelStatus(spiState->txDMAChannel) == DMA_CHN_ERROR)
    {
        spiState->status = SPI_TRANSMIT_FAIL;
    }
    if (DMA_DRV_GetChannelStatus(spiState->rxDMAChannel) == DMA_CHN_ERROR)
    {
        spiState->status = SPI_RECEIVE_FAIL;
    }

    if (spiState->callback != NULL)
    {
        spiState->callback(spiState, SPI_EVENT_END_TRANSFER, spiState->callbackParam);
    }
}

#endif /* FEATURE_SPI_HAS_DMA_ENABLE */
/*******************************************************************************
 * EOF
 ******************************************************************************/
