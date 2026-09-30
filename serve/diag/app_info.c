/**
 * @file    app_info.c
 * @brief   App 信息块 —— 固定放在 0x8400
 *
 * Bootloader 直接按绝对地址读取本结构，用于：
 *   1. 回答 0x22 的四个标识 DID（流程步骤 7~10 / 29~32）
 *      —— 刷写期间 ECU 停在 Boot 里，这几个 DID 必须由 Boot 回答
 *   2. 步骤 23 的依赖检查 31 01 FF 01
 *      —— 校验 magic 与 signature，通过后才写刷写记录、App 才算有效
 *
 * ── 修改须知 ────────────────────────────────────────────────────────────
 *  字符串长度必须与数组声明完全相等。C 语言允许字符数组初始化时丢弃
 *  结尾的 '\0'，所以 16 字节数组要正好填 16 个字符，多一个就编译报错。
 *  不足的位数用空格补齐，不要用 '\0'（诊断仪按定长读取）。
 *
 *  swVersion / hwVersion 对外只返回前 10 字节（UDS_LEN_OEM_SW_VER /
 *  UDS_LEN_OEM_HW_VER = 10），数组仍按 16 字节声明是为了跟其余字段
 *  对齐到统一步长，多出的 6 字节不会被读取但必须填满以满足初始化规则。
 *
 *  每次发布量产版本时，swVersion / swNumber 需要同步更新，并且要与
 *  Bootloader 侧步骤 23 的 App 大小/CRC 依赖检查数据保持配套关系
 *  （由产线/CI 在刷写记录阶段处理，本文件本身不参与该校验）。
 */

#include "boot_app_if.h"

#pragma location = 0x00008400
__root const AppInfo_t g_appInfo = {
    .magic       = APP_INFO_MAGIC,
    .infoVersion = 1UL,

    /*                  1234567890123456   ← 必须正好 16 字符 */
    .swVersion   = "SW01000001      ",   /* F189，对外返回前 10 字节 */
    .swNumber    = "SN01000000000001",   /* F188，对外返回全部 16 字节 */
    .hwVersion   = "HW01000001      ",   /* F191，对外返回前 10 字节 */
    .partNumber  = "PN01000000000001",   /* F187，对外返回全部 16 字节 */
    .signature   = APP_SIGNATURE_TEXT,   /* 必须与 Boot 侧完全一致 */

    .reserved    = { 0UL, 0UL, 0UL, 0UL, 0UL, 0UL },
};
