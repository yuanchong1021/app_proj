/**
 * @file    can_tx_build.h
 * @brief   仪表外发 CAN 报文打包（0x380 / 0x381）
 *
 * 这两个发送帧由本仪表节点生成，向整车发布油量百分比、里程、
 * 安全带、转向、外温等信息。打包逻辑独立成单独的 .c/.h，
 * 方便后续按 CAN 矩阵改动单独维护。
 */

#ifndef CLUSTER_CAN_TX_BUILD_H
#define CLUSTER_CAN_TX_BUILD_H

#include "cluster_context.h"

/**
 * @brief 构造 0x380 报文数据（8 字节）
 *
 * @param sys           系统上下文（只读）
 * @param[out] data     输出缓冲区，需至少 8 字节
 */
void CAN_BuildTx380(const ClusterSystem_t *sys, u8 *data);

/**
 * @brief 构造 0x381 报文数据（8 字节）
 *
 * @param sys           系统上下文（只读）
 * @param[out] data     输出缓冲区，需至少 8 字节
 */
void CAN_BuildTx381(const ClusterSystem_t *sys, u8 *data);

#endif /* CLUSTER_CAN_TX_BUILD_H */
