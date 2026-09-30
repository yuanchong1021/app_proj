/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2026-05-06 17:03:27
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2026-05-07 17:05:51
 * @FilePath: \instrument\middleware\can\can_app.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/**
 * @file    can_app.h
 * @brief   CAN 应用调度（接收循环 / 发送循环 / 超时检测）
 *
 * 该文件提供 CAN 服务层的"任务级"对外接口：
 *   - CAN_Init           ：模块初始化
 *   - CAN_RxTask         ：从 HAL 收报文，按 ID 路由到对应解析函数（5ms 周期）
 *   - CAN_TxTask         ：构造并发送 0x380 / 0x381（100ms 周期）
 *   - CAN_CheckTimeouts  ：根据 last_recv_ms 判定各报文超时（10ms 周期）
 *
 * 具体单条报文的解析与打包细节分别在 can_rx_parse.c / can_tx_build.c 中。
 */

#ifndef CLUSTER_CAN_APP_H
#define CLUSTER_CAN_APP_H

#include "cluster_context.h"

/**
 * @brief CAN 服务初始化
 *
 * 当前为空（CAN 控制器初始化在 driver 层完成）。
 * 保留接口用于未来扩展（例如：发送 FIFO 初始化、过滤表配置）。
 */
void CAN_Init(void);

/**
 * @brief CAN 接收任务（建议 5ms 调用）
 *
 * @param sys  系统上下文，解析结果写入 sys->can_rx
 *
 * 行为：
 *   1) 反复调用 HAL_CAN_RxPending / HAL_CAN_Recv，把所有可读报文取空
 *   2) 按报文 ID 路由到 CAN_ParseXxx
 *   3) 若仪表当前为 SLEEP，收到任意报文后切换到 WAKE
 */
void CAN_RxTask(ClusterSystem_t *sys);

/**
 * @brief CAN 发送任务（建议 100ms 调用）
 *
 * @param sys  系统上下文（只读，构造发送数据）
 *
 * 仅在 PWR_ON 状态下发送 0x380 / 0x381。
 */
void CAN_TxTask(ClusterSystem_t *sys);

/**
 * @brief CAN 报文超时检查（建议 10ms 调用）
 *
 * @param sys  系统上下文，写入 timeout 字段
 *
 * 各报文的超时阈值由 cfg_can.h 定义。
 */
void CAN_CheckTimeouts(ClusterSystem_t *sys);

#endif /* CLUSTER_CAN_APP_H */
