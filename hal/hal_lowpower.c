/**
 * @file  hal_lowpower.c
 * @brief 低功耗 HAL 实现（YTM32B1MC0，DeepSleep + GPIO/CAN 唤醒）
 *
 * ===========================================================================
 *  一、本文件用到的芯片资源（全部已对照工程内的头文件确认，非猜测）
 * ===========================================================================
 *    POWER_SYS_Init / POWER_SYS_SetMode   power_manager.h   模式切换（默认路径）
 *    powerConfigsArr[]      board/power_config.c       RUN/SLEEP/DEEPSLEEP/STANDBY
 *    IPC->CTRL[n].CLKEN     YTM32B1MC0.h:1563          外设时钟门控（自己做）
 *    SCB->SCR.SLEEPDEEP     core_cm33.h                回退路径用
 *    PCU->CSR.PMCLPEN       YTM32B1MC0.h:1774          回退路径用
 *    SCU->FIRC_CTRL.FIRC_DS_EN / FXOSC_CTRL.FXOSC_DS_EN            回退路径用
 *    IPC_xxx_INDEX          YTM32B1MC0_features.h:497~ 外设时钟门下标
 *    GPIOC_IRQn / GPIOC_IRQHandler                     端口中断（当前只用 C）
 *    PINS_DRV_SetPinIntSel(GPIO_Type*, pin, cfg)       边沿中断配置
 *
 *  ⚠ PINS_DRV 的两组 API 参数类型不同，写错编译能过但地址完全不同：
 *      中断 / 电平 / 方向  → GPIO_Type*  （GPIOA ~ GPIOE）
 *      上下拉 / 复用       → PCTRL_Type* （PCTRLA ~ PCTRLE）
 *
 * ===========================================================================
 *  二、关外设的顺序为什么不能改
 * ===========================================================================
 *  由外向内：先停会往外灌电流的负载（背光/蜂鸣器/段码屏），再停通信外设
 *  （CAN/UART/SPI/I2C/ADC），最后泊车 GPIO、门控时钟。恢复严格反序。
 *
 *  反过来做的典型后果：
 *   - 先泊车 GPIO 再关 SPI → SC5037 片选被拉成不确定电平，屏上残留半帧；
 *   - 先关 I2C 再写 NVM   → 里程丢失（所以 NVM 写在应用层 PRE_SLEEP 完成）；
 *   - 先门控时钟再 Deinit → Deinit 里访问寄存器会挂死（时钟没了）。
 *
 * ===========================================================================
 *  三、★ 已发现的两处引脚定义错误，本文件刻意绕开 ★
 * ===========================================================================
 *  对照 board/pin_mux.c，工程里有两个**已经写错但目前碰巧没被调用**的宏：
 *
 *    hal_lcd.h : LCD_BACKLIGHT_PORT = GPIOD, PIN = 0   → PTD0 实际是 I2C1_SDA
 *    hal_buzzer.c: BUZZER_PORT      = GPIOD, PIN = 1   → PTD1 实际是 I2C1_SCL
 *
 *  HAL_LCD_SetBacklight() 目前没有任何调用点（背光走 hal_backlight.c 的
 *  MPWM 通道 1 = PTA3），蜂鸣器也走 MPWM 通道 0 = PTA2，所以现在没出问题。
 *  但如果低功耗里"顺手把背光和蜂鸣器引脚拉低"，就会直接把 I2C 总线拉死，
 *  表现为唤醒后 RTC 时间和 EEPROM 全部读不出来 —— 这个坑很隐蔽。
 *
 *  本文件因此**只操作 PTA1 / PTA2 / PTA3 / PTA12 / PTA13 / PTB3 / PTE0**，
 *  绝不碰 PTD0 / PTD1。
 *  建议另行提单修掉那两个宏，避免以后有人真的去调 HAL_LCD_SetBacklight()。
 *
 * ===========================================================================
 *  四、静态电流预算（2mA 指标）
 * ===========================================================================
 *      MCU DeepSleep（时钟门控后）           ~150 uA
 *      CAN 收发器 Standby（PTE10 拉高）      ~30  uA
 *      SC5037（SLEEP_IN + RST 拉低）         ~10  uA
 *      DS1307 / BL24C08F（I2C 空闲）         ~200 uA
 *      12V 分压链 R60+R61+R64 = 153K 常接 BAT ~78 uA（无法关闭）
 *      -------------------------------------------------
 *      合计典型                              < 0.5 mA
 *
 *  好消息：pin_mux.c 里**所有引脚都是 PCTRL_INTERNAL_PULL_NOT_ENABLED**，
 *  没有开内部上拉，所以不存在"开关闭合到地时每路灌 66uA"的那类漏电，
 *  这是低功耗改造里最常见的超标原因，本板天然规避了。
 */

#include "hal_lowpower.h"
#include "hal_tick.h"
#include "hal_backlight.h"
#include "hal_buzzer.h"
#include "hal_adc.h"
#include "hal_can.h"
#include "sdk_project_config.h"
#include "pins_driver.h"
#include "interrupt_manager.h"
#include "flexcan_driver.h"
#include "uart_driver.h"
#include "spi_master_driver.h"
#include "i2c_driver.h"
#include "adc_driver.h"
#include "dma_driver.h"
#include "mpwm_common.h"
#include "sc5037.h"
#include "power_manager.h"
#include "power_config.h"
#include "device_registers.h"   /* YTM32B1MC0.h + _features.h（IPC_xxx_INDEX 在这里） */

/** 本文件是否提供 GPIOx_IRQHandler 的强定义。
 *  board/vector_table_copy.c 里是 __weak 默认实现，这里会覆盖它。 */
#ifndef LPM_PROVIDE_GPIO_ISR
#define LPM_PROVIDE_GPIO_ISR    1
#endif

/*===========================================================================
 *  实例号（与 main.c 的 Board_Init 一一对应）
 *===========================================================================*/
#define LP_SPI_INST             2u      /* SPI_DRV_MasterInit(2, ...)  SC5037   */
#define LP_UART1_INST           1u      /* UART_DRV_Init(1, ...)                */
#define LP_UART2_INST           2u      /* UART_DRV_Init(2, ...)  SOC / 收音机  */
#define LP_I2C_INST             1u      /* I2C_DRV_MasterInit(1, ...) RTC + EE  */
#define LP_MPWM_INST            0u      /* CH0 蜂鸣器(PTA2) + CH1 背光(PTA3)    */
#define LP_ADC_INST             0u

/** MPWM 使能掩码：submodule 0（蜂鸣器）+ 1（背光），与 BACKLIGHT_Init 一致 */
#define LP_MPWM_CH_MASK         0x03u

/*===========================================================================
 *  内部状态
 *===========================================================================*/
static volatile LpmWakeSrc_t s_wake_src = LPM_WAKE_NONE;
static volatile bool         s_armed    = false;  /**< 唤醒中断已武装 */
static bool                  s_prepared = false;  /**< PrepareSleep 已执行 */

/*---------------------------------------------------------------------------
 *  唤醒引脚表
 *
 *  表的顺序即优先级：多脚同时触发时只上报第一个命中的。
 *  IGN 排第一 —— 它决定唤醒后要不要重走指示灯自检。
 *-------------------------------------------------------------------------*/
typedef struct {
    GPIO_Type   *gpio;
    uint32_t     pin;
    LpmWakeSrc_t src;
} LpWakePin_t;

static const LpWakePin_t k_wake_pins[] = {
#if (CFG_LPM_WAKE_ON_IGN != 0)
    { LP_WAKE_IGN_GPIO,   LP_WAKE_IGN_PIN,   LPM_WAKE_IGN  },
#endif
#if (CFG_LPM_WAKE_ON_DOOR != 0)
    { LP_WAKE_DOOR_GPIO,  LP_WAKE_DOOR_PIN,  LPM_WAKE_DOOR },
#endif
#if (CFG_LPM_WAKE_ON_TURN != 0)
    { LP_WAKE_TURNL_GPIO, LP_WAKE_TURNL_PIN, LPM_WAKE_TURN },
    { LP_WAKE_TURNR_GPIO, LP_WAKE_TURNR_PIN, LPM_WAKE_TURN },
#endif
#if (CFG_LPM_WAKE_ON_CAN != 0)
    { LP_CAN_RX_GPIO,     LP_CAN_RX_PIN,     LPM_WAKE_CAN  },
#endif
};

/* 一路唤醒源都没使能的话，MCU 睡下去就再也醒不过来了。
 * 这种配置一定是改宏时手滑，编译期直接拦住，不要等到装车上才发现。 */
#if ((CFG_LPM_WAKE_ON_IGN == 0) && (CFG_LPM_WAKE_ON_DOOR == 0) && \
     (CFG_LPM_WAKE_ON_TURN == 0) && (CFG_LPM_WAKE_ON_CAN == 0))
#error "至少要使能一路唤醒源，否则进入休眠后无法唤醒"
#endif

#define LP_WAKE_PIN_COUNT   (sizeof(k_wake_pins) / sizeof(k_wake_pins[0]))

/*---------------------------------------------------------------------------
 *  需要门控的外设时钟门下标（IPC->CTRL[n]）
 *
 *  power_manager 只管内核和调压器，**不会**关外设时钟，这一步必须自己做。
 *
 *  ★ 绝对不能列进来的三类 ★
 *    - IPC_GPIO_INDEX / IPC_PCTRLx_INDEX：关了就没有 GPIO 边沿中断，唤不醒
 *    - IPC_SIRC_INDEX / IPC_FIRC_INDEX  ：时钟源，由 SCU 的 DS_EN 位管
 *    - IPC_SCU_INDEX / IPC_CIM_INDEX    ：系统基础模块
 *-------------------------------------------------------------------------*/
#if (CFG_LPM_GATE_PERIPH_CLK != 0)
static const uint16_t k_gate_idx[] = {
    IPC_FlexCAN0_INDEX,
    IPC_UART1_INDEX,
    IPC_UART2_INDEX,
    IPC_SPI2_INDEX,
    IPC_I2C1_INDEX,
    IPC_ADC0_INDEX,
    IPC_MPWM0_INDEX,
    IPC_lpTMR0_INDEX,
    IPC_DMA_INDEX,
};
#define LP_GATE_COUNT   (sizeof(k_gate_idx) / sizeof(k_gate_idx[0]))
#endif

/*===========================================================================
 *  内部：唤醒中断的武装 / 解除
 *===========================================================================*/
/*---------------------------------------------------------------------------
 *  需要使能哪几个端口的 GPIO 中断
 *
 *  只开真正用得上的端口。多开一个端口不会导致功能错误，但会把该端口上
 *  其它引脚的边沿也变成唤醒源 —— 那些引脚在休眠期间可能因为外部电路
 *  掉电而产生毛刺，结果就是"隔一会儿自己醒一次"，电流怎么也压不下去。
 *
 *  当前只有 IGN（PTC7）→ 只开 GPIOC。
 *-------------------------------------------------------------------------*/
#if ((CFG_LPM_WAKE_ON_IGN != 0) || (CFG_LPM_WAKE_ON_TURN != 0))
#define LP_NEED_GPIOC_IRQ   1
#else
#define LP_NEED_GPIOC_IRQ   0
#endif

#if (CFG_LPM_WAKE_ON_DOOR != 0)
#define LP_NEED_GPIOD_IRQ   1
#else
#define LP_NEED_GPIOD_IRQ   0
#endif

#if (CFG_LPM_WAKE_ON_CAN != 0)
#define LP_NEED_GPIOE_IRQ   1
#else
#define LP_NEED_GPIOE_IRQ   0
#endif

static void lp_wake_irq_arm(void)
{
    uint8_t i;

    s_wake_src = LPM_WAKE_NONE;

#if (CFG_LPM_WAKE_ON_CAN != 0)
    /* CAN_RX(PTE4) 平时复用给 FlexCAN（ALT5），要产生 GPIO 边沿中断
     * 必须先把它切回 GPIO 功能。唤醒后由 PINS_DRV_Init() 整体复原。 */
    PINS_DRV_SetMuxModeSel(LP_CAN_RX_PCTRL, LP_CAN_RX_PIN, PCTRL_MUX_AS_GPIO);
    PINS_DRV_SetPinDirection(LP_CAN_RX_GPIO, LP_CAN_RX_PIN, GPIO_INPUT_DIRECTION);
#endif

    for (i = 0u; i < LP_WAKE_PIN_COUNT; i++) {
        /* 先清历史标志再使能：否则进睡瞬间会被一个陈旧标志立刻叫醒，
         * 现象是"一进休眠马上唤醒"的死循环。 */
        PINS_DRV_ClearPinIntFlagCmd(k_wake_pins[i].gpio, k_wake_pins[i].pin);
        PINS_DRV_SetPinIntSel(k_wake_pins[i].gpio, k_wake_pins[i].pin,
                              PCTRL_INT_FALLING_EDGE);
    }

#if (LP_NEED_GPIOC_IRQ != 0)
    INT_SYS_ClearPending(GPIOC_IRQn);
    INT_SYS_EnableIRQ(GPIOC_IRQn);
#endif
#if (LP_NEED_GPIOD_IRQ != 0)
    INT_SYS_ClearPending(GPIOD_IRQn);
    INT_SYS_EnableIRQ(GPIOD_IRQn);
#endif
#if (LP_NEED_GPIOE_IRQ != 0)
    INT_SYS_ClearPending(GPIOE_IRQn);
    INT_SYS_EnableIRQ(GPIOE_IRQn);
#endif

    s_armed = true;
}

static void lp_wake_irq_disarm(void)
{
    uint8_t i;

    s_armed = false;

#if (LP_NEED_GPIOC_IRQ != 0)
    INT_SYS_DisableIRQ(GPIOC_IRQn);
#endif
#if (LP_NEED_GPIOD_IRQ != 0)
    INT_SYS_DisableIRQ(GPIOD_IRQn);
#endif
#if (LP_NEED_GPIOE_IRQ != 0)
    INT_SYS_DisableIRQ(GPIOE_IRQn);
#endif

    for (i = 0u; i < LP_WAKE_PIN_COUNT; i++) {
        PINS_DRV_SetPinIntSel(k_wake_pins[i].gpio, k_wake_pins[i].pin,
                              PCTRL_DMA_INT_DISABLED);
        PINS_DRV_ClearPinIntFlagCmd(k_wake_pins[i].gpio, k_wake_pins[i].pin);
    }
}

/*===========================================================================
 *  内部：CAN 收发器 STB（PTE10）
 *===========================================================================*/
static void lp_can_transceiver_standby(bool standby)
{
#if (LP_CAN_STB_ACTIVE_HIGH != 0)
    PINS_DRV_WritePin(LP_CAN_STB_GPIO, LP_CAN_STB_PIN, standby ? 1u : 0u);
#else
    PINS_DRV_WritePin(LP_CAN_STB_GPIO, LP_CAN_STB_PIN, standby ? 0u : 1u);
#endif
}

/*===========================================================================
 *  内部：段码屏进 / 出睡眠
 *
 *  SC5037 自带 SLEEP_IN / SLEEP_OUT 命令（sc5037.h: SC5037_Sleep/Wakeup），
 *  比单纯清屏省得多 —— 清屏只是把段关掉，LCD 偏压电路还在工作。
 *
 *  顺序：清缓冲并刷出 → DISPLAY_OFF → SLEEP_IN → CS 抬高 → RST 拉低保持复位。
 *  必须在关 SPI **之前**做完，之后 SPI 就不能用了。
 *
 *  引脚（sc5037_hal_stm32.c 已确认）：CS=PTC14  RST=PTA7  A0=PTC0
 *-------------------------------------------------------------------------*/
static void lp_lcd_sleep(void)
{
    SC5037_ClearBuffer();
    SC5037_Flush();
    SC5037_DisplayOff();
    SC5037_Sleep();

    SC5037_HAL_CS(false);       /* 片选抬高，释放总线      */
    SC5037_HAL_RST(true);       /* 复位拉低，功耗最低      */

    /* 注意：这里抬高的 CS 会在 lp_periph_off() → lp_lcd_park() 里被重新
     * 拉低到 0V。因为 LCD 与收音机共用 PTE0 那一路供电，pwr_mgr.c 在
     * IGN OFF 时就把它断了，此刻 SC5037 已经没电，CS 挂 3.3V 会经它输入脚
     * 的上钳位二极管倒灌（实测把引脚顶到 4.4V）。两处的先后不要对调。 */
}

/*===========================================================================
 *  内部：与山景 SOC 通信的 UART 引脚泊车（PTA13 = TX，PTA12 = RX）
 *
 *  两根都切成 GPIO 输出低电平：
 *    TX 侧 —— 引脚钳到 0V，与 SOC 已掉电的电源轨之间没有电位差，
 *             倒灌路径直接消失（若维持 UART 空闲高电平，电流会经 SOC 的
 *             ESD 保护二极管倒灌，静态电流下不来，还可能把 SOC 顶到
 *             既没上电也没断干净的中间态，导致下次起不来）；
 *    RX 侧 —— SOC 掉电后它的 TX 变高阻，MCU 这边的 RX 就悬空了，
 *             悬空数字输入会让输入缓冲器停在阈值附近产生直通电流。
 *             给一个确定的低电平就没有这个问题。
 *
 *  ★★★ 调用时机：必须在 SOC 供电（PTB3）已经切断之后 ★★★
 *
 *  拉低是**主动驱动**。若此时 SOC 还带电、并且正把它的 TX（接 MCU 的 RX）
 *  驱动到高电平，两个驱动器就顶在一起 —— MCU 拉低、SOC 拉高，形成灌电流
 *  通路，轻则电流异常，重则烧 IO。
 *
 *  这一点和"泊成高阻输入"的方案**顺序相反**（高阻可以随时撒手，拉低不行），
 *  维护时最容易搞错，所以在这里和 cfg_lowpower.h 里都写明。
 *
 *  恢复不在这里做：唤醒时 PINS_DRV_Init() 会按 g_pin_mux_InitConfigArr0
 *  把 PTA12/PTA13 整体还原成 ALT5，比逐个引脚恢复更不容易漏。
 *===========================================================================*/
static void lp_soc_uart_park(void)
{
#if (CFG_LPM_PARK_SOC_UART_TX != 0)
    /* 三步顺序不能乱：先切复用 → 再设方向 → 最后写电平。
     *
     * 若先写电平再切复用：写的时候引脚还归 UART 管，GPIO 数据寄存器的值
     * 不会输出；等切了复用，引脚立刻按数据寄存器的**残留值**输出，
     * 而那个残留值可能是 1，于是出现"本想拉低，实际瞬间输出高"的毛刺，
     * 恰好打在 SOC 掉电的电源轨上。 */
    PINS_DRV_SetMuxModeSel(LP_SOC_UART_PCTRL, LP_SOC_UART_TX_PIN, PCTRL_MUX_AS_GPIO);
    PINS_DRV_SetPinDirection(LP_SOC_UART_GPIO, LP_SOC_UART_TX_PIN, GPIO_OUTPUT_DIRECTION);
    PINS_DRV_WritePin(LP_SOC_UART_GPIO, LP_SOC_UART_TX_PIN, 0u);
#endif

#if (CFG_LPM_PARK_SOC_UART_RX != 0)
    /* RX 平时是输入，这里改成输出低 —— 全工程唯一一处"把输入脚变成输出脚"。
     * 前提是对端 SOC 确实已断电，见函数头的时机说明。 */
    PINS_DRV_SetMuxModeSel(LP_SOC_UART_PCTRL, LP_SOC_UART_RX_PIN, PCTRL_MUX_AS_GPIO);
    PINS_DRV_SetPinDirection(LP_SOC_UART_GPIO, LP_SOC_UART_RX_PIN, GPIO_OUTPUT_DIRECTION);
    PINS_DRV_WritePin(LP_SOC_UART_GPIO, LP_SOC_UART_RX_PIN, 0u);
#endif
}

/*===========================================================================
 *  内部：段码屏 SPI2 接口泊车
 *
 *  和 lp_soc_uart_park() 是同一类事情，只是对端换成了 SC5037。
 *
 *  ★ 时机：必须排在 SPI_DRV_MasterDeinit() 之后 ★
 *    SPI2 还活着的时候改 PTC15/PTC1 的复用，驱动下一次访问会抢回 ALT3。
 *    所以调用点在 lp_periph_off() 里紧跟 Deinit，而不是在 lp_loads_off()。
 *
 *  ★ 与 UART 那一路的一个区别：这里不存在"顶牛"问题 ★
 *    PTA12 是 RX，对端 SOC 的 TX 是输出，两边都驱动会顶在一起，所以
 *    lp_soc_uart_park() 必须等 PTB3 断电后才能调。
 *    而 SC5037 这五根全是它的**输入**，永远不会反向驱动，什么时候拉低
 *    都不会顶牛。约束只剩"不能出现引脚高、对端 VDD 低"，而 PTE0 早在
 *    pwr_mgr.c 里 IGN OFF 时就断了，所以越早拉低越好。
 *
 *  ★ 三步顺序（切复用 → 设方向 → 写电平）不能乱 ★
 *    理由与 lp_soc_uart_park() 完全相同：先写电平的话引脚还归 SPI 管，
 *    GPIO 数据寄存器写不进输出；等切了复用，引脚立刻按数据寄存器的
 *    **残留值**输出，而残留值可能是 1，于是出现"本想拉低，实际瞬间
 *    输出高"的毛刺，恰好打在 LCD 已经掉电的电源轨上。
 *
 *  CS / A0 / RST 本来就是 GPIO 输出，只需要写电平，不用切复用。
 *  其中 CS 是五根里唯一空闲为高的（pin_mux.c: PTC14 initValue = 1），
 *  也就是实测到 4.4V 的那一根，是这个函数存在的主要原因。
 *
 *  唤醒后不需要专门的 restore：ResumeFromSleep() 里的 PINS_DRV_Init()
 *  会照 g_pin_mux_InitConfigArr0 把 SCK/SI 还原成 ALT3、CS 还原成高、
 *  A0/RST 还原成 0，和 UART 那一路走的是同一条路径。
 *-------------------------------------------------------------------------*/
static void lp_lcd_park(void)
{
#if (CFG_LPM_PARK_LCD_SPI != 0)
    /* SCK / SOUT：从 ALT3 切成 GPIO 输出低 */
    PINS_DRV_SetMuxModeSel(LP_LCD_PCTRL, LP_LCD_SCK_PIN, PCTRL_MUX_AS_GPIO);
    PINS_DRV_SetPinDirection(LP_LCD_GPIO, LP_LCD_SCK_PIN, GPIO_OUTPUT_DIRECTION);
    PINS_DRV_WritePin(LP_LCD_GPIO, LP_LCD_SCK_PIN, 0u);

    PINS_DRV_SetMuxModeSel(LP_LCD_PCTRL, LP_LCD_SI_PIN, PCTRL_MUX_AS_GPIO);
    PINS_DRV_SetPinDirection(LP_LCD_GPIO, LP_LCD_SI_PIN, GPIO_OUTPUT_DIRECTION);
    PINS_DRV_WritePin(LP_LCD_GPIO, LP_LCD_SI_PIN, 0u);

    /* CS / A0：已经是 GPIO 输出，只改电平。
     * CS 就是实测 4.4V 的那一根，lp_lcd_sleep() 末尾刚把它抬成高，
     * 这里覆盖成低 —— 两处顺序不要对调。 */
    PINS_DRV_WritePin(LP_LCD_GPIO, LP_LCD_CS_PIN, 0u);
    PINS_DRV_WritePin(LP_LCD_GPIO, LP_LCD_A0_PIN, 0u);

    /* RST：已经是 GPIO 输出，lp_lcd_sleep() 已拉低，这里再确认一次。
     * 注意在 A 口，用 LP_LCD_RST_GPIO 而不是 LP_LCD_GPIO。 */
    PINS_DRV_WritePin(LP_LCD_RST_GPIO, LP_LCD_RST_PIN, 0u);
#endif
}

/*===========================================================================
 *  内部：负载关断
 *
 *  只碰 PTA1 / PTA2 / PTA3 / PTA12 / PTA13 / PTB3 / PTE0，
 *  绝不碰 PTD0 / PTD1（I2C 总线），原因见文件头第三节。
 *===========================================================================*/
static void lp_loads_off(void)
{
    /* 1) 背光与蜂鸣器：先把占空比清 0，再停计数器。
     *    只停计数器而不清占空比，PWM 输出可能停在高电平上 ——
     *    背光反而常亮，这是低功耗改造里最常见的一个坑。 */
    HAL_BACKLIGHT_Set(0u);
    HAL_Buzzer_Off();
    (void)MPWM_DRV_DisableCounter(LP_MPWM_INST, LP_MPWM_CH_MASK);

    /* 2) PTA2(CH0 蜂鸣器) / PTA3(CH1 背光) 复用回 GPIO 并输出低。
     *    MPWM 时钟被门控之后输出会保持最后状态，改成 GPIO 低电平才是
     *    确定性的"关"。唤醒后由 PINS_DRV_Init() 整体复原为 ALT2。 */
    PINS_DRV_SetMuxModeSel(PCTRLA, 2u, PCTRL_MUX_AS_GPIO);
    PINS_DRV_SetPinDirection(GPIOA, 2u, GPIO_OUTPUT_DIRECTION);
    PINS_DRV_WritePin(GPIOA, 2u, 0u);

    PINS_DRV_SetMuxModeSel(PCTRLA, 3u, PCTRL_MUX_AS_GPIO);
    PINS_DRV_SetPinDirection(GPIOA, 3u, GPIO_OUTPUT_DIRECTION);
    PINS_DRV_WritePin(GPIOA, 3u, 0u);

    /* 3) 三路外设供电。pwr_mgr.c 在 IGN OFF 时已经断过，
     *    这里再显式拉一次，不依赖任务执行顺序。 */
    PINS_DRV_WritePin(GPIOA, 1u, 0u);   /* 功放使能      PTA1 */
    PINS_DRV_WritePin(GPIOB, 3u, 0u);   /* 山景 SOC 5V   PTB3 */
    PINS_DRV_WritePin(GPIOE, 0u, 0u);   /* 收音机 3.3V   PTE0 */

    /* 4) ★ 先断电，再拉低 ★
     *    SOC 的 UART TX/RX 泊成 GPIO 输出低，必须排在上面切断 PTB3 之后。
     *    拉低是主动驱动，SOC 若还带电并驱动它的 TX 为高，两边会顶在一起。
     *    顺序说明见 lp_soc_uart_park() 的函数头注释。 */
    lp_soc_uart_park();

    /* 5) 段码屏进睡眠（必须在关 SPI 之前） */
    lp_lcd_sleep();
}

static void lp_loads_on(void)
{
    (void)MPWM_DRV_EnableCounter(LP_MPWM_INST, LP_MPWM_CH_MASK);
    /* 背光亮度不在这里恢复：交给 BACKLIGHT_Task 按小灯状态重新计算，
     * 写死 100% 会让夜间开车门时突然全亮一下。
     * PTA12/PTA13（SOC UART）也不在这里恢复：由 ResumeFromSleep() 里的
     * PINS_DRV_Init() 统一还原成 ALT5，见那里的顺序说明。 */
}

/*===========================================================================
 *  内部：通信外设关断
 *
 *  一律用 Deinit 而不是"只停时钟"：时钟门控之后寄存器不可访问，
 *  唤醒时重新 Init 反而更干净、更好排查。
 *===========================================================================*/
static void lp_periph_off(void)
{
    /* CAN：先停模块，再切收发器。顺序反了会在总线上发出一个错误帧。 */
    (void)FLEXCAN_DRV_Deinit(CAN_INST);
    lp_can_transceiver_standby(true);

    /* UART2 的 TX/RX 引脚在 lp_loads_off() 里已经被泊成 GPIO 输出低，
     * 此刻若还有一笔 DMA 发送在飞，数据不会出现在引脚上 —— 这没关系，
     * 对端 SOC 早就断电了。
     * UART_DRV_Deinit() 内部会把未完成的传输和 DMA 通道一并收掉。 */
    (void)UART_DRV_Deinit(LP_UART1_INST);
    (void)UART_DRV_Deinit(LP_UART2_INST);

    (void)SPI_DRV_MasterDeinit(LP_SPI_INST);

    /* ★ 先关 SPI，再泊引脚 ★
     * SPI2 还活着的时候去改 PTC15/PTC1 的复用，驱动下一次访问会把它们
     * 抢回 ALT3，泊车等于没做。顺序说明见 lp_lcd_park() 的函数头注释。 */
    lp_lcd_park();

    /* I2C（DS1307 + BL24C08F）。
     * 必须在应用层写完 NVM 之后调用 —— PRE_SLEEP 已经等了
     * CFG_LPM_PRESLEEP_MS，EEPROM 的写周期到这里已经走完。 */
    (void)I2C_DRV_MasterDeinit(LP_I2C_INST);

    /* ADC 连续扫描 + DMA 循环搬运是个持续的电流源，必须停 */
    (void)DMA_DRV_StopChannel(ADC_DMA_CHANNEL);
    ADC_DRV_Disable(LP_ADC_INST);

    /* 系统 1ms 时基。停 lpTMR 会让 system_tick_ms 冻结在进睡那一刻，
     * 这是**有意为之**：唤醒后所有 last_recv_ms 与 tick 仍然自洽，
     * 不会出现"睡了 8 小时，醒来所有 CAN 报文一起判超时"。 */
    HAL_Tick_Suspend();
}

static void lp_periph_on(void)
{
    HAL_Tick_Resume();

    HAL_ADC_Init();     /* 内部含 DMA 配置 + ADC_DRV_Enable + Start */

    (void)I2C_DRV_MasterInit(LP_I2C_INST, &I2C_MasterConfig0, &I2C_MasterConfig0_State);
    (void)SPI_DRV_MasterInit(LP_SPI_INST, &spi_MasterConfig0_State, &spi_MasterConfig0);
    (void)UART_DRV_Init(LP_UART2_INST, &uart_config2_State, &uart_config2);
    (void)UART_DRV_Init(LP_UART1_INST, &uart_config0_State, &uart_config0);

    /* CAN：先唤醒收发器再初始化模块，与关断顺序严格相反。
     * 邮箱配置与接收回调由应用层调 CAN_Init() 重建（见 lowpower.c）。 */
    lp_can_transceiver_standby(false);
    (void)FLEXCAN_DRV_Init(CAN_INST, &flexcanInitConfig0_State, &flexcanInitConfig0);

    (void)MPWM_DRV_EnableCounter(LP_MPWM_INST, LP_MPWM_CH_MASK);
    /* 背光亮度不在这里恢复：交给 BACKLIGHT_Task 按小灯状态重新计算，
     * 写死 100% 会让夜间开车门时突然全亮一下。 */
}

/*===========================================================================
 *  内部：外设时钟门控
 *===========================================================================*/
#if (CFG_LPM_GATE_PERIPH_CLK != 0)
static void lp_clock_gate(bool gate)
{
    uint8_t i;

    for (i = 0u; i < LP_GATE_COUNT; i++) {
        if (gate) {
            IPC->CTRL[k_gate_idx[i]] &= ~IPC_CTRL_CLKEN_MASK;
        } else {
            IPC->CTRL[k_gate_idx[i]] |=  IPC_CTRL_CLKEN_MASK;
        }
    }
}
#else
#define lp_clock_gate(g)    do { (void)(g); } while (0)
#endif

/*===========================================================================
 *  内部：进入内核低功耗态
 *
 *  两条路径二选一，由 CFG_LPM_USE_POWER_MANAGER 控制。
 *
 *  ---------------------------------------------------------------------
 *  路径 A（默认）：SDK 的 power_manager
 *  ---------------------------------------------------------------------
 *  已对照 platform/drivers/src/power/YTM32B1Mx/power_manager_YTM32B1Mx.c
 *  的实际实现确认，本芯片（工程定义了 CPU_YTM32B1MC0）走这个分支：
 *
 *      DEEPSLEEP : SCB->SCR.SLEEPDEEP = 1;  PCU->CSR.PMCLPEN = 0
 *      STANDBY   : SCB->SCR.SLEEPDEEP = 1;  PCU->CSR.PMCLPEN = 1
 *
 *  两者**只差 PMCLPEN 一个位**（是否切低功耗调压器），唤醒机制、RAM 保持、
 *  寄存器保持全都一样。所以从 DEEPSLEEP 换到 STANDBY 只需要改
 *  CFG_LPM_SLEEP_MODE_IDX 一个宏，其余代码零改动。
 *
 *  ★ 驱动已经替我们做掉的四件事，本文件不要重复做 ★
 *    1) 进低功耗前关 CMU（SCU->CMU_CTRL = 0）、唤醒后恢复
 *       —— 低功耗下时钟源变化会被时钟监测判成故障并复位，这是最容易漏的
 *          一步，驱动内部已处理；
 *    2) 关 SysTick 并清 PENDSTCLR，防止刚睡下就被 SysTick 挂起位叫醒；
 *    3) 等 EFM 空闲（ERRATA_E220008），避免 Flash 还在忙时进低功耗；
 *    4) 唤醒后等 FXOSC 稳定（CLOCK_SYS_WaitFXOSCValid）。
 *
 *    注意第 2 条管的是 **SysTick**。本工程的 1ms 时基走的是 **lpTMR0**，
 *    驱动管不到，必须由 lp_periph_off() 里的 HAL_Tick_Suspend() 自己停 ——
 *    不停的话 lpTMR 中断会在进睡后立刻把 MCU 叫醒，表现为"根本睡不进去"。
 *
 *  ---------------------------------------------------------------------
 *  路径 B（回退）：直接写寄存器
 *  ---------------------------------------------------------------------
 *  SCB->SCR.SLEEPDEEP = 1 → DSB → WFI，可选 PMCLPEN。
 *  保留它是为了在 power_manager 出问题时还能把唤醒链路单独跑通，
 *  但要注意这条路**没有**上面那四件事的保护，只适合临时排查，不要长期用。
 *===========================================================================*/
static void lp_enter_core(void)
{
#if (CFG_LPM_DRYRUN != 0)
    /* 干跑模式：不真的睡，只验证关/开外设序列有没有把某个模块搞死 */
    return;
#elif (CFG_LPM_USE_POWER_MANAGER != 0)

    status_t st;

    /* DSB 保证前面所有对外设寄存器的写都已落地再睡。
     * 关外设的写是我们自己发的，驱动不知道，所以这条屏障要在这里加。 */
    __DSB();

    /* SetMode 内部会执行 WFI，被唤醒中断打断后才返回。
     * 返回时驱动已经清了 SLEEPDEEP、恢复了 SysTick / CMU / FXOSC。 */
    st = POWER_SYS_SetMode(CFG_LPM_SLEEP_MODE_IDX, POWER_MANAGER_POLICY_AGREEMENT);

    if (st != STATUS_SUCCESS) {
        /* 切换被回调否决或失败：不睡，直接返回。
         * 上层 LPM_SleepAndWait() 的 do-while 会读到 LPM_WAKE_NONE 并重试，
         * 下一个任务周期再试一次，不会死锁。 */
        return;
    }

    /* 显式切回 RUN。
     * 不是为了硬件 —— 硬件已经在跑了；是为了驱动的内部状态：
     * POWER_SYS_SetMode() 成功后会把 gPowerManagerState.currentConfig 记成
     * 休眠那个下标，不切回来的话 POWER_SYS_GetLastMode() 会一直报
     * DEEPSLEEP，调试时看到的模式和实际对不上。
     * 用 FORCIBLE：这一步不能被任何回调否决，否则状态就永久错位了。 */
    (void)POWER_SYS_SetMode(CFG_LPM_RUN_MODE_IDX, POWER_MANAGER_POLICY_FORCIBLE);

#else
    /* ---- 路径 B：裸寄存器回退（无 CMU / SysTick / Flash 保护，仅供排查）---- */

#if (CFG_LPM_STOP_OSC_IN_DS != 0)
    SCU->FIRC_CTRL  &= ~SCU_FIRC_CTRL_FIRC_DS_EN_MASK;
    SCU->FXOSC_CTRL &= ~SCU_FXOSC_CTRL_FXOSC_DS_EN_MASK;
#endif

#if (CFG_LPM_PMC_LOWPOWER != 0)
    PCU->CSR |= PCU_CSR_PMCLPEN_MASK;       /* 等价于 STANDBY */
#else
    PCU->CSR &= ~PCU_CSR_PMCLPEN_MASK;      /* 等价于 DEEPSLEEP */
#endif

    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    __DSB();
    __WFI();
    __ISB();
    SCB->SCR &= (uint32_t)~SCB_SCR_SLEEPDEEP_Msk;

#if (CFG_LPM_STOP_OSC_IN_DS != 0)
    SCU->FIRC_CTRL  |= SCU_FIRC_CTRL_FIRC_DS_EN_MASK;
    SCU->FXOSC_CTRL |= SCU_FXOSC_CTRL_FXOSC_DS_EN_MASK;
#endif

#endif /* CFG_LPM_DRYRUN / CFG_LPM_USE_POWER_MANAGER */
}

/*===========================================================================
 *  公开接口
 *===========================================================================*/
void HAL_LP_Init(void)
{
    uint8_t i;

    s_wake_src = LPM_WAKE_NONE;
    s_armed    = false;
    s_prepared = false;

#if (CFG_LPM_USE_POWER_MANAGER != 0)
    /* power_manager 初始化。整个工程只能调一次，放在这里而不是 Board_Init，
     * 是为了让"低功耗相关的初始化"集中在一处，便于 CFG_LPM_ENABLE = 0
     * 一键回退时连它一起摘掉。
     *
     * 第 3、4 个参数是回调数组和回调数量：power_config.h 里
     * POWER_MANAGER_CALLBACK_CNT = 0，所以传 NULL / 0。
     * 将来若要在休眠前后做统一处理（例如打印日志），在这里注册回调即可，
     * 不必改 lowpower.c 的状态机。 */
    (void)POWER_SYS_Init(&powerConfigsArr, POWER_MANAGER_CONFIG_CNT, NULL, 0u);
#endif

    /* 上电先确保唤醒中断是关的：上电阶段 IGN 的抖动不该产生唤醒标志 */
#if (LP_NEED_GPIOC_IRQ != 0)
    INT_SYS_DisableIRQ(GPIOC_IRQn);
#endif
#if (LP_NEED_GPIOD_IRQ != 0)
    INT_SYS_DisableIRQ(GPIOD_IRQn);
#endif
#if (LP_NEED_GPIOE_IRQ != 0)
    INT_SYS_DisableIRQ(GPIOE_IRQn);
#endif

    for (i = 0u; i < LP_WAKE_PIN_COUNT; i++) {
        PINS_DRV_SetPinIntSel(k_wake_pins[i].gpio, k_wake_pins[i].pin,
                              PCTRL_DMA_INT_DISABLED);
        PINS_DRV_ClearPinIntFlagCmd(k_wake_pins[i].gpio, k_wake_pins[i].pin);
    }

    /* 收发器保持 Normal，正常工作期间要收发报文 */
    lp_can_transceiver_standby(false);
}

void HAL_LP_PrepareSleep(void)
{
    if (s_prepared) { return; }

    lp_loads_off();         /* 1. 断负载：背光 / 蜂鸣器 / 三路供电 / 段码屏 */
    lp_periph_off();        /* 2. 断通信：CAN / UART / SPI / I2C / ADC / 时基 */
    lp_wake_irq_arm();      /* 3. 武装唤醒中断（必须在门控时钟之前）        */
    lp_clock_gate(true);    /* 4. 门控外设时钟                              */

    s_prepared = true;
}

void HAL_LP_EnterLowPower(void)
{
    lp_enter_core();
}

LpmWakeSrc_t HAL_LP_GetWakeSource(void)
{
    LpmWakeSrc_t src;

    /* 关中断读取，避免"读到一半又来一次唤醒"把结果覆盖掉 */
    INT_SYS_DisableIRQGlobal();
    src        = s_wake_src;
    s_wake_src = LPM_WAKE_NONE;
    INT_SYS_EnableIRQGlobal();

    return src;
}

void HAL_LP_ResumeFromSleep(void)
{
    if (!s_prepared) { return; }

    lp_clock_gate(false);   /* 顺序与 PrepareSleep 严格相反 */
    lp_wake_irq_disarm();

    /* 引脚复用 / 方向 / 初值整体复原：把 PTA2、PTA3 还给 MPWM，
     * 把 PTA12、PTA13 还给 UART2（SOC 链路），同时恢复 PTB3、PTE0 的输出初值。
     * 必须在 lp_periph_on() 之前 —— UART_DRV_Init() 只配外设寄存器，
     * 不管引脚复用；先 Init 后还原的话，UART 初始化完引脚还挂在 GPIO 上，
     * 表现为"串口初始化成功但一个字节都发不出去"。
     * 对 PTA12（RX）还有一层：泊车时它被改成了输出，不还原的话
     * UART 收不到任何数据，而且 MCU 会一直把对端的 TX 线拉低。 */
    (void)PINS_DRV_Init(NUM_OF_CONFIGURED_PINS0, g_pin_mux_InitConfigArr0);

    lp_periph_on();

    /* 段码屏：PINS_DRV_Init 已经把 RST(PTA7) 恢复成初值 0（复位态）。
     * 完整的 硬件复位 + 寄存器重配 由应用层调 Meter_Init() 完成，
     * 这里不做，避免 HAL 反向依赖 app/gauges。 */

    s_prepared = false;
}

/*===========================================================================
 *  GPIO 端口中断
 *
 *  ISR 里只做三件事：认出是哪根脚、记下唤醒源、清标志。
 *  绝不在这里做重配置或调用驱动 Init —— 此时时钟门刚恢复，
 *  访问还没重新初始化的外设会挂死。
 *===========================================================================*/
void HAL_LP_OnGpioIrq(void)
{
    uint8_t  i;
    uint32_t flags;
    bool     hit = false;

    for (i = 0u; i < LP_WAKE_PIN_COUNT; i++) {
        flags = PINS_DRV_GetPortIntFlag(k_wake_pins[i].gpio);

        if ((flags & (1UL << k_wake_pins[i].pin)) != 0UL) {
            PINS_DRV_ClearPinIntFlagCmd(k_wake_pins[i].gpio, k_wake_pins[i].pin);
            hit = true;

            /* 表的顺序即优先级：只记第一个命中的，后面的不覆盖。
             * 这样 IGN 与车门同时触发时上报 IGN，唤醒后会正确重走自检。 */
            if (s_wake_src == LPM_WAKE_NONE) {
                s_wake_src = k_wake_pins[i].src;
            }
        }
    }

    /* 一个都没认出来：可能是同端口别的引脚触发的。
     *
     * 这里必须**把整个端口的标志清掉**，不能只上报了事：
     * 表里没有的引脚，上面的循环不会去清它的标志，而标志不清中断就一直
     * 挂着，退出 ISR 后立刻又进来，直接死在中断里。理论上非唤醒引脚都配了
     * PCTRL_DMA_INT_DISABLED 不会置标志，但这是唯一一条"出错就彻底卡死"
     * 的路径，值得多两行做兜底。
     *
     * 上报 UNKNOWN 而不是 NONE：中断已经清了，WFI 不会再被它触发，
     * 报 NONE 的话上层 do-while 会永远睡下去。 */
    if (s_armed && !hit) {
#if (LP_NEED_GPIOC_IRQ != 0)
        PINS_DRV_ClearPortIntFlagCmd(GPIOC);
#endif
#if (LP_NEED_GPIOD_IRQ != 0)
        PINS_DRV_ClearPortIntFlagCmd(GPIOD);
#endif
#if (LP_NEED_GPIOE_IRQ != 0)
        PINS_DRV_ClearPortIntFlagCmd(GPIOE);
#endif
        if (s_wake_src == LPM_WAKE_NONE) { s_wake_src = LPM_WAKE_UNKNOWN; }
    }
}

#if (LPM_PROVIDE_GPIO_ISR != 0)
/* 覆盖 board/vector_table_copy.c 里的 __weak 默认实现。
 * 向量名取自 board/vector.S:186~188。
 *
 * 只为真正使能了唤醒的端口提供强定义：多定义一个的话，
 * 那个端口上别的引脚触发中断时会跑进这里，而 HAL_LP_OnGpioIrq()
 * 在表里找不到它、又不清它的标志，中断会一直挂着反复进入，直接卡死。
 * 当前只有 IGN（PTC7）→ 只定义 GPIOC_IRQHandler。 */
#if (LP_NEED_GPIOC_IRQ != 0)
void GPIOC_IRQHandler(void) { HAL_LP_OnGpioIrq(); }
#endif
#if (LP_NEED_GPIOD_IRQ != 0)
void GPIOD_IRQHandler(void) { HAL_LP_OnGpioIrq(); }
#endif
#if (LP_NEED_GPIOE_IRQ != 0)
void GPIOE_IRQHandler(void) { HAL_LP_OnGpioIrq(); }
#endif
#endif