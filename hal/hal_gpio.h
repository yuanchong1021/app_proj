/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2026-05-11 10:00:15
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2026-06-29 10:11:46
 * @FilePath: \instrument\hal\hal_gpio.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/**
 * @file    hal_gpio.h
 * @brief   GPIO 硬件抽象层接口
 *
 * 该接口由 driver/gpio_drv.c 实现。应用层 / 服务层不得直接调用底层
 * 寄存器或厂商 SDK，所有 GPIO 操作必须经过本接口。
 */

#ifndef CLUSTER_HAL_GPIO_H
#define CLUSTER_HAL_GPIO_H

#include "types.h"
#include"YTM32B1MC0.h"

/*---------------------------------------------------------------------------
 *  逻辑引脚号定义（与硬件原理图编号对齐）
 *
 *  说明：使用立即数而非 enum 是为了与硬件文档/原理图直接保持一致，
 *        修改硬件分配时可通过比对原理图最快定位本文件。
 *---------------------------------------------------------------------------*/

/* === 数字输入：高电平有效 === */
#define PIN_IGN_ON          1u      /**< 点火 ON         */
#define PIN_BAT_ON          2u      /**< 蓄电池常电      */
#define PIN_POS_LIGHT       8u      /**< 小灯（位置灯）  */
#define PIN_TURN_RIGHT      18u     /**< 右转向灯        */
#define PIN_FOG_REAR        20u     /**< 后雾灯          */
#define PIN_FOG_FRONT       21u     /**< 前雾灯          */
#define PIN_TURN_LEFT       23u     /**< 左转向灯        */

/* === 数字输入：低电平有效 === */
#define PIN_SEATBELT_N      12u     /**< 安全带未系       */
#define PIN_DOOR_OPEN_N     14u     /**< 车门未关         */
#define PIN_AIRBAG_FAULT_N  16u     /**< 安全气囊故障      */
#define PIN_HIGH_BEAM_N     19u     /**< 远光灯           */
#define PIN_LOW_BEAM_N      22u     /**< 近光灯           */
#define PIN_CHARGER_N       24u     /**< 充电异常         */
#define PIN_OIL_PRESS_N     25u     /**< 机油压力低        */
#define PIN_EPS_FAULT_N     28u     /**< EPS 故障         */
#define PIN_BRAKE_FAULT_N   31u     /**< 制动故障         */
#define PIN_PARKING_N       32u     /**< 驻车制动         */

typedef enum{
    GPIO_PIN_0 = 0,
    GPIO_PIN_1,
    GPIO_PIN_2,
    GPIO_PIN_3,
    GPIO_PIN_4,
    GPIO_PIN_5,
    GPIO_PIN_6,
    GPIO_PIN_7,
    GPIO_PIN_8,
    GPIO_PIN_9,
    GPIO_PIN_10,
    GPIO_PIN_11,
    GPIO_PIN_12,
    GPIO_PIN_13,
    GPIO_PIN_14,
    GPIO_PIN_15,
    GPIO_PIN_16,
    GPIO_PIN_17,
    GPIO_PIN_18,
    GPIO_PIN_19,
    GPIO_PIN_20,
    GPIO_PIN_21,
    GPIO_PIN_22,
    GPIO_PIN_23,
    GPIO_PIN_24,
    GPIO_PIN_25,
    GPIO_PIN_26,
    GPIO_PIN_27,
    GPIO_PIN_28,
    GPIO_PIN_29,
    GPIO_PIN_30,
    GPIO_PIN_31,
}PIN_NUM;

typedef struct {
    GPIO_Type* port;
    uint16_t pin;
} PinMap_t;

static const PinMap_t pin_table[] = {
    [PIN_IGN_ON]   = {GPIOC, GPIO_PIN_7},
    [PIN_BAT_ON]   = {GPIOB, GPIO_PIN_0},
    [PIN_POS_LIGHT]   = {GPIOC, GPIO_PIN_6},
    [PIN_TURN_RIGHT]  = {GPIOC, GPIO_PIN_16},
    [PIN_FOG_REAR]  = {GPIOD, GPIO_PIN_5},
    [PIN_FOG_FRONT]  = {GPIOD, GPIO_PIN_6},
    [PIN_TURN_LEFT]  = {GPIOC, GPIO_PIN_2},

    [PIN_SEATBELT_N]  = {GPIOE, GPIO_PIN_3},
    [PIN_DOOR_OPEN_N]  = {GPIOD, GPIO_PIN_16},
    [PIN_AIRBAG_FAULT_N]  = {GPIOD, GPIO_PIN_15},
    [PIN_HIGH_BEAM_N]  = {GPIOC, GPIO_PIN_17},
    [PIN_LOW_BEAM_N]  = {GPIOD, GPIO_PIN_7},
    [PIN_CHARGER_N]  = {GPIOC, GPIO_PIN_3},
    [PIN_OIL_PRESS_N]  = {GPIOB, GPIO_PIN_4},
    [PIN_EPS_FAULT_N]  = {GPIOB, GPIO_PIN_5},
    [PIN_BRAKE_FAULT_N]  = {GPIOE, GPIO_PIN_8},
    [PIN_PARKING_N]  = {GPIOE, GPIO_PIN_9},
};


/**
 * @brief 读取数字输入引脚电平
 *
 * @param pin_id  逻辑引脚编号（由 driver/gpio_drv.c 内部映射到具体 PORT/PIN）
 * @return 当前电平（true = 高电平 / false = 低电平）
 */
bool HAL_GPIO_ReadPin(u8 pin_id);

/**
 * @brief 设置数字输出引脚电平
 *
 * @param pin_id  逻辑引脚编号
 * @param state   true = 输出高电平 / false = 输出低电平
 */
void HAL_GPIO_WritePin(u8 pin_id, bool state);

#endif /* CLUSTER_HAL_GPIO_H */
