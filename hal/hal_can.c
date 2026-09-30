/**
 * @file  hal_can.c
 * @brief CAN HAL 实现
 *
 * ===========================================================================
 * 本次修改（问题 1：一个报文超时导致全部报文超时）
 * ===========================================================================
 *   1) FLEXCAN_DRV_Receive() 的返回值不再丢弃。
 *      原代码 `status = FLEXCAN_DRV_Receive(...); (void)status;`：
 *      7 个 ID 共用一个邮箱，该函数是"一次性 arm"的，只要某一次 re-arm
 *      返回 STATUS_BUSY / 出错，邮箱就永远不再挂上，从此**所有** ID 一起
 *      收不到报文 → 全部超时。现在用 s_rx_armed 记录 arm 结果。
 *
 *   2) 新增 HAL_CAN_RxRecover()，供上层看门狗（can_app.c CAN_RxTask）在
 *      长时间收不到帧时调用：先尝试重新 arm，失败再整体重配邮箱。
 *
 *   3) 邮箱重配与 FIFO 复位分离：Hal_SetCanMailbox() 才复位 FIFO 指针，
 *      恢复路径不动 FIFO，避免与中断产生竞态丢数据。
 *
 * 既有修复保留：
 *   - 软件接收 FIFO（原来所有报文共用一个 rxMsgBuffer，任务每 5ms 只取一帧）
 *   - 驱动独占 s_rx_mb，中断里再搬进 FIFO，消除撕裂读
 *   - 验收掩码由 0x100 改为 0x700，只放行 0x300~0x3FF
 */
#include "hal_can.h"
#include "uds_tp.h"
#include "boot_app_if.h"
#include <string.h>

/*---------------------------------------------------------------------------
 *  接收 FIFO（容量必须是 2 的幂）
 *-------------------------------------------------------------------------*/
#define CAN_RX_FIFO_SIZE    32u
#define CAN_RX_FIFO_MASK    (CAN_RX_FIFO_SIZE - 1u)

static CanRxFrame_t       s_fifo[CAN_RX_FIFO_SIZE];
static volatile u8        s_fifo_head = 0u;   /* 中断写入位置 */
static volatile u8        s_fifo_tail = 0u;   /* 任务读出位置 */

volatile u32              g_can_rx_overflow = 0u;  /* FIFO 溢出计数，便于调试 */
volatile u32              g_can_rx_rearm_err = 0u; /* re-arm 失败次数，便于调试 */
volatile u32              g_can_rx_recover  = 0u;  /* 看门狗恢复次数，便于调试 */
volatile uint8_t          g_can_rx_pending  = 0u;  /* 兼容旧代码，等价于 FIFO 非空 */

/* 接收邮箱当前是否已挂上（armed）。false 表示接收通道已死，需要恢复。 */
static volatile bool      s_rx_armed = false;

/* 驱动独占的接收缓冲：只有 FlexCAN 驱动往里写，应用层不再直接访问 */
static flexcan_msgbuff_t  s_rx_mb = {0};

/*---------------------------------------------------------------------------
 *  诊断（UDS）专用邮箱的独立接收缓冲与 armed 状态
 *
 *  诊断帧不进入 s_fifo，而是在中断里直接调用 UDS_TP_CanRxPush()，
 *  所以这里不需要额外的软件 FIFO，只需要各自的驱动接收缓冲。
 *---------------------------------------------------------------------------*/
static flexcan_msgbuff_t  s_rx_mb_uds_phys = {0};
static flexcan_msgbuff_t  s_rx_mb_uds_func = {0};
static volatile bool      s_rx_armed_uds_phys = false;
static volatile bool      s_rx_armed_uds_func = false;

/* 保留旧的全局符号，避免其它模块引用出错（已不再作为数据通路） */
flexcan_msgbuff_t rxMsgBuffer = {0};
flexcan_msgbuff_t rxStdMsg;

static const flexcan_data_info_t s_tx_info = {
    .msg_id_type = FLEXCAN_MSG_ID_STD,
    .data_length = 8u,
    .fd_enable   = false,
    .fd_padding  = 0u,
    .enable_brs  = false,
    .is_remote   = false,
};
static const flexcan_data_info_t s_rx_info = {
    .msg_id_type = FLEXCAN_MSG_ID_STD,
    .data_length = 8u,
    .fd_enable   = false,
    .fd_padding  = 0u,
    .enable_brs  = false,
    .is_remote   = false,
};

/*---------------------------------------------------------------------------
 *  内部：重新挂接收邮箱，并记录结果
 *
 *  三个 RX 邮箱（遥测 / 诊断物理寻址 / 诊断功能寻址）各自独立 arm，
 *  用 mbIdx 区分，armed 标志也是各邮箱一份，互不影响：诊断邮箱短暂
 *  失去 arm 不该连累遥测报文判超时，反之亦然。
 *-------------------------------------------------------------------------*/
static bool can_rx_arm_mb(u8 mbIdx, flexcan_msgbuff_t *mb, volatile bool *armedFlag)
{
    status_t st = FLEXCAN_DRV_Receive(CAN_INST, mbIdx, mb);

    if (st == STATUS_SUCCESS) {
        *armedFlag = true;
    } else {
        *armedFlag = false;
        g_can_rx_rearm_err++;
    }
    return *armedFlag;
}

static bool can_rx_arm(void)
{
    return can_rx_arm_mb((u8)RX_STD_MAILBOX, &s_rx_mb, &s_rx_armed);
}

void Hal_SetCanMailbox(void)
{
    s_fifo_head = 0u;
    s_fifo_tail = 0u;

    /* --- 遥测邮箱（0x300~0x3FF，个体掩码） --- */
    FLEXCAN_DRV_ConfigRxMb(CAN_INST, RX_STD_MAILBOX, &s_rx_info, RX_STD_MSG_ID);
    FLEXCAN_DRV_ConfigTxMb(CAN_INST, TX_STD_MAILBOX, &s_tx_info, 0x01u);
    FLEXCAN_DRV_SetRxMaskType(CAN_INST, FLEXCAN_RX_MASK_INDIVIDUAL);

    /* 掩码 1 = 该位必须匹配。0x700 → 只接收 0x300~0x3FF */
    FLEXCAN_DRV_SetRxIndividualMask(CAN_INST, FLEXCAN_MSG_ID_STD,
                                    RX_STD_MAILBOX, CAN_RX_STD_MASK);

    /* --- 诊断邮箱：各自精确匹配一个 ID，互不影响 --- */
    FLEXCAN_DRV_ConfigRxMb(CAN_INST, RX_UDS_PHYS_MAILBOX, &s_rx_info, (uint32_t)UDS_ID_PHYS_REQ);
    FLEXCAN_DRV_SetRxIndividualMask(CAN_INST, FLEXCAN_MSG_ID_STD,
                                    RX_UDS_PHYS_MAILBOX, CAN_RX_UDS_MASK);

    FLEXCAN_DRV_ConfigRxMb(CAN_INST, RX_UDS_FUNC_MAILBOX, &s_rx_info, (uint32_t)UDS_ID_FUNC_REQ);
    FLEXCAN_DRV_SetRxIndividualMask(CAN_INST, FLEXCAN_MSG_ID_STD,
                                    RX_UDS_FUNC_MAILBOX, CAN_RX_UDS_MASK);

    FLEXCAN_DRV_ConfigTxMb(CAN_INST, TX_UDS_MAILBOX, &s_tx_info, (uint32_t)UDS_ID_RESP);

    (void)can_rx_arm();
    (void)can_rx_arm_mb((u8)RX_UDS_PHYS_MAILBOX, &s_rx_mb_uds_phys, &s_rx_armed_uds_phys);
    (void)can_rx_arm_mb((u8)RX_UDS_FUNC_MAILBOX, &s_rx_mb_uds_func, &s_rx_armed_uds_func);
}

/**
 * @brief 恢复接收通道（由 CAN_RxTask 的看门狗调用）
 *
 * 先尝试轻量恢复（重新 arm）；若仍失败，说明驱动状态机或控制器状态异常，
 * 再做一次完整的邮箱重配。注意此处不清空 FIFO，避免丢掉已入队的数据。
 *
 * 遥测邮箱的看门狗（CAN_RxTask 里，连续 500ms 无遥测帧触发）顺带一并
 * 恢复两个诊断邮箱：诊断帧本来就是偶发的（只有联调/售后时才有），不会
 * 有专门盯着诊断邮箱的看门狗，借用遥测看门狗的触发时机一起修复即可。
 */
void HAL_CAN_RxRecover(void)
{
    g_can_rx_recover++;

    if (!can_rx_arm()) {
        /* 轻量恢复失败：整体重配邮箱（内部会再 arm 一次） */
        FLEXCAN_DRV_ConfigRxMb(CAN_INST, RX_STD_MAILBOX, &s_rx_info, RX_STD_MSG_ID);
        FLEXCAN_DRV_SetRxMaskType(CAN_INST, FLEXCAN_RX_MASK_INDIVIDUAL);
        FLEXCAN_DRV_SetRxIndividualMask(CAN_INST, FLEXCAN_MSG_ID_STD,
                                        RX_STD_MAILBOX, CAN_RX_STD_MASK);
        (void)can_rx_arm();
    }

    if (!can_rx_arm_mb((u8)RX_UDS_PHYS_MAILBOX, &s_rx_mb_uds_phys, &s_rx_armed_uds_phys)) {
        FLEXCAN_DRV_ConfigRxMb(CAN_INST, RX_UDS_PHYS_MAILBOX, &s_rx_info, (uint32_t)UDS_ID_PHYS_REQ);
        FLEXCAN_DRV_SetRxIndividualMask(CAN_INST, FLEXCAN_MSG_ID_STD,
                                        RX_UDS_PHYS_MAILBOX, CAN_RX_UDS_MASK);
        (void)can_rx_arm_mb((u8)RX_UDS_PHYS_MAILBOX, &s_rx_mb_uds_phys, &s_rx_armed_uds_phys);
    }

    if (!can_rx_arm_mb((u8)RX_UDS_FUNC_MAILBOX, &s_rx_mb_uds_func, &s_rx_armed_uds_func)) {
        FLEXCAN_DRV_ConfigRxMb(CAN_INST, RX_UDS_FUNC_MAILBOX, &s_rx_info, (uint32_t)UDS_ID_FUNC_REQ);
        FLEXCAN_DRV_SetRxIndividualMask(CAN_INST, FLEXCAN_MSG_ID_STD,
                                        RX_UDS_FUNC_MAILBOX, CAN_RX_UDS_MASK);
        (void)can_rx_arm_mb((u8)RX_UDS_FUNC_MAILBOX, &s_rx_mb_uds_func, &s_rx_armed_uds_func);
    }
}

bool HAL_CAN_RxIsArmed(void)
{
    return s_rx_armed && s_rx_armed_uds_phys && s_rx_armed_uds_func;
}

void Hal_CAN_RxEventCall(uint8_t instance, flexcan_event_type_t eventType,
                         uint32_t objIdx, flexcan_state_t *flexcanState)
{
    (void)instance; (void)flexcanState;

    u8 next;
    u8 i;
    u8 len;

    switch (eventType) {
        case FLEXCAN_EVENT_RX_COMPLETE:

            /* --- 诊断邮箱：直接喂给 ISO-TP 层，不进遥测 FIFO ---
             * 诊断帧要满足 ISO-TP 的帧间时序（N_Ar/N_Cr 等，毫秒级），
             * 若还要等 CAN_RxTask 的 5ms 轮询才转发，多帧传输会偶发
             * 失败。中断里直接调用足够轻量（只是拷贝 8 字节入环形
             * 缓冲，UDS_TP_CanRxPush 内部无阻塞）。 */
            if (objIdx == (uint32_t)RX_UDS_PHYS_MAILBOX) {
                UDS_TP_CanRxPush((u32)s_rx_mb_uds_phys.msgId,
                                 s_rx_mb_uds_phys.data,
                                 (u8)s_rx_mb_uds_phys.dataLen);
                (void)can_rx_arm_mb((u8)RX_UDS_PHYS_MAILBOX, &s_rx_mb_uds_phys, &s_rx_armed_uds_phys);
                break;
            }
            if (objIdx == (uint32_t)RX_UDS_FUNC_MAILBOX) {
                UDS_TP_CanRxPush((u32)s_rx_mb_uds_func.msgId,
                                 s_rx_mb_uds_func.data,
                                 (u8)s_rx_mb_uds_func.dataLen);
                (void)can_rx_arm_mb((u8)RX_UDS_FUNC_MAILBOX, &s_rx_mb_uds_func, &s_rx_armed_uds_func);
                break;
            }

            /* --- 遥测邮箱：走原有软件 FIFO，5ms 任务里排空 --- */
            next = (u8)((s_fifo_head + 1u) & CAN_RX_FIFO_MASK);
            if (next != s_fifo_tail) {          /* FIFO 未满 */
                len = (s_rx_mb.dataLen > 8u) ? 8u : (u8)s_rx_mb.dataLen;
                s_fifo[s_fifo_head].id  = (u32)s_rx_mb.msgId;
                s_fifo[s_fifo_head].len = len;
                for (i = 0u; i < len; i++) {
                    s_fifo[s_fifo_head].data[i] = s_rx_mb.data[i];
                }
                s_fifo_head = next;             /* 数据写完再推进 head */
                g_can_rx_pending = 1u;
            } else {
                g_can_rx_overflow++;            /* 溢出：丢弃本帧 */
            }

            /* 重新挂接收。返回值不再丢弃：失败会清 s_rx_armed，
             * 由 CAN_RxTask 的看门狗在 500ms 内调用 HAL_CAN_RxRecover() 恢复。 */
            (void)can_rx_arm();
            break;

        case FLEXCAN_EVENT_TX_COMPLETE:
            break;

        default:
            /* 其它事件（错误 / 总线关闭等）：标记全部接收通道不可信，
             * 交给上层看门狗统一恢复，避免在中断里做重配置。 */
            s_rx_armed          = false;
            s_rx_armed_uds_phys = false;
            s_rx_armed_uds_func = false;
            break;
    }
}

bool HAL_CAN_RxPop(CanRxFrame_t *frame)
{
    u8 tail;
    u8 i;

    if (frame == NULL) { return false; }
    if (s_fifo_tail == s_fifo_head) {           /* 空 */
        g_can_rx_pending = 0u;
        return false;
    }

    tail = s_fifo_tail;
    frame->id  = s_fifo[tail].id;
    frame->len = s_fifo[tail].len;
    for (i = 0u; i < 8u; i++) {
        frame->data[i] = s_fifo[tail].data[i];
    }
    s_fifo_tail = (u8)((tail + 1u) & CAN_RX_FIFO_MASK);

    if (s_fifo_tail == s_fifo_head) { g_can_rx_pending = 0u; }
    return true;
}

bool HAL_CAN_RxPending(void)
{
    return (s_fifo_tail != s_fifo_head);
}

bool HAL_CAN_Send(u32 id, const u8 *data, u8 len)
{
    if (!data || len == 0u || len > 8u) { return false; }
    uint32_t timeout = 1000u;
    while (FLEXCAN_DRV_GetTransferStatus(CAN_INST, TX_STD_MAILBOX) == STATUS_BUSY
           && timeout > 0u) { timeout--; }
    if (timeout == 0u) { return false; }
    uint8_t buf[8u] = {0u};
    memcpy(buf, data, len);
    return (FLEXCAN_DRV_Send(CAN_INST, TX_STD_MAILBOX, &s_tx_info, id, buf) == STATUS_SUCCESS);
}

bool HAL_CAN_SendUds(u32 id, const u8 *data, u8 len)
{
    if (!data || len == 0u || len > 8u) { return false; }
    uint32_t timeout = 1000u;
    while (FLEXCAN_DRV_GetTransferStatus(CAN_INST, TX_UDS_MAILBOX) == STATUS_BUSY
           && timeout > 0u) { timeout--; }
    if (timeout == 0u) { return false; }
    uint8_t buf[8u] = {0u};
    memcpy(buf, data, len);
    return (FLEXCAN_DRV_Send(CAN_INST, TX_UDS_MAILBOX, &s_tx_info, id, buf) == STATUS_SUCCESS);
}

/* 兼容旧接口：等价于 pop 一帧 */
void HAL_CAN_Recv(u32 *id, u8 *data, u8 *len)
{
    CanRxFrame_t f;

    if (!id || !data || !len) { return; }
    if (HAL_CAN_RxPop(&f)) {
        *id  = f.id;
        *len = f.len;
        memcpy(data, f.data, 8u);
    } else {
        *len = 0u;
    }
}