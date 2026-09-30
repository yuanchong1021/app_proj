/**
 * @file    scheduler.h
 * @brief   合作式任务调度器
 *
 * 该调度器是仪表系统的运行核心：
 *   - 维护一张任务表，每项 = (周期, 上次运行时刻, 函数指针, 名称)
 *   - 主循环里持续调用 Scheduler_RunOnce，按周期到点的任务依次触发
 *
 * 设计上是单线程合作式调度（没有抢占），因此各任务函数必须保证：
 *   1) 不阻塞（不调用阻塞 IO、不死循环）
 *   2) 单次执行时间应远小于 1ms（最快任务周期）
 *   3) 任务之间的同步通过 ClusterSystem_t 数据完成
 *
 * 任务表初始化在 scheduler.c 内部完成；如果将来需要动态注册任务，
 * 可以扩展 Scheduler_AddTask 接口。
 */

#ifndef CLUSTER_SCHEDULER_H
#define CLUSTER_SCHEDULER_H

#include "../common/cluster_context.h"

/**
 * @brief 调度器初始化
 *
 * 当前实现仅清零任务表的"上次运行时刻"，使各任务都能在第一次主循环时立刻触发。
 */
void Scheduler_Init(void);

/**
 * @brief 主循环驱动（应在 main 的 while(1) 中持续调用）
 *
 * @param sys  系统上下文（会被传递给每个 Task 函数）
 *
 * 实现步骤：
 *   1) 查询 HAL_GetTickMs() 获得当前时间，写入 sys->system_tick_ms
 *   2) 遍历任务表，对每个到期任务调用其 task_fn(sys)
 */
void Scheduler_RunOnce(ClusterSystem_t *sys);

#endif /* CLUSTER_SCHEDULER_H */
