/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file spi_irq.c
 * @version 1.4.0
 */

#include "device_registers.h"
#include "spi_shared_function.h"


/*!
 * @addtogroup spi_driver
 * @{
 */

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

#ifdef SPI0

/*!
 * @brief This function is the implementation of SPI0 handler named in startup code.
 *
 * It passes the instance to the shared SPI IRQ handler.
 */
void SPI0_IRQHandler(void);

void SPI0_IRQHandler(void)
{
    SPI_DRV_IRQHandler(0U);
}

#endif

#ifdef SPI1

/*!
 * @brief This function is the implementation of SPI1 handler named in startup code.
 *
 * It passes the instance to the shared SPI IRQ handler.
 */
void SPI1_IRQHandler(void);

void SPI1_IRQHandler(void)
{
    SPI_DRV_IRQHandler(1U);
}

#endif

#ifdef SPI2

/*!
 * @brief This function is the implementation of SPI2 handler named in startup code.
 *
 * It passes the instance to the shared SPI IRQ handler.
 */
void SPI2_IRQHandler(void);

void SPI2_IRQHandler(void)
{
    SPI_DRV_IRQHandler(2U);
}

#endif

#ifdef SPI3

/*!
 * @brief This function is the implementation of SPI3 handler named in startup code.
 *
 * It passes the instance to the shared SPI IRQ handler.
 */
void SPI3_IRQHandler(void);

void SPI3_IRQHandler(void)
{
    SPI_DRV_IRQHandler(3U);
}

#endif

#ifdef SPI4

/*!
 * @brief This function is the implementation of SPI4 handler named in startup code.
 *
 * It passes the instance to the shared SPI IRQ handler.
 */
void SPI4_IRQHandler(void);

void SPI4_IRQHandler(void)
{
    SPI_DRV_IRQHandler(4U);
}

#endif

#ifdef SPI5

/*!
 * @brief This function is the implementation of SPI5 handler named in startup code.
 *
 * It passes the instance to the shared SPI IRQ handler.
 */
void SPI5_IRQHandler(void);

void SPI5_IRQHandler(void)
{
    SPI_DRV_IRQHandler(5U);
}

#endif

#ifdef SPI6

/*!
 * @brief This function is the implementation of SPI6 handler named in startup code.
 *
 * It passes the instance to the shared SPI IRQ handler.
 */
void SPI6_IRQHandler(void);

void SPI6_IRQHandler(void)
{
    SPI_DRV_IRQHandler(6U);
}

#endif

#ifdef SPI7

/*!
 * @brief This function is the implementation of SPI7 handler named in startup code.
 *
 * It passes the instance to the shared SPI IRQ handler.
 */
void SPI7_IRQHandler(void);

void SPI7_IRQHandler(void)
{
    SPI_DRV_IRQHandler(7U);
}

#endif


/*! @} */

/*******************************************************************************
 * EOF
 ******************************************************************************/

