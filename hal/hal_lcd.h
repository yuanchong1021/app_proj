/**
 * @file  hal_lcd.h
 * @brief LCD 段码屏 HAL（对接 SC5037）
 */
#ifndef CLUSTER_HAL_LCD_H
#define CLUSTER_HAL_LCD_H

#include "types.h"
#include "cfg_lcd_addr.h"
#include "pins_driver.h"
#include "YTM32B1MC0.h"

/* 背光控制引脚（按实际原理图修改） */
#define LCD_BACKLIGHT_PORT   GPIOD
#define LCD_BACKLIGHT_PIN    0u

void HAL_LCD_Clear(void);
void HAL_LCD_WriteSeg(u16 seg_addr, u8 seg_data);
void HAL_LCD_WriteNum(u16 base_addr, u16 value);
void HAL_LCD_WriteNum32(u16 base_addr, u32 value);
void HAL_LCD_WriteDecimalPoint(u16 base_addr, u8 pos);
void HAL_LCD_SetBacklight(u8 pct);

#endif /* CLUSTER_HAL_LCD_H */
