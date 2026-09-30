/**
 * @file    uds_tp.c
 * @brief   ISO 15765-2 网络层实现（App 侧）
 *
 * 本文件与 Bootloader 工程的 uds_tp.c 逻辑完全一致，独立维护一份拷贝。
 *
 * 帧格式（标准帧 / 正常寻址 / CAN_DL = 8）：
 *   SF  : [0x0L][data...]                    L = 1~7
 *   FF  : [0x1H][LL][data×6]                 长度 = 0xHLL（12 位）
 *   CF  : [0x2S][data×7]                     S = 序号 0~15 循环
 *   FC  : [0x3F][BS][STmin]                  F = 0:CTS 1:WAIT 2:OVFLW
 *
 * 所有帧不足 8 字节以 UDS_TP_PADDING 填充。
 */

#include "uds_tp.h"
#include "hal_can.h"
#include "hal_tick.h"
#include <string.h>

/*===========================================================================
 *  PCI 常量
 *===========================================================================*/
#define PCI_TYPE_MASK   (0x30U)
#define PCI_SF          (0x00U)
#define PCI_FF          (0x10U)
#define PCI_CF          (0x20U)
#define PCI_FC          (0x30U)

#define FC_CTS          (0x00U)
#define FC_WAIT         (0x01U)
#define FC_OVFLW        (0x02U)

/*===========================================================================
 *  中断投递用的环形缓冲
 *
 *  深度 32：500kbps 下 CF 最快约 0.2ms 一帧，1ms 任务周期最多积压 5 帧，
 *  32 深度足够吸收调度抖动。
 *===========================================================================*/
#define TP_RING_DEPTH   (32U)
#define TP_RING_MASK    (TP_RING_DEPTH - 1U)

typedef struct {
    u32 id;
    u8  len;
    u8  data[8];
} TpFrame_t;

static volatile TpFrame_t s_ring[TP_RING_DEPTH];
static volatile u8        s_ring_head = 0U;
static volatile u8        s_ring_tail = 0U;
static volatile u32       s_ring_lost = 0U;   /**< 溢出计数，调试用 */

/*===========================================================================
 *  接收状态机
 *===========================================================================*/
typedef enum {
    RX_IDLE = 0,
    RX_WAIT_CF
} TpRxState_t;

static TpRxState_t   s_rx_state  = RX_IDLE;
static u8            s_rx_buf[UDS_BUF_SIZE];
static u16           s_rx_len    = 0U;    /**< 期望总长度 */
static u16           s_rx_pos    = 0U;    /**< 已收字节数 */
static u8            s_rx_sn     = 0U;    /**< 期望的下一个序号 */
static u32           s_rx_timer  = 0U;    /**< N_Cr 计时起点 */
static UdsAddrType_t s_rx_addr   = UDS_ADDR_PHYSICAL;

/*===========================================================================
 *  发送状态机
 *===========================================================================*/
typedef enum {
    TX_IDLE = 0,
    TX_WAIT_FC,
    TX_SEND_CF
} TpTxState_t;

static TpTxState_t s_tx_state = TX_IDLE;
static u8          s_tx_buf[UDS_BUF_SIZE];
static u16         s_tx_len   = 0U;
static u16         s_tx_pos   = 0U;
static u8          s_tx_sn    = 0U;
static u32         s_tx_timer = 0U;   /**< N_Bs 计时起点 */
static u8          s_tx_bs    = 0U;   /**< 对端给的 BlockSize，0 = 不限 */
static u8          s_tx_bs_cnt = 0U;  /**< 本块已发 CF 数 */
static u8          s_tx_stmin = 0U;   /**< 对端要求的 STmin（已折算为 ms） */
static u32         s_tx_last  = 0U;   /**< 上一帧 CF 发送时刻 */

/*===========================================================================
 *  内部工具
 *===========================================================================*/

/** 发送一帧，不足 8 字节按规范填充 */
static bool tp_send_frame(const u8 *payload, u8 len)
{
    u8 frame[8];
    u8 i;

    for (i = 0U; i < 8U; i++) {
        frame[i] = (i < len) ? payload[i] : UDS_TP_PADDING;
    }
    return HAL_CAN_SendUds(UDS_ID_RESP, frame, 8U);
}

/** 发送流控帧 */
static void tp_send_fc(u8 flowStatus)
{
    u8 fc[3];

    fc[0] = (u8)(PCI_FC | (flowStatus & 0x0FU));
    fc[1] = UDS_TP_BS;
    fc[2] = UDS_TP_STMIN;
    (void)tp_send_frame(fc, 3U);
}

/** STmin 编码 → 毫秒（1ms 分辨率，亚毫秒一律按 1ms 处理） */
static u8 tp_decode_stmin(u8 raw)
{
    if (raw <= 0x7FU)                    { return raw; }
    if ((raw >= 0xF1U) && (raw <= 0xF9U)) { return 1U; }
    return 0x7FU;                        /* 保留值：按规范取最大 127ms */
}

/** 复位接收状态机 */
static void tp_rx_reset(void)
{
    s_rx_state = RX_IDLE;
    s_rx_len   = 0U;
    s_rx_pos   = 0U;
    s_rx_sn    = 0U;
}

/** 复位发送状态机 */
static void tp_tx_reset(void)
{
    s_tx_state  = TX_IDLE;
    s_tx_len    = 0U;
    s_tx_pos    = 0U;
    s_tx_sn     = 0U;
    s_tx_bs     = 0U;
    s_tx_bs_cnt = 0U;
    s_tx_stmin  = 0U;
}

/*===========================================================================
 *  单帧处理
 *===========================================================================*/
static void tp_handle_sf(UdsAddrType_t addr, const u8 *d, u8 dlc)
{
    u8 len = (u8)(d[0] & 0x0FU);

    /* len==0 是 CAN FD 的长度转义，经典 CAN 不支持 */
    if ((len == 0U) || (len > 7U) || ((u8)(len + 1U) > dlc)) { return; }

    /* SF 抢占：正在收多帧时收到新的 SF，按规范丢弃旧的、处理新的 */
    tp_rx_reset();

    memcpy(s_rx_buf, &d[1], len);
    UDS_TP_RxIndication(addr, s_rx_buf, len);
}

static void tp_handle_ff(UdsAddrType_t addr, const u8 *d, u8 dlc)
{
    u16 total;

    /* 功能寻址不允许多帧（ISO 15765-2 §6.4） */
    if (addr != UDS_ADDR_PHYSICAL) { return; }
    if (dlc < 8U)                  { return; }

    total = (u16)(((u16)(d[0] & 0x0FU) << 8) | d[1]);

    /* total==0 是 32 位长度转义（FF_DL > 4095），本实现不支持 */
    if (total <= 7U) { return; }

    if (total > UDS_BUF_SIZE) {
        tp_send_fc(FC_OVFLW);
        tp_rx_reset();
        return;
    }

    memcpy(s_rx_buf, &d[2], 6U);
    s_rx_len   = total;
    s_rx_pos   = 6U;
    s_rx_sn    = 1U;
    s_rx_addr  = addr;
    s_rx_state = RX_WAIT_CF;
    s_rx_timer = HAL_GetTickMs();

    tp_send_fc(FC_CTS);
}

static void tp_handle_cf(const u8 *d, u8 dlc)
{
    u8  sn;
    u16 remain;
    u16 take;

    if (s_rx_state != RX_WAIT_CF) { return; }

    sn = (u8)(d[0] & 0x0FU);
    if (sn != s_rx_sn) {              /* 序号错：按规范中止本次接收 */
        tp_rx_reset();
        return;
    }

    remain = (u16)(s_rx_len - s_rx_pos);
    take   = (remain > 7U) ? 7U : remain;
    if ((u16)(take + 1U) > dlc) {     /* 帧长不足以容纳应有数据 */
        tp_rx_reset();
        return;
    }

    memcpy(&s_rx_buf[s_rx_pos], &d[1], take);
    s_rx_pos = (u16)(s_rx_pos + take);
    s_rx_sn  = (u8)((s_rx_sn + 1U) & 0x0FU);
    s_rx_timer = HAL_GetTickMs();

    if (s_rx_pos >= s_rx_len) {
        u16 len = s_rx_len;
        UdsAddrType_t addr = s_rx_addr;
        tp_rx_reset();
        UDS_TP_RxIndication(addr, s_rx_buf, len);
    }
}

static void tp_handle_fc(const u8 *d, u8 dlc)
{
    u8 fs;

    if (s_tx_state != TX_WAIT_FC) { return; }
    if (dlc < 3U)                 { return; }

    fs = (u8)(d[0] & 0x0FU);

    switch (fs) {
        case FC_CTS:
            s_tx_bs     = d[1];
            s_tx_stmin  = tp_decode_stmin(d[2]);
            s_tx_bs_cnt = 0U;
            s_tx_state  = TX_SEND_CF;
            s_tx_last   = HAL_GetTickMs();
            /* STmin 为 0 时本周期立刻开始发，不必等到下一个 tick */
            break;

        case FC_WAIT:
            s_tx_timer = HAL_GetTickMs();   /* 重置 N_Bs，继续等 */
            break;

        case FC_OVFLW:
        default:
            tp_tx_reset();                  /* 对端缓冲不足或非法 FS：放弃 */
            break;
    }
}

/*===========================================================================
 *  发送推进
 *===========================================================================*/
static void tp_tx_advance(void)
{
    u8  frame[8];
    u16 remain;
    u8  take;
    u32 now = HAL_GetTickMs();

    if (s_tx_state != TX_SEND_CF) { return; }

    /* STmin 间隔控制 */
    if (s_tx_stmin != 0U) {
        if ((u32)(now - s_tx_last) < (u32)s_tx_stmin) { return; }
    }

    remain = (u16)(s_tx_len - s_tx_pos);
    take   = (remain > 7U) ? 7U : (u8)remain;

    frame[0] = (u8)(PCI_CF | (s_tx_sn & 0x0FU));
    memcpy(&frame[1], &s_tx_buf[s_tx_pos], take);

    if (!tp_send_frame(frame, (u8)(take + 1U))) {
        return;                     /* 发送忙，下个 tick 重试 */
    }

    s_tx_pos  = (u16)(s_tx_pos + take);
    s_tx_sn   = (u8)((s_tx_sn + 1U) & 0x0FU);
    s_tx_last = now;

    if (s_tx_pos >= s_tx_len) {
        tp_tx_reset();
        return;
    }

    /* BlockSize 用完：回到等待下一个 FC */
    if (s_tx_bs != 0U) {
        s_tx_bs_cnt++;
        if (s_tx_bs_cnt >= s_tx_bs) {
            s_tx_state = TX_WAIT_FC;
            s_tx_timer = now;
        }
    }
}

/*===========================================================================
 *  对外接口
 *===========================================================================*/

void UDS_TP_Init(void)
{
    s_ring_head = 0U;
    s_ring_tail = 0U;
    s_ring_lost = 0U;
    tp_rx_reset();
    tp_tx_reset();
}

void UDS_TP_CanRxPush(u32 canId, const u8 *data, u8 dlc)
{
    u8 next;
    u8 i;

    if ((canId != UDS_ID_PHYS_REQ) && (canId != UDS_ID_FUNC_REQ)) { return; }

    next = (u8)((s_ring_head + 1U) & TP_RING_MASK);
    if (next == s_ring_tail) {          /* 满 */
        s_ring_lost++;
        return;
    }

    s_ring[s_ring_head].id  = canId;
    s_ring[s_ring_head].len = (dlc > 8U) ? 8U : dlc;
    for (i = 0U; i < 8U; i++) {
        s_ring[s_ring_head].data[i] = (i < dlc) ? data[i] : 0U;
    }
    s_ring_head = next;                 /* 数据写完再推进 head */
}

bool UDS_TP_Send(const u8 *data, u16 len)
{
    u8 frame[8];

    if ((data == NULL) || (len == 0U) || (len > UDS_BUF_SIZE)) { return false; }
    if (s_tx_state != TX_IDLE)                                 { return false; }

    if (len <= 7U) {
        /* 单帧 */
        frame[0] = (u8)(PCI_SF | (u8)len);
        memcpy(&frame[1], data, len);
        return tp_send_frame(frame, (u8)(len + 1U));
    }

    /* 多帧：先发首帧，再等流控 */
    memcpy(s_tx_buf, data, len);
    s_tx_len = len;

    frame[0] = (u8)(PCI_FF | (u8)((len >> 8) & 0x0FU));
    frame[1] = (u8)(len & 0xFFU);
    memcpy(&frame[2], s_tx_buf, 6U);

    if (!tp_send_frame(frame, 8U)) {
        tp_tx_reset();
        return false;
    }

    s_tx_pos   = 6U;
    s_tx_sn    = 1U;
    s_tx_state = TX_WAIT_FC;
    s_tx_timer = HAL_GetTickMs();
    return true;
}

bool UDS_TP_IsTxIdle(void)
{
    return (s_tx_state == TX_IDLE);
}

void UDS_TP_Abort(void)
{
    tp_rx_reset();
    tp_tx_reset();
}

void UDS_TP_Task(void)
{
    TpFrame_t     f;
    u8            i;
    UdsAddrType_t addr;
    u32           now;

    /* --- 1) 排空接收环形缓冲 --- */
    while (s_ring_tail != s_ring_head) {
        u8 tail = s_ring_tail;

        f.id  = s_ring[tail].id;
        f.len = s_ring[tail].len;
        for (i = 0U; i < 8U; i++) { f.data[i] = s_ring[tail].data[i]; }
        s_ring_tail = (u8)((tail + 1U) & TP_RING_MASK);

        if (f.len == 0U) { continue; }

        addr = (f.id == UDS_ID_FUNC_REQ) ? UDS_ADDR_FUNCTIONAL : UDS_ADDR_PHYSICAL;

        switch (f.data[0] & PCI_TYPE_MASK) {
            case PCI_SF: tp_handle_sf(addr, f.data, f.len); break;
            case PCI_FF: tp_handle_ff(addr, f.data, f.len); break;
            case PCI_CF: tp_handle_cf(f.data, f.len);       break;
            case PCI_FC: tp_handle_fc(f.data, f.len);       break;
            default: break;
        }
    }

    now = HAL_GetTickMs();

    /* --- 2) 接收超时 N_Cr --- */
    if (s_rx_state == RX_WAIT_CF) {
        if ((u32)(now - s_rx_timer) > UDS_N_CR_MS) {
            tp_rx_reset();
        }
    }

    /* --- 3) 发送超时 N_Bs --- */
    if (s_tx_state == TX_WAIT_FC) {
        if ((u32)(now - s_tx_timer) > UDS_N_BS_MS) {
            tp_tx_reset();
        }
    }

    /* --- 4) 推进 CF 连发 ---
     * STmin = 0 时一个 tick 内连发多帧，否则 19 字节响应要拖 3ms。
     * 上限 8 帧，避免长报文把 1ms 任务撑爆。 */
    {
        u8  burst = 0U;
        u16 before;

        while ((s_tx_state == TX_SEND_CF) && (burst < 8U)) {
            before = s_tx_pos;
            tp_tx_advance();

            if (s_tx_pos == before) { break; }   /* STmin 未到 / 发送忙 */
            if (s_tx_stmin != 0U)   { break; }   /* 有间隔要求，本 tick 只发一帧 */
            burst++;
        }
    }
}
