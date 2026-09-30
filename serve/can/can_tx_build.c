/**
 * @file    can_tx_build.c
 * @brief   仪表外发 CAN 报文打包实现（0x380 / 0x381）
 *
 * 打包依据：《长安跨越仪表软件-袁工20260728.xlsx》工作表“外发CAN”测试用例
 *           反推出的外发矩阵（见《仪表外发CAN矩阵_0x380_0x381.xlsx》）。
 *
 * 矩阵约定：bit 编号 = 字节号 × 8 + 字节内 bit 号（byte0.bit0 = bit0）；
 *           用例中给出的“值”是该信号所在字节的整字节掩码值；
 *           多字节信号为 Motorola（大端）排列。
 *
 *   0x380  byte0 : bit0 SeatBelt / bit1 SeatBelt_Valid
 *          byte1 : ODO 高字节        \  16bit，精度 1 km
 *          byte2 : ODO 低字节        /
 *          byte3 : 预留 0
 *          byte4 : bit1 短接BAT / bit2 对地短路 / bit3 开路
 *          byte5 : Remain_Oil 0~100 %，0x7F = 开路/短路无效
 *          byte6~7 : 预留 0
 *
 *   0x381  byte0 : ODO[23:16]        \
 *          byte1 : ODO[15:8]          >  24bit，精度 0.1 km
 *          byte2 : ODO[7:0]          /
 *          byte3 : bit0 预留 / bit1~7 Remain_Oil（整字节值 = 百分比 × 2）
 *          byte4 : bit1 SeatBelt / bit2 副驾安全带 / bit3~4 Turn_Signal
 *                  bit5 短接BAT / bit6 对地短路 / bit7 开路
 *          byte5 : Ambient_Temp，0xFF = 无效
 *          byte6~7 : 预留 0
 *
 * 燃油百分比通过 FUEL_GetPercent() 获取，外温走 sys->lcd.ambient 路径。
 */

#include "can_tx_build.h"
#include "fuel.h"        /* 提供 FUEL_GetPercent / FUEL_GetSensorFault */

/*---------------------------------------------------------------------------
 *  常量定义
 *-------------------------------------------------------------------------*/
#define FUEL_PERCENT_MAX        100u        /* 剩余油量物理上限 */
#define FUEL_PERCENT_INVALID    0x7Fu       /* 开路/短路时的无效码（用例要求 0x7F） */

#define ODO_380_MAX             0xFFFFu     /* 0x380 ODO：16bit，1 km/bit   */
#define ODO_381_MAX             0x00FFFFFFu /* 0x381 ODO：24bit，0.1 km/bit */

#define TURN_SIGNAL_NONE        0u
#define TURN_SIGNAL_LEFT        1u
#define TURN_SIGNAL_RIGHT       2u
#define TURN_SIGNAL_BOTH        3u

#define AMBIENT_TEMP_INVALID    0xFFu       /* 外温无效码 */

/*---------------------------------------------------------------------------
 *  内部辅助
 *-------------------------------------------------------------------------*/

/**
 * @brief  取外发用的剩余油量百分比
 * @note   开路/短路时必须发满量程 0x7F（0x380 的 byte5 = 0x7F，
 *         0x381 的 byte3 = 0x7F << 1 = 0xFE），否则用例“外发剩余油量-2”不通过。
 */
static u8 CAN_GetFuelPercentSignal(const ClusterSystem_t *sys,
                                   FuelSensorFault_t fault)
{
    u8 percent;

    if ((fault == FUEL_SENSOR_OPEN) || (fault == FUEL_SENSOR_SHORT)) {
        percent = FUEL_PERCENT_INVALID;
    } else {
        percent = (u8)FUEL_GetPercent(sys);
        if (percent > FUEL_PERCENT_MAX) {
            percent = FUEL_PERCENT_MAX;
        }
    }

    return percent;
}

/**
 * @brief  安全带外发状态
 * @note   用例：硬线拉低 → 发 0；硬线悬空 → 发 1。
 *         此处假定 hw_input.seatbelt_n 保存的是引脚电平（1 = 高/悬空）。
 *         若该字段实际已做过一次取反，请把返回值改回取反形式。
 */
static u8 CAN_GetSeatbeltSignal(const ClusterSystem_t *sys)
{
    return sys->hw_input.seatbelt_n ? 1u : 0u;
}

/**
 * @brief  转向灯编码：0=无 1=左 2=右 3=同时（双闪）
 */
static u8 CAN_GetTurnSignal(const ClusterSystem_t *sys)
{
    u8 turn;

    if (sys->hw_input.turn_left && sys->hw_input.turn_right) {
        turn = TURN_SIGNAL_BOTH;
    } else if (sys->hw_input.turn_left) {
        turn = TURN_SIGNAL_LEFT;
    } else if (sys->hw_input.turn_right) {
        turn = TURN_SIGNAL_RIGHT;
    } else {
        turn = TURN_SIGNAL_NONE;
    }

    return turn;
}

/*===========================================================================
 *  0x380 ：燃油 / 里程 / 安全带
 *===========================================================================*/
void CAN_BuildTx380(const ClusterSystem_t *sys, u8 *data)
{
    CanTx380_t tx;
    FuelSensorFault_t fault = FUEL_GetSensorFault();
    u32 odo_km;

    /* --- 1) 收集业务数据 --- */
    tx.seatbelt_drv       = CAN_GetSeatbeltSignal(sys);
    tx.seatbelt_drv_valid = 0u;                     /* 用例未覆盖，固定发 0 */

    tx.fuel_open      = (fault == FUEL_SENSOR_OPEN)  ? 1u : 0u;  /* 400Ω 开路 */
    tx.fuel_short_gnd = (fault == FUEL_SENSOR_SHORT) ? 1u : 0u;  /* 8Ω 对地短路 */
    tx.fuel_short_bat = 0u;                         /* 硬件电路无法区分接BAT/GND，保留 0 */
    tx.fuel_percent   = CAN_GetFuelPercentSignal(sys, fault);

    /* 0x380 中里程精度为 1 km（用例：总里程 100km → 信号值 100） */
    odo_km = (u32)sys->lcd.odometer.odo_km;
    if (odo_km > ODO_380_MAX) {
        odo_km = ODO_380_MAX;                       /* 16bit 上限，防止翻转 */
    }
    tx.odometer_km = (u16)odo_km;

    /* --- 2) 按 CAN 矩阵打包到 8 字节 --- */
    data[0] = (u8)((tx.seatbelt_drv       & 0x01u) |
                   ((tx.seatbelt_drv_valid & 0x01u) << 1u));

    /* ODO：bit16(LSB) ~ bit15(MSB)，Motorola —— byte1 高字节、byte2 低字节 */
    data[1] = (u8)((tx.odometer_km >> 8u) & 0xFFu);
    data[2] = (u8)( tx.odometer_km        & 0xFFu);

    data[3] = 0u;                                   /* 预留 */

    data[4] = (u8)(((tx.fuel_short_bat & 0x01u) << 1u) |   /* bit33 */
                   ((tx.fuel_short_gnd & 0x01u) << 2u) |   /* bit34 → 0x04 */
                   ((tx.fuel_open      & 0x01u) << 3u));   /* bit35 → 0x08 */

    /* Remain_Oil：bit40~47，整字节，0x7F = 无效 */
    data[5] = (u8)tx.fuel_percent;

    data[6] = 0u;
    data[7] = 0u;
}

/*===========================================================================
 *  0x381 ：扩展状态（含外温与转向）
 *===========================================================================*/
void CAN_BuildTx381(const ClusterSystem_t *sys, u8 *data)
{
    CanTx381_t tx;
    FuelSensorFault_t fault = FUEL_GetSensorFault();
    u32 odo_x10;

    /* --- 1) 收集业务数据 --- */

    /* 0x381 中里程精度为 0.1 km（用例：总里程 100km → 信号值 100×10 = 1000）
     * 注：系统当前只保存整公里的总里程，故小数位恒为 0；
     *     若后续总里程支持 0.1km 分辨率，请在此处补上小数位。
     */
    odo_x10 = (u32)sys->lcd.odometer.odo_km * 10u;
    if (odo_x10 > ODO_381_MAX) {
        odo_x10 = ODO_381_MAX;                      /* 24bit 上限 */
    }
    tx.odometer_km_x10 = odo_x10;

    tx.fuel_open      = (fault == FUEL_SENSOR_OPEN)  ? 1u : 0u;
    tx.fuel_short_gnd = (fault == FUEL_SENSOR_SHORT) ? 1u : 0u;
    tx.fuel_short_bat = 0u;
    tx.fuel_percent   = CAN_GetFuelPercentSignal(sys, fault);

    tx.seatbelt_drv   = CAN_GetSeatbeltSignal(sys);
    tx.seatbelt_pass  = 0u;                         /* 用例未覆盖，固定发 0 */
    tx.turn_signal    = CAN_GetTurnSignal(sys);

    /* 外温编码：物理值 + CFG_AMBIENT_OFFSET，无效时 0xFF */
    if (sys->lcd.ambient.valid) {
        tx.ambient_temp = (s8)(sys->lcd.ambient.temp_c + CFG_AMBIENT_OFFSET);
    } else {
        tx.ambient_temp = (s8)AMBIENT_TEMP_INVALID;
    }

    /* --- 2) 按 CAN 矩阵打包到 8 字节 --- */

    /* ODO：bit16(LSB) ~ bit7(MSB)，Motorola 24bit —— byte0 为高字节 */
    data[0] = (u8)((tx.odometer_km_x10 >> 16u) & 0xFFu);
    data[1] = (u8)((tx.odometer_km_x10 >>  8u) & 0xFFu);
    data[2] = (u8)( tx.odometer_km_x10         & 0xFFu);

    /* Remain_Oil：bit25~31，占 byte3 的 bit1~7，整字节值 = 百分比 × 2 */
    data[3] = (u8)((tx.fuel_percent & 0x7Fu) << 1u);

    data[4] = (u8)(((tx.seatbelt_drv   & 0x01u) << 1u) |   /* bit33 → 0x02 */
                   ((tx.seatbelt_pass  & 0x01u) << 2u) |   /* bit34 */
                   ((tx.turn_signal    & 0x03u) << 3u) |   /* bit35~36 → 0x08/0x10/0x18 */
                   ((tx.fuel_short_bat & 0x01u) << 5u) |   /* bit37 */
                   ((tx.fuel_short_gnd & 0x01u) << 6u) |   /* bit38 → 0x40 */
                   ((tx.fuel_open      & 0x01u) << 7u));   /* bit39 → 0x80 */

    data[5] = (u8)tx.ambient_temp;

    data[6] = 0u;
    data[7] = 0u;
}