/**
 * @file    odo.h
 * @brief   里程模块（Section 8.5）
 *
 * 该模块负责：
 *   1) 累计里程：根据车速 × 时间累积，单位最终为 odo_km / trip_dm
 *   2) 小计清零：方向盘 OK 键持续按压 3s
 *   3) 周期持久化：每分钟把 odo_km / trip_dm 写入 NVM
 *
 * 单位约定：
 *   odo_km  : 总计里程，u32，单位 km，最大 999999
 *   trip_dm : 小计里程，u16，单位 0.1km，最大 9999（999.9 km）
 */

#ifndef CLUSTER_ODO_H
#define CLUSTER_ODO_H

#include "../common/cluster_context.h"

/**
 * @brief 里程模块初始化
 *
 * 仅清零内部累积器与按键计时器。
 * NVM 加载由 main 在 PWR_Init 之前显式调用 NVM_LoadOdometer() 完成。
 */
void ODO_Init(ClusterSystem_t *sys);

/**
 * @brief 里程模块周期任务（建议 200ms 调用）
 *
 * 行为：
 *   1) 调用 ODO_Accumulate 进行车速积分累加
 *   2) 检测方向盘 OK 键（电阻 ~8.9kΩ），持续 3s 清零小计里程
 *   3) 每 60 秒把 odo_km / trip_dm 写入 NVM
 */
void ODO_Task(ClusterSystem_t *sys);

/**
 * @brief 请求清零【总里程】odo_km（CMD=0x09 专用）
 *
 * 调用者：serve/input/radio_input.c 在收到并确认 SOC 下发的
 *         CMD=0x09（帧 AA 55 09 5A 53，两帧确认）后调用。
 *
 * 本函数只置一个请求标志、立即返回，真正的清零动作在下一次 ODO_Task
 * （周期 200ms）中执行。这样做的原因是 NVM_SaveOdometer() 底层的 EFM
 * 擦写会关中断并轮询等待数毫秒，不适合放在 10ms 周期的 UART 解析路径上。
 *
 * 执行内容（见 odo.c）：
 *   1) sys->lcd.odometer.odo_km = 0
 *   2) 清空内部积分器与整公里计数，避免清零后立刻被残留里程顶成 1km
 *   3) 立即调用 NVM_SaveOdometer() 落盘，不等 60s 的周期性保存
 *
 * 注意：只清总里程，不动小计里程 trip_dm（小计清零走 CMD=0x03 或方向盘 OK 键）。
 *
 * 线程安全：标志为 volatile bool，写在 Task 上下文（RADIO_Task），
 *           读在 Task 上下文（ODO_Task），单一调度器无并发风险。
 */
void ODO_RequestClearTotal(void);

#endif /* CLUSTER_ODO_H */