/**
 * @file    uds_did.c
 * @brief   DID 表
 *
 * 四个标识 DID 全部指向 0x8400 的 g_appInfo，保证 App 运行态
 * （步骤 7~10）与 Boot 驻留态（同样读 0x8400）返回同一份数据。
 * 这一点很关键：流程表步骤 29~32 会在刷写完成后复读这四个 DID
 * 与刷写前比对，两边数据源不一致会直接判失败。
 */

#include "uds_did.h"
#include "uds_cfg.h"
#include "boot_app_if.h"
#include <string.h>

/** 定义在 app_info.c，位于 0x8400 */
extern const AppInfo_t g_appInfo;

/*===========================================================================
 *  DID 表
 *===========================================================================*/
static const UdsDidEntry_t s_didTable[] = {
    /*  DID                   长度                    属性          写安全级  数据指针 */
    { UDS_DID_OEM_SW_VER,   UDS_LEN_OEM_SW_VER,   DID_READONLY, 0U, g_appInfo.swVersion  },
    { UDS_DID_OEM_SW_NUM,   UDS_LEN_OEM_SW_NUM,   DID_READONLY, 0U, g_appInfo.swNumber   },
    { UDS_DID_OEM_HW_VER,   UDS_LEN_OEM_HW_VER,   DID_READONLY, 0U, g_appInfo.hwVersion  },
    { UDS_DID_OEM_PART_NUM, UDS_LEN_OEM_PART_NUM, DID_READONLY, 0U, g_appInfo.partNumber },
};

#define DID_TABLE_COUNT  (u8)(sizeof(s_didTable) / sizeof(s_didTable[0]))

/*===========================================================================
 *  接口实现
 *===========================================================================*/
const UdsDidEntry_t *UDS_DID_Find(u16 did)
{
    u8 i;

    for (i = 0U; i < DID_TABLE_COUNT; i++) {
        if (s_didTable[i].did == did) {
            return &s_didTable[i];
        }
    }
    return NULL;
}

bool UDS_DID_Write(const UdsDidEntry_t *entry, const u8 *data, u8 len)
{
    if ((entry == NULL) || (data == NULL)) { return false; }
    if (entry->attr != DID_READWRITE)      { return false; }
    if (len != entry->len)                 { return false; }

    /* 当前表中全部为只读。新增可写 DID 时在这里分发到 NVM，例如：
     *
     *   if (entry->did == 0xF198U) {
     *       return HAL_EEPROM_Write(EE_ZONE_PARAM_BASE + 0x10U, data, len);
     *   }
     */
    return false;
}
