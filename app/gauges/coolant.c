/**
 * @file    coolant.c
 * @brief   水温模块实现
 *
 * 9 段格数对应温度区间（按需求 8.4 表格，注意 F8/F9 是**下限闭、上限开**）：
 *   F1:  40<C≤50    F2:  50<C≤60    F3:  60<C≤72
 *   F4:  72<C≤100   F5: 100<C≤104   F6: 104<C≤108
 *   F7: 108<C<113   F8: 113≤C<120   F9: C≥120
 *   （C≤40 也按最小 1 格显示）
 *
 * 报警：报警点 113℃（= 进入 F8），报警解除点 110℃，中间为迟滞带。
 *
 * ===========================================================================
 * 本次修改
 * ===========================================================================
 *   1) 水温数据源改为 0x360（整车矩阵：0x360 byte1，精度 0.75，偏移 -48），
 *      超时判据相应改为 ems->timeout_360。
 *      原实现从 0x306 取水温，而整车并不在该帧发送水温，导致
 *      coolant_temp_raw 恒为初值 0 → -48℃ → 段柱恒显示 1 格。
 *   2) 丢失计时改用系统时间戳，不再依赖"任务周期 = 累加步长"的隐含约定。
 *   3) 新增输出 lcd->signal_lost：段柱压到 1 格与红色报警灯 R55 共用同一个
 *      判据，避免"CAN 抖一下红灯就闪、20 秒后格柱才掉"的时序不一致。
 */

#include "coolant.h"

/*---------------------------------------------------------------------------
 *  内部状态
 *---------------------------------------------------------------------------*/
static bool s_lost_timing   = false;    /**< 正在累计信号丢失时间   */
static u32  s_lost_start_ms = 0u;       /**< 信号丢失的起始时刻(ms) */

/*===========================================================================
 *  初始化
 *===========================================================================*/
void COOLANT_Init(ClusterSystem_t *sys)
{
    s_lost_timing               = false;
    s_lost_start_ms             = 0u;
    sys->lcd.coolant.segments   = 1u;
    sys->lcd.coolant.blink_high = false;
    sys->lcd.coolant.signal_lost = false;
    sys->lcd.coolant.high_temp_warn = false;
}

/*===========================================================================
 *  公开换算函数
 *===========================================================================*/
s16 COOLANT_RawToTemp(u8 raw)
{
    /* temp = raw × 0.75 - 48，**四舍五入**取整（不是截断）。
     *
     * ⚠ 关键点：0.75 的步长使得多数整数℃ 无法被精确编码，
     *   例如 113℃ → raw = (113+48)/0.75 = 214.67。上位机发 113 时
     *   实际下发 raw 214（真实 112.5℃）。
     *   若按 raw*75/100 截断，算回来是 112 → 判到 F7 → 报警点测不出来。
     *   改为 (raw*75 + 50)/100 后，raw 214 → 113，与上位机发的值一致，
     *   109/113 等所有整数℃ 边界都能正确还原。 */
    s32 t = ((s32)raw * 75 + 50) / 100 - 48;
    return (s16)t;
}

/*---------------------------------------------------------------------------
 *  内部辅助：℃ → 格数
 *---------------------------------------------------------------------------*/
static u8 COOLANT_TempToSegments(s16 temp_c)
{
    /* 自高向低比较，边界含义与需求 8.4 表格逐格对应。
     * 注意 F8/F9 的下边界是**闭区间**（113≤C、C≥120），
     * 与 F1~F7 的「上限闭」写法不同——原查找表统一用
     * `min < C ≤ max`，导致 C=113 落到 F7、C=120 落到 F8，各差一格。 */
    if (temp_c >= 120)                      { return 9u; }  /* F9: C≥120      */
    if (temp_c >= CFG_COOLANT_TEMP_WARN_C)  { return 8u; }  /* F8: 113≤C<120  */
    if (temp_c >  108)                      { return 7u; }  /* F7: 108<C<113  */
    if (temp_c >  104)                      { return 6u; }  /* F6: 104<C≤108  */
    if (temp_c >  100)                      { return 5u; }  /* F5: 100<C≤104  */
    if (temp_c >   72)                      { return 4u; }  /* F4:  72<C≤100  */
    if (temp_c >   60)                      { return 3u; }  /* F3:  60<C≤72   */
    if (temp_c >   50)                      { return 2u; }  /* F2:  50<C≤60   */
    return 1u;                                              /* F1: C≤50（含<40）*/
}

/*===========================================================================
 *  周期任务（20ms 调用）
 *===========================================================================*/
void COOLANT_Task(ClusterSystem_t *sys)
{
    CanEMS_t         *ems = &sys->can_rx.ems;
    CoolantLcdData_t *lcd = &sys->lcd.coolant;

    /* 水温来自 0x360。矩阵中没有水温故障位，异常统一由本帧超时表达。 */
    bool sig_bad = ems->timeout_360;

    /* --- 1) 信号丢失或故障：持续 CFG_COOLANT_TIMEOUT_S 后强制 1 格 --- */
    if (sig_bad) {
        if (!s_lost_timing) {
            s_lost_timing   = true;
            s_lost_start_ms = sys->system_tick_ms;
        }
        if ((sys->system_tick_ms - s_lost_start_ms)
                >= ((u32)CFG_COOLANT_TIMEOUT_S * 1000u)) {
            lcd->signal_lost = true;
            lcd->segments    = 1u;
            /* 信号已确认丢失：温度不可信，解除高温报警锁存 */
            lcd->high_temp_warn = false;
        }
        /* 判定时间未到：保持上一次显示，不跳变 */
    } else {
        s_lost_timing    = false;
        lcd->signal_lost = false;

        /* --- 2) 正常路径：原始值 → ℃ → 格数 --- */
        s16 temp = COOLANT_RawToTemp(ems->coolant_temp_raw);
        u8  seg  = COOLANT_TempToSegments(temp);

        /* --- 2b) 高温报警锁存（迟滞） ---
         * 报警点 113℃，解除点 110℃，110<C<113 保持原状态。
         * 原实现按 segments >= CFG_COOLANT_WARN_SEGMENT 判断，旧表第 8 格
         * 下边界是"C>113"，等价于"114 进、113 出"，
         * CFG_COOLANT_TEMP_WARN_CLEAR_C(110) 全工程从未被引用。 */
        if (temp >= (s16)CFG_COOLANT_TEMP_WARN_C) {
            lcd->high_temp_warn = true;
        } else if (temp <= (s16)CFG_COOLANT_TEMP_WARN_CLEAR_C) {
            lcd->high_temp_warn = false;
        }
        /* else：迟滞带内，保持 */

        /* --- 2c) 迟滞带内段柱不回落 ---
         * 需求表把"报警解除点 110"写在 F7 格里，即报警一旦触发，
         * 段柱要保持 F8 直到降到 110℃ 才退回 F7；否则 111~112℃ 时
         * 段柱已是 7 格、第 8 格无从闪烁，"解除点 110"在段柱上无法体现。
         * ⚠ 若客户确认段柱应严格按温度表回落（111℃ 即回 F7、仅红灯保持
         *   到 110），删掉这三行即可，其余逻辑不受影响。 */
        if (lcd->high_temp_warn && (seg < CFG_COOLANT_WARN_SEGMENT)) {
            seg = CFG_COOLANT_WARN_SEGMENT;
        }

        if (seg < 1u) { seg = 1u; }
        lcd->segments = seg;
    }

    /* --- 3) 高温闪烁标志 ---
     * 需求 8.4：显示段数 ≥8 时第 8 格 1Hz 闪烁，≥9 时第 8/9 格闪烁。
     * 闪烁按**段数**判定（迟滞带 110<C<113 内段柱只有 7 格，没有可闪的段，
     * 此时仅红灯 R55 保持常亮）。信号丢失时段柱压到 1 格，不闪。 */
    lcd->blink_high = (!lcd->signal_lost) &&
                      (lcd->segments >= CFG_COOLANT_WARN_SEGMENT);
}

/*===========================================================================
 *  显示值访问
 *===========================================================================*/
u8 COOLANT_GetSegments(const ClusterSystem_t *sys)
{
    return sys->lcd.coolant.segments;
}