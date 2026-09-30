/**
 * @file    voltage.c
 * @brief   12V 电源电压监测实现
 *
 * 设计要点：
 *   1) 滤波先行。ADC 单点值受点火、喷油、背光 PWM 干扰会跳动，直接拿去
 *      比阈值必然抖。这里用移位一阶低通，无乘除、无浮点。
 *   2) 迟滞 + 确认时间双重防抖。迟滞解决"停在阈值上反复翻转"，确认时间
 *      解决"起动瞬间跌落被误判为欠压"。两者作用不同，都需要。
 *   3) 只输出状态，不直接驱动任何显示。指示灯 / 段码 / CAN 由各自模块
 *      读 sys->voltage 自行决定，保持与工程既有分层一致。
 */

#include "voltage.h"
#include "hal_adc.h"
#include "cfg_power.h"

/*---------------------------------------------------------------------------
 *  内部状态
 *-------------------------------------------------------------------------*/
static u32         s_filt_mv    = 0u;                 /* 滤波累加器（放大后） */
static bool        s_filt_init  = false;
static VoltState_t s_cand_state = VOLT_STATE_INIT;    /* 候选状态           */
static u32         s_cand_ms    = 0u;                  /* 候选状态起始时刻   */

/* 滤波器内部按 (1 << SHIFT) 倍放大保存，避免整数截断累积误差 */
#define FILT_SCALE      (1u << CFG_VOLTAGE_FILTER_SHIFT)

/*---------------------------------------------------------------------------
 *  阈值判定（含迟滞）
 *
 *  规则：向"更严重"方向用原始阈值立即判定；向"恢复"方向必须多走
 *        CFG_VOLTAGE_TOLERANCE_MV 才允许退出，这样电压停在阈值上不会翻转。
 *-------------------------------------------------------------------------*/
static VoltState_t volt_classify_fresh(u16 mv)
{
    if (mv > CFG_VOLTAGE_NORMAL_MAX_MV)  { return VOLT_STATE_OVER;   }
    if (mv <  CFG_VOLTAGE_CAN_MIN_MV)    { return VOLT_STATE_UNDER;  }
    if (mv <  CFG_VOLTAGE_NORMAL_MIN_MV) { return VOLT_STATE_LOW;    }
    return VOLT_STATE_NORMAL;
}

static VoltState_t volt_next_state(VoltState_t cur, u16 mv)
{
    const u16 h = (u16)CFG_VOLTAGE_TOLERANCE_MV;

    switch (cur) {
        case VOLT_STATE_OVER:
            /* 过压恢复：必须降到 18V - 0.2V 以下 */
            if (mv <= (u16)(CFG_VOLTAGE_NORMAL_MAX_MV - h)) {
                return volt_classify_fresh(mv);
            }
            return VOLT_STATE_OVER;

        case VOLT_STATE_UNDER:
            /* 欠压恢复：必须升到 6.5V + 0.2V 以上 */
            if (mv >= (u16)(CFG_VOLTAGE_CAN_MIN_MV + h)) {
                return volt_classify_fresh(mv);
            }
            return VOLT_STATE_UNDER;

        case VOLT_STATE_LOW:
            if (mv <  CFG_VOLTAGE_CAN_MIN_MV)    { return VOLT_STATE_UNDER; }
            if (mv >  CFG_VOLTAGE_NORMAL_MAX_MV) { return VOLT_STATE_OVER;  }
            /* 升入正常：必须超过 9V + 0.2V */
            if (mv >= (u16)(CFG_VOLTAGE_NORMAL_MIN_MV + h)) {
                return VOLT_STATE_NORMAL;
            }
            return VOLT_STATE_LOW;

        case VOLT_STATE_NORMAL:
        default:
            return volt_classify_fresh(mv);
    }
}

/*===========================================================================
 *  初始化
 *===========================================================================*/
void VOLT_Init(ClusterSystem_t *sys)
{
    s_filt_mv    = 0u;
    s_filt_init  = false;
    s_cand_state = VOLT_STATE_INIT;
    s_cand_ms    = 0u;

    sys->voltage.vbat_mv     = 0u;
    sys->voltage.vbat_raw_mv = 0u;
    sys->voltage.state       = VOLT_STATE_INIT;
    sys->voltage.valid       = false;
    /* can_ok 未测出电压前按"可用"处理，避免上电瞬间把 CAN 判成不可通信；
     * 该字段的所有读取点（can_app.c）都同时判了 voltage.valid，是安全的。 */
    sys->voltage.can_ok      = true;
    /* 【修复】normal 不能沿用 can_ok 那套"乐观默认"。normal 语义是
     * "已确认电压在 9~18V"，backlight.c 直接拿它做背光开关判断、没有
     * 再判 valid，默认 true 会让"还没测出电压"被误当成"电压正常"。
     * 这里改成 false，和 valid=false 语义对齐：没测出来之前一律
     * 按"未确认正常"处理，交给下面 VOLT_Task() 首次有效采样后再判定。
     * pwr_mgr.c 的供电门限对 VOLT_STATE_INIT 有专门的旁路分支，不受
     * 这里改动影响，仍然维持上电乐观默认。 */
    sys->voltage.normal      = false;
}

/*===========================================================================
 *  周期任务
 *===========================================================================*/
void VOLT_Task(ClusterSystem_t *sys)
{
    PowerVoltage_t *v = &sys->voltage;
    u16 raw_mv;
    u16 filt_mv;
    VoltState_t next;

    /* --- 1) 取样。ADC 未就绪时不做状态判定，但 valid/normal 必须
     *        同步置为"未确认"，不能停留在旧值（尤其是上电默认值）--- */
    if (!HAL_ADC_IsReady()) {
        v->valid  = false;
        /* 【修复】原来这里只改 valid，没有同步改 normal。VOLT_Init() 把
         * normal 预置为 true（专门是为了 can_ok / 供电门限在没测出电压
         * 前保持乐观默认，避免上电瞬间误断 SOC/收音机——这个意图只写在
         * can_ok 上，注释被错误地也套用到了 normal 上）。backlight.c
         * 直接读 normal 决定背光通断，于是 ADC 没 ready 的这段窗口
         * （对应 IAR 断点看到的 s_ready==0）会被误当成"电压正常"，
         * 该关的背光没关。这里强制同步为 false，从数据源头保证
         * valid==false 时 normal 不可能是 true。 */
        v->normal = false;
        return;
    }

    raw_mv = HAL_ADC_ReadVbatMv();
    v->vbat_raw_mv = raw_mv;

    /* --- 2) 一阶低通：filt += (new - filt) >> SHIFT --- */
    if (!s_filt_init) {
        s_filt_mv   = (u32)raw_mv * FILT_SCALE;   /* 首次直接置位，避免从 0 慢爬 */
        s_filt_init = true;
    } else {
        s_filt_mv = s_filt_mv - (s_filt_mv >> CFG_VOLTAGE_FILTER_SHIFT)
                              + (u32)raw_mv;
    }
    filt_mv    = (u16)(s_filt_mv / FILT_SCALE);
    v->vbat_mv = filt_mv;
    v->valid   = true;

    /* --- 3) 状态判定：迟滞 + 确认时间 --- */
    next = volt_next_state((v->state == VOLT_STATE_INIT)
                               ? volt_classify_fresh(filt_mv)
                               : v->state,
                           filt_mv);

    if (v->state == VOLT_STATE_INIT) {
        /* 首次判定不等确认时间，立即建立初始状态 */
        v->state     = next;
        s_cand_state = next;
        s_cand_ms    = sys->system_tick_ms;
    } else if (next != v->state) {
        if (next != s_cand_state) {
            s_cand_state = next;                       /* 候选变了，重新计时 */
            s_cand_ms    = sys->system_tick_ms;
        } else if ((u32)(sys->system_tick_ms - s_cand_ms) >= CFG_VOLTAGE_DEBOUNCE_MS) {
            v->state = next;                           /* 维持够久，正式切换 */
        } else {
            /* 确认中，暂不切换 */
        }
    } else {
        s_cand_state = v->state;                       /* 未偏离，清候选 */
        s_cand_ms    = sys->system_tick_ms;
    }

    /* --- 4) 派生标志 --- */
    v->can_ok = (v->state == VOLT_STATE_LOW) || (v->state == VOLT_STATE_NORMAL);
    v->normal = (v->state == VOLT_STATE_NORMAL);
}

/*===========================================================================
 *  查询接口
 *===========================================================================*/
u16  VOLT_GetMv(const ClusterSystem_t *sys)       { return sys->voltage.vbat_mv; }
bool VOLT_IsCanUsable(const ClusterSystem_t *sys) { return sys->voltage.can_ok;  }
bool VOLT_IsNormal(const ClusterSystem_t *sys)    { return sys->voltage.normal;  }