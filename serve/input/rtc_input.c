/**
 * @file    rtc_input.c
 * @brief   北京时间显示数据输入服务（数据来源：DS1307 硬件 RTC）
 *
 * 实现策略：
 *   - RTC_USE_HARDWARE = 1（默认）：经 HAL_RTC_ReadTime() 读 DS1307，
 *     每 1s 刷新一次 sys->lcd.clock。读失败时 valid=false，显示区熄灭。
 *   - RTC_USE_HARDWARE = 0：软件回退走时（上电从 00:00 起计），
 *     用于 DS1307 未焊接或调试阶段，保证时间区有内容可显示。
 *
 * 北京时间说明：
 *   DS1307 存储 UTC+8（北京时间），由外部校时写入（RTC_SetTime）。
 *   本模块不做时区换算，直接读出即用。
 *
 * 校时接口：
 *   调用 RTC_SetTime(hour, minute) 即可更新 RTC 芯片中的时间（同时更新
 *   软件走时变量，使两条路径都得到刷新）。
 *
 * 与 LCD 的关系：
 *   只写 sys->lcd.clock（hour / minute / valid），
 *   LCD_UpdateClock() 读该字段驱动显示，两者解耦。
 */

#include "rtc_input.h"
#include "hal_rtc_ds1307.h"   /* DS1307 硬件驱动 */

/* ─── 编译选项 ──────────────────────────────────────────────── */
/**
 * RTC_USE_HARDWARE
 *   1 = 使用 DS1307 硬件 RTC（量产固件应置 1）
 *   0 = 软件回退走时（上电从 00:00 开始，调试用）
 */
#define RTC_USE_HARDWARE   1

/** RTC_Task 调度周期（须与 scheduler.c 中的值一致，单位 ms） */
#define RTC_TASK_MS        1000u

/* ─── 模块内部状态（软件走时回退） ─────────────────────────── */
static u8  s_hour   = 0u;
static u8  s_minute = 0u;
static u32 s_acc_ms = 0u;

/* ─── 连续读取失败计数（硬件路径健壮性） ───────────────────── */
/**
 * 连续读失败超过此次数后，将 valid 置 false，触发 LCD 清屏。
 * 避免单次 I2C 抖动就让显示区闪灭。
 */
#define RTC_FAIL_THRESHOLD  3u
static u8 s_fail_cnt = 0u;

/* ═══════════════════════════════════════════════════════════════
 *  内部辅助：从 DS1307 读取当前时间并写入 sys->lcd.clock
 * ═══════════════════════════════════════════════════════════════ */
#if RTC_USE_HARDWARE
static void rtc_update_from_hw(ClusterSystem_t *sys)
{
    u8 h = 0u, m = 0u;

    if (HAL_RTC_ReadTime(&h, &m)) {
        /* 读取成功：刷新显示数据，同步软件走时变量（方便 SetTime 后切回） */
        sys->lcd.clock.hour   = h;
        sys->lcd.clock.minute = m;
        sys->lcd.clock.valid  = true;
        s_hour                = h;
        s_minute              = m;
        s_acc_ms              = 0u;
        s_fail_cnt            = 0u;
    } else {
        /* 读取失败：累计失败计数 */
        if (s_fail_cnt < RTC_FAIL_THRESHOLD) {
            s_fail_cnt++;
        }
        if (s_fail_cnt >= RTC_FAIL_THRESHOLD) {
            /* 连续多次失败 → 标记无效，LCD 熄灭时间区 */
            sys->lcd.clock.valid = false;
        }
        /* 失败期间保持上次成功的时间值不变（避免显示跳变） */
    }
}
#endif /* RTC_USE_HARDWARE */

/* ═══════════════════════════════════════════════════════════════
 *  公开 API
 * ═══════════════════════════════════════════════════════════════ */

void RTC_Init(ClusterSystem_t *sys)
{
    /* 初始化内部状态 */
    s_hour    = 0u;
    s_minute  = 0u;
    s_acc_ms  = 0u;
    s_fail_cnt = 0u;

    /* LCD 数据归零 */
    sys->lcd.clock.hour   = 0u;
    sys->lcd.clock.minute = 0u;
    sys->lcd.clock.valid  = false;

#if RTC_USE_HARDWARE
    /*
     * 初始化 DS1307 硬件：
     *   - 配置 SCL/SDA GPIO
     *   - 检查 CH 位，若振荡器停止则初始化为 00:00:00（2000-01-01）
     *   - 确保 24 小时模式
     *
     * 初始化失败（芯片无响应）：保持 valid=false，
     * 仪表时间区不显示，不影响其他功能。
     */
    if (HAL_RTC_Init()) {
        /* 初始化成功：立即读一次，避免等到 RTC_Task 首次运行才显示 */
        rtc_update_from_hw(sys);
    }
    /* 失败情况：valid 保持 false，在 RTC_Task 中继续重试 */
#endif /* RTC_USE_HARDWARE */
}

void RTC_Task(ClusterSystem_t *sys)
{
#if RTC_USE_HARDWARE

    /*
     * 硬件路径：每次 Task 被调用（1s）都从 DS1307 读一次。
     * DS1307 在 START 时刻将时间锁入内部影子寄存器，
     * 读操作不影响计时，也不需要补偿通信耗时（< 1ms，远小于 1s 显示精度）。
     */
    rtc_update_from_hw(sys);

#else  /* RTC_USE_HARDWARE == 0 */

    /*
     * 软件回退走时：
     * 每次 Task（1000ms）累加，每累计 60s 进一分钟。
     * 精度受调度器抖动影响（±1ms 级），但对仪表时间显示已足够。
     */
    s_acc_ms += RTC_TASK_MS;
    while (s_acc_ms >= 60000u) {
        s_acc_ms -= 60000u;
        if (++s_minute >= 60u) {
            s_minute = 0u;
            if (++s_hour >= 24u) {
                s_hour = 0u;
            }
        }
    }
    sys->lcd.clock.hour   = s_hour;
    sys->lcd.clock.minute = s_minute;
    sys->lcd.clock.valid  = true;   /* 软件走时始终有效 */

#endif /* RTC_USE_HARDWARE */
}

bool RTC_SetTime(u8 hour, u8 minute)
{
    if (hour > 23u || minute > 59u) {
        return false;
    }

    /* 更新软件走时变量（无论硬件路径是否开启，保持一致） */
    s_hour    = hour;
    s_minute  = minute;
    s_acc_ms  = 0u;

#if RTC_USE_HARDWARE
    /* 写入 DS1307 芯片：秒清零（按下校时键时归整到整分） */
    if (!HAL_RTC_WriteTime(hour, minute, 0u)) {
        return false;
    }
    /* 重置失败计数，允许重新开始正常读取 */
    s_fail_cnt = 0u;
#endif /* RTC_USE_HARDWARE */

    return true;
}