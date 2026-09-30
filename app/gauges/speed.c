/**
 * @file  speed.c
 * @brief 车速模块
 *
 * 修改点（本次）：
 *   1) 取消"低于 3km/h 强制显示 0"的截断，1~2km/h 正常显示。
 *      归零只由三种情况产生：ABS 超时、speed_valid 无效、原始车速为 0。
 *   2) 配套修掉滞回在低速段的副作用：原逻辑 (prev > HYS) 使得 prev==1 时
 *      永远不下降，车停下后会一直卡在 1km/h。现在 kmh==0 直接归零。
 *
 * 注意：actual_x10 / display_x10 两级除法必须保持分开写。
 *       合并成 raw*23625/40000 虽然比值相同，但少了一次向下截断，
 *       经 (x+9)/10 向上取整放大后会使 155 个 raw 点的显示值偏高 1km/h。
 */
#include "speed.h"

#define SPEED_TASK_MS    10u
#define SPEED_TIMEOUT_MS 200u

static u16 s_display_kmh = 0u;
static u32 s_timeout_cnt = 0u;

void SPEED_Init(ClusterSystem_t *sys)
{
    s_display_kmh = 0u;
    s_timeout_cnt = 0u;
    sys->lcd.speed_kmh = 0u;
}

void SPEED_Task(ClusterSystem_t *sys)
{
    CanABS_t *abs = &sys->can_rx.abs;

    /* --- 1) ABS 报文超时：200ms 未收 → 归零 --- */
    if (abs->timeout) {
        s_timeout_cnt += SPEED_TASK_MS;
        if (s_timeout_cnt >= SPEED_TIMEOUT_MS) { s_display_kmh = 0u; }
        sys->lcd.speed_kmh = s_display_kmh;
        return;
    }
    s_timeout_cnt = 0u;

    /* --- 2) 有效位校验（0 = 有效） --- */
    if (abs->speed_valid != 0u) {
        s_display_kmh = 0u;
        sys->lcd.speed_kmh = 0u;
        return;
    }

    /* --- 3) 线性化 + 校准缩放（保持原两级除法，勿合并） --- */
    u32 actual_x10  = ((u32)abs->vehicle_speed_raw * 225u) / 400u;
    u32 display_x10 = (actual_x10 * 105u) / 100u;

    u16 kmh = (u16)((display_x10 + 9u) / 10u);          /* 向上取整 */
    if (kmh > CFG_SPEED_MAX_KMH) { kmh = CFG_SPEED_MAX_KMH; }
    /* 已删除：if (kmh < CFG_SPEED_ZERO_THRESHOLD) { kmh = 0u; } */

    /* --- 4) 滞回更新：上升立即跟随，停车立即归零，其余走滞回 --- */
    u16 prev = s_display_kmh;
    if (kmh >= prev) {
        s_display_kmh = kmh;
    } else if (kmh == 0u) {
        s_display_kmh = 0u;                             /* 停车立即回 0 */
    } else if ((u16)(prev - kmh) > CFG_SPEED_HYSTERESIS) {
        s_display_kmh = kmh;
    }

    sys->lcd.speed_kmh = s_display_kmh;
}

u16 SPEED_GetDisplayKmh(const ClusterSystem_t *sys) { return sys->lcd.speed_kmh; }