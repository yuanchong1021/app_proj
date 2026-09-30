/* 
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.                                                                  
 *                                                                                       
 * SPDX-License-Identifier: BSD-3-Clause                          
 */

/*!
 * @file i2c_irq.h
 * @version 1.4.0
 */

#ifndef I2C_IRQ_H__
#define I2C_IRQ_H__

#include "device_registers.h"

/*******************************************************************************
 * Code
 ******************************************************************************/

#if defined (YTM32B1L_SERIES)

/* Implementation of I2C0 master and slave handler named in startup code. */
void I2C0_IRQHandler(void);

/* Implementation of I2C1 master and slave handler named in startup code. */
void I2C1_IRQHandler(void);

#elif defined(YTM32B1M_SERIES)

#if (I2C_INSTANCE_COUNT > 0u)

/* I2C0 master handler named in startup code. */
void I2C0_Master_IRQHandler(void);

/* I2C0 slave handler named in startup code. */
void I2C0_Slave_IRQHandler(void);

#if(I2C_INSTANCE_COUNT > 1u)

/* I2C1 master handler named in startup code. */
void I2C1_Master_IRQHandler(void);

/* I2C1 slave handler named in startup code. */
void I2C1_Slave_IRQHandler(void);

#if(I2C_INSTANCE_COUNT > 2u)

/* I2C2 master handler named in startup code. */
void I2C2_Master_IRQHandler(void);

/* I2C2 slave handler named in startup code. */
void I2C2_Slave_IRQHandler(void);

#endif /* I2C_INSTANCE_COUNT == 3u */

#endif /* I2C_INSTANCE_COUNT == 2u */

#endif /* I2C_INSTANCE_COUNT > 0u */

#elif defined(YTM32B1H_SERIES)

#if (I2C_INSTANCE_COUNT > 0u)

/* Implementation of I2C0 handler named in startup code. */
void I2C0_Master_Slave_IRQHandler(void);

#endif /* I2C_INSTANCE_COUNT > 0u */

#if (I2C_INSTANCE_COUNT > 1u)

/* Implementation of I2C1 handler named in startup code. */
void I2C1_Master_Slave_IRQHandler(void);

#endif /* I2C_INSTANCE_COUNT > 1u */

#if (I2C_INSTANCE_COUNT > 2u)


/* Implementation of I2C2 handler named in startup code. */
void I2C2_Master_Slave_IRQHandler(void);

#endif /* I2C_INSTANCE_COUNT > 2u */

#if (I2C_INSTANCE_COUNT > 3u)
/* Implementation of I2C3 handler named in startup code. */
void I2C3_Master_Slave_IRQHandler(void);

#endif /* I2C_INSTANCE_COUNT > 3u */

#if (I2C_INSTANCE_COUNT > 4u)

/* Implementation of I2C4 handler named in startup code. */
void I2C4_Master_Slave_IRQHandler(void);

#endif /* I2C_INSTANCE_COUNT > 4u */

#endif

#endif /* I2C_IRQ_H__ */
/*******************************************************************************
 * EOF
 ******************************************************************************/
