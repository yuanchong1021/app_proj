/**
 * @file  hal_gpio.c
 * @brief GPIO HAL 实现
 */
#include "hal_gpio.h"
#include "pins_driver.h"

bool HAL_GPIO_ReadPin(u8 pin_id)
{
    return (bool)PINS_DRV_ReadPin(pin_table[pin_id].port, pin_table[pin_id].pin);
}

void HAL_GPIO_WritePin(u8 pin_id, bool s)
{
    PINS_DRV_WritePin(pin_table[pin_id].port, pin_table[pin_id].pin, s ? 1u : 0u);
}
