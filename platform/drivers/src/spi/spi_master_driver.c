/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file spi_master_driver.c
 * @version 1.4.0
 */

/*!
 * @page misra_violations MISRA-C:2012 violations list
 *
 * PRQA S 3387 Rule 13.3: A full expression containing an increment (++) or 
 *                        decrement (--) operator should have no potential side effects 
 *                        other than that caused by the increment or decrement operator.
 *
 * PRQA S 0326 Rule 11.6: Cast between a pointer to void and an integral type.
 */

#include "spi_master_driver.h"
#include "interrupt_manager.h"
#include "spi_hw_access.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* This function initialize a new SPI transfer */
static status_t SPI_DRV_MasterStartTransfer(uint32_t instance,
                                            const uint8_t *sendBuffer,
                                            uint8_t *receiveBuffer,
                                            uint16_t transferByteCount);

/* This function cleans up state structure and hardware after a transfer is complete .*/
static void SPI_DRV_MasterCompleteTransfer(uint32_t instance);

#if defined(FEATURE_SPI_HAS_DMA_ENABLE) && (FEATURE_SPI_HAS_DMA_ENABLE > 0U)

/* Callback for DMA transfer done.*/
static void SPI_DRV_MasterCompleteDMATransfer(void *parameter, dma_chn_status_t status);

/* Callback for RX DMA channel*/
static void SPI_DRV_MasterCompleteRX(void *parameter, dma_chn_status_t status);

#endif /* FEATURE_SPI_HAS_DMA_ENABLE */

/*******************************************************************************
 * Code
 ******************************************************************************/

/*FUNCTION**********************************************************************
*
* Function Name : SPI_DRV_MasterGetDefaultConfig
* Description   : Initializes a structured provided by user with the configuration
* of an interrupt based SPI transfer. Source clock for SPI is configured to
* 8MHz. If the applications uses other frequency is necessary to update spiSrcClk field.
* Implements : SPI_DRV_MasterGetDefaultConfig_Activity
*
*END**************************************************************************/
void SPI_DRV_MasterGetDefaultConfig(spi_master_config_t *spiConfig)
{
    DEV_ASSERT(spiConfig != NULL);

    spiConfig->bitsPerSec = 50000U;
    spiConfig->whichPcs = SPI_PCS0;
    spiConfig->pcsPolarity = SPI_ACTIVE_LOW;
    spiConfig->isPcsContinuous = false;
    spiConfig->bitcount = 8U;
    spiConfig->clkPhase = SPI_CLOCK_PHASE_1ST_EDGE;
    spiConfig->clkPolarity = SPI_SCK_ACTIVE_HIGH;
    spiConfig->lsbFirst = false;
    spiConfig->transferType = SPI_USING_INTERRUPTS;
#if defined(FEATURE_SPI_HAS_DMA_ENABLE) && (FEATURE_SPI_HAS_DMA_ENABLE > 0U)
    spiConfig->rxDMAChannel = 0;
    spiConfig->txDMAChannel = 0;
#endif
    spiConfig->callback = NULL;
    spiConfig->callbackParam = NULL;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : SPI_DRV_MasterInit
 * Description   : Initializes a SPI instance for interrupt driven master mode operation.
 *
 * This function uses an interrupt-driven method for transferring data.
 * In this function, the term "spiConfig" is used to indicate the SPI device for which the SPI
 * master is communicating.
 * This function initializes the run-time state structure to track the ongoing
 * transfers, un-gates the clock to the SPI module, resets the SPI module,
 * configures the IRQ state structure, enables the module-level interrupt to the core, and
 * enables the SPI module.
 * Implements : SPI_DRV_MasterInit_Activity
 *
 *END**************************************************************************/
status_t SPI_DRV_MasterInit(uint32_t instance, spi_state_t *spiState, const spi_master_config_t *spiConfig)
{
    DEV_ASSERT(instance < SPI_INSTANCE_COUNT);
    DEV_ASSERT(spiState != NULL);
    DEV_ASSERT(spiConfig != NULL);
    SPI_Type *base = g_spiBase[instance];
    status_t errorCode = STATUS_SUCCESS;

    /* Save runtime structure pointers so irq handler can point to the correct state structure */
    g_spiStatePtr[instance] = spiState;
    /* Reset the SPI registers to their default state */
    SPI_Init(base);
    /* Set for master mode */
    (void)SPI_SetMasterSlaveMode(base, SPI_MASTER);
#if !defined(FEATURE_SPI_LITE_VERSION)
    /* Set Pin configuration such that SDO=out and SDI=in */
    (void)SPI_SetPinConfigMode(base, SPI_SDI_IN_SDO_OUT, SPI_DATA_OUT_TRISTATE, true);
#else
    /* Set Pin configuration such that SDO=out and SDI=in */
    (void)SPI_SetPinConfigMode(base, SPI_SDI_IN_SDO_OUT, SPI_DATA_OUT_TRISTATE, false);
#endif /* FEATURE_SPI_LITE_VERSION */
#if !defined(FEATURE_SPI_LITE_VERSION)
    /* Calculate the FIFO size for the SPI */
    SPI_GetFifoSizes(base, &(spiState->fifoSize));
#endif /* FEATURE_SPI_LITE_VERSION */

    /* Configure bus for this device. If NULL is passed, we assume the caller has
     * preconfigured the bus and doesn't wish to re-configure it again for this transfer.
     * Do nothing for calculatedBaudRate. If the user wants to know the calculatedBaudRate
     * then they can call this function separately.
     */
    errorCode = SPI_DRV_MasterConfigureBus(instance, spiConfig, NULL);
    if (errorCode == STATUS_SUCCESS)
    {
        /* When TX is null the value sent on the bus will be 0 */
        spiState->dummy = 0;
        /* Initialize the semaphore */
        errorCode = OSIF_SemaCreate(&(spiState->spiSemaphore), 0);
        DEV_ASSERT(errorCode == STATUS_SUCCESS);
        /* Enable the interrupt */
        INT_SYS_EnableIRQ(g_spiIrqId[instance]);
        /* Finally, enable SPI */
        SPI_Enable(base);
    }
    return errorCode;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : SPI_DRV_MasterDeinit
 * Description   : Shuts down a SPI instance.
 *
 * This function resets the SPI peripheral, gates its clock, and disables the interrupt to
 * the core.  It first checks to see if a transfer is in progress and if so returns an error
 * status.
 * Implements : SPI_DRV_MasterDeinit_Activity
 *
 *END**************************************************************************/
status_t SPI_DRV_MasterDeinit(uint32_t instance)
{
    DEV_ASSERT(instance < SPI_INSTANCE_COUNT);
    DEV_ASSERT(g_spiStatePtr[instance] != NULL);
    /* Instantiate local variable of type spi_state_t and point to global state */
    const spi_state_t *spiState = g_spiStatePtr[instance];
    SPI_Type *base = g_spiBase[instance];
    status_t errorCode = STATUS_SUCCESS;

    /* Check if a transfer is still in progress */
    DEV_ASSERT(spiState->isTransferInProgress == false);

    /* Reset the SPI registers to their default state, inlcuding disabling the SPI */
    SPI_Init(base);
    /* Disable the interrupt */
    INT_SYS_DisableIRQ(g_spiIrqId[instance]);
    /* Clear the state pointer. */
    g_spiStatePtr[instance] = NULL;

    /* Destroy the semaphore */
    errorCode = OSIF_SemaDestroy(&(spiState->spiSemaphore));
    DEV_ASSERT(errorCode == STATUS_SUCCESS);
    return errorCode;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : SPI_DRV_MasterSetDelay
 * Description   : Configures the SPI master mode bus timing delay options.
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
 * Implements : SPI_DRV_MasterSetDelay_Activity
 *
 *END**************************************************************************/
status_t
SPI_DRV_MasterSetDelay(uint32_t instance, uint32_t delayBetwenTransfers, uint32_t delaySCKtoPCS, uint32_t delayPCStoSCK)
{
    DEV_ASSERT(instance < SPI_INSTANCE_COUNT);

    /* Instantiate local variable of type SPI_Type and point to global state */
    SPI_Type *base = g_spiBase[instance];
    status_t errorCode = STATUS_SUCCESS;

    /* Disable module */
    errorCode = SPI_Disable(base);
    if (errorCode == STATUS_SUCCESS)
    {
        (void)SPI_SetDelay(base, SPI_SCK_TO_PCS, delaySCKtoPCS);
        (void)SPI_SetDelay(base, SPI_PCS_TO_SCK, delayPCStoSCK);
        (void)SPI_SetDelay(base, SPI_BETWEEN_TRANSFER, delayBetwenTransfers);
        /* Enable module */
        SPI_Enable(base);
    }

    return errorCode;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : SPI_DRV_MasterConfigureBus
 * Description   : Configures the SPI port physical parameters to access a device on the bus when
 *                 the LSPI instance is configured for interrupt operation.
 *
 * In this function, the term "spiConfig" is used to indicate the SPI device for which the SPI
 * master is communicating. This is an optional function as the spiConfig parameters are
 * normally configured in the initialization function or the transfer functions, where these various
 * functions would call the configure bus function.
 * The user can pass in a different spiConfig structure to the transfer function which contains
 * the parameters for the SPI bus to allow for communication to a different SPI device
 * (the transfer function then calls this function). However, the user also has the option to call
 * this function directly especially to get the calculated baud rate, at which point they may pass
 * in NULL for the spiConfig structure in the transfer function (assuming they have called this
 * configure bus function first).
 * Implements : SPI_DRV_MasterConfigureBus_Activity
 *
 *END**************************************************************************/
status_t
SPI_DRV_MasterConfigureBus(uint32_t instance, const spi_master_config_t *spiConfig, uint32_t *calculatedBaudRate)
{
    DEV_ASSERT(instance < SPI_INSTANCE_COUNT);
    DEV_ASSERT(spiConfig != NULL);
    DEV_ASSERT(g_spiStatePtr[instance] != NULL);
    status_t status = STATUS_SUCCESS;
    /* Instantiate local variable of type spi_state_t and point to global state */
    spi_state_t *spiState = g_spiStatePtr[instance];
    SPI_Type *base = g_spiBase[instance];
    clock_names_t instanceClkName = s_spiClkNames[instance];
    uint32_t baudRate;

    /* The Transmit Command Register (TCR) Prescale value is calculated as part of the baud rate
       calculation. The value is stored in the run-time state structure for later programming
       in the TCR. */
    uint32_t tcrPrescaleValue;

    /* First, per the spec, we need to disable the SPI module before setting the delay */
    status = SPI_Disable(base);
    if(status == STATUS_SUCCESS)
    {
        /* Check the bitcount to make sure it falls within the boundary conditions */
        if (spiConfig->bitcount > 4096U)
        {
            status = STATUS_ERROR;
        }else{

            /* Get the SPI clock as configured in the clock manager */
            (void)CLOCK_SYS_GetFreq(instanceClkName, &(spiState->spiSrcClk));
        
            /* Configure internal state structure for SPI */
            spiState->bitsPerFrame = spiConfig->bitcount;
            spiState->isPcsContinuous = spiConfig->isPcsContinuous;
            spiState->lsb = spiConfig->lsbFirst;
            /* Save transfer type DMA/Interrupt */
            spiState->transferType = spiConfig->transferType;
            /* Update transfer status */
            spiState->isTransferInProgress = false;
            spiState->isBlocking = false;
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
#if defined(FEATURE_SPI_HAS_DMA_ENABLE) && (FEATURE_SPI_HAS_DMA_ENABLE > 0U)
            /* Store DMA channel number used in transfer */
            spiState->rxDMAChannel = spiConfig->rxDMAChannel;
            spiState->txDMAChannel = spiConfig->txDMAChannel;
#endif
            /* Store callback */
            spiState->callback = spiConfig->callback;
            spiState->callbackParam = spiConfig->callbackParam;
            /* Configure the desired PCS polarity */
            (void)SPI_SetPcsPolarityMode(base, spiConfig->whichPcs, spiConfig->pcsPolarity);
            /* Set up the baud rate */
            baudRate = SPI_SetBaudRate(base, spiConfig->bitsPerSec, spiState->spiSrcClk, &tcrPrescaleValue);
            /* Enable sampling point delay */
            SPI_SetSamplingPoint(base, true);
            /* Now, re-enable the SPI module */
            SPI_Enable(base);
            /* If the baud rate return is "0", it means there was an error */
            if (baudRate == (uint32_t)0)
            { 
                status = STATUS_ERROR;
            }else{

                /* If the user wishes to know the calculated baud rate, then pass it back */
                if (calculatedBaudRate != NULL)
                {
                    *calculatedBaudRate = baudRate;
                }
                /* Write the TCR for this transfer. */
                spi_tx_cmd_config_t txCmdCfg = { .frameSize = spiState->bitsPerFrame,
                                                    .width = spiConfig->width,
                                                    .txMask = false,
                                                    .rxMask = false,
                                                    .contCmd = false,
                                                    .contTransfer = spiConfig->isPcsContinuous,
                                                    .byteSwap = false,
                                                    .lsbFirst = spiConfig->lsbFirst,
                                                    .whichPcs = spiConfig->whichPcs,
                                                    .preDiv = tcrPrescaleValue,
                                                    .clkPhase = spiConfig->clkPhase,
                                                    .clkPolarity = spiConfig->clkPolarity };
                SPI_SetTxCommandReg(base, &txCmdCfg);
            }
        }
    }

    return status;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : SPI_DRV_MasterTransferPolling
 * Description   : Performs an interrupt driven blocking SPI master mode transfer.
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
 * Implements : SPI_DRV_MasterTransferPolling_Activity
 *
 *END**************************************************************************/
status_t SPI_DRV_MasterTransferPolling(uint32_t instance,
                                       const uint8_t *sendBuffer,
                                       uint8_t *receiveBuffer,
                                       uint16_t transferByteCount)
{
    DEV_ASSERT(instance < SPI_INSTANCE_COUNT);
    DEV_ASSERT(g_spiStatePtr[instance] != NULL);
    /* Instantiate local variable of type spi_state_t and point to global state */
    spi_state_t *spiState = g_spiStatePtr[instance];
    SPI_Type *base = g_spiBase[instance];
    status_t status = STATUS_SUCCESS;


    /* If the transfer count is zero, then return immediately.*/
    if (transferByteCount == (uint16_t)0)
    {
        status = STATUS_SUCCESS;
    }else{
        
        /* Check if another transfer is in progress */
        if (SPI_GetStatusFlag(base, SPI_MODULE_BUSY))
        {
            status = STATUS_BUSY;
        }else{
            
#if !defined(FEATURE_SPI_LITE_VERSION)
            /* Clean RX and TX buffers */
            SPI_SetFlushFifoCmd(base, true, true);
            /* The second flush command is used to avoid the case when one word is still in shifter. */
            SPI_SetFlushFifoCmd(base, true, true);
            if (spiState->isPcsContinuous == true)
            {
                SPI_SetContBit(base);
            }
#endif /* FEATURE_SPI_LITE_VERSION */
        
            /* Configure rxCount depending on transfer type.*/
            if (receiveBuffer == NULL)
            {
                spiState->rxCount = 0;
                SPI_SetRxmskBit(base);
            }
            else
            {
                spiState->rxCount = transferByteCount;
                SPI_ClearRxmaskBit(base);
            }
        
#if !defined(FEATURE_SPI_LITE_VERSION)
            /* Configure watermarks */
            SPI_SetRxWatermarks(base, 0U);
            SPI_SetTxWatermarks(base, 2U);
#endif /* FEATURE_SPI_LITE_VERSION */
            /* Fill out the other members of the run-time state structure. */
            spiState->txBuff = (const uint8_t *)sendBuffer;
            spiState->rxBuff = (uint8_t *)receiveBuffer;
            spiState->txFrameCnt = 0;
            spiState->rxFrameCnt = 0;
            spiState->txCount = transferByteCount;
            /*For continuous mode an extra word must be written to negate the PCS */
            if (spiState->isPcsContinuous == true)
            {
                spiState->txCount++; /* PRQA S 3387 */
            }
            while (spiState->txCount != (uint16_t)0)
            {
                /* Receive data */
                if (SPI_GetStatusFlag(base, SPI_RX_DATA_FLAG))
                {
                    if (spiState->rxCount != (uint16_t)0)
                    {
                        SPI_DRV_ReadRXBuffer(instance);
                    }
                }
                /* Transmit data */
                if (SPI_GetStatusFlag(base, SPI_TX_DATA_FLAG))
                {
                    if ((spiState->txCount != (uint16_t)0))
                    {
                        SPI_DRV_FillupTxBuffer(instance);
                    }
                }
            }
            while (spiState->rxCount != (uint16_t)0)
            {
                /* Read the last word from the RX FIFO */
                if (SPI_GetStatusFlag(base, SPI_RX_DATA_FLAG))
                {
                    SPI_DRV_ReadRXBuffer(instance);
                }
            }
            /* Wait all transfer ends */
            while (true == SPI_GetStatusFlag(base, SPI_MODULE_BUSY))
            {
            }
        }
    }

    return status;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : SPI_DRV_MasterTransferBlocking
 * Description   : Performs an interrupt driven blocking SPI master mode transfer.
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
 * Implements : SPI_DRV_MasterTransferBlocking_Activity
 *
 *END**************************************************************************/
status_t SPI_DRV_MasterTransferBlocking(uint32_t instance,
                                        const uint8_t *sendBuffer,
                                        uint8_t *receiveBuffer,
                                        uint16_t transferByteCount,
                                        uint32_t timeout)
{
    DEV_ASSERT(instance < SPI_INSTANCE_COUNT);
    DEV_ASSERT(g_spiStatePtr[instance] != NULL);
    /* Instantiate local variable of type spi_state_t and point to global state */
    spi_state_t *spiState = g_spiStatePtr[instance];
    SPI_Type *base = g_spiBase[instance];
    status_t status = STATUS_SUCCESS;
    /* If the transfer count is zero, then return immediately.*/
    if (transferByteCount == (uint16_t)0)
    {
        status = STATUS_SUCCESS;
    }else{
        
        /* Check if another transfer is in progress */
        if (SPI_GetStatusFlag(base, SPI_MODULE_BUSY))
        {
            status = STATUS_BUSY;
        }else{
            
            /* Dummy wait to ensure the semaphore is 0, no need to check result */
            (void)OSIF_SemaWait(&(spiState->spiSemaphore), 0);
            spiState->isBlocking = true;
        
            status = SPI_DRV_MasterStartTransfer(instance, sendBuffer, receiveBuffer, transferByteCount);
            /* Start the transfer process, if it returns an error code, return this back to user */
            if (status != STATUS_SUCCESS)
            {
                /* Disable interrupt requests*/
                SPI_SetIntMode(base, SPI_TX_DATA_FLAG, false);
                SPI_SetIntMode(base, SPI_RX_DATA_FLAG, false);
        
                SPI_DRV_DisableTEIEInterrupts(instance);
                SPI_SetIntMode(base, SPI_TRANSFER_COMPLETE, false);
                (void)SPI_ClearStatusFlag(base, SPI_TRANSFER_COMPLETE);
        
                spiState->isBlocking = false;
            }else{
                
                /* As this is a synchronous transfer, wait until the transfer is complete.*/
                status = OSIF_SemaWait(&(spiState->spiSemaphore), timeout);
            
                /* If a timeout occurs, stop the transfer by setting the isTransferInProgress to false and
                 * disabling interrupts, then return the timeout error status.
                 */
                if (status == STATUS_TIMEOUT)
                {
                    /* Set isBlocking variable to false to avoid dummy semaphore post. */
                    spiState->isBlocking = false;
                    /* Complete transfer. */
                    SPI_DRV_MasterCompleteTransfer(instance);
                    status = STATUS_TIMEOUT;
                }else{
                    
                    SPI_DRV_DisableTEIEInterrupts(instance);
                    SPI_SetIntMode(base, SPI_TRANSFER_COMPLETE, false);
                    (void)SPI_ClearStatusFlag(base, SPI_TRANSFER_COMPLETE);          
                }
            }
        }
    }

    return status;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : SPI_DRV_MasterTransfer
 * Description   : Performs an interrupt driven non-blocking SPI master mode transfer.
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
 * Implements : SPI_DRV_MasterTransfer_Activity
 *
 *END**************************************************************************/
status_t
SPI_DRV_MasterTransfer(uint32_t instance, const uint8_t *sendBuffer, uint8_t *receiveBuffer, uint16_t transferByteCount)
{
    DEV_ASSERT(instance < SPI_INSTANCE_COUNT);
    DEV_ASSERT(g_spiStatePtr[instance] != NULL);
    status_t status = STATUS_SUCCESS;
    /* If the transfer count is zero, then return immediately.*/
    if (transferByteCount == (uint16_t)0)
    {
        status = STATUS_SUCCESS;
    }else{
        
        /* Start the transfer process, if it returns an error code, return this back to user */
        status = SPI_DRV_MasterStartTransfer(instance, sendBuffer, receiveBuffer, transferByteCount);
    }

    /* Else, return immediately as this is an async transfer */
    return status;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : SPI_DRV_MasterGetTransferStatus
 * Description   : Returns whether the previous interrupt driven transfer is completed.
 *
 * When performing an a-sync (non-blocking) transfer, the user can call this function to ascertain
 * the state of the current transfer: in progress (or busy) or complete (success).
 * In addition, if the transfer is still in progress, the user can get the number of words that
 * should be receive.
 * Implements : SPI_DRV_MasterGetTransferStatus_Activity
 *
 *END**************************************************************************/
status_t SPI_DRV_MasterGetTransferStatus(uint32_t instance, uint32_t *bytesRemained)
{
    DEV_ASSERT(instance < SPI_INSTANCE_COUNT);
    DEV_ASSERT(g_spiStatePtr[instance] != NULL);
    status_t status = STATUS_SUCCESS;
    /* Instantiate local variable of type spi_state_t and point to global state */
    const spi_state_t *spiState = g_spiStatePtr[instance];
    /* Fill in the bytes transferred.*/
    if (bytesRemained != NULL)
    {
        *bytesRemained = spiState->rxCount;
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

/*FUNCTION**********************************************************************
 *
 * Function Name : SPI_DRV_MasterAbortTransfer
 * Description   : Terminates an interrupt driven asynchronous transfer early.
 *
 * During an a-sync (non-blocking) transfer, the user has the option to terminate the transfer early
 * if the transfer is still in progress.
 * Implements : SPI_DRV_MasterAbortTransfer_Activity
 *END**************************************************************************/
status_t SPI_DRV_MasterAbortTransfer(uint32_t instance)
{
    DEV_ASSERT(instance < SPI_INSTANCE_COUNT);
    DEV_ASSERT(g_spiStatePtr[instance] != NULL);
    /* Stop the running transfer. */
    SPI_DRV_MasterCompleteTransfer(instance);
#if !defined(FEATURE_SPI_LITE_VERSION)
    SPI_Type *base = g_spiBase[instance];
    SPI_SetFlushFifoCmd(base, true, true);
    /* The second flush command is used to avoid the case when one word is still in shifter. */
    SPI_SetFlushFifoCmd(base, true, true);
#endif /* FEATURE_SPI_LITE_VERSION */
    return STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : SPI_DRV_SetPcs
 * Description   : Select the chip to communicate with.
 *
 *
 * The main purpose of this function is to set the PCS and the appropriate polarity.
 * Implements : SPI_DRV_SetPcs_Activity
 *END**************************************************************************/
status_t SPI_DRV_SetPcs(uint32_t instance, spi_which_pcs_t whichPcs, spi_signal_polarity_t polarity)
{
    DEV_ASSERT(instance < SPI_INSTANCE_COUNT);
    DEV_ASSERT(g_spiStatePtr[instance] != NULL);
#if defined(FEATURE_SPI_HAS_EXTERNAL_DEVICES_SELECTION) && (FEATURE_SPI_HAS_EXTERNAL_DEVICES_SELECTION > 4)
    DEV_ASSERT((uint32_t)whichPcs < FEATURE_SPI_HAS_EXTERNAL_DEVICES_SELECTION);
#else
    DEV_ASSERT((uint32_t)whichPcs < 4U);
#endif

    SPI_Type *base = g_spiBase[instance];
    status_t status = STATUS_SUCCESS;

    if (SPI_Disable(base) != STATUS_SUCCESS)
    {
        status = STATUS_ERROR;
    }else{

        status = SPI_SetPcsPolarityMode(base, whichPcs, polarity);
        if (status == STATUS_SUCCESS)
        {
            SPI_Enable(base);
            SPI_SetPcs(base, whichPcs);
        }
    }

    return status;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : SPI_DRV_MasterStartTransfer
 * Description   : Configure a non-blocking transfer.
 *
 * The number of transferByteCount must be divided by number of bytes/frame.
 * The sendBuffer must be not NULL, but receiveBuffer can be NULL.
 *
 *END**************************************************************************/
static status_t SPI_DRV_MasterStartTransfer(uint32_t instance,
                                            const uint8_t *sendBuffer,
                                            uint8_t *receiveBuffer,
                                            uint16_t transferByteCount)
{
    DEV_ASSERT(instance < SPI_INSTANCE_COUNT);
    DEV_ASSERT(g_spiStatePtr[instance] != NULL);
    status_t status = STATUS_SUCCESS;
    /* Instantiate local variable of type dspi_master_state_t and point to global state */
    spi_state_t *spiState = g_spiStatePtr[instance];
    SPI_Type *base = g_spiBase[instance];
#if defined(FEATURE_SPI_HAS_DMA_ENABLE) && (FEATURE_SPI_HAS_DMA_ENABLE > 0U)
    dma_transfer_size_t dmaTransferSize = DMA_TRANSFER_SIZE_1B;
#endif

    /* Check that we're not busy. */
    if (SPI_GetStatusFlag(base, SPI_MODULE_BUSY))
    {
        status = STATUS_BUSY;
    }else {

        /* Verify if the number of bytes is divided by number of bytes/frame. */
        if ((transferByteCount % spiState->bytesPerFrame) != (uint16_t)0)
        {
            status = STATUS_ERROR;
        }else{
            
#if !defined(FEATURE_SPI_LITE_VERSION)
                /* Clean RX and TX buffers */
                SPI_SetFlushFifoCmd(base, true, true);
                /* The second flush command is used to avoid the case when one word is still in shifter. */
                SPI_SetFlushFifoCmd(base, true, true);
                if (spiState->isPcsContinuous == true)
                {
                    SPI_SetContBit(base);
                }
#endif /* FEATURE_SPI_LITE_VERSION */
            
                spiState->status = SPI_TRANSFER_OK;
                /* Clear all interrupts sources */
                (void)SPI_ClearStatusFlag(base, SPI_ALL_STATUS);
                /* Enable fault interrupts sources */
                SPI_SetIntMode(base, SPI_TRANSMIT_ERROR, true);
                if (receiveBuffer != NULL)
                {
                    SPI_SetIntMode(base, SPI_RECEIVE_ERROR, true);
                }
            
                /* Configure rxCount depending on transfer type.*/
                if (receiveBuffer == NULL)
                {
                    spiState->rxCount = 0;
                    SPI_SetRxmskBit(base);
                }
                else
                {
                    spiState->rxCount = transferByteCount;
                    SPI_ClearRxmaskBit(base);
                }
            
#if !defined(FEATURE_SPI_LITE_VERSION)
                /* Configure watermarks */
                SPI_SetRxWatermarks(base, 0U);
                SPI_SetTxWatermarks(base, 2U);
#endif /* FEATURE_SPI_LITE_VERSION */
            
                if (spiState->transferType == SPI_USING_INTERRUPTS)
                {
                    /* Fill out the other members of the run-time state structure. */
                    spiState->txBuff = (const uint8_t *)sendBuffer;
                    spiState->rxBuff = (uint8_t *)receiveBuffer;
                    spiState->txFrameCnt = 0;
                    spiState->rxFrameCnt = 0;
                    spiState->txCount = transferByteCount;
                    /*For continuous mode an extra word must be written to negate the PCS */
                    if (spiState->isPcsContinuous == true)
                    {
                        spiState->txCount++; /* PRQA S 3387 */
                    }
            
                    /* Update transfer status */
                    spiState->isTransferInProgress = true;
                    /* Enable RDF interrupt if RX buffer is not NULL. */
                    if (spiState->rxBuff != NULL)
                    {
                        SPI_SetIntMode(base, SPI_RX_DATA_FLAG, true);
                    }
                    /* Enable the TDF and RDF interrupt. */
                    SPI_SetIntMode(base, SPI_TX_DATA_FLAG, true);
                }
#if defined(FEATURE_SPI_HAS_DMA_ENABLE) && (FEATURE_SPI_HAS_DMA_ENABLE > 0U)
                else
                {
                    /* When SPI use DMA frames with 3 bytes size are not accepted. */
                    switch (spiState->bytesPerFrame)
                    {
                        case 1:
                            dmaTransferSize = DMA_TRANSFER_SIZE_1B;
                            break;
                        case 2:
                            dmaTransferSize = DMA_TRANSFER_SIZE_2B;
                            break;
                        case 4:
                            dmaTransferSize = DMA_TRANSFER_SIZE_4B;
                            break;
                        default:
                            dmaTransferSize = DMA_TRANSFER_SIZE_4B;
                            break;
                    }
                    /* Configure TX DMA channel */
                    if (sendBuffer != NULL)
                    {
                        (void)DMA_DRV_ConfigMultiBlockTransfer(spiState->txDMAChannel,
                                                               DMA_TRANSFER_MEM2PERIPH,
                                                               (uint32_t)sendBuffer,
                                                               (uint32_t)(&(base->DATA)),
                                                               dmaTransferSize,
                                                               (uint32_t)1U << (uint8_t)(dmaTransferSize),
                                                               (uint32_t)transferByteCount /
                                                                   (uint32_t)((uint32_t)1U << (uint8_t)(dmaTransferSize)),
                                                               true);
                    }
                    else
                    {
                        (void)DMA_DRV_ConfigMultiBlockTransfer(spiState->txDMAChannel,
                                                               DMA_TRANSFER_PERIPH2PERIPH,
                                                               (uint32_t)(&(spiState->dummy)),
                                                               (uint32_t)(&(base->DATA)),
                                                               dmaTransferSize,
                                                               (uint32_t)1U << (uint8_t)(dmaTransferSize),
                                                               (uint32_t)transferByteCount /
                                                                   (uint32_t)((uint32_t)1U << (uint8_t)(dmaTransferSize)),
                                                               true);
                    }
                    /* Configure RX DMA channel if is used in current transfer. */
                    if (receiveBuffer != NULL)
                    {
                        (void)DMA_DRV_ConfigMultiBlockTransfer(spiState->rxDMAChannel,
                                                               DMA_TRANSFER_PERIPH2MEM,
                                                               (uint32_t)(&(base->DATA)),
                                                               (uint32_t)receiveBuffer,
                                                               dmaTransferSize,
                                                               (uint32_t)1U << (uint8_t)(dmaTransferSize),
                                                               (uint32_t)transferByteCount /
                                                                   (uint32_t)((uint32_t)1U << (uint8_t)(dmaTransferSize)),
                                                               true);
                        (void)DMA_DRV_InstallCallback(spiState->rxDMAChannel, (SPI_DRV_MasterCompleteRX), (void *)(instance)); /* PRQA S 0326 */
                        /* Start RX channel */
                        (void)DMA_DRV_StartChannel(spiState->rxDMAChannel);
                    }
            
                    /* If RX buffer is null the transfer is done when all bytes were sent. */
                    (void)DMA_DRV_InstallCallback(spiState->txDMAChannel, (SPI_DRV_MasterCompleteDMATransfer), (void *)(instance)); /* PRQA S 0326 */
            
                    /* Start TX channel */
                    (void)DMA_DRV_StartChannel(spiState->txDMAChannel);
                    /* Update transfer status */
                    spiState->isTransferInProgress = true;
                    /* Enable SPI DMA request */
                    if (receiveBuffer != NULL)
                    {
                        SPI_SetRxDmaCmd(base, true);
                    }
                    SPI_SetTxDmaCmd(base, true);
                }
#endif
        }
    }

    return status;
}

/*!
 * @brief Finish up a transfer.
 * Cleans up after a transfer is complete. Interrupts are disabled, and the SPI module
 * is disabled. This is not a public API as it is called from other driver functions.
 */
static void SPI_DRV_MasterCompleteTransfer(uint32_t instance)
{
    /* instantiate local variable of type dspi_master_state_t and point to global state */
    spi_state_t *spiState = g_spiStatePtr[instance];
    SPI_Type *base = g_spiBase[instance];
    /* The transfer is complete.*/
    spiState->isTransferInProgress = false;
    if (spiState->transferType == SPI_USING_DMA)
    {
#if !defined(FEATURE_SPI_LITE_VERSION)
        /* Disable SPI DMA request */
        SPI_SetRxDmaCmd(base, false);
        SPI_SetTxDmaCmd(base, false);
#endif /* FEATURE_SPI_LITE_VERSION */
    }
    else
    {
        /* Disable (clear) interrupt requests */
        SPI_SetIntMode(base, SPI_RX_DATA_FLAG, false);
        SPI_SetIntMode(base, SPI_TX_DATA_FLAG, false);
    }

    SPI_DRV_DisableTEIEInterrupts(instance);
    SPI_SetIntMode(base, SPI_TRANSFER_COMPLETE, false);
    (void)SPI_ClearStatusFlag(base, SPI_TRANSFER_COMPLETE);
    if (spiState->isBlocking == true)
    {
        (void)OSIF_SemaPost(&(spiState->spiSemaphore));
        spiState->isBlocking = false;
    }
    if (spiState->callback != NULL)
    {
        spiState->callback(spiState, SPI_EVENT_END_TRANSFER, spiState->callbackParam);
    }
}

#if defined(FEATURE_SPI_HAS_DMA_ENABLE) && (FEATURE_SPI_HAS_DMA_ENABLE > 0U)

/*!
 * @brief Finish up a transfer DMA.
 * The main purpose of this function is to create a function compatible with DMA callback type
 */
static void SPI_DRV_MasterCompleteDMATransfer(void *parameter, dma_chn_status_t status)
{
    uint32_t instance = (uint32_t)parameter; /* PRQA S 0326 */
    spi_state_t *spiState = g_spiStatePtr[instance];
    SPI_Type *base = g_spiBase[instance];

    if (status == DMA_CHN_ERROR)
    {
        (void)SPI_DRV_MasterAbortTransfer(instance);
        spiState->status = SPI_TRANSMIT_FAIL;
    }
    else
    {
        if (spiState->isPcsContinuous == true)
        {
            SPI_ClearContBit(base);
        }

        /* Enable transfer complete flag interrupt to catch the end of the transfer. */
        spiState->txCount = 0;
        spiState->rxCount = 0;
        SPI_SetIntMode(base, SPI_TRANSFER_COMPLETE, true);
    }
}

#endif /* FEATURE_SPI_HAS_DMA_ENABLE */

#if defined(FEATURE_SPI_HAS_DMA_ENABLE) && (FEATURE_SPI_HAS_DMA_ENABLE > 0U)

/*!
 * @brief Check if errors are detected on RX channel
 * The main purpose of this function is to check DMA errors on rx channel
 */
static void SPI_DRV_MasterCompleteRX(void *parameter, dma_chn_status_t status)
{
    uint32_t instance = (uint32_t)parameter; /* PRQA S 0326 */
    spi_state_t *spiState = g_spiStatePtr[instance];

    if (status == DMA_CHN_ERROR)
    {
        (void)SPI_DRV_MasterAbortTransfer(instance);
        spiState->status = SPI_TRANSMIT_FAIL;
    }
}

#endif /* FEATURE_SPI_HAS_DMA_ENABLE */

/*!
 * @brief Interrupt handler for SPI master mode.
 * This handler uses the buffers stored in the spi_state_t structs to transfer data.
 * This is not a public API as it is called whenever an interrupt occurs.
 */
void SPI_DRV_MasterIRQHandler(uint32_t instance)
{
    /* Instantiate local variable of type dspi_master_state_t and point to global state */
    spi_state_t *spiState = g_spiStatePtr[instance];
    SPI_Type *base = g_spiBase[instance];

    /* If an error is detected the transfer will be aborted */
    if ((bool)SPI_GetStatusFlag(base, SPI_TRANSMIT_ERROR) && (spiState->txBuff != NULL))
    {
        (void)SPI_DRV_MasterAbortTransfer(instance);
        (void)SPI_ClearStatusFlag(base, SPI_TRANSMIT_ERROR);
        spiState->status = SPI_TRANSMIT_FAIL;
    }else {

        if (SPI_GetStatusFlag(base, SPI_RECEIVE_ERROR) && (spiState->rxBuff != NULL))
        {
            (void)SPI_DRV_MasterAbortTransfer(instance);
            (void)SPI_ClearStatusFlag(base, SPI_RECEIVE_ERROR);
            spiState->status = SPI_RECEIVE_FAIL;
        }else {

            /* RECEIVE IRQ handler: Check read buffer only if there are remaining bytes to read. */
            if (SPI_GetStatusFlag(base, SPI_RX_DATA_FLAG))
            {
                if (spiState->rxCount != (uint16_t)0)
                {
                    SPI_DRV_ReadRXBuffer(instance);
                }
            }
            /* Transmit data */
            if (SPI_GetStatusFlag(base, SPI_TX_DATA_FLAG))
            {
                if ((spiState->txCount != (uint16_t)0))
                {
                    SPI_DRV_FillupTxBuffer(instance);
                }
            }
            if (spiState->txCount == (uint16_t)0)
            {
                /* Disable TX flag. Software buffer is empty.*/
                SPI_SetIntMode(base, SPI_TX_DATA_FLAG, false);
                SPI_SetIntMode(base, SPI_TRANSFER_COMPLETE, true);
                /* Check if we're done with this transfer.*/
                if (spiState->rxCount == (uint16_t)0)
                {
                    if (SPI_GetStatusFlag(base, SPI_TRANSFER_COMPLETE) == true)
                    {
                        SPI_DRV_MasterCompleteTransfer(instance);
                    }
                }
            }
        }
    }
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
