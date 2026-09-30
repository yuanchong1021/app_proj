/**
 * @file    nvm_storage.h
 * @brief   非易失存储服务（里程持久化）
 *
 * 本模块是业务层与 odo_log 之间的薄封装：业务层只认识 ClusterSystem_t，
 * odo_log 只认识 u32/u16，转换放在这里。
 *
 * 底层为自研日志式存储（odo_log.c），位于数据 Flash array1，与 Bootloader
 * 刷写记录所在的 array0 物理隔离。60 秒保存周期下，乘用车 10 年工况单扇区
 * 擦除约 1700 次，占 10 万次寿命的 1.7%，余量 58 倍。
 *
 * 详见《里程日志式存储设计说明》。
 */

#ifndef CLUSTER_NVM_STORAGE_H
#define CLUSTER_NVM_STORAGE_H

#include "cluster_context.h"

/**
 * @brief 初始化里程存储，扫描日志区
 *
 * 必须在 PWR_Init() 与 ODO_Init() 之前调用（这两者都会访问 NVM）。
 * 内部为同步操作，返回时即可读写。
 *
 * @return true = 可用
 */
bool NVM_Init(void);

/**
 * @brief 从 NVM 读取里程到 sys->lcd.odometer
 *
 * 首次上电（日志区全空）时把 odo_km / trip_dm 置 0。
 * 数据在 NVM_Init 时已扫描到 RAM，本函数不访问 Flash。
 */
void NVM_LoadOdometer(ClusterSystem_t *sys);

/**
 * @brief 把当前里程写入 NVM
 *
 * 同步阻塞，返回时已落盘。这对 uds_jump_to_boot()（返回后软复位）、
 * lowpower.c 与 pwr_mgr.c（返回后掉电）是必需的。
 *
 * 正常写入约 100 微秒；每 32 次中有一次需先擦扇区，那次额外约 5.8 毫秒。
 */
void NVM_SaveOdometer(const ClusterSystem_t *sys);

#endif /* CLUSTER_NVM_STORAGE_H */