/**
 * @file    lcd_ui.h
 * @brief   LCD 段码屏显示业务逻辑
 *
 * 该模块根据 sys->lcd 中的"显示数据"，通过 meter_display.h 的
 * Meter_SetIcon / Meter_SetDigit / Meter_SetNumberField / Meter_Refresh
 * 把段码写到 SC5037 LCD 控制器：
 *   FuelLcdData_t      → 燃油 9 格 + 油泵 / CNG 图标 + 闪烁
 *   CoolantLcdData_t   → 水温 9 格 + T8/T9 高温闪烁
 *   TpmsLcdData_t      → 4 路胎压数字 + 4 个轮胎图标
 *   OdometerData_t     → 6 位里程数字 + ODO/TRIP 标签 + 小数点
 *   AmbientData_t      → 3 位外温数字 + 负号
 *
 * 闪烁节拍统一来自 sys->blink_1hz_visible（500ms 翻转）。
 *
 * 业务名称 → 面板物理位置的映射见：
 *   app/common/config/cfg_meter_map.h
 */

#ifndef CLUSTER_LCD_UI_H
#define CLUSTER_LCD_UI_H

#include "../../common/cluster_context.h"

/** LCD 显示模块初始化（复位 SC5037 + 清屏） */
void LCD_Init(void);

/** LCD 总刷新任务（建议 50ms 调用） */
void LCD_Task(ClusterSystem_t *sys);

/* --- 各子区域刷新（只写 Meter 缓冲，不触发 SPI，可独立调用） --- */
void LCD_UpdateClock(ClusterSystem_t *sys);       /* 北京时间 Digit1-4（RTC）   */
void LCD_UpdateRpm(ClusterSystem_t *sys);         /* 转速 Digit5-6              */
void LCD_UpdateRadio(ClusterSystem_t *sys);       /* 收音机 Digit7-11（串口2）  */
void LCD_UpdateSpeed(ClusterSystem_t *sys);       /* 车速 Digit12-13            */
void LCD_UpdateFuel(ClusterSystem_t *sys);
void LCD_UpdateCoolant(ClusterSystem_t *sys);
void LCD_UpdateTpms(ClusterSystem_t *sys);
void LCD_UpdateOdometer(ClusterSystem_t *sys);
void LCD_UpdateVolume(ClusterSystem_t *sys);      /* 音量 Digit21-22（串口2）   */
void LCD_UpdateAmbientTemp(ClusterSystem_t *sys); /* 本面板未使用（空实现）     */

#endif /* CLUSTER_LCD_UI_H */