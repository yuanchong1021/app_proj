/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file dma_irq.h
 * @version 1.4.0
 */

#ifndef DMA_IRQ_H
#define DMA_IRQ_H

#include "dma_hw_access.h"

/*! @brief DMA channel interrupt handler, implemented in driver c file. */
void DMA_DRV_IRQHandler(uint8_t virtualChannel);

#ifdef FEATURE_DMA_HAS_ERROR_IRQ

/*! @brief DMA error interrupt handler, implemented in driver c file. */
void DMA_DRV_ErrorIRQHandler(uint8_t virtualChannel);

#if (defined(FEATURE_DMA_SUPPORT_ECC_ERROR_CHECK) && FEATURE_DMA_SUPPORT_ECC_ERROR_CHECK == 1)
/*! @brief DMA ECC error interrupt handler, implemented in driver c file. */
void DMA_DRV_ECCErrorIRQHandler(uint8_t dmaInstance);
#endif /* FEATURE_DMA_SUPPORT_ECC_ERROR_CHECK */

#endif /* FEATURE_DMA_HAS_ERROR_IRQ */

#endif /* DMA_IRQ_H */
/*******************************************************************************
 * EOF
 ******************************************************************************/
