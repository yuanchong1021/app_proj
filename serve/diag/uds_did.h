/**
 * @file    uds_did.h
 * @brief   数据标识符（DID）表
 */
#ifndef CLUSTER_UDS_DID_H
#define CLUSTER_UDS_DID_H

#include "types.h"

/** DID 读写属性 */
typedef enum {
    DID_READONLY  = 0,   /**< 仅支持 0x22 读取         */
    DID_READWRITE = 1    /**< 同时支持 0x2E 写入       */
} UdsDidAttr_t;

/** DID 表项 */
typedef struct {
    u16          did;    /**< 数据标识符                     */
    u8           len;    /**< 固定长度（字节）               */
    UdsDidAttr_t attr;   /**< 读写属性                       */
    u8           secLvl; /**< 写入所需安全级别，0=不需要      */
    const u8    *ptr;    /**< 数据存放地址（读取直接从此拷贝）*/
} UdsDidEntry_t;

/**
 * @brief 按 DID 查表
 * @return 找到返回表项指针；未定义返回 NULL
 */
const UdsDidEntry_t *UDS_DID_Find(u16 did);

/**
 * @brief 写入一个可写 DID
 * @return true = 写入成功
 *
 * 当前表内全部为只读，返回 false。新增可写 DID 时在 uds_did.c 里
 * 按 entry->did 分发到具体的 NVM/EEPROM 写接口。
 */
bool UDS_DID_Write(const UdsDidEntry_t *entry, const u8 *data, u8 len);

#endif /* CLUSTER_UDS_DID_H */
