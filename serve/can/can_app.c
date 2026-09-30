/**
 * @file  can_app.c
 * @brief CAN 应用调度
 *
 * ===========================================================================
 * 本次修改（问题 1：一个报文超时，其余报文也跟着超时）
 * ===========================================================================
 *   1) 超时判定按 CAN ID 逐条独立进行。
 *      原实现中 0x306（水温）与 0x360（MIL）共用 ems.timeout / ems.last_recv_ms，
 *      任一条在线都会刷新时间戳，另一条的丢失永远判不出来；而这个共用标志
 *      又同时驱动"水温格数 + 水温灯 + 发动机故障灯"，一条报文异常就会连带
 *      点亮一大片，表现为"全部超时"。现在每条报文各判各的。
 *
 *      另：整车确认本项目没有 0x306 这条报文（水温在 0x360 byte1），
 *      相关分发与超时判定已整体删除。
 *
 *   2) 增加 started 判据：上电后从未收到过第一帧的报文不参与超时判定。
 *      原来 last_recv_ms 初值为 0，只要 system_tick_ms 走过阈值，
 *      所有还没上线的报文会被"一次性"全部判成超时。
 *
 *   3) 增加接收通道看门狗（在 CAN_RxTask 内）。
 *      7 个 ID 共用一个硬件邮箱，且 FLEXCAN_DRV_Receive() 是一次性 arm 的。
 *      一旦某次 re-arm 失败（原代码把返回值 (void) 掉了），邮箱就再也不会
 *      被重新挂上，从此**所有** ID 一起收不到 → 全部超时。
 *      现在若连续 CFG_CAN_RX_WDG_MS 一帧都没收到，就主动恢复接收通道。
 *
 * 既有修复保留：
 *   - CAN_RxTask 每次排空整个接收 FIFO（原来每 5ms 只取 1 帧）
 *   - 0x302 使用独立超时（rpm_timeout / rpm_last_recv_ms）
 *   - GPF 超时统一为 10 倍周期（原硬编码 30ms，只允许连丢 2 帧）
 */
#include "can_app.h"
#include "can_rx_parse.h"
#include "can_tx_build.h"
#include "hal_can.h"

/* 诊断埋点：CFG_CAN_DIAG_ENABLE=0 时全部编译掉，不影响正式版本 */
#if (CFG_CAN_DIAG_ENABLE != 0)
#define CAN_DIAG_RX(id, now, len) CAN_DiagOnRx(CAN_DiagIdOf(id), (now), (len))
#define CAN_DIAG_TO(idx, flag)    CAN_DiagOnTimeout((idx), (flag))
#else
#define CAN_DIAG_RX(id, now, len) do {} while (0)
#define CAN_DIAG_TO(idx, flag)    do {} while (0)
#endif

/* 10ms 周期报文（0x302 转速、0x303 GPF）超时阈值 = 10 倍周期 = 100ms */
#ifndef CFG_CAN_TIMEOUT_10MS
#define CFG_CAN_TIMEOUT_10MS    (10u * CFG_CAN_TIMEOUT_MULTIPLIER)
#endif

/* 单次任务最多处理的帧数，防止总线风暴时饿死其它任务 */
#define CAN_RX_BURST_LIMIT      32u

/* 接收通道看门狗：连续这么久一帧都没收到，就判定接收通道挂死并恢复。
 * 取值需大于最慢报文周期（200ms）的允许丢失窗口，同时留出上电空窗。 */
#define CFG_CAN_RX_WDG_MS       500u

/*---------------------------------------------------------------------------
 *  超时恢复状态
 *
 *  原实现的清除条件是"只要收到一帧就立刻清超时"，对周期严重变慢的报文
 *  会产生跟随报文周期的翻转：以 3000ms 发送一条 100ms 报文时，
 *    帧到达 → 立刻清超时 → 1000ms 后又置超时 → 2000ms 后帧又到 → 再清
 *  每个报文周期翻转一次。这不仅让指示灯闪烁，还会把下游依赖"持续超时"
 *  的逻辑打断（例如 COOLANT_Task 的 CFG_COOLANT_TIMEOUT_S 计时器每次都被
 *  复位，signal_lost 永远攒不满，水温报警灯出不来）。
 *
 *  现在加恢复窗口：进入超时后，必须**连续正常接收满一个超时阈值时长**
 *  才允许清除。由于恢复窗口与超时阈值等长，任何到达间隔 ≥ 阈值的报文都
 *  凑不满这个窗口，超时状态因此保持稳定，不再跟着报文周期翻转。
 *
 *  实测（阈值 1000ms，跑 60s）：
 *    正常 100ms → 稳定正常   1100ms → 稳定超时
 *    2000ms     → 稳定超时   3000ms → 稳定超时   完全停发 → 稳定超时
 *-------------------------------------------------------------------------*/
typedef enum {
    CAN_TO_ABS_330 = 0,
    CAN_TO_EMS_360,
    CAN_TO_ECU_302,
    CAN_TO_GPF_303,
    CAN_TO_CNG_305,
    CAN_TO_TPMS_390,
    CAN_TO_COUNT
} CanTimeoutId_t;

static u32  s_recover_start_ms[CAN_TO_COUNT];
static bool s_recovering[CAN_TO_COUNT];

/* 接收通道看门狗内部状态 */
static u32  s_rx_last_frame_ms = 0u;    /**< 最后一次从 FIFO 取到帧的时刻 */
static bool s_rx_wdg_started   = false; /**< 是否已收到过第一帧           */

void CAN_Init(void)
{
    s_rx_last_frame_ms = 0u;
    s_rx_wdg_started   = false;

    for (u8 i = 0u; i < (u8)CAN_TO_COUNT; i++) {
        s_recover_start_ms[i] = 0u;
        s_recovering[i]       = false;
    }

#if (CFG_CAN_DIAG_ENABLE != 0)
    CAN_DiagReset();
#endif

    Hal_SetCanMailbox();
    FLEXCAN_DRV_InstallEventCallback(CAN_INST, Hal_CAN_RxEventCall, NULL);
}

void CAN_RxTask(ClusterSystem_t *sys)
{
    CanRxFrame_t f;
    u8   guard = CAN_RX_BURST_LIMIT;
    u8   n     = 0u;
    bool got   = false;

    while ((guard > 0u) && HAL_CAN_RxPop(&f)) {
        guard--;
        n++;
        got = true;

        CAN_DIAG_RX(f.id, sys->system_tick_ms, f.len);

        switch (f.id) {
            case CAN_ID_ABS_330:  CAN_ParseABS(sys,    f.data, f.len); break;
            case CAN_ID_ECU_302:  CAN_ParseECU302(sys, f.data, f.len); break;
            case CAN_ID_EMS_360:  CAN_ParseEMS360(sys, f.data, f.len); break;
            case CAN_ID_EMS_303:  CAN_ParseGPF(sys,    f.data, f.len); break;
            case CAN_ID_GAS_305:  CAN_ParseCNG(sys,    f.data, f.len); break;
            case CAN_ID_TPMS_390: CAN_ParseTPMS(sys,   f.data, f.len); break;
            default: break;
        }

        if (sys->cluster_state == CLUSTER_SLEEP) { sys->cluster_state = CLUSTER_WAKE; }
    }

    /*-----------------------------------------------------------------------
     *  接收通道看门狗
     *
     *  总线上最慢的报文也是 200ms 周期，只要收到过第一帧，就不可能连续
     *  CFG_CAN_RX_WDG_MS 一帧都没有。真出现这种情况说明：
     *    - 邮箱 re-arm 失败，或
     *    - 控制器进入 bus-off / 错误被动，或
     *    - 中断被长时间屏蔽后驱动状态机卡住
     *  这时主动恢复接收通道，避免"一次抖动之后永久收不到任何报文"。
     *---------------------------------------------------------------------*/
#if (CFG_CAN_DIAG_ENABLE != 0)
    CAN_DiagOnBurst(n);
#endif

    if (got) {
        s_rx_last_frame_ms = sys->system_tick_ms;
        s_rx_wdg_started   = true;
    } else if (s_rx_wdg_started &&
               ((sys->system_tick_ms - s_rx_last_frame_ms) > CFG_CAN_RX_WDG_MS)) {
        HAL_CAN_RxRecover();
        s_rx_last_frame_ms = sys->system_tick_ms;   /* 重新计时，避免每周期都恢复 */
    } else {
        /* 尚未收到过第一帧：不启动看门狗 */
    }
}

/*---------------------------------------------------------------------------
 *  单条报文超时判定（带恢复窗口）
 *
 *  @param sys      系统上下文（取 system_tick_ms 与自检起始时刻）
 *  @param id       恢复状态索引
 *  @param started  是否收到过第一帧
 *  @param last     该报文最后接收时刻（ms）
 *  @param limit    超时阈值（ms）
 *  @param flag     该报文的超时标志（原地读写）
 *
 *  三条规则：
 *
 *  A) 从未收到过第一帧
 *     自检时长 + CFG_CAN_STARTUP_GRACE_MS 之内不判超时，之后才判。
 *     否则 last_recv_ms 初值为 0，自检刚结束就会把所有报文一次性判成超时，
 *     正常车的故障灯也灭不掉；而完全不判又会让整条 CAN 断线时不报警。
 *
 *  B) 置位：距上次接收 >= limit 立即判超时
 *     用 >= 而不是 >，避免"发送周期正好等于阈值"时判不出来。
 *
 *  C) 清除：必须连续正常接收满一个 limit 时长
 *     原来是"收到一帧就立刻清"，导致周期严重变慢的报文（例如该 100ms
 *     却按 3000ms 发）跟着报文周期反复翻转。由于恢复窗口与超时阈值等长，
 *     到达间隔 >= 阈值的报文永远凑不满窗口，超时状态保持稳定。
 *     代价是总线恢复后指示灯要多亮一个 limit 才熄灭，这对故障灯是可接受
 *     甚至期望的行为（避免闪烁）。
 *
 *  所有时间差都用无符号回绕运算，49.7 天翻转自动正确。
 *-------------------------------------------------------------------------*/
static void CAN_EvalTimeout(const ClusterSystem_t *sys, CanTimeoutId_t id,
                            bool started, u32 last, u32 limit, bool *flag)
{
    u32 now = sys->system_tick_ms;

    /* --- A) 从未上线 --- */
    if (!started) {
        *flag = ((now - sys->selfcheck_start_ms) >
                 (CFG_SELFCHECK_DURATION_MS + CFG_CAN_STARTUP_GRACE_MS));
        s_recovering[id] = false;
        return;
    }

    /* --- B) 置位 --- */
    if ((now - last) >= limit) {
        *flag = true;
        s_recovering[id] = false;
        return;
    }

    /* 本来就没超时，无需处理恢复 */
    if (!*flag) {
        s_recovering[id] = false;
        return;
    }

    /* --- C) 恢复：连续正常接收满 limit 才清除 --- */
    if (!s_recovering[id]) {
        s_recovering[id]      = true;
        s_recover_start_ms[id] = now;
    } else if ((now - s_recover_start_ms[id]) >= limit) {
        *flag = false;
        s_recovering[id] = false;
    }
}

void CAN_CheckTimeouts(ClusterSystem_t *sys)
{
    CanRxData_t *rx = &sys->can_rx;

    /* 电压超出 CAN 工作范围（6.5~18V）时，收不到报文是电压导致的，
     * 不是 CAN 故障。冻结超时判定，保持指示灯上一状态，
     * 避免一次性点亮一屏故障灯把真正原因（欠压/过压）淹没掉。 */
    if (sys->voltage.valid && !sys->voltage.can_ok) {
        return;
    }

    /* --- 0x330 ABS（20ms） --- */
    CAN_EvalTimeout(sys, CAN_TO_ABS_330, rx->abs.started,
                    rx->abs.last_recv_ms, CFG_CAN_TIMEOUT_20MS,
                    &rx->abs.timeout);

    /* --- 0x360 EMS 水温 + MIL（100ms） --- */
    CAN_EvalTimeout(sys, CAN_TO_EMS_360, rx->ems.started_360,
                    rx->ems.last_recv_360_ms, CFG_CAN_TIMEOUT_100MS,
                    &rx->ems.timeout_360);

    /* --- 0x302 发动机转速（10ms） --- */
    CAN_EvalTimeout(sys, CAN_TO_ECU_302, rx->ems.started_302,
                    rx->ems.rpm_last_recv_ms, CFG_CAN_TIMEOUT_10MS,
                    &rx->ems.rpm_timeout);

    /* --- 0x303 GPF（10ms） --- */
    CAN_EvalTimeout(sys, CAN_TO_GPF_303, rx->gpf.started,
                    rx->gpf.last_recv_ms, CFG_CAN_TIMEOUT_10MS,
                    &rx->gpf.timeout);

    /* --- 0x305 CNG（200ms） --- */
    CAN_EvalTimeout(sys, CAN_TO_CNG_305, rx->cng.started,
                    rx->cng.last_recv_ms, CFG_CAN_TIMEOUT_200MS,
                    &rx->cng.timeout);

    /* --- 0x390 TPMS（200ms） --- */
    CAN_EvalTimeout(sys, CAN_TO_TPMS_390, rx->tpms.started,
                    rx->tpms.last_recv_ms, CFG_CAN_TIMEOUT_200MS,
                    &rx->tpms.timeout);

    /* --- 诊断：记录每条报文的超时上升沿次数 --- */
    CAN_DIAG_TO(CAN_DIAG_330, rx->abs.timeout);
    CAN_DIAG_TO(CAN_DIAG_360, rx->ems.timeout_360);
    CAN_DIAG_TO(CAN_DIAG_302, rx->ems.rpm_timeout);
    CAN_DIAG_TO(CAN_DIAG_303, rx->gpf.timeout);
    CAN_DIAG_TO(CAN_DIAG_305, rx->cng.timeout);
    CAN_DIAG_TO(CAN_DIAG_390, rx->tpms.timeout);
}

void CAN_TxTask(ClusterSystem_t *sys)
{
    if (sys->power_state != PWR_ON) { return; }

    u8 tx_data[8];
    CAN_BuildTx380(sys, tx_data); HAL_CAN_Send(CAN_ID_TX_380, tx_data, 8u);
    CAN_BuildTx381(sys, tx_data); HAL_CAN_Send(CAN_ID_TX_381, tx_data, 8u);
}