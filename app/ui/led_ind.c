/**
 * @file    led_ind.c
 * @brief   指示灯逻辑模块实现
 *
 * ===========================================================================
 * 本次修改（自检期间非自检灯实时显示 + 报警音照常）
 * ===========================================================================
 *  【需求】上电自检过程中，除了参与自检的故障灯之外，其余指示灯
 *          （转向灯、安全带未系报警灯、车门、气囊、燃气泄漏、位置灯、
 *           远近光/雾灯、驻车制动、充电灯……）必须按真实信号正常显示，
 *          并且报警音（蜂鸣器、SOC 转向提示音）照常播放。
 *
 *  【原实现的问题】LED_Task 开头是：
 *          if (!sys->selfcheck_done) { LED_SelfCheck(sys); return; }
 *      整个业务判定被跳过，且 LED_SelfCheck() 会先把 LedStatus_t 整体
 *      清零。后果：
 *        1) 自检 3s 内打转向，转向灯不亮；
 *        2) SOC_UART_NotifyTurnSignal 不被调用，SOC 收不到转向帧，
 *           没有转向提示音；且因为它内部按“状态变化”发帧，自检结束时
 *           若转向状态未变，会一直静音到下一次拨杆动作；
 *        3) 安全带未系报警灯、车门灯、气囊灯自检期间全灭；
 *        4) LED_UpdateGasLeak 不执行 → 燃气泄漏蜂鸣器不响；
 *        5) 充电灯/机油灯的时序计时器自检期间不启动。
 *
 *  【现改法】LED_Task 每周期都完整跑一遍业务判定（含 UART 通知与蜂鸣器
 *      触发），只在最后用 LED_ApplySelfCheckPattern() 把
 *      LED_ApplySelfCheckPattern() 里列出的灯**覆盖**成常亮。
 *      清单之外的灯保持业务判定结果 → 自检期间实时显示。
 *      LED_SelfCheck() 不再整体清零 LedStatus_t（25 个字段每周期都会被
 *      业务逻辑重写，不存在残留），否则会把刚算好的实时状态抹掉。
 *
 * ===========================================================================
 * 历史修改
 * ===========================================================================
 *  【问题 2】水温报警灯改为读 sys->lcd.coolant.signal_lost，与段柱共用同一
 *            个消抖后的判据；点亮的段由 cfg_meter_map.h 改指 R55（红灯）。
 *            发动机故障灯的超时判据由共用的 ems.timeout 改为 ems.timeout_360。
 *
 *  【问题 3/5】充电指示灯、机油压力低指示灯的延时全部改用系统时间戳。
 *            原实现是 `s_xxx_delay_ms += 10u`，但 LED_Task 在 scheduler.c 中
 *            登记的周期是 **20ms**，累加步长和调用周期对不上，实际延时是配置
 *            值的 2 倍以上（机油 2s → 实测 4~5s，充电 3s → 实测 6s+）。
 *            改用 system_tick_ms 后与任务周期彻底解耦。
 *
 *  【问题 3】充电指示灯的判定窗口原来由"是否曾经起过机"这一粘滞标志决定，
 *            熄火后转速回 0 仍在用 3s 窗口。现改为按**当前转速**动态选择：
 *              转速 ≤ 350 r/min → 1s
 *              转速 >  350 r/min → 3s
 *            并加一次性锁存，避免转速跨过阈值时灯已亮又灭掉。
 *
 * 25 个指示灯按"驱动来源"可分为三类：
 *   1) 硬线直驱：转向灯、远近光、雾灯、车门、位置灯、安全气囊等
 *      → 直接根据 hw_input.* 字段赋值
 *   2) CAN 间接驱动：燃油低、CNG、GPF、TPMS、ABS、EBD、发动机故障、燃气泄漏
 *      → 由 LED_UpdateXxx 综合 CAN + 业务模块输出
 *   3) 时序驱动：充电指示灯、机油压力低（带启动延迟）
 *      → 维护内部累加计时器
 *
 * 【本次新增】CNG 燃气系统故障灯（cng_fault，R8/SEG51）：
 *   对应整车矩阵里的 EMS_CNGSystemFailure（0x305 byte0.bit0），此前
 *   can_rx_parse.c 未解析该位，面板 R8/SEG51 也一直没有信号驱动，
 *   见 CAN_ParseCNG() 与 LED_UpdateCngFault()。与 cng_warn（仅 CNG 模式
 *   下的低气量报警）不同，系统故障不区分当前燃料模式，只要 0x305 在线
 *   且置位就点亮。
 *
 * 所有指示灯均为 LCD 面板上的图标段，由 meter_display.h 的
 * Meter_SetIcon() 驱动，不再使用 HAL_LED_Set / HAL_LED_SetBlink。
 *
 * 闪烁实现：
 *   LED_BLINK_1HZ  → 使用 sys->blink_1hz_visible（500ms 翻转，软件节拍）
 *   LED_BLINK_2HZ  → 使用 sys->system_tick_ms 推导 250ms 翻转节拍
 *
 * 业务名称 → 面板图标段的映射见：
 *   app/common/config/cfg_meter_map.h
 */

#include "led_ind.h"
#include "meter_display.h"
#include "cfg_meter_map.h"
#include "buzzer.h"          /* 燃气泄漏需要触发蜂鸣器 */
#include "hal_gpio.h"


/*===========================================================================
 *  内部函数前向声明（按更新顺序排列）
 *===========================================================================*/
static void LED_UpdateFuelLow(ClusterSystem_t *sys);
static void LED_UpdateCngWarn(ClusterSystem_t *sys);
static void LED_UpdateCngFault(ClusterSystem_t *sys);
static void LED_UpdateGpf(ClusterSystem_t *sys);
static void LED_UpdateEps(ClusterSystem_t *sys);
static void LED_UpdateTpms(ClusterSystem_t *sys);
static void LED_UpdateAbs(ClusterSystem_t *sys);
static void LED_UpdateSeatbelt(ClusterSystem_t *sys);
static void LED_UpdateWaterTemp(ClusterSystem_t *sys);
static void LED_UpdateCharger(ClusterSystem_t *sys);
static void LED_UpdateEngineFault(ClusterSystem_t *sys);
static void LED_UpdateEbd(ClusterSystem_t *sys);
static void LED_UpdateOilPress(ClusterSystem_t *sys);
static void LED_UpdateGasLeak(ClusterSystem_t *sys);
static void LED_ApplySelfCheckPattern(ClusterSystem_t *sys);

/*---------------------------------------------------------------------------
 *  时序型指示灯的内部状态
 *---------------------------------------------------------------------------*/

/* 充电指示灯：转速 ≤350 时 1s 判定，转速 >350 时 3s 判定
 * 全部基于 sys->system_tick_ms，不依赖 LED_Task 的调用周期。 */
static bool s_charger_timing  = false;  /**< 正在计时（硬线已拉低）   */
static u32  s_charger_start_ms = 0u;    /**< 硬线拉低的时刻（ms）     */
static bool s_charger_latched = false;  /**< 判定已通过，锁存点亮     */

/* 机油压力低：CFG_OIL_PRESS_DELAY_MS 延迟 */
static bool s_oil_timing   = false;     /**< 正在计时（硬线已拉低）   */
static u32  s_oil_start_ms = 0u;        /**< 硬线拉低的时刻（ms）     */

/*===========================================================================
 *  初始化
 *===========================================================================*/
void LED_Init(ClusterSystem_t *sys)
{
    LedStatus_t *led = &sys->led;

    /* 把 LedStatus_t 整体清零（LED_OFF == 0） */
    uint8_t *p = (uint8_t *)led;
    for (uint8_t i = 0; i < sizeof(LedStatus_t); i++) {
        p[i] = (uint8_t)LED_OFF;
    }

    s_charger_timing   = false;
    s_charger_start_ms = 0u;
    s_charger_latched  = false;
    s_oil_timing       = false;
    s_oil_start_ms     = 0u;
}

/*===========================================================================
 *  上电自检
 *===========================================================================*/
/**
 * @brief 把"自检清单"里的灯覆盖为常亮（不影响清单之外的灯）
 *
 * ★关键★ 这里只做"覆盖"，不做"清零"。转向灯、安全带报警灯、车门灯、
 * 燃气泄漏灯等**不在**下面这张清单里的灯，保留 LED_Task 里刚算出来的
 * 实时状态，自检期间照常显示、照常出报警音。
 *
 * 【增删自检灯】直接在下面加/删一行即可。
 *   ⚠ 不要把这些灯加进来，否则与"自检期间实时显示"的需求冲突：
 *       turn_left / turn_right   转向灯（同时驱动 SOC 转向提示音）
 *       seatbelt_warn            安全带未系报警灯（同时驱动蜂鸣器）
 *       gas_leak                 燃气泄漏灯（同时驱动蜂鸣器）
 *       door_open / airbag_fault / eps_fault / brake_fault
 *       low_beam / high_beam / fog_front / fog_rear / pos_light
 *       charger_fault            充电指示灯（带 1s/3s 时序判定）
 *
 * 注：早前版本用 cfg_selfcheck.h 里的 X-Macro (CFG_SELFCHECK_LAMP_LIST)
 *     展开这张表。改回明写，是为了不让本文件依赖另一个头文件里的宏——
 *     那个头文件一旦漏更新，IAR 会在本函数里报
 *     Error[Pe020]: identifier "X" is undefined，排查成本远高于省下的几行。
 */
static void LED_ApplySelfCheckPattern(ClusterSystem_t *sys)
{
    LedStatus_t *led = &sys->led;

    led->fuel_low        = LED_ON;
    led->gpf_warn        = LED_ON;
    led->gpf_regen       = LED_ON;
    led->tpms_warn       = LED_ON;
    led->abs_fault       = LED_ON;
    led->water_temp_high = LED_ON;
    led->engine_fault    = LED_ON;
    led->ebd_fault       = LED_ON;
    led->oil_press_low   = LED_ON;
}

/**
 * 点亮自检图案（对外接口，供 main.c 的 Cluster_Init 调用一次）。
 *
 * 本函数是幂等的，可在自检期间被重复调用。
 *
 * 【本次修改】删除了函数开头"把 LedStatus_t 整体清零"的动作：
 *   - LED_Task 每周期都会重写全部 25 个灯的状态，不存在跨点火循环的残留；
 *   - 若在这里清零，会把同一周期内刚算好的转向灯/安全带灯等实时状态抹掉，
 *     这正是"自检期间转向灯不亮"的直接原因。
 */
void LED_SelfCheck(ClusterSystem_t *sys)
{
    LED_ApplySelfCheckPattern(sys);
    LED_DriveAll(sys);
}

/*===========================================================================
 *  周期任务（20ms 调用）
 *===========================================================================*/
void LED_Task(ClusterSystem_t *sys)
{
    //PINS_DRV_WritePin(GPIOC,9,1); yc
    
    /* 【本次修改】自检期间不再 return。
     * 业务判定每周期照常全跑一遍（包括 SOC 转向通知和燃气泄漏蜂鸣器触发），
     * 自检图案在函数末尾以"覆盖"的方式叠加，见 LED_ApplySelfCheckPattern。 */

    const HardwireInput_t *hw  = &sys->hw_input;
    LedStatus_t           *led = &sys->led;

    /* --- 第一组：硬线直驱（无需复杂判定） --- */
    led->turn_left  = hw->turn_left   ? LED_ON : LED_OFF;
    led->turn_right = hw->turn_right  ? LED_ON : LED_OFF;
    led->fog_front  = hw->fog_front   ? LED_ON : LED_OFF;
    led->low_beam   = hw->low_beam_n  ? LED_ON : LED_OFF;
    led->high_beam  = hw->high_beam_n ? LED_ON : LED_OFF;
    led->fog_rear   = hw->fog_rear    ? LED_ON : LED_OFF;

    /* --- 转向灯变化通知 SOC（UART2），SOC 负责播放转向提示音 ---
     * 仅当左/右转灯状态发生变化时才发送 UART2 帧，不会每 50ms 重复触发。
     * SOC 收到帧后根据 DATA 字段决策：
     *   0x00=停止提示音  0x01=左转提示音  0x02=右转提示音  0x03=双闪提示音
     *
     * 【本次修改】该调用现在在自检期间也会执行。SOC_UART_NotifyTurnSignal
     * 内部按"状态变化"发帧，若自检 3s 内被跳过，期间打的转向不仅灯不亮，
     * 自检结束后因状态没再变化也补不出提示音，必须等下一次拨杆动作。
     */
    SOC_UART_NotifyTurnSignal(hw->turn_left, hw->turn_right);

    /* --- 第二组：CAN + 业务综合 --- */
    LED_UpdateFuelLow(sys);
    LED_UpdateCngWarn(sys);
    LED_UpdateCngFault(sys);
    LED_UpdateGpf(sys);

    led->airbag_fault = hw->airbag_fault_n ? LED_ON : LED_OFF;

    LED_UpdateEps(sys);
    LED_UpdateTpms(sys);
    LED_UpdateAbs(sys);
    LED_UpdateSeatbelt(sys);
    LED_UpdateWaterTemp(sys);
    LED_UpdateCharger(sys);

    led->door_open = hw->door_open_n ? LED_ON : LED_OFF;
    led->pos_light = hw->pos_light   ? LED_ON : LED_OFF;

    LED_UpdateEngineFault(sys);
    LED_UpdateEbd(sys);

    led->brake_fault = (hw->brake_fault_n || hw->parking_n) ? LED_ON : LED_OFF;

    LED_UpdateOilPress(sys);
    LED_UpdateGasLeak(sys);      /* 泄漏时会 BUZZER_Trigger()，自检期间同样生效 */

    /* --- 第三组：自检覆盖（★本次新增★） ---
     * 自检未结束时，把 LED_ApplySelfCheckPattern 里列出的故障灯强制点亮，
     * 覆盖上面刚算出来的业务值；清单之外的灯（转向、安全带、车门、气囊、
     * 燃气泄漏、位置灯、远近光/雾灯、驻车制动、充电灯）保持实时状态。
     *
     * 放在所有业务判定之后、LED_DriveAll 之前，保证"自检优先"且只影响清单内的灯。 */
    if (!sys->selfcheck_done) {
        LED_ApplySelfCheckPattern(sys);
    }

    /* --- 最后一次性把 LedStatus_t 翻译为 Meter_SetIcon + 刷屏 --- */
    LED_DriveAll(sys);
    //PINS_DRV_WritePin(GPIOC,9,0);
}

/*===========================================================================
 *  各指示灯独立判定（逻辑层，只写 LedStatus_t，不接触显示层）
 *===========================================================================*/

/** 燃油低：仅汽油模式下生效，格数 ≤ 2 时点亮 */
static void LED_UpdateFuelLow(ClusterSystem_t *sys)
{
    if (sys->fuel_mode == FUEL_MODE_GASOLINE) {
        sys->led.fuel_low = (sys->lcd.fuel.segments <= CFG_FUEL_WARN_SEGMENT)
                            ? LED_ON : LED_OFF;
    } else {
        sys->led.fuel_low = LED_OFF;
    }
}

/** CNG 警告：仅 CNG 模式且 CAN 在线时生效 */
static void LED_UpdateCngWarn(ClusterSystem_t *sys)
{
    if (sys->fuel_mode != FUEL_MODE_CNG) {
        sys->led.cng_warn = LED_OFF;
        return;
    }
    if (sys->can_rx.cng.timeout) {
        sys->led.cng_warn = LED_OFF;
        return;
    }
    sys->led.cng_warn = (sys->lcd.fuel.segments <= CFG_FUEL_WARN_SEGMENT)
                        ? LED_ON : LED_OFF;
}

/**
 * [新增] CNG 燃气系统故障灯（R8/SEG51）：
 *   由 0x305 的 EMS_CNGSystemFailure（byte0.bit0）驱动。
 *   与 cng_warn（低气量报警，仅 CNG 模式生效）不同，系统故障属于
 *   CNG 供气系统本身的硬件/通信异常，不管当前是汽油还是 CNG 模式，
 *   只要在线且置位就应该提示驾驶员，因此不按 fuel_mode 门控，
 *   仅在 0x305 超时（保留上一状态没有意义，直接熄灭）时不显示。
 */
static void LED_UpdateCngFault(ClusterSystem_t *sys)
{
    if (sys->can_rx.cng.timeout) {
        sys->led.cng_fault = LED_OFF;
        return;
    }
    sys->led.cng_fault = sys->can_rx.cng.system_failure ? LED_ON : LED_OFF;
}

/** GPF 颗粒捕集器：黄/绿/红三态对应不同显示 */
static void LED_UpdateGpf(ClusterSystem_t *sys)
{
    if (sys->can_rx.gpf.timeout) {
        sys->led.gpf_warn  = LED_OFF;
        sys->led.gpf_regen = LED_OFF;
        sys->led.gpf_red = LED_OFF;
        return;
    }
    switch (sys->can_rx.gpf.status) {
        case GPF_OFF:
            sys->led.gpf_warn  = LED_OFF;
            sys->led.gpf_regen = LED_OFF;
            sys->led.gpf_red = LED_OFF;
            break;
        case GPF_YELLOW:
            sys->led.gpf_warn  = LED_ON;
            sys->led.gpf_regen = LED_OFF;
            sys->led.gpf_red = LED_OFF;
            break;
        case GPF_GREEN:
            sys->led.gpf_warn  = LED_OFF;
            sys->led.gpf_red = LED_OFF;
            sys->led.gpf_regen = LED_ON;
            break;
        case GPF_RED:
            sys->led.gpf_red  = LED_ON;
            sys->led.gpf_warn  = LED_OFF;
            sys->led.gpf_regen = LED_OFF;
            break;
        default:
            break;
    }
}

/** EPS 故障：直接对应硬线信号 */
static void LED_UpdateEps(ClusterSystem_t *sys)
{
    bool hw_eps_fault = sys->hw_input.eps_fault_n;
    sys->led.eps_fault = hw_eps_fault ? LED_ON : LED_OFF;
}

/** TPMS 报警：CAN 超时或任一胎异常或系统级报警 */
static void LED_UpdateTpms(ClusterSystem_t *sys)
{
    const CanTPMS_t *tpms = &sys->can_rx.tpms;
    if (tpms->timeout) {
        sys->led.tpms_warn = LED_ON;
        return;
    }
    bool tire_warn = (tpms->status_lf != TPMS_NO_WARN) ||
                     (tpms->status_rf != TPMS_NO_WARN) ||
                     (tpms->status_lr != TPMS_NO_WARN) ||
                     (tpms->status_rr != TPMS_NO_WARN);
    sys->led.tpms_warn = (tire_warn || tpms->sys_alarm) ? LED_ON : LED_OFF;
}

/** ABS 故障：CAN 超时或 abs_fault 位 */
static void LED_UpdateAbs(ClusterSystem_t *sys)
{
    const CanABS_t *abs = &sys->can_rx.abs;
    sys->led.abs_fault = (abs->timeout || abs->abs_fault) ? LED_ON : LED_OFF;
}

/** 安全带警告：未系且车速 > 5kmh 时 1Hz 闪烁，静止时常亮 */
static void LED_UpdateSeatbelt(ClusterSystem_t *sys)
{
    bool not_buckled = sys->hw_input.seatbelt_n;
    if (!not_buckled) {
        sys->led.seatbelt_warn = LED_OFF;
        return;
    }
    sys->led.seatbelt_warn =
        (sys->lcd.speed_kmh > CFG_SPEED_SEATBELT_WARN_KMH)
        ? LED_BLINK_1HZ : LED_ON;
}

/**
 * 水温报警灯（红色 R55）：水温信号长期丢失 / 故障，或高温报警锁存时点亮。
 *
 * 高温判据取 COOLANT_Task 输出的 high_temp_warn（113℃ 进 / 110℃ 出，带迟滞），
 * 不再用 segments >= 8。第 8 格下边界正好是 113℃，按格数判断等于"113 进 113 出"，
 * 迟滞失效。
 *
 * 判据统一取 COOLANT_Task 输出的 signal_lost（已按 CFG_COOLANT_TIMEOUT_S
 * 消抖），而不是直接读 ems->timeout_306。否则 CAN 抖一下红灯就闪一下，
 * 而段柱要 20s 后才掉到 1 格，灯和格柱对不上。
 *
 * 白色水温指示图标 R53 是另一个段，常亮，由 lcd_ui.c 驱动，不在这里处理。
 */
static void LED_UpdateWaterTemp(ClusterSystem_t *sys)
{
    const CoolantLcdData_t *ct = &sys->lcd.coolant;

    sys->led.water_temp_high =
        (ct->signal_lost || ct->high_temp_warn)
        ? LED_ON : LED_OFF;
}

/**
 * 充电（蓄电池）指示灯：硬线低 → 进入判定
 *  - 发动机转速 ≤ CFG_RPM_CHARGER_NO_SPEED(350)  → 延迟 1s 点亮
 *  - 发动机转速 >  CFG_RPM_CHARGER_NO_SPEED(350) → 延迟 3s 点亮
 *  - 硬线恢复高 → 立即熄灭并复位计时
 *
 * 实现要点：
 *  1) 用 system_tick_ms 做绝对时间差，不再 `+= 10u`，与任务周期无关；
 *  2) 判定窗口按**当前**转速动态选择，而不是"是否曾经起过机"的粘滞标志，
 *     熄火后转速回 0 会立刻恢复成 1s 窗口；
 *  3) 判定一旦通过就锁存（s_charger_latched），避免转速在 350 附近变化时
 *     窗口从 1s 变成 3s 导致已点亮的灯又灭掉。
 */
static void LED_UpdateCharger(ClusterSystem_t *sys)
{
    if (!sys->hw_input.charger_n) {          /* 硬线恢复高：立即清除 */
        sys->led.charger_fault = LED_OFF;
        s_charger_timing  = false;
        s_charger_latched = false;
        return;
    }

    if (!s_charger_timing) {
        s_charger_timing   = true;
        s_charger_start_ms = sys->system_tick_ms;
    }

    if (!s_charger_latched) {
        u32 need = (sys->lcd.rpm > CFG_RPM_CHARGER_NO_SPEED)
                 ? CFG_CHARGER_DELAY_WITH_RPM_MS   /* >350 r/min → 3s */
                 : CFG_CHARGER_DELAY_NO_RPM_MS;    /* 转速 0 / 低转速 → 1s */

        if ((sys->system_tick_ms - s_charger_start_ms) >= need) {
            s_charger_latched = true;
        }
    }

    sys->led.charger_fault = s_charger_latched ? LED_ON : LED_OFF;
}

/** 发动机故障：0x360 超时或 MIL 位置位
 *  原来读的是 0x306/0x360 共用的 ems.timeout，水温报文丢失也会点亮 MIL 灯。 */
static void LED_UpdateEngineFault(ClusterSystem_t *sys)
{
    sys->led.engine_fault =
        (sys->can_rx.ems.timeout_360 || sys->can_rx.ems.mil_on) ? LED_ON : LED_OFF;
}

/** EBD 故障：CAN 超时或 ebd_fault 位 */
static void LED_UpdateEbd(ClusterSystem_t *sys)
{
    const CanABS_t *abs = &sys->can_rx.abs;
    sys->led.ebd_fault = (abs->timeout || abs->ebd_fault) ? LED_ON : LED_OFF;
}

/**
 * 机油压力低：硬线低保持 CFG_OIL_PRESS_DELAY_MS（2s）后点亮
 *
 * 原实现 `s_oil_delay_ms += 10u`，而 LED_Task 的登记周期是 20ms，
 * 2000ms 的配置需要 200 次调用 × 20ms = 4000ms 才满足，叠加调度抖动后
 * 实测约 5s。改用时间戳后与调用周期解耦，配置多少就是多少。
 */
static void LED_UpdateOilPress(ClusterSystem_t *sys)
{
    if (!sys->hw_input.oil_press_n) {
        sys->led.oil_press_low = LED_OFF;
        s_oil_timing = false;
        return;
    }

    if (!s_oil_timing) {
        s_oil_timing   = true;
        s_oil_start_ms = sys->system_tick_ms;
    }

    sys->led.oil_press_low =
        ((sys->system_tick_ms - s_oil_start_ms) >= CFG_OIL_PRESS_DELAY_MS)
        ? LED_ON : LED_OFF;
}

/**
 * 燃气泄漏指示灯 + 蜂鸣器联动：
 *  - 检测到泄漏：点亮 + 触发 BUZZER_GAS_LEAK
 *  - 传感器错误：仅点亮，不触发蜂鸣
 *  - 正常：熄灭
 */
static void LED_UpdateGasLeak(ClusterSystem_t *sys)
{
    if (sys->can_rx.cng.timeout) {
        sys->led.gas_leak = LED_OFF;
        return;
    }
    GasLeakStatus_t status = sys->can_rx.cng.leak_status;
    if (status == GAS_LEAK || status == GAS_ERROR) {
        sys->led.gas_leak = LED_ON;
        if (status == GAS_LEAK) {
            BUZZER_Trigger(sys, BUZZER_GAS_LEAK);
        }
    } else {
        sys->led.gas_leak = LED_OFF;
    }
}

/*===========================================================================
 *  下发到 Meter 图标段（替代原 HAL_LED_Set / HAL_LED_SetBlink）
 *===========================================================================*/
void LED_DriveAll(const ClusterSystem_t *sys)
{
    const LedStatus_t *led = &sys->led;

    /*
     * 闪烁状态由软件节拍推导，不依赖硬件 PWM：
     *   blink_1hz：来自 sys->blink_1hz_visible（SYS_Task 每 500ms 翻转）
     *   blink_2hz：由 system_tick_ms 推导，每 250ms 翻转（= 2Hz）
     */
    bool blink_1hz = sys->blink_1hz_visible;
    bool blink_2hz = ((sys->system_tick_ms / 250u) & 1u) != 0u;

    /*
     * LED_TO_ON(state)：把 LedState_t → bool（当前是否点亮）
     *   LED_OFF       → false
     *   LED_ON        → true
     *   LED_BLINK_1HZ → blink_1hz（随 1Hz 节拍闪烁）
     *   LED_BLINK_2HZ → blink_2hz（随 2Hz 节拍闪烁）
     */
#define LED_TO_ON(state) \
    ( ((state) == LED_ON)        ? true      : \
      ((state) == LED_BLINK_1HZ) ? blink_1hz : \
      ((state) == LED_BLINK_2HZ) ? blink_2hz : false )

#define DRIVE_LED(icon, state)  Meter_SetIcon((icon), LED_TO_ON(state))

    DRIVE_LED(METER_ICON_TURN_LEFT,       led->turn_left);
    DRIVE_LED(METER_ICON_TURN_RIGHT,      led->turn_right);
    DRIVE_LED(METER_ICON_FOG_FRONT,       led->fog_front);
    DRIVE_LED(METER_ICON_LOW_BEAM,        led->low_beam);
    DRIVE_LED(METER_ICON_HIGH_BEAM,       led->high_beam);
    DRIVE_LED(METER_ICON_FOG_REAR,        led->fog_rear);
    DRIVE_LED(METER_ICON_FUEL_LOW,        led->fuel_low);

    /* CNG 报警灯只有一个物理段（METER_ICON_CNG_ALARM / R8），但驱动它的
     * 逻辑状态有两个：低气量报警（cng_warn）和系统故障（cng_fault）。
     * 不能各自 DRIVE_LED——那样后一条会直接覆盖前一条。这里先分别转成
     * on/off，按位或合并后只驱动一次。 */
    {
        bool cng_warn_on  = LED_TO_ON(led->cng_warn);
        bool cng_fault_on = LED_TO_ON(led->cng_fault);
        Meter_SetIcon(METER_ICON_CNG_ALARM, cng_warn_on || cng_fault_on);
    }

    DRIVE_LED(METER_ICON_GPF_YELLOW,      led->gpf_warn);
    DRIVE_LED(METER_ICON_GPF_GREEN,       led->gpf_regen);
    DRIVE_LED(METER_ICON_GPF_RED,         led->gpf_red);
    DRIVE_LED(METER_ICON_AIRBAG_FAULT,    led->airbag_fault);
    DRIVE_LED(METER_ICON_EPS_FAULT,       led->eps_fault);
    DRIVE_LED(METER_ICON_TPMS_WARN,       led->tpms_warn);
    DRIVE_LED(METER_ICON_ABS_FAULT,       led->abs_fault);
    DRIVE_LED(METER_ICON_SEATBELT_WARN,   led->seatbelt_warn);
    DRIVE_LED(METER_ICON_WATER_TEMP_HIGH, led->water_temp_high);
    DRIVE_LED(METER_ICON_CHARGER_FAULT,   led->charger_fault);
    DRIVE_LED(METER_ICON_DOOR_OPEN,       led->door_open);
    DRIVE_LED(METER_ICON_POS_LIGHT,       led->pos_light);
    DRIVE_LED(METER_ICON_ENGINE_FAULT,    led->engine_fault);
    DRIVE_LED(METER_ICON_EBD_FAULT,       led->ebd_fault);
    DRIVE_LED(METER_ICON_BRAKE_FAULT,     led->brake_fault);
    DRIVE_LED(METER_ICON_OIL_PRESS_LOW,   led->oil_press_low);
    DRIVE_LED(METER_ICON_GAS_LEAK,        led->gas_leak);

#undef DRIVE_LED
#undef LED_TO_ON

    /*
     * 把指示灯本次更新推送到 SC5037 DDRAM。
     * LED_Task 每 10ms 调用此函数，指示灯响应延迟 ≤ 10ms。
     * LCD_Task（50ms）的 Meter_Refresh() 会再次全量推送，
     * 两次刷新共用同一帧缓冲，内容始终一致，无需额外同步。
     */
    Meter_Refresh();
}