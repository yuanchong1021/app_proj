/**
 * @file    ambient.c
 * @brief   车外温度模块实现
 *
 * NTC 阻温表来源：传感器规格书（10kΩ@25℃ B = 3950 类型）
 * 由于嵌入式环境不使用浮点，曲线表使用"电阻 × 1000"作为索引，
 * 在两个相邻表项之间做线性插值得到温度。
 */

#include "ambient.h"
#include "hal_adc.h"

/*---------------------------------------------------------------------------
 *  内部状态
 *---------------------------------------------------------------------------*/
static u32  s_ambient_update_timer_ms = 0u;     /**< 距离上次更新的累积时间 */
static s16  s_ambient_temp_c          = 25;     /**< 上次成功换算的温度（℃）*/
static bool s_ambient_valid           = false;  /**< 上次换算是否有效        */

/*===========================================================================
 *  初始化
 *===========================================================================*/
void AMBIENT_Init(ClusterSystem_t *sys)
{
    s_ambient_update_timer_ms    = 0u;
    s_ambient_valid              = false;
    sys->lcd.ambient.valid       = false;
    sys->lcd.ambient.temp_c      = 0;
}

/*===========================================================================
 *  阻温曲线插值
 *===========================================================================*/
s16 AMBIENT_OhmToTemp(u32 ohms_x1000)
{
    /* 越界保护：电阻过大或过小都视为无效 */
    if (ohms_x1000 > 1500000u || ohms_x1000 < 100u) {
        return (s16)0x7FFF;
    }

    /* 阻温表：电阻 × 1000 → 对应温度 */
    static const struct { u32 r_x1000; s16 temp; } tbl[] = {
        {   2661u,  90 }, {   3685u,  80 }, {   5180u,  70 },
        {   7401u,  60 }, {  10764u,  50 }, {  15961u,  40 },
        {  24171u,  30 }, {  37461u,  20 }, {  59554u,  10 },
        {  97371u,   0 }, { 164235u, -10 }, { 286782u, -20 },
        { 520558u, -30 }, { 994350u, -40 },
    };
    u8 tbl_size = (u8)(sizeof(tbl) / sizeof(tbl[0]));

    /* 在两个相邻表项之间做线性插值 */
    for (u8 i = 0; i < tbl_size - 1u; i++) {
        if (ohms_x1000 <= tbl[i].r_x1000 && ohms_x1000 >= tbl[i+1].r_x1000) {
            s32 t = (s32)tbl[i].temp +
                    ((s32)(tbl[i+1].temp - tbl[i].temp) *
                     (s32)(ohms_x1000 - tbl[i].r_x1000)) /
                    (s32)(tbl[i+1].r_x1000 - tbl[i].r_x1000);
            return (s16)t;
        }
    }
    /* 表外兜底（取最末项温度） */
    return (s16)tbl[tbl_size - 1u].temp;
}

/*===========================================================================
 *  周期任务
 *===========================================================================*/
void AMBIENT_Task(ClusterSystem_t *sys)
{
    /* --- 1) 节流：每 CFG_AMBIENT_UPDATE_S 秒才进行一次换算 --- */
    s_ambient_update_timer_ms += 100u;
    if (s_ambient_update_timer_ms < (u32)CFG_AMBIENT_UPDATE_S * 1000u) {
        return;
    }
    s_ambient_update_timer_ms = 0u;

    /* --- 2) 读取 ADC --- */
    u16 adc_raw = HAL_ADC_ReadTempRaw(0u);

    /* 端值检测：ADC 0 或 4095 表示电路异常 */
    if (adc_raw == 0u || adc_raw >= 4095u) {
        s_ambient_valid = false;
    } else {
        /* --- 3) 由分压电路反推 NTC 电阻：
         *  R_ntc = R_ref × ADC / (4096 - ADC)
         *  其中 R_ref = 10000Ω，使用 ×1000 精度做插值
         */
        u32 ohms_x1000 = (u32)10000u * adc_raw / (4096u - adc_raw);
        s16 temp       = AMBIENT_OhmToTemp(ohms_x1000);
        if (temp == (s16)0x7FFF) {
            s_ambient_valid = false;
        } else {
            s_ambient_temp_c = temp;
            s_ambient_valid  = true;
        }
    }

    /* --- 4) 写回上下文 --- */
    sys->lcd.ambient.temp_c = s_ambient_temp_c;
    sys->lcd.ambient.valid  = s_ambient_valid;
}
