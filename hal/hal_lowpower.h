/**
 * @file    hal_lowpower.h
 * @brief   低功耗硬件抽象层（YTM32B1MC0）
 *
 * 本文件把**所有芯片相关**的低功耗操作收在一起，应用层
 * （app/power/lowpower.c）只调用这里的 6 个函数，不碰任何寄存器。
 *
 * 调用时序（由 app/power/lowpower.c 严格按序执行）：
 *
 *      HAL_LP_PrepareSleep()      关负载 → 关外设 → 泊车 GPIO → 配唤醒源 → 门控时钟
 *              ↓
 *      HAL_LP_EnterLowPower()     SLEEPDEEP + WFI（阻塞，直到中断唤醒）
 *              ↓  ← 唤醒中断
 *      HAL_LP_GetWakeSource()     读出是谁唤醒的（读完即清）
 *              ↓
 *      HAL_LP_ResumeFromSleep()   开时钟门 → 撤唤醒中断 → 恢复 GPIO → 重开外设
 *
 * 注：FlexCAN 模块本身由本 HAL 负责 Deinit/Init，但**邮箱与回调**由应用层
 *     在 ResumeFromSleep 之后调用 CAN_Init() 重建（见 lowpower.c），
 *     避免 HAL 反向依赖 service 层。
 */

#ifndef CLUSTER_HAL_LOWPOWER_H
#define CLUSTER_HAL_LOWPOWER_H

#include "types.h"
#include "cfg_lowpower.h"

/* LpmWakeSrc_t / LpmState_t 定义在 cfg_lowpower.h，HAL 与 app 共用。 */

/*===========================================================================
 *  唤醒引脚（已按 board/pin_mux.c 与 hal_gpio.h 的 pin_table 逐条核对）
 *
 *  ★ 当前只有 IGN（PTC7）参与唤醒 ★
 *  其余三组宏保留完整定义，由 cfg_lowpower.h 里的 CFG_LPM_WAKE_ON_xxx
 *  控制是否编译进唤醒表。宏关着的时候这些定义不产生任何代码。
 *
 *  ⚠ 注意：PINS_DRV_SetPinIntSel() / PINS_DRV_GetPortIntFlag() 的第一个参数
 *     是 **GPIO_Type\***（GPIOA~GPIOE），不是 PCTRL_Type*。只有
 *     PINS_DRV_SetPullSel() / SetMuxModeSel() 才用 PCTRLx。写错类型编译能过
 *     （都是指针）但地址完全不同，表现为"配了唤醒却永远唤不醒"。
 *
 *  极性：这几路在 hw_input.c 里全部是 `!HAL_GPIO_ReadPin(...)`，
 *        即低电平有效，所以唤醒边沿一律配下降沿。
 *===========================================================================*/
#define LP_WAKE_IGN_GPIO        GPIOC       /**< PTC7  IGN —— 当前唯一唤醒源 */
#define LP_WAKE_IGN_PIN         7u

/* ---- 以下三组默认不参与编译，见 CFG_LPM_WAKE_ON_DOOR / TURN / CAN ---- */

#define LP_WAKE_DOOR_GPIO       GPIOD       /**< PTD16 门开           */
#define LP_WAKE_DOOR_PIN        16u

#define LP_WAKE_TURNL_GPIO      GPIOC       /**< PTC2  左转灯         */
#define LP_WAKE_TURNL_PIN       2u

#define LP_WAKE_TURNR_GPIO      GPIOC       /**< PTC16 右转灯         */
#define LP_WAKE_TURNR_PIN       16u

/*===========================================================================
 *  CAN 收发器（已确认）
 *
 *    CAN0_RX  = PTE4  （pin_mux.c: PCTRL_MUX_ALT5，CAN0_RX）
 *    CAN0_STB = PTE10 （pin_mux.c: GPIO 输出，initValue = 0）
 *
 *  PTE10 上电初值为 0 且此时收发器必须能正常收发 → **STB 低 = Normal，
 *  STB 高 = Standby**（TJA1042 类极性）。若实测相反，只改
 *  LP_CAN_STB_ACTIVE_HIGH 这一个宏。
 *
 *  ★ 当前不使用 CAN 唤醒 ★
 *  休眠时把 STB 拉高只是为了省掉收发器那 5~10mA，不依赖它的远程唤醒能力，
 *  PTE4 也保持 ALT5 复用不动（少一次复用切换，少一处出错的地方）。
 *  若将来打开 CFG_LPM_WAKE_ON_CAN，PTE4 会在进睡前被切回 GPIO 配下降沿，
 *  唤醒后由 PINS_DRV_Init() 还原。
 *===========================================================================*/
#define LP_CAN_RX_GPIO          GPIOE
#define LP_CAN_RX_PCTRL         PCTRLE
#define LP_CAN_RX_PIN           4u

#define LP_CAN_STB_GPIO         GPIOE
#define LP_CAN_STB_PIN          10u

/** STB 脚极性：1 = 拉高进 Standby（当前板子），0 = 拉低进 Standby */
#ifndef LP_CAN_STB_ACTIVE_HIGH
#define LP_CAN_STB_ACTIVE_HIGH  1
#endif

/*===========================================================================
 *  与山景 SOC 通信的 UART2 引脚（board/pin_mux.c 已确认）
 *
 *    PTA13 = UART2_TX（PCTRL_MUX_ALT5）→ 送到山景 SOC
 *    PTA12 = UART2_RX（PCTRL_MUX_ALT5）← 来自山景 SOC / 收音机
 *
 *  休眠时 SOC 的 5V 供电（PTB3）是断的，MCU 不能继续把 TX 驱动到高电平，
 *  否则电流经 SOC 的 ESD 二极管倒灌；RX 则会因对端高阻而悬空，输入缓冲器
 *  产生直通电流。进睡前把**两根都切成 GPIO 输出低电平**，
 *  唤醒后由 PINS_DRV_Init() 整体还原成 ALT5。详见 cfg_lowpower.h。
 *
 *  ⚠ 泊车必须排在切断 PTB3 之后 —— 拉低是主动驱动，SOC 若还带电会顶牛。
 *
 *  ⚠ 复用切换用 PCTRL_Type*（PCTRLA），方向设置用 GPIO_Type*（GPIOA），
 *    两者参数类型不同，不要混用。
 *===========================================================================*/
#define LP_SOC_UART_PCTRL       PCTRLA
#define LP_SOC_UART_GPIO        GPIOA
#define LP_SOC_UART_TX_PIN      13u
#define LP_SOC_UART_RX_PIN      12u

/*===========================================================================
 *  段码屏 SC5037 的 SPI2 接口泊车
 *
 *  与上面 SOC UART 完全同类的问题，只是对端换成了 SC5037：
 *  LCD 与收音机共用 PTE0 那一路 3.3V（pin_mux.c 第 498 行注释：
 *  "PTE_0-60-GPIO-收音机LCD供电"），而 pwr_mgr.c 在 power_state 离开
 *  PWR_ON 时就把 PTE0 拉低了。也就是说进休眠时 SC5037 早已断电，
 *  MCU 这边任何一根输出高电平都会经它输入脚的上钳位二极管往回灌。
 *
 *  实测：休眠时 /CS（PTC14）由 3.3V 升到 4.4V，拔掉 LCD 即恢复。
 *  /CS 的空闲态恰恰是高（pin_mux.c: PTC14 initValue = 1），
 *  是五根线里唯一需要主动改电平的，绝不能漏。
 *
 *  五根线全部泊成 GPIO 输出 0V，唤醒后由 PINS_DRV_Init() 整体还原
 *  （SCK/SI 回 ALT3，CS 回高，A0/RST 回 0），与 UART 那一路的处理方式一致。
 *
 *  ⚠ 泊车必须排在 SPI_DRV_MasterDeinit() 之后 —— SPI 还活着的时候改
 *    PTC15/PTC1 的复用，驱动下一次访问会把它们抢回 ALT3。
 *
 *  ⚠ /RST 在 A 口，其余四根在 C 口，别把 GPIOA 写成 GPIOC。
 *
 *  ⚠ 复用切换用 PCTRL_Type*（PCTRLA/PCTRLC），方向设置用 GPIO_Type*，
 *    两者参数类型不同，不要混用。
 *===========================================================================*/

/** 休眠时把 SC5037 的 SPI2 接口五根线泊为 GPIO 输出低电平。
 *  与 CFG_LPM_PARK_SOC_UART_TX/RX 同类的开关，置 0 可临时回退对比电流。
 *  若要统一管理，可把这个 #define 挪到 cfg_lowpower.h 里那两个旁边。 */
#ifndef CFG_LPM_PARK_LCD_SPI
#define CFG_LPM_PARK_LCD_SPI    1u
#endif

/* SCK / SOUT / CS / A0 都在 C 口 */
#define LP_LCD_PCTRL            PCTRLC
#define LP_LCD_GPIO             GPIOC
#define LP_LCD_SCK_PIN          15u     /**< SPI2_SCK，pin_mux 里是 ALT3   */
#define LP_LCD_SI_PIN           1u      /**< SPI2_SOUT，pin_mux 里是 ALT3  */
#define LP_LCD_CS_PIN           14u     /**< /CS，GPIO 输出，初值 1（高）  */
#define LP_LCD_A0_PIN           0u      /**< A0，GPIO 输出，初值 0         */

/* /RST 在 A 口，单独一组宏，防止误用成 GPIOC */
#define LP_LCD_RST_PCTRL        PCTRLA
#define LP_LCD_RST_GPIO         GPIOA
#define LP_LCD_RST_PIN          7u      /**< /RST，GPIO 输出，初值 0       */

/*===========================================================================
 *  接口
 *===========================================================================*/

/** 上电调用一次：清唤醒标志、关掉所有唤醒中断、收发器置 Normal */
void HAL_LP_Init(void);

/** 进入休眠前的硬件准备（关负载 → 关外设 → 泊车 GPIO → 配唤醒 → 门控时钟） */
void HAL_LP_PrepareSleep(void);

/** 真正进入 DeepSleep（阻塞在 WFI，被唤醒中断打断后返回） */
void HAL_LP_EnterLowPower(void);

/**
 * @brief 取出并清除唤醒源
 * @return 唤醒源；LPM_WAKE_NONE 表示是伪唤醒，调用者应继续睡
 */
LpmWakeSrc_t HAL_LP_GetWakeSource(void);

/** 唤醒后的硬件恢复（顺序与 PrepareSleep 严格相反） */
void HAL_LP_ResumeFromSleep(void);

/**
 * @brief GPIO 端口中断统一处理入口
 *
 * 本文件末尾已经提供 GPIOC/GPIOD/GPIOE_IRQHandler 的强定义
 * （board/vector_table_copy.c 里是 __weak 的默认实现，会被覆盖）。
 * 若工程别处也要用 GPIO 中断，把 LPM_PROVIDE_GPIO_ISR 置 0，
 * 并在自己的 ISR 里调用本函数。
 */
void HAL_LP_OnGpioIrq(void);

#endif /* CLUSTER_HAL_LOWPOWER_H */