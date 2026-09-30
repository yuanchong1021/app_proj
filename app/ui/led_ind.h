/**
 * @file    led_ind.h
 * @brief   指示灯逻辑模块
 *
 * 该模块负责把"硬线 + CAN + 内部状态"综合判断，得出 24 个仪表指示灯
 * 各自的逻辑状态（OFF / ON / BLINK_1HZ / BLINK_2HZ），最后通过
 * meter_display.h 的 Meter_SetIcon() 将逻辑状态翻译为 LCD 面板图标段
 * 的亮灭，并调用 Meter_Refresh() 刷新到 SC5037 DDRAM。
 *
 * 各指示灯独立由 LED_UpdateXxx 计算，便于增删和单元测试。
 *
 * 业务名称 → 面板图标段的映射见：
 *   app/common/config/cfg_meter_map.h
 */

#ifndef CLUSTER_LED_IND_H
#define CLUSTER_LED_IND_H

#include "../../common/cluster_context.h"
#include "soc_uart.h"

/** 指示灯模块初始化（清零 LedStatus_t） */
void LED_Init(ClusterSystem_t *sys);

/**
 * @brief 上电自检：把"自检清单"里的故障指示灯点亮一段时间
 *
 * 规格要求：上电后 CFG_SELFCHECK_DURATION_MS 内故障类指示灯亮起，
 * 方便驾驶员目视检查 LCD 图标段。结束计时由 sys_mgr.c 维护。
 *
 * 参与自检的灯由 cfg_selfcheck.h::CFG_SELFCHECK_LAMP_LIST 定义。
 *
 * 【本次修改】本函数只"覆盖"清单内的灯，**不再整体清零 LedStatus_t**。
 * 清单之外的指示灯（转向灯、安全带未系报警灯、车门、气囊、燃气泄漏、
 * 位置灯、远近光/雾灯、驻车制动、充电灯）在自检期间保持实时业务状态，
 * 对应的报警音也照常播放。
 */
void LED_SelfCheck(ClusterSystem_t *sys);

/**
 * @brief 指示灯周期任务（scheduler.c 中登记周期为 20ms）
 *
 * 每周期都完整执行全部指示灯的业务判定（含 SOC 转向提示音通知、燃气泄漏
 * 蜂鸣器触发），随后若自检尚未结束，再把自检清单里的灯覆盖为常亮，
 * 最后统一刷新到 Meter 缓冲。
 *
 * 【本次修改】自检期间不再直接 return，否则转向灯 / 安全带报警灯等
 * 非自检指示灯在自检的 3s 内无法响应真实信号，报警音也不会播放。
 */
void LED_Task(ClusterSystem_t *sys);

/**
 * @brief 一次性把 LedStatus_t 中所有逻辑状态翻译为 Meter_SetIcon 调用，
 *        并在末尾调用 Meter_Refresh() 推送到 LCD 控制器。
 *
 * 翻译规则：
 *   LED_OFF       → Meter_SetIcon(icon, false)
 *   LED_ON        → Meter_SetIcon(icon, true)
 *   LED_BLINK_1HZ → Meter_SetIcon(icon, sys->blink_1hz_visible)
 *   LED_BLINK_2HZ → Meter_SetIcon(icon, (sys->system_tick_ms/250)&1)
 */
void LED_DriveAll(const ClusterSystem_t *sys);

#endif /* CLUSTER_LED_IND_H */