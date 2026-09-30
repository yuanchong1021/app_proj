/**
 * @file    backlight.h
 * @brief   LCD 背光控制
 *
 * 亮度策略：
 *   电源 OFF                     →   0%（关闭）
 *   小灯（位置灯 / A8）打开      →  CFG_BACKLIGHT_DIM_PERCENT（见 cfg_gauges.h）
 *   其它情况                     → 100%
 *
 * 输出：HAL_BACKLIGHT_Set()（hal_backlight.h）+ 同步 sys->lcd.backlight_pct
 *       背光控制与 LCD 段码显示（meter_display.h）相互独立。
 */
 
#ifndef CLUSTER_BACKLIGHT_H
#define CLUSTER_BACKLIGHT_H
 
#include "../common/cluster_context.h"
 
/** 背光初始化（开机默认 100%） */
void BACKLIGHT_Init(void);
 
/** 背光周期任务（建议 50ms 调用） */
void BACKLIGHT_Task(ClusterSystem_t *sys);
 
#endif /* CLUSTER_BACKLIGHT_H */