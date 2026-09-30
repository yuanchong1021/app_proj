/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2026-05-07 17:08:08
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2026-05-07 17:27:23
 * @FilePath: \instrument\app\system\sys_mgr.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/**
 * @file    sys_mgr.h
 * @brief   系统管理模块（自检计时 / 系统时基 / 1Hz 闪烁节拍）
 *
 * 该模块负责整个仪表系统级别的"全局节拍"信息：
 *   1. 维护 system_tick_ms（每个调度周期由 main 调用 SYS_UpdateTick 写入）
 *   2. 自检计时：上电后累计到 CFG_SELFCHECK_DURATION_MS 即标记自检完成
 *   3. 1Hz 闪烁节拍：每 500ms 翻转 blink_1hz_visible，
 *      供 LCD / LED 模块所有"闪烁类显示"统一对齐节拍。
 */

#ifndef CLUSTER_SYS_MGR_H
#define CLUSTER_SYS_MGR_H

#include "cluster_context.h"

/**
 * @brief 系统管理模块初始化
 *
 * @param sys  系统上下文
 *
 * 重置自检计时器、闪烁节拍状态、燃料模式默认值（汽油）等。
 */
void SYS_Init(ClusterSystem_t *sys);

/**
 * @brief 启动一轮指示灯自检
 *
 * @param sys  系统上下文
 *
 * 置 selfcheck_done = false 并把计时拉回起点，持续 CFG_SELFCHECK_DURATION_MS。
 * 自检期间 LED_Task 点亮全部故障类指示灯，fuel / cng / tpms 不读真实值。
 *
 * 调用点：
 *   - SYS_Init()（MCU 上电）
 *   - PWR_Task() 检测到点火 ACC/OFF → ON 的跳变时
 *
 * 本函数幂等，可重复调用。
 */
void SYS_StartSelfCheck(ClusterSystem_t *sys);

/**
 * @brief 更新系统时基
 *
 * @param sys     系统上下文
 * @param now_ms  当前 ms 时间戳（HAL_GetTickMs() 的返回值）
 *
 * 由调度器在每轮调度入口处调用一次，保证后续所有任务读取到的时间戳一致。
 */
void SYS_UpdateTick(ClusterSystem_t *sys, u32 now_ms);

/**
 * @brief 系统管理周期任务（建议 10ms 调用）
 *
 * @param sys  系统上下文
 *
 * 职责：
 *   1) 累加自检计时器，到点后置位 selfcheck_done
 *   2) 每 500ms 翻转 blink_1hz_visible，形成 1Hz 闪烁节拍
 */
void SYS_Task(ClusterSystem_t *sys);

#endif /* CLUSTER_SYS_MGR_H */
