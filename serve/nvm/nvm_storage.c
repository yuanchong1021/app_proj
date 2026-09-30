/**
 * @file  nvm_storage.c
 * @brief NVM 存储服务（里程持久化）
 *
 * 底层已由 hal_nvm 的双扇区乒乓改为 odo_log 的日志式追加。
 *
 * 旧实现每次保存"擦备用扇区→写→擦旧活跃扇区"，且 ODO 与 TRIP 分两次
 * 调用，一次保存擦 4 次扇区，10 万次寿命只够约 833 小时行驶。现在 ODO
 * 与 TRIP 合成一条 16 字节记录追加写入，32 次才擦一次扇区，寿命提升
 * 两个数量级。
 *
 * 本层不做校验也不做掉电保护——那些都在 odo_log 里，且已用 PC 仿真
 * 注入掉电验证过（见 odo_log_sim.c）。这里只负责结构体字段搬运。
 */

#include "nvm_storage.h"
#include "odo_log.h"

bool NVM_Init(void)
{
    return OdoLog_Init();
}

void NVM_LoadOdometer(ClusterSystem_t *sys)
{
    u32 odo = 0u;
    u16 trip = 0u;

    /* 返回 false 表示日志区为空（首次上电）。此时 odo_log 已经把两个
     * 出参置 0，直接采用即可，不需要额外分支。 */
    (void)OdoLog_Read(&odo, &trip);

    sys->lcd.odometer.odo_km  = odo;
    sys->lcd.odometer.trip_dm = trip;
}

void NVM_SaveOdometer(const ClusterSystem_t *sys)
{
    /* 写失败只可能是 Flash 硬件异常。此处不重试也不上报：里程每 60 秒
     * 还会再存一次，熄火时也会再存一次，单次失败可以自愈；而在这里阻塞
     * 重试会拖慢熄火流程，反而增加丢数据的风险。 */
    (void)OdoLog_Write(sys->lcd.odometer.odo_km,
                       sys->lcd.odometer.trip_dm);
}