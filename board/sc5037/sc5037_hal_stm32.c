/**
 ******************************************************************************
 * @file    sc5037_hal_stm32.c
 * @brief   STM32 HAL 库的 HAL 参考实现 —— 4 线 SPI
 *
 *  本文件仅为移植参考,请按实际工程修改 SPI 句柄与引脚宏。
 *
 *  典型接线(单片模式,4 线 SPI):
 *
 *    MCU 引脚            SC5037 引脚
 *    ---------------     -------------
 *    SPIx_SCK (PTC15)  →   D1  (SCK)
 *    SPIx_MOSI(PTC1)  →   D0  (SI)
 *    GPIO    (PTC14)   →   /CS
 *    GPIO    (PB0)   →   /RST
 *    GPIO    (PB1)   →   A0  (D/C)
 *    3V3                  VDD / VDD1 / VDD2 / VDD3
 *    GND                  VSS / VSS1 / VSS2 / VSS3
 *
 *  LCD 侧硬接线(按 datasheet):
 *    IF1 = H,  IF2 = L              →  4 线 SPI 8 bit
 *    /WR = /RD = VDD                →  SPI 模式下必须上拉
 *    D2..D7          = VDD           →  未使用,按规格书要求固定
 *    MS              = VSS           →  单片工作
 *    T0, T10         = VSS           →  测试引脚按规格固定
 *    T1..T9, T11, Dummy               →  全部悬空
 *    CL, SYNC, DON                    →  非主从模式下悬空
 ******************************************************************************
 */

#include "sc5037.h"
#include "pins_gpio_hw_access.h"
#include "spi_master_driver.h"
#include "hal_tick.h"
/* 使用时请 include 你的 STM32 HAL 头文件,例如:                        */
/* #include "stm32f1xx_hal.h"                                          */
/* #include "stm32f4xx_hal.h"                                          */

/* 外部 SPI 句柄,通常在 main.c 或 spi.c 中声明                         */
//extern SPI_HandleTypeDef hspi1;
#define HAL_MAX_DELAY  10
uint8_t MasterRxBuf[HAL_MAX_DELAY] = {0};

/* 引脚别名 —— 请按 CubeMX 生成的名字修改                               */
#define LCD_CS_GPIO_Port   GPIOC
#define LCD_CS_Pin         14U
#define LCD_RST_GPIO_Port  GPIOA
#define LCD_RST_Pin        7U
#define LCD_A0_GPIO_Port   GPIOC
#define LCD_A0_Pin         0U

/* =================================================================== */
/*  驱动要求的 HAL 回调 —— 本工程必须实现                              */
/* =================================================================== */

/* 片选控制:active=true → 拉低选中;false → 拉高释放                   */
void SC5037_HAL_CS(bool active)
{
    PINS_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin,
                      active ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

/* 复位控制:active=true → 拉低进入复位;false → 拉高退出复位           */
void SC5037_HAL_RST(bool active)
{
    PINS_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin,
                      active ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

/* A0(D/C)控制:data_mode=true → 数据(A0=1),false → 命令(A0=0)  */
void SC5037_HAL_A0(bool data_mode)
{
    PINS_GPIO_WritePin(LCD_A0_GPIO_Port, LCD_A0_Pin,
                      data_mode ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/* SPI 发送一个字节(阻塞);如需提速可改成 DMA/中断                    */
void SC5037_HAL_SPI_Write(uint8_t byte)
{
    SPI_DRV_MasterTransferBlocking(2, &byte, MasterRxBuf, 1, 10);  // 10ms 超时
}

/* 毫秒级阻塞延时                                                       */
void SC5037_HAL_Delay_Ms(uint32_t ms)
{
    uint32_t start = HAL_GetTickMs();
    while ((HAL_GetTickMs() - start) < ms) { }
}
