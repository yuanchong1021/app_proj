/**
 * @file    hal_rtc_ds1307.c
 * @brief   DS1307 RTC 驱动实现（I2C_DRV 标准接口，blocking 模式）
 *
 * ── DS1307 寄存器读写流程 ─────────────────────────────────────────────
 *
 *  写寄存器（I2C write-only 帧）：
 *    [START][0xD0 W][ACK][reg_addr][ACK][data0][ACK]...[dataN][ACK][STOP]
 *    → 将 reg_addr + data 打包到同一个 txBuff，一次 SendData 完成
 *
 *  读寄存器（I2C write-then-read 帧）：
 *    [START][0xD0 W][ACK][reg_addr][ACK]          ← sendStop=false（不发 STOP）
 *    [START][0xD1 R][ACK][data0][ACK]...[dataN][NACK][STOP]  ← ReceiveData
 *    → 先 SendData(sendStop=false) 发寄存器地址，再 ReceiveData(sendStop=true) 读数据
 *      驱动在两次调用之间自动生成 Repeated START，无需手动处理
 *
 * ── BCD 格式说明 ───────────────────────────────────────────────────────
 *
 *  DS1307 所有时间寄存器均为 BCD 编码：
 *    bin 2bcd: 59 → 0x59 = (5<<4)|9
 *    bcd 2bin: 0x59 → 5*10+9 = 59
 *
 *  SECONDS (0x00): [CH | 10SEC[6:4] | SEC[3:0]]  → 屏蔽 bit7 后才是 BCD 秒
 *  HOURS   (0x02): [x | 12/!24 | 10H[5:4] | H[3:0]] → 24h 模式 bit6=0
 */

#include "hal_rtc_ds1307.h"
#include "i2c_driver.h"   /* I2C_DRV_MasterSetSlaveAddr / SendDataBlocking / ReceiveDataBlocking */

/* ═══════════════════════════════════════════════════════════════
 *  内部辅助：DS1307 寄存器连续读写
 * ═══════════════════════════════════════════════════════════════ */

/**
 * @brief 向 DS1307 连续写寄存器
 *
 * 将 [reg_start, data[0], ..., data[len-1]] 作为一个 I2C 写帧发出。
 *
 * @param reg_start 起始寄存器地址
 * @param data      数据缓冲区
 * @param len       数据字节数（不含寄存器地址字节）
 * @return true 成功，false I2C 通信失败（NACK / 超时）
 */
static bool ds1307_write_regs(u8 reg_start, const u8 *data, u8 len)
{
    /* txBuf: [寄存器地址, 数据字节...] 最多 1+7=8 字节（DS1307 0x00~0x06）*/
    u8 tx_buf[8U];
    if (len > 7U) { return false; }   /* 防御：超出 DS1307 寄存器范围 */

    tx_buf[0U] = reg_start;
    for (u8 i = 0U; i < len; i++) {
        tx_buf[i + 1U] = data[i];
    }

    status_t ret = I2C_DRV_MasterSendDataBlocking(
                       DS1307_I2C_INSTANCE,
                       tx_buf,
                       (uint32_t)(len + 1U),
                       true,                   /* sendStop = true：发完后生成 STOP */
                       DS1307_I2C_TIMEOUT_MS);

    return (ret == STATUS_SUCCESS);
}

/**
 * @brief 从 DS1307 连续读寄存器
 *
 * 先发寄存器地址（不含 STOP），再 Repeated START 读数据。
 *
 * @param reg_start 起始寄存器地址
 * @param buf       接收缓冲区
 * @param len       读取字节数
 * @return true 成功，false I2C 通信失败
 */
static bool ds1307_read_regs(u8 reg_start, u8 *buf, u8 len)
{
    /* 第一阶段：发寄存器指针，sendStop=false → 驱动不产生 STOP，总线保持占用 */
    status_t ret = I2C_DRV_MasterSendDataBlocking(
                       DS1307_I2C_INSTANCE,
                       &reg_start,
                       1U,
                       false,                  /* sendStop = false：不发 STOP */
                       DS1307_I2C_TIMEOUT_MS);
    if (ret != STATUS_SUCCESS) { return false; }

    /* 第二阶段：Repeated START + 读数据，sendStop=true → 末字节后产生 NACK + STOP */
    ret = I2C_DRV_MasterReceiveDataBlocking(
              DS1307_I2C_INSTANCE,
              buf,
              (uint32_t)len,
              true,                            /* sendStop = true */
              DS1307_I2C_TIMEOUT_MS);

    return (ret == STATUS_SUCCESS);
}

/* ═══════════════════════════════════════════════════════════════
 *  BCD ↔ 二进制
 * ═══════════════════════════════════════════════════════════════ */

static inline u8 bcd2bin(u8 bcd)
{
    return (u8)(((bcd >> 4U) & 0x0FU) * 10U + (bcd & 0x0FU));
}

static inline u8 bin2bcd(u8 bin)
{
    return (u8)(((bin / 10U) << 4U) | (bin % 10U));
}

/* ═══════════════════════════════════════════════════════════════
 *  公开 API
 * ═══════════════════════════════════════════════════════════════ */

bool HAL_RTC_Init(void)
{
    /* 1. 设置从机地址（驱动内部保存，后续 Send/Receive 自动使用） */
    I2C_DRV_MasterSetSlaveAddr(DS1307_I2C_INSTANCE, DS1307_SLAVE_ADDR, false);

    /* 2. 读 SECONDS 寄存器，检查 CH 位 */
    u8 sec_reg = 0U;
    if (!ds1307_read_regs(DS1307_REG_SECONDS, &sec_reg, 1U)) {
        return false;   /* DS1307 无响应：检查供电 / 晶振 / 上拉电阻 / 引脚复用 */
    }

    if (sec_reg & DS1307_CH_BIT) {
        /*
         * CH=1：振荡器已停止（首次上电或掉电复位）。
         * 规格书："initial power on state of all registers is not defined"
         * → 必须软件写入初值，同时清 CH=0 启动振荡器，配置 24h 模式。
         *
         * 初值：2000-01-01（星期一）00:00:00
         *   buf[0] = SECONDS : 0x00（CH=0, 秒=00）
         *   buf[1] = MINUTES : 0x00（分=00）
         *   buf[2] = HOURS   : 0x00（bit6=0=24h，时=00）
         *   buf[3] = DAY     : 0x01（周一）
         *   buf[4] = DATE    : 0x01（1日）
         *   buf[5] = MONTH   : 0x01（1月）
         *   buf[6] = YEAR    : 0x00（00=2000年）
         */
        const u8 init_buf[7U] = { 0x00U, 0x00U, 0x00U, 0x01U, 0x01U, 0x01U, 0x00U };
        if (!ds1307_write_regs(DS1307_REG_SECONDS, init_buf, 7U)) {
            return false;
        }
    } else {
        /*
         * CH=0：振荡器正常运行（热启动）。
         * 仅检查 HOURS bit6，确保芯片处于 24h 模式。
         * 若芯片之前被配置为 12h 模式，强制切回 24h。
         */
        u8 hour_reg = 0U;
        if (!ds1307_read_regs(DS1307_REG_HOURS, &hour_reg, 1U)) {
            return false;
        }
        if (hour_reg & DS1307_12H_BIT) {
            /* 12h → 24h 换算：去掉 bit6/bit5 标志位，提取 BCD 时，加 PM 偏移 */
            u8 h = bcd2bin(hour_reg & 0x1FU);         /* bit5 在 12h 模式下为 AM/PM */
            if (hour_reg & (1U << 5U)) { h += 12U; }  /* PM：加 12 */
            if (h >= 24U)              { h  = 0U;  }  /* 防溢出 */
            u8 hr_bcd = bin2bcd(h);                    /* bit6=0，24h 模式 */
            if (!ds1307_write_regs(DS1307_REG_HOURS, &hr_bcd, 1U)) {
                return false;
            }
        }
    }

    return true;
}

bool HAL_RTC_ReadTime(u8 *hour, u8 *minute)
{
    /*
     * 连续读 3 字节：SECONDS(0x00) / MINUTES(0x01) / HOURS(0x02)
     *
     * DS1307 在 I2C START 时刻将当前时间锁入影子寄存器，
     * 读操作期间计时不停但读出值一致，无需二次读取。
     */
    u8 buf[3U] = { 0U, 0U, 0U };
    if (!ds1307_read_regs(DS1307_REG_SECONDS, buf, 3U)) {
        return false;
    }

    /*
     * buf[0] = SECONDS：bit7=CH（忽略），低 7 位为 BCD 秒（本函数不返回秒）
     * buf[1] = MINUTES：低 7 位为 BCD 分
     * buf[2] = HOURS  ：24h 模式下低 6 位为 BCD 时（bit6=0）
     */
    *minute = bcd2bin(buf[1U] & 0x7FU);
    *hour   = bcd2bin(buf[2U] & 0x3FU);

    /* 合法性检查（BCD 解码异常保护，例如芯片数据损坏）*/
    if (*hour > 23U || *minute > 59U) {
        return false;
    }
    return true;
}

bool HAL_RTC_WriteTime(u8 hour, u8 minute, u8 second)
{
    if (hour > 23U || minute > 59U || second > 59U) {
        return false;
    }

    /*
     * 写 SECONDS / MINUTES / HOURS 三字节（从 0x00 起）：
     *   SECONDS：bin2bcd(second) → bit7=0（CH=0，振荡器保持运行）
     *   MINUTES：bin2bcd(minute)
     *   HOURS  ：bin2bcd(hour)  → bit6=0（24h 模式，bin2bcd 结果 ≤0x23）
     */
    const u8 buf[3U] = {
        bin2bcd(second),
        bin2bcd(minute),
        bin2bcd(hour),
    };
    return ds1307_write_regs(DS1307_REG_SECONDS, buf, 3U);
}