/**
 ******************************************************************************
 * @file    meter_display.h
 * @brief   仪表段码 LCD 上层显示 API
 *
 *  本面板由 SC5037-G4-1 驱动,全部段均接在 COM0 上。
 *  面板规格(依据 CAD 原理图中的段-SEG 对照表):
 *    ——  30 个 7 段数码管位    (Digit 1 ~ Digit 30,每位 7 段 a/b/c/d/e/f/g)
 *    ——  77 个图标段           (R1 ~ R75,其中 R16/R45 各拆成 a/b 两段)
 *
 *  应用层只与图标名称和数字位打交道,底层 SEG 编号由本文件的映射表
 *  翻译给 sc5037 驱动。
 ******************************************************************************
 */

#ifndef METER_DISPLAY_H
#define METER_DISPLAY_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------- */
/*  面板数码管位数(Digit 1 ~ Digit 30,对应 position 0 ~ 29)          */
/* -------------------------------------------------------------------- */
#define METER_NUM_DIGITS    30U

/* -------------------------------------------------------------------- */
/*  图标段枚举(与 CAD 图一一对应)                                     */
/*                                                                      */
/*  命名规则:                                                          */
/*    ICON_Rnn    —— 对应 CAD 中的 "Rnn" 单个图标段                     */
/*    ICON_RnnA / ICON_RnnB —— 对应 "Rnna" / "Rnnb",单个图标被         */
/*                              拆成 2 个 SEG 控制(可独立点亮)        */
/*                                                                      */
/*  注意:枚举顺序决定了 g_meter_icon_table[] 的下标,不要随意调整。    */
/* -------------------------------------------------------------------- */
typedef enum {
    ICON_R1   =  0, ICON_R2,   ICON_R3,   ICON_R4,   ICON_R5,
    ICON_R6,        ICON_R7,   ICON_R8,   ICON_R9,   ICON_R10,
    ICON_R11,       ICON_R12,  ICON_R13,  ICON_R14,  ICON_R15,

    ICON_R16A,      ICON_R16B,                 /* R16 拆为 a/b 两段     */

    ICON_R17,       ICON_R18,  ICON_R19,  ICON_R20,  ICON_R21,
    ICON_R22,       ICON_R23,  ICON_R24,  ICON_R25,  ICON_R26,
    ICON_R27,       ICON_R28,  ICON_R29,  ICON_R30,  ICON_R31,
    ICON_R32,       ICON_R33,  ICON_R34,  ICON_R35,  ICON_R36,
    ICON_R37,       ICON_R38,  ICON_R39,  ICON_R40,  ICON_R41,
    ICON_R42,       ICON_R43,  ICON_R44,

    ICON_R45A,      ICON_R45B,                 /* R45 拆为 a/b 两段     */

    ICON_R46,       ICON_R47,  ICON_R48,  ICON_R49,  ICON_R50,
    ICON_R51,       ICON_R52,  ICON_R53,  ICON_R54,  ICON_R55,
    ICON_R56,       ICON_R57,  ICON_R58,  ICON_R59,  ICON_R60,
    ICON_R61,       ICON_R62,  ICON_R63,  ICON_R64,  ICON_R65,
    ICON_R66,       ICON_R67,  ICON_R68,  ICON_R69,  ICON_R70,
    ICON_R71,       ICON_R72,  ICON_R73,  ICON_R74,  ICON_R75,

    ICON__COUNT                                 /* 枚举元素个数(77)    */
} meter_icon_t;

/* -------------------------------------------------------------------- */
/*  公开 API                                                            */
/* -------------------------------------------------------------------- */

/**
 * @brief  初始化 SC5037 并清空仪表面板
 */
void Meter_Init(void);

/**
 * @brief  熄灭面板所有段(只清帧缓冲,需再调用 Meter_Refresh 才生效)
 */
void Meter_ClearAll(void);

/**
 * @brief  按图标枚举点亮/熄灭某段(只改帧缓冲)
 */
void Meter_SetIcon(meter_icon_t icon, bool on);

/**
 * @brief  用 R 编号访问图标(1 ~ 75,不含 a/b 变体)。
 *         对于 R16、R45 这类拆分图标请直接用 ICON_R16A / ICON_R45B 等。
 */
void Meter_SetIconR(uint8_t r_num, bool on);

/**
 * @brief  在指定数字位上显示一个字符
 * @param  position 0 ~ 29(对应 Digit 1 ~ Digit 30)
 * @param  c        '0'~'9','A'~'F','a'~'f',' ','-'
 */
void Meter_SetDigit(uint8_t position, char c);

/**
 * @brief  在[起始位, 起始位+长度)的数字位上右对齐显示有符号整数
 *         负数会占用一位显示减号;宽度不够会溢出显示 "-----"
 * @param  start_pos 起始数字位(0 ~ 29)
 * @param  width     使用的数字位个数
 * @param  value     待显示的整数
 */
void Meter_SetNumberField(uint8_t start_pos, uint8_t width, int32_t value);

/**
 * @brief  在前 width 位上显示整数(start_pos = 0 的快捷版本)
 */
void Meter_SetNumber(uint8_t width, int32_t value);

/**
 * @brief  按图标点亮电池(Meter_Set 的封装)—— 可根据实际 R 编号替换
 */
void Meter_SetBatteryLevel(uint8_t level_0_to_4);

/**
 * @brief  把帧缓冲刷新到 LCD,一帧刷一次即可
 */
void Meter_Refresh(void);

/**
 * @brief  自检:点亮面板上所有映射到 COM0 的段
 */
void Meter_SelfTest(void);

/**
 * @brief  调试接口:直接按物理 SEG 编号点亮/熄灭(绕过映射表)
 */
void Meter_SetSegmentRaw(uint16_t seg, bool on);

#ifdef __cplusplus
}
#endif

#endif /* METER_DISPLAY_H */
