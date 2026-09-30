/**
 * @file    cluster_context.h
 * @brief   仪表系统总上下文（ClusterSystem_t）及其子结构定义
 *
 * 该头文件聚合了贯穿整个仪表系统的"运行时数据"：
 *   - 电源状态、自检状态、闪烁节拍
 *   - 硬线 GPIO/ADC 输入快照
 *   - CAN 接收数据缓存
 *   - 指示灯状态、LCD 显示数据
 *   - 蜂鸣器控制状态
 *
 * 各业务模块通过指针读写 ClusterSystem_t 上的对应子结构，
 * 子结构按"业务域"分组，是后续把 god struct 拆解为独立模块状态的过渡形态。
 *
 * 设计约束：
 *   - 本文件只描述数据结构，不放任何函数声明；
 *   - 各模块子结构（FuelLcdData_t / BuzzerCtrl_t 等）可以在此查阅完整字段，
 *     但写操作建议通过对应模块的 Setter / Task 接口完成。
 */

#ifndef CLUSTER_CONTEXT_H
#define CLUSTER_CONTEXT_H

#include "types.h"
#include "cfg_power.h"
#include "cfg_can.h"
#include "cfg_gauges.h"
#include "cfg_buzzer.h"
#include "cfg_selfcheck.h"
#include "cfg_lcd_addr.h"
#include "cfg_lowpower.h"

/*===========================================================================
 *  系统粗粒度状态
 *===========================================================================*/

/**
 * @brief 电源状态（点火钥匙位置）
 */
typedef enum {
    PWR_OFF   = 0,  /**< 电源关闭         */
    PWR_ACC   = 1,  /**< ACC 档（附件电）  */
    PWR_ON    = 2,  /**< 点火 ON 档        */
    PWR_START = 3   /**< 起机 START 档     */
} PowerState_t;

/**
 * @brief 仪表运行状态（醒来 / 睡眠）
 */
typedef enum {
    CLUSTER_SLEEP = 0,  /**< 仪表休眠 */
    CLUSTER_WAKE  = 1   /**< 仪表唤醒 */
} ClusterState_t;

/**
 * @brief 当前燃料模式
 */
typedef enum {
    FUEL_MODE_GASOLINE  = 0,    /**< 汽油模式             */
    FUEL_MODE_CNG       = 1,    /**< 天然气（CNG）模式   */
    FUEL_MODE_SWITCHING = 2     /**< 模式切换中           */
} FuelMode_t;

/*===========================================================================
 *  CAN 接收数据子结构
 *
 *  每个 ECU 来源对应一个子结构，统一带 timeout / last_recv_ms 字段，
 *  由 CAN 服务层（service/can/）维护。
 *===========================================================================*/

/** ABS（车速 / ABS+EBD 故障） */
typedef struct {
    u16  vehicle_speed_raw;     /**< 车速原始值（13 位）            */
    u8   abs_fault;             /**< ABS 故障位（1=故障）            */
    u8   ebd_fault;             /**< EBD 故障位（1=故障）            */
    u8   speed_valid;           /**< 车速有效位（0=有效，1=无效）    */
    u8   msg_counter;           /**< 报文计数器                      */
    u8   checksum;              /**< 校验和                          */
    bool started;               /**< ★新增：是否收到过第一帧          */
    bool timeout;               /**< 超时标志                        */
    u32  last_recv_ms;          /**< 最后接收时间戳（ms）            */
} CanABS_t;

/** GPF（颗粒捕集器再生）状态机 */
typedef enum {
    GPF_OFF    = 0,             /**< 不工作              */
    GPF_YELLOW = 1,             /**< 黄灯：需要再生      */
    GPF_GREEN  = 2,             /**< 绿灯：再生进行中    */
    GPF_RED    = 3              /**< 红灯：再生失败/异常 */
} GpfStatus_t;

/** GPF 报文 */
typedef struct {
    GpfStatus_t status;
    bool        started;        /**< ★新增：是否收到过第一帧 */
    bool        timeout;
    u32         last_recv_ms;
} CanGPF_t;

/** 燃气泄漏状态 */
typedef enum {
    GAS_NORMAL = 0,     /**< 正常       */
    GAS_LEAK   = 1,     /**< 泄漏       */
    GAS_ERROR  = 2      /**< 传感器错误 */
} GasLeakStatus_t;

/** CNG 燃气控制器报文 */
typedef struct {
    u8              cng_mode;           /**< 0=汽油 / 1=CNG / 2=切换中 */
    u8              cng_fuel_percent;   /**< CNG 燃料剩余百分比         */
    GasLeakStatus_t leak_status;        /**< 泄漏状态                   */
    bool            system_failure;     /**< ★新增：燃气系统故障（0x305 byte0.bit0，
                                          *   EMS_CNGSystemFailure，0=正常/1=故障）
                                          *   驱动 METER_ICON_CNG_FAULT（R8/SEG51） */
    bool            started;            /**< ★新增：是否收到过第一帧    */
    bool            timeout;
    u32             last_recv_ms;
} CanCNG_t;

/** TPMS 单胎传感器状态 */
typedef enum {
    TPMS_NO_WARN     = 0,   /**< 正常             */
    TPMS_HIGH_PRESS  = 1,   /**< 胎压过高          */
    TPMS_LOW_PRESS   = 2,   /**< 胎压过低          */
    TPMS_QUICK_LEAK  = 3,   /**< 快速漏气          */
    TPMS_LOST_SENSOR = 4,   /**< 传感器丢失        */
    TPMS_BAT_LOW     = 5,   /**< 传感器电池电量低  */
    TPMS_SENSOR_FAIL = 6    /**< 传感器故障        */
} TpmsSensorStatus_t;

/*===========================================================================
 *  CanEMS_t —— 三条 EMS 相关报文各自持有一套独立的超时状态
 *
 *  【本次修改】
 *  1) 原实现中 0x306（水温）与 0x360（MIL）共用同一个 timeout / last_recv_ms，
 *     任一条在线都会刷新时间戳，另一条的丢失永远判不出来；而这个共用标志
 *     又同时驱动"水温格数 + 水温灯 + 发动机故障灯"，一条报文出问题就会连带
 *     点亮一大片，表现为"一个报文超时，其余报文也跟着超时"。
 *
 *  2) 整车确认 **本项目没有 0x306 这条报文**，水温实际在 0x360 的 byte1
 *     （精度 0.75、偏移 -48、有效 0x00~0xFE）。0x306 相关字段已整体删除。
 *
 *  3) 整车矩阵中**没有水温故障位**，coolant_fault 字段已删除。
 *     水温异常统一由 0x360 超时（timeout_360）表达。
 *
 *  现在每条报文一套 started / timeout / last_recv_ms：
 *    0x360 → started_360 / timeout_360 / last_recv_360_ms（水温 + MIL）
 *    0x302 → started_302 / rpm_timeout / rpm_last_recv_ms（转速）
 *
 *  started_xxx：是否收到过第一帧。上电后没收到过的报文**不参与超时判定**，
 *  避免 last_recv_ms 初值 0 导致的"上电即超时"。
 *===========================================================================*/

/** EMS（发动机水温 / 转速 / 故障灯） */
typedef struct {
    /* --- 0x360（100ms）：水温 + MIL --- */
    u8   coolant_temp_raw;      /**< 水温原始值（byte1，0x00~0xFE）  */
    u8   mil_on;                /**< 发动机故障灯（MIL）             */
    bool started_360;           /**< 0x360 是否收到过第一帧          */
    bool timeout_360;           /**< 0x360 超时标志                  */
    u32  last_recv_360_ms;      /**< 0x360 最后接收时间戳（ms）      */

    /* --- 0x302（10ms）：转速 --- */
    u16  engine_rpm_raw;        /**< 转速原始值（0x302，精度0.25）   */
    u8   rpm_fault;             /**< 转速信号故障                    */
    bool started_302;           /**< 0x302 是否收到过第一帧          */
    bool rpm_timeout;           /**< 0x302 超时标志                  */
    u32  rpm_last_recv_ms;      /**< 0x302 最后接收时间戳（ms）      */
} CanEMS_t;

/** TPMS 报文 */
typedef struct {
    u8                 tire_pos;        /**< 胎位标识         */
    u8                 press_lf_raw;    /**< 左前胎压原始值   */
    u8                 press_rf_raw;    /**< 右前胎压原始值   */
    u8                 press_rr_raw;    /**< 右后胎压原始值   */
    u8                 press_lr_raw;    /**< 左后胎压原始值   */
    s8                 temp_raw;        /**< 温度原始值       */
    TpmsSensorStatus_t status_rf;       /**< 右前胎状态       */
    TpmsSensorStatus_t status_lf;       /**< 左前胎状态       */
    TpmsSensorStatus_t status_lr;       /**< 左后胎状态       */
    TpmsSensorStatus_t status_rr;       /**< 右后胎状态       */
    u8                 sys_alarm;       /**< 系统级报警       */
    u8                 temp_alarm;      /**< 温度报警         */
    bool               started;         /**< ★新增：是否收到过第一帧 */
    bool               timeout;
    u32                last_recv_ms;
} CanTPMS_t;

/** CAN 接收数据集合 */
typedef struct {
    CanABS_t  abs;
    CanEMS_t  ems;
    CanGPF_t  gpf;
    CanCNG_t  cng;
    CanTPMS_t tpms;
} CanRxData_t;

/*===========================================================================
 *  硬线输入子结构（GPIO + ADC 采样结果）
 *
 *  约定：
 *    - 高电平有效信号：bool 字段 true 表示信号激活
 *    - 低电平有效信号：字段名后缀 _n，bool true 表示"激活/故障"（已取反）
 *===========================================================================*/
typedef struct {
    /* --- 高电平有效（开关型） --- */
    bool ign_on;            /**< 点火开关 ON         */
    bool bat_on;            /**< 蓄电池常电          */
    bool pos_light;         /**< 小灯（位置灯）      */
    bool turn_left;         /**< 左转向灯            */
    bool turn_right;        /**< 右转向灯            */
    bool fog_front;         /**< 前雾灯              */
    bool high_beam;         /**< 远光灯（保留）      */
    /* --- 低电平有效（已取反，字段含义为"激活/故障"） --- */
    bool low_beam_n;        /**< 近光灯              */
    bool high_beam_n;       /**< 远光灯              */
    bool fog_rear;          /**< 后雾灯              */
    bool airbag_fault_n;    /**< 安全气囊故障        */
    bool eps_fault_n;       /**< 电动助力转向故障    */
    bool seatbelt_n;        /**< 驾驶员安全带未系    */
    bool door_open_n;       /**< 车门未关闭          */
    bool charger_n;         /**< 充电异常            */
    bool oil_press_n;       /**< 机油压力低          */
    bool brake_fault_n;     /**< 制动故障            */
    bool parking_n;         /**< 驻车制动            */
    /* --- ADC 模拟量 --- */
    u16  fuel_ohms;         /**< 燃油传感器电阻（Ω）        */
    u16  steering_ohms;     /**< 方向盘按键电阻（Ω）        */
    s16  ambient_temp_raw;  /**< 外温传感器 ADC 原始值       */
} HardwireInput_t;


/** 电源电压区间状态（阈值见 cfg_power.h） */
typedef enum {
    VOLT_STATE_INIT = 0,    /**< 尚未测出有效电压     */
    VOLT_STATE_UNDER,       /**< < 6.5V   总线不能工作 */
    VOLT_STATE_LOW,         /**< 6.5~9V   CAN 正常但欠压 */
    VOLT_STATE_NORMAL,      /**< 9~18V    正常工作     */
    VOLT_STATE_OVER,        /**< > 18V    总线不能工作 */
} VoltState_t;

/** 12V 电源电压监测数据（来源 ADC0_SE4） */
typedef struct {
    u16         vbat_mv;        /**< 滤波后电池电压（mV）         */
    u16         vbat_raw_mv;    /**< 未滤波瞬时值（mV），标定用   */
    VoltState_t state;          /**< 当前区间状态                 */
    bool        can_ok;         /**< 6.5V ≤ V ≤ 18V：CAN 通信可用 */
    bool        normal;         /**< 9V ≤ V ≤ 18V：仪表正常工作   */
    bool        valid;          /**< ADC 已就绪、电压值可信       */
} PowerVoltage_t;

/*===========================================================================
 *  指示灯状态
 *===========================================================================*/

/**
 * @brief 单个指示灯状态枚举
 *
 * 应用层只填该枚举值，最终由 LED_DriveAll() 翻译为 HAL 调用：
 *   - LED_OFF       : HAL_LED_Set(false) + 取消闪烁
 *   - LED_ON        : HAL_LED_Set(true)  + 取消闪烁
 *   - LED_BLINK_1HZ : HAL_LED_SetBlink(1)
 *   - LED_BLINK_2HZ : HAL_LED_SetBlink(2)
 */
typedef enum {
    LED_OFF       = 0,
    LED_ON        = 1,
    LED_BLINK_1HZ = 2,
    LED_BLINK_2HZ = 3
} LedState_t;

/** 全部指示灯逻辑状态集合 */
typedef struct {
    LedState_t turn_left;
    LedState_t turn_right;
    LedState_t fog_front;
    LedState_t low_beam;
    LedState_t high_beam;
    LedState_t fog_rear;
    LedState_t fuel_low;
    LedState_t cng_warn;
    LedState_t cng_fault;
    LedState_t gpf_warn;
    LedState_t gpf_regen;
    LedState_t gpf_red;
    LedState_t airbag_fault;
    LedState_t eps_fault;
    LedState_t tpms_warn;
    LedState_t abs_fault;
    LedState_t seatbelt_warn;
    LedState_t water_temp_high;
    LedState_t charger_fault;
    LedState_t door_open;
    LedState_t pos_light;
    LedState_t engine_fault;
    LedState_t ebd_fault;
    LedState_t brake_fault;
    LedState_t oil_press_low;
    LedState_t gas_leak;
} LedStatus_t;

/*===========================================================================
 *  LCD 显示数据子结构
 *
 *  各业务模块（fuel/cng/coolant/tpms/odo/ambient）将处理后的"显示数据"
 *  写入这里，由 lcd_ui.c 统一刷段码。
 *===========================================================================*/

/**
 * @brief 燃油 / CNG 格数显示数据
 */
typedef struct {
    u8         segments;        /**< 当前格数 0~9                              */
    bool       blink;           /**< F1/F2 低格报警闪烁                         */
    bool       fault_blink;     /**< 开路/短路故障：9 格整体闪烁                 */
    bool       cng_switching;   /**< 燃料切换中：CNG 图标闪烁                    */
    FuelMode_t mode;            /**< 当前燃料模式（影响油泵 / CNG 图标互斥）     */
} FuelLcdData_t;

/**
 * @brief 水温格数显示数据
 *
 * @note blink_high 在 T8 / T9 报警时统一为 true，触发对应格 1Hz 闪烁。
 *       如需独立控制 T8 / T9，可扩展为 blink_seg8 / blink_seg9。
 */
typedef struct {
    u8   segments;      /**< 当前格数 1~9        */
    bool blink_high;    /**< 高温闪烁标志        */
    bool high_temp_warn;/**< ★新增：高温报警锁存态（带迟滞）
                         *   进入：temp ≥ CFG_COOLANT_TEMP_WARN_C(113℃)
                         *   解除：temp ≤ CFG_COOLANT_TEMP_WARN_CLEAR_C(110℃)
                         *   中间区间保持上一状态。红色报警灯 R55 与段柱闪烁
                         *   统一取此标志，不再按格数(≥8)判断。 */
    bool signal_lost;   /**< ★新增：水温信号长期丢失（已过 CFG_COOLANT_TIMEOUT_S）
                         *   段柱强制 1 格与水温报警灯 R55 共用此标志，
                         *   避免"灯立刻亮、格柱 20s 后才掉"的时序不一致。 */
} CoolantLcdData_t;

/**
 * @brief 胎压显示数据
 *
 * 每个轮胎独立维护：
 *   - press_*_kpa : 当前胎压（kPa）
 *   - show_*      : 是否显示数值（false 显示 "---"）
 *   - blink_*     : 是否闪烁（异常状态）
 */
typedef struct {
    u16  press_lf_kpa;
    u16  press_rf_kpa;
    u16  press_lr_kpa;
    u16  press_rr_kpa;
    bool show_lf;
    bool show_rf;
    bool show_lr;
    bool show_rr;
    bool blink_lf;
    bool blink_rf;
    bool blink_lr;
    bool blink_rr;
} TpmsLcdData_t;

/**
 * @brief 里程显示数据
 *
 * @note 单位约定：
 *   odo_km   : 总计里程，单位 km（精度 1km）
 *   trip_dm  : 小计里程，单位 0.1km（精度 100m，d 表示 deci-）
 */
typedef struct {
    u32  odo_km;        /**< 总计里程（km）                          */
    u16  trip_dm;       /**< 小计里程（0.1km 精度，最大 9999=999.9km）*/
    bool show_trip;     /**< true=显示小计  false=显示总计           */
    bool odo_or_trip;    /**< true = odo, flase = trip */
} OdometerData_t;

/**
 * @brief 车外温度显示数据
 */
typedef struct {
    s16  temp_c;        /**< 温度值（℃）-40 ~ +90      */
    bool valid;         /**< 传感器数据有效标志         */
} AmbientData_t;

/**
 * @brief 收音机频段枚举
 */
typedef enum {
    RADIO_BAND_NONE = 0,   /**< 关闭 / 无数据：频率区熄灭 */
    RADIO_BAND_AM,         /**< AM 频段（单位 kHz）       */
    RADIO_BAND_FM,         /**< FM 频段（单位 MHz）       */
    RADIO_BAND_BT,         /**< 蓝牙模式（无频率，显示 BT 图标） */
} RadioBand_t;



/**
 * @brief 收音机显示数据（数据来源：串口 2）
 *
 * 显示约定：
 *   - 频率以整数形式给出：
 *       FM：freq_raw 单位 0.1MHz（例 875 → 87.5MHz，小数点 R28 点亮）
 *       AM：freq_raw 单位 1kHz （例 1080 → 1080kHz，无小数点）
 *   - band 决定 AM/FM(R26/R27) 与 kHz/MHz(R29/R30) 图标互斥点亮
 *   - valid=false 时整块频率区熄灭
 */
typedef struct {
    RadioBand_t band;       /**< 当前频段 AM/FM/NONE                 */
    u16         freq_raw;   /**< 频率原始值（FM:0.1MHz, AM:1kHz）    */
    bool        valid;      /**< 数据有效（串口在线且解析成功）     */
    bool        bt_connected;   /**< 蓝牙已连接标志（R67）
                                  *   true=已连接/R67亮，false=未连接/R67灭
                                  *   仅 band=RADIO_BAND_BT 时有意义       */
} RadioLcdData_t;

/**
 * @brief 音量显示数据（数据来源：串口 2 ← SOC，协议 CMD=0x08）
 *
 * 显示位置：Digit21 = 十位，Digit22 = 个位（pos 20 / pos 21）
 *
 * 显示约定：
 *   - level 有效范围 0~99，超范围由接收层钳位
 *   - valid=false 时（上电后 SOC 尚未下发 / SOC 下发 0xFF 请求熄灭）
 *     Digit21、Digit22 全部熄灭，不显示 "00"
 */
typedef struct {
    u8   level;         /**< 音量值 0~99                                  */
    bool valid;         /**< 数据有效（已收到 SOC 下发的音量帧）          */
} VolumeLcdData_t;

/**
 * @brief 北京时间显示数据（数据来源：RTC）
 *
 * Digit 1~4 显示 HH:MM；冒号段在两位之间常亮（见 cfg_meter_map.h）。
 */
typedef struct {
    u8   hour;          /**< 时 0~23                    */
    u8   minute;        /**< 分 0~59                    */
    bool valid;         /**< RTC 数据有效               */
} ClockLcdData_t;

/**
 * @brief LCD 总显示数据集合
 */
typedef struct {
    u16              speed_kmh;     /**< 显示车速（km/h，CAN ABS 解算）      */
    u16              rpm;           /**< 显示转速（r/min，CAN EMS 解算）     */
    FuelLcdData_t    fuel;          /**< 燃油 / CNG 显示                      */
    CoolantLcdData_t coolant;       /**< 水温显示                             */
    TpmsLcdData_t    tpms;          /**< 胎压显示                             */
    OdometerData_t   odometer;      /**< 里程显示                             */
    AmbientData_t    ambient;       /**< 外温显示（本面板未使用，保留）       */
    RadioLcdData_t   radio;         /**< 收音机 AM/FM 显示（串口 2）          */
    ClockLcdData_t   clock;         /**< 北京时间显示（RTC）                  */
    VolumeLcdData_t  volume;        /**< 音量显示 Digit21/22（串口 2）        */
    u8               backlight_pct; /**< 背光亮度 0~100（%）                 */
} LcdDisplay_t;

/*===========================================================================
 *  蜂鸣器控制
 *===========================================================================*/

/**
 * @brief 蜂鸣器报警类型
 *        枚举值越大优先级越高（数值大者抢占数值小者）
 */
typedef enum {
    BUZZER_IDLE     = 0,    /**< 无报警       */
    BUZZER_GAS_LEAK = 1,    /**< 燃气泄漏      */
    BUZZER_SEATBELT = 2     /**< 安全带未系    */
} BuzzerAlarm_t;

/**
 * @brief 蜂鸣器运行状态
 *
 * @note pending_restore：被高优先级抢占后待恢复的报警类型，目前主要用于
 *       安全带报警被燃气泄漏抢占后的恢复处理。
 */
typedef struct {
    BuzzerAlarm_t active_alarm;     /**< 当前正在执行的报警类型             */
    BuzzerAlarm_t pending_restore;  /**< 被抢占后待恢复的报警类型           */
    u8            beep_count;       /**< 单轮已鸣叫次数                     */
    u8            cycle_count;      /**< 已循环轮数                         */
    u32           timer_ms;         /**< 当前阶段累计计时（ms）             */
    bool          buzzer_on;        /**< 当前蜂鸣器输出状态                 */
    bool          seatbelt_latched; /**< ★新增：本次条件下 5 循环已跑完，
                                     *   闭锁期间不再重复报警；车速降到
                                     *   阈值以下或系上安全带时自动解除    */
} BuzzerCtrl_t;

/*===========================================================================
 *  低功耗（ACC OFF 休眠）控制状态
 *
 *  LpmState_t / LpmWakeSrc_t 定义在 cfg_lowpower.h（HAL 与 app 共用）。
 *  本结构只保存"状态"，所有判定逻辑在 app/power/lowpower.c。
 *===========================================================================*/
typedef struct {
    LpmState_t   state;         /**< 当前低功耗状态机状态                 */
    LpmWakeSrc_t last_wake;     /**< 最近一次唤醒源（诊断/自检决策用）    */
    u32          sleep_cnt;     /**< 累计进入休眠次数（调试用）           */
    bool         inhibit;       /**< true = 外部模块申请禁止休眠          */
} LowPowerCtrl_t;

/*===========================================================================
 *  系统总上下文（god struct，渐进迁移过程中保留）
 *
 *  TODO: 后续重构计划——按模块所有权拆解：
 *    - buzzer  → app/ui/buzzer.c 自己 static 持有
 *    - led     → app/ui/led_ind.c 自己 static 持有
 *    - lcd     → app/ui/lcd_ui.c 自己 static 持有
 *    - can_rx  → service/can/ 自己持有
 *  目前先维持单结构体，避免大规模改动业务逻辑。
 *===========================================================================*/
typedef struct {
    /* --- 全局粗粒度状态 --- */
    PowerState_t    power_state;            /**< 电源状态                         */
    ClusterState_t  cluster_state;          /**< 仪表唤醒/休眠状态                */
    FuelMode_t      fuel_mode;              /**< 当前燃料模式                     */
    bool            selfcheck_done;         /**< 自检完成标志                     */
    u32             selfcheck_timer_ms;     /**< 自检计时器（ms）                  */
    u32             selfcheck_start_ms;     /**< ★新增：本轮自检的起始时刻（ms）
                                             *   由 SYS_StartSelfCheck() 写入。
                                             *   CAN 上线宽限期以此为基准，见 can_app.c */
    u32             system_tick_ms;         /**< 系统时基（ms，由 SYS_UpdateTick 写入）*/
    bool            blink_1hz_visible;      /**< 1Hz 闪烁可见状态（500ms 翻转一次） */

    /* --- 各业务子结构 --- */
    HardwireInput_t hw_input;       /**< 硬线输入快照     */
    CanRxData_t     can_rx;         /**< CAN 接收数据缓存 */
    PowerVoltage_t  voltage;        /**< 12V 电源电压监测数据 */
    LedStatus_t     led;            /**< 指示灯逻辑状态   */
    LcdDisplay_t    lcd;            /**< LCD 显示数据     */
    BuzzerCtrl_t    buzzer;         /**< 蜂鸣器控制状态   */
    LowPowerCtrl_t  lpm;            /**< 低功耗休眠状态   */
} ClusterSystem_t;

/*===========================================================================
 *  外发 CAN 报文打包结构（仅供 service/can/can_tx_build.c 使用）
 *===========================================================================*/

/** 外发报文 0x380（燃油/里程/安全带等） */
typedef struct {
    u8   seatbelt_drv;          /**< 驾驶员安全带状态     */
    u8   seatbelt_drv_valid;    /**< 安全带状态有效位      */
    u16  odometer_km;           /**< 总里程（10km 精度）   */
    u8   fuel_short_bat;        /**< 油位传感器对电源短路  */
    u8   fuel_short_gnd;        /**< 油位传感器对地短路    */
    u8   fuel_open;             /**< 油位传感器开路        */
    u8   fuel_percent;          /**< 燃油百分比            */
} CanTx380_t;

/** 外发报文 0x381（扩展状态） */
typedef struct {
    u32  odometer_km_x10;       /**< 里程（0.1km 精度）    */
    u8   fuel_percent;          /**< 燃油百分比            */
    u8   seatbelt_drv;          /**< 驾驶员安全带           */
    u8   seatbelt_pass;         /**< 副驾安全带             */
    u8   turn_signal;           /**< 转向：0/1/2/3          */
    u8   fuel_short_bat;        /**< 油位短路（电源）       */
    u8   fuel_short_gnd;        /**< 油位短路（地）          */
    u8   fuel_open;             /**< 油位开路                */
    s8   ambient_temp;          /**< 外温（含偏移编码）     */
} CanTx381_t;

#endif /* CLUSTER_CONTEXT_H */