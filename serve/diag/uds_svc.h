/**
 * @file    uds_svc.h
 * @brief   App 侧 UDS 应用层对外接口
 *
 * 覆盖 32 步刷写流程中的 App 侧步骤：1~11 与 25~32。
 */
#ifndef CLUSTER_UDS_SVC_H
#define CLUSTER_UDS_SVC_H

#include "cluster_context.h"

/** UDS 诊断会话类型（与 ISO 14229-1 子功能值一致，便于直接比较） */
typedef enum {
    UDS_SESSION_DEFAULT     = 0x01,
    UDS_SESSION_PROGRAMMING = 0x02,
    UDS_SESSION_EXTENDED    = 0x03
} UdsSession_t;

/**
 * @brief UDS 模块初始化
 * @param sys  系统上下文（用于预编程条件检查读电压/车速，
 *             以及跳转前保存里程）
 *
 * 调用点：Cluster_Init()，须在 CAN_Init() 之后（依赖 CAN 邮箱已配置）。
 */
void UDS_Init(ClusterSystem_t *sys);

/**
 * @brief 网络层周期任务
 * @note  ★ 必须 1ms 调用一次，负责 ISO-TP 分帧/超时推进 ★
 */
void UDS_TpTask(ClusterSystem_t *sys);

/**
 * @brief 应用层周期任务（建议 10ms）
 *
 * 职责：
 *   1) 安全访问锁定倒计时
 *   2) S3Server 会话超时 → 回默认会话
 *   3) 收到 10 02 / 11 01 后的延时挂起动作（等响应上总线再复位/跳转）
 */
void UDS_Task(ClusterSystem_t *sys);

/** @brief 查询当前诊断会话 */
UdsSession_t UDS_GetSession(void);

/** @brief 常规 CAN 收发是否使能（0x28 控制，供 CAN_TxTask 判断是否需要抑制） */
bool UDS_IsNormalCommEnabled(void);

/** @brief DTC 记录是否使能（0x85 控制，供故障诊断模块判断） */
bool UDS_IsDtcRecordingEnabled(void);

/**
 * @brief 清除全部 DTC 的钩子函数（步骤 28：14 FF FF FF）
 * @return true = 清除成功
 *
 * 默认弱实现返回 true。项目里做好故障诊断模块后，在该模块中提供
 * 强实现覆盖默认版本，并删除 uds_svc.c 里的弱实现。
 */
bool UDS_Hook_ClearAllDtc(void);

#endif /* CLUSTER_UDS_SVC_H */
