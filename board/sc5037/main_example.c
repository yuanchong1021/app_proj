/**
 ******************************************************************************
 * @file    main_example.c
 * @brief   SC5037 + 30 位段码仪表屏的使用示例
 *
 *  典型使用流程:
 *    1. 上电后调用 Meter_Init() 完成硬件复位与控制器初始化;
 *    2. 按需修改帧缓冲: Meter_SetIcon / Meter_SetDigit / Meter_SetNumber;
 *    3. 调用 Meter_Refresh() 把帧缓冲统一推送到 LCD;
 *    4. 两次刷新之间,控制器自动驱动波形,不需要 MCU 干预。
 ******************************************************************************
 */

#include "sc5037.h"
#include "meter_display.h"

/* ---- 示例 1:在 Digit1 ~ Digit6 显示一个温度读数 ------------------ */
static void demo_temperature(int32_t centigrade_x10)
{
    Meter_ClearAll();

    /* 假设前 6 位用于显示温度,格式 "  25.3"                         */
    /* centigrade_x10 是实际温度 × 10,例如 253 表示 25.3 ℃          */
    Meter_SetNumberField(0, 6, centigrade_x10);

    /* 点亮温度单位图标(这里用 R1 作示例,实际请按 CAD 替换)        */
    Meter_SetIcon(ICON_R1, true);

    /* 点亮小数点(假设 R2 是 Digit5 之后的小数点)                   */
    Meter_SetIcon(ICON_R2, true);

    Meter_Refresh();
}

/* ---- 示例 2:同屏显示多个读数(30 位屏可分成多段区域)------------ */
static void demo_multi_region(int32_t voltage_mV,
                              int32_t current_mA,
                              int32_t power_mW)
{
    Meter_ClearAll();

    /* 电压:6 位,显示在 Digit1..6                                    */
    Meter_SetNumberField(0, 6, voltage_mV);

    /* 电流:6 位,显示在 Digit7..12                                   */
    Meter_SetNumberField(6, 6, current_mA);

    /* 功率:8 位,显示在 Digit13..20                                  */
    Meter_SetNumberField(12, 8, power_mW);

    /* 后面 10 位(Digit21..30)留空,可显示日期时间或状态            */
    Meter_Refresh();
}

/* ---- 示例 3:2Hz 闪烁图标(例如报警) ----------------------------- */
static void demo_alarm_blink(uint32_t tick_250ms)
{
    /* 假设 R37 是报警图标 —— 实际请按 CAD 替换                      */
    Meter_SetIconR(37, (tick_250ms & 1U) != 0U);
    Meter_Refresh();
}

/* ---- 示例 4:整屏消息滚动(逐位写入字符)------------------------- */
static void demo_text(const char *msg)
{
    /* 把字符串直接铺到前 N 位(最多 METER_NUM_DIGITS 位)             */
    Meter_ClearAll();
    for (uint8_t i = 0; i < METER_NUM_DIGITS && msg[i] != '\0'; i++) {
        Meter_SetDigit(i, msg[i]);
    }
    Meter_Refresh();
}

/* ---- 上电自检:点亮全部已映射 SEG --------------------------------- */
static void demo_selftest(void)
{
    Meter_SelfTest();                 /* 全部点亮           */
    SC5037_HAL_Delay_Ms(800);

    Meter_ClearAll();
    Meter_Refresh();                  /* 全部熄灭           */
    SC5037_HAL_Delay_Ms(200);
}

/* ---- 单独测试某个 SEG(调试映射表用)----------------------------- */
static void demo_walk_single_seg(void)
{
    /* 从 SEG0 开始,每 100ms 点亮一个 SEG,依次走到 SEG319。
     * 可以配合 CAD 图人工核对映射表是否正确。                         */
    for (uint16_t s = 0; s < 320; s++) {
        Meter_ClearAll();
        Meter_SetSegmentRaw(s, true);
        Meter_Refresh();
        SC5037_HAL_Delay_Ms(100);
    }
}

/* =================================================================== */
/*  主程序                                                             */
/* =================================================================== */
int app_main(void)
{
    /* 1. 上电初始化                                                   */
    Meter_Init();

    /* 2. 上电自检                                                     */
    demo_selftest();

    /* 3. 开机问候("HELLO") —— 测试字符表                            */
    demo_text("HELLO");
    SC5037_HAL_Delay_Ms(1000);

    /* 4. 主循环                                                       */
    uint32_t tick = 0;
    for (;;) {
        int32_t mV = 12345 + (int32_t)((tick & 7U) * 10U);  /* 模拟 ADC */
        int32_t mA =  5000 - (int32_t)((tick & 3U) * 7U);
        int32_t mW = mV * mA / 1000;

        demo_multi_region(mV, mA, mW);

        /* 每 4 秒闪烁一下报警图标                                     */
        demo_alarm_blink(tick);

        SC5037_HAL_Delay_Ms(250);
        tick++;
    }
    /* 不会到达 */
}
