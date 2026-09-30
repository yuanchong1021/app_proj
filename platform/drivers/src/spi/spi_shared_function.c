/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file spi_shared_function.c
 * @version 1.4.0
 */

/*!
 * @page misra_violations MISRA-C:2012 violations list
 *
 * PRQA S 0771 Rule 15.4: More than one 'break' statement has been used to terminate 
 *                        this iteration statement.
 * PRQA S 1533 Rule 8.9: The object '%1s' is only referenced by function '%2s'.
 *
 */

#include "spi_hw_access.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*! @brief Table of base pointers for SPI instances. */
SPI_Type *g_spiBase[SPI_INSTANCE_COUNT] = SPI_BASE_PTRS;

/*! @brief Table to save SPI IRQ enumeration numbers defined in the CMSIS header file. */
IRQn_Type g_spiIrqId[SPI_INSTANCE_COUNT] = SPI_IRQS;

/* Pointer to runtime state structure.*/
spi_state_t *g_spiStatePtr[SPI_INSTANCE_COUNT] = FEATURE_SPI_STATE_STRUCTURES_NULL;

/* Table to save SPI clock names as defined in clock manager. */
const clock_names_t s_spiClkNames[SPI_INSTANCE_COUNT] = FEATURE_SPI_CLOCKS_NAMES; /*PRQA S 1533*/

/*******************************************************************************
 * Code
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
void SPI_DRV_IRQHandler(uint32_t instance)
{
    if (instance < SPI_INSTANCE_COUNT)
    {
        const SPI_Type *base = g_spiBase[instance];

        if (SPI_IsMaster(base))
        {
            /* Master mode.*/
            SPI_DRV_MasterIRQHandler(instance);
        }
        else
        {
            /* Slave mode.*/
            SPI_DRV_SlaveIRQHandler(instance);
        }
    }
}

/*!
 * @brief Fill up the TX FIFO with data.
 * This function fills up the TX FIFO with data based on the bytes/frame.
 * This is not a public API as it is called from other driver functions.
 */
void SPI_DRV_FillupTxBuffer(uint32_t instance)
{
    /* Instantiate local variable of type dspi_master_state_t and point to global state. */
    spi_state_t *spiState = g_spiStatePtr[instance];
    SPI_Type *base = g_spiBase[instance];
    uint32_t wordToSend = 0;
    uint16_t numOfBytes;
#if !defined(FEATURE_SPI_LITE_VERSION)
    uint8_t availableSpace = (uint8_t)(spiState->fifoSize - (uint8_t)SPI_ReadTxCount(base));
#else
    uint8_t availableSpace = 1;
#endif

    /* Fill the TX buffer. */
    while (availableSpace != 0U) /* PRQA S 0771 */
    {
        if (spiState->isPcsContinuous == true)
        {
            if (spiState->txCount == 1U)
            {
                /* Disable continuous PCS */
#if !defined(FEATURE_SPI_LITE_VERSION)
                SPI_ClearContCBit(base);
#endif /* FEATURE_SPI_LITE_VERSION */
                SPI_ClearContBit(base);
                spiState->txCount = 0U;
                break;
            }
        }
        /* Get the number of bytes which can be written in a single 32 bits word. */
        if ((spiState->bytesPerFrame - spiState->txFrameCnt) <= (uint16_t)4)
        {
            numOfBytes = (uint16_t)(spiState->bytesPerFrame - spiState->txFrameCnt);
        }
        else
        {
            numOfBytes = 4U;
        }

        if (spiState->txBuff != NULL)
        {
            switch (spiState->bytesPerFrame)
            {
                case 1:
                    wordToSend = *((const uint8_t *)(spiState->txBuff));
                    spiState->txBuff = &(spiState->txBuff[sizeof(uint8_t)]);
                    spiState->txFrameCnt = (uint16_t)((spiState->txFrameCnt + numOfBytes) & 0x00U);
                    break;
                case 2:
                    wordToSend = ((uint32_t)spiState->txBuff[1] << 8) | (uint32_t)spiState->txBuff[0];
                    spiState->txBuff = &(spiState->txBuff[sizeof(uint16_t)]);
                    spiState->txFrameCnt = (uint16_t)((spiState->txFrameCnt + numOfBytes) & 0x01U);
                    break;
                default:
                    wordToSend = ((uint32_t)spiState->txBuff[3] << 24) | ((uint32_t)spiState->txBuff[2] << 16) | ((uint32_t)spiState->txBuff[1] << 8) | (uint32_t)spiState->txBuff[0];
                    spiState->txBuff = &(spiState->txBuff[sizeof(uint32_t)]);
                    spiState->txFrameCnt = (uint16_t)((spiState->txFrameCnt + numOfBytes) & 0x03U);
                    break;
            }
            /* spiState->txFrameCnt = (uint16_t)((spiState->txFrameCnt + numOfBytes) % spiState->bytesPerFrame); */
            SPI_WriteData(base, wordToSend);

        }
        else
        {
            SPI_SetTxmskBit(base);
        }
        /* Update internal variable used in transmission. */
        spiState->txCount = (uint16_t)(spiState->txCount - numOfBytes);
        /* Verify if all bytes were send. */
        if (spiState->txCount == 0U)
        {
            break;
        }
        availableSpace = (uint8_t)(availableSpace - 1U);
    }
}

/*!
 * @brief Read all data from RX FIFO
 * This function will read all data from RX FIFO and will transfer this information in
 * RX software buffer.
 * This is not a public API as it is called from other driver functions.
 */
void SPI_DRV_ReadRXBuffer(uint32_t instance)
{
    spi_state_t *spiState = g_spiStatePtr[instance];
    const SPI_Type *base = g_spiBase[instance];
    uint32_t receivedWord;
    uint16_t numOfBytes;
    uint16_t j;
#if !defined(FEATURE_SPI_LITE_VERSION)
    uint8_t filledSpace = (uint8_t)SPI_ReadRxCount(base);
#else
    uint8_t filledSpace = 1;
#endif /* FEATURE_SPI_LITE_VERSION */
    while (filledSpace != 0U)
    {
        receivedWord = SPI_ReadData(base);
        /* Get the number of bytes which can be read from this 32 bites */
        if ((spiState->bytesPerFrame - spiState->rxFrameCnt) <= (uint16_t)4)
        {
            numOfBytes = (uint16_t)(spiState->bytesPerFrame - spiState->rxFrameCnt);
        }
        else
        {
            numOfBytes = 4U;
        }
        /* Generate the word which will be write in buffer. */
        for (j = 0; j < numOfBytes; j++)
        {
            *(spiState->rxBuff) = (uint8_t)(receivedWord >> (j * 8U));
            spiState->rxBuff++;
        }
        switch (spiState->bytesPerFrame)
        {
            case 1:
                spiState->rxFrameCnt = (uint16_t)((spiState->rxFrameCnt + numOfBytes) & 0x00U);
                break;
            case 2:
                spiState->rxFrameCnt = (uint16_t)((spiState->rxFrameCnt + numOfBytes) & 0x01U);
                break;
            default:
                spiState->rxFrameCnt = (uint16_t)((spiState->rxFrameCnt + numOfBytes) & 0x03U);
                break;
        }
        /* spiState->rxFrameCnt = (uint16_t)((spiState->rxFrameCnt + numOfBytes) % spiState->bytesPerFrame); */

        /* Update internal variable used in transmission. */
        spiState->rxCount = (uint16_t)(spiState->rxCount - numOfBytes);
        /* Verify if all bytes were sent. */
        if (spiState->rxCount == 0U)
        {
            break;
        }
        filledSpace = (uint8_t)(filledSpace - 1U);
    }
}

/*!
 * @brief Disable the TEIE interrupts at the end of a transfer.
 * Disable the interrupts and clear the status for transmit/receive errors.
 */
void SPI_DRV_DisableTEIEInterrupts(uint32_t instance)
{
    SPI_Type *base = g_spiBase[instance];

    SPI_SetIntMode(base, SPI_TRANSMIT_ERROR, false);
    SPI_SetIntMode(base, SPI_RECEIVE_ERROR, false);
    (void)SPI_ClearStatusFlag(base, SPI_TRANSMIT_ERROR);
    (void)SPI_ClearStatusFlag(base, SPI_RECEIVE_ERROR);
}

/*******************************************************************************
 * EOF
 ******************************************************************************/

