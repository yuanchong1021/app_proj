/**
 * @file    tpms_data.c
 * @brief   胎压数据处理模块实现
 *
 * 4 路胎压（LF / RF / LR / RR）使用 PROC_TIRE 宏批量展开，
 * 避免 4 段重复代码。
 */

#include "tpms_data.h"

/*===========================================================================
 *  初始化
 *===========================================================================*/
void TPMS_Init(ClusterSystem_t *sys)
{
    TpmsLcdData_t *t = &sys->lcd.tpms;
    t->press_lf_kpa = t->press_rf_kpa = t->press_lr_kpa = t->press_rr_kpa = 0u;
    t->show_lf  = t->show_rf  = t->show_lr  = t->show_rr  = false;
    t->blink_lf = t->blink_rf = t->blink_lr = t->blink_rr = false;
}

/*===========================================================================
 *  原始值换算
 *
 *  CAN 数据 raw（0~254）→ kPa：raw × 5.5（× 55/10 整数算）
 *  raw == 0xFF 表示该胎数据无效（如传感器丢失）
 *===========================================================================*/
u16 TPMS_RawToKpa(u8 raw)
{
    if (raw == 0xFFu) { return 0xFFFFu; }       /* 无效标志 */
    u16 kpa = (u16)((u32)raw * 55u / 10u);
    return (kpa > CFG_TPMS_MAX_PRESSURE_KPA) ? CFG_TPMS_MAX_PRESSURE_KPA : kpa;
}

/*===========================================================================
 *  周期任务
 *===========================================================================*/
void TPMS_Task(ClusterSystem_t *sys)
{
    /* 自检阶段不更新 */
    if (!sys->selfcheck_done) { return; }

    const CanTPMS_t *can = &sys->can_rx.tpms;
    TpmsLcdData_t   *lcd = &sys->lcd.tpms;

    /* CAN 超时：4 路都隐藏数值 + 闪烁报警 */
    if (can->timeout) {
        lcd->show_lf  = lcd->show_rf  = lcd->show_lr  = lcd->show_rr  = false;
        lcd->blink_lf = lcd->blink_rf = lcd->blink_lr = lcd->blink_rr = true;
        return;
    }

    /*
     * 单胎处理宏：把 (raw, status) → (show, press_kpa, blink) 的转换抽出来
     *   - 原始值 0xFF → 不显示数值
     *   - 状态非 NO_WARN（异常）→ 闪烁
     */
#define PROC_TIRE(raw, status, show_f, press_f, blink_f)            \
    do {                                                             \
        u16 kpa = TPMS_RawToKpa(raw);                                \
        if (kpa == 0xFFFFu) {                                        \
            (show_f)  = false;                                       \
            (press_f) = 0u;                                          \
        } else {                                                     \
            (show_f)  = true;                                        \
            (press_f) = kpa;                                         \
        }                                                            \
        (blink_f) = ((status) != TPMS_NO_WARN);                      \
    } while (0)

    PROC_TIRE(can->press_lf_raw, can->status_lf,
              lcd->show_lf, lcd->press_lf_kpa, lcd->blink_lf);
    PROC_TIRE(can->press_rf_raw, can->status_rf,
              lcd->show_rf, lcd->press_rf_kpa, lcd->blink_rf);
    PROC_TIRE(can->press_lr_raw, can->status_lr,
              lcd->show_lr, lcd->press_lr_kpa, lcd->blink_lr);
    PROC_TIRE(can->press_rr_raw, can->status_rr,
              lcd->show_rr, lcd->press_rr_kpa, lcd->blink_rr);

#undef PROC_TIRE
}
