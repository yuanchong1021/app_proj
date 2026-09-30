/**
 * @file  odo.c
 * @brief 里程模块（修复s_sub_km_cnt不随Init清零，增加脏标志减少Flash写入）
 */
#include "odo.h"
#include "nvm_storage.h"

#define ODO_TASK_MS         200u

/*
 * 周期落盘间隔。
 *
 * NVM 底层已由 hal_nvm 的双扇区乒乓改为 odo_log 的日志式追加：一条 16
 * 字节记录追加写入，一个扇区放 32 条，写满才擦下一个扇区，8 个扇区轮转。
 * 60 秒周期下的寿命占用（单扇区，按 10 万次擦写计）：
 *
 *   乘用车  2h/天 × 10 年 →  1.7%   余量 58 倍
 *   商用车 12h/天 × 15 年 → 15.4%   余量  6 倍
 *   极端   16h/天 × 20 年 → 27.4%   余量  4 倍
 *
 * 余量充足，无需为寿命牺牲里程精度。异常断电（拔蓄电池、保险丝熔断）
 * 最多丢 60 秒里程；正常熄火不受影响，lowpower.c 与 pwr_mgr.c 会立即落盘。
 *
 * 改这个值前先重算寿命，见《里程日志式存储设计说明》第 4 章。
 */
#define ODO_NVM_INTERVAL_MS 60000u

static u32  s_trip_clear_ms = 0u;
static u32  s_nvm_accum_ms  = 0u;
static u32  s_accum_x10m    = 0u;
static u8   s_sub_km_cnt    = 0u;
static bool s_dirty         = false;

/* 【新增】总里程清零请求标志。
 * 置位者：radio_input.c 收到并确认 SOC 的 CMD=0x09（AA 55 09 5A 53）后
 *         调用 ODO_RequestClearTotal()。
 * 清位者：ODO_Task 执行完清零动作。
 * volatile：写/读虽同在 Task 上下文，但两个模块分处不同编译单元，
 *           加 volatile 可防止编译器把读操作提到循环外。 */
static volatile bool s_odo_clear_req = false;

void ODO_RequestClearTotal(void)
{
    s_odo_clear_req = true;
}

/*
 * 执行总里程清零。
 *
 * 只清 odo_km，不动 trip_dm —— 小计里程有独立的清零通道
 * （CMD=0x03 或方向盘 OK 键长按 3s）。
 *
 * 三件事缺一不可：
 *   1) odo_km 归零
 *   2) s_accum_x10m / s_sub_km_cnt 归零。否则积分器里残留的不足 1km
 *      的里程会在清零后立刻进位，屏上刚显示 0 就跳成 1。
 *   3) 立即落盘。总里程清零是用户的明确操作，如果只等 60s 的周期保存，
 *      用户清完就熄火拔电，下次上电总里程又回来了。
 */
static void ODO_DoClearTotal(ClusterSystem_t *sys)
{
    sys->lcd.odometer.odo_km = 0u;

    s_accum_x10m = 0u;
    s_sub_km_cnt = 0u;

    /* 立即写 NVM。NVM_SaveOdometer 同步阻塞，返回时已落盘。
     * odo_log 是日志式追加，写入过程不修改已有数据，任一步掉电都只会让
     * 新记录校验失败被忽略、退回上一条，不会丢失已保存的里程。 */
    NVM_SaveOdometer(sys);

    s_dirty        = false;   /* 已落盘，清脏标志 */
    s_nvm_accum_ms = 0u;      /* 周期保存计时重新起算，避免紧接着再写一次 */
}

void ODO_Init(ClusterSystem_t *sys)
{
    sys->lcd.odometer.show_trip = false;
    s_trip_clear_ms = 0u; s_nvm_accum_ms = 0u;
    s_accum_x10m = 0u; s_sub_km_cnt = 0u; s_dirty = false;
    s_odo_clear_req = false;
}

static void ODO_Accumulate(ClusterSystem_t *sys)
{
    u16 spd = sys->lcd.speed_kmh;
    if (spd == 0u) { return; }
    s_accum_x10m += (u32)spd * 10u / 18u;
    while (s_accum_x10m >= 1000u) {
        s_accum_x10m -= 1000u;
        if (sys->lcd.odometer.trip_dm < CFG_TRIP_MAX_DM) { sys->lcd.odometer.trip_dm++; }
        if (++s_sub_km_cnt >= 10u) {
            s_sub_km_cnt = 0u;
            if (sys->lcd.odometer.odo_km < CFG_ODO_MAX_KM) { sys->lcd.odometer.odo_km++; }
        }
        s_dirty = true;
    }
}

void ODO_Task(ClusterSystem_t *sys)
{
    /* 【新增】总里程清零请求（SOC CMD=0x09）。
     * 放在 ODO_Accumulate 之前：先把积分器清干净，本周期再正常累加，
     * 这样行驶中清零也不会把清零前的残留里程算进来。 */
    if (s_odo_clear_req) {
        s_odo_clear_req = false;
        ODO_DoClearTotal(sys);
    }

    ODO_Accumulate(sys);
    u16  sw = sys->hw_input.steering_ohms;
    bool ok = (sw >= (CFG_TRIP_CLEAR_RESIST_OHM - 200u) && sw <= (CFG_TRIP_CLEAR_RESIST_OHM + 200u));
    if (ok) {
        s_trip_clear_ms += ODO_TASK_MS;
        if (s_trip_clear_ms >= CFG_TRIP_CLEAR_HOLD_MS) {
            sys->lcd.odometer.trip_dm = 0u; s_trip_clear_ms = 0u; s_dirty = true;
        }
    } else { s_trip_clear_ms = 0u; }
    s_nvm_accum_ms += ODO_TASK_MS;
    if (s_nvm_accum_ms >= ODO_NVM_INTERVAL_MS) {
        s_nvm_accum_ms = 0u;
        if (s_dirty) { NVM_SaveOdometer(sys); s_dirty = false; }
    }
}