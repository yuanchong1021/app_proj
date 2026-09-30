/**
 * @file    soc_uart.c
 * @brief   仪表 → SOC 事件通知实现（UART2 DMA 发送）
 *
 * 设计说明：
 *   1. 本模块只负责“仪表 → SOC”方向的主动通知，SOC → 仪表方向由
 *      radio_input.c 的状态机负责接收解析；UART TX/RX 使用独立的硬件
 *      移位寄存器与独立 DMA 通道（TX=通道1，RX=通道0），互不干扰。
 *
 *   2. 发送改用 UART_DRV_SendData（异步 DMA 发送，已核对驱动源码）：
 *      uart_config2 为 UART_USING_DMA、txDMAChannel=1；dma_config1 已把
 *      DMA_REQ_UART2_TX 绑定到 DMA 通道1。调用 UART_DRV_SendData() 时，
 *      驱动内部执行 DMA_DRV_ConfigMultiBlockTransfer(通道1, 内存→外设,
 *      源=缓冲区, 目的=&UART2->DATA, 1B, N次) + DMA_DRV_StartChannel(通道1)，
 *      CPU 不再逐字节忙等。5 字节 @115200bps ≈ 0.43ms，远小于 50ms 周期。
 *
 *      对比原 UART_DRV_SendDataPolling：它是寄存器轮询发送，即使 UART 配了
 *      DMA 也不走 DMA。改为 SendData 才是真正的 DMA 发送。
 *
 *   3. s_tx_buf 为静态缓冲区，生命周期覆盖整次 DMA 传输，避免 DMA 读取
 *      栈上已回收内存。发送前用 UART_DRV_GetTransmitStatus 做防御性检查：
 *      若上一帧仍在发送（正常 50ms 周期内必已完成），先中止再发，确保
 *      s_tx_buf 可安全覆写。
 *
 *   4. 状态变化检测（s_last_turn_data）：仅在转向状态变化时发送，防止每
 *      50ms 重复给 SOC 触发提示音。
 *
 * 帧格式（短帧，5 字节）—— 协议 V1.1 §2.1：
 *   [0xAA][0x55][CMD][DATA][CHECKSUM]     CHECKSUM = CMD ^ DATA
 *
 * CMD=0x01 转向灯（MCU → SOC，§3）：DATA 0x00=关闭 0x01=左转 0x02=右转
 */

#include "soc_uart.h"
#include "uart_driver.h"

/*---------------------------------------------------------------------------
 *  内部常量
 *---------------------------------------------------------------------------*/
/** UART2 实例号（与 Board_Init 中 UART_DRV_Init(2, ...) 对应） */
#define SOC_UART_INST       2u

/* 协议 V1.1 §2：通用帧头 HEAD1 = 0xAA  HEAD2 = 0x55 */
#define SOC_FRAME_HDR1      0xAAu   /**< 帧头字节 1 */
#define SOC_FRAME_HDR2      0x55u   /**< 帧头字节 2 */
#define SOC_CMD_TURN        0x01u   /**< 命令：转向灯状态通知（§3 CMD=0x01） */
#define SOC_CMD_MCU_READY   0x10u   /**< 命令：仪表上电就绪通知（可选，见 .h） */
#define SOC_FRAME_LEN       5u      /**< 短帧总长度（字节）                   */

/** DATA 编码（CMD=0x01 时，§3 DATA取值定义） */
#define SOC_TURN_OFF        0x00u   /**< 转向灯关闭  */
#define SOC_TURN_LEFT       0x01u   /**< 左转向灯亮  */
#define SOC_TURN_RIGHT      0x02u   /**< 右转向灯亮  */

/*---------------------------------------------------------------------------
 *  内部状态
 *---------------------------------------------------------------------------*/
/** 上一次发送的转向 DATA；0xFF 表示“未初始化，强制首次发送” */
static uint8_t s_last_turn_data = 0xFFu;

/** DMA TX 静态缓冲区：DMA 传输期间必须保持有效，故为模块级静态变量 */
static uint8_t s_tx_buf[SOC_FRAME_LEN];

/*---------------------------------------------------------------------------
 *  内部函数：组帧并经 UART2 DMA 异步发送
 *---------------------------------------------------------------------------*/
static void soc_send_frame(uint8_t cmd, uint8_t data)
{
    uint32_t bytesRemaining = 0u;

    /* 防御性检查：若上一次 DMA 发送仍在进行（正常 50ms 内必已完成），
     * 先中止再发送，保证 s_tx_buf 可安全覆写。 */
    if (UART_DRV_GetTransmitStatus(SOC_UART_INST, &bytesRemaining) == STATUS_BUSY) {
        (void)UART_DRV_AbortSendingData(SOC_UART_INST);
    }

    /* 填充协议帧 */
    s_tx_buf[0] = SOC_FRAME_HDR1;
    s_tx_buf[1] = SOC_FRAME_HDR2;
    s_tx_buf[2] = cmd;
    s_tx_buf[3] = data;
    s_tx_buf[4] = (uint8_t)(cmd ^ data);   /* CHECKSUM = CMD ^ DATA（§2.1）*/

    /* 异步 DMA 发送：驱动配置 DMA 通道1 并启动传输后立即返回，不阻塞 50ms 周期 */
    (void)UART_DRV_SendData(SOC_UART_INST, s_tx_buf, SOC_FRAME_LEN);
}

/*---------------------------------------------------------------------------
 *  公开 API
 *---------------------------------------------------------------------------*/

void SOC_UART_Init(void)
{
    /* 0xFF：非法 DATA，确保上电后首次调用 NotifyTurnSignal 必然发送。
     * DMA TX 无需额外初始化：Board_Init 中 UART_DRV_Init(2, ...) 已按
     * uart_config2（UART_USING_DMA、txDMAChannel=1）完成绑定。 */
    s_last_turn_data = 0xFFu;
}

void SOC_UART_NotifyMcuReady(void)
{
    /* DATA 固定 0x01；CHECKSUM = 0x10 ^ 0x01 = 0x11。
     * 无状态缓存，每次调用都发送（仅在开机初始化时调用一次）。 */
    soc_send_frame(SOC_CMD_MCU_READY, 0x01u);
}

void SOC_UART_NotifyTurnSignal(bool left, bool right)
{
    uint8_t turn_data;

    /* 协议 §3 CMD=0x01 DATA取值：0x00=关闭 0x01=左转 0x02=右转
     * 协议未定义 0x03（双闪），left&&right 时按“左转优先”处理。 */
    if (left && right) {
        turn_data = SOC_TURN_LEFT;
    } else if (left) {
        turn_data = SOC_TURN_LEFT;
    } else if (right) {
        turn_data = SOC_TURN_RIGHT;
    } else {
        turn_data = SOC_TURN_OFF;
    }

    /* 仅在状态发生变化时发送，避免 50ms 周期重复触发 SOC 提示音 */
    if (turn_data != s_last_turn_data) {
        s_last_turn_data = turn_data;
        soc_send_frame(SOC_CMD_TURN, turn_data);
    }
}