/**
 * @file    uds_svc.c
 * @brief   UDS 应用层实现
 *
 * ── 与刷写流程的对应关系 ────────────────────────────────────────────────
 *   步骤 1   10 03            进入扩展会话
 *   步骤 2   85 82            关闭 DTC 记录（抑制响应）
 *   步骤 3   28 83 03         关闭常规收发（抑制响应）
 *   步骤 4   27 01            请求种子（一级安全）
 *   步骤 5   27 02            发送密钥
 *   步骤 6   31 01 DF FD      预编程条件检查
 *   步骤 7   22 F1 89         读 OEM 软件版本号
 *   步骤 8   22 F1 88         读 OEM 软件号
 *   步骤 9   22 F1 91         读 OEM 硬件版本号
 *   步骤 10  22 F1 87         读 OEM 零部件号
 *   步骤 11  10 02            进入编程会话 → 置标志 + 软复位进 Boot
 *   ── 步骤 12~24 由 Bootloader 执行 ──
 *   步骤 25  10 03            回到扩展会话
 *   步骤 26  28 80 03         恢复常规收发
 *   步骤 27  85 81            恢复 DTC 记录
 *   步骤 28  14 FF FF FF      清除全部 DTC
 *   步骤 29~32                复读四个标识 DID
 * ────────────────────────────────────────────────────────────────────────
 *
 * 诊断/编程会话期间调用 LPM_Inhibit(sys, true) 禁止休眠，回到默认会话
 * 时解除——避免诊断仪还在跟 ECU 打交道时仪表自己先睡着。
 */

#include "uds_svc.h"
#include "uds_cfg.h"
#include "uds_tp.h"
#include "uds_did.h"
#include "hal_tick.h"
#include "nvm_storage.h"
#include "voltage.h"
#include "lowpower.h"
#include "sdk_project_config.h"
#include <string.h>

/*===========================================================================
 *  否定响应码
 *===========================================================================*/
#define NRC_GR      (0x10U)  /**< generalReject                       */
#define NRC_SNS     (0x11U)  /**< serviceNotSupported                 */
#define NRC_SFNS    (0x12U)  /**< subFunctionNotSupported             */
#define NRC_IMLOIF  (0x13U)  /**< incorrectMessageLengthOrInvalidFormat */
#define NRC_CNC     (0x22U)  /**< conditionsNotCorrect                */
#define NRC_RSE     (0x24U)  /**< requestSequenceError                */
#define NRC_ROOR    (0x31U)  /**< requestOutOfRange                   */
#define NRC_SAD     (0x33U)  /**< securityAccessDenied                */
#define NRC_IK      (0x35U)  /**< invalidKey                          */
#define NRC_ENOA    (0x36U)  /**< exceedNumberOfAttempts              */
#define NRC_RTDNE   (0x37U)  /**< requiredTimeDelayNotExpired         */
#define NRC_RCRRP   (0x78U)  /**< responsePending                     */
#define NRC_SFNSIAS (0x7EU)  /**< subFunctionNotSupportedInActiveSession */
#define NRC_SNSIAS  (0x7FU)  /**< serviceNotSupportedInActiveSession  */

/*===========================================================================
 *  会话掩码
 *===========================================================================*/
#define SESS_DEF    (0x01U)
#define SESS_PROG   (0x02U)
#define SESS_EXT    (0x04U)
#define SESS_ALL    (SESS_DEF | SESS_PROG | SESS_EXT)

typedef struct {
    u8 sid;
    u8 sessionMask;   /**< 允许的会话 */
    u8 needSecurity;  /**< 1 = 需要一级安全解锁 */
} UdsSvcCfg_t;

static const UdsSvcCfg_t s_svcCfg[] = {
    { 0x10U, SESS_ALL,              0U },
    { 0x11U, SESS_ALL,              0U },
    { 0x14U, SESS_DEF | SESS_EXT,   0U },
    { 0x22U, SESS_DEF | SESS_EXT,   0U },
    { 0x27U, SESS_EXT,              0U },
    { 0x28U, SESS_EXT,              0U },
    { 0x2EU, SESS_EXT,              1U },
    { 0x31U, SESS_EXT,              0U },
    { 0x3EU, SESS_ALL,              0U },
    { 0x85U, SESS_EXT,              0U },
};
#define SVC_CFG_COUNT (u8)(sizeof(s_svcCfg) / sizeof(s_svcCfg[0]))

/*===========================================================================
 *  模块状态
 *===========================================================================*/
static ClusterSystem_t *s_sys = NULL;

static UdsSession_t s_session      = UDS_SESSION_DEFAULT;
static u32          s_s3_timer     = 0U;

static bool s_sec_unlocked  = false;
static bool s_sec_seedValid = false;
static u8   s_sec_seed[UDS_SEC_SEED_LEN];
static u8   s_sec_errCnt    = 0U;
static bool s_sec_locked    = false;
static u32  s_sec_lockTimer = 0U;
static u32  s_sec_rand      = 0U;

static bool s_commEnabled = true;
static bool s_dtcEnabled  = true;

/** 延时动作：收到 10 02 / 11 01 后先把响应发出去再执行 */
typedef enum {
    PEND_NONE = 0,
    PEND_JUMP_BOOT,
    PEND_RESET
} UdsPending_t;

static UdsPending_t s_pending      = PEND_NONE;
static u32          s_pendingTimer = 0U;

static u8 s_resp[UDS_BUF_SIZE];

/*===========================================================================
 *  基础工具
 *===========================================================================*/

static void uds_send_pos(u16 len)
{
    (void)UDS_TP_Send(s_resp, len);
}

static void uds_send_neg(u8 sid, u8 nrc, UdsAddrType_t addr)
{
    u8 buf[3];

    /* 功能寻址时，以下 NRC 不回负响应（ISO 14229-1 §7.5） */
    if (addr == UDS_ADDR_FUNCTIONAL) {
        if ((nrc == NRC_SNS) || (nrc == NRC_SFNS) ||
            (nrc == NRC_ROOR) || (nrc == NRC_SFNSIAS) || (nrc == NRC_SNSIAS)) {
            return;
        }
    }

    buf[0] = 0x7FU;
    buf[1] = sid;
    buf[2] = nrc;
    (void)UDS_TP_Send(buf, 3U);
}

/** 会话切换。切回默认会话时按规范复位安全状态与通信控制，并联动低功耗禁止位 */
static void uds_set_session(UdsSession_t s)
{
    s_session  = s;
    s_s3_timer = HAL_GetTickMs();

    if (s == UDS_SESSION_DEFAULT) {
        s_sec_unlocked  = false;
        s_sec_seedValid = false;
        s_commEnabled   = true;
        s_dtcEnabled    = true;
        if (s_sys != NULL) { LPM_Inhibit(s_sys, false); }
    } else {
        /* 扩展会话 / 编程会话期间禁止休眠，避免诊断过程中仪表自行断电 */
        if (s_sys != NULL) { LPM_Inhibit(s_sys, true); }
    }
}

/*===========================================================================
 *  安全访问算法（占位算法：异或 + 位重排）
 *
 *  与 Bootloader 的 calc_key()（二级安全）完全一致，只是子功能号不同
 *  （App 一级 01/02，Boot 二级 11/12）。若 OEM 提供正式算法，只需替换
 *  本函数，其余重试计数/锁定计时逻辑均不受影响；Boot 侧同名函数需要
 *  同步替换。
 *===========================================================================*/
static void uds_calc_key(const u8 *seed, u8 *key)
{
    u8 calc[UDS_SEC_SEED_LEN];
    const u8 xorMask[UDS_SEC_SEED_LEN] = { 0x65U, 0x67U, 0x77U, 0xE9U };
    u8 i;

    for (i = 0U; i < UDS_SEC_SEED_LEN; i++) {
        calc[i] = (u8)(seed[i] ^ xorMask[i]);
    }

    key[0] = (u8)(((calc[2] & 0x03U) << 6) | ((calc[3] & 0xFCU) >> 2));
    key[1] = (u8)(((calc[3] & 0x03U) << 6) |  (calc[0] & 0x3FU));
    key[2] = (u8)(( calc[0] & 0xFCU)       | ((calc[1] & 0xC0U) >> 6));
    key[3] = (u8)(( calc[1] & 0xFCU)       |  (calc[2] & 0x03U));
}

/** 生成种子：时基 + LCG 扰动，保证每次请求不同 */
static void uds_gen_seed(u8 *seed)
{
    u32 t = HAL_GetTickMs();

    s_sec_rand = (s_sec_rand * 1664525UL) + 1013904223UL + t;

    seed[0] = (u8)( s_sec_rand        ^ (t >> 3));
    seed[1] = (u8)((s_sec_rand >>  8) ^ (t >> 7));
    seed[2] = (u8)((s_sec_rand >> 16) ^ (t >> 11));
    seed[3] = (u8)((s_sec_rand >> 24) ^ (t >> 5));

    /* 全零种子在 UDS 中表示"已解锁"，避免误判 */
    if ((seed[0] | seed[1] | seed[2] | seed[3]) == 0U) {
        seed[0] = 0x5AU;
    }
}

/*===========================================================================
 *  服务实现
 *===========================================================================*/

/* ---- 0x10 会话控制（步骤 1 / 11 / 25）---- */
static void svc_10_session(const u8 *d, u16 len, UdsAddrType_t addr)
{
    u8 sub;
    bool suppress;

    if (len != 2U) { uds_send_neg(0x10U, NRC_IMLOIF, addr); return; }

    suppress = ((d[1] & 0x80U) != 0U);
    sub      = (u8)(d[1] & 0x7FU);

    if ((sub != UDS_SESSION_DEFAULT) &&
        (sub != UDS_SESSION_PROGRAMMING) &&
        (sub != UDS_SESSION_EXTENDED)) {
        uds_send_neg(0x10U, NRC_SFNS, addr);
        return;
    }

    /* 进入编程会话必须已通过一级安全（流程步骤 4~5 在步骤 11 之前） */
    if (sub == UDS_SESSION_PROGRAMMING) {
        if (!s_sec_unlocked) {
            uds_send_neg(0x10U, NRC_SAD, addr);
            return;
        }
        /* 先回 0x78 告知诊断仪"正在处理"，随后复位进 Boot。
         * 正响应 50 02 由 Bootloader 在跳转后补发。 */
        uds_send_neg(0x10U, NRC_RCRRP, addr);
        s_pending      = PEND_JUMP_BOOT;
        s_pendingTimer = HAL_GetTickMs();
        return;
    }

    uds_set_session((UdsSession_t)sub);

    if (suppress) { return; }

    s_resp[0] = 0x50U;
    s_resp[1] = sub;
    s_resp[2] = (u8)((UDS_P2_SERVER_MS >> 8) & 0xFFU);
    s_resp[3] = (u8)( UDS_P2_SERVER_MS       & 0xFFU);
    s_resp[4] = (u8)(((UDS_P2_STAR_MS / 10U) >> 8) & 0xFFU);  /* P2* 单位 10ms */
    s_resp[5] = (u8)( (UDS_P2_STAR_MS / 10U)       & 0xFFU);
    uds_send_pos(6U);
}

/* ---- 0x11 ECU 复位 ---- */
static void svc_11_reset(const u8 *d, u16 len, UdsAddrType_t addr)
{
    u8 sub;
    bool suppress;

    if (len != 2U) { uds_send_neg(0x11U, NRC_IMLOIF, addr); return; }

    suppress = ((d[1] & 0x80U) != 0U);
    sub      = (u8)(d[1] & 0x7FU);

    if ((sub != 0x01U) && (sub != 0x03U)) {   /* 硬复位 / 软复位 */
        uds_send_neg(0x11U, NRC_SFNS, addr);
        return;
    }

    if (!suppress) {
        s_resp[0] = 0x51U;
        s_resp[1] = sub;
        uds_send_pos(2U);
    }

    s_pending      = PEND_RESET;
    s_pendingTimer = HAL_GetTickMs();
}

/* ---- 0x14 清除 DTC（步骤 28）---- */
static void svc_14_clear_dtc(const u8 *d, u16 len, UdsAddrType_t addr)
{
    u32 group;

    if (len != 4U) { uds_send_neg(0x14U, NRC_IMLOIF, addr); return; }

    group = ((u32)d[1] << 16) | ((u32)d[2] << 8) | (u32)d[3];

    /* 目前只支持 0xFFFFFF（全部） */
    if (group != 0x00FFFFFFUL) {
        uds_send_neg(0x14U, NRC_ROOR, addr);
        return;
    }

    if (!UDS_Hook_ClearAllDtc()) {
        uds_send_neg(0x14U, NRC_CNC, addr);
        return;
    }

    s_resp[0] = 0x54U;
    uds_send_pos(1U);
}

/* ---- 0x22 读 DID（步骤 7~10 / 29~32）---- */
static void svc_22_read_did(const u8 *d, u16 len, UdsAddrType_t addr)
{
    u16 pos = 1U;
    u16 outLen = 1U;
    u16 did;
    const UdsDidEntry_t *e;
    bool anyHit = false;

    /* 长度必须是 1 + 2n */
    if ((len < 3U) || (((len - 1U) % 2U) != 0U)) {
        uds_send_neg(0x22U, NRC_IMLOIF, addr);
        return;
    }

    s_resp[0] = 0x62U;

    while (pos < len) {
        did = (u16)(((u16)d[pos] << 8) | d[pos + 1U]);
        pos = (u16)(pos + 2U);

        e = UDS_DID_Find(did);
        if (e == NULL) { continue; }

        if ((u16)(outLen + 2U + e->len) > UDS_BUF_SIZE) {
            uds_send_neg(0x22U, NRC_ROOR, addr);
            return;
        }

        s_resp[outLen]      = (u8)(did >> 8);
        s_resp[outLen + 1U] = (u8)(did & 0xFFU);
        memcpy(&s_resp[outLen + 2U], e->ptr, e->len);
        outLen = (u16)(outLen + 2U + e->len);
        anyHit = true;
    }

    if (!anyHit) {
        uds_send_neg(0x22U, NRC_ROOR, addr);
        return;
    }
    uds_send_pos(outLen);
}

/* ---- 0x27 安全访问（步骤 4~5，一级）---- */
static void svc_27_security(const u8 *d, u16 len, UdsAddrType_t addr)
{
    u8 sub;
    u8 expectKey[UDS_SEC_SEED_LEN];
    u8 i;

    if (len < 2U) { uds_send_neg(0x27U, NRC_IMLOIF, addr); return; }

    sub = (u8)(d[1] & 0x7FU);

    /* 锁定期内一律拒绝 */
    if (s_sec_locked) {
        uds_send_neg(0x27U, NRC_RTDNE, addr);
        return;
    }

    if (sub == UDS_SEC_L1_REQ_SEED) {
        if (len != 2U) { uds_send_neg(0x27U, NRC_IMLOIF, addr); return; }

        s_resp[0] = 0x67U;
        s_resp[1] = sub;

        if (s_sec_unlocked) {
            /* 已解锁：按规范返回全零种子 */
            memset(&s_resp[2], 0, UDS_SEC_SEED_LEN);
        } else {
            uds_gen_seed(s_sec_seed);
            s_sec_seedValid = true;
            memcpy(&s_resp[2], s_sec_seed, UDS_SEC_SEED_LEN);
        }
        uds_send_pos((u16)(2U + UDS_SEC_SEED_LEN));
        return;
    }

    if (sub == UDS_SEC_L1_SEND_KEY) {
        if (len != (u16)(2U + UDS_SEC_SEED_LEN)) {
            uds_send_neg(0x27U, NRC_IMLOIF, addr);
            return;
        }
        if (!s_sec_seedValid) {          /* 没先请求种子 */
            uds_send_neg(0x27U, NRC_RSE, addr);
            return;
        }

        uds_calc_key(s_sec_seed, expectKey);

        for (i = 0U; i < UDS_SEC_SEED_LEN; i++) {
            if (d[2U + i] != expectKey[i]) { break; }
        }

        s_sec_seedValid = false;         /* 种子一次性有效 */

        if (i == UDS_SEC_SEED_LEN) {
            s_sec_unlocked = true;
            s_sec_errCnt   = 0U;
            s_resp[0] = 0x67U;
            s_resp[1] = sub;
            uds_send_pos(2U);
        } else {
            s_sec_errCnt++;
            if (s_sec_errCnt >= UDS_SEC_MAX_RETRY) {
                s_sec_locked    = true;
                s_sec_lockTimer = HAL_GetTickMs();
                s_sec_errCnt    = 0U;
                uds_send_neg(0x27U, NRC_ENOA, addr);
            } else {
                uds_send_neg(0x27U, NRC_IK, addr);
            }
        }
        return;
    }

    uds_send_neg(0x27U, NRC_SFNS, addr);
}

/* ---- 0x28 通信控制（步骤 3 / 26）---- */
static void svc_28_comm_ctrl(const u8 *d, u16 len, UdsAddrType_t addr)
{
    u8 sub;
    bool suppress;

    if (len != 3U) { uds_send_neg(0x28U, NRC_IMLOIF, addr); return; }

    suppress = ((d[1] & 0x80U) != 0U);
    sub      = (u8)(d[1] & 0x7FU);

    switch (sub) {
        case 0x00U: s_commEnabled = true;  break;   /* enableRxAndTx  */
        case 0x03U: s_commEnabled = false; break;   /* disableRxAndTx */
        default:
            uds_send_neg(0x28U, NRC_SFNS, addr);
            return;
    }

    if (suppress) { return; }

    s_resp[0] = 0x68U;
    s_resp[1] = sub;
    uds_send_pos(2U);
}

/* ---- 0x2E 写 DID ---- */
static void svc_2e_write_did(const u8 *d, u16 len, UdsAddrType_t addr)
{
    u16 did;
    const UdsDidEntry_t *e;

    if (len < 4U) { uds_send_neg(0x2EU, NRC_IMLOIF, addr); return; }

    did = (u16)(((u16)d[1] << 8) | d[2]);
    e   = UDS_DID_Find(did);

    if (e == NULL)                  { uds_send_neg(0x2EU, NRC_ROOR, addr);   return; }
    if (e->attr != DID_READWRITE)   { uds_send_neg(0x2EU, NRC_ROOR, addr);   return; }
    if ((u16)(len - 3U) != e->len)  { uds_send_neg(0x2EU, NRC_IMLOIF, addr); return; }

    if (!UDS_DID_Write(e, &d[3], e->len)) {
        uds_send_neg(0x2EU, NRC_CNC, addr);
        return;
    }

    s_resp[0] = 0x6EU;
    s_resp[1] = (u8)(did >> 8);
    s_resp[2] = (u8)(did & 0xFFU);
    uds_send_pos(3U);
}

/* ---- 0x31 例程控制（步骤 6：预编程条件检查）---- */

/** 预编程条件检查结果码 */
#define PRECHK_OK           (0x00U)
#define PRECHK_VOLT_LOW     (0x01U)
#define PRECHK_VOLT_HIGH    (0x02U)
#define PRECHK_SPEED        (0x03U)

static u8 uds_pre_prog_check(void)
{
    u16 mv;

    if (s_sys == NULL) { return PRECHK_OK; }

    mv = VOLT_GetMv(s_sys);
    if (mv < UDS_PROG_VOLT_MIN_MV) { return PRECHK_VOLT_LOW;  }
    if (mv > UDS_PROG_VOLT_MAX_MV) { return PRECHK_VOLT_HIGH; }

    if (s_sys->lcd.speed_kmh > UDS_PROG_SPEED_MAX_KMH) { return PRECHK_SPEED; }

    return PRECHK_OK;
}

static void svc_31_routine(const u8 *d, u16 len, UdsAddrType_t addr)
{
    u8  sub;
    u16 rid;

    if (len < 4U) { uds_send_neg(0x31U, NRC_IMLOIF, addr); return; }

    sub = (u8)(d[1] & 0x7FU);
    rid = (u16)(((u16)d[2] << 8) | d[3]);

    if (sub != 0x01U) {   /* 只支持 startRoutine */
        uds_send_neg(0x31U, NRC_SFNS, addr);
        return;
    }

    if (rid != UDS_RID_PRE_PROG) {
        uds_send_neg(0x31U, NRC_ROOR, addr);
        return;
    }

    if (len != 4U) { uds_send_neg(0x31U, NRC_IMLOIF, addr); return; }

    s_resp[0] = 0x71U;
    s_resp[1] = sub;
    s_resp[2] = (u8)(rid >> 8);
    s_resp[3] = (u8)(rid & 0xFFU);
    s_resp[4] = uds_pre_prog_check();
    uds_send_pos(5U);
}

/* ---- 0x3E 保持连接 ---- */
static void svc_3e_tester_present(const u8 *d, u16 len, UdsAddrType_t addr)
{
    u8 sub;
    bool suppress;

    if (len != 2U) { uds_send_neg(0x3EU, NRC_IMLOIF, addr); return; }

    suppress = ((d[1] & 0x80U) != 0U);
    sub      = (u8)(d[1] & 0x7FU);

    if (sub != 0x00U) { uds_send_neg(0x3EU, NRC_SFNS, addr); return; }
    if (suppress)     { return; }

    s_resp[0] = 0x7EU;
    s_resp[1] = sub;
    uds_send_pos(2U);
}

/* ---- 0x85 DTC 设置控制（步骤 2 / 27）---- */
static void svc_85_dtc_setting(const u8 *d, u16 len, UdsAddrType_t addr)
{
    u8 sub;
    bool suppress;

    if (len != 2U) { uds_send_neg(0x85U, NRC_IMLOIF, addr); return; }

    suppress = ((d[1] & 0x80U) != 0U);
    sub      = (u8)(d[1] & 0x7FU);

    switch (sub) {
        case 0x01U: s_dtcEnabled = true;  break;   /* on  */
        case 0x02U: s_dtcEnabled = false; break;   /* off */
        default:
            uds_send_neg(0x85U, NRC_SFNS, addr);
            return;
    }

    if (suppress) { return; }

    s_resp[0] = 0xC5U;
    s_resp[1] = sub;
    uds_send_pos(2U);
}

/*===========================================================================
 *  分发
 *===========================================================================*/

static const UdsSvcCfg_t *uds_find_svc(u8 sid)
{
    u8 i;

    for (i = 0U; i < SVC_CFG_COUNT; i++) {
        if (s_svcCfg[i].sid == sid) { return &s_svcCfg[i]; }
    }
    return NULL;
}

void UDS_TP_RxIndication(UdsAddrType_t addrType, const u8 *data, u16 len)
{
    u8 sid;
    u8 sessBit;
    const UdsSvcCfg_t *cfg;

    if ((data == NULL) || (len == 0U)) { return; }

    /* 正在等待跳转 / 复位时不再受理新请求 */
    if (s_pending != PEND_NONE) { return; }

    sid = data[0];

    /* 任何请求都刷新 S3 */
    s_s3_timer = HAL_GetTickMs();

    cfg = uds_find_svc(sid);
    if (cfg == NULL) {
        uds_send_neg(sid, NRC_SNS, addrType);
        return;
    }

    switch (s_session) {
        case UDS_SESSION_PROGRAMMING: sessBit = SESS_PROG; break;
        case UDS_SESSION_EXTENDED:    sessBit = SESS_EXT;  break;
        default:                      sessBit = SESS_DEF;  break;
    }

    if ((cfg->sessionMask & sessBit) == 0U) {
        uds_send_neg(sid, NRC_SNSIAS, addrType);
        return;
    }

    if ((cfg->needSecurity != 0U) && (!s_sec_unlocked)) {
        uds_send_neg(sid, NRC_SAD, addrType);
        return;
    }

    switch (sid) {
        case 0x10U: svc_10_session(data, len, addrType);       break;
        case 0x11U: svc_11_reset(data, len, addrType);         break;
        case 0x14U: svc_14_clear_dtc(data, len, addrType);     break;
        case 0x22U: svc_22_read_did(data, len, addrType);      break;
        case 0x27U: svc_27_security(data, len, addrType);      break;
        case 0x28U: svc_28_comm_ctrl(data, len, addrType);     break;
        case 0x2EU: svc_2e_write_did(data, len, addrType);     break;
        case 0x31U: svc_31_routine(data, len, addrType);       break;
        case 0x3EU: svc_3e_tester_present(data, len, addrType); break;
        case 0x85U: svc_85_dtc_setting(data, len, addrType);   break;
        default:    uds_send_neg(sid, NRC_SNS, addrType);      break;
    }
}

/*===========================================================================
 *  跳转 Bootloader
 *===========================================================================*/
static void uds_jump_to_boot(void)
{
    UDS_TP_Abort();

    /* 落盘里程等易失数据，避免刷写期间掉电丢失 */
    if (s_sys != NULL) { NVM_SaveOdometer(s_sys); }

    /* 置编程请求标志。g_bootShared 定义在 boot_shared.c，链接脚本把它放进
     * .SharedSegment 段（0x20007BF8~0x20007BFF）。该段跨复位保留：
     *   - RamInit0.S 的 0x5A5A5A5A 填充被 RCU->RSSR 的 POR 位门控
     *   - RamInit1.c 里 CopyLayout 中该段是 INIT_POR，只有上电复位才拷初值
     * 所以软复位后 Bootloader 能原样读到这个标志，从而驻留而不跳回 App。 */
    g_bootShared.flag = BOOT_FLAG_PROG_REQUEST;
    __DSB();

    /* 必须是软复位：Boot 侧的复位源判断只认软复位，上电复位与看门狗复位
     * 都会被当作"无编程请求"处理。 */
    SystemSoftwareReset();

    for (;;) { }   /* 不会到达 */
}

/*===========================================================================
 *  对外接口
 *===========================================================================*/

void UDS_Init(ClusterSystem_t *sys)
{
    s_sys = sys;

    UDS_TP_Init();

    s_session       = UDS_SESSION_DEFAULT;
    s_s3_timer      = HAL_GetTickMs();
    s_sec_unlocked  = false;
    s_sec_seedValid = false;
    s_sec_errCnt    = 0U;
    s_sec_locked    = false;
    s_sec_rand      = HAL_GetTickMs() ^ 0xA5A5A5A5UL;
    s_commEnabled   = true;
    s_dtcEnabled    = true;
    s_pending       = PEND_NONE;
}

void UDS_TpTask(ClusterSystem_t *sys)
{
    (void)sys;
    UDS_TP_Task();
}

void UDS_Task(ClusterSystem_t *sys)
{
    u32 now = HAL_GetTickMs();

    (void)sys;

    /* --- 安全访问锁定计时 --- */
    if (s_sec_locked) {
        if ((u32)(now - s_sec_lockTimer) >= UDS_SEC_LOCK_MS) {
            s_sec_locked = false;
            s_sec_errCnt = 0U;
        }
    }

    /* --- S3 超时：回默认会话 --- */
    if (s_session != UDS_SESSION_DEFAULT) {
        if ((u32)(now - s_s3_timer) >= UDS_S3_SERVER_MS) {
            uds_set_session(UDS_SESSION_DEFAULT);
        }
    }

    /* --- 延时动作：等响应上了总线再复位 ---
     * 固定延时而非只看 TX 空闲，因为 FlexCAN 发送完成还有总线仲裁时间，
     * 复位太快会让诊断仪收不到 0x78 / 51 01。 */
    if (s_pending != PEND_NONE) {
        if ((u32)(now - s_pendingTimer) >= UDS_JUMP_DELAY_MS) {
            UdsPending_t act = s_pending;
            s_pending = PEND_NONE;   /* 先清除，避免复位失败时反复重入 */

            if (act == PEND_JUMP_BOOT) {
                uds_jump_to_boot();
            } else {
                SystemSoftwareReset();
            }
        }
    }
}

UdsSession_t UDS_GetSession(void)      { return s_session; }
bool UDS_IsNormalCommEnabled(void)     { return s_commEnabled; }
bool UDS_IsDtcRecordingEnabled(void)   { return s_dtcEnabled; }

/*===========================================================================
 *  默认钩子实现
 *
 *  项目里做好故障诊断/DTC 模块后，把本函数移到该模块中实现强版本，
 *  并删除这里的弱实现。
 *===========================================================================*/
__weak bool UDS_Hook_ClearAllDtc(void)
{
    return true;
}