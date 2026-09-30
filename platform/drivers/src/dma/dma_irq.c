/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file dma_irq.c
 * @version 1.4.0
 */

#include "dma_irq.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
#ifdef FEATURE_DMA_ORED_IRQ_LINES_16_CHN
void DMA0_15_IRQHandler(void);
#if (FEATURE_DMA_VIRTUAL_CHANNELS > 16U)
void DMA16_31_IRQHandler(void);
#endif
#endif

#ifdef FEATURE_DMA_SEPARATE_IRQ_LINES_PER_CHN

void DMA0_IRQHandler(void);

void DMA1_IRQHandler(void);

void DMA2_IRQHandler(void);

void DMA3_IRQHandler(void);

#if (FEATURE_DMA_VIRTUAL_CHANNELS > 4U)

void DMA4_IRQHandler(void);

void DMA5_IRQHandler(void);

void DMA6_IRQHandler(void);

void DMA7_IRQHandler(void);

#endif
#if (FEATURE_DMA_VIRTUAL_CHANNELS > 8U)

void DMA8_IRQHandler(void);

void DMA9_IRQHandler(void);

void DMA10_IRQHandler(void);

void DMA11_IRQHandler(void);

void DMA12_IRQHandler(void);

void DMA13_IRQHandler(void);

void DMA14_IRQHandler(void);

void DMA15_IRQHandler(void);

#endif
#if (FEATURE_DMA_VIRTUAL_CHANNELS > 16U)
void DMA16_IRQHandler(void);
void DMA17_IRQHandler(void);
void DMA18_IRQHandler(void);
void DMA19_IRQHandler(void);
void DMA20_IRQHandler(void);
void DMA21_IRQHandler(void);
void DMA22_IRQHandler(void);
void DMA23_IRQHandler(void);
void DMA24_IRQHandler(void);
void DMA25_IRQHandler(void);
void DMA26_IRQHandler(void);
void DMA27_IRQHandler(void);
void DMA28_IRQHandler(void);
void DMA29_IRQHandler(void);
void DMA30_IRQHandler(void);
void DMA31_IRQHandler(void);
#endif
#endif

#ifdef FEATURE_DMA_HAS_ERROR_IRQ

void DMA_Error_IRQHandler(void);

#endif

/*******************************************************************************
 * Code
 ******************************************************************************/

#ifdef FEATURE_DMA_ORED_IRQ_LINES_16_CHN

/*! @brief DMA IRQ handler with the same name in the startup code*/
void DMA0_15_IRQHandler(void)
{
    /* Read the status flags register */
    const DMA_Type * dmaRegBase = DMA_DRV_GetDmaRegBaseAddr(0U);
    uint32_t mask = 0xFFFF;
    uint32_t flags = dmaRegBase->CHTLDIF;
    uint8_t virtualChannel = 0U;
    flags &= mask;
    /* Check all the flags from 0 to 15 and call the handler for the appropriate channel */
    while (flags > 0U)
    {
       if ((flags & 1U) > 0U)
       {
           DMA_DRV_IRQHandler(virtualChannel);
       }
       virtualChannel++;
       flags >>= 1U;
    }
}
#if (FEATURE_DMA_VIRTUAL_CHANNELS > 16U)
/*! @brief DMA16_31_IRQn IRQ handler with the same name in the startup code*/
void DMA16_31_IRQHandler(void)
{
    /* Read the status flags register */
    const DMA_Type * dmaRegBase = DMA_DRV_GetDmaRegBaseAddr(0U);
    uint32_t flags = dmaRegBase->INT;
    uint8_t virtualChannel = 16U;
    flags >>= 16U;
    /* Check all the flags from 16 to 31 and call the handler for the appropriate channel */
    while (flags > 0U)
    {
       if ((flags & 1U) > 0U)
       {
           DMA_DRV_IRQHandler(virtualChannel);
       }
       virtualChannel++;
       flags >>= 1U;
    }
}
#endif /* (FEATURE_DMA_VIRTUAL_CHANNELS > 16U) */
#endif /* FEATURE_DMA_ORED_IRQ_LINES_16_CHN */

#ifdef FEATURE_DMA_ORED_IRQ_LINES_2_CHN
/*! @brief DMA IRQ handler with the same name in the startup code*/
void DMA0_CH0_CH1_IRQHandler(void)
{
    DMA_DRV_IRQHandler(0U);
    DMA_DRV_IRQHandler(1U);
}
void DMA0_CH2_CH3_IRQHandler(void)
{
    DMA_DRV_IRQHandler(2U);
    DMA_DRV_IRQHandler(3U);
}
void DMA0_CH4_CH5_IRQHandler(void)
{
    DMA_DRV_IRQHandler(4U);
    DMA_DRV_IRQHandler(5U);
}
void DMA0_CH6_CH7_IRQHandler(void)
{
    DMA_DRV_IRQHandler(6U);
    DMA_DRV_IRQHandler(7U);
}
void DMA0_CH8_CH9_IRQHandler(void)
{
    DMA_DRV_IRQHandler(8U);
    DMA_DRV_IRQHandler(9U);
}
void DMA0_CH10_CH11_IRQHandler(void)
{
    DMA_DRV_IRQHandler(10U);
    DMA_DRV_IRQHandler(11U);
}
void DMA0_CH12_CH13_IRQHandler(void)
{
    DMA_DRV_IRQHandler(12U);
    DMA_DRV_IRQHandler(13U);
}
void DMA0_CH14_CH15_IRQHandler(void)
{
    DMA_DRV_IRQHandler(14U);
    DMA_DRV_IRQHandler(15U);
}
void DMA0_CH16_CH17_IRQHandler(void)
{
    DMA_DRV_IRQHandler(16U);
    DMA_DRV_IRQHandler(17U);
}
void DMA0_CH18_CH19_IRQHandler(void)
{
    DMA_DRV_IRQHandler(18U);
    DMA_DRV_IRQHandler(19U);
}
void DMA0_CH20_CH21_IRQHandler(void)
{
    DMA_DRV_IRQHandler(20U);
    DMA_DRV_IRQHandler(21U);
}
void DMA0_CH22_CH23_IRQHandler(void)
{
    DMA_DRV_IRQHandler(22U);
    DMA_DRV_IRQHandler(23U);
}
void DMA0_CH24_CH25_IRQHandler(void)
{
    DMA_DRV_IRQHandler(24U);
    DMA_DRV_IRQHandler(25U);
}
void DMA0_CH26_CH27_IRQHandler(void)
{
    DMA_DRV_IRQHandler(26U);
    DMA_DRV_IRQHandler(27U);
}
void DMA0_CH28_CH29_IRQHandler(void)
{
    DMA_DRV_IRQHandler(28U);
    DMA_DRV_IRQHandler(29U);
}
void DMA0_CH30_CH31_IRQHandler(void)
{
    DMA_DRV_IRQHandler(30U);
    DMA_DRV_IRQHandler(31U);
}
void DMA1_CH0_CH1_IRQHandler(void)
{
    DMA_DRV_IRQHandler(32U);
    DMA_DRV_IRQHandler(33U);
}
void DMA1_CH2_CH3_IRQHandler(void)
{
    DMA_DRV_IRQHandler(34U);
    DMA_DRV_IRQHandler(35U);
}
void DMA1_CH4_CH5_IRQHandler(void)
{
    DMA_DRV_IRQHandler(36U);
    DMA_DRV_IRQHandler(37U);
}
void DMA1_CH6_CH7_IRQHandler(void)
{
    DMA_DRV_IRQHandler(38U);
    DMA_DRV_IRQHandler(39U);
}
void DMA1_CH8_CH9_IRQHandler(void)
{
    DMA_DRV_IRQHandler(40U);
    DMA_DRV_IRQHandler(41U);
}
void DMA1_CH10_CH11_IRQHandler(void)
{
    DMA_DRV_IRQHandler(42U);
    DMA_DRV_IRQHandler(43U);
}
void DMA1_CH12_CH13_IRQHandler(void)
{
    DMA_DRV_IRQHandler(44U);
    DMA_DRV_IRQHandler(45U);
}
void DMA1_CH14_CH15_IRQHandler(void)
{
    DMA_DRV_IRQHandler(46U);
    DMA_DRV_IRQHandler(47U);
}
void DMA1_CH16_CH17_IRQHandler(void)
{
    DMA_DRV_IRQHandler(48U);
    DMA_DRV_IRQHandler(49U);
}
void DMA1_CH18_CH19_IRQHandler(void)
{
    DMA_DRV_IRQHandler(50U);
    DMA_DRV_IRQHandler(51U);
}
void DMA1_CH20_CH21_IRQHandler(void)
{
    DMA_DRV_IRQHandler(52U);
    DMA_DRV_IRQHandler(53U);
}
void DMA1_CH22_CH23_IRQHandler(void)
{
    DMA_DRV_IRQHandler(54U);
    DMA_DRV_IRQHandler(55U);
}
void DMA1_CH24_CH25_IRQHandler(void)
{
    DMA_DRV_IRQHandler(56U);
    DMA_DRV_IRQHandler(57U);
}
void DMA1_CH26_CH27_IRQHandler(void)
{
    DMA_DRV_IRQHandler(58U);
    DMA_DRV_IRQHandler(59U);
}
void DMA1_CH28_CH29_IRQHandler(void)
{
    DMA_DRV_IRQHandler(60U);
    DMA_DRV_IRQHandler(61U);
}
void DMA1_CH30_CH31_IRQHandler(void)
{
    DMA_DRV_IRQHandler(62U);
    DMA_DRV_IRQHandler(63U);
}

#endif /* FEATURE_DMA_ORED_IRQ_LINES_2_CHN */

#ifdef FEATURE_DMA_SEPARATE_IRQ_LINES_PER_CHN

/*! @brief DMA IRQ handler with the same name in the startup code*/
void DMA0_IRQHandler(void)
{
    DMA_DRV_IRQHandler(0U);
}

/*! @brief DMA IRQ handler with the same name in the startup code*/
void DMA1_IRQHandler(void)
{
    DMA_DRV_IRQHandler(1U);
}

/*! @brief DMA IRQ handler with the same name in the startup code*/
void DMA2_IRQHandler(void)
{
    DMA_DRV_IRQHandler(2U);
}

/*! @brief DMA IRQ handler with the same name in the startup code*/
void DMA3_IRQHandler(void)
{
    DMA_DRV_IRQHandler(3U);
}

#if (FEATURE_DMA_VIRTUAL_CHANNELS > 4U)

/*! @brief DMA IRQ handler with the same name in the startup code*/
void DMA4_IRQHandler(void)
{
    DMA_DRV_IRQHandler(4U);
}

/*! @brief DMA IRQ handler with the same name in the startup code*/
void DMA5_IRQHandler(void)
{
    DMA_DRV_IRQHandler(5U);
}

/*! @brief DMA IRQ handler with the same name in the startup code*/
void DMA6_IRQHandler(void)
{
    DMA_DRV_IRQHandler(6U);
}

/*! @brief DMA IRQ handler with the same name in the startup code*/
void DMA7_IRQHandler(void)
{
    DMA_DRV_IRQHandler(7U);
}

#endif /* (FEATURE_DMA_VIRTUAL_CHANNELS > 4U) */
#if (FEATURE_DMA_VIRTUAL_CHANNELS > 8U)

/*! @brief DMA IRQ handler with the same name in the startup code*/
void DMA8_IRQHandler(void)
{
    DMA_DRV_IRQHandler(8U);
}

/*! @brief DMA IRQ handler with the same name in the startup code*/
void DMA9_IRQHandler(void)
{
    DMA_DRV_IRQHandler(9U);
}

/*! @brief DMA IRQ handler with the same name in the startup code*/
void DMA10_IRQHandler(void)
{
    DMA_DRV_IRQHandler(10U);
}

/*! @brief DMA IRQ handler with the same name in the startup code*/
void DMA11_IRQHandler(void)
{
    DMA_DRV_IRQHandler(11U);
}

/*! @brief DMA IRQ handler with the same name in the startup code*/
void DMA12_IRQHandler(void)
{
    DMA_DRV_IRQHandler(12U);
}

/*! @brief DMA IRQ handler with the same name in the startup code*/
void DMA13_IRQHandler(void)
{
    DMA_DRV_IRQHandler(13U);
}

/*! @brief DMA IRQ handler with the same name in the startup code*/
void DMA14_IRQHandler(void)
{
    DMA_DRV_IRQHandler(14U);
}

/*! @brief DMA IRQ handler with the same name in the startup code*/
void DMA15_IRQHandler(void)
{
    DMA_DRV_IRQHandler(15U);
}

#endif /* (FEATURE_DMA_VIRTUAL_CHANNELS > 8U) */
#if (FEATURE_DMA_VIRTUAL_CHANNELS > 16U)
void DMA16_IRQHandler(void)
{
    DMA_DRV_IRQHandler(16U);
}

/*! @brief DMA IRQ handler with the same name in the startup code*/
void DMA17_IRQHandler(void)
{
    DMA_DRV_IRQHandler(17U);
}

/*! @brief DMA IRQ handler with the same name in the startup code*/
void DMA18_IRQHandler(void)
{
    DMA_DRV_IRQHandler(18U);
}

/*! @brief DMA IRQ handler with the same name in the startup code*/
void DMA19_IRQHandler(void)
{
    DMA_DRV_IRQHandler(19U);
}

/*! @brief DMA IRQ handler with the same name in the startup code*/
void DMA20_IRQHandler(void)
{
    DMA_DRV_IRQHandler(20U);
}

/*! @brief DMA IRQ handler with the same name in the startup code*/
void DMA21_IRQHandler(void)
{
    DMA_DRV_IRQHandler(21U);
}

/*! @brief DMA IRQ handler with the same name in the startup code*/
void DMA22_IRQHandler(void)
{
    DMA_DRV_IRQHandler(22U);
}

/*! @brief DMA IRQ handler with the same name in the startup code*/
void DMA23_IRQHandler(void)
{
    DMA_DRV_IRQHandler(23U);
}

/*! @brief DMA IRQ handler with the same name in the startup code*/
void DMA24_IRQHandler(void)
{
    DMA_DRV_IRQHandler(24U);
}

/*! @brief DMA IRQ handler with the same name in the startup code*/
void DMA25_IRQHandler(void)
{
    DMA_DRV_IRQHandler(25U);
}

/*! @brief DMA IRQ handler with the same name in the startup code*/
void DMA26_IRQHandler(void)
{
    DMA_DRV_IRQHandler(26U);
}

/*! @brief DMA IRQ handler with the same name in the startup code*/
void DMA27_IRQHandler(void)
{
    DMA_DRV_IRQHandler(27U);
}

/*! @brief DMA IRQ handler with the same name in the startup code*/
void DMA28_IRQHandler(void)
{
    DMA_DRV_IRQHandler(28U);
}

/*! @brief DMA IRQ handler with the same name in the startup code*/
void DMA29_IRQHandler(void)
{
    DMA_DRV_IRQHandler(29U);
}

/*! @brief DMA IRQ handler with the same name in the startup code*/
void DMA30_IRQHandler(void)
{
    DMA_DRV_IRQHandler(30U);
}

/*! @brief DMA IRQ handler with the same name in the startup code*/
void DMA31_IRQHandler(void)
{
    DMA_DRV_IRQHandler(31U);
}
#endif /* (FEATURE_DMA_VIRTUAL_CHANNELS > 16U) */
#endif /* FEATURE_DMA_SEPARATE_IRQ_LINES_PER_CHN */

#ifdef FEATURE_DMA_HAS_ERROR_IRQ

#if defined(CPU_YTM32B1HB0)
/*! @brief DMA ERROR IRQ handler with the same name in the startup code*/
void DMA0_Error_IRQHandler(void)
{
    const DMA_Type *dmaRegBase = DMA_DRV_GetDmaRegBaseAddr(0U);
    uint32_t error = DMA_GetErrorIntStatusFlag(dmaRegBase);
    uint8_t virtualChannel;

    for (virtualChannel = 0U;
         virtualChannel < FEATURE_DMA_CHANNELS;
         virtualChannel++)
    {
        if ((error & DMA_ERR_LSB_MASK) != 0UL)
        {
            DMA_DRV_ErrorIRQHandler(virtualChannel);
        }
        error = error >> 1U;
    }
#if (defined(FEATURE_DMA_SUPPORT_ECC_ERROR_CHECK) && FEATURE_DMA_SUPPORT_ECC_ERROR_CHECK == 1)
    if ((DMA_SECCES_SECCEF_MASK & dmaRegBase->SECCES) != 0UL)
    {
        DMA_DRV_ECCErrorIRQHandler(0U);
    }
#endif /* FEATURE_DMA_SUPPORT_ECC_ERROR_CHECK */
}
/*! @brief DMA ERROR IRQ handler with the same name in the startup code*/
void DMA1_Error_IRQHandler(void)
{
    const DMA_Type *dmaRegBase = DMA_DRV_GetDmaRegBaseAddr(1U);
    uint32_t error = DMA_GetErrorIntStatusFlag(dmaRegBase);
    uint8_t virtualChannel;

    for (virtualChannel = FEATURE_DMA_CHANNELS;
         virtualChannel < FEATURE_DMA_CHANNELS * 2;
         virtualChannel++)
    {
        if ((error & DMA_ERR_LSB_MASK) != 0UL)
        {
            DMA_DRV_ErrorIRQHandler(virtualChannel);
        }
        error = error >> 1U;
    }
#if (defined(FEATURE_DMA_SUPPORT_ECC_ERROR_CHECK) && FEATURE_DMA_SUPPORT_ECC_ERROR_CHECK == 1)
    if ((DMA_SECCES_SECCEF_MASK & dmaRegBase->SECCES) != 0UL)
    {
        DMA_DRV_ECCErrorIRQHandler(1U);
    }
#endif /* FEATURE_DMA_SUPPORT_ECC_ERROR_CHECK */
}
#else

/*! @brief DMA ERROR IRQ handler with the same name in the startup code*/
void DMA_Error_IRQHandler(void)
{
    const DMA_Type *dmaRegBase = DMA_DRV_GetDmaRegBaseAddr(0U);
    uint32_t error = DMA_GetErrorIntStatusFlag(dmaRegBase);
    uint8_t virtualChannel;

    for (virtualChannel = 0U;
         virtualChannel < FEATURE_DMA_VIRTUAL_CHANNELS;
         virtualChannel++)
    {
        if ((error & DMA_ERR_LSB_MASK) != 0UL)
        {
            DMA_DRV_ErrorIRQHandler(virtualChannel);
        }
        error = error >> 1U;
    }
}
#endif /* CPU_YTM32B1HB0 */

#endif /* FEATURE_DMA_HAS_ERROR_IRQ */

/*******************************************************************************
 * EOF
 ******************************************************************************/
