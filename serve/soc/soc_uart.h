/**
 * @file    soc_uart.h
 * @brief   仪表 → SOC 事件通知接口（UART2 发送）
 *
 * 仪表 MCU 通过 UART2 向 SOC 发送事件通知帧，SOC 根据帧内容播放相应提示音。
 *
 * 通信帧格式（短帧，5 字节）—— 协议 V1.1 §2.1：
 * ┌────────┬────────┬──────┬──────┬──────────┐
 * │ 0xAA   │ 0x55   │ CMD  │ DATA │ CHECKSUM │
 * └────────┴────────┴──────┴──────┴──────────┘
 *   帧头1    帧头2    命令    数据   校验
 *   CHECKSUM = CMD ^ DATA（单字节异或）
 *
 * 已定义命令（MCU → SOC 方向，§3）：
 *   CMD=0x01  转向灯状态通知
 *     DATA: 0x00=关闭  0x01=左转  0x02=右转
 *
 * UART 参数：UART2（实例 2），115200 bps，8N1，轮询发送。
 * 帧长 5 字节，传输时间约 0.45ms，不影响 50ms 调度周期。
 *
 * 状态变化检测：内部维护上一次发送的状态，仅在状态变化时发送，
 * 不会对 SOC 造成周期性重复通知。
 */

#ifndef SOC_UART_H
#define SOC_UART_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief 初始化 SOC UART 通知模块
 *
 * 清零内部状态缓存，使模块在首次调用 NotifyXxx 时强制发送一次当前状态。
 * 须在 Board_Init() 完成（UART2 已初始化）之后、业务任务启动之前调用。
 */
void SOC_UART_Init(void);

/**
 * @brief 通知 SOC 当前转向灯状态（仅状态变化时发送）
 *
 * @param left   左转灯当前是否激活（hw_input.turn_left）
 * @param right  右转灯当前是否激活（hw_input.turn_right）
 *
 * 内部根据 left/right 组合编码为 DATA 字节（协议 §3 CMD=0x01）：
 *   left=false, right=false → DATA=0x00（关闭）
 *   left=true,  right=false → DATA=0x01（左转）
 *   left=false, right=true  → DATA=0x02（右转）
 *   left=true,  right=true  → DATA=0x01（双闪，左转优先，协议未定义）
 *
 * 只有与上次发送的 DATA 不同时，才会实际发送 UART2 帧。
 */
void SOC_UART_NotifyTurnSignal(bool left, bool right);

/**
 * @brief 通知 SOC「仪表已上电就绪」（CMD=0x10，可选功能）
 *
 * 用途：仪表 MCU 复位后，屏上音量等由 SOC 下发的显示数据会全部丢失。
 *       MCU 在初始化完成时主动发一帧就绪通知，SOC 收到后立即补发一次
 *       音量（CMD=0x08）、音源（CMD=0x07）、时间（CMD=0x05）等状态，
 *       避免出现「MCU 重启后音量位一直空白，直到用户下次调音量」的问题。
 *
 * 帧内容：AA 55 10 01 11
 * 若 SOC 端暂不实现该命令，可直接忽略该帧（不影响其它功能）。
 */
void SOC_UART_NotifyMcuReady(void);

#ifdef __cplusplus
}
#endif

#endif /* SOC_UART_H */