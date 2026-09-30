/**
 * @file    can_rx_parse.h
 * @brief   CAN 接收报文解析（按报文 ID 拆分一组解析函数）
 *
 * 每个 CAN_ParseXxx 函数对应一条来自整车的报文：
 *   ABS_330  ECU_302(转速)  EMS_306  EMS_360  EMS_303(GPF)
 *   GAS_305(CNG)  TPMS_390
 *
 * 解析函数职责：
 *   1) 校验数据长度
 *   2) 按整车 CAN 矩阵的位偏移提取信号
 *   3) 写入 sys->can_rx 对应子结构
 *   4) 清 timeout 标志、刷新 last_recv_ms
 *
 * 字节序：整车矩阵为 Motorola（大端），矩阵"起始位"是信号 LSB 的
 *         全局 bit 号，信号向低字节方向增长（字节号小者存高位）。
 */

#ifndef CLUSTER_CAN_RX_PARSE_H
#define CLUSTER_CAN_RX_PARSE_H

#include "cluster_context.h"

/**
 * @brief 解析 ABS 报文（ID 0x330，周期 20ms）
 *        提供：车速 / 车速有效位 / ABS 故障 / EBD 故障 / 报文计数 / 校验和
 *
 * 车速：起始位 8，长度 13，精度 0.05625 km/h，Motorola
 *       → byte1 为低字节，byte0 的 bit0~4 为高位
 */
void CAN_ParseABS(ClusterSystem_t *sys, const u8 *data, u8 len);

/**
 * @brief 解析发动机转速报文（ID 0x302，周期 10ms，整车下发）
 *        提供：发动机转速 / 转速信号故障
 *
 * 转速：起始位 16，长度 16，精度 0.25 r/min，Motorola
 *       → byte1 为高字节，byte2 为低字节，0xFFFF = 无效
 *
 * @note 使用独立的 rpm_timeout / rpm_last_recv_ms，不复用 EMS 的
 *       timeout 字段，避免 10ms 周期刷新掩盖 0x306 的超时。
 */
void CAN_ParseECU302(ClusterSystem_t *sys, const u8 *data, u8 len);

/**
 * @brief 解析 EMS 第一帧（ID 0x306，周期 100ms）
 *        提供：水温 / 水温故障
 */
void CAN_ParseEMS306(ClusterSystem_t *sys, const u8 *data, u8 len);

/**
 * @brief 解析 EMS 第二帧（ID 0x360，周期 100ms）
 *        提供：MIL（发动机故障灯）
 */
void CAN_ParseEMS360(ClusterSystem_t *sys, const u8 *data, u8 len);

/**
 * @brief 解析 GPF 状态报文（ID 0x303，周期 10ms）
 *        提供：颗粒捕集器再生状态
 */
void CAN_ParseGPF(ClusterSystem_t *sys, const u8 *data, u8 len);

/**
 * @brief 解析 CNG 燃气控制器报文（ID 0x305，周期 200ms）
 *        提供：燃气系统故障 / 泄漏状态 / 模式 / CNG 剩余百分比
 */
void CAN_ParseCNG(ClusterSystem_t *sys, const u8 *data, u8 len);

/**
 * @brief 解析 TPMS 报文（ID 0x390，周期 200ms）
 *        提供：4 轮胎压原始值 / 温度 / 各胎传感器状态 / 系统报警
 */
void CAN_ParseTPMS(ClusterSystem_t *sys, const u8 *data, u8 len);

#endif /* CLUSTER_CAN_RX_PARSE_H */