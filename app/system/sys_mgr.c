/**
 * @file    sys_mgr.c
 * @brief   系统管理模块实现
 *
 * ===========================================================================
 * 本次修改（自检触发时机）
 * ===========================================================================
 * 需求：点火开关从 ACC 档转到 ON 档时，组合仪表进行指示灯自检；自检结束后
 *       系统正常则指示灯熄灭，收到故障信号才按信号点亮。
 *
 * 原实现把自检绑在 MCU 上电（Cluster_Init 里调一次 LED_SelfCheck，
 * SYS_Task 从 t=0 开始累加计时）。若 MCU 挂在蓄电池常电上，自检只会在接
 * 蓄电池那一刻发生一次，之后钥匙 ACC→ON 不会再自检。
 *
 * 现新增 SYS_StartSelfCheck()，由 PWR_Task 在进入 PWR_ON 的跳变上调用，
 * 每次点火 ON 都重新走一遍 1.5s 自检。
 */

#include "sys_mgr.h"

/*===========================================================================
 *  初始化
 *===========================================================================*/
void SYS_Init(ClusterSystem_t *sys)
{
    sys->system_tick_ms       = 0u;
    sys->blink_1hz_visible    = true;           /* 闪烁节拍初值 */
    sys->cluster_state        = CLUSTER_SLEEP;
    sys->fuel_mode            = FUEL_MODE_GASOLINE;
    sys->lcd.backlight_pct    = 100u;

    SYS_StartSelfCheck(sys);
}

/*===========================================================================
 *  启动一轮自检
 *
 *  幂等：重复调用只会把计时重新拉回起点。
 *  调用点：
 *    - SYS_Init()（MCU 上电）
 *    - PWR_Task() 进入 PWR_ON 的跳变（点火 ACC/OFF → ON）
 *===========================================================================*/
void SYS_StartSelfCheck(ClusterSystem_t *sys)
{
    sys->selfcheck_done     = false;
    sys->selfcheck_timer_ms = 0u;
    sys->selfcheck_start_ms = sys->system_tick_ms;
}

/*===========================================================================
 *  时基更新
 *===========================================================================*/
void SYS_UpdateTick(ClusterSystem_t *sys, u32 now_ms)
{
    sys->system_tick_ms = now_ms;
}

/*===========================================================================
 *  周期任务（10ms 调用）
 *===========================================================================*/
void SYS_Task(ClusterSystem_t *sys)
{
    /* --- 自检计时 ---
     * 用 selfcheck_start_ms 做绝对时间差，不再按调用次数累加，
     * 这样即使某一轮调度被拖慢，自检时长也仍然是 CFG_SELFCHECK_DURATION_MS。 */
    if (!sys->selfcheck_done) {
        sys->selfcheck_timer_ms = sys->system_tick_ms - sys->selfcheck_start_ms;
        if (sys->selfcheck_timer_ms >= CFG_SELFCHECK_DURATION_MS) {
            sys->selfcheck_done = true;
        }
    }

    /* --- 1Hz 闪烁节拍（500ms 翻转一次） ---
     *
     * 用 static 局部变量保存上一次翻转时刻，避免污染 sys 结构体；
     * 翻转后的 blink_1hz_visible 由所有需要 1Hz 闪烁的 UI 逻辑统一读取。
     */
    static u32 s_blink_last_ms = 0u;
    if ((sys->system_tick_ms - s_blink_last_ms) >= 500u) {
        s_blink_last_ms        = sys->system_tick_ms;
        sys->blink_1hz_visible = !sys->blink_1hz_visible;
    }
}
