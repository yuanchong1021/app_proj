/**
 * @file    hal_eeprom_bl24c08f.c
 * @brief   BL24C08F I2C EEPROM 驱动实现（I2C_DRV 标准接口，blocking 模式）
 *
 * ── BL24C08F 10-bit 字地址拆分规则 ────────────────────────────────────────
 *
 *  设备地址帧（7-bit）：[1][0][1][0][A2][B9][B8]
 *    A2  ：硬件引脚电平（BL24C08F_A2_LEVEL）
 *    B9B8：字地址 addr[9:8]，嵌入设备地址 bit[1:0]
 *
 *  单次 I2C 帧携带的 Word Address 字节 = addr[7:0]
 *
 *  示例（A2=0）：
 *    addr=0x000 → dev=0x50, waddr=0x00
 *    addr=0x100 → dev=0x51, waddr=0x00
 *    addr=0x280 → dev=0x52, waddr=0x80
 *    addr=0x3FF → dev=0x53, waddr=0xFF
 *
 * ── 写操作帧格式 ──────────────────────────────────────────────────────────
 *
 *  [START][devAddr W][ACK][waddr_byte][ACK][data0][ACK]...[dataN][ACK][STOP]
 *  → 将 waddr_byte + data 合并到 txBuf，一次 SendData 完成（与 DS1307 一致）
 *
 * ── 读操作帧格式（Random Read + Sequential）──────────────────────────────
 *
 *  第一阶段：[START][devAddr W][ACK][waddr_byte][ACK]   （sendStop=false）
 *  第二阶段：[START][devAddr R][ACK][data0][ACK]...[dataN][NACK][STOP]
 *  → 两阶段之间驱动自动产生 Repeated START
 *
 * ── ACK Polling（写周期等待）─────────────────────────────────────────────
 *
 *  BL24C08F 内部写周期最长 3ms，写周期内芯片不响应 I2C 寻址（返回 NACK）。
 *  通过循环发送设备地址检测 ACK/NACK 来等待写完成，无需固定延时。
 *
 * ── 页写约束 ─────────────────────────────────────────────────────────────
 *
 *  BL24C08F 页写时，字地址低 4 位在芯片内部自动递增，但不越过页边界：
 *  当内部地址到达页末（低 4 位=0xF），下一字节回绕到同页页首（低 4 位=0x0）。
 *  → HAL_EEPROM_Write 内部自动按页边界拆分，确保每次 eeprom_write_page
 *    的起始地址 + 长度 ≤ 该页末尾地址。
 */

#include "hal_eeprom_bl24c08f.h"
#include "i2c_driver.h"   /* I2C_DRV_MasterSetSlaveAddr / Send / Receive Blocking */

/* ══════════════════════════════════════════════════════════════════
 *  内部辅助：地址计算
 * ══════════════════════════════════════════════════════════════════ */

/**
 * @brief  由 10-bit 字地址计算 7-bit 设备地址
 *         addr[9:8] 嵌入设备地址 bit[1:0]
 */
static inline u8 dev_addr_of(u16 addr)
{
    return (u8)(BL24C08F_BASE_ADDR | (u8)((addr >> 8U) & 0x03U));
}

/**
 * @brief  当前页末尾前还可以写多少字节（不含当前字节）
 *         页大小 16，低 4 位决定页内偏移
 */
static inline u8 page_remain(u16 addr)
{
    return (u8)(BL24C08F_PAGE_SIZE - (u8)(addr & (BL24C08F_PAGE_SIZE - 1U)));
}

/* ══════════════════════════════════════════════════════════════════
 *  内部辅助：单次页写（不跨页，len ≤ 16）
 * ══════════════════════════════════════════════════════════════════ */

/**
 * @brief  向 EEPROM 写入 [1, 16] 字节（调用方保证不跨页）
 *
 * txBuf 布局：[waddr_byte, data0, data1, ..., dataN]
 * 一次 SendData 完成地址 + 数据的发送，与 DS1307 写寄存器方式完全一致。
 *
 * @param addr  10-bit 字地址
 * @param data  数据指针
 * @param len   字节数 [1, BL24C08F_PAGE_SIZE]
 * @return true 成功，false I2C 通信失败
 */
static bool eeprom_write_page(u16 addr, const u8 *data, u8 len)
{
    /* txBuf: [Word Address(1B), Data(1~16B)]，最多 17 字节 */
    u8 tx_buf[1U + BL24C08F_PAGE_SIZE];

    tx_buf[0U] = (u8)(addr & 0xFFU);           /* Word Address 低 8 位 */
    for (u8 i = 0U; i < len; i++) {
        tx_buf[1U + i] = data[i];
    }

    /* 设置从机地址：addr[9:8] 决定设备地址（B9B8 字段） */
    I2C_DRV_MasterSetSlaveAddr(BL24C08F_I2C_INSTANCE, dev_addr_of(addr), false);

    status_t ret = I2C_DRV_MasterSendDataBlocking(
                       BL24C08F_I2C_INSTANCE,
                       tx_buf,
                       (uint32_t)(1U + len),
                       true,                     /* sendStop = true */
                       BL24C08F_I2C_TIMEOUT_MS);

    return (ret == STATUS_SUCCESS);
}

/* ══════════════════════════════════════════════════════════════════
 *  内部辅助：ACK Polling（等待内部写周期完成）
 * ══════════════════════════════════════════════════════════════════ */

/**
 * @brief  轮询 EEPROM 直至写周期完成（ACK 应答）
 *
 * BL24C08F 在内部写周期（tWR ≤ 3ms）内对 I2C 寻址返回 NACK。
 * 循环发送 0 字节数据帧（仅设备地址），通过 ACK/NACK 判断是否完成。
 *
 * @param addr  任意属于该设备的字地址（用于计算设备地址）
 * @return true 写周期已完成，false 超时（EEPROM 异常）
 */
static bool eeprom_wait_ready(u16 addr)
{
    u8 dummy = 0U;
    u8 retry = 0U;

    I2C_DRV_MasterSetSlaveAddr(BL24C08F_I2C_INSTANCE, dev_addr_of(addr), false);

    do {
        /* 短暂等待后重试：每次等 1ms，最多 6 次（覆盖 3ms 写周期留有余量）*/
        for (volatile u16 d = 0U; d < 8000U; d++) { /* ~1ms @80MHz */ }

        /*
         * 发送 1 字节哑数据帧，用以触发一次 START+地址帧。
         * 若 EEPROM 写周期已结束 → 返回 ACK → STATUS_SUCCESS。
         * 若仍在写周期中       → 返回 NACK → STATUS_I2C_RECEIVED_NACK。
         */
        status_t ret = I2C_DRV_MasterSendDataBlocking(
                           BL24C08F_I2C_INSTANCE,
                           &dummy,
                           1U,
                           true,
                           BL24C08F_I2C_TIMEOUT_MS);

        if (ret == STATUS_SUCCESS) {
            return true;
        }
        retry++;
    } while (retry < BL24C08F_WRITE_POLL_MAX);

    return false;   /* 超时：芯片未响应，检查电源或总线 */
}

/* ══════════════════════════════════════════════════════════════════
 *  公开 API
 * ══════════════════════════════════════════════════════════════════ */

bool HAL_EEPROM_Init(void)
{
    /*
     * 向所有可能的设备地址（B9B8=00~11）各发一次 ACK 探测：
     *   - 地址 0x000（devAddr=BL24C08F_BASE_ADDR）发送设备地址帧
     *   - 若芯片存在且未处于写周期，将返回 ACK
     * 只要基础地址能响应，即认为芯片在线。
     */
    u8 dummy = 0U;
    I2C_DRV_MasterSetSlaveAddr(BL24C08F_I2C_INSTANCE, BL24C08F_BASE_ADDR, false);

    status_t ret = I2C_DRV_MasterSendDataBlocking(
                       BL24C08F_I2C_INSTANCE,
                       &dummy,
                       1U,
                       true,
                       BL24C08F_I2C_TIMEOUT_MS);

    return (ret == STATUS_SUCCESS);
}

bool HAL_EEPROM_Write(u16 addr, const u8 *data, u16 len)
{
    /* 参数合法性检查 */
    if ((data == NULL) || (len == 0U)) {
        return false;
    }
    if ((u32)addr + (u32)len > (u32)BL24C08F_TOTAL_BYTES) {
        return false;   /* 地址越界 */
    }

    u16        cur_addr = addr;
    const u8  *cur_data = data;
    u16        remaining = len;

    while (remaining > 0U)
    {
        /*
         * 计算本次允许写入的最大字节数：
         *   - 不超过当前页剩余空间（保证不跨页）
         *   - 不超过 remaining
         *   - 不超过 BL24C08F_PAGE_SIZE（硬件限制）
         */
        u8 chunk = page_remain(cur_addr);
        if ((u16)chunk > remaining) {
            chunk = (u8)remaining;
        }

        if (!eeprom_write_page(cur_addr, cur_data, chunk)) {
            return false;
        }

        /*
         * ACK Polling：等待当前页写完成后再继续下一页。
         * 跨越 256 字节边界时设备地址变化，使用 cur_addr 计算正确的设备地址。
         */
        if (!eeprom_wait_ready(cur_addr)) {
            return false;
        }

        cur_addr  += (u16)chunk;
        cur_data  += chunk;
        remaining -= (u16)chunk;
    }

    return true;
}

bool HAL_EEPROM_Read(u16 addr, u8 *data, u16 len)
{
    /* 参数合法性检查 */
    if ((data == NULL) || (len == 0U)) {
        return false;
    }
    if ((u32)addr + (u32)len > (u32)BL24C08F_TOTAL_BYTES) {
        return false;   /* 地址越界 */
    }

    /*
     * BL24C08F 跨 256 字节边界时设备地址不同，需分段读取。
     * 每段长度受限于 256 字节页（即 B9B8 相同的区域），超出则换段。
     */
    u16  cur_addr = addr;
    u8  *cur_buf  = data;
    u16  remaining = len;

    while (remaining > 0U)
    {
        /* 本段在同一设备地址内还剩多少字节（256字节边界对齐） */
        u16 seg_remain = (u16)(256U - (cur_addr & 0xFFU));
        if (seg_remain > remaining) {
            seg_remain = remaining;
        }

        u8 dev_addr   = dev_addr_of(cur_addr);
        u8 waddr_byte = (u8)(cur_addr & 0xFFU);

        /* 第一阶段：发 Word Address（sendStop=false → 不发 STOP，总线保持占用） */
        I2C_DRV_MasterSetSlaveAddr(BL24C08F_I2C_INSTANCE, dev_addr, false);

        status_t ret = I2C_DRV_MasterSendDataBlocking(
                           BL24C08F_I2C_INSTANCE,
                           &waddr_byte,
                           1U,
                           false,                    /* sendStop = false */
                           BL24C08F_I2C_TIMEOUT_MS);
        if (ret != STATUS_SUCCESS) {
            return false;
        }

        /* 第二阶段：Repeated START，读数据，sendStop=true → 末字节 NACK + STOP */
        ret = I2C_DRV_MasterReceiveDataBlocking(
                  BL24C08F_I2C_INSTANCE,
                  cur_buf,
                  (uint32_t)seg_remain,
                  true,                              /* sendStop = true */
                  BL24C08F_I2C_TIMEOUT_MS);
        if (ret != STATUS_SUCCESS) {
            return false;
        }

        cur_addr  += seg_remain;
        cur_buf   += seg_remain;
        remaining -= seg_remain;
    }

    return true;
}
