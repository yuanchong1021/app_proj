/**
 * @file  hal_backlight.c
 * @brief LCD 背光亮度 HAL 实现（MPWM 调光）
 *
 * ===========================================================================
 * ⚠ 重要：本文件的 PWM 硬件参数一律保持原工程行为，未经实测不要改动
 * ===========================================================================
 * 本驱动中 MPWM 的 "channel" 与 "submodule" 是 1:1 对应的，不是 A/B 配对：
 *   BACKLIGHT_Init() 里 MPWM_DRV_EnableCounter(0, 0x03) 使能 submodule 0+1，
 *   而背光走 channel 1 → LDOK 必须打 bit1，即 (0x01 << 1)。
 * 曾经按 "submodule = channel / 2" 推导成 bit0，结果 LDOK 打在错误的
 * submodule 上，新的 period/duty 永远不会被 load，背光直接不亮。
 * 现已恢复为 (0x01 << BL_MPWM_CHANNEL)，与原工程完全一致。
 *
 * 本次相对原工程只保留两处**不改变默认行为**的整理：
 *   1) 占空比换算集中到 BL_DutyFromPct() 并做入参钳位（pct ≤ 100 时结果
 *      与原式 0x8000*pct/100 完全相同，只是 pct > 100 时不再溢出）；
 *   2) 满量程 / 极性提成宏，方便后续实测后一行调整。
 *
 * 若背光亮度仍与 pct 对不上，再按下面两条排查（不要盲改）：
 *   a) BL_DUTY_FULL_SCALE：若驱动第 5 参数期望的不是 Q15（0x8000=100%）
 *      而是计数值，改成实际 period_ticks；
 *   b) BL_ACTIVE_LOW：若背光驱动管为低有效，置 1。
 */

#include "hal_backlight.h"

/*---------------------------------------------------------------------------
 *  背光 PWM 参数（与原工程一致，勿随意改动）
 *-------------------------------------------------------------------------*/
#define BL_MPWM_INST        0u          /**< MPWM 实例号                    */
#define BL_MPWM_CHANNEL     1u          /**< 背光所在通道（= submodule 号） */
#define BL_PWM_FREQ_HZ      20000u      /**< 调光频率 20kHz                 */

/** 占空比满量程。原工程按 Q15 定标，0x8000 = 100% */
#define BL_DUTY_FULL_SCALE  0x8000u

/** 背光驱动极性：0 = 高有效（占空比越大越亮，原工程行为），1 = 低有效 */
#ifndef BL_ACTIVE_LOW
#define BL_ACTIVE_LOW       0
#endif

/** LDOK 掩码：本驱动 channel 与 submodule 一一对应，直接按通道号移位。
 *  BL_MPWM_CHANNEL = 1 → (0x01 << 1)，与原工程写法等价。 */
#define BL_LDOK_MASK        (0x01u << BL_MPWM_CHANNEL)

/*---------------------------------------------------------------------------
 *  百分比 → 占空比（pct ≤ 100 时与原式 0x8000*pct/100 结果一致）
 *-------------------------------------------------------------------------*/
static uint32_t BL_DutyFromPct(u8 pct)
{
    uint32_t p = (pct > 100u) ? 100u : (uint32_t)pct;

#if (BL_ACTIVE_LOW != 0)
    p = 100u - p;
#endif

    return ((uint32_t)BL_DUTY_FULL_SCALE * p) / 100u;
}

void HAL_BACKLIGHT_Set(u8 pct)
{
    MPWM_DRV_UpdateChannelPeriodAndDutyCycle(BL_MPWM_INST,
                                             BL_MPWM_CHANNEL,
                                             MPWM_PERIOD_IN_HZ,
                                             BL_PWM_FREQ_HZ,
                                             BL_DutyFromPct(pct));

    MPWM_DRV_SetLdok(BL_MPWM_INST, BL_LDOK_MASK);
}