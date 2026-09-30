/**
 * @file    cng.c
 * @brief   CNG（天然气）显示模块实现
 *
 * 百分比 → 格数：
 *   CAN 原始百分比 raw（0x305 byte1，0~100，直接就是电磁力表百分比）
 *   直接查 9 段表得到格数，表中范围与需求文档 CNG 表完全一致。
 *
 * 【本次修正】原实现在查表前多做了一步 pct = raw × 0.4 的折算，
 *   与需求表（0-13/15-20/…/89-98 对应 F1~F9）对不上：例如 raw=50%
 *   本应落在 44-53 区间显示 F5，乘 0.4 后变成 pct=20，被错误地判成
 *   F2（15-20 区间），气量表整体"缩水"到只剩表面读数的四成左右。
 *   已删除该折算，raw 直接参与查表。
 */

#include "cng.h"

/*---------------------------------------------------------------------------
 *  百分比 → 格数 查找表
 *
 *  约定：进入第 i 格的条件是 min_pct ≤ pct ≤ max_pct
 *---------------------------------------------------------------------------*/
static const struct { u8 min_pct; u8 max_pct; u8 seg; } s_cng_tbl[] = {
    {  0u, 13u, 1u }, { 15u, 20u, 2u }, { 22u, 31u, 3u },
    { 33u, 42u, 4u }, { 44u, 53u, 5u }, { 55u, 64u, 6u },
    { 66u, 76u, 7u }, { 78u, 87u, 8u }, { 89u, 98u, 9u },
};
#define CNG_TBL_SIZE (sizeof(s_cng_tbl) / sizeof(s_cng_tbl[0]))

/*---------------------------------------------------------------------------
 *  内部状态
 *---------------------------------------------------------------------------*/
static u32  s_cng_detect_timer_ms = 0u;     /**< 上电检测计时（ms）   */
static bool s_cng_mode_locked     = false;  /**< 上电模式已锁定标志    */

/*===========================================================================
 *  初始化
 *===========================================================================*/
void CNG_Init(ClusterSystem_t *sys)
{
    (void)sys;
    s_cng_detect_timer_ms = 0u;
    s_cng_mode_locked     = false;
}

/*---------------------------------------------------------------------------
 *  内部：百分比 → 格数
 *---------------------------------------------------------------------------*/
static u8 CNG_PercentToSegments(u8 raw)
{
    /* 需求：若当前报文（气量值）为 0，气量表显示 1 格闪烁 */
    if (raw == 0u) { return 1u; }

    for (u8 i = 0; i < CNG_TBL_SIZE; i++) {
        if (raw >= s_cng_tbl[i].min_pct && raw <= s_cng_tbl[i].max_pct) {
            return s_cng_tbl[i].seg;
        }
    }
    /* 落在表格间隙（如 14%、21%……回差区间）：就近取较低一格，
     * 避免虚高显示；理论上传感器不会稳定停在间隙里。 */
    if (raw < s_cng_tbl[0].min_pct) { return 1u; }
    for (u8 i = CNG_TBL_SIZE; i > 0u; i--) {
        if (raw > s_cng_tbl[i - 1u].max_pct) { return s_cng_tbl[i - 1u].seg; }
    }
    return 9u;
}

/*===========================================================================
 *  周期任务
 *===========================================================================*/
void CNG_Task(ClusterSystem_t *sys)
{
    /* 自检阶段：仅做模式预探测，不修改 fuel.segments */
    if (!sys->selfcheck_done) {
        s_cng_detect_timer_ms += 20u;
        if (sys->can_rx.cng.cng_mode == 1u) {
            sys->fuel_mode    = FUEL_MODE_CNG;
            s_cng_mode_locked = true;
            (void)s_cng_mode_locked;
        }
        return;
    }

    const CanCNG_t *cng  = &sys->can_rx.cng;
    FuelLcdData_t  *fuel = &sys->lcd.fuel;

    /* CAN 超时：保留上一帧显示，不更新 */
    if (cng->timeout) {
        return;
    }

    /* --- 1) 模式同步 --- */
    switch (cng->cng_mode) {
        case 0u: sys->fuel_mode = FUEL_MODE_GASOLINE;   break;
        case 1u: sys->fuel_mode = FUEL_MODE_CNG;        break;
        case 2u: sys->fuel_mode = FUEL_MODE_SWITCHING;  break;
        default:
            /* 保持当前模式不变 */
            break;
    }

    fuel->mode = sys->fuel_mode;

    /* --- 2) 根据模式刷新 fuel 子结构 --- */
    if (sys->fuel_mode == FUEL_MODE_CNG) {
        u8 seg              = CNG_PercentToSegments(cng->cng_fuel_percent);
        fuel->segments      = seg;
        fuel->blink         = (seg <= CFG_FUEL_WARN_SEGMENT);
        fuel->cng_switching = false;
        /* CNG 模式不受燃油故障影响：即使 fuel.c 曾经（例如任务顺序改动后）
         * 置位过 fault_blink，这里也要在 CNG 显示生效前清掉，防止整排
         * CNG 格柱被燃油开路/短路的 1Hz 闪烁覆盖。 */
        fuel->fault_blink   = false;
    } else if (sys->fuel_mode == FUEL_MODE_SWITCHING) {
        /* 切换中（cng_mode=2）：CNG 图标闪烁。
         * 【本次修正】需求原文："CNG 模式=2 的时候，段码显示根据报文值来指示"，
         * 即格数仍须实时跟随 0x305 的百分比刷新，不能冻结在切换前的旧值——
         * 原实现只置 cng_switching=true 就直接 return，格数一直停留在进入
         * 切换前的最后一次数值，与需求不符，这里改为和 CNG 模式一样实时算格。 */
        u8 seg              = CNG_PercentToSegments(cng->cng_fuel_percent);
        fuel->segments      = seg;
        fuel->blink         = (seg <= CFG_FUEL_WARN_SEGMENT);
        fuel->cng_switching = true;
        fuel->fault_blink   = false;   /* 切换中同样不受燃油故障影响 */
    } else {
        /* 汽油模式：清切换标志，格数由 fuel.c 维护 */
        fuel->cng_switching = false;
    }
}

/*===========================================================================
 *  显示值访问
 *===========================================================================*/
u8 CNG_GetSegments(const ClusterSystem_t *sys)
{
    if (sys->fuel_mode == FUEL_MODE_CNG) {
        return sys->lcd.fuel.segments;
    }
    return 0u;
}