/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file i2c_irq.c
 * @version 1.4.0
 */

#include "device_registers.h"
#include "i2c_driver.h"
#include "i2c_irq.h"



/*******************************************************************************
 * Define
 ******************************************************************************/


/*******************************************************************************
 * Code
 ******************************************************************************/

#if defined (YTM32B1L_SERIES)
/* Implementation of I2C0 master and slave handler named in startup code. */
void I2C0_IRQHandler(void)
{
    I2C_DRV_ModuleIRQHandler(0);
}
void I2C1_IRQHandler(void)
{
    I2C_DRV_ModuleIRQHandler(1);
}

#elif defined(YTM32B1M_SERIES)
#if (I2C_INSTANCE_COUNT > 0u)

/* Implementation of I2C0 master handler named in startup code. */
void I2C0_Master_IRQHandler(void) 
{
    I2C_DRV_MasterIRQHandler(0);
}

/* Implementation of I2C0 slave handler named in startup code. */
void I2C0_Slave_IRQHandler(void)
{
    I2C_DRV_SlaveIRQHandler(0);
}

#if(I2C_INSTANCE_COUNT > 1u)

/* Implementation of I2C1 master handler named in startup code. */
void I2C1_Master_IRQHandler(void)
{
    I2C_DRV_MasterIRQHandler(1);
}

/* Implementation of I2C1 slave handler named in startup code. */
void I2C1_Slave_IRQHandler(void)
{
    I2C_DRV_SlaveIRQHandler(1);
}

#endif

#if(I2C_INSTANCE_COUNT > 2u)

/* Implementation of I2C2 master handler named in startup code. */
void I2C2_Master_IRQHandler(void)
{
    I2C_DRV_MasterIRQHandler(2);
}

/* Implementation of I2C2 slave handler named in startup code. */
void I2C2_Slave_IRQHandler(void)
{
    I2C_DRV_SlaveIRQHandler(2);
}

#endif

#endif

#elif defined(YTM32B1H_SERIES)
#if (I2C_INSTANCE_COUNT > 0u)

/* Implementation of I2C0 handler named in startup code. */
void I2C0_Master_Slave_IRQHandler(void)
{
    I2C_DRV_ModuleIRQHandler(0);
}


#if(I2C_INSTANCE_COUNT > 1u)

/* Implementation of I2C1 handler named in startup code. */
void I2C1_Master_Slave_IRQHandler(void)
{
    I2C_DRV_ModuleIRQHandler(1);
}

#endif

#if(I2C_INSTANCE_COUNT > 2u)

/* Implementation of I2C2 handler named in startup code. */
void I2C2_Master_Slave_IRQHandler(void)
{
    I2C_DRV_ModuleIRQHandler(2);
}

#endif
#if(I2C_INSTANCE_COUNT > 3u)

/* Implementation of I2C3 handler named in startup code. */
void I2C3_Master_Slave_IRQHandler(void)
{
    I2C_DRV_ModuleIRQHandler(3);
}

#endif
#if(I2C_INSTANCE_COUNT > 4u)

/* Implementation of I2C4 handler named in startup code. */
void I2C4_Master_Slave_IRQHandler(void)
{
    I2C_DRV_ModuleIRQHandler(4);
}

#endif

#endif
#endif
/*******************************************************************************
 * EOF
 ******************************************************************************/
