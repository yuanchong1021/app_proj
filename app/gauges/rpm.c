/**
 * @file  rpm.c
 * @brief 转速模块
 *
 * 数据来源：CAN 0x302（10ms，整车下发）
 *   ems->engine_rpm_raw  精度 0.25 r/min
 *   ems->rpm_fault       1 = 转速信号故障或收到无效值 0xFFFF
 *
 * 本次变更：超时判断由 ems->timeout 改为 ems->rpm_timeout。
 *   ems->timeout 由 0x306/0x360（100ms）维护，与 0x302 周期不同，
 *   混用会导致转速的超时行为不可控。
 */
#include "rpm.h"

#define RPM_TASK_MS    10u
#define RPM_TIMEOUT_MS 100u

static u16 s_display     = 0u;
static u32 s_timeout_cnt = 0u;

void RPM_Init(ClusterSystem_t *sys) { s_display = 0u; s_timeout_cnt = 0u; sys->lcd.rpm = 0u; }

void RPM_Task(ClusterSystem_t *sys)
{
    CanEMS_t *ems = &sys->can_rx.ems;

    if (ems->rpm_timeout) {
        s_timeout_cnt += RPM_TASK_MS;
        if (s_timeout_cnt >= RPM_TIMEOUT_MS) { s_display = 0u; }
        sys->lcd.rpm = s_display;
        return;
    }
    s_timeout_cnt = 0u;

    /* 故障或无效值时保持上一次显示，不跳变 */
    if (!ems->rpm_fault) {
        u32 rpm = (u32)ems->engine_rpm_raw * 25u / 100u;   /* × 0.25 */
        if (rpm > CFG_RPM_MAX) { rpm = CFG_RPM_MAX; }
        s_display = (u16)rpm;
    }

    sys->lcd.rpm = s_display;
}

u16 RPM_GetDisplay(const ClusterSystem_t *sys) { return sys->lcd.rpm; }