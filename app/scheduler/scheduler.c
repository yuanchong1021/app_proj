/**
 * @file  scheduler.c
 * @brief 合作式任务调度器
 */
#include "scheduler.h"
#include "hw_input.h"
#include "can_app.h"
#include "sys_mgr.h"
#include "pwr_mgr.h"
#include "speed.h"
#include "rpm.h"
#include "coolant.h"
#include "fuel.h"
#include "cng.h"
#include "ambient.h"
#include "odo.h"
#include "tpms_data.h"
#include "led_ind.h"
#include "lcd_ui.h"
#include "backlight.h"
#include "buzzer.h"
#include "radio_input.h"
#include "rtc_input.h"
#include "hal_tick.h"
#include "voltage.h"
#include "lowpower.h"
#include "hal_adc.h"
#include "uds_svc.h"


typedef struct {
    u32   period_ms;
    u32   last_run_ms;
    void (*task_fn)(ClusterSystem_t *);
    const
      char *name;
} TaskEntry_t;

static TaskEntry_t s_tasks[] = {
    {   1u, 0u, HW_SampleTask,     "HW_Sample"   },
    {   1u, 0u, UDS_TpTask,        "UDS_Tp"      },   /* ★新增：ISO-TP 网络层，必须 1ms */
    {   5u, 0u, PWR_Task,          "Power"       },
    {   5u, 0u, CAN_RxTask,        "CAN_Rx"      },
    {  20u, 0u, VOLT_Task,         "Voltage"     },   /* 新增，在超时判定前 */
    {  10u, 0u, SYS_Task,          "SysTask"     },
    {  10u, 0u, CAN_CheckTimeouts, "CAN_Timeout" },
    {  10u, 0u, SPEED_Task,        "Speed"       },
    {  10u, 0u, RPM_Task,          "RPM"         },
    {  20u, 0u, LED_Task,          "LED"         },
    {  10u, 0u, BUZZER_Task,       "Buzzer"      },
    {  20u, 0u, FUEL_Task,         "Fuel"        },
    {  20u, 0u, CNG_Task,          "CNG"         },
    {  20u, 0u, COOLANT_Task,      "Coolant"     },
    {  20u, 0u, TPMS_Task,         "TPMS"        },
    {  50u, 0u, BACKLIGHT_Task,    "Backlight"   },
    {  10u, 0u, RADIO_Task,        "Radio"       },
    {  10u, 0u, UDS_Task,          "UDS_Task"    },   /* ★新增：会话/安全/跳转挂起逻辑 */
    {  30u, 0u, LCD_Task,          "LCD"         },
    { 100u, 0u, CAN_TxTask,        "CAN_Tx"      },
    { 100u, 0u, AMBIENT_Task,      "Ambient"     },
    { 200u, 0u, ODO_Task,          "Odometer"    },
    {1000u, 0u, RTC_Task,          "RTC"         },

    /* ★ LPM_Task 必须是任务表的最后一项 ★
     *
     * 进入休眠时 LPM_Task 会阻塞在 WFI 上。放在最后，意味着停下来的那一刻
     * 本轮所有任务都已跑完：屏幕内容、CAN 发送队列、NVM 写入都处于一致
     * 状态，不会在半截事务上断电。
     *
     * 若把它挪到中间，最直接的后果是 LCD_Task 刚写了一半段码就被停住，
     * 熄火瞬间屏上会残留半帧。改动任务表时请勿移动这一行的位置。 */
    {  10u, 0u, LPM_Task,          "LowPower"    },
};
#define TASK_COUNT (u8)(sizeof(s_tasks)/sizeof(s_tasks[0]))

void Scheduler_Init(void)
{
    for (u8 i = 0u; i < TASK_COUNT; i++) { s_tasks[i].last_run_ms = 0u; }
}

/* 落后超过这么多个周期就放弃追赶，直接对齐到当前时刻，
 * 避免长时间被阻塞后任务连续补跑一大串。 */
#define SCHED_MAX_CATCHUP   3u

void Scheduler_RunOnce(ClusterSystem_t *sys)
{
    u32 now = HAL_GetTickMs();
    SYS_UpdateTick(sys, now);

    for (u8 i = 0u; i < TASK_COUNT; i++) {
        TaskEntry_t *t = &s_tasks[i];

        if ((now - t->last_run_ms) >= t->period_ms) {
            /* 【本次修改】原来是 `t->last_run_ms = now;`，用实际执行时刻做
             * 基准，每次的调度延迟都会被累加进去。20ms 的 LED_Task 实测会
             * 漂到 20~25ms，配合原来按次数累加的延时逻辑，机油压力灯的 2s
             * 延时被拉长到 4~5s。改为按周期递推，消除累积漂移。 */
            if ((now - t->last_run_ms) > (t->period_ms * SCHED_MAX_CATCHUP)) {
                t->last_run_ms = now;               /* 落后太多：放弃追赶 */
            } else {
                t->last_run_ms += t->period_ms;     /* 正常：按周期递推   */
            }
            t->task_fn(sys);
        }
    }
}