/**
 * @file  pwr_mgr.c
 * @brief 电源状态机（修复default分支误杀ACC/START状态）
 *
 * ===========================================================================
 * 本次修改：功放使能延时解耦（阻塞 → 非阻塞）
 * ===========================================================================
 * 原实现在 PWR_OFF → PWR_ON 跳变里直接调用 SC5037_HAL_Delay_Ms(5000)，
 * 5 秒内整个调度器停摆：CAN_RxTask 不跑、LCD 不刷、按键无响应，而
 * system_tick_ms 照常走，恢复瞬间所有 CAN 报文会同时判超时。
 *
 * 现改为记录时刻 + 周期轮询，延时期间其余任务照常运行。
 * **上下电顺序与 5 秒时长均未改变。**
 *
 * ★★ 关键实现约束 ★★
 *   PWR_AmpDelayPoll() 必须在 switch 之前**无条件**调用。
 *   若把它写进某个 case 里，状态机稳定在 PWR_ON 之后就再也不会进入该
 *   分支，PTA1 永远不会被拉高 —— 这正是上一版非阻塞改造导致功放不
 *   工作的原因。改动本文件时请勿移动这一行的位置。
 *
 * ===========================================================================
 * 本次修改 A：补上 IGN OFF/ACC → ON 的上电自检触发
 * ===========================================================================
 * sys_mgr.h / sys_mgr.c 里已经写明 SYS_StartSelfCheck() 应"由 PWR_Task 在
 * 进入 PWR_ON 的跳变上调用"，但本文件一直没有真正调用过，实际效果是：
 * 自检只在 Cluster_Init() 里发生一次。仪表挂蓄电池常电时，这一次自检发生
 * 在接蓄电池的瞬间，之后钥匙 OFF→ON 不会再自检。
 *
 * 现在 PWR_Task 里对 IGN 硬线做去抖 + 边沿检测，确认的 OFF→ON 边沿上调用
 * SYS_StartSelfCheck()，重新走一轮 CFG_SELFCHECK_DURATION_MS 自检。
 * 检测直接看硬线 ign_on、不看 power_state，因此 OFF→ON、ACC→ON 都能触发。
 *
 * ===========================================================================
 * 本次修改 B：电压超出 9~18V 时切断 SOC / 收音机 / 功放供电
 * ===========================================================================
 * 规格要求电压不在 9~18V 时关闭这三路供电。判定直接复用 voltage.c 的
 * sys->voltage.normal（已含 ±0.2V 迟滞 + 200ms 确认），不在本文件重新
 * 造一套阈值。
 *
 * 为此把三路供电的引脚操作从 switch 里挪到 switch **之后**，按一条规则
 * 统一执行：
 *
 *     供电目标 = (power_state == PWR_ON) && 电压许可
 *
 * 这段代码每周期无条件跑一次，只在目标发生变化时动引脚。这样"点火关"和
 * "电压超范围"两条断电路径合并成一条，上下电顺序只有一份实现，不会出现
 * 两边不一致。**上电顺序与 5s 功放延时均未改变。**
 *
 * ★ 为什么必须放在 switch 外面 ★
 *   电压是在状态机**不发生任何跳变**的情况下变化的。若把通断判定写进
 *   某个 case，状态稳定在 PWR_ON 之后就再也不会进入该分支，电压超范围
 *   永远等不到断电 —— 与 PWR_AmpDelayPoll() 是同一类坑。
 *
 * 顺带修掉一个老问题：原 PWR_ON → PWR_OFF 分支只关了 SOC 和收音机，
 * 忘了把功放使能 PTA1 拉低，钥匙关掉后功放使能一直挂在高电平。
 *
 * 注：断电前不向山景 SOC 发关机通知（暂不需要），是硬切。
 *
 * ===========================================================================
 * 本次修改 C：与低功耗模块（app/power/lowpower.c）的分工
 * ===========================================================================
 * 新增的 lowpower.c 负责 "MCU 自己睡不睡"，本文件负责 "给谁供电"，两者
 * 边界如下，改动任一侧前请先确认没有越界：
 *
 *   本文件（pwr_mgr.c）
 *     - IGN 状态机、power_state 的唯一写入者
 *     - SOC / 收音机 / 功放三路供电的唯一控制者
 *     - IGN ON→OFF 时关背光 + 存里程
 *   lowpower.c
 *     - 只读 power_state / hw_input，**从不修改 power_state**
 *     - 在三路供电已经断开之后，才决定要不要让 MCU 进 DeepSleep
 *     - 唤醒后调用 PWR_Init() 重建本文件的全部静态状态
 *
 * ★ 为什么唤醒后要调 PWR_Init() 而不是什么都不做 ★
 *   本文件用了一批 static 变量做 IGN 去抖（s_ign_debounced / s_ign_raw_ms
 *   等）。休眠期间 system_tick_ms 是冻结的，这些时刻值醒来后全部失去意义。
 *   更关键的是：若被 IGN 唤醒时 s_ign_debounced 还停留在睡前的 false 之外
 *   的值，OFF→ON 边沿会被漏掉，这次点火就不做指示灯自检。
 *   PWR_Init() 把它们全部拉回"上电初值"，等价于重新走一次点火循环。
 */
#include "pwr_mgr.h"
#include "sys_mgr.h"         /* SYS_StartSelfCheck()：IGN OFF→ON 重新自检 */
#include "voltage.h"         /* sys->voltage：9~18V 供电门限判定           */
#include "cfg_power.h"       /* CFG_PWR_RAIL_*_MS                          */
#include "hal_backlight.h"   /* 背光关闭：HAL_BACKLIGHT_Set(0u)；
                              * 段码显示已由 meter_display.h 统一管理，
                              * 此处不再包含 hal_lcd.h。               */
#include "nvm_storage.h"
#include "pins_driver.h"
#include "sc5037.h"          /* 已不再使用 SC5037_HAL_Delay_Ms()，
                              * 保留 include 以免影响其它依赖。       */

/*---------------------------------------------------------------------------
 *  音频通路使能引脚
 *-------------------------------------------------------------------------*/
#define PWR_AMP_PORT        GPIOA       /**< 功放 VA2221L 使能 → PTA1 */
#define PWR_AMP_PIN         1u
#define PWR_SOC_5V_PORT     GPIOB       /**< 山景 SOC 5V 供电  → PTB3 */
#define PWR_SOC_5V_PIN      3u
#define PWR_RADIO_PORT      GPIOE       /**< 收音机 3.3V 供电  → PTE0 */
#define PWR_RADIO_PIN       0u

/* 功放使能延时：等山景 SOC 输出稳定后再使能，时长不足会导致功放进保护。
 * 若确认硬件只需更短时间，只改这一个数。 */
#ifndef CFG_PWR_AMP_ENABLE_DELAY_MS
#define CFG_PWR_AMP_ENABLE_DELAY_MS     5000u
#endif

/*---------------------------------------------------------------------------
 *  非阻塞延时状态
 *-------------------------------------------------------------------------*/
static bool s_amp_pending  = false;     /**< 功放使能等待中     */
static u32  s_amp_start_ms = 0u;        /**< 供电拉高的时刻(ms) */

/*---------------------------------------------------------------------------
 *  IGN 边沿检测状态（仅用于自检触发，不参与电源状态机判定）
 *-------------------------------------------------------------------------*/
static bool s_ign_debounced   = false;  /**< 去抖后的 IGN 电平        */
static bool s_ign_raw_last    = false;  /**< 上一周期的原始 IGN 电平  */
static u32  s_ign_raw_ms      = 0u;     /**< 原始电平最近一次变化时刻 */
static u32  s_ign_off_ms      = 0u;     /**< 去抖后进入 OFF 的时刻    */
static bool s_ign_off_timing  = false;  /**< OFF 保持计时中           */

/*---------------------------------------------------------------------------
 *  三路外设供电（SOC / 收音机 / 功放）的当前状态与电压门限去抖
 *-------------------------------------------------------------------------*/
static bool s_rails_on       = false;   /**< 三路供电当前是否已打开   */
static bool s_rail_volt_ok   = true;    /**< 去抖后的"电压允许供电"   */
static bool s_rail_volt_cand = true;    /**< 候选值                   */
static u32  s_rail_volt_ms   = 0u;      /**< 候选值起始时刻           */

void PWR_Init(ClusterSystem_t *sys)
{
    sys->power_state = PWR_OFF;
    s_amp_pending    = false;
    s_amp_start_ms   = 0u;

    /* IGN 边沿检测复位。
     * 认为上电时 IGN 处于 OFF：若钥匙本来就在 ON 档（例如带电插钥匙 /
     * 仪表复位），去抖满足后会补一次 OFF→ON 边沿，重新触发自检——
     * 这与 SYS_Init() 里那次自检重叠，只是把起点后移几十 ms，无副作用。 */
    s_ign_debounced  = false;
    s_ign_raw_last   = false;
    s_ign_raw_ms     = 0u;
    s_ign_off_ms     = 0u;
    s_ign_off_timing = false;

    /* 供电门限复位：还没测出电压前按"允许"处理，避免上电瞬间误断电 */
    s_rail_volt_ok   = true;
    s_rail_volt_cand = true;
    s_rail_volt_ms   = 0u;

    /* 三路供电显式拉低，与 power_state = PWR_OFF 对齐，
     * 不依赖 PINS_DRV_Init() 的引脚默认电平 */
    s_rails_on = false;
    PINS_DRV_WritePin(PWR_AMP_PORT,    PWR_AMP_PIN,    0);
    PINS_DRV_WritePin(PWR_SOC_5V_PORT, PWR_SOC_5V_PIN, 0);
    PINS_DRV_WritePin(PWR_RADIO_PORT,  PWR_RADIO_PIN,  0);
}

/*---------------------------------------------------------------------------
 *  功放使能延时轮询
 *
 *  必须每个 PWR_Task 周期都调用一次（见文件头的实现约束）。
 *-------------------------------------------------------------------------*/
static void PWR_AmpDelayPoll(ClusterSystem_t *sys)
{
    if (!s_amp_pending) { return; }

    /* 延时期间若供电已被切断（点火被拔 / 电压超范围），取消使能。
     * 原来判的是 power_state，现在改判 s_rails_on，把"电压超范围断电"
     * 这条新路径也一并覆盖。 */
    if (!s_rails_on) {
        s_amp_pending = false;
        return;
    }

    /* 无符号相减，tick 回绕安全 */
    if ((u32)(sys->system_tick_ms - s_amp_start_ms) >= CFG_PWR_AMP_ENABLE_DELAY_MS) {
        PINS_DRV_WritePin(PWR_AMP_PORT, PWR_AMP_PIN, 1);   /* 功放VA2221L使能 */
        s_amp_pending = false;
    }
}

void PWR_Task(ClusterSystem_t *sys)
{
    const HardwireInput_t *hw   = &sys->hw_input;
    const PowerVoltage_t  *v    = &sys->voltage;
    const u32              now  = sys->system_tick_ms;
    const bool             ign  = hw->ign_on;
    bool wake_hw    = (hw->door_open_n || hw->turn_left || hw->turn_right);
    bool rails_want;

    /* ★ 必须在 switch 之前，每周期无条件执行，勿移入任何 case ★ */
    PWR_AmpDelayPoll(sys);

    /*=======================================================================
     *  1) IGN 去抖 + OFF→ON 边沿 → 重新启动一轮指示灯自检
     *
     *  两级过滤（时间常数见 cfg_selfcheck.h）：
     *    a) 去抖：原始电平必须连续保持 CFG_SELFCHECK_IGN_DEBOUNCE_MS 才被
     *       采纳。hw_input.ign_on 是裸读 GPIO，钥匙触点抖动会被 1ms 采样
     *       完整看到，不去抖会让自检被反复从头拉起，指示灯一直不灭。
     *    b) OFF 保持：OFF 至少稳定 CFG_SELFCHECK_IGN_OFF_HOLD_MS 才算一次
     *       新的点火循环，滤掉起机拖动时 IGN 的瞬时跌落（否则着车瞬间会
     *       再自检一次，表现为故障灯全亮一下）。
     *
     *  SYS_StartSelfCheck() 只改 selfcheck_* 三个字段，是幂等的：
     *    - LED_Task  看到 selfcheck_done==false 会把自检清单里的灯覆盖为常亮，
     *                清单外的灯（转向/安全带/车门/气囊/燃气泄漏等）仍实时显示，
     *                报警音也照常播放（清单见 cfg_selfcheck.h）
     *    - FUEL / CNG / TPMS 自检期间不读真实值，避免上电跳格
     *    - CAN 上线宽限期以 selfcheck_start_ms 为基准，一并顺延，
     *      不会出现"自检刚结束就把所有报文判超时"的误报警
     *=====================================================================*/
    if (ign != s_ign_raw_last) {
        s_ign_raw_last = ign;
        s_ign_raw_ms   = now;                   /* 原始电平变了：重新去抖 */
    }

    if ((ign != s_ign_debounced) &&
        ((u32)(now - s_ign_raw_ms) >= CFG_SELFCHECK_IGN_DEBOUNCE_MS)) {

        s_ign_debounced = ign;

        if (s_ign_debounced) {
            /* OFF → ON：OFF 保持够久才算新的点火循环 */
            if ((!s_ign_off_timing) ||
                ((u32)(now - s_ign_off_ms) >= CFG_SELFCHECK_IGN_OFF_HOLD_MS)) {
                SYS_StartSelfCheck(sys);
            }
            s_ign_off_timing = false;
        } else {
            /* ON → OFF：开始计 OFF 保持时长 */
            s_ign_off_ms     = now;
            s_ign_off_timing = true;
        }
    }

    /*=======================================================================
     *  2) 供电电压门限：9V ≤ V ≤ 18V 才允许给 SOC / 收音机 / 功放供电
     *
     *  判定源是 v->normal（voltage.c 产出，已含 ±0.2V 迟滞 +
     *  CFG_VOLTAGE_DEBOUNCE_MS 确认），这里只再叠加一层"断/通"不对称的
     *  确认时间，理由见 cfg_power.h：
     *    - 过压：CFG_PWR_RAIL_OVER_OFF_MS （默认 0）     → 立即断，保护器件
     *    - 欠压：CFG_PWR_RAIL_UNDER_OFF_MS（默认 500ms） → 躲开起机拖动跌落
     *    - 恢复：CFG_PWR_RAIL_ON_MS       （默认 1000ms）→ 避免门限附近反复上下电
     *
     *  ★ 失效安全策略 ★
     *  ADC 还没测出有效电压（VOLT_STATE_INIT）时按"允许供电"处理，避免
     *  仪表一上电就把收音机断掉。若希望反过来（测不到电压就不供电），
     *  把下面 VOLT_STATE_INIT 分支里的 true 改成 false 即可。
     *=====================================================================*/
    if (v->state == VOLT_STATE_INIT) {
        /* 尚未测出有效电压：维持允许，且不启动任何确认计时 */
        s_rail_volt_ok   = true;
        s_rail_volt_cand = true;
        s_rail_volt_ms   = now;
    } else if (v->normal == s_rail_volt_ok) {
        /* 与当前结论一致：清候选计时 */
        s_rail_volt_cand = v->normal;
        s_rail_volt_ms   = now;
    } else {
        u32 confirm_ms;

        if (v->normal != s_rail_volt_cand) {
            s_rail_volt_cand = v->normal;       /* 候选变了：重新计时 */
            s_rail_volt_ms   = now;
        }

        if (v->normal) {
            confirm_ms = CFG_PWR_RAIL_ON_MS;                /* 恢复 */
        } else if (v->state == VOLT_STATE_OVER) {
            confirm_ms = CFG_PWR_RAIL_OVER_OFF_MS;          /* 过压 */
        } else {
            confirm_ms = CFG_PWR_RAIL_UNDER_OFF_MS;         /* 欠压 */
        }

        /* 无符号相减，tick 回绕安全 */
        if ((u32)(now - s_rail_volt_ms) >= confirm_ms) {
            s_rail_volt_ok = v->normal;
        }
    }

    /*=======================================================================
     *  3) 电源状态机（只切状态，不再直接动供电引脚）
     *=====================================================================*/
    switch (sys->power_state) {
        case PWR_OFF:
            if (ign)
            {
                sys->power_state=PWR_ON;
                sys->cluster_state=CLUSTER_WAKE;
                /* SOC / 收音机 / 功放的实际上电由下面第 4 段统一执行
                 * （顺序与 5s 功放延时不变），这里只切状态。 */
            }
            else if (hw->bat_on || wake_hw) { sys->cluster_state=CLUSTER_WAKE; }
            break;
        case PWR_ON:
            if (!ign) {
                sys->power_state=PWR_OFF;
                /* 三路供电的关闭同样交给下面第 4 段 */
                HAL_BACKLIGHT_Set(0u);          /* 关闭背光 */
                NVM_SaveOdometer(sys);
            }
            break;
        case PWR_ACC:
            if (ign)              { sys->power_state=PWR_ON; sys->cluster_state=CLUSTER_WAKE; }
            else if (!hw->bat_on) { sys->power_state=PWR_OFF; }
            break;
        case PWR_START:
            sys->power_state = PWR_ON;
            break;
        default:
            sys->power_state = PWR_OFF;
            break;
    }

    /*=======================================================================
     *  4) 三路外设供电通断
     *
     *      供电目标 = (power_state == PWR_ON) && 电压在 9~18V 内
     *
     *  ★ 必须在 switch 之后、每周期无条件执行，勿移入任何 case ★
     *  放在 switch 之后是为了让本周期刚发生的状态跳变立即生效，
     *  不用多等一个 5ms 周期。
     *
     *  ★ 上电顺序（与原实现完全一致，勿改）★
     *      SOC 5V(PTB3) + 收音机 3.3V(PTE0) 先上电
     *        → 等 CFG_PWR_AMP_ENABLE_DELAY_MS(5s)
     *        → 功放使能 PTA1 拉高（由 PWR_AmpDelayPoll 到点执行）
     *
     *  ★ 下电顺序 ★
     *      功放使能 PTA1 先拉低 → 再切 SOC / 收音机供电。
     *      先关功放可以避免供电塌陷时功放输出"砰"的一声。
     *=====================================================================*/
    rails_want = ((sys->power_state == PWR_ON) && s_rail_volt_ok);

    if (rails_want != s_rails_on) {
        s_rails_on = rails_want;

        if (rails_want) {
            /* 打开山景SOC供电5V->PTB3拉高 */
            PINS_DRV_WritePin(PWR_SOC_5V_PORT, PWR_SOC_5V_PIN, 1);
            /* 打开收音机供电 PTE0拉高3.3v */
            PINS_DRV_WritePin(PWR_RADIO_PORT,  PWR_RADIO_PIN,  1);
            /* 原 SC5037_HAL_Delay_Ms(5000) 改为非阻塞计时：
             * 记录时刻，功放使能交给 PWR_AmpDelayPoll() 到点再拉高。 */
            s_amp_start_ms = now;
            s_amp_pending  = true;
        } else {
            s_amp_pending = false;              /* 取消未到时的功放使能 */
            /* 关闭功放使能 PTA1。
             * 【修复】原实现漏了这一句，钥匙关掉后 PTA1 一直挂在高电平。 */
            PINS_DRV_WritePin(PWR_AMP_PORT,    PWR_AMP_PIN,    0);
            /* 关闭山景SOC供电5V->PTB3拉低 */
            PINS_DRV_WritePin(PWR_SOC_5V_PORT, PWR_SOC_5V_PIN, 0);
            /* 关闭收音机供电 PTE0拉低3.3v */
            PINS_DRV_WritePin(PWR_RADIO_PORT,  PWR_RADIO_PIN,  0);
        }
    }
}