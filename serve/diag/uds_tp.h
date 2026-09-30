/**
 * @file    uds_tp.h
 * @brief   ISO 15765-2 网络层（App 侧）
 *
 * 与 Bootloader 工程的 uds_tp.c / uds_tp.h 逻辑完全一致（网络层与会话
 * 内容无关），差异全部收敛在 uds_cfg.h。两边分属不同工程，这里是独立
 * 编译的一份拷贝，修改网络层行为时请与 Boot 侧同步。
 */
#ifndef UDS_TP_H
#define UDS_TP_H

#include "types.h"
#include "uds_cfg.h"

typedef enum {
    UDS_ADDR_PHYSICAL = 0,    /**< 0x7C6 */
    UDS_ADDR_FUNCTIONAL       /**< 0x7DF */
} UdsAddrType_t;

void UDS_TP_Init(void);

/**
 * 把收到的 CAN 帧投进环形缓冲。
 * 本工程由 Hal_CAN_RxEventCall() 在 CAN RX 中断里直接调用（诊断帧走
 * 独立邮箱，不经过 serve/can 的 5ms 轮询 FIFO，以满足 ISO-TP 的
 * 帧间时序要求）。
 */
void UDS_TP_CanRxPush(u32 canId, const u8 *data, u8 dlc);

/** 发送一条完整的 UDS 报文，长度 > 7 时自动走多帧 */
bool UDS_TP_Send(const u8 *data, u16 len);

bool UDS_TP_IsTxIdle(void);
void UDS_TP_Abort(void);

/**
 * 周期任务。
 * ★ 必须 1ms 调用一次。挂 5ms 时多帧会偶发失败，报文越长越容易失败。
 */
void UDS_TP_Task(void);

/**
 * 收到完整报文的回调，由应用层实现（本工程在 uds_svc.c）。
 */
void UDS_TP_RxIndication(UdsAddrType_t addrType, const u8 *data, u16 len);

#endif /* UDS_TP_H */
