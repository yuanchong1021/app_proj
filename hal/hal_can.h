/**
 * @file    hal_can.h
 * @brief   CAN 控制器硬件抽象层接口
 *
 * 由 driver/can_drv.c 实现。
 * 服务层 service/can/ 所有报文收发都通过本接口完成，
 * 不依赖具体 CAN 控制器（bxCAN / FlexCAN / TJA1043 等）。
 */

#ifndef CLUSTER_HAL_CAN_H
#define CLUSTER_HAL_CAN_H

#include "types.h"
#include "sdk_project_config.h"

#define CAN_INST (0U)

#define TX_STD_MAILBOX (0x01UL)

extern flexcan_msgbuff_t rxStdMsg;

#define RX_STD_MSG_ID  (0x300UL)
#define RX_STD_MAILBOX (0x00UL)

/* 验收掩码：1 = 该位必须匹配。
 * 原值 0x100 只比较 bit8，会放行 0x100~0x1FF / 0x300~0x3FF /
 * 0x500~0x5FF / 0x700~0x7FF，整车其它 ECU 报文都会挤进同一个邮箱。
 * 改为 0x700 后只放行 0x300~0x3FF，覆盖本项目全部接收 ID：
 *   0x303 / 0x305 / 0x306 / 0x330 / 0x360 / 0x390
 */
#define CAN_RX_STD_MASK   (0x700UL)

/*---------------------------------------------------------------------------
 *  诊断（UDS）专用邮箱
 *
 *  诊断请求 ID（0x7C6 物理 / 0x7DF 功能）落在 0x700~0x7FF 范围，与整车
 *  遥测报文（0x300~0x3FF）不冲突，但不能复用同一个邮箱：诊断走 ISO-TP
 *  分帧协议，帧间时序（N_Ar/N_Cr 等）要求比遥测报文紧得多，必须在 CAN
 *  RX 中断里直接喂给 UDS_TP_CanRxPush()，不能等 5ms 轮询任务。
 *  响应 ID（0x7CE）也单独占一个 TX 邮箱，避免和 0x380/0x381 周期发送
 *  抢占同一个邮箱、互相阻塞。
 *  芯片支持 32 个邮箱（见 board/can_config.c 的 max_num_mb），足够富余。
 *---------------------------------------------------------------------------*/
#define RX_UDS_PHYS_MAILBOX (0x02UL)   /**< 诊断物理寻址请求 0x7C6 */
#define RX_UDS_FUNC_MAILBOX (0x03UL)   /**< 诊断功能寻址请求 0x7DF */
#define TX_UDS_MAILBOX      (0x04UL)   /**< 诊断响应 0x7CE          */

/** 精确匹配掩码：全部 11 位都必须相同，只放行邮箱配置的那一个 ID */
#define CAN_RX_UDS_MASK     (0x7FFUL)


extern volatile uint8_t  g_can_rx_pending;   // 0：无新报文  1：有待处理报文
extern flexcan_msgbuff_t rxMsgBuffer;

/** 接收帧（软件 FIFO 的元素） */
typedef struct {
    u32 id;         /**< 标准帧 11 位 ID */
    u8  len;        /**< 数据长度 0~8    */
    u8  data[8];    /**< 数据            */
} CanRxFrame_t;
 
extern volatile u32 g_can_rx_overflow;   /**< FIFO 溢出计数，调试用 */
extern volatile u32 g_can_rx_rearm_err;  /**< 接收邮箱 re-arm 失败次数，调试用 */
extern volatile u32 g_can_rx_recover;    /**< 接收通道看门狗恢复次数，调试用 */
 
/**
 * @brief 从接收 FIFO 取出一帧
 * @param[out] frame 输出帧
 * @return true = 取到一帧 / false = FIFO 为空
 *
 * @note 典型用法（务必用 while 排空，不要一次只取一帧）：
 *   CanRxFrame_t f;
 *   while (HAL_CAN_RxPop(&f)) { ...按 f.id 分发... }
 */
bool HAL_CAN_RxPop(CanRxFrame_t *frame);

/**
 * @brief 设置CAN邮箱（含 FIFO 复位，仅上电初始化调用）
 */
void Hal_SetCanMailbox(void);

/**
 * @brief 恢复接收通道
 *
 * 7 个 ID 共用一个硬件邮箱，FLEXCAN_DRV_Receive() 是一次性 arm 的。
 * 只要某次 re-arm 失败（或控制器进入 bus-off），邮箱就不再挂上，
 * 从此**所有** ID 一起收不到报文，表现为"一个报文超时其余也全超时"。
 *
 * 由 can_app.c 的接收看门狗在连续 500ms 收不到任何帧时调用。
 * 本函数不清空接收 FIFO。
 */
void HAL_CAN_RxRecover(void);

/**
 * @brief 查询接收邮箱当前是否已挂上
 * @return true = 接收通道正常
 */
bool HAL_CAN_RxIsArmed(void);

/**
 * @brief 接收中断回调
 */
void Hal_CAN_RxEventCall(uint8_t instance, flexcan_event_type_t eventType, uint32_t objIdx, flexcan_state_t *flexcanState);

/**
 * @brief 发送一帧 CAN 报文
 *
 * @param id    标准帧 11 位 ID
 * @param data  数据指针（最多 8 字节）
 * @param len   数据长度（0~8）
 * @return true = 发送成功（已入发送队列）/ false = 失败
 */
bool HAL_CAN_Send(u32 id, const u8 *data, u8 len);

/**
 * @brief 发送一帧 UDS 诊断响应报文（走独立的 TX_UDS_MAILBOX）
 *
 * @param id    标准帧 11 位 ID（诊断响应固定为 UDS_ID_RESP = 0x7CE）
 * @param data  数据指针，固定 8 字节（ISO-TP 帧不足 8 字节按规范补 0xAA）
 * @param len   数据长度，固定为 8
 * @return true = 发送成功
 *
 * @note 使用独立邮箱是为了不与 CAN_TxTask 的 0x380/0x381 周期发送抢占
 *       同一个邮箱互相阻塞——诊断报文的实时性要求（ISO-TP 帧间隔）比
 *       100ms 周期报文高得多，不能被后者的忙等挡住。
 */
bool HAL_CAN_SendUds(u32 id, const u8 *data, u8 len);

/**
 * @brief 接收一帧 CAN 报文
 *
 * @param[out] id    返回报文 ID
 * @param[out] data  返回数据缓冲区（至少 8 字节）
 * @param[out] len   返回实际数据长度
 */
void HAL_CAN_Recv(u32 *id, u8 *data, u8 *len);

/**
 * @brief 查询是否有待接收的报文
 *
 * @return true = 接收队列中有未读报文
 *
 * @note 调用者通常的循环：
 *   while (HAL_CAN_RxPending()) { HAL_CAN_Recv(&id, data, &len); ... }
 */
bool HAL_CAN_RxPending(void);

#endif /* CLUSTER_HAL_CAN_H */
