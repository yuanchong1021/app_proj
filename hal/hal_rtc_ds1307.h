/**
 * @file    hal_rtc_ds1307.h
 * @brief   DS1307 RTC 驱动（基于 I2C_DRV 标准驱动接口）
 *
 * 芯片规格（XBLW DS1307）：
 *   - I2C 从机，7-bit 地址 0x68，最大 100kHz
 *   - 寄存器全 BCD 编码
 *   - SECONDS bit7 = CH（Clock Halt）：1=停振，上电初始值未定义，必须软件清零
 *   - HOURS   bit6 = 12/24h 模式：本驱动固定 24h（bit6=0）
 *
 * 硬件资源（调用方已完成初始化，本驱动不做配置）：
 *   - I2C 实例：1（I2C1）
 *   - SDA：PTD0，SCL：PTD1，板级 4.7kΩ 上拉到 VCC
 *   - DS1307 VBAT 接 3V 纽扣电池，X1/X2 接 32.768kHz 晶振
 */

#ifndef CLUSTER_HAL_RTC_DS1307_H
#define CLUSTER_HAL_RTC_DS1307_H

#include "types.h"   /* u8, bool */

/* ─── DS1307 I2C 参数 ──────────────────────────────────── */
#define DS1307_I2C_INSTANCE   1U       /**< I2C 实例号                   */
#define DS1307_SLAVE_ADDR     0x68U    /**< 7-bit 从机地址               */
#define DS1307_I2C_TIMEOUT_MS 10U      /**< 单次传输超时（ms）           */

/* ─── DS1307 寄存器地址 ─────────────────────────────────── */
#define DS1307_REG_SECONDS  0x00U  /**< 秒（bit7=CH，BCD 00~59）        */
#define DS1307_REG_MINUTES  0x01U  /**< 分（BCD 00~59）                 */
#define DS1307_REG_HOURS    0x02U  /**< 时（bit6=12/24h，BCD 00~23）    */
#define DS1307_REG_DAY      0x03U  /**< 星期（1~7）                     */
#define DS1307_REG_DATE     0x04U  /**< 日（BCD 01~31）                 */
#define DS1307_REG_MONTH    0x05U  /**< 月（BCD 01~12）                 */
#define DS1307_REG_YEAR     0x06U  /**< 年（BCD 00~99 = 2000~2099）     */

#define DS1307_CH_BIT       (1U << 7U)  /**< Clock Halt bit              */
#define DS1307_12H_BIT      (1U << 6U)  /**< 12/24h 模式 bit            */

/* ─── 公开 API ──────────────────────────────────────────── */

/**
 * @brief 配置 DS1307 从机地址并初始化时钟寄存器
 *
 * 调用时 I2C1 已就绪。本函数：
 *   1. 调用 I2C_DRV_MasterSetSlaveAddr 设置从机地址 0x68
 *   2. 读 SECONDS 寄存器，判断 CH 位
 *   3. 若 CH=1（首次上电/掉电复位），写入初值 00:00:00，清 CH=0，设 24h 模式
 *   4. 若 CH=0（热启动），仅确保 24h 模式
 *
 * @return true  成功  false  I2C 通信失败（芯片未响应）
 */
bool HAL_RTC_Init(void);

/**
 * @brief 读取当前时/分
 *
 * 从 REG_SECONDS（0x00）连续读 3 字节，解码 MINUTES 和 HOURS。
 *
 * @param[out] hour    0~23
 * @param[out] minute  0~59
 * @return true 成功  false I2C 错误或 BCD 值越界
 */
bool HAL_RTC_ReadTime(u8 *hour, u8 *minute);

/**
 * @brief 写入时间（校时）
 *
 * 写 SECONDS/MINUTES/HOURS 三字节，CH=0 保持振荡器运行，24h 模式不变。
 *
 * @param hour    0~23
 * @param minute  0~59
 * @param second  0~59
 * @return true 成功  false 参数越界或 I2C 错误
 */
bool HAL_RTC_WriteTime(u8 hour, u8 minute, u8 second);

#endif /* CLUSTER_HAL_RTC_DS1307_H */