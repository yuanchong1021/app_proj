/**
 * @file    cfg_meter_map.h
 * @brief   仪表面板业务符号 → meter_display.h 图标 / 数码管位 映射表
 *
 *  所有 LCD 显示内容（屏幕数字、格柱图标）及 LCD 指示灯均通过
 *  meter_display.h 的 API 统一驱动，不再直接调用 HAL_LCD 或 HAL_LED。
 *
 *  本文件统一定义「业务名称 → meter_icon_t 枚举值 / digit position」的对应关系：
 *    - 图标段（单段控制） → meter_icon_t 枚举常量
 *    - 数码管位（7 段字符）→ digit position 编号（0 ~ 29）
 *
 * ⚠  以下映射编号基于典型面板布局预设，正式量产前必须与实际 CAD 原理图
 *    中的 R 标注及 Digit 编号逐一核对，修改此文件不影响驱动底层。
 *
 * 修改影响范围：app/ui/lcd_ui.c、app/ui/led_ind.c
 */

#ifndef CLUSTER_CFG_METER_MAP_H
#define CLUSTER_CFG_METER_MAP_H

#include "meter_display.h"   /* meter_icon_t 枚举 + ICON_Rxx 常量 */

#ifdef __cplusplus
extern "C" {
#endif

/* ===================================================================== */
/*  一、LCD 屏幕区域 —— 图标段（单段点亮/熄灭）                             */
/* ===================================================================== */

/* ── 燃油 / CNG 格柱（F1 ~ F9，共 9 格） ────────────────────────────── */
/**
 * @brief 燃油格柱图标，n = 1~9，对应 ICON_R16a ~ ICON_R23
 * @note  CAD 核对点：确认 R16a~R23 为 9 格燃油段
 */
#define METER_ICON_FUEL_BAR(n)      ((meter_icon_t)(ICON_R16A + (n) - 1u))

/** 油泵图标（ICON_R2）；与 CNG 图标互斥 */
#define METER_ICON_FUEL_PUMP        ICON_R24

/** CNG 文字图标（ICON_R14）；与油泵图标互斥
 *  注：R14 是 CNG 模式指示图标，R8 是另一个独立的段——CNG 报警灯，
 *  见下方 METER_ICON_CNG_ALARM，两者不是同一个段。 */
#define METER_ICON_CNG              ICON_R14

/** 【本次新增】燃油/CNG 区域的静态图标，三者都是常亮，不参与任何业务
 *  判定，由 lcd_ui.c 的 LCD_UpdateFuel() 每周期直接置 true：
 *    R15 = 燃油/CNG 格柱外边框
 *    R13 = 字母 "E"（Empty，格柱左端）
 *    R25 = 字母 "F"（Full， 格柱右端） */
#define METER_ICON_FUEL_BORDER       ICON_R15
#define METER_ICON_FUEL_LETTER_E     ICON_R13
#define METER_ICON_FUEL_LETTER_F     ICON_R25

/* ── 水温格柱（T1 ~ T9，共 9 格） ────────────────────────────────────── */
/**
 * @brief 水温格柱图标，n = 1~9，对应 ICON_R52 ~ ICON_R45b
 * @note  CAD 核对点：确认 R12~R20 为 9 格水温段
 */
#define METER_ICON_COOLANT_BAR(n)   ((meter_icon_t)(ICON_R45A + (n) - 1u))  //目前暂时没有使用，改用了数组自定义顺序

/* ── 胎压区图标 ──────────────────────────────────────────────────────────
 * 经确认：本面板无独立“轮胎轮廓”图标，每轮仅由 数字 + 小数点 表示。
 * 故不再定义 TIRE_LF/RF/LR/RR（原 R21~R24 与燃油段冲突，已废弃）。 */

/* 胎压小数点图标（每轮一个，用户已确认）*/
#define METER_ICON_TPMS_DP_LF       ICON_R37    /**< 左前胎压小数点（SEG214） */
#define METER_ICON_TPMS_DP_LR       ICON_R38    /**< 左后胎压小数点（SEG229） */
#define METER_ICON_TPMS_DP_RR       ICON_R54    /**< 右后胎压小数点（SEG261） */
#define METER_ICON_TPMS_DP_RF       ICON_R58    /**< 右前胎压小数点（SEG279） */

/* 胎压单位 / 故障图标 */
#define METER_ICON_TPMS_BAR_UNIT    ICON_R40    /**< "bar" 单位图标（用户已确认 R49） */
#define METER_ICON_TPMS_FAULT       ICON_R39    /**< 胎压故障图标（R39）：任一轮异常时闪烁 */

/* ── 里程区域 ─────────────────────────────────────────────────────────── */
/* ODO/TRIP 文字标签：原 R25/R26 与收音机 AM(R26) 冲突，改用里程区独立段
 * R32/R33（图2：R32=SEG133、R33=SEG132）。按常见排布 TRIP 上 / ODO 下。 */
#define METER_ICON_TRIP_LABEL       ICON_R32    /**< "TRIP" 文字标签（SEG133） */
#define METER_ICON_ODO_LABEL        ICON_R33    /**< "ODO"  文字标签（SEG132） */
/** 里程小数点（Digit20 为小数位，对应 ICON_R34，用户已确认）*/
#define METER_ICON_ODO_DP           ICON_R34
/** 里程单位/km/h*/
#define METER_ICOM_ODO_UNIT         ICON_R36

/* ── 车速区域 ─────────────────────────────────────────────────────────── */
/** 车速百位 "1" 图标（车速 ≥ 100km/h 时点亮，对应 ICON_R31，用户已确认）*/
#define METER_ICON_SPEED_HUNDRED    ICON_R31
/** "km/h" 单位图标（常亮，对应 ICON_R35，用户已确认）*/
#define METER_ICON_SPEED_UNIT       ICON_R35

/* ── 转速区域 ─────────────────────────────────────────────────────────── */
/** 转速小数点（Digit6 为小数位，对应 ICON_R10，用户已确认）*/
#define METER_ICON_RPM_DP           ICON_R10
/** "x1000 r/min" 单位图标（常亮，对应 ICON_R11，用户已确认）*/
#define METER_ICON_RPM_UNIT         ICON_R11

/* ── 收音机区域（数据来源串口2） ─────────────────────────────────────── */
#define METER_ICON_RADIO_AM         ICON_R26    /**< "AM" 文字（与 FM 互斥 */
#define METER_ICON_RADIO_FM         ICON_R27    /**< "FM" 文字（与 AM 互斥 */
#define METER_ICON_RADIO_KHZ        ICON_R29    /**< "kHz" 单位（AM 时点亮） */
#define METER_ICON_RADIO_MHZ        ICON_R30    /**< "MHz" 单位（FM 时点亮） */
#define METER_ICON_RADIO_DP         ICON_R28    /**< 频率小数点（FM 87.5 用 */
/** 蓝牙模式图标（R66）：切换到蓝牙音源时点亮，与 AM/FM 互斥 */
#define METER_ICON_RADIO_BT_MODE    ICON_R66
/** 蓝牙已连接图标（R67）：蓝牙设备实际连接后点亮，未连接时熄灭 */
#define METER_ICON_RADIO_BT_CONN    ICON_R67

/* ── 北京时间区域（数据来源 RTC） ────────────────────────────────────── */
/** 时间冒号 ":"（HH:MM 之间，常亮，用户已确认 R1） */
#define METER_ICON_CLOCK_COLON      ICON_R1     /**< 时间冒号 ":" */

/* ── 车外温度区域（本面板未使用，保留定义不再驱动） ──────────────────── */
/** 负号段 ICON_R27 —— 注意 R27 已确认为收音机 FM 图标。
 *  因本面板外温功能不显示（LCD_UpdateAmbientTemp 为空实现），
 *  R27 实际只被 FM 使用，无运行期冲突。此宏仅为历史兼容保留。 */
#define METER_ICON_AMBIENT_SIGN     ICON_R27

/* ===================================================================== */
/*  二、LCD 屏幕区域 —— 数码管位 position（0 ~ 29）                      */
/*                                                                       */
/*  digit position 0 对应面板 Digit 1，position 29 对应 Digit 30。        */
/*  同一区域内：低 position = 高有效位（最左/最高位先显示）。              */
/* ===================================================================== */

/* ── 北京时间数字（HH:MM，Digit1~4 = pos 0~3） ───────────────────────── */
#define METER_DIGIT_CLOCK_H         0u   /**< 时 高位起始 position（pos 0、1 = Digit1、2） */
#define METER_DIGIT_CLOCK_M         2u   /**< 分 高位起始 position（pos 2、3 = Digit3、4） */
#define METER_CLOCK_FIELD_WIDTH     2u   /**< 时 / 分各 2 位 */

/* ── 转速数字（Digit5、6 = pos 4、5；Digit5=个位，Digit6=小数位） ─────── */
/* 显示形如 X.Y（×1000 r/min），例 rpm=3500 → "3.5"。
 * pos4 = 整数个位，pos5 = 小数位，小数点用 R10。 */
#define METER_DIGIT_RPM             4u   /**< 转速起始 position（pos 4、5） */
#define METER_RPM_DIGIT_WIDTH       2u   /**< 共 2 位（1 整数 + 1 小数） */

/* ── 收音机频率数字（Digit7~11 = pos 6~10，共 5 位） ─────────────────── */
#define METER_DIGIT_RADIO           6u   /**< 频率起始 position（pos 6~10） */
#define METER_RADIO_DIGIT_WIDTH     5u   /**< 频率显示 5 位 */

#define METER_RADIO_INT_WIDTH       3u
#define METER_DIGIT_RADIO_FRAC      (METER_DIGIT_RADIO + METER_RADIO_INT_WIDTH)

/* ── 车速数字（Digit12、13 = pos 11、12；Digit12=十位，Digit13=个位） ── */
/* 百位 "1"（≥100km/h）由独立图标 R31 表示，故数码管只需 2 位。 */
#define METER_DIGIT_SPEED           11u  /**< 车速起始 position（pos 11、12） */
#define METER_SPEED_DIGIT_WIDTH     2u   /**< 十位 + 个位 */

/* ── 胎压数字（每轮 2 位，百位在低 position；用户已确认四轮位置） ────── */
/* ── 胎压数字（每轮 = 1 位整数 + 1 位小数，单位 bar；用户已确认） ──────
 * 数据为 kPa，显示换算 bar = kPa/100，保留 1 位小数（如 250kPa → 2.5bar）。
 * 整数位与小数位之间为各自的小数点图标（R37/R38/R54/R58）。 */
#define METER_DIGIT_TPMS_LF_INT     22u   /**< 左前 整数位 pos22 = Digit23 */
#define METER_DIGIT_TPMS_LF_FRAC    23u   /**< 左前 小数位 pos23 = Digit24 */
#define METER_DIGIT_TPMS_LR_INT     24u   /**< 左后 整数位 pos24 = Digit25 */
#define METER_DIGIT_TPMS_LR_FRAC    25u   /**< 左后 小数位 pos25 = Digit26 */
#define METER_DIGIT_TPMS_RR_INT     26u   /**< 右后 整数位 pos26 = Digit27 */
#define METER_DIGIT_TPMS_RR_FRAC    27u   /**< 右后 小数位 pos27 = Digit28 */
#define METER_DIGIT_TPMS_RF_INT     28u   /**< 右前 整数位 pos28 = Digit29 */
#define METER_DIGIT_TPMS_RF_FRAC    29u   /**< 右前 小数位 pos29 = Digit30 */

/* ── 音量数字（Digit21、22 = pos 20、21；Digit21=十位，Digit22=个位） ──
 * 数据来源：UART2 ← SOC（协议 CMD=0x08），取值 0~99。
 * 本区无独立单位图标与小数点，仅 2 位数码管。 */
#define METER_DIGIT_VOLUME          20u  /**< 音量起始 position（pos 20 = Digit21，十位） */
#define METER_VOLUME_DIGIT_WIDTH    2u   /**< 音量占 2 位（十位 + 个位）                  */
#define METER_DIGIT_VOLUME_TENS     (METER_DIGIT_VOLUME + 0u)   /**< 十位 pos20 = Digit21 */
#define METER_DIGIT_VOLUME_ONES     (METER_DIGIT_VOLUME + 1u)   /**< 个位 pos21 = Digit22 */

/* ── 里程数字（Digit14~20 = pos 13~19，共 7 位） ─────────────────────── */
/* 用户已确认：pos13~18（Digit14~19）= 6 位整数，pos19（Digit20）= 小数位，
 * 小数点用 R34。TRIP 模式显示 1 位小数；ODO 模式整 km（小数位补 0 或熄灭）。 */
#define METER_DIGIT_ODO             13u  /**< 里程整数起始 position（pos 13~18，6 位） */
#define METER_ODO_DIGIT_WIDTH       6u   /**< 里程整数位数 */
#define METER_DIGIT_ODO_FRAC        19u  /**< 里程小数位 position（pos 19 = Digit20，1 位） */

/* 注：本面板无独立外温数字区，外温功能不显示（见 lcd_ui.c）。 */

/* ===================================================================== */
/*  三、LCD 指示灯 —— 图标段（对应 R28 ~ R50 及 R45a/b）                 */
/*                                                                       */
/*  每个指示灯对应面板上一个图标段，通过 Meter_SetIcon() 控制亮灭。       */
/*  闪烁由调用层（led_ind.c）根据 sys->blink_1hz_visible 等软件节拍      */
/*  自行计算 on/off，不依赖硬件 PWM。                                    */
/*                                                                       */
/* ⚠  CAD 核对点：确认 R28~R50 与面板实际指示灯位置一一对应。           */
/* ===================================================================== */

#define METER_ICON_TURN_LEFT        ICON_R12    /**< 左转向 */
#define METER_ICON_TURN_RIGHT       ICON_R60    /**< 右转向 */
#define METER_ICON_FOG_FRONT        ICON_R69    /**< 前雾灯 */
#define METER_ICON_LOW_BEAM         ICON_R70    /**< 近光灯 */
#define METER_ICON_HIGH_BEAM        ICON_R71    /**< 远光灯 */
#define METER_ICON_FOG_REAR         ICON_R72    /**< 后雾灯 */
#define METER_ICON_FUEL_LOW         ICON_R2    /**< 燃油低 */
/**
 * CNG 报警灯（R8）。用户确认实际面板：R8 = CNG 报警灯，R14 = CNG 指示灯
 * （文字图标，见上面的 METER_ICON_CNG）。
 *
 * 【本次修正】这个物理段只有一个，但触发它的逻辑条件有两个：
 *   - 低气量报警（对应旧宏 METER_ICON_CNG_WARN）
 *   - CNG 系统故障（对应旧宏 METER_ICON_CNG_FAULT，EMS_CNGSystemFailure）
 * 之前给同一个 ICON_R8 起了两个宏名分别代表这两个条件，结果
 * led_ind.c 里两次独立调用 Meter_SetIcon(同一个段, ...)，后一次会
 * 直接覆盖前一次，等于其中一个条件的状态被丢弃——而且两个宏名都
 * 指向同一个值，本身就是重复定义，容易被误改成两个不同的段。
 * 现在只保留一个宏；两个逻辑条件（cng_warn / cng_fault）在
 * led_ind.c 的 LED_DriveAll() 里先分别算出 on/off，再按位或后
 * 只驱动这一个宏一次。 */
#define METER_ICON_CNG_ALARM        ICON_R8    /**< CNG 报警灯：低气量或系统故障任一成立即点亮/闪烁 */
#define METER_ICON_GPF_YELLOW       ICON_R63    /**< GPF 黄灯（需再生） */
#define METER_ICON_GPF_GREEN        ICON_R74    /**< GPF 绿灯（再生进行中） */
#define METER_ICON_GPF_RED          ICON_R73    /**< GPF 红灯 (再生失败) > */
#define METER_ICON_AIRBAG_FAULT     ICON_R57    /**< 安全气囊故障 */
#define METER_ICON_EPS_FAULT        ICON_R3     /**< 电动助力转向故障 */
#define METER_ICON_TPMS_WARN        ICON_R41    /**< 胎压系统报警 */
#define METER_ICON_ABS_FAULT        ICON_R6     /**< ABS 故障 */
#define METER_ICON_SEATBELT_WARN    ICON_R59    /**< 安全带未系 */
/* 【本次修改】水温相关有两个段，原实现把两者混成了一个：
 *   R53 = 白色水温指示图标 → 常亮，由 lcd_ui.c 的 LCD_UpdateCoolant() 驱动
 *   R55 = 红色水温报警灯   → 报警时点亮，由 led_ind.c 驱动
 * 原来 METER_ICON_WATER_TEMP_HIGH 指向 R53，导致：
 *   1) 报警时点亮的是白色指示图标，红色 R55 全工程无人引用，永远不亮；
 *   2) 白色 R53 反过来跟着报警状态亮灭，平时是灭的。 */
#define METER_ICON_WATER_TEMP_HIGH  ICON_R55    /**< 水温报警灯（红），报警时点亮 */
#define METER_ICON_COOLANT_SYMBOL   ICON_R53    /**< 水温指示图标（白），常亮     */

/** 【本次新增】水温区域的静态图标，三者都是常亮，不参与任何业务判定，
 *  由 lcd_ui.c 的 LCD_UpdateCoolant() 每周期直接置 true：
 *    R43 = 水温格柱外边框
 *    R42 = 字母 "C"（Cold，格柱左端）
 *    R44 = 字母 "H"（Hot， 格柱右端） */
#define METER_ICON_COOLANT_BORDER    ICON_R43
#define METER_ICON_COOLANT_LETTER_C  ICON_R42
#define METER_ICON_COOLANT_LETTER_H  ICON_R44
#define METER_ICON_CHARGER_FAULT    ICON_R64    /**< 充电故障 */
#define METER_ICON_DOOR_OPEN        ICON_R56    /**< 车门未关（R45 拆 a 段） */
#define METER_ICON_POS_LIGHT        ICON_R68    /**< 小灯（R45 拆 b 段） */
#define METER_ICON_ENGINE_FAULT     ICON_R7     /**< 发动机故障（MIL） */
#define METER_ICON_EBD_FAULT        ICON_R9     /**< EBD 故障 */
#define METER_ICON_BRAKE_FAULT      ICON_R65    /**< 制动 / 驻车故障 */
#define METER_ICON_OIL_PRESS_LOW    ICON_R61    /**< 机油压力低 */
#define METER_ICON_GAS_LEAK         ICON_R62    /**< 燃气泄漏 */
/* CNG 燃气系统故障状态由 EMS_CNGSystemFailure（0x305 byte0.bit0）驱动，
 * 见 led_ind.c LED_UpdateCngFault()；点亮的仍是上面唯一的 METER_ICON_CNG_ALARM
 * （R8），不再单独定义一个指向同一段的宏。 */


#ifdef __cplusplus
}
#endif

#endif /* CLUSTER_CFG_METER_MAP_H */