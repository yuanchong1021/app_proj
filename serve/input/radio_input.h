/**
 * @file    radio_input.h
 * @brief   SOC → MCU 下行数据接收解析服务接口（UART2 接收）
 *
 * 职责：
 *   1. 从 UART2 接收 SOC 下发的协议帧（V1.1），支持短帧（5B）与长帧（LEN+5B）
 *   2. 按 CMD 分发：
 *      CMD=0x02/0x03 → 里程切换/清零   → sys->lcd.odometer
 *      CMD=0x04      → 蓝牙连接状态    → sys->lcd.radio.bt_connected / band
 *      CMD=0x05      → 北京时间同步    → RTC_SetTime() + sys->lcd.clock
 *      CMD=0x06      → 收音机 FM/AM    → sys->lcd.radio (band/freq_raw/valid)
 *      CMD=0x07      → 音源类型        → sys->lcd.radio.band
 *      CMD=0x08      → 音量值          → sys->lcd.volume
 *      CMD=0x09      → 总里程(ODO)清零 → ODO_RequestClearTotal()
 *                      帧固定为 AA 55 09 5A 53，DATA 必须=0x5A，
 *                      且 200ms 窗口内需收到 2 帧才执行（不可逆操作，防误码）
 *   3. 通信超时检测：连续 1s 无有效帧 → sys->lcd.radio.valid=false（频率区熄灭）
 *   4. 帧间超时保护：收到帧头后 50ms 内未收完整帧 → 丢弃复位
 */

#ifndef CLUSTER_RADIO_INPUT_H
#define CLUSTER_RADIO_INPUT_H

#include "../../app/common/cluster_context.h"

/** 初始化接收解析服务（清缓冲、复位状态机、清 radio 显示数据） */
void RADIO_Init(ClusterSystem_t *sys);

/**
 * @brief SOC 下行接收周期任务（建议 50ms 调用，与 scheduler.c 一致）
 *
 * 流程：
 *   1) 帧间超时保护检查
 *   2) 排空 UART2 接收缓冲，喂帧解析状态机
 *   3) 业务超时累计管理
 */
void RADIO_Task(ClusterSystem_t *sys);

/**
 * @brief UART2 接收回调入口（由 UART2 RX ISR 调用）
 *
 * 将接收到的字节推入环形缓冲，供 RADIO_Task 在任务上下文中解析。
 * @param byte 接收到的一个字节
 */
void SOC_OnUart2Byte(uint8_t byte);

/**
 * @brief 兼容旧命名（等价于 SOC_OnUart2Byte）
 * @deprecated 新代码请使用 SOC_OnUart2Byte
 */
void RADIO_OnUart2Byte(uint8_t byte);

#endif /* CLUSTER_RADIO_INPUT_H */