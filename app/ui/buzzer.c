/**
 * @file    buzzer.c
 * @brief   蜂鸣器报警模块实现
 *
 * 修改点（本次）：
 *   1) 安全带报警改为"条件驱动"：
 *        车速 <= CFG_SPEED_SEATBELT_WARN_KMH 或安全带已系 → 立即停止并复位
 *        车速 >  CFG_SPEED_SEATBELT_WARN_KMH 且未系      → 立即开始 5 个循环
 *   2) 5 个循环跑完后置 seatbelt_latched 闭锁，条件保持满足时不再重复；
 *      条件一旦不满足（减速 / 系上安全带）即解除闭锁，再次满足时重新报警。
 *   3) 第 5 轮鸣叫结束立即收工，不再空等尾部那段 9s 静音。
 *
 * ===========================================================================
 * 本次新增：IGN 下电 → 安全带报警音关闭
 * ===========================================================================
 *  需求：点火开关下电（IGN OFF）后，安全带未系报警音立即停止。
 *
 *  原实现只看"未系 + 车速 > 5km/h"两个条件，不看点火状态。钥匙关掉后：
 *    - hw_input.seatbelt_n 仍由 HW_SampleTask（1ms）继续采样，保持"未系";
 *    - lcd.speed_kmh 不会瞬间归零（ABS 报文停发后要等 CAN 超时判定 +
 *      车速平滑衰减），滑行熄火时更是明显大于 5km/h；
 *  两个条件同时成立，蜂鸣器会在熄火后继续鸣叫，直到车速判到 0 或者
 *  5 轮循环跑完为止。
 *
 *  现把"IGN ON"并入 buzzer_seatbelt_cond()：条件不成立时走的仍是原来那条
 *  "立即停止 + 解除闭锁"路径（BUZZER_Task 第 1 步），因此下电瞬间 HAL_Buzzer_Off()
 *  会被立刻调用，哪怕正处在一声鸣叫的中间；解除闭锁保证下一次 IGN ON
 *  是一个全新的报警循环。
 *
 *  IGN 电平经 CFG_BUZZER_IGN_DEBOUNCE_MS 去抖后使用，避免起机（START）
 *  拖动时 IGN 的瞬时跌落误判成一次下电。
 *
 *  ⚠ 本次改动**只作用于安全带报警**。燃气泄漏报警（BUZZER_GAS_LEAK）不受
 *    IGN 影响：泄漏是安全事件，熄火后仍应能鸣叫报警。若产品要求熄火后
 *    一并静音，把 BUZZER_Task 第 0 步的注释掉的分支打开即可。
 */

#include "buzzer.h"
#include "hal_buzzer.h"

/*---------------------------------------------------------------------------
 *  内部：IGN 电平去抖（★本次新增★）
 *
 *  hw_input.ign_on 是裸读 GPIO，没有任何去抖。这里做一层"电平连续保持
 *  CFG_BUZZER_IGN_DEBOUNCE_MS 才采纳"的过滤，输出 s_ign_on 供
 *  buzzer_seatbelt_cond() 使用。
 *
 *  用 sys->system_tick_ms 做绝对时间差，与 BUZZER_Task 的调用周期无关；
 *  无符号相减，tick 回绕安全。
 *
 *  与 pwr_mgr.c 里那套 IGN 去抖是相互独立的两份状态，互不干扰：
 *  那边决定"什么时候重新自检 / 给谁供电"，这边只决定"什么时候闭嘴"。
 *---------------------------------------------------------------------------*/
static bool s_ign_on       = false;     /**< 去抖后的 IGN 电平          */
static bool s_ign_raw_last = false;     /**< 上一次的原始 IGN 电平      */
static u32  s_ign_raw_ms   = 0u;        /**< 原始电平最近一次变化时刻   */

/** 供 BUZZER_Init() 复位去抖状态用（以当前电平为初值） */
static void buzzer_ign_reset(const ClusterSystem_t *sys)
{
    s_ign_on       = sys->hw_input.ign_on;
    s_ign_raw_last = sys->hw_input.ign_on;
    s_ign_raw_ms   = sys->system_tick_ms;
}

static void buzzer_update_ign(const ClusterSystem_t *sys)
{
    bool raw = sys->hw_input.ign_on;

    if (raw != s_ign_raw_last) {
        s_ign_raw_last = raw;
        s_ign_raw_ms   = sys->system_tick_ms;    /* 电平变了：重新计时 */
    }

    if ((raw != s_ign_on) &&
        ((u32)(sys->system_tick_ms - s_ign_raw_ms) >= CFG_BUZZER_IGN_DEBOUNCE_MS)) {
        s_ign_on = raw;
    }
}

/*===========================================================================
 *  初始化
 *===========================================================================*/
void BUZZER_Init(ClusterSystem_t *sys)
{
    BuzzerCtrl_t *bz = &sys->buzzer;
    bz->active_alarm     = BUZZER_IDLE;
    bz->pending_restore  = BUZZER_IDLE;
    bz->beep_count       = 0u;
    bz->cycle_count      = 0u;
    bz->timer_ms         = 0u;
    bz->buzzer_on        = false;
    bz->seatbelt_latched = false;

    /* ★本次新增★ IGN 去抖状态复位：以**当前**电平为初值，
     * 而不是无脑写 false，避免刚初始化完还要再等一个去抖周期
     * 才认得出 IGN 已经是 ON。 */
    buzzer_ign_reset(sys);

    HAL_Buzzer_Off();
}

/*---------------------------------------------------------------------------
 *  内部：根据报警类型给出半周期长度（ms）
 *---------------------------------------------------------------------------*/
static u32 buzzer_half_period_ms(BuzzerAlarm_t alarm)
{
    switch (alarm) {
        case BUZZER_GAS_LEAK: return 1000u / CFG_BUZZER_GAS_FREQ_HZ      / 2u;
        case BUZZER_SEATBELT: return 1000u / CFG_BUZZER_SEATBELT_FREQ_HZ / 2u;
        default:              return 500u;
    }
}

/*---------------------------------------------------------------------------
 *  内部：安全带报警条件（IGN ON + 未系 + 车速高于阈值）
 *
 *  注：seatbelt_n == true 表示"未系"（hw_input.c 中取反读脚）。
 *
 *  ★本次新增 s_ign_on 一项★
 *  IGN 下电后本函数立刻返回 false，BUZZER_Task 第 1 步随即调用
 *  buzzer_seatbelt_stop() 关掉蜂鸣器并解除闭锁 —— 这就是
 *  "IGN 下电 → 安全带报警音关闭" 的全部实现，不需要额外的状态分支。
 *---------------------------------------------------------------------------*/
static bool buzzer_seatbelt_cond(const ClusterSystem_t *sys)
{
    return (s_ign_on != false)                                  /* ★ IGN ON  */
        && (sys->hw_input.seatbelt_n != false)                  /*   未系     */
        && (sys->lcd.speed_kmh > CFG_SPEED_SEATBELT_WARN_KMH);  /*   车速     */
}

/*---------------------------------------------------------------------------
 *  内部：停止安全带报警并复位计数（不改 seatbelt_latched）
 *---------------------------------------------------------------------------*/
static void buzzer_seatbelt_stop(BuzzerCtrl_t *bz)
{
    HAL_Buzzer_Off();
    bz->active_alarm    = BUZZER_IDLE;
    bz->pending_restore = BUZZER_IDLE;
    bz->beep_count      = 0u;
    bz->cycle_count     = 0u;
    bz->timer_ms        = 0u;
    bz->buzzer_on       = false;
}

/*===========================================================================
 *  外部触发入口
 *===========================================================================*/
void BUZZER_Trigger(ClusterSystem_t *sys, BuzzerAlarm_t alarm)
{
    BuzzerCtrl_t *bz = &sys->buzzer;

    /* 已有更高或同等优先级报警在执行 → 忽略 */
    if (bz->active_alarm != BUZZER_IDLE && alarm >= bz->active_alarm) {
        return;
    }
    /* 当前是安全带：被抢占后由 BUZZER_Task 依实时条件自动重启，
     * 此处仅保留标记供外部查询。 */
    if (bz->active_alarm == BUZZER_SEATBELT) {
        bz->pending_restore = BUZZER_SEATBELT;
    }
    /* 切换到新报警，重置计数 */
    bz->active_alarm = alarm;
    bz->beep_count   = 0u;
    bz->cycle_count  = 0u;
    bz->timer_ms     = 0u;
    bz->buzzer_on    = false;
}

/*---------------------------------------------------------------------------
 *  子状态机：燃气泄漏（2Hz × 12 声后停止）
 *---------------------------------------------------------------------------*/
static void buzzer_run_gas_leak(BuzzerCtrl_t *bz)
{
    bz->timer_ms += 10u;
    if (bz->timer_ms >= buzzer_half_period_ms(BUZZER_GAS_LEAK)) {
        bz->timer_ms  = 0u;
        bz->buzzer_on = !bz->buzzer_on;
        if (!bz->buzzer_on) { bz->beep_count++; }   /* 完整一声 = 一上一下 */
    }
    bz->buzzer_on ? HAL_Buzzer_On() : HAL_Buzzer_Off();

    /* 鸣叫足够次数后停止 */
    if (bz->beep_count >= CFG_BUZZER_GAS_BEEPS) {
        HAL_Buzzer_Off();
        bz->active_alarm = BUZZER_IDLE;
        bz->beep_count   = 0u;
        bz->buzzer_on    = false;
    }
}

/*---------------------------------------------------------------------------
 *  子状态机：安全带（1Hz × 6 声 → 停 9s，共 5 轮）
 *
 *  cycle_count 偶数：发声阶段
 *  cycle_count 奇数：静音阶段
 *  时序：鸣-静-鸣-静-鸣-静-鸣-静-鸣 → 结束（5 轮鸣叫，中间 4 段静音）
 *
 *  进入条件由 BUZZER_Task 保证（未系 + 车速 > 阈值），本函数不再自行判定。
 *---------------------------------------------------------------------------*/
static void buzzer_run_seatbelt(BuzzerCtrl_t *bz)
{
    bz->timer_ms += 10u;

    if ((bz->cycle_count & 1u) == 0u) {
        /* --- 发声阶段 --- */
        if (bz->timer_ms >= buzzer_half_period_ms(BUZZER_SEATBELT)) {
            bz->timer_ms  = 0u;
            bz->buzzer_on = !bz->buzzer_on;
            if (!bz->buzzer_on) { bz->beep_count++; }
        }
        bz->buzzer_on ? HAL_Buzzer_On() : HAL_Buzzer_Off();

        if (bz->beep_count >= CFG_BUZZER_SEATBELT_BEEPS) {
            HAL_Buzzer_Off();
            bz->buzzer_on  = false;
            bz->beep_count = 0u;
            bz->timer_ms   = 0u;
            bz->cycle_count++;

            /* 最后一轮鸣叫结束 → 立即收工并闭锁，不再等尾部 9s 静音 */
            if (bz->cycle_count >= (CFG_BUZZER_SEATBELT_CYCLES * 2u - 1u)) {
                bz->active_alarm     = BUZZER_IDLE;
                bz->pending_restore  = BUZZER_IDLE;
                bz->cycle_count      = 0u;
                bz->seatbelt_latched = true;
            }
        }
    } else {
        /* --- 静音阶段（9s） --- */
        HAL_Buzzer_Off();
        if (bz->timer_ms >= CFG_BUZZER_SEATBELT_PAUSE_MS) {
            bz->timer_ms = 0u;
            bz->cycle_count++;          /* 进入下一发声阶段 */
        }
    }
}

/*===========================================================================
 *  周期任务（10ms）
 *===========================================================================*/
/**
 * @note 本任务**有意不受上电自检影响**（不判 sys->selfcheck_done）。
 *       需求：自检期间安全带未系、燃气泄漏等报警音照常播放。
 *       触发源同样不受自检影响：
 *         - 安全带：本函数每周期按 hw_input.seatbelt_n + 车速实时判定；
 *         - 燃气泄漏：led_ind.c::LED_UpdateGasLeak 每周期调用 BUZZER_Trigger，
 *           该函数在自检期间也会执行（自检早退逻辑已取消）。
 */
void BUZZER_Task(ClusterSystem_t *sys)
{
    BuzzerCtrl_t *bz = &sys->buzzer;

    /* --- 0) IGN 电平去抖（★本次新增★，必须在条件评估之前） --- */
    buzzer_update_ign(sys);

    /* 若产品要求"IGN 下电后连燃气泄漏报警也一并静音"，打开下面这段：
     *
     *  if (!s_ign_on) {
     *      HAL_Buzzer_Off();
     *      bz->active_alarm     = BUZZER_IDLE;
     *      bz->pending_restore  = BUZZER_IDLE;
     *      bz->beep_count       = 0u;
     *      bz->cycle_count      = 0u;
     *      bz->timer_ms         = 0u;
     *      bz->buzzer_on        = false;
     *      bz->seatbelt_latched = false;
     *      return;
     *  }
     *
     * 当前按"只关安全带"实现：燃气泄漏是安全事件，熄火后仍允许鸣叫。 */

    /* --- 1) 安全带条件评估：每周期都判，保证即时起停 --- */
    if (!buzzer_seatbelt_cond(sys)) {
        /* IGN 下电 / 车速 <= 5km/h / 已系上 → 立即停止 + 解除闭锁 */
        bz->seatbelt_latched = false;
        if (bz->active_alarm == BUZZER_SEATBELT) {
            buzzer_seatbelt_stop(bz);
        }
        bz->pending_restore = BUZZER_IDLE;
    }
    else if (!bz->seatbelt_latched && bz->active_alarm == BUZZER_IDLE) {
        /* IGN ON + 未系 + 车速达标，且未闭锁、无其它报警占用
         * → 立即开始新一轮 5 循环
         * （燃气泄漏结束后也走这条路径自动恢复）                     */
        bz->active_alarm    = BUZZER_SEATBELT;
        bz->pending_restore = BUZZER_IDLE;
        bz->beep_count      = 0u;
        bz->cycle_count     = 0u;
        bz->timer_ms        = 0u;
        bz->buzzer_on       = false;
    }

    /* --- 2) 空闲 → 兜底关闭 --- */
    if (bz->active_alarm == BUZZER_IDLE) {
        HAL_Buzzer_Off();
        return;
    }

    /* --- 3) 派发到子状态机 --- */
    switch (bz->active_alarm) {
        case BUZZER_GAS_LEAK:
            buzzer_run_gas_leak(bz);
            break;
        case BUZZER_SEATBELT:
            buzzer_run_seatbelt(bz);
            break;
        default:
            HAL_Buzzer_Off();
            bz->active_alarm = BUZZER_IDLE;
            break;
    }
}