/**
 * @file    cfg_selfcheck.h
 * @brief   上电自检与启动延迟参数配置
 *
 * 修改影响范围：app/system/sys_mgr.c、app/ui/led_ind.c、app/gauges/cng.c
 */

#ifndef CLUSTER_CFG_SELFCHECK_H
#define CLUSTER_CFG_SELFCHECK_H

/*===========================================================================
 *  自检流程参数
 *===========================================================================*/
#define CFG_SELFCHECK_DURATION_MS       3000u   /**< 整体自检持续时间（ms），所有指示灯点亮     */
#define CFG_SELFCHECK_FUEL_DELAY_MS     1000u   /**< 油表自检延迟（ms），自检期间不读真实油位   */
#define CFG_CNG_MODE_DETECT_MS          3000u   /**< CNG 模式上电检测窗口（ms），首次确认燃料模式 */

/*===========================================================================
 *  自检触发参数（IGN OFF → ON 边沿）
 *
 *  这两个数只影响"什么时候重新开始一轮自检"，不参与 PWR_Task 的电源
 *  状态机判定，改动不会影响 SOC / 收音机 / 功放的上下电时序。
 *===========================================================================*/

/** IGN 电平去抖时间（ms）。
 *  hw_input 层对 IGN 是裸读 GPIO（无去抖），钥匙触点抖动会在 1ms 采样
 *  周期上被完整看到。电平必须连续保持这么久才被认可为一次真实变化，
 *  否则一次插钥匙的抖动会把自检反复从头拉起，指示灯一直不灭。 */
#define CFG_SELFCHECK_IGN_DEBOUNCE_MS   20u

/** 判定"新一次点火循环"所需的 IGN OFF 最短保持时间（ms）。
 *
 *  起机（START）拖动时 IGN ON 线可能被瞬时拉低几十~几百 ms。若把这种
 *  跌落也当成一次 OFF→ON，起机成功后会再走一遍 1.5s 自检，表现为
 *  "着车瞬间故障灯全亮一下"。要求 OFF 至少稳定保持这么久，才认为钥匙
 *  真的回到了 OFF/ACC 档。
 *
 *  ⚠ 若整车起机时 IGN 掉电时间超过这个值，请按实测波形调大。 */
#define CFG_SELFCHECK_IGN_OFF_HOLD_MS   500u

#endif /* CLUSTER_CFG_SELFCHECK_H */