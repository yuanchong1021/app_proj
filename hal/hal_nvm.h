/**
 * @file    hal_nvm.h
 * @brief   非易失存储硬件抽象层接口
 *
 * 包含两套独立的 NVM 后端：
 *
 * 1. 片内 DFlash（乒乓扇区，16 字节）
 *    - HAL_NVM_Write / HAL_NVM_Read
 *    - 用于里程数据（ODO/Trip），上层通过 nvm_storage 调用
 *
 * 2. 外部 BL24C08F I2C EEPROM（1024 字节）
 *    - HAL_NVM_EE_Write / HAL_NVM_EE_Read
 *    - 用于 UDS 诊断数据持久化（DTC / DID / 统计计数等）
 *    - 地址规划详见 hal_eeprom_bl24c08f.h 中的 EE_ZONE_* 定义
 */

#ifndef CLUSTER_HAL_NVM_H
#define CLUSTER_HAL_NVM_H

#include "types.h"

/* ═══════════════════════════════════════════════════════════
 *  一、片内 DFlash（原有接口，保持不变）
 *  后端：YTM32B1MC0 EFM 乒乓扇区，容量 16 字节，地址 0x0~0xF
 * ═══════════════════════════════════════════════════════════ */

/**
 * @brief 向片内 DFlash 写入数据（乒乓扇区保护）
 *
 * @param addr  NVM 内部地址，范围 [0, 15]
 * @param data  数据指针
 * @param len   写入字节数
 * @return true = 成功
 */
bool HAL_NVM_Write(u16 addr, const u8 *data, u8 len);

/**
 * @brief 从片内 DFlash 读取数据
 *
 * @param addr     NVM 内部地址，范围 [0, 15]
 * @param[out] data 接收缓冲区
 * @param len      读取字节数
 * @return true = 成功
 */
bool HAL_NVM_Read(u16 addr, u8 *data, u8 len);

/* ═══════════════════════════════════════════════════════════
 *  二、外部 BL24C08F EEPROM（新增接口）
 *  后端：I2C1 上的 BL24C08F，容量 1024 字节，地址 0x000~0x3FF
 *
 *  地址分区（见 hal_eeprom_bl24c08f.h EE_ZONE_* 宏）：
 *    0x000~0x07F  DTC 故障码（32条 × 4字节）
 *    0x080~0x0FF  系统参数（VIN / ECU编号 / SW版本）
 *    0x100~0x17F  统计计数（ODO备份 / 点火次数 / 运行小时）
 *    0x180~0x1FF  UDS 快照（冻结帧 / DID 持久化数据）
 *    0x200~0x3FF  扩展预留
 * ═══════════════════════════════════════════════════════════ */

/**
 * @brief  初始化并检测 BL24C08F EEPROM
 *
 * 应在 Board_Init（I2C1 初始化）之后、其余 NVM_EE 调用之前调用一次。
 *
 * @return true 芯片在线，false 通信失败（检查供电/接线）
 */
bool HAL_NVM_EE_Init(void);

/**
 * @brief  向 BL24C08F EEPROM 写入数据
 *
 * 驱动内部自动处理页边界拆分和 ACK Polling 写周期等待，
 * 调用方无需关心芯片细节。
 *
 * @param addr   EEPROM 字地址，范围 [0, 1023]
 * @param data   数据指针
 * @param len    写入字节数
 * @return true = 成功
 */
bool HAL_NVM_EE_Write(u16 addr, const u8 *data, u16 len);

/**
 * @brief  从 BL24C08F EEPROM 读取数据
 *
 * @param addr   EEPROM 字地址，范围 [0, 1023]
 * @param[out] data 接收缓冲区
 * @param len    读取字节数
 * @return true = 成功
 */
bool HAL_NVM_EE_Read(u16 addr, u8 *data, u16 len);

/* ── UDS 专用便捷接口（基于 EE_ZONE_* 地址分区） ─── */

/**
 * @brief  存储 DTC 条目
 *
 * 每个 DTC 条目 4 字节：[dtcHigh, dtcLow, statusMask, occurrCount]
 * 存储地址 = EE_ZONE_DTC_BASE + index × 4
 *
 * @param index   DTC 槽号，范围 [0, 31]
 * @param entry   4 字节 DTC 条目数组指针
 * @return true = 成功
 */
bool HAL_NVM_EE_StoreDTC(u8 index, const u8 *entry);

/**
 * @brief  读取 DTC 条目
 *
 * @param index   DTC 槽号，范围 [0, 31]
 * @param[out] entry 4 字节接收缓冲区
 * @return true = 成功
 */
bool HAL_NVM_EE_ReadDTC(u8 index, u8 *entry);

/**
 * @brief  清除所有 DTC（将 DTC 区填充 0xFF）
 *
 * @return true = 成功
 */
bool HAL_NVM_EE_ClearAllDTC(void);

#endif /* CLUSTER_HAL_NVM_H */
