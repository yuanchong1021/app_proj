/**
 * @file    can_rx_parse.c
 * @brief   CAN 接收报文解析实现
 *
 * 设计要点：
 *   - 所有 ParseXxx 函数都做长度校验，避免 data[N] 越界。
 *   - 信号定位使用统一的 GET_BIT / GET_BITS 宏，对应 CAN 矩阵中
 *     "字节内 LSB 起始位 + 长度" 的描述方式。
 *   - 解析时只触碰对应子结构 (sys->can_rx.xxx)，不直接驱动 LCD/LED；
 *     LCD / LED 状态由 app/ 层的 Task 函数读取后再决定。
 *
 * 字节序约定（重要）：
 *   整车矩阵采用 Motorola（大端）。矩阵"起始位"给出的是信号 **LSB**
 *   的全局 bit 号（全局 bit = 字节号 × 8 + 字节内 bit 号），信号从该位
 *   开始向 **低字节方向** 增长。即多字节信号中，字节号小的存放高位。
 *
 *   例：0x330 车速 起始位 8、长度 13
 *       值 bit0~7  → byte1
 *       值 bit8~12 → byte0 的 bit0~4
 *
 * ===========================================================================
 * 【超时标志所有权】本次修改
 * ===========================================================================
 *   解析函数**只**刷新 started / last_recv_ms 两个"事实"字段，
 *   timeout 标志一律由 can_app.c 的 CAN_EvalTimeout() 独占读写。
 *
 *   原实现在每个 ParseXxx 末尾直接写 `timeout = false`，等于绕过了
 *   CAN_EvalTimeout() 里的恢复窗口（"进入超时后必须连续正常接收满一个
 *   超时阈值时长才允许清除"）——标志在到达 C 分支之前就已经被抹成
 *   false，恢复窗口一次都执行不到。
 *
 *   后果（以 0x360、阈值 1000ms、上位机按 2000ms 发送为例）：
 *     t=0     收帧 → false
 *     t=1000  EvalTimeout 置位 → true
 *     t=2000  收帧 → 被 Parse 直接抹成 false
 *     ...     标志以 50% 占空比反复翻转，判不出稳定超时
 *
 *   下游依赖"持续超时"的逻辑因此全部失效：COOLANT_Task 的
 *   CFG_COOLANT_TIMEOUT_S(20s) 计时器每 2000ms 被复位一次，
 *   signal_lost 永远攒不满，水温格数压不到 1 格，表现为"根本没超时"；
 *   MIL 灯则跟着报文周期闪烁。
 *
 *   改后：0x360 按 2000ms 发送时，收帧不再清标志，EvalTimeout 开恢复
 *   计时后又被下一次"距上次接收 >= 阈值"打断，恢复窗口永远凑不满，
 *   超时状态稳定保持 true。
 *
 *   涉及的 6 条报文：0x330 / 0x302 / 0x360 / 0x303 / 0x305 / 0x390
 * ===========================================================================
 */

#include "can_rx_parse.h"

/*---------------------------------------------------------------------------
 *  位提取宏
 *
 *  GET_BIT(data, byte, bit)         取 data[byte] 的第 bit 位（0 或 1）
 *  GET_BITS(data, byte, bit, len)   取 data[byte] 从 bit 起 len 个位
 *---------------------------------------------------------------------------*/
#define GET_BIT(data, byte, bit)        (((data)[(byte)] >> (bit)) & 0x01u)
#define GET_BITS(data, byte, bit, len)  \
    (((data)[(byte)] >> (bit)) & ((1u << (len)) - 1u))

/*---------------------------------------------------------------------------
 *  信号量程常量（来自整车矩阵）
 *---------------------------------------------------------------------------*/
#define ABS_SPEED_RAW_MAX       0x12C0u   /**< 车速总线最大值 = 270 km/h      */
#define ABS_SPEED_RAW_INIT      0x1FFEu   /**< 车速初始值（上电默认）         */
#define ABS_SPEED_RAW_INVALID   0x1FFFu   /**< 车速无效值                     */

#define EMS_RPM_RAW_INVALID     0xFFFFu   /**< 转速无效值                     */

/* 水温（0x360 byte1）：输入值 0x00~0xFE 有效，对应 -48℃ ~ 142.5℃；0xFF 无效 */
#define EMS_COOLANT_RAW_MAX     0xFEu

/*===========================================================================
 *  ABS 报文 (ID 0x330, 20ms)
 *
 *  矩阵：
 *    vehicle_speed              起始位 8   长度 13  精度 0.05625 km/h
 *                               总线最大 0x12C0  初始 0x1FFE  无效 0x1FFF
 *    ABS_Vehicle speed valid    起始位 16  长度 1   0=Valid  1=Invalid
 *    ABS1_message_counter       起始位 52  长度 4
 *    ABS1_checksum              起始位 56  长度 8
 *
 *  @note abs_fault / ebd_fault 两位未出现在已获取的矩阵截图中，
 *        此处沿用原实现（byte0.bit6 / byte0.bit5），需与完整 DBC 核对。
 *        按 Motorola 解析后车速高位占 byte0.bit0~4，与这两位不冲突。
 *===========================================================================*/
void CAN_ParseABS(ClusterSystem_t *sys, const u8 *data, u8 len)
{
    if (len < 8u) { return; }

    CanABS_t *abs = &sys->can_rx.abs;
    u16 speed_raw;

    /* 车速：LSB 在 bit8(byte1.bit0)，13 位，Motorola → byte0 为高字节
     * 原实现误取 data[2] 作为高位，而 byte2.bit0 是 valid 位，
     * 导致车速高位与 valid 位互相污染 → 车速被判无效恒显示 0。 */
    speed_raw = (u16)data[1] | ((u16)(data[0] & 0x1Fu) << 8u);

    /* 始终保存解析结果，便于区分"没收到"与"收到但被判无效" */
    abs->vehicle_speed_raw = speed_raw;
    abs->speed_valid       = GET_BIT(data, 2, 0);

    /* 初始值 / 无效值 / 超量程一律按无效处理，并保留上一次有效车速 */
    if ((speed_raw > ABS_SPEED_RAW_MAX) ||
        (speed_raw == ABS_SPEED_RAW_INIT) ||
        (speed_raw == ABS_SPEED_RAW_INVALID)) {
        abs->speed_valid = 0u;
    } else if (abs->speed_valid == 1u) {
        abs->vehicle_speed_raw = speed_raw;
    } else {
        /* valid=0：不更新原始值 */
    }

    abs->abs_fault   = GET_BIT(data, 0, 6);       /* 待与完整 DBC 核对 */
    abs->ebd_fault   = GET_BIT(data, 0, 5);       /* 待与完整 DBC 核对 */

    abs->msg_counter = GET_BITS(data, 6, 4, 4);   /* 起始位 52 → byte6.bit4~7 */
    abs->checksum    = data[7];                   /* 起始位 56 → byte7        */

    /* 超时标志不在此清除，见文件头【超时标志所有权】说明 */
    abs->started      = true;
    abs->last_recv_ms = sys->system_tick_ms;
}

/*===========================================================================
 *  发动机转速报文 (ID 0x302, 10ms)
 *
 *  矩阵：
 *    发动机转速故障   起始字节 0  起始位 2   长度 1   0=No error 1=Error
 *    发动机转速       起始字节 2  起始位 16  长度 16  精度 0.25 r/min
 *                     总线最大 0xFFFE  初始 0x0000  无效 0xFFFF
 *
 *  @note 本报文为 **整车下发**（原 cfg_can.h 中注释为"仪表外发"有误）。
 *        原固件从未解析本帧，engine_rpm_raw / rpm_fault 始终未被赋值，
 *        这是转速无显示的根本原因。
 *
 *  @note 本帧使用独立的 rpm_timeout / rpm_last_recv_ms，不复用 EMS 的
 *        timeout 字段。否则 10ms 周期的本帧会不断刷新 EMS 时间戳，
 *        使 100ms 周期的 0x360（水温 / MIL）永远无法被判超时。
 *===========================================================================*/
void CAN_ParseECU302(ClusterSystem_t *sys, const u8 *data, u8 len)
{
    if (len < 8u) { return; }

    CanEMS_t *ems = &sys->can_rx.ems;
    u16 rpm_raw;

    /* 转速故障：byte0.bit2 */
    ems->rpm_fault = GET_BIT(data, 0, 2);

    /* 转速：LSB 在 bit16(byte2.bit0)，16 位，Motorola → byte1 为高字节 */
    rpm_raw = (u16)(((u16)data[1] << 8) | (u16)data[2]);

    if (rpm_raw == EMS_RPM_RAW_INVALID) {
        ems->rpm_fault = 1u;            /* 无效值按故障处理，保持上次显示 */
    } else {
        ems->engine_rpm_raw = rpm_raw;
    }

    /* 超时标志不在此清除，见文件头【超时标志所有权】说明 */
    ems->started_302      = true;
    ems->rpm_last_recv_ms = sys->system_tick_ms;
}

/*===========================================================================
 *  EMS 报文 (ID 0x360, 100ms) —— 水温 / MIL
 *
 *  @note 原有的 CAN_ParseEMS306() 已删除：整车确认本项目没有 0x306 这条报文，
 *        水温在 0x360 的 byte1。保留一个永远收不到的解析函数只会让
 *        "0x306 超时"这个恒为真的状态误导后续维护。
 *
 *  矩阵（水温）：
 *    起始字节 1   起始位 8   bit 数 8
 *    精度 0.75    偏移量 -48
 *    输入值 0x00~0xFE   有效范围 -48℃ ~ 142.5℃
 *
 *  起始位 8 = 全局 bit 8 = byte1.bit0，长度 8 → 正好整个 byte1，
 *  单字节信号无字节序问题，直接取 data[1]。
 *  换算 temp(℃) = raw × 0.75 - 48，实现见 COOLANT_RawToTemp()。
 *
 *  @note 【本次修改】水温原来错误地从 0x306 解析，而整车根本不发这条报文，
 *        coolant_temp_raw 恒为初值 0（-48℃），水温段柱恒显示 1 格。
 *  @note 矩阵中没有水温故障位，水温异常统一由本帧超时（timeout_360）表达。
 *  @note 转速不在本帧，见 CAN_ParseECU302()。
 *  @note MIL 位（byte3.bit0）沿用原实现，**尚未与矩阵核对**，需确认。
 *===========================================================================*/
void CAN_ParseEMS360(ClusterSystem_t *sys, const u8 *data, u8 len)
{
    if (len < 8u) { return; }

    CanEMS_t *ems = &sys->can_rx.ems;
    u8 coolant_raw;

    ems->mil_on = GET_BIT(data, 3, 0);      /* 待与矩阵核对 */

    /* 水温：byte1，0x00~0xFE 有效，0xFF 为无效值 → 保持上一次显示不跳变 */
    coolant_raw = data[1];
    if (coolant_raw <= EMS_COOLANT_RAW_MAX) {
        ems->coolant_temp_raw = coolant_raw;
    }

    /* 只刷新 0x360 自己的时间戳（不再与 0x306 共用）；
     * 超时标志不在此清除，见文件头【超时标志所有权】说明 */
    ems->started_360      = true;
    ems->last_recv_360_ms = sys->system_tick_ms;
}

/*===========================================================================
 *  GPF 再生状态 (ID 0x303, 10ms)
 *
 *  @note byte6.bit2~3 的位置尚未与整车 DBC 核对，需确认。
 *===========================================================================*/
void CAN_ParseGPF(ClusterSystem_t *sys, const u8 *data, u8 len)
{
    if (len < 8u) { return; }

    CanGPF_t *gpf = &sys->can_rx.gpf;

    /* GPF 状态：2 位（OFF/YELLOW/GREEN/RED） */
    gpf->status       = (GpfStatus_t)GET_BITS(data, 6, 2, 2);
    /* 超时标志不在此清除，见文件头【超时标志所有权】说明 */
    gpf->started      = true;
    gpf->last_recv_ms = sys->system_tick_ms;
}

/*===========================================================================
 *  CNG 燃气控制器报文 (ID 0x305, 200ms)
 *===========================================================================*/
void CAN_ParseCNG(ClusterSystem_t *sys, const u8 *data, u8 len)
{
    if (len < 8u) { return; }

    CanCNG_t *cng = &sys->can_rx.cng;

    /* [BUG-I] EMS_CNGSystemFailure（byte0.bit0，燃气系统故障）此前未解析，
     * 对应面板图标 METER_ICON_CNG_FAULT（R8/SEG51）也就一直没有信号驱动。
     * 矩阵：0x0=no error，0x1=error。 */
    cng->system_failure   = (bool)GET_BIT(data, 0, 0);
    cng->leak_status      = (GasLeakStatus_t)GET_BITS(data, 0, 1, 2);
    cng->cng_mode         = GET_BITS(data, 0, 3, 2);
    cng->cng_fuel_percent = data[1];

    /* 超时标志不在此清除，见文件头【超时标志所有权】说明 */
    cng->started      = true;
    cng->last_recv_ms = sys->system_tick_ms;
}

/*===========================================================================
 *  TPMS 胎压 (ID 0x390, 200ms)
 *
 *  Byte 布局简述：
 *    byte0 [bit2:0]  胎位 tire_pos
 *    byte1           左前胎压原始值
 *    byte2           右前胎压原始值
 *    byte3           右后胎压原始值
 *    byte4           左后胎压原始值
 *    byte5           温度（带符号）
 *    byte6 [bit2:0]  右前胎状态
 *    byte6 [bit5:3]  左前胎状态
 *    byte7 [bit0]    系统报警
 *    byte7 [bit1]    温度报警
 *    byte7 [bit3:1]  右后胎状态
 *    byte7 [bit6:4]  左后胎状态
 *===========================================================================*/
void CAN_ParseTPMS(ClusterSystem_t *sys, const u8 *data, u8 len)
{
    if (len < 8u) { return; }

    CanTPMS_t *tpms = &sys->can_rx.tpms;

    tpms->tire_pos     = GET_BITS(data, 0, 0, 3);
    tpms->press_lf_raw = data[1];
    tpms->press_rf_raw = data[2];
    tpms->press_rr_raw = data[3];
    tpms->press_lr_raw = data[4];
    tpms->temp_raw     = (s8)data[5];

    tpms->status_rf    = (TpmsSensorStatus_t)GET_BITS(data, 6, 0, 3);
    tpms->status_lf    = (TpmsSensorStatus_t)GET_BITS(data, 6, 3, 3);

    tpms->sys_alarm    = GET_BIT(data, 7, 0);
    tpms->temp_alarm   = GET_BIT(data, 7, 1);
    tpms->status_lr    = (TpmsSensorStatus_t)GET_BITS(data, 7, 4, 3);
    tpms->status_rr    = (TpmsSensorStatus_t)GET_BITS(data, 7, 1, 3);

    /* 超时标志不在此清除，见文件头【超时标志所有权】说明 */
    tpms->started      = true;
    tpms->last_recv_ms = sys->system_tick_ms;
}