/**
 * @file    hal_eeprom_bl24c08f.h
 * @brief   BL24C08F I2C EEPROM 驱动（基于 I2C_DRV 标准驱动接口）
 *
 * 芯片规格（Belling BL24C08F）：
 *   - I2C 从机，7-bit 地址 1010_A2_B9_B8，最高 1MHz，本工程配置 400kHz
 *   - 容量：8K bits = 1024 Bytes，组织为 64 页 × 16 Bytes/页
 *   - 页写入：单次最多 16 字节，写周期最长 3ms，支持 ACK Polling 检测完成
 *   - 字地址（10-bit）拆分规则：
 *       高 2 位 addr[9:8] → 设备地址字节 bit[1:0]（B9B8 字段）
 *       低 8 位 addr[7:0] → 独立发送的 Word Address 字节
 *   - WP 引脚：接 GND = 允许读写；接 VCC = 全片写保护
 *
 * 硬件资源（Board_Init 已完成初始化，本驱动不做重复配置）：
 *   - I2C 实例：1（与 DS1307 RTC 共用同一 I2C 总线，总线上两从机地址不冲突）
 *   - SDA：PTD0，SCL：PTD1，板级 4.7kΩ 上拉到 VCC
 *   - A2 引脚接 GND（A2=0），7-bit 基础地址 = 0x50
 *   - WP 引脚接 GND（默认允许读写）
 *
 * EEPROM 存储区地址规划（供上层 NVM/UDS 模块使用）：
 *   0x000 ~ 0x07F  (128 B)  DTC 故障码表：32 条 × 4 字节/条
 *   0x080 ~ 0x0FF  (128 B)  系统参数：VIN / ECU 编号 / SW 版本 / 标定
 *   0x100 ~ 0x17F  (128 B)  统计计数：里程 / 点火次数 / 运行小时
 *   0x180 ~ 0x1FF  (128 B)  UDS 数据快照（$19 冻结帧 / $2E DID 持久化）
 *   0x200 ~ 0x3FF  (512 B)  扩展预留
 */

#ifndef CLUSTER_HAL_EEPROM_BL24C08F_H
#define CLUSTER_HAL_EEPROM_BL24C08F_H

#include "types.h"   /* u8, u16, bool */

/* ─── BL24C08F I2C 参数 ────────────────────────────────── */
#define BL24C08F_I2C_INSTANCE    1U      /**< I2C 实例号（与 DS1307 共总线） */
#define BL24C08F_A2_LEVEL        0U      /**< A2 引脚电平：0=接GND, 1=接VCC  */
/** 7-bit 基础从机地址：1010_A2_00（B9B8=0）*/
#define BL24C08F_BASE_ADDR       (0x50U | ((BL24C08F_A2_LEVEL & 0x01U) << 2U))
#define BL24C08F_I2C_TIMEOUT_MS  10U     /**< 单次 I2C 传输超时（ms）        */
#define BL24C08F_WRITE_POLL_MS   1U      /**< ACK Polling 每次等待（ms）      */
#define BL24C08F_WRITE_POLL_MAX  6U      /**< ACK Polling 最大重试次数        */

/* ─── 存储器参数 ────────────────────────────────────────── */
#define BL24C08F_TOTAL_BYTES     1024U   /**< 总容量（字节）                  */
#define BL24C08F_PAGE_SIZE       16U     /**< 页大小（字节）                  */
#define BL24C08F_PAGE_COUNT      64U     /**< 总页数                          */

/* ─── 区域起始地址 ──────────────────────────────────────── */
#define EE_ZONE_DTC_BASE         0x000U  /**< DTC 区起始地址                  */
#define EE_ZONE_DTC_SIZE         128U    /**< DTC 区大小（32 条 DTC）         */
#define EE_ZONE_PARAM_BASE       0x080U  /**< 系统参数区起始地址              */
#define EE_ZONE_PARAM_SIZE       128U    /**< 系统参数区大小                  */
#define EE_ZONE_STAT_BASE        0x100U  /**< 统计计数区起始地址              */
#define EE_ZONE_STAT_SIZE        128U    /**< 统计计数区大小                  */
#define EE_ZONE_SNAP_BASE        0x180U  /**< UDS 快照区起始地址              */
#define EE_ZONE_SNAP_SIZE        128U    /**< UDS 快照区大小                  */
#define EE_ZONE_EXT_BASE         0x200U  /**< 扩展预留区起始地址              */
#define EE_ZONE_EXT_SIZE         512U    /**< 扩展预留区大小                  */

/* ─── 公开 API ──────────────────────────────────────────── */

/**
 * @brief  检测 BL24C08F 是否在线（ACK 检测）
 *
 * 调用时 I2C1 已就绪。通过向基础地址发 START+地址帧，
 * 检测是否收到 ACK 来确认芯片存在。
 *
 * @return true 芯片在线，false 未响应（检查供电/接线/上拉电阻）
 */
bool HAL_EEPROM_Init(void);

/**
 * @brief  向 EEPROM 写入任意长度数据（自动处理页边界拆分）
 *
 * 驱动内部自动将跨页写入拆分为多次页写操作，每次页写后
 * 通过 ACK Polling 等待内部写周期完成（最长 3ms）。
 *
 * @param addr   字地址，范围 [0, 1023]
 * @param data   发送数据缓冲区
 * @param len    写入字节数
 * @return true 全部写入成功，false 参数非法 / I2C 通信失败 / 写超时
 */
bool HAL_EEPROM_Write(u16 addr, const u8 *data, u16 len);

/**
 * @brief  从 EEPROM 顺序读取数据
 *
 * 采用 Random Read 启动（哑写装载地址），随后 Repeated START
 * 转为顺序读取，直至读完 len 字节。读操作无跨页限制。
 *
 * @param addr   字地址，范围 [0, 1023]
 * @param data   接收缓冲区
 * @param len    读取字节数
 * @return true 成功，false 参数非法 / I2C 通信失败
 */
bool HAL_EEPROM_Read(u16 addr, u8 *data, u16 len);

#endif /* CLUSTER_HAL_EEPROM_BL24C08F_H */
