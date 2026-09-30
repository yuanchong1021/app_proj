/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file uart_driver.c
 * @version 1.4.0
 */

/*!
 * @page misra_violations MISRA-C:2012 violations list
 *
 * PRQA S 0326 Rule 11.6: Cast between a pointer to void and an integral type.
 *
 * PRQA S 3387 Rule 13.3: A full expression containing an increment (++) or 
 *                        decrement (--) operator should have no potential side effects 
 *                        other than that caused by the increment or decrement operator.
 */

#include <stddef.h>
#include <stdbool.h>
#include "uart_hw_access.h"
#include "uart_irq.h"
#include "clock_manager.h"
#include <stdint.h>

/******************************************************************************
* Define
*******************************************************************************/

#define UART_DEFAULT_TX_FIFO_WATERMARK (0x02)
/*******************************************************************************
 * Variables
 ******************************************************************************/

/* Pointer to uart runtime state structure */
static uart_state_t *s_uartStatePtr[UART_INSTANCE_COUNT] = FEATURE_UART_STATE_PTR;

/* Table of base addresses for uart instances. */
static UART_Type *const s_uartBase[UART_INSTANCE_COUNT] = UART_BASE_PTRS;

/* Table to save UART enum numbers defined in CMSIS files. */
static const IRQn_Type s_uartRxTxIrqId[UART_INSTANCE_COUNT] = UART_RX_TX_IRQS;

/* Table to save UART clock names as defined in clock manager. */
static const clock_names_t s_uartClkNames[UART_INSTANCE_COUNT] = UART_CLOCK_NAMES;

/*******************************************************************************
 * Private Functions
 ******************************************************************************/
static status_t UART_DRV_StartSendDataUsingInt(uint32_t instance, const uint8_t *txBuff, uint32_t txSize);
static void UART_DRV_CompleteSendDataUsingInt(uint32_t instance);
static status_t UART_DRV_StartReceiveDataUsingInt(uint32_t instance, uint8_t *rxBuff, uint32_t rxSize);
static void UART_DRV_CompleteReceiveDataUsingInt(uint32_t instance);
#if FEATURE_UART_HAS_DMA_ENABLE
static void UART_DRV_StopTxDma(uint32_t instance);
static void UART_DRV_StopRxDma(uint32_t instance);
static status_t UART_DRV_StartSendDataUsingDma(uint32_t instance, const uint8_t *txBuff, uint32_t txSize);
static void UART_DRV_TxDmaCallback(void *parameter, dma_chn_status_t status);
static status_t UART_DRV_StartReceiveDataUsingDma(uint32_t instance, uint8_t *rxBuff, uint32_t rxSize);
static void UART_DRV_RxDmaCallback(void *parameter, dma_chn_status_t status);
#endif
static void UART_DRV_PutData(uint32_t instance);
static void UART_DRV_GetData(uint32_t instance);
static void UART_DRV_RxIrqHandler(uint32_t instance);
static void UART_DRV_TxEmptyIrqHandler(uint32_t instance);
static void UART_DRV_TxCompleteIrqHandler(uint32_t instance);
static void UART_DRV_ErrIrqHandler(uint32_t instance);
static void UART_DRV_SetErrorInterrupts(uint32_t instance, bool enable);

/*******************************************************************************
 * Code
 ******************************************************************************/

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_GetDefaultConfig
 * Description   : Initializes the UART configuration structure with
 *                 default values.
 *
 * Implements    : UART_DRV_GetDefaultConfig_Activity
 *END**************************************************************************/
void UART_DRV_GetDefaultConfig(uart_user_config_t *uartUserConfig)
{
    DEV_ASSERT(uartUserConfig != NULL);

    uartUserConfig->transferType = UART_USING_INTERRUPTS;
    uartUserConfig->baudRate = 9600U;
    uartUserConfig->parityMode = UART_PARITY_DISABLED;
    uartUserConfig->stopBitCount = UART_ONE_STOP_BIT;
    uartUserConfig->bitCountPerChar = UART_8_BITS_PER_CHAR;
    uartUserConfig->rxDMAChannel = 0U;
    uartUserConfig->txDMAChannel = 0U;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_Init
 * Description   : This function initializes a UART instance for operation.
 * This function will initialize the run-time state structure to keep track of
 * the on-going transfers, ungate the clock to the UART module, initialize the
 * module to user defined settings and default settings, configure the IRQ state
 * structure and enable the module-level interrupt to the core, and enable the
 * UART module transmitter and receiver.
 * The following is an example of how to set up the uart_state_t and the
 * uart_user_config_t parameters and how to call the UART_DRV_Init function
 * by passing in these parameters:
 *    uart_user_config_t uartConfig;
 *    uartConfig.baudRate = 9600;
 *    uartConfig.bitCountPerChar = UART_8_BITS_PER_CHAR;
 *    uartConfig.parityMode = UART_PARITY_DISABLED;
 *    uartConfig.stopBitCount = UART_ONE_STOP_BIT;
 *    uartConfig.transferType = UART_USING_INTERRUPTS;
 *    uart_state_t uartState;
 *    UART_DRV_Init(instance, &uartState, &uartConfig);
 *
 * Implements    : UART_DRV_Init_Activity
 *END**************************************************************************/
status_t UART_DRV_Init(uint32_t instance, uart_state_t *uartStatePtr, const uart_user_config_t *uartUserConfig)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);
    DEV_ASSERT(uartStatePtr != NULL);
    DEV_ASSERT(uartUserConfig != NULL);

    status_t osStatusRxSem;
    status_t osStatusTxSem;
    status_t retVal = STATUS_SUCCESS;

    UART_Type *base = s_uartBase[instance];
    uint32_t idx;
    uint32_t uartSourceClock;
    clock_names_t instanceClkName = s_uartClkNames[instance];

    /* Get the UART clock as configured in the clock manager */
    (void)CLOCK_SYS_GetFreq(instanceClkName, &uartSourceClock);

    /* Check if current instance is clock gated off. */
    DEV_ASSERT(uartSourceClock > 0U);

    /* Check if current instance is already initialized. */
    DEV_ASSERT(s_uartStatePtr[instance] == NULL);

#if FEATURE_UART_HAS_DMA_ENABLE
    /* In DMA mode, only 8-bits chars are supported */
    DEV_ASSERT((uartUserConfig->transferType != UART_USING_DMA) ||
               (uartUserConfig->bitCountPerChar <= UART_8_BITS_PER_CHAR));
#endif

    /* For 10 bits per char, parity bit cannot be enabled */
    DEV_ASSERT((uartUserConfig->bitCountPerChar != UART_10_BITS_PER_CHAR) ||
               (uartUserConfig->parityMode == UART_PARITY_DISABLED));

    /* Clear the state struct for this instance. */
    uint8_t *clearStructPtr = (uint8_t *)uartStatePtr;
    for (idx = 0; idx < sizeof(uart_state_t); idx++)
    {
        clearStructPtr[idx] = 0;
    }

    /* Save runtime structure pointer.*/
    s_uartStatePtr[instance] = uartStatePtr;

    /* Save whether to enable idle interrupt. */
    uartStatePtr->idleErrorIntEnable = uartUserConfig->idleErrorIntEnable;

    /* Save the transfer information for runtime retrieval */
    uartStatePtr->transferType = uartUserConfig->transferType;
    uartStatePtr->bitCountPerChar = uartUserConfig->bitCountPerChar;
#if FEATURE_UART_HAS_DMA_ENABLE
    uartStatePtr->rxDMAChannel = uartUserConfig->rxDMAChannel;
    uartStatePtr->txDMAChannel = uartUserConfig->txDMAChannel;
#endif

    /* initialize the UART instance */
    UART_Init(base);

    /* initialize the parameters of the UART config structure with desired data */
    (void)UART_DRV_SetBaudRate(instance, uartUserConfig->baudRate);

    if (uartUserConfig->parityMode != UART_PARITY_DISABLED)
    {
        UART_SetBitCountPerChar(base, uartUserConfig->bitCountPerChar, true);
    }
    else
    {
        UART_SetBitCountPerChar(base, uartUserConfig->bitCountPerChar, false);
    }
    UART_SetParityMode(base, uartUserConfig->parityMode);
    UART_SetStopBitCount(base, uartUserConfig->stopBitCount);
    (void)UART_DRV_SetLineIdleDetect(instance, UART_LINE_IDLE_SIZE_2_CHAR, false);

    /* initialize last driver operation status */
    uartStatePtr->transmitStatus = STATUS_SUCCESS;
    uartStatePtr->receiveStatus = STATUS_SUCCESS;

    /* Create the synchronization objects */
    osStatusRxSem = OSIF_SemaCreate(&uartStatePtr->rxComplete, 0);
    osStatusTxSem = OSIF_SemaCreate(&uartStatePtr->txComplete, 0);
    if ((osStatusRxSem == STATUS_ERROR) || (osStatusTxSem == STATUS_ERROR))
    {
        retVal = STATUS_ERROR;
    }
    
    if (retVal != STATUS_ERROR)
    {
        /* Install UART irq handler */
        INT_SYS_InstallHandler(s_uartRxTxIrqId[instance], g_uartIsr[instance], (isr_t *)0);

        /* Enable UART interrupt. */
        INT_SYS_EnableIRQ(s_uartRxTxIrqId[instance]);  
    }

    return retVal;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_Deinit
 * Description   : This function shuts down the UART by disabling interrupts and
 *                 transmitter/receiver.
 *
 * Implements    : UART_DRV_Deinit_Activity
 *END**************************************************************************/
status_t UART_DRV_Deinit(uint32_t instance)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);

    clock_names_t instanceClkName = s_uartClkNames[instance];
    uint32_t uartSourceClock;
    const UART_Type *base = s_uartBase[instance];
    const uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    (void)CLOCK_SYS_GetFreq(instanceClkName, &uartSourceClock);

    /* Check if current instance is already de-initialized or is gated.*/
    DEV_ASSERT(s_uartStatePtr[instance] != NULL);
    DEV_ASSERT(uartSourceClock > 0U);

    /* Wait until the data is completely shifted out of shift register */
    while (!UART_GetStatusFlag(base, UART_TX_COMPLETE))
    {
    }

    /* ipc software reset */
    CLOCK_DRV_ResetModule(s_uartClkNames[instance]);

    /* Destroy the synchronization objects */
    (void)OSIF_SemaDestroy(&uartState->rxComplete);
    (void)OSIF_SemaDestroy(&uartState->txComplete);

    /* Disable UART interrupt. */
    INT_SYS_DisableIRQ(s_uartRxTxIrqId[instance]);

    /* Restore default handler. */
    INT_SYS_InstallHandler(s_uartRxTxIrqId[instance], DefaultISR, (isr_t *)0);

    /* Clear our saved pointer to the state structure */
    s_uartStatePtr[instance] = NULL;

    return STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_InstallRxCallback
 * Description   : Install receive data callback function.
 *
 * Implements    : UART_DRV_InstallRxCallback_Activity
 *END**************************************************************************/
uart_callback_t UART_DRV_InstallRxCallback(uint32_t instance, uart_callback_t function, void *callbackParam)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);

    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    uart_callback_t currentCallback = uartState->rxCallback;
    uartState->rxCallback = function;
    uartState->rxCallbackParam = callbackParam;

    return currentCallback;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_InstallTxCallback
 * Description   : Install transmit data callback function, pass in NULL pointer
 * as callback will uninstall.
 *
 * Implements    : UART_DRV_InstallTxCallback_Activity
 *END**************************************************************************/
uart_callback_t UART_DRV_InstallTxCallback(uint32_t instance, uart_callback_t function, void *callbackParam)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);

    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    uart_callback_t currentCallback = uartState->txCallback;
    uartState->txCallback = function;
    uartState->txCallbackParam = callbackParam;

    return currentCallback;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_SendDataBlocking
 * Description   : This function sends data out through the UART module using
 * blocking method. The function does not return until the transmit is complete.
 *
 * Implements    : UART_DRV_SendDataBlocking_Activity
 *END**************************************************************************/
status_t UART_DRV_SendDataBlocking(uint32_t instance, const uint8_t *txBuff, uint32_t txSize, uint32_t timeout)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);
    DEV_ASSERT(txBuff != NULL);

    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    status_t retVal = STATUS_SUCCESS;
    status_t syncStatus;

    /* Indicates this is a blocking transaction. */
    uartState->isTxBlocking = true;
    /* Reset signal before transfer */
    (void)OSIF_SemaWait(&(uartState->txComplete), 0);

    DEV_ASSERT((uartState->transferType == UART_USING_INTERRUPTS) || (uartState->transferType == UART_USING_DMA));

    if (uartState->transferType == UART_USING_INTERRUPTS)
    {
        /* Start the transmission process using interrupts */
        retVal = UART_DRV_StartSendDataUsingInt(instance, txBuff, txSize);
    }
#if FEATURE_UART_HAS_DMA_ENABLE
    else
    {
        /* Start the transmission process using DMA */
        retVal = UART_DRV_StartSendDataUsingDma(instance, txBuff, txSize);
    }
#endif

    if (retVal == STATUS_SUCCESS)
    {
        /* Wait until the transmit is complete. */
        syncStatus = OSIF_SemaWait(&uartState->txComplete, timeout);

        /* Finish the transmission if timeout expired */
        if (syncStatus == STATUS_TIMEOUT)
        {
            uartState->isTxBlocking = false;
            uartState->transmitStatus = STATUS_TIMEOUT;

            if (uartState->transferType == UART_USING_INTERRUPTS)
            {
                UART_DRV_CompleteSendDataUsingInt(instance);
            }
#if FEATURE_UART_HAS_DMA_ENABLE
            else
            {
                UART_DRV_StopTxDma(instance);
            }
#endif
        }
    }

    return uartState->transmitStatus;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_SendDataPolling
 * Description   : Send out multiple bytes of data using polling method.
 *
 * Implements    : UART_DRV_SendDataPolling_Activity
 *END**************************************************************************/
status_t UART_DRV_SendDataPolling(uint32_t instance, const uint8_t *txBuff, uint32_t txSize)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);
    DEV_ASSERT(txBuff != NULL);

    UART_Type *base = s_uartBase[instance];
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    const uint8_t *currentTxBuff = txBuff; 
    uint32_t remainingSize = txSize; 
    status_t retVal = STATUS_SUCCESS;

    /* Check the validity of the parameters */
    DEV_ASSERT(txSize > 0U);
    DEV_ASSERT((uartState->bitCountPerChar <= UART_8_BITS_PER_CHAR) || ((txSize & 1U) == 0U));

    /* Check driver is not busy transmitting data from a previous asynchronous call */
    if (uartState->isTxBusy)
    {
        retVal = STATUS_BUSY;
    }

    if (retVal == STATUS_SUCCESS)
    {
        /* Enable the UART transmitter */
        UART_SetTransmitterCmd(base, true);

        while (remainingSize > 0U)
        {
            while (!UART_GetStatusFlag(base, UART_TX_DATA_REG_EMPTY))
            {
            }

            uartState->txBuff = currentTxBuff;
            UART_DRV_PutData(instance);

            if (uartState->bitCountPerChar <= UART_8_BITS_PER_CHAR)
            {
                ++currentTxBuff;
                --remainingSize;
            }
            else
            {
                ++currentTxBuff;
                ++currentTxBuff;
                remainingSize -= 2U;
            }
        }

        /* Disable the UART transmitter */
        UART_SetTransmitterCmd(base, false);
    }
    else
    {

    }

    return retVal;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_SendData
 * Description   : This function sends data out through the UART module using
 * non-blocking method. The function will return immediately after calling this
 * function.
 *
 * Implements    : UART_DRV_SendData_Activity
 *END**************************************************************************/
status_t UART_DRV_SendData(uint32_t instance, const uint8_t *txBuff, uint32_t txSize)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);
    DEV_ASSERT(txBuff != NULL);

    status_t retVal = STATUS_SUCCESS;
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    /* Indicates this is a non-blocking transaction. */
    uartState->isTxBlocking = false;

    DEV_ASSERT((uartState->transferType == UART_USING_INTERRUPTS) || (uartState->transferType == UART_USING_DMA));

    if (uartState->transferType == UART_USING_INTERRUPTS)
    {
        /* Start the transmission process using interrupts */
        retVal = UART_DRV_StartSendDataUsingInt(instance, txBuff, txSize);
    }
#if FEATURE_UART_HAS_DMA_ENABLE
    else
    {
        /* Start the transmission process using DMA */
        retVal = UART_DRV_StartSendDataUsingDma(instance, txBuff, txSize);
    }
#endif

    return retVal;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_GetTransmitStatus
 * Description   : This function returns whether the previous UART transmit has
 * finished. When performing non-blocking transmit, the user can call this
 * function to ascertain the state of the current transmission:
 * in progress (or busy) or complete (success). In addition, if the transmission
 * is still in progress, the user can obtain the number of words that have been
 * currently transferred.
 *
 * Implements    : UART_DRV_GetTransmitStatus_Activity
 *END**************************************************************************/
status_t UART_DRV_GetTransmitStatus(uint32_t instance, uint32_t *bytesRemaining)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);

    const uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    if (bytesRemaining != NULL)
    {
        if (uartState->isTxBusy)
        {
            /* Fill in the bytes not transferred yet. */
            if (uartState->transferType == UART_USING_INTERRUPTS)
            {
                /* In interrupt-based communication, the remaining bytes are retrieved
                 * from the state structure
                 */
                *bytesRemaining = uartState->txSize;
                ;
            }
#if FEATURE_UART_HAS_DMA_ENABLE
            else
            {
                /* In DMA-based communication, the remaining bytes are retrieved
                 * from the current DMA trigger loop count
                 */
                *bytesRemaining = DMA_DRV_GetRemainingTriggerIterationsCount(uartState->txDMAChannel);
            }
#endif
        }
        else
        {
            *bytesRemaining = 0;
        }
    }

    return uartState->transmitStatus;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_AbortSendingData
 * Description   : This function terminates an non-blocking UART transmission
 * early. During a non-blocking UART transmission, the user has the option to
 * terminate the transmission early if the transmission is still in progress.
 *
 * Implements    : UART_DRV_AbortSendingData_Activity
 *END**************************************************************************/
status_t UART_DRV_AbortSendingData(uint32_t instance)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);

    status_t retVal = STATUS_BUSY;
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    /* Check if a transfer is running. */
    if (!uartState->isTxBusy)
    {
        retVal = STATUS_SUCCESS;
        
    }
    else
    {
        /* Uart state is sending */
    }
    
    if (retVal != STATUS_SUCCESS)
    {    /* Update the tx status */
        uartState->transmitStatus = STATUS_UART_ABORTED;

        /* Stop the running transfer. */
        if (uartState->transferType == UART_USING_INTERRUPTS)
        {
            UART_DRV_CompleteSendDataUsingInt(instance);
        }
#if FEATURE_UART_HAS_DMA_ENABLE
        else
        {
            UART_DRV_StopTxDma(instance);
        }
#endif
        retVal = STATUS_SUCCESS;
    }
    else
    {
        /* Uart state is not busy */
    }
    
    return retVal;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_ReceiveDataBlocking
 * Description   : This function receives data from UART module using blocking
 * method, the function does not return until the receive is complete.
 *
 * Implements    : UART_DRV_ReceiveDataBlocking_Activity
 *END**************************************************************************/
status_t UART_DRV_ReceiveDataBlocking(uint32_t instance, uint8_t *rxBuff, uint32_t rxSize, uint32_t timeout)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);
    DEV_ASSERT(rxBuff != NULL);

    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    status_t retVal = STATUS_SUCCESS;
    status_t syncStatus;

    /* Indicates this is a blocking transaction. */
    uartState->isRxBlocking = true;
    /* Reset signal before transfer */
    (void)OSIF_SemaWait(&(uartState->rxComplete), 0);

    DEV_ASSERT((uartState->transferType == UART_USING_INTERRUPTS) || (uartState->transferType == UART_USING_DMA));

    if (uartState->transferType == UART_USING_INTERRUPTS)
    {
        /* Start the reception process using interrupts */
        retVal = UART_DRV_StartReceiveDataUsingInt(instance, rxBuff, rxSize);
    }
#if FEATURE_UART_HAS_DMA_ENABLE
    else
    {
        /* Start the reception process using DMA */
        retVal = UART_DRV_StartReceiveDataUsingDma(instance, rxBuff, rxSize);
    }
#endif

    if (retVal == STATUS_SUCCESS)
    {
        /* Wait until the receive is complete. */
        syncStatus = OSIF_SemaWait(&uartState->rxComplete, timeout);

        /* Finish the reception if timeout expired */
        if (syncStatus == STATUS_TIMEOUT)
        {
            uartState->isRxBlocking = false;
            uartState->receiveStatus = STATUS_TIMEOUT;

            if (uartState->transferType == UART_USING_INTERRUPTS)
            {
                UART_DRV_CompleteReceiveDataUsingInt(instance);
            }
#if FEATURE_UART_HAS_DMA_ENABLE
            else
            {
                UART_DRV_StopRxDma(instance);
            }
#endif
        }
    }

    return uartState->receiveStatus;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_ReceiveDataPolling
 * Description   : Receive multiple bytes of data using polling method.
 *
 * Implements    : UART_DRV_ReceiveDataPolling_Activity
 *END**************************************************************************/
status_t UART_DRV_ReceiveDataPolling(uint32_t instance, uint8_t *rxBuff, uint32_t rxSize)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);
    DEV_ASSERT(rxBuff != NULL);

    status_t retVal = STATUS_SUCCESS;
    status_t tmpState = STATUS_SUCCESS;
    UART_Type *base = s_uartBase[instance];
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    bool statusFlag = false;

    uint8_t *currentRxBuff = rxBuff; 
    uint32_t remainingSize = rxSize; 
    /* Check the validity of the parameters */
    DEV_ASSERT(rxSize > 0U);
    DEV_ASSERT((uartState->bitCountPerChar <= UART_8_BITS_PER_CHAR) || ((rxSize & 1U) == 0U));

    /* Check driver is not busy receiving data from a previous asynchronous call */
    if (uartState->isRxBusy)
    {
        retVal = STATUS_BUSY;
    }

    if (retVal == STATUS_SUCCESS)
    {
         /* Enable the UART receiver */
        UART_SetReceiverCmd((UART_Type *)base, true);

#if defined(FEATURE_UART_FIFO_SIZE) && FEATURE_UART_FIFO_SIZE > 0
        /* Initialize rx fifo */
        UART_ResetRxFifo(base);
        UART_SetRxFifoWatermark(base, 0);
        UART_EnableRxFifo(base, true);
#endif /* FEATURE_UART_FIFO_SIZE > 0 */

        while (remainingSize > 0U)
        {
            while (!UART_GetStatusFlag(base, UART_RX_DATA_REG_FULL))
            {
            }

            uartState->rxBuff = currentRxBuff;
            UART_DRV_GetData(instance);

            if (uartState->bitCountPerChar <= UART_8_BITS_PER_CHAR)
            {
                ++currentRxBuff;
                --remainingSize;
            }
            else
            {
                ++currentRxBuff;
                ++currentRxBuff;
                remainingSize -= 2U;
            }

            /* Check for errors on received data */
            if (UART_GetStatusFlag(base, UART_FRAME_ERR))
            {
                tmpState = STATUS_UART_FRAMING_ERROR;
                /* Disable the UART receiver */
                UART_SetReceiverCmd((UART_Type *)base, false);
                /* Clear the flag */
                (void)UART_ClearStatusFlag(base, UART_FRAME_ERR);
                statusFlag = true;
            }
            else if (UART_GetStatusFlag(base, UART_NOISE_DETECT))
            {
                tmpState = STATUS_UART_NOISE_ERROR;
                /* Disable the UART receiver */
                UART_SetReceiverCmd((UART_Type *)base, false);
                /* Clear the flag */
                (void)UART_ClearStatusFlag(base, UART_NOISE_DETECT);
                statusFlag = true;
            }
            else if (UART_GetStatusFlag(base, UART_PARITY_ERR))
            {
                tmpState = STATUS_UART_PARITY_ERROR;
                /* Disable the UART receiver */
                UART_SetReceiverCmd((UART_Type *)base, false);
                /* Clear the flag */
                (void)UART_ClearStatusFlag(base, UART_PARITY_ERR);
                statusFlag = true;
            }
            else if (UART_GetStatusFlag(base, UART_RX_OVERRUN))
            {
                tmpState = STATUS_UART_RX_OVERRUN;
                /* Disable the UART receiver */
                UART_SetReceiverCmd((UART_Type *)base, false);
                /* Clear the flag */
                (void)UART_ClearStatusFlag(base, UART_RX_OVERRUN);
                statusFlag = true;
            }
            else
            {
                statusFlag = false;
            }

            if (statusFlag)
            {
                break;
            }
        }

        /* Update received status */
        if ((remainingSize == 0U) && (tmpState == STATUS_UART_RX_OVERRUN))
        {
            retVal = STATUS_SUCCESS;
        }
        else
        {
            retVal = tmpState;
        }

        if (retVal == STATUS_SUCCESS)
        {
            /* Disable the UART receiver */
            UART_SetReceiverCmd((UART_Type *)base, false);
        }
#if defined(FEATURE_UART_FIFO_SIZE) && FEATURE_UART_FIFO_SIZE > 0
        /* Reset rx fifo to clear RDRF flag */
        UART_ResetRxFifo(base);
#endif /* FEATURE_UART_FIFO_SIZE > 0 */
    }

    return retVal;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_ReceiveData
 * Description   : This function receives data from UART module using
 * non-blocking method.  This function returns immediately after initiating the
 * receive function. The application has to get the receive status to see when
 * the receive is complete. In other words, after calling non-blocking get
 * function, the application must get the receive status to check if receive
 * is completed or not.
 *
 * Implements    : UART_DRV_ReceiveData_Activity
 *END**************************************************************************/
status_t UART_DRV_ReceiveData(uint32_t instance, uint8_t *rxBuff, uint32_t rxSize)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);
    DEV_ASSERT(rxBuff != NULL);

    status_t retVal = STATUS_SUCCESS;
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    /* Indicates this is a non-blocking transaction. */
    uartState->isRxBlocking = false;

    DEV_ASSERT((uartState->transferType == UART_USING_INTERRUPTS) || (uartState->transferType == UART_USING_DMA));

    if (uartState->transferType == UART_USING_INTERRUPTS)
    {
        /* Start the reception process using interrupts */
        retVal = UART_DRV_StartReceiveDataUsingInt(instance, rxBuff, rxSize);
    }
#if FEATURE_UART_HAS_DMA_ENABLE
    else
    {
        /* Start the reception process using DMA */
        retVal = UART_DRV_StartReceiveDataUsingDma(instance, rxBuff, rxSize);
    }
#endif

    return retVal;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_GetReceiveStatus
 * Description   : This function returns whether the previous UART receive is
 * complete. When performing a non-blocking receive, the user can call this
 * function to ascertain the state of the current receive progress: in progress
 * or complete. In addition, if the receive is still in progress, the user can
 * obtain the number of words that have been currently received.
 *
 * Implements    : UART_DRV_GetReceiveStatus_Activity
 *END**************************************************************************/
status_t UART_DRV_GetReceiveStatus(uint32_t instance, uint32_t *bytesRemaining)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);

    const uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    if (bytesRemaining != NULL)
    {
        if (uartState->isRxBusy)
        {
            /* Fill in the bytes transferred. */
            if (uartState->transferType == UART_USING_INTERRUPTS)
            {
                /* In interrupt-based communication, the remaining bytes are retrieved
                 * from the state structure
                 */
                *bytesRemaining = uartState->rxSize;
            }
#if FEATURE_UART_HAS_DMA_ENABLE
            else
            {
                /* In DMA-based communication, the remaining bytes are retrieved
                 * from the current DMA trigger loop count
                 */
                *bytesRemaining = DMA_DRV_GetRemainingTriggerIterationsCount(uartState->rxDMAChannel);
            }
#endif
        }
        else
        {
            *bytesRemaining = 0;
        }
    }

    return uartState->receiveStatus;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_AbortReceivingData
 * Description   : Terminates a non-blocking receive early.
 *
 * Implements    : UART_DRV_AbortReceivingData_Activity
 *END**************************************************************************/
status_t UART_DRV_AbortReceivingData(uint32_t instance)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);

    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    /* Check if a transfer is running. */
    if (uartState->isRxBusy)
    {
        /* Update the rx status */
        uartState->receiveStatus = STATUS_UART_ABORTED;

        /* Stop the running transfer. */
        if (uartState->transferType == UART_USING_INTERRUPTS)
        {
            UART_DRV_CompleteReceiveDataUsingInt(instance);
        }
#if FEATURE_UART_HAS_DMA_ENABLE
        else
        {
            UART_DRV_StopRxDma(instance);
        }
#endif
    }

    return STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_SetBaudRate
 * Description   : Configures the UART baud rate.
 * In some UART instances the user must disable the transmitter/receiver
 * before calling this function.
 * Generally, this may be applied to all UARTs to ensure safe operation.
 *
 * Implements    : UART_DRV_SetBaudRate_Activity
 *END**************************************************************************/
status_t UART_DRV_SetBaudRate(uint32_t instance, uint32_t desiredBaudRate)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);

    uint16_t sbr, sbrTemp, i;
    uint32_t osr, tempDiff, calculatedBaud, baudDiff, maxOsr;
    uint32_t uartSourceClock;
    clock_names_t instanceClkName = s_uartClkNames[instance];
    UART_Type *base = s_uartBase[instance];
    const uart_state_t *uartState;
    uartState = (uart_state_t *)s_uartStatePtr[instance];
    status_t retVal = STATUS_SUCCESS;

    if (uartState != NULL)
    {
        /* Check if there is an ongoing transfer */
        if (uartState->isTxBusy == true)
        {
            retVal = STATUS_BUSY;
        }
        else if (uartState->isRxBusy == true)
        {
            retVal = STATUS_BUSY;
        }
        else
        {
            /*Nothing to do*/
        }
    }

    if (retVal != STATUS_BUSY)
    {
        /* Get the UART clock as configured in the clock manager */
        (void)CLOCK_SYS_GetFreq(instanceClkName, &uartSourceClock);

        /* Check if current instance is clock gated off. */
        DEV_ASSERT(uartSourceClock > 0U);
        /* Check if the desired baud rate can be configured with the current protocol clock. */
        DEV_ASSERT(uartSourceClock >= (desiredBaudRate * 4U));

        /* This uart instantiation uses a slightly different baud rate calculation
        * The idea is to use the best OSR (over-sampling rate) possible
        * Note, osr is typically hard-set to 16 in other uart instantiations
        * First calculate the baud rate using the minimum OSR possible (4) */
        osr = 4;
        sbr = (uint16_t)(uartSourceClock / (desiredBaudRate * osr));
        calculatedBaud = (uartSourceClock / (osr * sbr));
        if (calculatedBaud > desiredBaudRate)
        {
            baudDiff = calculatedBaud - desiredBaudRate;
        }
        else
        {
            baudDiff = desiredBaudRate - calculatedBaud;
        }
        /* find maximum osr */
        maxOsr = uartSourceClock / desiredBaudRate;
        if (maxOsr > 32U)
        {
            maxOsr = 32U;
        }
        /* loop to find the best osr value possible, one that generates minimum baudDiff
        * iterate through the rest of the supported values of osr */
        if (maxOsr >= 5U)
        {
            for (i = 5U; i <= maxOsr; i++)
            {
                /* calculate the temporary sbr value   */
                sbrTemp = (uint16_t)(uartSourceClock / (desiredBaudRate * i));
                /* calculate the baud rate based on the temporary osr and sbr values */
                calculatedBaud = (uartSourceClock / (i * (uint32_t)sbrTemp));

                if (calculatedBaud > desiredBaudRate)
                {
                    tempDiff = calculatedBaud - desiredBaudRate;
                }
                else
                {
                    tempDiff = desiredBaudRate - calculatedBaud;
                }

                if (tempDiff <= baudDiff)
                {
                    baudDiff = tempDiff;
                    osr = i;       /* update and store the best osr value calculated */
                    sbr = sbrTemp; /* update store the best sbr value calculated */
                }
            }
        }
        /* Check if osr is between 4x and 7x oversampling.
        * If so, then "BOTHEDGE" sampling must be turned on */
        if (osr < 8U)
        {
            UART_EnableBothEdgeSamplingCmd(base);
        }

        /* program the osr value (bit value is one less than actual value) */
        UART_SetOversamplingRatio(base, (osr - 1U));

        /* write the sbr value to the BAUD registers */
        UART_SetBaudRateDivisor(base, sbr);
    }

    return retVal;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_GetBaudRate
 * Description   : Returns the UART configured baud rate.
 *
 * Implements    : UART_DRV_GetBaudRate_Activity
 *END**************************************************************************/
void UART_DRV_GetBaudRate(uint32_t instance, uint32_t *configuredBaudRate)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);
    DEV_ASSERT(configuredBaudRate != NULL);

    uint8_t osr;
    uint16_t sbr;
    uint32_t uartSourceClock;
    clock_names_t instanceClkName = s_uartClkNames[instance];
    const UART_Type *base = s_uartBase[instance];

    /* Get the UART clock as configured in the clock manager */
    (void)CLOCK_SYS_GetFreq(instanceClkName, &uartSourceClock);

    osr = UART_GetOversamplingRatio(base);
    sbr = UART_GetBaudRateDivisor(base);

    *configuredBaudRate = (uartSourceClock / ((osr + 1UL) * sbr));
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_SetTxBuffer
 * Description   : Sets the driver internal reference to the tx buffer.
 *                 Can be called from the tx callback to provide a different
 *                 buffer for continuous transmission.
 *
 * Implements    : UART_DRV_SetTxBuffer_Activity
 *END**************************************************************************/
status_t UART_DRV_SetTxBuffer(uint32_t instance, const uint8_t *txBuff, uint32_t txSize)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);
    DEV_ASSERT(txBuff != NULL);
    DEV_ASSERT(txSize > 0U);

    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    uartState->txBuff = txBuff;
    uartState->txSize = txSize;

    return STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_SetRxBuffer
 * Description   : Sets the driver internal reference to the rx buffer.
 *                 Can be called from the rx callback to provide a different
 *                 buffer for continuous reception.
 *
 * Implements    : UART_DRV_SetRxBuffer_Activity
 *END**************************************************************************/
status_t UART_DRV_SetRxBuffer(uint32_t instance, uint8_t *rxBuff, uint32_t rxSize)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);
    DEV_ASSERT(rxBuff != NULL);
    DEV_ASSERT(rxSize > 0U);

    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    uartState->rxBuff = rxBuff;
    uartState->rxSize = rxSize;

    return STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_SetLineIDLE
 * Description   : Enable Line IDLE detect interrupt
 *
 * Implements    : UART_DRV_SetRxBuffer_Activity
 *END**************************************************************************/
status_t UART_DRV_SetLineIdleDetect(uint32_t instance, uart_line_size_t idleSize, bool intEnable)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);

    UART_Type *base = s_uartBase[instance];
    UART_SetLineIdleSize(base, (uint8_t)idleSize);
    UART_SetIntMode(base, UART_INT_IDLE_LINE, intEnable);
    return STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_IRQHandler
 * Description   : Interrupt handler for UART.
 * This handler uses the buffers stored in the uart_state_t structs to transfer
 * data. This is not a public API as it is called by IRQ whenever an interrupt
 * occurs.
 *
 *END**************************************************************************/
void UART_DRV_IRQHandler(uint32_t instance)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);

    const UART_Type *base = s_uartBase[instance];

    /* Handle receive data full interrupt */
    if (UART_GetIntMode(base, UART_INT_RX_DATA_REG_FULL))
    {
        if (UART_GetStatusFlag(base, UART_RX_DATA_REG_FULL))
        {
            UART_DRV_RxIrqHandler(instance);
        }
    }

    /* Handle error interrupt */
    UART_DRV_ErrIrqHandler(instance);

    /* Handle transmitter data register empty interrupt */
    if (UART_GetIntMode(base, UART_INT_TX_DATA_REG_EMPTY))
    {
        if (UART_GetStatusFlag(base, UART_TX_DATA_REG_EMPTY))
        {
            UART_DRV_TxEmptyIrqHandler(instance);
        }
    }

    /* Handle transmission complete interrupt */
    if (UART_GetIntMode(base, UART_INT_TX_COMPLETE))
    {
        if (UART_GetStatusFlag(base, UART_TX_COMPLETE))
        {
            UART_DRV_TxCompleteIrqHandler(instance);
        }
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_RxIrqHandler
 * Description   : Rx Interrupt handler for UART.
 * This function treats the rx full interrupt.
 *
 *END**************************************************************************/
static void UART_DRV_RxIrqHandler(uint32_t instance)
{
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    uint32_t rxFifoWordCount;

#if defined(FEATURE_UART_FIFO_SIZE) && FEATURE_UART_FIFO_SIZE > 0
    UART_Type *base = s_uartBase[instance];
    rxFifoWordCount = UART_GetWordCountInRxFifo(base);
#else
    rxFifoWordCount = 1;
#endif /* FEATURE_UART_FIFO_SIZE > 0 */

    for (uint32_t i = 0; i < rxFifoWordCount; i++)
    {
        /* Get data and put in receive buffer  */
        UART_DRV_GetData(instance);

        /* Update the internal state */
        if (uartState->bitCountPerChar <= UART_8_BITS_PER_CHAR)
        {
            ++uartState->rxBuff;
            --uartState->rxSize; /* PRQA S 3387 */
        }
        else
        {
            uartState->rxBuff = &uartState->rxBuff[2];
            uartState->rxSize -= 2U;
        }

        /* Check if this was the last byte in the current buffer */
        if (uartState->rxSize == 0U)
        {
            /* Invoke callback if there is one (callback may reset the rx buffer for continuous reception) */
            if (uartState->rxCallback != NULL)
            {
                uartState->rxCallback(uartState, UART_EVENT_RX_FULL, uartState->rxCallbackParam);
            }
        }

        /* Finish reception if this was the last byte received */
        if (uartState->rxSize == 0U)
        {
            /* Complete transfer (disable rx logic) */
            UART_DRV_CompleteReceiveDataUsingInt(instance);

            /* Invoke callback if there is one */
            if (uartState->rxCallback != NULL)
            {
                uartState->rxCallback(uartState, UART_EVENT_END_TRANSFER, uartState->rxCallbackParam);
            }
        }
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_TxEmptyIrqHandler
 * Description   : Tx Empty Interrupt handler for UART.
 * This function treats the tx empty interrupt.
 *
 *END**************************************************************************/
static void UART_DRV_TxEmptyIrqHandler(uint32_t instance)
{
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    UART_Type *base = s_uartBase[instance];
    uint32_t txFifoEmptyCount;
    bool status = false;

#if defined(FEATURE_UART_FIFO_SIZE) && FEATURE_UART_FIFO_SIZE > 0
    uint32_t txFifoWordCount;
    /* Get txfifo word count */
    txFifoWordCount = UART_GetWordCountInTxFifo(base);
    /* Get empty count in tx fifo */
    txFifoEmptyCount = UART_GetTxFifoSize(base) - txFifoWordCount;
#else
    txFifoEmptyCount = 1;
#endif /* FEATURE_UART_FIFO_SIZE > 0 */

    for (uint32_t i = 0; i < txFifoEmptyCount; i++)
    {
        /* Check if there are any more bytes to send */
        if (uartState->txSize > 0U)
        {
            /* Transmit the data */
            UART_DRV_PutData(instance);

            /* Update the internal state */
            if (uartState->bitCountPerChar <= UART_8_BITS_PER_CHAR)
            {
                ++uartState->txBuff;
                --uartState->txSize; /* PRQA S 3387 */
            }
            else
            {
                uartState->txBuff = &uartState->txBuff[2];
                uartState->txSize -= 2U;
            }

            /* Check if this was the last byte in the current buffer */
            if (uartState->txSize == 0U)
            {
                /* Invoke callback if there is one (callback may reset the tx buffer for continuous transmission)*/
                if (uartState->txCallback != NULL)
                {
                    uartState->txCallback(uartState, UART_EVENT_TX_EMPTY, uartState->txCallbackParam);
                }
                
                /* If there's no new data, disable tx empty interrupt and enable transmission complete interrupt */
                if (uartState->txSize == 0U)
                {
                    UART_SetIntMode(base, UART_INT_TX_DATA_REG_EMPTY, false);
                    UART_SetIntMode(base, UART_INT_TX_COMPLETE, true);
                    status = true;
                }
            }
        }
        else
        {
            status = true;
        }

        if (status)
        {
            break;
        }
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_TxCompleteIrqHandler
 * Description   : Tx Complete Interrupt handler for UART.
 * This function treats the tx complete interrupt.
 *
 *END**************************************************************************/
static void UART_DRV_TxCompleteIrqHandler(uint32_t instance)
{
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    if (uartState->txSize == 0U)
    {
        if (uartState->transferType == UART_USING_INTERRUPTS)
        {
            /* Complete the interrupt based transfer */
            UART_DRV_CompleteSendDataUsingInt(instance);
        }
#if FEATURE_UART_HAS_DMA_ENABLE
        else
        {
            /* Complete the DMA based transfer */
            UART_DRV_StopTxDma(instance);
        }
#endif
        /* Invoke callback if there is one */
        if (uartState->txCallback != NULL)
        {
            uartState->txCallback(uartState, UART_EVENT_END_TRANSFER, uartState->txCallbackParam);
        }
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_ErrIrqHandler
 * Description   : Error Interrupt handler for UART.
 * This function treats the error interrupts.
 *
 *END**************************************************************************/
static void UART_DRV_ErrIrqHandler(uint32_t instance)
{
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    UART_Type *base = s_uartBase[instance];

    /* Handle IDLE interrupt */
    if (UART_GetIntMode(base, UART_INT_IDLE_LINE))
    {
        if (UART_GetStatusFlag(base, UART_IDLE_LINE_DETECT))
        {
            (void)UART_ClearStatusFlag(base, UART_IDLE_LINE_DETECT);
            /* Update the internal status */
            uartState->receiveStatus = STATUS_UART_IDLE_ERROR;
            /* Invoke callback if there is one */
            if (uartState->rxCallback != NULL)
            {
                uartState->rxCallback(uartState, UART_EVENT_ERROR, uartState->rxCallbackParam);
            }
        }
    }
    /* Handle receive overrun interrupt */
    if (UART_GetStatusFlag(base, UART_RX_OVERRUN))
    {
        (void)UART_ClearStatusFlag(base, UART_RX_OVERRUN);
        /* Update the internal status */
        uartState->receiveStatus = STATUS_UART_RX_OVERRUN;
        if (uartState->transferType == UART_USING_INTERRUPTS)
        {
            /* Complete the transfer (disable rx logic) */
            UART_DRV_CompleteReceiveDataUsingInt(instance);
        }
#if FEATURE_UART_HAS_DMA_ENABLE
        else
        {
            /* Complete the transfer (stop DMA channel) */
            UART_DRV_StopRxDma(instance);
        }
#endif
        /* Invoke callback if there is one */
        if (uartState->rxCallback != NULL)
        {
            uartState->rxCallback(uartState, UART_EVENT_ERROR, uartState->rxCallbackParam);
        }
    }
    /* Handle framing error interrupt */
    if (UART_GetStatusFlag(base, UART_FRAME_ERR))
    {
        (void)UART_ClearStatusFlag(base, UART_FRAME_ERR);
        /* Update the internal status */
        uartState->receiveStatus = STATUS_UART_FRAMING_ERROR;
        if (uartState->transferType == UART_USING_INTERRUPTS)
        {
            /* Complete the transfer (disable rx logic) */
            UART_DRV_CompleteReceiveDataUsingInt(instance);
        }
#if FEATURE_UART_HAS_DMA_ENABLE
        else
        {
            /* Complete the transfer (stop DMA channel) */
            UART_DRV_StopRxDma(instance);
        }
#endif
        /* Invoke callback if there is one */
        if (uartState->rxCallback != NULL)
        {
            uartState->rxCallback(uartState, UART_EVENT_ERROR, uartState->rxCallbackParam);
        }
    }
    /* Handle parity error interrupt */
    if (UART_GetStatusFlag(base, UART_PARITY_ERR))
    {
        (void)UART_ClearStatusFlag(base, UART_PARITY_ERR);
        /* Update the internal status */
        uartState->receiveStatus = STATUS_UART_PARITY_ERROR;
        if (uartState->transferType == UART_USING_INTERRUPTS)
        {
            /* Complete the transfer (disable rx logic) */
            UART_DRV_CompleteReceiveDataUsingInt(instance);
        }
#if FEATURE_UART_HAS_DMA_ENABLE
        else
        {
            /* Complete the transfer (stop DMA channel) */
            UART_DRV_StopRxDma(instance);
        }
#endif
        /* Invoke callback if there is one */
        if (uartState->rxCallback != NULL)
        {
            uartState->rxCallback(uartState, UART_EVENT_ERROR, uartState->rxCallbackParam);
        }
    }
    /* Handle noise error interrupt */
    if (UART_GetStatusFlag(base, UART_NOISE_DETECT))
    {
        (void)UART_ClearStatusFlag(base, UART_NOISE_DETECT);
        /* Update the internal status */
        uartState->receiveStatus = STATUS_UART_NOISE_ERROR;
        if (uartState->transferType == UART_USING_INTERRUPTS)
        {
            /* Complete transfer (disable rx logic) */
            UART_DRV_CompleteReceiveDataUsingInt(instance);
        }
#if FEATURE_UART_HAS_DMA_ENABLE
        else
        {
            /* Complete the transfer (stop DMA channel) */
            UART_DRV_StopRxDma(instance);
        }
#endif
        /* Invoke callback if there is one */
        if (uartState->rxCallback != NULL)
        {
            uartState->rxCallback(uartState, UART_EVENT_ERROR, uartState->rxCallbackParam);
        }
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_StartSendDataUsingInt
 * Description   : Initiate (start) a transmit by beginning the process of
 * sending data and enabling the interrupt.
 * This is not a public API as it is called from other driver functions.
 *
 *END**************************************************************************/
static status_t UART_DRV_StartSendDataUsingInt(uint32_t instance, const uint8_t *txBuff, uint32_t txSize)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);
    DEV_ASSERT(txBuff != NULL);

    UART_Type *base = s_uartBase[instance];
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    status_t retVal = STATUS_SUCCESS;

    /* Check it's not busy transmitting data from a previous function call */
    if (uartState->isTxBusy)
    {
        retVal = STATUS_BUSY;
    }
    if (retVal != STATUS_BUSY)
    {
            /* Check the validity of the parameters */
        DEV_ASSERT(txSize > 0U);
        DEV_ASSERT((uartState->bitCountPerChar <= UART_8_BITS_PER_CHAR) || ((txSize & 1U) == 0U));

        /* initialize the module driver state structsure */
        uartState->txBuff = txBuff;
        uartState->txSize = txSize;
        uartState->isTxBusy = true;
        uartState->transmitStatus = STATUS_BUSY;
#if defined(FEATURE_UART_FIFO_SIZE) && FEATURE_UART_FIFO_SIZE > 0
        /* initialize tx */
        UART_ResetTxFifo(base);
        UART_SetTxFifoWatermark(base, UART_DEFAULT_TX_FIFO_WATERMARK);
        UART_EnableTxFifo(base, true);
#endif /* FEATURE_UART_FIFO_SIZE > 0 */

        /* Enable the UART transmitter */
        UART_SetTransmitterCmd(base, true);

        /* Enable tx empty interrupt */
        UART_SetIntMode(base, UART_INT_TX_DATA_REG_EMPTY, true);
    }

    return retVal;
}

#if FEATURE_UART_HAS_DMA_ENABLE
/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_StartSendDataUsingDma
 * Description   : Initiate (start) a transmit by beginning the process of
 * sending data using DMA transfers.
 * This is not a public API as it is called from other driver functions.
 *
 *END**************************************************************************/
static status_t UART_DRV_StartSendDataUsingDma(uint32_t instance, const uint8_t *txBuff, uint32_t txSize)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);
    DEV_ASSERT(txBuff != NULL);

    UART_Type *base = s_uartBase[instance];
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    status_t retval = STATUS_SUCCESS;

    /* Check it's not busy transmitting data from a previous function call */
    if (uartState->isTxBusy)
    {
        retval = STATUS_BUSY;
    }
    
    if (retval != STATUS_BUSY)
    {
        DEV_ASSERT(txSize > 0U);

        /* Update state structure */
        uartState->txBuff = txBuff;
        uartState->txSize = 0U;
        uartState->isTxBusy = true;
        uartState->transmitStatus = STATUS_BUSY;

#if defined(FEATURE_UART_FIFO_SIZE) && FEATURE_UART_FIFO_SIZE > 0
        /* initialize tx */
        UART_ResetTxFifo(base);
        UART_SetTxFifoWatermark(base, UART_DEFAULT_TX_FIFO_WATERMARK);
        UART_EnableTxFifo(base, true);
#endif

        /* Configure the transfer control descriptor for the previously allocated channel */
        (void)DMA_DRV_ConfigMultiBlockTransfer(uartState->txDMAChannel,
                                               DMA_TRANSFER_MEM2PERIPH,
                                               (uint32_t)txBuff,
                                               (uint32_t)(&(base->DATA)),
                                               DMA_TRANSFER_SIZE_1B,
                                               1U,
                                               txSize,
                                               true);

        /* Call driver function to end the transmission when the DMA transfer is done */
        (void)DMA_DRV_InstallCallback(uartState->txDMAChannel,
                                      (dma_callback_t)(UART_DRV_TxDmaCallback),
                                      (void *)(instance)); /* PRQA S 0326 */

        /* Start the DMA channel */
        (void)DMA_DRV_StartChannel(uartState->txDMAChannel);

        /* Enable the UART transmitter */
        UART_SetTransmitterCmd(base, true);

        /* Enable tx DMA requests for the current instance */
        UART_SetTxDmaCmd(base, true);
    }

    return retval;
}
#endif

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_CompleteSendDataUsingInt
 * Description   : Finish up a transmit by completing the process of sending
 * data and disabling the interrupt.
 * This is not a public API as it is called from other driver functions.
 *
 *END**************************************************************************/
static void UART_DRV_CompleteSendDataUsingInt(uint32_t instance)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);

    UART_Type *base = s_uartBase[instance];
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    if (uartState->transmitStatus == STATUS_BUSY)
    {
        /* If the transfer is completed, update the transmit status */
        uartState->transmitStatus = STATUS_SUCCESS;
    }
    else
    {
        /* If the transfer is aborted or timed out, disable tx empty interrupt */
        UART_SetIntMode(base, UART_INT_TX_DATA_REG_EMPTY, false);
    }

    /* Disable transmission complete interrupt */
    UART_SetIntMode(base, UART_INT_TX_COMPLETE, false);

    /* Disable transmitter */
    UART_SetTransmitterCmd(base, false);

    /* Update the internal busy flag */
    uartState->isTxBusy = false;

    /* Signal the synchronous completion object. */
    if (uartState->isTxBlocking)
    {
        (void)OSIF_SemaPost(&uartState->txComplete);
    }
}

#if FEATURE_UART_HAS_DMA_ENABLE
/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_TxDmaCallback
 * Description   : Finish up a transmit by completing the process of sending
 * data and disabling the DMA requests. This is a callback for DMA trigger loop
 * completion, so it must match the DMA callback signature.
 * This is not a public API as it is called from other driver functions.
 *
 *END**************************************************************************/
static void UART_DRV_TxDmaCallback(void *parameter, dma_chn_status_t status)
{
    uint32_t instance = ((uint32_t)parameter); /* PRQA S 0326 */
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    UART_Type *base = s_uartBase[instance];

    /* Check if the DMA transfer completed with errors */
    if (status == DMA_CHN_ERROR)
    {
        /* Update the status */
        uartState->transmitStatus = STATUS_ERROR;
        /* Stop the transfer */
        UART_DRV_StopTxDma(instance);
        /* Notify the application that an error occurred */
        if (uartState->txCallback != NULL)
        {
            uartState->txCallback(uartState, UART_EVENT_ERROR, uartState->txCallbackParam);
        }
    }
    else
    {
        /* Invoke callback if there is one */
        if (uartState->txCallback != NULL)
        {
            /* Allow the user to provide a new buffer, for continuous transmission */
            uartState->txCallback(uartState, UART_EVENT_TX_EMPTY, uartState->txCallbackParam);
        }

        /* If the callback has updated the tx buffer, update the DMA descriptor to continue the transfer;
         * otherwise, stop the current transfer.
         */
        if (uartState->txSize > 0U)
        {
            /* Set the source address and the number of transfer loops (bytes to be transfered) */
            DMA_DRV_SetSrcAddr(uartState->txDMAChannel, (uint32_t)(uartState->txBuff));
            DMA_DRV_SetTriggerLoopIterationCount(uartState->txDMAChannel, uartState->txSize);

            /* Now that this tx is set up, clear remaining bytes count */
            uartState->txSize = 0U;

            /* Re-start the channel */
            (void)DMA_DRV_StartChannel(uartState->txDMAChannel);
        }
        else
        {
            /* Enable transmission complete interrupt */
            UART_SetIntMode(base, UART_INT_TX_COMPLETE, true);
        }
    }
}
#endif

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_StartReceiveDataUsingInt
 * Description   : Initiate (start) a receive by beginning the process of
 * receiving data and enabling the interrupt.
 * This is not a public API as it is called from other driver functions.
 *
 *END**************************************************************************/
static status_t UART_DRV_StartReceiveDataUsingInt(uint32_t instance, uint8_t *rxBuff, uint32_t rxSize)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);
    DEV_ASSERT(rxBuff != NULL);
    DEV_ASSERT(rxSize != 0);

    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    UART_Type *base = s_uartBase[instance];
    status_t retval = STATUS_SUCCESS;

    /* Check it's not busy receiving data from a previous function call */
    if (uartState->isRxBusy)
    {
        retval = STATUS_BUSY;
    }

    if (retval != STATUS_BUSY)
    {
        /* Check the validity of the parameters */
        DEV_ASSERT(rxSize > 0U);
        DEV_ASSERT((uartState->bitCountPerChar <= UART_8_BITS_PER_CHAR) || ((rxSize & 1U) == 0U));

        /* Initialize the module driver state struct to indicate transfer in progress
        * and with the buffer and byte count data. */
        uartState->isRxBusy = true;
        uartState->rxBuff = rxBuff;
        uartState->rxSize = rxSize;
        uartState->receiveStatus = STATUS_BUSY;
#if defined(FEATURE_UART_FIFO_SIZE) && FEATURE_UART_FIFO_SIZE > 0
        /* Initialize rx fifo */
        UART_ResetRxFifo(base);
        UART_SetRxFifoWatermark(base, 0);
        UART_EnableRxFifo(base, true);
#endif /* FEATURE_UART_FIFO_SIZE > 0 */

        /* Enable the receiver */
        UART_SetReceiverCmd(base, true);

        /* Enable error interrupts */
        UART_DRV_SetErrorInterrupts(instance, true);

        /* Enable receive data full interrupt */
        UART_SetIntMode(base, UART_INT_RX_DATA_REG_FULL, true);
    }

    return retval;
}

#if FEATURE_UART_HAS_DMA_ENABLE
/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_StartReceiveDataUsingDma
 * Description   : Initiate (start) a receive by beginning the process of
 * receiving data using DMA transfers.
 * This is not a public API as it is called from other driver functions.
 *
 *END**************************************************************************/
static status_t UART_DRV_StartReceiveDataUsingDma(uint32_t instance, uint8_t *rxBuff, uint32_t rxSize)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);
    DEV_ASSERT(rxBuff != NULL);

    UART_Type *base = s_uartBase[instance];
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    status_t retval = STATUS_SUCCESS;

    /* Check it's not busy transmitting data from a previous function call */
    if (uartState->isRxBusy)
    {
        retval = STATUS_BUSY;
    }

    if (retval != STATUS_BUSY)
    {
        DEV_ASSERT(rxSize > 0U);

        /* Configure the transfer control descriptor for the previously allocated channel */
        (void)DMA_DRV_ConfigMultiBlockTransfer(uartState->rxDMAChannel,
                                               DMA_TRANSFER_PERIPH2MEM,
                                               (uint32_t)(&(base->DATA)),
                                               (uint32_t)rxBuff,
                                               DMA_TRANSFER_SIZE_1B,
                                               1U,
                                               rxSize,
                                               true);

        /* Call driver function to end the reception when the DMA transfer is done */
        (void)DMA_DRV_InstallCallback(uartState->rxDMAChannel,
                                      (dma_callback_t)(UART_DRV_RxDmaCallback),
                                      (void *)(instance)); /* PRQA S 0326 */

        /* Start the DMA channel */
        (void)DMA_DRV_StartChannel(uartState->rxDMAChannel);

        /* Update the state structure */
        uartState->rxBuff = rxBuff;
        uartState->rxSize = 0U;
        uartState->isRxBusy = true;
        uartState->receiveStatus = STATUS_BUSY;

#if defined(FEATURE_UART_FIFO_SIZE) && FEATURE_UART_FIFO_SIZE > 0
        /* Initialize rx fifo */
        UART_ResetRxFifo(base);
        UART_SetRxFifoWatermark(base, 0);
        UART_EnableRxFifo(base, true);
#endif /* FEATURE_UART_FIFO_SIZE > 0 */

        /* Enable the receiver */
        UART_SetReceiverCmd(base, true);

        /* Enable error interrupts */
        UART_DRV_SetErrorInterrupts(instance, true);

        /* Enable rx DMA requests for the current instance */
        UART_SetRxDmaCmd(base, true);
    }

    return retval;
}
#endif

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_CompleteReceiveDataUsingInt
 * Description   : Finish up a receive by completing the process of receiving data
 * and disabling the interrupt.
 * This is not a public API as it is called from other driver functions.
 *
 *END**************************************************************************/
static void UART_DRV_CompleteReceiveDataUsingInt(uint32_t instance)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);

    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    UART_Type *base = s_uartBase[instance];

    /* Disable receiver */
    UART_SetReceiverCmd(base, false);

    /* Disable error interrupts */
    UART_DRV_SetErrorInterrupts(instance, false);
#if defined(FEATURE_UART_FIFO_SIZE) && FEATURE_UART_FIFO_SIZE > 0
    /* Reset rx fifo to clear RDRF flag */
    UART_ResetRxFifo(base);
#endif /* FEATURE_UART_FIFO_SIZE > 0 */

    /* Disable receive data full and rx overrun interrupt. */
    UART_SetIntMode(base, UART_INT_RX_DATA_REG_FULL, false);

    /* Signal the synchronous completion object. */
    if (uartState->isRxBlocking)
    {
        (void)OSIF_SemaPost(&uartState->rxComplete);
        uartState->isRxBlocking = false;
    }

    /* Update the information of the module driver state */
    uartState->isRxBusy = false;
    if (uartState->receiveStatus == STATUS_BUSY)
    {
        uartState->receiveStatus = STATUS_SUCCESS;
    }
}

#if FEATURE_UART_HAS_DMA_ENABLE
/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_RxDmaCallback
 * Description   : Finish up a receive by completing the process of receiving data
 * and disabling the DMA requests. This is a callback for DMA trigger loop
 * completion, so it must match the DMA callback signature.
 * This is not a public API as it is called from other driver functions.
 *
 *END**************************************************************************/
static void UART_DRV_RxDmaCallback(void *parameter, dma_chn_status_t status)
{
    uint32_t instance = ((uint32_t)parameter); /* PRQA S 0326 */
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    UART_Type *base = s_uartBase[instance];

    if (status == DMA_CHN_ERROR)
    {
        /* Update the status */
        uartState->receiveStatus = STATUS_ERROR;
        /* Stop the transfer */
        UART_DRV_StopRxDma(instance);
        /* Notify the application that an error occurred */
        if (uartState->rxCallback != NULL)
        {
            uartState->rxCallback(uartState, UART_EVENT_ERROR, uartState->rxCallbackParam);
        }

        /* Clear the flags */
        UART_DRV_ClearErrorFlags(base);
    }

    /* Return if an error occurred; error cases are treated by the interrupt handler */
    if (uartState->receiveStatus != STATUS_BUSY)
    {
        /* Nothing to do */
    }
    else
    {
            /* Invoke callback if there is one */
        if (uartState->rxCallback != NULL)
        {
            /* Allow the user to provide a new buffer inside the callback, to continue the reception */
            uartState->rxCallback(uartState, UART_EVENT_RX_FULL, uartState->rxCallbackParam);
        }

        /* If the callback has updated the rx buffer, update the DMA descriptor to continue the transfer;
        * otherwise, stop the current transfer.
        */
        if (uartState->rxSize > 0U)
        {
            /* Set the source address and the number of transfer loops (bytes to be transfered) */
            DMA_DRV_SetDestAddr(uartState->rxDMAChannel, (uint32_t)(uartState->rxBuff));
            DMA_DRV_SetTriggerLoopIterationCount(uartState->rxDMAChannel, uartState->rxSize);

            /* Now that this rx is set up, clear remaining bytes count */
            uartState->rxSize = 0U;

            /* Re-start the channel */
            (void)DMA_DRV_StartChannel(uartState->rxDMAChannel);
        }
        else
        {
            /* Stop the reception */
            UART_DRV_StopRxDma(instance);

            /* Invoke the callback to notify the end of the transfer */
            if (uartState->rxCallback != NULL)
            {
                uartState->rxCallback(uartState, UART_EVENT_END_TRANSFER, uartState->rxCallbackParam);
            }

            /* Clear the flags */
            UART_DRV_ClearErrorFlags(base);
        }
    }
}
#endif

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_PutData
 * Description   : Write data to the buffer register, according to configured
 * word length.
 * This is not a public API as it is called from other driver functions.
 *
 *END**************************************************************************/
static void UART_DRV_PutData(uint32_t instance)
{
    const uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    UART_Type *base = s_uartBase[instance];
    uint16_t data;
    const uint8_t *txBuff = uartState->txBuff;

    if (uartState->bitCountPerChar <= UART_8_BITS_PER_CHAR)
    {
        /* Transmit the data */
        UART_Putchar(base, *txBuff);
    }
    else if (uartState->bitCountPerChar == UART_9_BITS_PER_CHAR)
    {
        /* Create a 16-bits integer from two bytes */
        data = (uint16_t)(*txBuff);
        ++txBuff;
        data |= (uint16_t)(((uint16_t)(*txBuff)) << 8U);

        /* Transmit the data */
        UART_Putchar9(base, data);
    }
    else
    {
        /* Create a 16-bits integer from two bytes */
        data = (uint16_t)(*txBuff);
        ++txBuff;
        data |= (uint16_t)(((uint16_t)(*txBuff)) << 8U);

        /* Transmit the data */
        UART_Putchar10(base, data);
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_GetData
 * Description   : Read data from the buffer register, according to configured
 * word length.
 * This is not a public API as it is called from other driver functions.
 *
 *END**************************************************************************/
static void UART_DRV_GetData(uint32_t instance)
{
    const uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    const UART_Type *base = s_uartBase[instance];
    uint16_t data;
    uint8_t *rxBuff = uartState->rxBuff;

    if (uartState->bitCountPerChar == UART_7_BITS_PER_CHAR)
    {
        /* Receive the data */
        UART_Getchar7(base, rxBuff);
    }
    else if (uartState->bitCountPerChar == UART_8_BITS_PER_CHAR)
    {
        UART_Getchar8(base, rxBuff);
    }
    else if (uartState->bitCountPerChar == UART_9_BITS_PER_CHAR)
    {
        /* Receive the data */
        UART_Getchar9(base, &data);

        /* Write the least significant bits to the receive buffer */
        *rxBuff = (uint8_t)(data & 0xFFU);
        ++rxBuff;
        /* Write the ninth bit to the subsequent byte in the rx buffer */
        *rxBuff = (uint8_t)(data >> 8U);
    }
    else
    {
        /* Receive the data */
        UART_Getchar10(base, &data);

        /* Write the least significant bits to the receive buffer */
        *rxBuff = (uint8_t)(data & 0xFFU);
        ++rxBuff;
        /* Write the ninth and tenth bits to the subsequent byte in the rx buffer */
        *rxBuff = (uint8_t)(data >> 8U);
    }
}

#if FEATURE_UART_HAS_DMA_ENABLE
/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_StopTxDma
 * Description   : Finish up a DMA transmission by disabling the DMA requests,
 * transmission complete interrupt and tx logic. This function also resets the
 * internal driver state (busy flag/tx semaphore).
 * This is not a public API as it is called from other driver functions.
 *
 *END**************************************************************************/
static void UART_DRV_StopTxDma(uint32_t instance)
{
    UART_Type *base = s_uartBase[instance];
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    /* Disable tx DMA requests for the current instance */
    UART_SetTxDmaCmd(base, false);

    /* Stop the dma channel */
    (void)DMA_DRV_StopChannel(uartState->txDMAChannel);

    /* Disable transmission complete interrupt */
    UART_SetIntMode(base, UART_INT_TX_COMPLETE, false);

    /* Disable transmitter */
    UART_SetTransmitterCmd(base, false);

    /* Signal the synchronous completion object. */
    if (uartState->isTxBlocking)
    {
        (void)OSIF_SemaPost(&uartState->txComplete);
    }

    if (uartState->transmitStatus == STATUS_BUSY)
    {
        /* If the transfer is completed, update the transmit status */
        uartState->transmitStatus = STATUS_SUCCESS;
    }

    /* Update the internal busy flag */
    uartState->isTxBusy = false;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_StopRxDma
 * Description   : Finish up a DMA reception by disabling the DMA requests,
 * error interrupts and rx logic. This function also resets the internal driver
 * state (busy flag/rx semaphore).
 * This is not a public API as it is called from other driver functions.
 *
 *END**************************************************************************/
static void UART_DRV_StopRxDma(uint32_t instance)
{
    UART_Type *base = s_uartBase[instance];
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    /* Disable receiver */
    UART_SetReceiverCmd(base, false);

    /* Disable error interrupts */
    UART_DRV_SetErrorInterrupts(instance, false);

    /* Disable rx DMA requests for the current instance */
    UART_SetRxDmaCmd(base, false);

#if defined(FEATURE_UART_FIFO_SIZE) && FEATURE_UART_FIFO_SIZE > 0
    /* Reset rx fifo to clear RDRF flag */
    UART_ResetRxFifo(base);
#endif /* FEATURE_UART_FIFO_SIZE > 0 */

    /* Stop the DMA channel */
    (void)DMA_DRV_StopChannel(uartState->rxDMAChannel);

    /* Signal the synchronous completion object. */
    if (uartState->isRxBlocking)
    {
        (void)OSIF_SemaPost(&uartState->rxComplete);
        uartState->isRxBlocking = false;
    }

    /* Update the internal driver status */
    if (uartState->receiveStatus == STATUS_BUSY)
    {
        uartState->receiveStatus = STATUS_SUCCESS;
    }

    /* Update the information of the module driver state */
    uartState->isRxBusy = false;
}
#endif

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_SetErrorInterrupts
 * Description   : Enable or disable the UART error interrupts.
 * This is not a public API as it is called from other driver functions.
 *
 *END**************************************************************************/
static void UART_DRV_SetErrorInterrupts(uint32_t instance, bool enable)
{
    UART_Type *base = s_uartBase[instance];
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    bool idleIntEnable = uartState->idleErrorIntEnable;

    /* Disable/Enable error interrupts */
    UART_SetErrorInterrupts(base, enable);
    UART_SetIntMode(base, UART_INT_IDLE_LINE, idleIntEnable && enable);
}

#if FEATURE_UART_HAS_MODEM_SUPPORT
/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_SetTransmitCTS
 * Description   : Configure the UART CTS.
 *
 *END**************************************************************************/
void UART_DRV_SetTransmitCTS(uint32_t instance, uart_tx_cts_config_t *uartTxCtsConfig)
{
    UART_Type *base = s_uartBase[instance];

    UART_SetTxCtsSource(base, uartTxCtsConfig->txCtsSrc);
    UART_SetTxCtsDetectionTime(base, uartTxCtsConfig->txCtsDetTime);
    UART_EnableTxCts(base, uartTxCtsConfig->txCtsEnable);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_SetReceiveRTS
 * Description   : Configure the UART Receive RTS.
 *
 *END**************************************************************************/
void UART_DRV_SetReceiveRTS(uint32_t instance, uart_rx_rts_config_t *uartRxRtsConfig)
{
    UART_Type *base = s_uartBase[instance];

    DEV_ASSERT(uartRxRtsConfig->rxRtsWatermark < FEATURE_UART_FIFO_SIZE);
    DEV_ASSERT(!UART_IsEnableTxRts(base));

    UART_SetRxRtsWatermark(base, uartRxRtsConfig->rxRtsWatermark);
    UART_EnableRxRts(base, uartRxRtsConfig->rxRtsEnbale);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_SetTransmitRTS
 * Description   : Configure the UART Transmit RTS.
 *
 *END**************************************************************************/
void UART_DRV_SetTransmitRTS(uint32_t instance, uart_tx_rts_config_t *uartTxRtsConfig)
{
    UART_Type *base = s_uartBase[instance];

    DEV_ASSERT(!UART_IsEnableRxRts(base));

    UART_SetTxRtsPolarity(base, uartTxRtsConfig->txRtsPolarity);
    UART_EnableTxRts(base, uartTxRtsConfig->txRtsEnable);
}
#endif

#if defined(FEATURE_UART_HAS_ONE_WIRE_MODE_SUPPORT) && (FEATURE_UART_HAS_ONE_WIRE_MODE_SUPPORT == 1U) 
/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_SetOneWireMode
 * Description   : Configure uart one wire mode.
 *
 *END**************************************************************************/
void UART_DRV_SetOneWireMode(uint32_t instance, uart_one_wire_mode_config_t *oneWireModeConfig)
{
    UART_Type *base = s_uartBase[instance];

    if (oneWireModeConfig->oneWireModeEnable)
    {
        UART_EnableLoopMode(base, true);
        UART_EnableOneWireMode(base, true);
        UART_DRV_SetTxLineDirection(instance, oneWireModeConfig->txLineDirection);
    }
    else
    {
        UART_EnableLoopMode(base, false);
        UART_EnableOneWireMode(base, false);
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DRV_SetTxLineDirection
 * Description   : Configure uart tx line derection.
 *
 *END**************************************************************************/
void UART_DRV_SetTxLineDirection(uint32_t instance, uart_tx_line_derection_t txLinDerection)
{
    UART_Type *base = s_uartBase[instance];

    UART_SetTxLineDirection(base, txLinDerection);
}
#endif

/*******************************************************************************
 * EOF
 ******************************************************************************/
