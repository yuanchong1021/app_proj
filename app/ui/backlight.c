/**
 * @file    backlight.c
 * @brief   LCD 背光控制实现
 *
 * 背光控制通过 hal_backlight.h 的 HAL_BACKLIGHT_Set() 驱动 GPIO 引脚，
 * 与 LCD 段码显示（meter_display.h）完全独立，互不影响。
 */
 
#include "backlight.h"
#include "hal_backlight.h"
 
void BACKLIGHT_Init(void)
{
    MPWM_DRV_EnableCounter(0, 0x03);
    /* 【修复】原来这里默认全亮（100u）。BACKLIGHT_Task 挂在调度表里，
     * 周期 50ms，且 last_run_ms 从 0 起算——从 Cluster_Init() 里调用
     * 本函数，到调度器第一次真正跑到 BACKLIGHT_Task()，中间还有
     * NVM/RTC 读取、一堆模块 Init、LED_SelfCheck() 等一段不短的时间，
     * 这段时间背光会一直按这里设的默认值点亮，不受电压/点火状态影响。
     * 若这段窗口恰好叠加 IGN 重新上电、电压跌到 9V 以下，就是
     * "屏幕不熄屏"的另一个来源（VOLT_Task 本身的时序漏洞见 voltage.c）。
     * 改成开机默认关闭，具体亮度交给 BACKLIGHT_Task() 第一次真正执行
     * 时按 power_state / voltage.valid / voltage.normal 判定后再点亮，
     * 避免在状态尚未确定前先亮一段。 */
    HAL_BACKLIGHT_Set(0u);
}
 
void BACKLIGHT_Task(ClusterSystem_t *sys)
{
    u8 pct;
 
    if (sys->power_state == PWR_OFF) {
        pct = 0u;
    } else if (!sys->voltage.valid || !sys->voltage.normal) {
        /* 电压尚未测出有效值（ADC 未 ready / 刚上电，s_ready==0）或
         * 已测出但不在 9V~18V：都按"不确认正常"处理，与 LCD 熄屏同步关
         * 背光，避免"亮着的空白屏"。
         *
         * 【修复】原来只判 !sys->voltage.normal，而 voltage.normal 在
         * VOLT_Task() 里遇到 HAL_ADC_IsReady()==false 时会直接 return，
         * 不会被刷新，停留在上电默认值 true。IGN 下电后重新上电、ADC
         * 还没来得及出第一个有效读数（s_ready 还是 0）的这段窗口里，
         * 哪怕实际电压已经跌到 9V 以下，voltage.normal 依旧是 true，
         * 背光就会维持点亮——这正是"IGN 重新上电瞬间电压跌到 9V 以下、
         * 屏幕不熄屏"的根因。can_app.c 判 can_ok 时已经这样加了
         * voltage.valid 前置条件，这里补齐同样的保护。 */
        pct = 0u;
    } else if (sys->hw_input.pos_light) {
        /* 位置灯打开（夜间）→ 降亮，避免刺眼 */
        pct = CFG_BACKLIGHT_DIM_PERCENT;
    } else {
        pct = 50u;
    }
 
    sys->lcd.backlight_pct = pct;   /* 写入上下文供调试/上报 */
    HAL_BACKLIGHT_Set(pct);
}