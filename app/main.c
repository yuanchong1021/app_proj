/* USER CODE BEGIN Header */
/* you can remove the copyright */
/*
 * Copyright 2020-2025 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 * @file main.c
 * @brief 
 * 
 */

/* USER CODE END Header */
#include "sdk_project_config.h"
/* Includes ------------------------------------------------------------------*/

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cluster_context.h"
#include "scheduler.h"
#include "app/system/sys_mgr.h"
#include "app/power/pwr_mgr.h"
#include "app/power/lowpower.h"
#include "app/gauges/speed.h"
#include "app/gauges/rpm.h"
#include "app/gauges/coolant.h"
#include "app/gauges/fuel.h"
#include "app/gauges/cng.h"
#include "app/gauges/ambient.h"
#include "app/gauges/odo.h"
#include "app/gauges/tpms_data.h"
#include "app/ui/led_ind.h"
#include "app/ui/lcd_ui.h"
#include "app/ui/backlight.h"
#include "app/ui/buzzer.h"
#include "app/power/voltage.h"
#include "can_app.h"
#include "hw_input.h"
#include "radio_input.h"
#include "soc_uart.h"
#include "rtc_input.h"
#include "nvm_storage.h"
#include "hal_can.h"
#include "hal_tick.h"
#include "hal_adc.h"
#include "sc5037.h"
#include "uds_svc.h"
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SPI_MASTER_INST 0
#define LPTMR_INST 0

#define MPWM_INST  0
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
/* Define receive buffer */

/* Prepare message to be sent */


/* USER CODE END PV */

/* Private function declare --------------------------------------------------*/
/* USER CODE BEGIN PFDC */
/* USER CODE END PFDC */
static void Board_Init(void);

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/*===========================================================================
 *  全局系统状态（单例）
 *
 *  整个仪表运行时只有一份 ClusterSystem_t 实例，通过指针传递给每个 Task。
 *===========================================================================*/
static ClusterSystem_t g_sys;

/*===========================================================================
 *  系统初始化
 *===========================================================================*/
static void Cluster_Init(void)
{
    /* --- 1) 服务层 / 驱动层基础设施 --- */
    HW_Init();                  /* 硬线输入                  */
    CAN_Init();                 /* CAN 服务                  */
    LCD_Init();                 /* LCD 段码屏                */
    BACKLIGHT_Init();           /* 背光（开机默认 100%）     */

    /* --- 2) 系统上下文清零 --- */
    memset(&g_sys, 0, sizeof(g_sys));

    /* --- 3) 加载 NVM 中的里程数据（先于 ODO_Init，确保 odo_km/trip_dm 已正确）--- */
    /* ★ NVM_Init 必须排在 NVM_LoadOdometer 之前。
     * odo_log 需要先扫描日志区才能给出里程值，未初始化时 Read 会返回 0，
     * 表现为"每次上电里程归零"且不报任何错，极难定位。
     * 它只依赖 HAL_Tick，前面的外设初始化已经就绪。 */
    (void)NVM_Init();
    NVM_LoadOdometer(&g_sys);

    /* --- 4) 各业务模块初始化 ---
     *  顺序约束：
     *    SYS_Init  必须在最早，初始化 blink 节拍 / 自检计时器
     *    PWR_Init  必须在 ODO_Init 前（PWR 关机时会保存 NVM）
     *    LED_Init  最后调用，避免被前面模块覆盖
     */
    SYS_Init(&g_sys);


    PWR_Init(&g_sys);
    VOLT_Init(&g_sys);          /* 新增，须在 HAL_ADC_Init() 之后 */
    UDS_Init(&g_sys);           /* ★新增：UDS 诊断/OTA 刷写，须在 CAN_Init() 之后
                                  * （诊断邮箱已配置）；预编程电压/车速检查依赖
                                  * VOLT_Init 与 lcd.speed_kmh，顺序上放在此处即可，
                                  * 具体检查发生在诊断仪实际发起请求时，而非本函数内。 */
    SPEED_Init(&g_sys);
    RPM_Init(&g_sys);
    FUEL_Init(&g_sys);
    CNG_Init(&g_sys);
    COOLANT_Init(&g_sys);
    TPMS_Init(&g_sys);
    AMBIENT_Init(&g_sys);
    ODO_Init(&g_sys);
    RADIO_Init(&g_sys);     /* 收音机输入（串口2） */
    SOC_UART_Init();        /* SOC UART 转向通知初始化（UART2 TX） */
    SOC_UART_NotifyMcuReady();  /* 可选：通知 SOC 仪表已就绪，请求补发音量等状态 */
    RTC_Init(&g_sys);       /* 北京时间输入（RTC） */
    LED_Init(&g_sys);
    BUZZER_Init(&g_sys);

    /* 低功耗必须**最后**初始化：
     *   - HAL_LP_Init() 会把所有唤醒引脚的 PORT 中断先关掉再清标志，
     *     若放在各外设 Init 之前，后面的 PINS_DRV 配置可能又把中断打开；
     *   - LPM_Init() 会把休眠倒计时的起点设成当前时刻，放在最后可以保证
     *     上电后至少 CFG_LPM_ACC_OFF_HOLD_MS 内不会休眠，
     *     否则"接上蓄电池 → 还没自检完就睡了"。 */
    LPM_Init(&g_sys);

    /* --- 5) 调度器启动准备 --- */
    Scheduler_Init();

    /* --- 6) 上电自检：把所有故障类指示灯点亮 3s --- */
    LED_SelfCheck(&g_sys);

}
/* USER CODE END 0 */


/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    /* USER CODE BEGIN 1 */
    /* USER CODE END 1 */ 
    Board_Init();
    /* USER CODE BEGIN 2 */
    /* 主循环：单线程合作式调度。所有周期任务通过 Scheduler_RunOnce 触发。 */
    HAL_Tick_Init();
    //SC5037_Init();
    HAL_ADC_Init();
    Cluster_Init();

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        /* USER CODE END WHILE */
        /* USER CODE BEGIN 3 */
         Scheduler_RunOnce(&g_sys);
    }
    /* USER CODE END 3 */
}

static void Board_Init(void)
{
    CLOCK_SYS_Init(g_clockManConfigsArr,CLOCK_MANAGER_CONFIG_CNT,g_clockManCallbacksArr,CLOCK_MANAGER_CALLBACK_CNT);
    if(STATUS_SUCCESS != CLOCK_SYS_UpdateConfiguration(CLOCK_MANAGER_ACTIVE_INDEX,CLOCK_MANAGER_POLICY_AGREEMENT))
    {
        /* USER CODE BEGIN ERROR_HANDLER 1 */
        SystemSoftwareReset();
        /* USER CODE END ERROR_HANDLER 1 */
    }
    PINS_DRV_Init(NUM_OF_CONFIGURED_PINS0,g_pin_mux_InitConfigArr0);
    DMA_DRV_Init(&dmaState,&dmaController_InitConfig,dmaChnState,dmaChnConfigArray,NUM_OF_CONFIGURED_DMA_CHANNEL);
    FLASH_ECC_Fault_Init(&flash_ecc_fault_config0,&flash_ecc_fault_config0_State);
    ADC_DRV_ConfigConverter(0,&adc_config0);
    lpTMR_DRV_Init(0,&LPTMR_Config,false);
    FLEXCAN_DRV_Init(CAN_INST,&flexcanInitConfig0_State,&flexcanInitConfig0);
    SPI_DRV_MasterInit(2,&spi_MasterConfig0_State,&spi_MasterConfig0);
    UART_DRV_Init(1,&uart_config0_State,&uart_config0);
    UART_DRV_Init(2,&uart_config2_State,&uart_config2);
    MPWM_DRV_Init(MPWM_INST,&MPWM_State0);
    MPWM_DRV_InitPwm(MPWM_INST,&MPWM_PWM_Config0);
    I2C_DRV_MasterInit(1,&I2C_MasterConfig0,&I2C_MasterConfig0_State);
    POWER_SYS_Init(&powerConfigsArr,POWER_MANAGER_CONFIG_CNT,NULL,POWER_MANAGER_CALLBACK_CNT);
}

/* USER CODE BEGIN 4 */
/* USER CODE END 4 */