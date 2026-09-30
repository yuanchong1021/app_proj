/**
 * @file    cfg_lowpower.h
 * @brief   ACC OFF 低功耗（休眠 / 唤醒）参数配置
 *
 * 平台：YTM32B1MC0（Cortex-M33）。工程已引入 SDK 的 power_manager 组件
 *       （board/power_config.c/.h 由 YCT 生成，提供 RUN / SLEEP / DEEPSLEEP /
 *       STANDBY 四套配置），模式切换走 POWER_SYS_SetMode()。
 *       外设时钟门控 power_manager 不管，仍由 hal_lowpower.c 用 IPC 自己做。
 *
 * 指标来源：仪表硬件规格书 Section 1.2，
 *          静态电流 CFG_CURRENT_STATIC_MAX_UA = 2000uA（见 cfg_power.h）
 *
 * ===========================================================================
 *  设计前提
 * ===========================================================================
 *  1) 本仪表挂**蓄电池常电**，MCU 从不断电。ACC/IGN OFF 之后 MCU 仍满速跑
 *     21 个任务，整机电流就是规格书里的"静态电流"。低功耗不是可选项。
 *
 *  2) ★★★ 当前唤醒源只有 IGN（PTC7）一路 ★★★
 *
 *     这是项目的明确约束，不是实现上的偷懒。带来的直接后果：
 *       - 休眠后开车门 → 仪表不亮
 *       - 休眠后打双闪 → 转向指示图标不亮
 *       - 休眠后整车网络被唤醒（远程解锁 / T-BOX）→ 仪表不参与
 *     只有拧钥匙才能把仪表叫醒。这几条需要整车侧确认可接受，
 *     详见方案文档 5.4 节。
 *
 *     软件上，车门 / 转向 / CAN 仍然作为**休眠抑制**条件（醒着的时候不让睡），
 *     只是不作为**唤醒**条件（睡着了叫不醒）。这个不对称是有意的，
 *     见下面"休眠抑制条件"一节。
 *
 *     代码结构保留了另外三路的完整实现，各由一个宏控制（默认全关）。
 *     将来若整车要求增加唤醒源，把对应宏置 1 即可，不需要改逻辑。
 *
 *  3) 唤醒极性：IGN 在 hw_input.c 里是 `!HAL_GPIO_ReadPin()`，低电平有效，
 *     所以配**下降沿**。改极性前先看 hw_input.c 的取反逻辑，不要凭引脚名猜。
 *
 *  4) CAN 收发器 STB（PTE10）休眠时切 Standby。PTE10 在 pin_mux.c 里
 *     initValue = 0，即上电为 Normal，因此 **STB 高 = Standby**。
 *     由于不需要总线唤醒，这里只是单纯为了省掉收发器那 5~10mA，
 *     不依赖它的远程唤醒能力。
 *
 * 修改影响范围：app/power/lowpower.c、hal/hal_lowpower.c
 */

#ifndef CLUSTER_CFG_LOWPOWER_H
#define CLUSTER_CFG_LOWPOWER_H

/*===========================================================================
 *  共用类型
 *
 *  放在配置头里而不是 hal_lowpower.h，是因为 cluster_context.h（app 层）和
 *  hal_lowpower.h（HAL 层）都要用到，而 app/common/config 是两边都已经在
 *  包含路径上的唯一公共位置。若把类型放 HAL 头，app 层就得反向包含 HAL，
 *  破坏分层。
 *===========================================================================*/

/** 低功耗状态机状态 */
typedef enum {
    LPM_STATE_RUN       = 0,    /**< 正常运行                       */
    LPM_STATE_PRE_SLEEP = 1,    /**< 预休眠：熄屏 / 存 NVM / 等收尾 */
    LPM_STATE_SLEEP     = 2,    /**< 已进入 MCU 低功耗态            */
    LPM_STATE_WAKEUP    = 3     /**< 唤醒恢复中                     */
} LpmState_t;

/** 唤醒源 */
typedef enum {
    LPM_WAKE_NONE    = 0,   /**< 无唤醒事件（伪唤醒，应继续睡）       */
    LPM_WAKE_IGN     = 1,   /**< 点火 ACC/ON（PTC7）                  */
    LPM_WAKE_DOOR    = 2,   /**< 车门开（PTD16）                      */
    LPM_WAKE_TURN    = 3,   /**< 转向 / 双闪（PTC2 / PTC16）          */
    LPM_WAKE_CAN     = 4,   /**< CAN 总线活动（PTE4 收发器 RXD）      */
    LPM_WAKE_TIMER   = 5,   /**< 周期唤醒（lpTMR，当前未启用）        */
    LPM_WAKE_UNKNOWN = 6    /**< 唤醒了但认不出来源（按保守处理）     */
} LpmWakeSrc_t;

/*===========================================================================
 *  总开关
 *===========================================================================*/

/** 低功耗总使能。置 0 时 LPM_Task 退化为空函数，行为与改造前完全一致，
 *  便于出问题时一键回退对比。 */
#ifndef CFG_LPM_ENABLE
#define CFG_LPM_ENABLE                  1u
#endif

/*===========================================================================
 *  进入休眠的时间条件
 *===========================================================================*/

/** ACC/IGN OFF 后延时多久才允许休眠（ms）。
 *
 *  这个数是"熄火后仪表还亮多久"的直接体现，需要和整车确认。
 *  太短：熄火瞬间屏就黑，用户会认为仪表坏了；
 *  太长：白白多耗电（Active 态约 20~40mA，30s ≈ 0.3mAh，其实可以接受）。
 *  常见取值 3~30s，这里默认 3s。若整车要求"熄火后里程再显示 30s"，改成 30000。 */
#ifndef CFG_LPM_ACC_OFF_HOLD_MS
#define CFG_LPM_ACC_OFF_HOLD_MS         3000u
#endif

/** 预休眠阶段时长（ms）。
 *
 *  这段时间用来：写 NVM（BL24C08F 页写周期典型 5ms、最大 10ms）、
 *  熄屏、等 SPI/I2C 事务收尾。必须 > EEPROM 写周期，否则里程会丢。
 *  默认 50ms，留足余量。 */
#ifndef CFG_LPM_PRESLEEP_MS
#define CFG_LPM_PRESLEEP_MS             50u
#endif

/** 唤醒后最短保持清醒时间（ms）。
 *  防止"唤醒→立刻判定可以睡→再睡"的乒乓。默认 1s。 */
#ifndef CFG_LPM_MIN_AWAKE_MS
#define CFG_LPM_MIN_AWAKE_MS            1000u
#endif

/*===========================================================================
 *  休眠抑制条件（下面任一条成立就不睡）
 *
 *  ★ 为什么唤醒源只有 IGN，这里却还要看车门 / 转向 / CAN ★
 *
 *  两者管的是不同的时刻：
 *      抑制条件 → 决定"现在能不能睡"（此时 MCU 醒着，能看到这些信号）
 *      唤醒源   → 决定"睡着了谁能叫醒"（此时 MCU 睡着，只有 IGN 有效）
 *
 *  正因为睡着之后车门和双闪叫不醒仪表，**醒着的时候就更不能轻易睡**：
 *  一旦在双闪亮着的时候睡过去，转向图标会直接定格熄灭，而且要等到拧钥匙
 *  才会恢复，用户会认为仪表坏了。所以这里的保持窗口不是可有可无的优化，
 *  在"只有 IGN 能唤醒"的前提下，它是唯一的兜底。
 *
 *  仍然存在的缺口：睡着之后**新出现**的双闪 / 开门叫不醒仪表。
 *  这一条无法用软件解决，需要整车确认可接受，或者打开 CFG_LPM_WAKE_ON_TURN。
 *===========================================================================*/

/** CAN 总线静默判定时长（ms）。最后一帧之后超过这么久才认为总线已睡。
 *  必须大于最慢周期报文（200ms）的若干倍，默认 2s。 */
#ifndef CFG_LPM_CAN_IDLE_MS
#define CFG_LPM_CAN_IDLE_MS             2000u
#endif

/** 车门打开 → 关闭之后继续保持清醒的时长（ms）。默认 30s。 */
#ifndef CFG_LPM_DOOR_HOLD_MS
#define CFG_LPM_DOOR_HOLD_MS            30000u
#endif

/** 转向/双闪信号消失后继续保持清醒的时长（ms）。
 *
 *  双闪（危险报警灯）在 ACC OFF 下是可以工作的，闪烁周期约 0.8s，
 *  硬线会周期性地断开。这个值必须 > 一个闪烁周期，默认 5s。
 *
 *  ★ 在"只有 IGN 能唤醒"的前提下，这个值比通常情况更关键 ★
 *  若它小于闪烁周期，仪表会在双闪熄灭的那 0.4s 里判定"转向已消失"并进睡，
 *  而睡下去之后双闪再亮也叫不醒 —— 结果是双闪还在闪、仪表上的转向图标
 *  却永久定格熄灭，只能拧钥匙恢复。这不是"图标闪烁不同步"的小问题，
 *  是功能直接失效。改小这个值之前务必想清楚。 */
#ifndef CFG_LPM_TURN_HOLD_MS
#define CFG_LPM_TURN_HOLD_MS            5000u
#endif

/*===========================================================================
 *  蓄电池深放电保护
 *===========================================================================*/

/** 深放电保护门限（mV）。0 = 关闭该保护。
 *
 *  电压低于此值时，只有 IGN 能把仪表叫醒并保持；车门 / 转向 / CAN 唤醒
 *  会在 CFG_LPM_LOW_BAT_AWAKE_MS 之后立刻回睡，避免仪表把已经亏电的
 *  电瓶彻底放死。
 *
 *  ⚠ 启用前需整车确认：亏电时开车门仪表只亮 3s 就灭，是可接受的行为。 */
#ifndef CFG_LPM_DEEP_DISCHARGE_MV
#define CFG_LPM_DEEP_DISCHARGE_MV       8000u
#endif

/** 深放电状态下，非 IGN 唤醒允许的清醒时长（ms） */
#ifndef CFG_LPM_LOW_BAT_AWAKE_MS
#define CFG_LPM_LOW_BAT_AWAKE_MS        3000u
#endif

/*===========================================================================
 *  唤醒源
 *
 *  ★ 当前项目只使能 IGN 一路 ★
 *
 *  另外三路的实现是完整的（引脚表、中断使能、ISR 识别、唤醒后差异化处理
 *  都写好了），只是宏默认为 0 不参与编译。将来整车若要求增加，
 *  把对应宏改成 1 就行，lowpower.c 的状态机一行都不用动。
 *
 *  ⚠ 打开任何一路之前，先确认对应端口的 GPIO 中断没有被别的模块占用，
 *    并且该硬线在 ACC OFF 下确实有电（否则永远等不到边沿）。
 *===========================================================================*/

/** IGN 唤醒（PTC7）。当前唯一的唤醒源，不要关。 */
#ifndef CFG_LPM_WAKE_ON_IGN
#define CFG_LPM_WAKE_ON_IGN             1u
#endif

/** 车门唤醒（PTD16）。开启后需额外使能 GPIOD 中断。 */
#ifndef CFG_LPM_WAKE_ON_DOOR
#define CFG_LPM_WAKE_ON_DOOR            1u
#endif

/** 转向 / 双闪唤醒（PTC2 / PTC16）。与 IGN 同在 GPIOC，不需要额外中断号。
 *
 *  ⚠ 若整车要求"休眠后打双闪仪表要亮转向灯"，这一路是最该先打开的：
 *    它和 IGN 共用 GPIOC 中断，增量成本最小。 */
#ifndef CFG_LPM_WAKE_ON_TURN
#define CFG_LPM_WAKE_ON_TURN            1u
#endif

/** CAN 唤醒（PTE4 收发器 RXD 下降沿）。开启后需额外使能 GPIOE 中断，
 *  并且休眠时要把 PTE4 从 ALT5 复用切回 GPIO。 */
#ifndef CFG_LPM_WAKE_ON_CAN
#define CFG_LPM_WAKE_ON_CAN             0u
#endif

/** 被 CAN 唤醒后，多久内没收到**有效**报文就判定为误唤醒并回睡（ms）。
 *  仅在 CFG_LPM_WAKE_ON_CAN = 1 时有意义。 */
#ifndef CFG_LPM_CAN_WAKE_VALID_MS
#define CFG_LPM_CAN_WAKE_VALID_MS       500u
#endif

/*===========================================================================
 *  与山景 SOC 通信的 UART 引脚泊车（PTA13 = TX，PTA12 = RX）
 *
 *  ★ 为什么必须做这一步 ★
 *
 *  UART TX 空闲电平是**高**。休眠时山景 SOC 的 5V 供电（PTB3）已经被切断，
 *  此时 MCU 若继续把 PTA13 驱动到 3.3V，电流会经 SOC 侧的 ESD 保护二极管
 *  倒灌进 SOC 已经掉电的电源轨。后果有三层，从轻到重：
 *
 *    1) 静态电流下不来 —— 倒灌电流直接计进整机电流，几百 uA 起，
 *       而且这部分电流查起来很隐蔽：量 MCU 不高、量各路供电也不高，
 *       就是总电流对不上；
 *    2) SOC 被"寄生供电" —— 倒灌电流可能把 SOC 的电源轨顶到 1~2V，
 *       SOC 处于既没上电又没断干净的中间态，下次上电时序错乱、起不来；
 *    3) 长期倒灌会损伤 SOC 的 ESD 二极管。
 *
 *  RX（PTA12）虽然不往外驱动、不会倒灌，但 SOC 掉电后它的 TX 变成高阻，
 *  MCU 这边的 RX 就**悬空**了。悬空的数字输入会让输入缓冲器停在阈值附近，
 *  两个管子同时半导通产生直通电流，每个脚几十到几百 uA。
 *
 *  ★ 处理方式：两根都切成 GPIO 输出低电平 ★
 *
 *  相比"切成高阻输入"，输出低电平更彻底：
 *    - TX 侧：引脚被钳到 0V，与 SOC 电源轨之间没有电位差，倒灌路径直接消失；
 *      高阻虽然不主动驱动，但引脚电平由外部电路决定，若 SOC 侧有上拉，
 *      节点仍会被顶到某个中间电位，倒灌只是变小不是归零。
 *    - RX 侧：低电平是确定态，彻底消除悬空输入的直通电流；
 *      高阻做不到这一点，它恰恰是"悬空"本身。
 *
 *  ★★★ 顺序：先断电，再拉低 ★★★
 *
 *  这一条和"高阻方案"是**相反**的，改动时最容易搞错：
 *
 *    高阻方案 → 先撒手再断电（撒手在任何时候都安全）
 *    拉低方案 → 必须先断电再拉低
 *
 *  因为拉低是**主动驱动**。若此时 SOC 还带电、并且正在把它的 TX（接 MCU 的
 *  RX）驱动到高电平，两个驱动器就会顶在一起 —— MCU 拉低、SOC 拉高，
 *  形成灌电流通路，轻则电流异常，重则烧 IO。
 *
 *  所以 lp_loads_off() 里泊车动作放在切断 PTB3 **之后**。实际上 pwr_mgr.c
 *  在 IGN OFF 时（比休眠早 3 秒）就已经拉低 PTB3 了，SOC 早就断电，
 *  但代码顺序必须写对，将来有人调整 pwr_mgr 的时序时才不会踩坑。
 *
 *  恢复：唤醒后由 PINS_DRV_Init() 按 g_pin_mux_InitConfigArr0 整体还原成
 *  ALT5，必须在 UART_DRV_Init() 之前 —— 顺序反了 UART 初始化会成功，
 *  但引脚还挂在 GPIO 上，表现为"一个字节都发不出去"。
 *===========================================================================*/

/** 休眠时把 SOC UART TX（PTA13）泊为 GPIO 输出低电平。 */
#ifndef CFG_LPM_PARK_SOC_UART_TX
#define CFG_LPM_PARK_SOC_UART_TX        1u
#endif

/** 休眠时把 SOC UART RX（PTA12）泊为 GPIO 输出低电平。
 *
 *  ⚠ 这一路平时是输入，泊车时改成输出，是本方案里唯一一处"把输入脚变成
 *    输出脚"的地方。前提是对端 SOC 确实已经断电 —— 见上面的顺序说明。
 *    若板级评审认为这个风险不可接受，把本宏置 0，RX 保持原状即可，
 *    代价是可能多几十 uA 的悬空直通电流。 */
#ifndef CFG_LPM_PARK_SOC_UART_RX
#define CFG_LPM_PARK_SOC_UART_RX        1u
#endif

/*===========================================================================
 *  MCU 低功耗模式（走 SDK 的 power_manager 组件）
 *
 *  board/power_config.c 由 YCT 生成，powerConfigsArr[] 的下标与模式一一对应：
 *
 *      下标 0  POWER_MANAGER_RUN         正常运行
 *      下标 1  POWER_MANAGER_SLEEP       只停 CPU，时钟全开 —— 省电有限
 *      下标 2  POWER_MANAGER_DEEPSLEEP   总线时钟关，功能时钟可配
 *      下标 3  POWER_MANAGER_STANDBY     切低压调压器，功耗更低
 *
 *  这四个下标必须与 power_config.c 里 powerConfigsArr[] 的**排列顺序**一致。
 *  以后在 YCT 里增删配置项会改变下标，改完记得回来同步这四个宏 ——
 *  下标错位不会编译报错，只会睡进错误的模式，现象是"电流没降下来"或
 *  "唤不醒"，很难查。
 *
 *  ★ 模式选择 ★
 *    已对照 power_manager_YTM32B1Mx.c 的实际实现（工程定义了 CPU_YTM32B1MC0）：
 *
 *        DEEPSLEEP : SCB->SCR.SLEEPDEEP = 1;  PCU->CSR.PMCLPEN = 0
 *        STANDBY   : SCB->SCR.SLEEPDEEP = 1;  PCU->CSR.PMCLPEN = 1
 *
 *    在本芯片上两者**只差 PMCLPEN 一个位**（是否切低功耗调压器），
 *    唤醒机制、RAM 保持、寄存器保持完全相同。所以从 DEEPSLEEP 换到
 *    STANDBY 只需要改 CFG_LPM_SLEEP_MODE_IDX 一个宏，其余代码零改动，
 *    可以放心在实测阶段来回切。
 *
 *    默认 DEEPSLEEP：先把功能跑通，电流不达标再换 STANDBY。
 *    不使用 POWER_MANAGER_SLEEP：只清 SLEEPDEEP，时钟全开，省不下多少电。
 *===========================================================================*/
#define CFG_LPM_MODE_IDX_RUN            0u
#define CFG_LPM_MODE_IDX_SLEEP          1u
#define CFG_LPM_MODE_IDX_DEEPSLEEP      2u
#define CFG_LPM_MODE_IDX_STANDBY        3u

/** 休眠时进入的模式下标 */
#ifndef CFG_LPM_SLEEP_MODE_IDX
#define CFG_LPM_SLEEP_MODE_IDX          CFG_LPM_MODE_IDX_DEEPSLEEP
#endif

/** 唤醒后切回的模式下标 */
#ifndef CFG_LPM_RUN_MODE_IDX
#define CFG_LPM_RUN_MODE_IDX            CFG_LPM_MODE_IDX_RUN
#endif

/** 是否使用 SDK 的 power_manager。
 *
 *  置 0 时回退到直接写寄存器的实现，见 hal_lowpower.c 的 lp_enter_core()。
 *
 *  ⚠ 回退路径**不包含**驱动替我们做的四件保护：关 CMU、关 SysTick 并清
 *    挂起位、等 EFM 空闲（ERRATA_E220008）、唤醒后等 FXOSC 稳定。
 *    其中"关 CMU"最关键 —— 低功耗下时钟源变化会被时钟监测判成故障并复位。
 *    所以回退路径只适合临时排查，**不要长期使用**。 */
#ifndef CFG_LPM_USE_POWER_MANAGER
#define CFG_LPM_USE_POWER_MANAGER       1u
#endif

/*---------------------------------------------------------------------------
 *  外设时钟门控（与模式选择正交，两条路径都适用）
 *
 *  通过 IPC->CTRL[n].CLKEN 逐个关掉外设时钟门。power_manager 只管内核和
 *  调压器，**不会**帮你把 SPI / I2C / CAN 这些外设的时钟关掉，所以这一步
 *  仍然要自己做。低风险：唤醒时先开时钟门再 Init 驱动，顺序与关断相反。
 *
 *  ★ 特别注意 lpTMR ★
 *    驱动在进低功耗前会关 SysTick 并清它的挂起位，但本工程的 1ms 时基走的
 *    是 **lpTMR0**，驱动管不到。必须由 HAL_Tick_Suspend() 自己停掉 ——
 *    不停的话 lpTMR 中断会在进睡后立刻把 MCU 叫醒，
 *    表现为"怎么也睡不进去、电流一点没降"。
 *-------------------------------------------------------------------------*/
#ifndef CFG_LPM_GATE_PERIPH_CLK
#define CFG_LPM_GATE_PERIPH_CLK         1u
#endif

/*---------------------------------------------------------------------------
 *  以下两个只在 CFG_LPM_USE_POWER_MANAGER = 0 的回退路径下生效。
 *  走 power_manager 时这些动作由 POWER_SYS_SetMode() 内部完成，
 *  再自己写一遍反而可能和驱动打架。
 *-------------------------------------------------------------------------*/

/** 回退路径：PCU->CSR.PMCLPEN = 1，DeepSleep 期间切低功耗调压器 */
#ifndef CFG_LPM_PMC_LOWPOWER
#define CFG_LPM_PMC_LOWPOWER            0u
#endif

/** 回退路径：清 FIRC_DS_EN / FXOSC_DS_EN 让振荡器在 DeepSleep 停振。
 *  ⚠ 系统时钟就跑在这上面，启用前必须按 RM 确认唤醒时硬件会自动起振。 */
#ifndef CFG_LPM_STOP_OSC_IN_DS
#define CFG_LPM_STOP_OSC_IN_DS          0u
#endif

/*===========================================================================
 *  调试
 *===========================================================================*/

/** 置 1 时不真正进 WFI，只走完整的关外设/开外设流程并立即返回。
 *  用来单独验证"关外设序列"有没有把某个模块搞死，排查唤醒问题时很有用。 */
#ifndef CFG_LPM_DRYRUN
#define CFG_LPM_DRYRUN                  0u
#endif

#endif /* CLUSTER_CFG_LOWPOWER_H */