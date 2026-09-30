/**
 * @file  lowpower.c
 * @brief ACC OFF 低功耗状态机实现
 *
 * ===========================================================================
 *  一、四个必须理解的实现约束
 * ===========================================================================
 *
 *  ★ 约束 1：LPM_Task 会阻塞 ★
 *    进入休眠时本任务阻塞在 HAL_LP_EnterLowPower() 的 WFI 上。合作式调度器
 *    不支持抢占，这是唯一能安全停下来的地方。因此 LPM_Task 必须登记在
 *    scheduler.c 任务表的**最后一项**：前面的任务本轮已跑完，屏幕内容、
 *    NVM、CAN 发送队列都处于一致状态，此时停下来不会留下半截事务。
 *
 *  ★ 约束 2：system_tick_ms 在休眠期间是冻结的 ★
 *    HAL 的 lp_periph_off() 停掉了 lpTMR，1ms 中断不再来。这是**有意的**：
 *      - 好处：唤醒后 system_tick_ms 与所有 last_recv_ms 仍然自洽，
 *        不会出现"睡了 8 小时，醒来一瞬间所有 CAN 报文同时判超时、
 *        故障灯全亮"的误报；
 *      - 代价：休眠时长不计入系统时基。仪表显示的北京时间来自 DS1307，
 *        由 RTC_Task 每秒重读，不依赖 system_tick_ms，所以不受影响。
 *    若将来要统计"总休眠时长"，应从 DS1307 读时间差，不要去补 tick。
 *
 *  ★ 约束 3：唤醒后必须重置 CAN 的 started 标志 ★
 *    见 LPM_ResetCanTimeouts()。休眠期间 FlexCAN 被 Deinit，软件 FIFO 被
 *    CAN_Init() 清空，但 cluster_context 里的 last_recv_ms / started 还停留
 *    在睡前的值。统一按"从未收到过第一帧"处理最安全 —— can_app.c 的超时
 *    判定会跳过 started == false 的报文，唤醒瞬间不会闪一片故障灯。
 *
 *  ★ 约束 4：唤醒源只有 IGN ★
 *    这是项目约束（见 cfg_lowpower.h）。对本文件的影响是：
 *      - LPM_DoWakeup() 里除 IGN 之外的分支实际走不到，但要保留 ——
 *        干扰引起的 LPM_WAKE_UNKNOWN 会走到那里；
 *      - 休眠条件里车门 / 转向 / CAN 的保持窗口**不能删**。
 *        睡着之后这三样叫不醒仪表，所以醒着的时候就更不能轻易睡：
 *        一旦在双闪亮着时睡过去，转向图标会定格熄灭直到拧钥匙。
 *        在"只有 IGN 能唤醒"的前提下，这些窗口是唯一的兜底。
 *
 *  ★ 约束 5：唤醒后必须重挂三类服务 ★
 *    这三条漏掉任意一条都会产生"能唤醒，但某个功能永远不工作"的疑难问题：
 *      Meter_Init()   段码屏（SPI Deinit 过，SC5037 寄存器配置已失效）
 *      CAN_Init()     CAN 邮箱 + 验收掩码 + 接收事件回调
 *      RADIO_Init()   UART2 的 DMA 接收（收音机数据的唯一入口）
 *
 * ===========================================================================
 *  二、休眠期间的电流责任划分
 * ===========================================================================
 *    三路外设供电（SOC/收音机/功放）→ pwr_mgr.c 在 IGN OFF 时已切断
 *    里程保存 / 熄屏决策            → 本文件 PRE_SLEEP 阶段
 *    背光 / 蜂鸣器 / 段码屏 / CAN 收发器 / MCU 外设 / 时钟 → hal_lowpower.c
 *  本文件不直接碰任何寄存器。
 */

#include "lowpower.h"
#include "pwr_mgr.h"
#include "sys_mgr.h"
#include "nvm_storage.h"
#include "meter_display.h"
#include "voltage.h"
#include "radio_input.h"
#include "soc_uart.h"
#include "rtc_input.h"
#include "can_app.h"
#include "hal_lowpower.h"
#include "hal_backlight.h"
#include "cfg_lowpower.h"

#if (CFG_LPM_ENABLE == 0u)

/*---------------------------------------------------------------------------
 *  低功耗关闭：全部退化为空实现，行为与改造前完全一致
 *-------------------------------------------------------------------------*/
void LPM_Init(ClusterSystem_t *sys)                  { (void)sys; }
void LPM_Task(ClusterSystem_t *sys)                  { (void)sys; }
void LPM_Inhibit(ClusterSystem_t *sys, bool inhibit) { (void)sys; (void)inhibit; }
void LPM_KeepAwake(ClusterSystem_t *sys)             { (void)sys; }

#else

/*---------------------------------------------------------------------------
 *  内部状态
 *-------------------------------------------------------------------------*/
static u32  s_idle_start_ms  = 0u;  /**< 休眠条件开始连续满足的时刻     */
static u32  s_awake_start_ms = 0u;  /**< 本次清醒的起始时刻             */
static u32  s_pre_start_ms   = 0u;  /**< 进入 PRE_SLEEP 的时刻          */
static u32  s_door_last_ms   = 0u;  /**< 车门最后一次"开"的时刻         */
static u32  s_turn_last_ms   = 0u;  /**< 转向最后一次"亮"的时刻         */
static bool s_pre_done       = false; /**< PRE_SLEEP 的关断动作已执行   */
static bool s_low_bat        = false; /**< 本次清醒处于深放电保护状态   */

/*===========================================================================
 *  内部：CAN 最近一次收到报文的时刻
 *
 *  取六路报文时间戳的最大值。只统计 started == true 的：从未上线的报文
 *  时间戳恒为 0，参与比较没有意义。
 *===========================================================================*/
static u32 LPM_LastCanRxMs(const ClusterSystem_t *sys)
{
    const CanRxData_t *rx = &sys->can_rx;
    u32 last = 0u;

    if (rx->abs.started     && (rx->abs.last_recv_ms     > last)) { last = rx->abs.last_recv_ms;     }
    if (rx->ems.started_360 && (rx->ems.last_recv_360_ms > last)) { last = rx->ems.last_recv_360_ms; }
    if (rx->ems.started_302 && (rx->ems.rpm_last_recv_ms > last)) { last = rx->ems.rpm_last_recv_ms; }
    if (rx->gpf.started     && (rx->gpf.last_recv_ms     > last)) { last = rx->gpf.last_recv_ms;     }
    if (rx->cng.started     && (rx->cng.last_recv_ms     > last)) { last = rx->cng.last_recv_ms;     }
    if (rx->tpms.started    && (rx->tpms.last_recv_ms    > last)) { last = rx->tpms.last_recv_ms;    }

    return last;
}

static bool LPM_CanBusIdle(const ClusterSystem_t *sys)
{
    u32 last = LPM_LastCanRxMs(sys);

    if (last == 0u) { return true; }        /* 一帧都没收到过：算静默 */

    return ((u32)(sys->system_tick_ms - last) >= CFG_LPM_CAN_IDLE_MS);
}

/*===========================================================================
 *  内部：唤醒后清空 CAN 超时状态（理由见文件头约束 3）
 *===========================================================================*/
static void LPM_ResetCanTimeouts(ClusterSystem_t *sys)
{
    CanRxData_t *rx = &sys->can_rx;

    rx->abs.started      = false;  rx->abs.timeout      = false;
    rx->ems.started_360  = false;  rx->ems.timeout_360  = false;
    rx->ems.started_302  = false;  rx->ems.rpm_timeout  = false;
    rx->gpf.started      = false;  rx->gpf.timeout      = false;
    rx->cng.started      = false;  rx->cng.timeout      = false;
    rx->tpms.started     = false;  rx->tpms.timeout     = false;

    rx->abs.last_recv_ms     = sys->system_tick_ms;
    rx->ems.last_recv_360_ms = sys->system_tick_ms;
    rx->ems.rpm_last_recv_ms = sys->system_tick_ms;
    rx->gpf.last_recv_ms     = sys->system_tick_ms;
    rx->cng.last_recv_ms     = sys->system_tick_ms;
    rx->tpms.last_recv_ms    = sys->system_tick_ms;
}

/*===========================================================================
 *  内部：蓄电池是否已进入深放电区
 *
 *  只在电压读数可信（valid）时才判定。ADC 刚重启还没出有效值时按
 *  "电量正常"处理，避免唤醒瞬间因为一个 0 值就立刻回睡。
 *===========================================================================*/
static bool LPM_BatteryLow(const ClusterSystem_t *sys)
{
#if (CFG_LPM_DEEP_DISCHARGE_MV == 0u)
    (void)sys;
    return false;
#else
    if (!sys->voltage.valid) { return false; }
    return (sys->voltage.vbat_mv < CFG_LPM_DEEP_DISCHARGE_MV);
#endif
}

/*===========================================================================
 *  内部：休眠条件判定
 *===========================================================================*/
static bool LPM_SleepAllowed(ClusterSystem_t *sys)
{
    const HardwireInput_t *hw  = &sys->hw_input;
    const u32              now = sys->system_tick_ms;
    u32                    min_awake;

    /* ⑦ 外部禁止（UDS 刷写等长事务） */
    if (sys->lpm.inhibit) { return false; }

    /* ① 点火必须是 OFF。
     *    同时看硬线和 power_state：硬线是当前电平，power_state 是去抖后的
     *    结论，两个都满足才动手，避免在钥匙抖动的中间态进睡。 */
    if (hw->ign_on)                    { return false; }
    if (sys->power_state == PWR_ON)    { return false; }
    if (sys->power_state == PWR_START) { return false; }

    /* ★ 深放电保护 ★
     *  电压已经低于门限时，只有 IGN 值得让仪表保持清醒。车门 / 转向 / CAN
     *  唤醒只给 CFG_LPM_LOW_BAT_AWAKE_MS 就回睡，不再等 30s 的车门保持。
     *  直接 return true 会跳过下面②③④的保持窗口 —— 这是刻意的。 */
    if (s_low_bat && ((u32)(now - s_awake_start_ms) >= CFG_LPM_LOW_BAT_AWAKE_MS)) {
        return true;
    }

    /* ② 转向 / 双闪：活动时刷新时刻，消失后还要再保持一段 */
    if (hw->turn_left || hw->turn_right) {
        s_turn_last_ms = now;
        return false;
    }
    if ((u32)(now - s_turn_last_ms) < CFG_LPM_TURN_HOLD_MS) { return false; }

    /* ③ 车门 */
    if (hw->door_open_n) {
        s_door_last_ms = now;
        return false;
    }
    if ((u32)(now - s_door_last_ms) < CFG_LPM_DOOR_HOLD_MS) { return false; }

    /* ④ CAN 总线静默 */
    if (!LPM_CanBusIdle(sys)) { return false; }

    /* ⑤ 蜂鸣器：鸣叫中关 MPWM 会把输出停在一个固定电平上 */
    if (sys->buzzer.buzzer_on || (sys->buzzer.active_alarm != BUZZER_IDLE)) {
        return false;
    }

    /* ⑥ 最短清醒时间 */
    min_awake = s_low_bat ? CFG_LPM_LOW_BAT_AWAKE_MS : CFG_LPM_MIN_AWAKE_MS;
    if ((u32)(now - s_awake_start_ms) < min_awake) { return false; }

    return true;
}

/*===========================================================================
 *  内部：PRE_SLEEP 的关断动作（只做一次）
 *
 *  这里只做**必须在外设还活着时**完成的两件事：熄屏和存里程。
 *  段码屏进 SLEEP_IN、关 SPI/I2C 等由 HAL_LP_PrepareSleep() 负责。
 *
 *  顺序：
 *    1) 先关背光 —— 用户看到的"熄火后仪表灭"就发生在这一刻，要最先做，
 *       否则会看到背光先灭、段码再灭的两段式，观感很差；
 *    2) 再存 NVM —— 此时 I2C 还活着；
 *    3) 存完等 CFG_LPM_PRESLEEP_MS 让 EEPROM 走完写周期，再由
 *       HAL_LP_PrepareSleep() 关 I2C。顺序反了会丢里程。
 *===========================================================================*/
static void LPM_DoPreSleep(ClusterSystem_t *sys)
{
    HAL_BACKLIGHT_Set(0u);
    sys->lcd.backlight_pct = 0u;

    /* 段码缓冲清空并刷出。真正的 SLEEP_IN 命令和 RST 拉低在 HAL 里做，
     * 但缓冲必须先清 —— 否则唤醒后 Meter_Refresh 会把睡前的旧画面刷回来。 */
    Meter_ClearAll();
    Meter_Refresh();

    /* pwr_mgr.c 在 IGN ON→OFF 的跳变上已经存过一次里程，这里再存一次：
     * 熄火后 CFG_LPM_ACC_OFF_HOLD_MS 期间车还可能滑行（挂空挡溜车），
     * ODO_Task 仍在累加。代价只是一次 EEPROM 写周期。 */
    NVM_SaveOdometer(sys);

    s_pre_done = true;
}

/*===========================================================================
 *  内部：休眠 + 等待唤醒（阻塞）
 *===========================================================================*/
static LpmWakeSrc_t LPM_SleepAndWait(ClusterSystem_t *sys)
{
    LpmWakeSrc_t wake;

    sys->lpm.state = LPM_STATE_SLEEP;
    sys->lpm.sleep_cnt++;

    HAL_LP_PrepareSleep();

    /* 伪唤醒过滤：干扰可能在唤醒引脚上打出一个窄脉冲，中断进了但认不出
     * 来源。这种情况直接再睡回去，不做完整的外设恢复，省掉一次
     * CAN/SPI/I2C 的 Init-Deinit 往返。 */
    do {
        HAL_LP_EnterLowPower();     /* ← 阻塞在这里，直到中断 */
        wake = HAL_LP_GetWakeSource();
    } while (wake == LPM_WAKE_NONE);

    HAL_LP_ResumeFromSleep();

    sys->lpm.last_wake = wake;
    return wake;
}

/*===========================================================================
 *  内部：唤醒后的应用层恢复
 *===========================================================================*/
static void LPM_DoWakeup(ClusterSystem_t *sys, LpmWakeSrc_t wake)
{
    const u32 now = sys->system_tick_ms;

    /* 1) 段码屏重新初始化。
     *    SPI 在休眠期间被 Deinit、SC5037 已进 SLEEP_IN 且 RST 被拉低，
     *    必须走完整的 硬件复位 + Init，不能只 Meter_Refresh()。 */
    Meter_Init();

    /* 2) 电源状态机复位。
     *    PWR_Init 会重置 IGN 去抖的静态变量，否则被 IGN 唤醒时
     *    s_ign_debounced 还停留在睡前的值，OFF→ON 边沿会被漏掉，
     *    这次点火就不做指示灯自检。 */
    PWR_Init(sys);

    /* 3) CAN：HAL 只做了 FLEXCAN_DRV_Init，邮箱 / 验收掩码 / 接收事件回调
     *    要由 CAN_Init() 重建，否则一帧都收不到。
     *    注意这与"CAN 不作为唤醒源"不矛盾：叫不醒是一回事，
     *    醒来之后能不能正常收报文是另一回事，后者必须保证。 */
    CAN_Init();
    LPM_ResetCanTimeouts(sys);

    /* 4) 串口服务重挂。
     *    RADIO_Init() 里的 UART_DRV_InstallRxCallback + rx_arm() 是收音机
     *    数据的唯一入口，不重新调用的话唤醒后收音机频率/音量永远不再更新，
     *    现象是"完全没反应"，很难查。 */
    RADIO_Init(sys);
    SOC_UART_Init();
    SOC_UART_NotifyMcuReady();      /* 请求山景 SOC 补发音量等状态 */

    /* 5) 电压监测：ADC DMA 刚重启，滤波器里还是睡前的旧值。
     *    重新 Init 让它回到 VOLT_STATE_INIT，按"尚未测出有效电压"处理，
     *    避免用几小时前的电压去决定要不要给 SOC 供电。 */
    VOLT_Init(sys);

    /* 6) RTC：I2C 重新 Init 后要重配访问状态。
     *    DS1307 自己一直在走时（走纽扣电池），时间不会丢。 */
    RTC_Init(sys);

    /* 7) 唤醒源决定要不要重走自检。
     *
     *    ★ 当前唤醒源只有 IGN，所以实际只会走第一个分支 ★
     *    else 分支保留是因为：① 干扰导致的 LPM_WAKE_UNKNOWN 会走到那里；
     *    ② 将来打开 CFG_LPM_WAKE_ON_DOOR / TURN / CAN 时不用再改这里。
     *
     *    - IGN 唤醒：一次完整的点火循环，走自检（PWR_Task 检到 OFF→ON
     *      边沿时也会再触发一次，SYS_StartSelfCheck 幂等，不会叠加）；
     *    - 其它唤醒：只是"亮一下屏"，不做自检 ——
     *      那种场景下全亮故障灯 1.5s 会被用户当成故障。 */
    if (wake == LPM_WAKE_IGN) {
        SYS_StartSelfCheck(sys);
    } else {
        sys->selfcheck_done     = true;
        sys->selfcheck_timer_ms = 0u;
        sys->selfcheck_start_ms = now;
    }

    /* 8) 背光交回 BACKLIGHT_Task 按小灯状态决定，这里不写死亮度 */
    sys->cluster_state = CLUSTER_WAKE;

    /* 9) 计时器归零。深放电标志清掉，等 VOLT 出新结果后重新判。 */
    s_awake_start_ms = now;
    s_idle_start_ms  = now;
    s_door_last_ms   = now;
    s_turn_last_ms   = now;
    s_pre_done       = false;
    s_low_bat        = false;

#if (CFG_LPM_WAKE_ON_CAN != 0)
    /* 10) 被 CAN 唤醒时不给满额清醒时间：总线上的一个毛刺也能触发 RXD 边沿。
     *     把"车门保持"和"转向保持"的起点往回拨，使得只要
     *     CFG_LPM_CAN_WAKE_VALID_MS 内没有真正的有效帧就能尽快回睡。
     *     当前 CAN 不作为唤醒源，这段不编译。 */
    if (wake == LPM_WAKE_CAN) {
        s_door_last_ms = now - CFG_LPM_DOOR_HOLD_MS + CFG_LPM_CAN_WAKE_VALID_MS;
        s_turn_last_ms = now - CFG_LPM_TURN_HOLD_MS + CFG_LPM_CAN_WAKE_VALID_MS;
    }
#endif

    sys->lpm.state = LPM_STATE_RUN;
}

/*===========================================================================
 *  公开接口
 *===========================================================================*/
void LPM_Init(ClusterSystem_t *sys)
{
    const u32 now = sys->system_tick_ms;

    sys->lpm.state     = LPM_STATE_RUN;
    sys->lpm.last_wake = LPM_WAKE_NONE;
    sys->lpm.sleep_cnt = 0u;
    sys->lpm.inhibit   = false;

    s_idle_start_ms  = now;
    s_awake_start_ms = now;
    s_pre_start_ms   = now;
    s_door_last_ms   = now;
    s_turn_last_ms   = now;
    s_pre_done       = false;
    s_low_bat        = false;

    HAL_LP_Init();
}

void LPM_Inhibit(ClusterSystem_t *sys, bool inhibit)
{
    sys->lpm.inhibit = inhibit;
    if (inhibit) { s_idle_start_ms = sys->system_tick_ms; }
}

void LPM_KeepAwake(ClusterSystem_t *sys)
{
    s_idle_start_ms  = sys->system_tick_ms;
    s_awake_start_ms = sys->system_tick_ms;
}

void LPM_Task(ClusterSystem_t *sys)
{
    const u32    now = sys->system_tick_ms;
    LpmWakeSrc_t wake;

    /* 深放电标志每周期刷新一次。一旦置位就保持到下次唤醒后清零，
     * 不做"电压回升立刻解除"——电压是在负载被切掉后回弹的，
     * 让它解除会导致仪表在亏电时反复醒来，把电瓶放得更狠。 */
    if (LPM_BatteryLow(sys)) { s_low_bat = true; }

    switch (sys->lpm.state) {

        case LPM_STATE_RUN:
            /* 休眠条件只要有一条不满足就把倒计时拉回起点。
             * 用"连续满足时长"而不是"边沿触发"，是为了让熄火后又插回
             * 钥匙、开门关门这类操作能干净地打断休眠倒计时。 */
            if (!LPM_SleepAllowed(sys)) {
                s_idle_start_ms = now;
                break;
            }

            if ((u32)(now - s_idle_start_ms) >= CFG_LPM_ACC_OFF_HOLD_MS) {
                sys->lpm.state = LPM_STATE_PRE_SLEEP;
                s_pre_start_ms = now;
                s_pre_done     = false;
            }
            break;

        case LPM_STATE_PRE_SLEEP:
            /* 预休眠期间条件被打破（例如又插了钥匙）→ 立刻回 RUN。
             * 屏已经清了没关系，Meter_Init() 会把它重建起来。 */
            if (!LPM_SleepAllowed(sys)) {
                sys->lpm.state  = LPM_STATE_RUN;
                s_idle_start_ms = now;
                if (s_pre_done) {
                    Meter_Init();
                    s_pre_done = false;
                }
                break;
            }

            if (!s_pre_done) {
                LPM_DoPreSleep(sys);
                break;                  /* 本周期只做关断，下周期再判时间 */
            }

            /* 等 EEPROM 写周期走完再关 I2C，见 LPM_DoPreSleep 的注释 */
            if ((u32)(now - s_pre_start_ms) >= CFG_LPM_PRESLEEP_MS) {
                wake = LPM_SleepAndWait(sys);   /* ← 阻塞，唤醒后返回 */
                LPM_DoWakeup(sys, wake);
            }
            break;

        case LPM_STATE_SLEEP:
        case LPM_STATE_WAKEUP:
        default:
            /* 正常走不到：SLEEP/WAKEUP 都在 LPM_SleepAndWait / LPM_DoWakeup
             * 内部同步完成。走到这里说明状态被意外改写，保守回到 RUN。 */
            sys->lpm.state  = LPM_STATE_RUN;
            s_idle_start_ms = now;
            break;
    }
}

#endif /* CFG_LPM_ENABLE */
