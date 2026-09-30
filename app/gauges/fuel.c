/**
 * @file  fuel.c
 * @brief 燃油模块（V4）
 *
 * V3 → V4 变更清单:
 *   BUG-D  故障判定期间（CFG_FUEL_FAULT_HOLD_MS = 20s）s_state 仍为 NORMAL，
 *          越界原始值 0xFFFF（开路）/ 0（短路）会一路喂进 IIR 滤波器与加油
 *          检测：滤波值 20 秒内可从 54Ω 爬到约 60000Ω，故障解除后油量段码
 *          要从 0 格逐格爬回，表现为"虚惊一场后油表空掉几十秒"。
 *          → 喂入前先夹到 [CFG_FUEL_SHORT_OHMS, CFG_FUEL_OPEN_OHMS]
 *   BUG-E  故障态未复位滤波器，恢复后从故障前旧值缓慢收敛
 *          → 进入故障态时置 s_filter_init = false，恢复后首次采样直接置位
 *
 * V2 → V3 变更清单:
 *   BUG-A  熄灭阈值表 k_thr.on_thr 整列填错（误用上一格点亮值）
 *          → 照规格表（45L 油箱）严格填入"即将熄灭 >X"值
 *   BUG-B  降格门限方向与目标计算不一致，滤波值落死区时永久卡在
 *          目标上方一格 → 段码目标改为"带迟滞的单一 target"驱动，
 *          步进时不再叠加反向门限
 *   BUG-C  "target 变化即清零计时器"造成边界抖动饥饿（±1Ω 抖动使
 *          2s 门限永远凑不满）→ 引入 target 锁存，仅在锁存目标真正
 *          变化时才复位步进计时器
 *
 * 段码迟滞语义（与规格表 8.1 一致）:
 *   - 第 i 格点亮:   电阻 ≤ off_thr[i]（油多→电阻低）
 *   - 第 i 格熄灭:   电阻 ≥ on_thr[i] （油少→电阻高）
 *   - off_thr < on_thr，二者之间为迟滞死区，维持当前显示
 */

#include "fuel.h"

/* ══════════════════════════════════════════════════════════════════════
 *  配置宏
 * ══════════════════════════════════════════════════════════════════════ */
#define FUEL_FAULT_PCT_MARKER     (0x7Fu)
#define FUEL_TASK_PERIOD_MS       (20u)
#define SEG_CHANGE_INTERVAL_MS    (2000u)   /* 每 2s 最多变化 1 格 */
#define SEG_TARGET_LATCH_MS       (200u)    /* 目标需稳定 200ms 才被采纳，抑制边界抖动 */
#define PCT_CHANGE_INTERVAL_MS    (500u)    /* 每 500ms 允许百分比变化 ±1% → 等效 2%/s */

#define REFUEL_WINDOW_MS          (5000u)   /* 加油检测滑动窗口 5 秒 */
#define REFUEL_DELTA_OHMS         (20u)     /* 窗口内下降超过此值视为加油 */

#define FILTER_TAU_NORMAL_MS      (8000u)
#define FILTER_TAU_STATIC_MS      (30000u)
#define FILTER_TAU_REFUEL_MS      (500u)

#define OHMS_OFFSET_HIGH          (2u)
#define OHMS_OFFSET_LOW           (1u)

/* ══════════════════════════════════════════════════════════════════════
 *  [BUG-A] 段码阈值表 — 严格照规格表（45L 油箱）填写
 *
 *  规格表（检测点 → 段码 F1..F9）:
 *    格   止亮(点亮 ≤)   即将熄灭(熄灭 ≥)   检测点
 *    F1     <243            >253            E
 *    F2     <228            >236
 *    F3     <205            >210            1/4
 *    F4     <180            >185
 *    F5     <163            >167            1/2
 *    F6     <147            >150
 *    F7     <125            >135            3/4
 *    F8     <105            >110
 *    F9     < 54            > 61            F
 *
 *  索引 0..8 对应段位 1..9。
 * ══════════════════════════════════════════════════════════════════════ */
typedef struct {
    u16 off_thr;   /* 段码点亮阈值：电阻 ≤ 此值 → 该格应亮 */
    u16 on_thr;    /* 段码熄灭阈值：电阻 ≥ 此值 → 该格应灭 */
} FuelSegThr_t;

static const FuelSegThr_t k_thr[9u] = {
    {243u, 253u}, {228u, 236u}, {205u, 210u}, {180u, 185u}, {163u, 167u},
    {147u, 150u}, {125u, 135u}, {105u, 110u}, { 54u,  61u},
};

/* 百分比查找表（同 V2，无变更；已核查无空洞/重叠，油量方向单调） */
static const struct { u16 r_min; u16 r_max; u8 pct; } k_pct_tbl[] = {
    {260u,360u, 9u},{253u,260u,11u},{243u,253u,13u},{240u,243u,16u},
    {236u,240u,18u},{228u,236u,20u},{224u,228u,22u},{220u,224u,24u},
    {215u,220u,27u},{210u,215u,29u},{205u,210u,31u},{200u,205u,33u},
    {195u,200u,36u},{190u,195u,38u},{185u,190u,40u},{180u,185u,42u},
    {176u,180u,44u},{173u,176u,47u},{170u,173u,49u},{166u,170u,51u},
    {163u,166u,53u},{160u,163u,56u},{157u,160u,58u},{153u,157u,60u},
    {150u,153u,62u},{146u,150u,64u},{140u,146u,69u},{135u,140u,73u},
    {125u,135u,76u},{120u,125u,80u},{115u,120u,82u},{110u,115u,84u},
    {105u,110u,87u},{100u,105u,89u},{ 90u,100u,91u},{ 70u, 90u,93u},
    { 60u, 70u,96u},{ 52u, 60u,98u},{ 10u, 52u,100u},
};
#define PCT_TBL_SIZE  ((u8)(sizeof(k_pct_tbl) / sizeof(k_pct_tbl[0])))

/* ══════════════════════════════════════════════════════════════════════
 *  模块状态
 * ══════════════════════════════════════════════════════════════════════ */
typedef enum { FUEL_NORMAL=0, FUEL_OPEN=1, FUEL_SHORT=2 } SensorState_t;

static SensorState_t s_state          = FUEL_NORMAL;
static u32           s_fault_ms        = 0u;
static u8            s_segments        = 9u;
static u8            s_percent         = 100u;

/* IIR 滤波器 — 定点 ×10000 */
static u32           s_filtered_x10k   = 540000u;  /* 54.0000Ω × 10000 */
static bool          s_filter_init     = false;

/* 段码控制 — [BUG-B][BUG-C] target 锁存 + 步进计时 */
static u8            s_seg_target      = 9u;   /* 带迟滞的瞬时目标 */
static u8            s_seg_target_cand = 9u;   /* 候选目标（待稳定采纳） */
static u32           s_seg_cand_ms     = 0u;   /* 候选目标已稳定时长 */
static u32           s_seg_hold_ms     = 0u;   /* 距上次实际变格的时长 */

/* 百分比阻尼 — 时间累积机制 */
static u32           s_pct_accum_ms    = 0u;

/* 加油检测 — 滑动窗口环形缓冲 */
static u16           s_refuel_ring[8u];
static u8            s_refuel_idx       = 0u;
static u32           s_refuel_tick      = 0u;
#define REFUEL_SAMPLE_MS     (REFUEL_WINDOW_MS / 8u)   /* 5000/8 = 625ms 采样一次 */

/* ══════════════════════════════════════════════════════════════════════
 *  初始化
 * ══════════════════════════════════════════════════════════════════════ */
void FUEL_Init(ClusterSystem_t *sys)
{
    s_state           = FUEL_NORMAL;
    s_fault_ms        = 0u;
    s_segments        = 9u;
    s_percent         = 100u;
    s_filtered_x10k   = 540000u;
    s_filter_init     = false;
    s_seg_target      = 9u;
    s_seg_target_cand = 9u;
    s_seg_cand_ms     = 0u;
    s_seg_hold_ms     = 0u;
    s_pct_accum_ms    = 0u;
    s_refuel_idx      = 0u;
    s_refuel_tick     = 0u;

    for (u8 i = 0u; i < 8u; i++) {
        s_refuel_ring[i] = 54u;  /* 初始满油电阻 */
    }

    sys->fuel_mode              = FUEL_MODE_GASOLINE;
    sys->lcd.fuel.segments      = 9u;
    sys->lcd.fuel.blink         = false;
    sys->lcd.fuel.fault_blink   = false;
    sys->lcd.fuel.cng_switching = false;
    sys->lcd.fuel.mode          = FUEL_MODE_GASOLINE;
}

/* ══════════════════════════════════════════════════════════════════════
 *  IIR 滤波器 — 定点 ×10000
 *
 *  u32 范围验证（入参已由调用方夹到 ≤ CFG_FUEL_OPEN_OHMS = 360）：
 *      360 × 10000 = 3,600,000 << 4,294,967,295  ✓
 *
 *  注：V3 的注释按"最大 360Ω"论证，但当时 0xFFFF 确实会传进来
 *      （65535 × 10000 = 655,350,000，虽仍不溢出，前提是错的）。
 *      V4 起由 FUEL_Task 统一夹紧，该前提才真正成立。
 * ══════════════════════════════════════════════════════════════════════ */
static u16 iir_filter(u16 raw_ohms, u32 dt_ms, u32 tau_ms)
{
    if (!s_filter_init) {
        s_filtered_x10k = (u32)raw_ohms * 10000u;
        s_filter_init   = true;
        return raw_ohms;
    }

    u32 raw_x10k = (u32)raw_ohms * 10000u;
    u32 denom    = tau_ms + dt_ms;

    if (denom == 0u) {
        return (u16)(s_filtered_x10k / 10000u);
    }

    if (raw_x10k >= s_filtered_x10k) {
        u32 diff  = raw_x10k - s_filtered_x10k;
        u32 delta = diff / denom * dt_ms;           /* 先除后乘，防溢出 */
        u32 rem   = diff % denom * dt_ms / denom;   /* 余数补偿 */
        s_filtered_x10k += delta + rem;
    } else {
        u32 diff  = s_filtered_x10k - raw_x10k;
        u32 delta = diff / denom * dt_ms;
        u32 rem   = diff % denom * dt_ms / denom;
        s_filtered_x10k -= (delta + rem);
    }

    return (u16)(s_filtered_x10k / 10000u);
}

/* ══════════════════════════════════════════════════════════════════════
 *  [BUG-B] 带迟滞的瞬时目标计算
 *
 *  以"当前显示格数 cur"为基准，按每格独立迟滞判断增减：
 *    - 顶部已点亮格若 电阻 ≥ 其 on_thr  → 熄灭（目标 -1），继续向下检查
 *    - 下一未点亮格若 电阻 ≤ 其 off_thr → 点亮（目标 +1），继续向上检查
 *  off_thr < on_thr，落在死区内则维持 cur 不变。单调阈值保证两个
 *  while 不会同时推进，收敛且无振荡。
 * ══════════════════════════════════════════════════════════════════════ */
static u8 seg_hysteretic_target(u16 ohms, u8 cur)
{
    u8 t = cur;

    /* 油量下降（电阻升高）：自顶向下熄灭 */
    while (t > 0u && ohms >= k_thr[t - 1u].on_thr) {
        t--;
    }
    /* 油量上升（电阻降低）：自下向上点亮 */
    while (t < 9u && ohms <= k_thr[t].off_thr) {
        t++;
    }

    return t;
}

/* ══════════════════════════════════════════════════════════════════════
 *  [BUG-B][BUG-C] 段码更新
 *
 *  1) 计算带迟滞的瞬时目标；
 *  2) 目标锁存：候选目标需连续稳定 SEG_TARGET_LATCH_MS 才被采纳，
 *     避免滤波值卡在边界 ±1Ω 抖动时反复复位步进计时器（BUG-C）；
 *  3) 向锁存目标步进，每 SEG_CHANGE_INTERVAL_MS 最多 1 格，
 *     不再叠加反向门限，消除死区卡格（BUG-B）。
 * ══════════════════════════════════════════════════════════════════════ */
static u8 ohms_to_seg(u16 ohms, u32 dt_ms)
{
    /* 1. 瞬时目标（带迟滞） */
    u8 tgt = seg_hysteretic_target(ohms, s_segments);

    /* 2. 目标锁存：抑制边界抖动 */
    if (tgt == s_seg_target_cand) {
        s_seg_cand_ms += dt_ms;
    } else {
        s_seg_target_cand = tgt;
        s_seg_cand_ms     = 0u;
    }

    if (s_seg_cand_ms >= SEG_TARGET_LATCH_MS) {
        if (s_seg_target != s_seg_target_cand) {
            s_seg_target  = s_seg_target_cand;
            s_seg_hold_ms = 0u;   /* 仅在锁存目标真正变化时复位步进计时器 */
        }
    }

    /* 3. 已到位 → 保持 */
    if (s_seg_target == s_segments) {
        return s_segments;
    }

    /* 时间锁：距上次变格不足间隔 → 保持 */
    s_seg_hold_ms += dt_ms;
    if (s_seg_hold_ms < SEG_CHANGE_INTERVAL_MS) {
        return s_segments;
    }
    s_seg_hold_ms = 0u;

    /* 向目标步进 1 格 */
    if (s_seg_target < s_segments) {
        s_segments--;
    } else {
        s_segments++;
    }
    return s_segments;
}

/* ══════════════════════════════════════════════════════════════════════
 *  百分比阻尼 — 时间累积机制（每 500ms ±1%）
 * ══════════════════════════════════════════════════════════════════════ */
static u8 ohms_to_pct_raw(u16 ohms_filtered)
{
    u16 ohms = (ohms_filtered >= 200u)
             ? (ohms_filtered + OHMS_OFFSET_HIGH)
             : (ohms_filtered + OHMS_OFFSET_LOW);

    if (ohms >= 360u) { return 9u;   }
    if (ohms <  10u)  { return 100u; }

    for (u8 i = 0u; i < PCT_TBL_SIZE; i++) {
        if (ohms >= k_pct_tbl[i].r_min && ohms < k_pct_tbl[i].r_max) {
            return k_pct_tbl[i].pct;
        }
    }
    return 0u;
}

static u8 ohms_to_pct(u16 ohms_filtered, u32 dt_ms)
{
    u8 raw_pct = ohms_to_pct_raw(ohms_filtered);

    if (raw_pct == s_percent) {
        s_pct_accum_ms = 0u;
        return s_percent;
    }

    s_pct_accum_ms += dt_ms;

    if (s_pct_accum_ms >= PCT_CHANGE_INTERVAL_MS) {
        u32 steps = s_pct_accum_ms / PCT_CHANGE_INTERVAL_MS;
        s_pct_accum_ms %= PCT_CHANGE_INTERVAL_MS;

        if (raw_pct > s_percent) {
            u8 diff = raw_pct - s_percent;
            u8 inc  = (steps > (u32)diff) ? diff : (u8)steps;
            s_percent += inc;
        } else {
            u8 diff = s_percent - raw_pct;
            u8 dec  = (steps > (u32)diff) ? diff : (u8)steps;
            s_percent -= dec;
        }
    }

    return s_percent;
}

/* ══════════════════════════════════════════════════════════════════════
 *  传感器故障诊断（规格 25~29 条）
 * ══════════════════════════════════════════════════════════════════════ */
static SensorState_t check_sensor(u16 ohms, u32 elapsed_ms)
{
    SensorState_t instant;

    if      (ohms > CFG_FUEL_OPEN_OHMS)  { instant = FUEL_OPEN;   }
    else if (ohms < CFG_FUEL_SHORT_OHMS) { instant = FUEL_SHORT;  }
    else                                 { instant = FUEL_NORMAL;  }

    if (instant == s_state) {
        s_fault_ms = 0u;
        return s_state;
    }

    s_fault_ms += elapsed_ms;

    if (s_state == FUEL_NORMAL) {
        if (s_fault_ms >= CFG_FUEL_FAULT_HOLD_MS) {
            s_state    = instant;
            s_fault_ms = 0u;
        }
    } else {
        if (instant == FUEL_NORMAL && s_fault_ms >= CFG_FUEL_RECOVERY_MS) {
            s_state    = FUEL_NORMAL;
            s_fault_ms = 0u;
        }
    }

    return s_state;
}

/* ══════════════════════════════════════════════════════════════════════
 *  加油检测 — 滑动窗口
 *
 *  每 625ms 采样一次原始电阻，保留最近 8 个样本（覆盖 5 秒）。
 *  若最老样本与当前值之差超过 REFUEL_DELTA_OHMS，判定加油中。
 * ══════════════════════════════════════════════════════════════════════ */
static bool is_refueling(u16 raw_ohms, u32 dt_ms)
{
    s_refuel_tick += dt_ms;

    if (s_refuel_tick >= REFUEL_SAMPLE_MS) {
        s_refuel_tick -= REFUEL_SAMPLE_MS;
        s_refuel_ring[s_refuel_idx] = raw_ohms;
        s_refuel_idx = (s_refuel_idx + 1u) % 8u;
    }

    /* 环形缓冲区中 idx 指向的下一个即为最老样本 */
    u16 oldest = s_refuel_ring[s_refuel_idx];

    if (oldest > raw_ohms) {
        u16 drop = (u16)(oldest - raw_ohms);
        if (drop >= REFUEL_DELTA_OHMS) {
            return true;
        }
    }
    return false;
}

/* ══════════════════════════════════════════════════════════════════════
 *  滤波时间常数选择
 * ══════════════════════════════════════════════════════════════════════ */
static u32 get_filter_tau(const ClusterSystem_t *sys, bool refueling)
{
    if (refueling) {
        return FILTER_TAU_REFUEL_MS;
    }

    /* 若系统提供车速信号:
     * if (sys->hw_input.vehicle_speed == 0u) {
     *     return FILTER_TAU_STATIC_MS;
     * }
     */
    (void)sys;
    (void)FILTER_TAU_STATIC_MS;
    return FILTER_TAU_NORMAL_MS;
}

/* ══════════════════════════════════════════════════════════════════════
 *  主任务（20ms 周期）
 * ══════════════════════════════════════════════════════════════════════ */
void FUEL_Task(ClusterSystem_t *sys)
{
    if (!sys->selfcheck_done) {
        return;
    }

    const u32 dt_ms    = FUEL_TASK_PERIOD_MS;
    const u16 raw_ohms = sys->hw_input.fuel_ohms;

    /* 1. 故障诊断 */
    check_sensor(raw_ohms, dt_ms);

    FuelLcdData_t *fuel = &sys->lcd.fuel;

    /* 【本次修正】CNG 模式不受燃油故障（开路/短路等）影响：
     *   需求要求 CNG 模式下屏幕显示的是 CNG 格数/图标，燃油传感器仅在
     *   后台继续采集、按规格表 30 条做故障判定，并通过 CAN 把燃油信息
     *   外发（FUEL_GetPercent/FUEL_GetSensorFault 供 can_tx_build 使用），
     *   但不能把 sys->lcd.fuel.segments/blink/fault_blink 这些"燃油显示"
     *   字段写回去——它们与 CNG 显示共用同一块 FuelLcdData_t，一旦写入
     *   会被 lcd_ui.c 的 LCD_UpdateFuel() 当成当前显示状态直接渲染，
     *   CNG 格数会被开路 9 格闪烁"糊"掉，即使随后 CNG_Task 会重算
     *   segments/blink，fault_blink 也没人清，导致 CNG 模式下整排格柱
     *   跟着燃油故障一起 1Hz 闪烁。 */
    /* fuel.c 只在纯燃油模式下拥有显示所有权；CNG 模式和切换中（cng_mode=2）
     * 都由 cng.c 的 CNG_Task 驱动 segments/blink/fault_blink，即使这里先跑，
     * CNG_Task 随后也会用报文实时值覆盖——但仍应在源头上就不写故障态，
     * 避免任何时序假设、也让本文件的行为本身就是"不受影响"。 */
    bool update_display = (sys->fuel_mode == FUEL_MODE_GASOLINE);

    if (s_state != FUEL_NORMAL) {
        /* 规格 30 条：开路/短路 9 格 1Hz 闪烁（仅在燃油模式下显示） */
        if (update_display) {
            fuel->segments    = 9u;
            fuel->blink       = false;
            fuel->fault_blink = true;
        }
        s_segments        = 9u;
        s_percent         = FUEL_FAULT_PCT_MARKER;

        /* 故障中重置段码目标状态，恢复后从满格重新收敛，避免残留 */
        s_seg_target      = 9u;
        s_seg_target_cand = 9u;
        s_seg_cand_ms     = 0u;
        s_seg_hold_ms     = 0u;

        /* [BUG-E] 复位滤波器：恢复后首次采样直接置位，
         * 不再从故障前的旧值用 8 秒时间常数慢慢收敛。 */
        s_filter_init     = false;
        return;
    }

    if (update_display) {
        fuel->fault_blink = false;
    }

    /* [BUG-D] 越界值不得进入滤波器与加油检测。
     *
     * check_sensor() 需要 CFG_FUEL_FAULT_HOLD_MS（20 秒）才会切到故障态，
     * 这 20 秒里 s_state 仍是 NORMAL，代码会一路执行到下面。若把 0xFFFF
     * 直接喂进去，滤波值会爬到数万欧，且环形缓冲里的巨大跌落还会被误判成
     * "正在加油"。这里先夹到合法量程，故障与否交给 check_sensor() 判定。 */
    u16 valid_ohms = raw_ohms;
    if (valid_ohms > CFG_FUEL_OPEN_OHMS)  { valid_ohms = (u16)CFG_FUEL_OPEN_OHMS;  }
    if (valid_ohms < CFG_FUEL_SHORT_OHMS) { valid_ohms = (u16)CFG_FUEL_SHORT_OHMS; }

    /* 2. 加油检测（滑动窗口） */
    bool refueling = is_refueling(valid_ohms, dt_ms);

    /* 3. IIR 滤波 */
    u32 tau      = get_filter_tau(sys, refueling);
    u16 filtered = iir_filter(valid_ohms, dt_ms, tau);

    /* 4. 段码转换（内部状态 s_segments 始终更新，供故障解除后收敛用；
     *    仅在非 CNG 模式下才写回共享的显示结构体 fuel->segments/blink） */
    s_segments = ohms_to_seg(filtered, dt_ms);
    if (update_display) {
        fuel->segments = s_segments;
        fuel->blink    = (s_segments <= CFG_FUEL_WARN_SEGMENT);
    }

    /* 5. 百分比转换 */
    s_percent = ohms_to_pct(filtered, dt_ms);
}

/* ══════════════════════════════════════════════════════════════════════
 *  对外接口
 * ══════════════════════════════════════════════════════════════════════ */
u8 FUEL_GetSegments(const ClusterSystem_t *sys)
{
    return sys->lcd.fuel.segments;
}

u8 FUEL_GetPercent(const ClusterSystem_t *sys)
{
    (void)sys;
    return s_percent;
}

FuelSensorFault_t FUEL_GetSensorFault(void)
{
    switch (s_state) {
        case FUEL_OPEN:  return FUEL_SENSOR_OPEN;
        case FUEL_SHORT: return FUEL_SENSOR_SHORT;
        default:         return FUEL_SENSOR_NORMAL;
    }
}