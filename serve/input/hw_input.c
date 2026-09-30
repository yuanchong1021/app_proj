/**
 * @file    hw_input.c
 * @brief   硬线输入聚合模块实现
 *
 * @note 引脚号到具体 PORT/PIN 的映射在 driver/gpio_drv.c 内部完成，
 *       本模块仅使用逻辑引脚号。
 */

#include "hw_input.h"
#include "hal_gpio.h"
#include "hal_adc.h"
#include "cluster_context.h"

/*===========================================================================
 *  输入极性配置（问题 4：A8 接高电平仪表熄灭）
 *
 *  A8 = 小灯（位置灯）输入。原实现写死为取反：
 *      hw->pos_light = !HAL_GPIO_ReadPin(PIN_POS_LIGHT);
 *  这意味着"MCU 引脚读到低 = 小灯亮"。是否正确取决于 A8 与 MCU 之间有没有
 *  反相级（分压 + 三极管 / 光耦）。若外部为直通输入，这里就是反的，
 *  A8 拉高反而进不了降亮分支。
 *
 *  ⚠ 请对照原理图确认后二选一，不要改下面的业务代码：
 *      有反相级（A8 高 → MCU 低）→ HW_POS_LIGHT_ACTIVE_LOW = 1（当前）
 *      直通    （A8 高 → MCU 高）→ HW_POS_LIGHT_ACTIVE_LOW = 0
 *
 *  另注：`hw->ign_on` 同样是取反读取，且 PIN_IGN_ON(GPIOC_7) 与
 *  PIN_POS_LIGHT(GPIOC_6) 在 pin_table 中相邻。若这两脚在 pin_table 里
 *  接反，A8 拉高会被当成 IGN OFF，PWR_Task 立刻走 PWR_ON→PWR_OFF：
 *  关背光 + 关 SOC 供电 + 关收音机供电 —— 现象正是"整个仪表熄灭"。
 *  这是"整机熄灭"最优先要排除的一条，请先量 A8 实际进的是哪个 MCU 脚。
 *===========================================================================*/
#ifndef HW_POS_LIGHT_ACTIVE_LOW
#define HW_POS_LIGHT_ACTIVE_LOW     1
#endif

#if (HW_POS_LIGHT_ACTIVE_LOW != 0)
#define HW_READ_POS_LIGHT()     (!HAL_GPIO_ReadPin(PIN_POS_LIGHT))
#else
#define HW_READ_POS_LIGHT()     ( HAL_GPIO_ReadPin(PIN_POS_LIGHT))
#endif

static bool HW_ReadBatOn(const ClusterSystem_t *sys)
{
    static bool s_bat_on = true;
    const PowerVoltage_t *v = &sys->voltage;

    if (!v->valid) {
        return s_bat_on;                /* 电压还没测出来：维持上次结论 */
    }
    if (s_bat_on) {
        if (sys->voltage.state != VOLT_STATE_NORMAL)  { s_bat_on = false; }
    } else {
        if (sys->voltage.state == VOLT_STATE_NORMAL)  { s_bat_on = true;  }
    }
    return s_bat_on;
}

/* === ADC 通道 === */
//#define ADC_CH_FUEL         1u      /**< 燃油电阻         */
//#define ADC_CH_STEERING     1u      /**< 方向盘按键电阻   */
//#define ADC_CH_AMBIENT      2u      /**< 外温             */

/*===========================================================================
 *  公开接口
 *===========================================================================*/
void HW_Init(void)
{
    /* GPIO 方向、上下拉电阻、ADC 通道配置由 driver 层在 HAL_Init 中完成 */
   
}

/**
 * @brief 一次性采样所有硬线输入并写入系统上下文
 *
 * 实现要点：
 *   1) 高电平有效信号：直接读 → 赋给 bool 字段
 *   2) 低电平有效信号：读取后用 "!" 取反，使字段语义统一为 "true=激活/故障"
 *   3) ADC 模拟量：直接保存欧姆值或原始值，业务层再做线性化
 */
void HW_SampleTask(ClusterSystem_t *sys)
{
    HardwireInput_t *hw = &sys->hw_input;

    /*--- 高电平有效 GPIO ---*/
    hw->ign_on     = !HAL_GPIO_ReadPin(PIN_IGN_ON);
    hw->bat_on     = HW_ReadBatOn(sys);
    hw->pos_light  = HW_READ_POS_LIGHT();       /* 极性见文件头 HW_POS_LIGHT_ACTIVE_LOW */
    hw->turn_right = !HAL_GPIO_ReadPin(PIN_TURN_RIGHT);
    hw->fog_rear   = !HAL_GPIO_ReadPin(PIN_FOG_REAR);
    hw->fog_front  = !HAL_GPIO_ReadPin(PIN_FOG_FRONT);
    hw->turn_left  = !HAL_GPIO_ReadPin(PIN_TURN_LEFT);

    /*--- 低电平有效 GPIO（取反：true = 故障 / 激活）---*/
    hw->seatbelt_n     = !HAL_GPIO_ReadPin(PIN_SEATBELT_N);
    hw->door_open_n    = !HAL_GPIO_ReadPin(PIN_DOOR_OPEN_N);
    hw->airbag_fault_n = !HAL_GPIO_ReadPin(PIN_AIRBAG_FAULT_N);
    hw->high_beam_n    = !HAL_GPIO_ReadPin(PIN_HIGH_BEAM_N);
    hw->low_beam_n     = !HAL_GPIO_ReadPin(PIN_LOW_BEAM_N);
    hw->charger_n      = !HAL_GPIO_ReadPin(PIN_CHARGER_N);
    hw->oil_press_n    = !HAL_GPIO_ReadPin(PIN_OIL_PRESS_N);
    hw->eps_fault_n    = !HAL_GPIO_ReadPin(PIN_EPS_FAULT_N);
    hw->brake_fault_n  = !HAL_GPIO_ReadPin(PIN_BRAKE_FAULT_N);
    hw->parking_n      = !HAL_GPIO_ReadPin(PIN_PARKING_N);

    /*--- 模拟量 ADC 采样 ---*/
    hw->fuel_ohms        = HAL_ADC_ReadOhms(ADC_CH_FUEL);
    //hw->steering_ohms    = HAL_ADC_ReadOhms(ADC_CH_STEERING);
    //hw->ambient_temp_raw = HAL_ADC_ReadTempRaw(ADC_CH_AMBIENT);
}
