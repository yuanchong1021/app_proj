/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file spi_hw_access.c
 * @version 1.4.0
 */

/*!
 * @page misra_violations MISRA-C:2012 violations list
 *
 * PRQA S 2985 Rule 2.2: This operation is redundant. The value of the result is 
 *                       always that of the left-hand operand.
 */

#include "spi_hw_access.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

/*FUNCTION**********************************************************************
 *
 * Function Name : SPI_Init
 * Description   : Resets the SPI internal logic and registers to their default settings.
 *
 * This function first performs a software reset of the SPI module which resets the
 * internal SPI logic and most registers, then proceeds to manually reset all of the
 * SPI registers to their default setting to ensuring these registers at programmed to
 * their default value which includes disabling the module.
 *
 *END**************************************************************************/
void SPI_Init(SPI_Type *base)
{
    base->CTRL = 0x00000000; // software spi module
}

/*FUNCTION**********************************************************************
 *
 * Function Name : SPI_Disable
 * Description   : Disables the SPI module.
 *
 * Note that this function returns STATUS_BUSY if it is detected that the Module Busy Flag
 * (MBF) is set, otherwise, if success, it returns STATUS_SUCCESS.
 *
 *END**************************************************************************/
status_t SPI_Disable(SPI_Type *base)
{
    status_t status = STATUS_SUCCESS;
    uint32_t spi_tmp = base->STS;
    spi_tmp = (spi_tmp & SPI_STS_BUSY_MASK) >> SPI_STS_BUSY_SHIFT;

    if (spi_tmp == (uint32_t)1)
    {
        status = STATUS_BUSY;
    }
    else
    {
        base->CTRL = base->CTRL & (~(SPI_CTRL_EN_MASK));
    }
    return status;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : SPI_GetVersionId
 * Description   : Configures the SPI for master or slave.
 *
 * Note that the SPI module must first be disabled before configuring this.
 *
 *END**************************************************************************/
status_t SPI_SetMasterSlaveMode(SPI_Type *base, spi_master_slave_mode_t mode)
{
    base->CTRL = (base->CTRL & (~SPI_CTRL_MODE_MASK)) | ((uint32_t)mode << SPI_CTRL_MODE_SHIFT);
    return STATUS_SUCCESS;
}
#if !defined(FEATURE_SPI_LITE_VERSION)
/*FUNCTION**********************************************************************
 *
 * Function Name : SPI_SetPinConfigMode
 * Description   : Flushes the SPI FIFOs.
 *
 *END**************************************************************************/
void SPI_SetFlushFifoCmd(SPI_Type *base, bool flushTxFifo, bool flushRxFifo)
{
    base->TXFIFO |= (uint32_t)(flushTxFifo ? 1 : 0) << SPI_TXFIFO_RESET_SHIFT;
    base->RXFIFO |= (uint32_t)(flushRxFifo ? 1 : 0) << SPI_TXFIFO_RESET_SHIFT;
}

#endif /* FEATURE_SPI_LITE_VERSION */

/*FUNCTION**********************************************************************
 *
 * Function Name : SPI_ClearStatusFlag
 * Description   : Clears the SPI status flag.
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
 *END**************************************************************************/
status_t SPI_ClearStatusFlag(SPI_Type *base, spi_status_flag_t statusFlag)
{
    if (statusFlag == SPI_ALL_STATUS)
    {
        base->STS |= (uint32_t)SPI_ALL_STATUS;
    }
    else
    {
        base->STS |= ((uint32_t)1U << (uint32_t)statusFlag);
    }
    return STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : SPI_SetPcsPolarityMode
 * Description   : Configures the desired SPI PCS polarity.
 *
 * This function allows the user to configure the polarity of a particular PCS signal.
 * Note that the SPI module must first be disabled before configuring this.
 *
 *END**************************************************************************/
status_t SPI_SetPcsPolarityMode(SPI_Type *base, spi_which_pcs_t whichPcs, spi_signal_polarity_t pcsPolarity)
{
    uint32_t cfgr1Value = 0;

    /* Clear the PCS polarity bit */
    cfgr1Value = (base->CTRL) & (~((uint32_t)1U << (SPI_CTRL_CSPOL_SHIFT + (uint32_t)whichPcs)));

    /* Configure the PCS polarity bit according to the pcsPolarity setting */
    cfgr1Value |= (uint32_t)pcsPolarity << (SPI_CTRL_CSPOL_SHIFT + (uint32_t)whichPcs);

    base->CTRL = cfgr1Value;

    return STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : SPI_SetPinConfigMode
 * Description   : Configures the SPI SDO/SDI pin configuration mode.
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
 *END**************************************************************************/
status_t
SPI_SetPinConfigMode(SPI_Type *base, spi_pin_config_t pinCfg, spi_data_out_config_t dataOutConfig, bool pcs3and2Enable)
{
    uint32_t cfgr1Value = 0;
#if !defined(FEATURE_SPI_LITE_VERSION)
    cfgr1Value = base->CTRL & ~(SPI_CTRL_PINCFG_MASK | SPI_CTRL_DHZEN_MASK | SPI_CTRL_CSDEN_MASK);

    cfgr1Value |= ((uint32_t)(pinCfg) << SPI_CTRL_PINCFG_SHIFT) | ((uint32_t)(dataOutConfig) << SPI_CTRL_DHZEN_SHIFT) |
                  ((uint32_t)(pcs3and2Enable ? 0 : 1) << SPI_CTRL_CSDEN_SHIFT); /* enable = 0 */

    base->CTRL = cfgr1Value;
#else
    (void)pcs3and2Enable;
    DevAssert(pcs3and2Enable == false);
    cfgr1Value = base->CTRL & ~(SPI_CTRL_PINCFG_MASK | SPI_CTRL_DHZEN_MASK);

    cfgr1Value |= ((uint32_t)(pinCfg) << SPI_CTRL_PINCFG_SHIFT) | ((uint32_t)(dataOutConfig) << SPI_CTRL_DHZEN_SHIFT); /* enable = 0 */

    base->CTRL = cfgr1Value;
#endif /* FEATURE_SPI_LITE_VERSION */

    return STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : abs_dif
 * Description   : This is a helper function which implements absolute difference between
 * two numbers.
 *
 *END**************************************************************************/
static uint32_t abs_dif(uint32_t a, uint32_t b)
{
    return (a > b) ? (a - b) : (b - a);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : SPI_SetBaudRate
 * Description   : Sets the SPI baud rate in bits per second.
 *
 * This function takes in the desired bitsPerSec (baud rate) and calculates the nearest
 * possible baud rate, and returns the calculated baud rate in bits-per-second. It requires
 * that the caller also provide the frequency of the module source clock (in Hertz).
 * Also note that the baud rate does not take into affect until the Transmit Control
 * Register (TCR) is programmed with the PRESCALE value. Hence, this function returns the
 * PRESCALE tcrPrescaleValue parameter for later programming in the TCR.  It is up to the
 * higher level peripheral driver to alert the user of an out of range baud rate input.
 * Note that the SPI module must first be disabled before configuring this.
 * Note that the SPI module must be configure for master mode before configuring this.
 *
 *END**************************************************************************/
uint32_t SPI_SetBaudRate(SPI_Type *base, uint32_t bitsPerSec, uint32_t sourceClockInHz, uint32_t *tcrPrescaleValue)
{
    uint32_t bestFreq = 0xFFFFFFFFU;
    uint32_t bestScaler = 0U;
    uint32_t bestPrescaler = 0U;
    uint32_t freq1 = 0U;
    uint32_t freq2 = 0U;
    uint8_t scaler = 0U;
    uint8_t prescaler = 0U;
    uint32_t low, high;
    uint32_t tempBestFreq = 0U;
    uint32_t tempBestScaler = 0U;

    for (prescaler = 0; prescaler < 8U; prescaler++)
    {
        low = 0U;
        high = 256U;

        /* Implement golden section search algorithm */
        do
        {
            scaler = (uint8_t)((low + high) / 2U);
            freq1 = sourceClockInHz / (s_baudratePrescaler[prescaler] * (scaler + (uint32_t)2U));

            if (abs_dif(bitsPerSec, bestFreq) > abs_dif(bitsPerSec, freq1))
            {
                bestFreq = freq1;
            }
            if (freq1 < bitsPerSec)
            {
                high = scaler;
            }
            else
            {
                low = scaler;
            }
        } while ((high - low) > 1U);

        /* Evaluate last 2 scaler values */
        freq1 = sourceClockInHz / (s_baudratePrescaler[prescaler] * (low + (uint32_t)2U));
        freq2 = sourceClockInHz / (s_baudratePrescaler[prescaler] * (high + (uint32_t)2U));

        if (abs_dif(bitsPerSec, freq1) > abs_dif(bitsPerSec, freq2))
        {
            tempBestFreq = freq2;
            tempBestScaler = high;
        }
        else
        {
            tempBestFreq = freq1;
            tempBestScaler = low;
        }

        if (abs_dif(bitsPerSec, bestFreq) >= abs_dif(bitsPerSec, tempBestFreq))
        {
            bestFreq = tempBestFreq;
            bestScaler = tempBestScaler;
            bestPrescaler = prescaler;
        }

        /* If current frequency is equal to target frequency  stop the search */
        if (bestFreq == bitsPerSec)
        {
            break;
        }
    }

    /* Add default values for delay between transfers, delay between sck to pcs and between pcs to sck. */
    (void)SPI_SetDelay(base, SPI_SCK_TO_PCS, bestScaler >> 2U);
    (void)SPI_SetDelay(base, SPI_PCS_TO_SCK, bestScaler >> 2U);
    (void)SPI_SetDelay(base, SPI_BETWEEN_TRANSFER, bestScaler >> 2U);

    /* Write the best baud rate scalar to the CCR.
     * Note, no need to check for error since we've already checked to make sure the module is
     * disabled and in master mode. Also, there is a limit on the maximum divider so we will not
     * exceed this.
     */
    (void)SPI_SetBaudRateDivisor(base, bestScaler);

    /* return the best prescaler value for user to use later */
    *tcrPrescaleValue = bestPrescaler;

    /* return the actual calculated baud rate */
    return bestFreq;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : SPI_SetBaudRateDivisor
 * Description   : Configures the baud rate divisor manually (only the SPI_CCR[SCKDIV]).
 *
 * This function allows the caller to manually set the baud rate divisor in the event
 * that this divider is known and the caller does not wish to call the
 * SPI_SetBaudRate function. Note that this only affects the SPI_CCR[SCKDIV]).
 * The Transmit Control Register (TCR) is programmed separately with the PRESCALE value.
 * The valid range is 0x00 to 0xFF (255), if the user inputs outside of this range, an error
 * is returned.
 *
 * Note that the SPI module must first be disabled before configuring this.
 * Note that the SPI module must be configure for master mode before configuring this.
 *
 *END**************************************************************************/
status_t SPI_SetBaudRateDivisor(SPI_Type *base, uint32_t divisor)
{
    uint32_t spi_tmp;

    spi_tmp = base->CLK;
    spi_tmp &= ~(SPI_CLK_DIV_MASK);
    spi_tmp |= SPI_CLK_DIV(divisor); /*PRQA S 2985*/
    base->CLK = spi_tmp;

    return STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : SPI_SetTxCommandReg
 * Description   : Sets the Transmit Command Register (TCR) parameters.
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
 *END**************************************************************************/
void SPI_SetTxCommandReg(SPI_Type *base, const spi_tx_cmd_config_t *txCmdCfgSet)
{
    base->TXCFG = ((SPI_TXCFG_CPOL(txCmdCfgSet->clkPolarity)) | /*PRQA S 2985*/
                   (SPI_TXCFG_CPHA(txCmdCfgSet->clkPhase)) |
#if !defined(FEATURE_SPI_LITE_VERSION)
                   (SPI_TXCFG_PRESCALE(txCmdCfgSet->preDiv)) |
                   (SPI_TXCFG_BSW((txCmdCfgSet->byteSwap) ? 1U : 0U)) | /*PRQA S 2985*/
                   (SPI_TXCFG_CONTC((txCmdCfgSet->contCmd) ? 1U : 0U)) | /*PRQA S 2985*/
                   (SPI_TXCFG_WIDTH(txCmdCfgSet->width)) |
                   (SPI_TXCFG_FRAMESZ(txCmdCfgSet->frameSize - 1UL)) | /*PRQA S 2985*/
#endif /* FEATURE_SPI_LITE_VERSION */
                   (SPI_TXCFG_PCS(txCmdCfgSet->whichPcs)) |
                   (SPI_TXCFG_LSBF((txCmdCfgSet->lsbFirst) ? 1U : 0U)) | /*PRQA S 2985*/
                   (SPI_TXCFG_CONT((txCmdCfgSet->contTransfer) ? 1U : 0U)) | /*PRQA S 2985*/
                   (SPI_TXCFG_MSKRX((txCmdCfgSet->rxMask) ? 1U : 0U)) | /*PRQA S 2985*/
                   (SPI_TXCFG_MSKTX((txCmdCfgSet->txMask) ? 1U : 0U))); /*PRQA S 2985*/
}

/*FUNCTION**********************************************************************
 *
 * Function Name : SPI_SetPcs
 * Description   : Sets the PCS flag to a value between 0 and 3.
 *
 * This function modifies the TCR register and sets the value of the PCS flag
 * to the value of the whichPcs parameter.
 *
 *END**************************************************************************/
void SPI_SetPcs(SPI_Type *base, spi_which_pcs_t whichPcs)
{
    uint32_t regVal;

    regVal = base->TXCFG;
    regVal &= (uint32_t)(~(SPI_TXCFG_PCS_MASK));
    regVal |= (uint32_t)((uint32_t)whichPcs << SPI_TXCFG_PCS_SHIFT);
    base->TXCFG = regVal;
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
