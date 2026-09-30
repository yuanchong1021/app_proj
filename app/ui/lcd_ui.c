/**
 * @file    lcd_ui.c
 * @brief   LCD 段码屏显示业务逻辑实现
 *
 * 设计要点：
 *   1) 本模块只负责"业务数据 → 图标/数码管位"的映射，所有 LCD 操作
 *      都通过 meter_display.h 的 Meter_Setxxx() API 完成，不直接
 *      接触 HAL_LCD 或 SC5037 寄存器。
 *   2) 闪烁逻辑统一使用 sys->blink_1hz_visible（由 SYS_Task 维护）：
 *        on = blink_1hz_visible  ⇒ 闪烁同相
 *        off = !blink_1hz_visible ⇒ 反相
 *   3) 子刷新函数（LCD_UpdateXxx）都是幂等的，只向 Meter 帧缓冲写入，
 *      不触发 SPI 传输，可安全地重复调用。
 *   4) Meter_Refresh() 在 LCD_Task 末尾统一调用，把本周期所有改动
 *      一次性推送到 SC5037 DDRAM，避免多次刷屏带来的闪烁。
 *
 * 业务名称与面板物理位置的映射关系见：
 *   app/common/config/cfg_meter_map.h
 */

#include "app/ui/lcd_ui.h"
#include "meter_display.h"
#include "cfg_meter_map.h"
#include "sdk_project_config.h"

/*---------------------------------------------------------------------------
 *  内部缓存：当前周期的闪烁可见状态
 *
 *  在 LCD_Task 入口时从 sys->blink_1hz_visible 同步一次，
 *  本周期内多个 UpdateXxx 都使用同一个值，避免出现 fuel 闪烁亮、
 *  coolant 闪烁灭这样不同步的视觉问题。
 *---------------------------------------------------------------------------*/
static bool s_blink_1hz_visible = true;

/*---------------------------------------------------------------------------
 *  9段水温表对应的seg段映射表
 *---------------------------------------------------------------------------*/
static uint8_t Coolant[9] = {ICON_R52, ICON_R51, ICON_R50, ICON_R49, ICON_R48, ICON_R47, ICON_R46, ICON_R45A, ICON_R45B};

/*===========================================================================
 *  初始化
 *===========================================================================*/
void LCD_Init(void)
{
    /*
     * Meter_Init() 完成：
     *   1) SC5037 硬件复位（RESET 引脚拉低再释放）
     *   2) 初始化 SC5037 寄存器（偏压比、帧频等）
     *   3) 清空本地帧缓冲
     *   4) 把全黑缓冲推送到 DDRAM（面板全熄灭）
     */
    Meter_Init();
}

/*===========================================================================
 *  总刷新任务（50ms 周期调用）
 *===========================================================================*/
void LCD_Task(ClusterSystem_t *sys)
{
    GPIOC->PTOR = (1 << 9);  //yc
    /* 同步全局闪烁节拍（所有子函数共用同一个值） */
    s_blink_1hz_visible = sys->blink_1hz_visible;

    LCD_UpdateClock(sys);     /* Digit1-4  北京时间 */
    LCD_UpdateRpm(sys);       /* Digit5-6  转速     */
    LCD_UpdateRadio(sys);     /* Digit7-11 收音机   */
    LCD_UpdateSpeed(sys);     /* Digit12-13 车速    */
    LCD_UpdateFuel(sys);
    LCD_UpdateCoolant(sys);
    LCD_UpdateTpms(sys);
    LCD_UpdateOdometer(sys);  /* Digit14-20 里程    */
    LCD_UpdateVolume(sys);    /* Digit21-22 音量    */
    LCD_UpdateAmbientTemp(sys);

    /* 把本周期所有 Meter_SetIcon / Meter_SetDigit 写入一次性推送到 LCD 控制器 */
    Meter_Refresh();
}

/*===========================================================================
 *  北京时间显示（Digit1-4 = HH:MM，数据来源 RTC）
 *===========================================================================*/
void LCD_UpdateClock(ClusterSystem_t *sys)
{
    const ClockLcdData_t *clk = &sys->lcd.clock;

    if (!clk->valid) {
        /* RTC 无效：4 位全熄灭，冒号熄灭 */
        Meter_SetIcon(METER_ICON_CLOCK_COLON, false);
        for (u8 i = 0u; i < 4u; i++) {
            Meter_SetDigit((uint8_t)(METER_DIGIT_CLOCK_H + i), ' ');
        }
        return;
    }

    /* 冒号常亮（也可改为 1Hz 闪烁，按需求） */
    Meter_SetIcon(METER_ICON_CLOCK_COLON, true);

    /* 时：前导零保留（如 08:05 → 显示 "08:05"），故用前导补 0 而非空格 */
    u8 hh = (clk->hour   > 23u) ? 23u : clk->hour;
    u8 mm = (clk->minute > 59u) ? 59u : clk->minute;

    Meter_SetDigit((uint8_t)(METER_DIGIT_CLOCK_H + 0u), (char)('0' + (hh / 10u)));
    Meter_SetDigit((uint8_t)(METER_DIGIT_CLOCK_H + 1u), (char)('0' + (hh % 10u)));
    Meter_SetDigit((uint8_t)(METER_DIGIT_CLOCK_M + 0u), (char)('0' + (mm / 10u)));
    Meter_SetDigit((uint8_t)(METER_DIGIT_CLOCK_M + 1u), (char)('0' + (mm % 10u)));
}

/*===========================================================================
 *  转速显示（Digit5=个位，Digit6=小数位，形如 X.Y ×1000 r/min）
 *===========================================================================*/
void LCD_UpdateRpm(ClusterSystem_t *sys)
{
    u16 rpm = sys->lcd.rpm;                 /* 单位 r/min */

    /* 单位图标常亮 */
    Meter_SetIcon(METER_ICON_RPM_UNIT, true);
    /* 小数点常亮（X.Y 显示） */
    Meter_SetIcon(METER_ICON_RPM_DP, true);

    /* 转换为 0.1×1000r/min 档位：例 3500 r/min → 35（即 3.5）
     * 四舍五入到 100 r/min。最大 9.9（即 9900 r/min），溢出钳到 99。 */
    u16 d = (u16)((rpm + 50u) / 100u);      /* 35 表示 3.5 */
    if (d > 99u) { d = 99u; }

    u8 intpart  = (u8)(d / 10u);            /* 个位（Digit5） */
    u8 fracpart = (u8)(d % 10u);            /* 小数位（Digit6） */

    Meter_SetDigit((uint8_t)(METER_DIGIT_RPM + 0u), (char)('0' + intpart));
    Meter_SetDigit((uint8_t)(METER_DIGIT_RPM + 1u), (char)('0' + fracpart));
}

/*===========================================================================
 *  收音机 / 蓝牙显示（Digit7-11 = 频率 5 位，数据来源 UART2 ← SOC）
 *
 *  三种模式互斥显示：
 *    FM  → R27(FM) + R30(MHz) + R28(小数点) 点亮，频率区显示 0.1MHz 值
 *    AM  → R26(AM) + R29(kHz) 点亮，频率区显示 1kHz 值
 *    BT  → R66(BT模式) 常亮；R67(BT已连接) 按实际连接状态亮/灭；
 *           AM/FM/kHz/MHz/小数点全熄灭，频率区数码管清空
 *    关闭 → 所有图标熄灭，频率区清空
 *===========================================================================*/
void LCD_UpdateRadio(ClusterSystem_t *sys)
{
    const RadioLcdData_t *r = &sys->lcd.radio;

    /* --- 关闭 / 无数据：所有收音机 + 蓝牙图标全部熄灭 --- */
    if (!r->valid || r->band == RADIO_BAND_NONE) {
        Meter_SetIcon(METER_ICON_RADIO_AM,      false);
        Meter_SetIcon(METER_ICON_RADIO_FM,      false);
        Meter_SetIcon(METER_ICON_RADIO_BT_MODE, false);
        Meter_SetIcon(METER_ICON_RADIO_BT_CONN, false);
        Meter_SetIcon(METER_ICON_RADIO_KHZ,     false);
        Meter_SetIcon(METER_ICON_RADIO_MHZ,     false);
        Meter_SetIcon(METER_ICON_RADIO_DP,      false);
        for (u8 i = 0u; i < METER_RADIO_DIGIT_WIDTH; i++) {
            Meter_SetDigit((uint8_t)(METER_DIGIT_RADIO + i), ' ');
        }
        return;
    }

    if (r->band == RADIO_BAND_FM) {
        /* --- FM：频率单位 0.1MHz（例 875 → 87.5MHz）--- */
        Meter_SetIcon(METER_ICON_RADIO_AM,      false);
        Meter_SetIcon(METER_ICON_RADIO_FM,      true);
        Meter_SetIcon(METER_ICON_RADIO_BT_MODE, false);
        Meter_SetIcon(METER_ICON_RADIO_BT_CONN, false);
        Meter_SetIcon(METER_ICON_RADIO_KHZ,     false);
        Meter_SetIcon(METER_ICON_RADIO_MHZ,     true);
        Meter_SetIcon(METER_ICON_RADIO_DP,      true);

        u16 f = r->freq_raw;                       /* 单位 0.1MHz */
        if (f > 9999u) { f = 9999u; }

        Meter_SetNumberField(METER_DIGIT_RADIO, METER_RADIO_INT_WIDTH, (int32_t)(f / 10u));          /* 整数部 → pos6~8 */
        Meter_SetDigit((uint8_t)METER_DIGIT_RADIO_FRAC, (char)('0' + (f % 10u)));           /* 小数位 → pos9   */
        Meter_SetDigit((uint8_t)(METER_DIGIT_RADIO + METER_RADIO_DIGIT_WIDTH - 1u), ' ');

    } else if (r->band == RADIO_BAND_AM) {
        /* --- AM：频率单位 1kHz（例 1080 → 1080kHz）--- */
        Meter_SetIcon(METER_ICON_RADIO_AM,      true);
        Meter_SetIcon(METER_ICON_RADIO_FM,      false);
        Meter_SetIcon(METER_ICON_RADIO_BT_MODE, false);
        Meter_SetIcon(METER_ICON_RADIO_BT_CONN, false);
        Meter_SetIcon(METER_ICON_RADIO_KHZ,     true);
        Meter_SetIcon(METER_ICON_RADIO_MHZ,     false);
        Meter_SetIcon(METER_ICON_RADIO_DP,      false);
        Meter_SetNumberField(METER_DIGIT_RADIO, METER_RADIO_DIGIT_WIDTH,
                             (int32_t)r->freq_raw);

    } else {
        /* --- BT（蓝牙）模式 ---
         * R66 (BT_MODE)：切换到蓝牙音源，常亮
         * R67 (BT_CONN)：蓝牙设备已连接则点亮，未连接则熄灭
         * 频率区数码管清空，AM/FM/kHz/MHz/小数点全熄灭
         */
        Meter_SetIcon(METER_ICON_RADIO_AM,      false);
        Meter_SetIcon(METER_ICON_RADIO_FM,      false);
        Meter_SetIcon(METER_ICON_RADIO_BT_MODE, true);
        Meter_SetIcon(METER_ICON_RADIO_BT_CONN, r->bt_connected);
        Meter_SetIcon(METER_ICON_RADIO_KHZ,     false);
        Meter_SetIcon(METER_ICON_RADIO_MHZ,     false);
        Meter_SetIcon(METER_ICON_RADIO_DP,      false);
        for (u8 i = 0u; i < METER_RADIO_DIGIT_WIDTH; i++) {
            Meter_SetDigit((uint8_t)(METER_DIGIT_RADIO + i), ' ');
        }
    }
}

/*===========================================================================
 *  车速显示（Digit12=十位，Digit13=个位；百位"1"用图标 R31）
 *===========================================================================*/
void LCD_UpdateSpeed(ClusterSystem_t *sys)
{
    u16 spd = sys->lcd.speed_kmh;           /* km/h */

    /* 单位 km/h 常亮 */
    Meter_SetIcon(METER_ICON_SPEED_UNIT, true);

    if (spd > 199u) { spd = 199u; }         /* 面板最高 199（百位仅 "1"） */

    bool hundred = (spd >= 100u);
    Meter_SetIcon(METER_ICON_SPEED_HUNDRED, hundred);   /* 百位 "1" 图标 */

    u16 rem  = (u16)(spd % 100u);           /* 十位 + 个位 */
    u8  tens = (u8)(rem / 10u);
    u8  ones = (u8)(rem % 10u);

    /* 十位：百位亮时（1xx）十位需补 0（如 105 → "1""0""5"）；
     * 否则速度 < 10 时十位显示空格（如 9 → "  9"）。 */
    char tens_ch;
    if (hundred) {
        tens_ch = (char)('0' + tens);
    } else {
        tens_ch = (tens == 0u) ? ' ' : (char)('0' + tens);
    }

    Meter_SetDigit((uint8_t)(METER_DIGIT_SPEED + 0u), tens_ch);
    Meter_SetDigit((uint8_t)(METER_DIGIT_SPEED + 1u), (char)('0' + ones));
}

/*===========================================================================
 *  燃油 / CNG 显示
 *===========================================================================*/
void LCD_UpdateFuel(ClusterSystem_t *sys)
{
    const FuelLcdData_t *fuel = &sys->lcd.fuel;
    u8 seg = fuel->segments;

    /* 【本次新增】燃油/CNG 区域的静态图标：外边框 R15、字母 "E" R13、
     * 字母 "F" R25，三者都是常亮，与燃油/CNG 格柱的亮灭、模式、故障
     * 状态都无关，之前一直没有代码驱动它们，面板上这三段始终是灭的。
     * 和 R53（水温指示图标）一样归类为"静态图标"，放在 LCD 层常亮驱动。 */
    Meter_SetIcon(METER_ICON_FUEL_BORDER,   true);
    Meter_SetIcon(METER_ICON_FUEL_LETTER_E, true);
    Meter_SetIcon(METER_ICON_FUEL_LETTER_F, true);

    for (u8 i = 1u; i <= 9u; i++) {
        bool on = (i <= seg); // 基础亮灭

        /* 层级 1（最高优先级）：故障模式 - 所有 9 格整体 1Hz 闪烁 */
        if (fuel->fault_blink) {
            on = s_blink_1hz_visible; // 所有格子统一闪烁
        }
        /* 层级 2：低油量报警模式 - 仅当前可见的格子（1~seg）整体闪烁 */
        else if (fuel->blink && on) {
            on = s_blink_1hz_visible; // 当前显示的格子全部闪烁
        }
        /* 层级 3：普通模式，保持 on 不变 */

        Meter_SetIcon(METER_ICON_FUEL_BAR(i), on);
    }

    /* --- 2) 油泵 / CNG 图标互斥 --- */
    if (fuel->mode == FUEL_MODE_CNG) {
        Meter_SetIcon(METER_ICON_CNG,       true);
        Meter_SetIcon(METER_ICON_FUEL_PUMP, false);
    } else {
        Meter_SetIcon(METER_ICON_CNG,       false);
        Meter_SetIcon(METER_ICON_FUEL_PUMP, true);
    }

    /* --- 3) 切换中：CNG 图标 1Hz 闪烁（覆盖第 2 步的常亮设定） --- */
    if (fuel->cng_switching) {
        Meter_SetIcon(METER_ICON_CNG, s_blink_1hz_visible);
    }
}

/*===========================================================================
 *  水温显示（9 格）
 *===========================================================================*/
void LCD_UpdateCoolant(ClusterSystem_t *sys)
{
    const CoolantLcdData_t *ct = &sys->lcd.coolant;
    u8 seg = ct->segments;

    /* 白色水温指示图标 R53 常亮（与红色报警灯 R55 是两个独立段）。
     * 放在 LCD 层而不是 led_ind.c，是因为 R53 属于屏上静态图标，
     * 应与 km/h、×1000r/min 等一起由 LCD 层常亮驱动，与指示灯逻辑无关。
     * （注：LED_Task 自检期间早退的老逻辑已取消，现在自检期间也会刷指示灯，
     *   但 R53 仍留在 LCD 层，保持"静态图标归 LCD"的分层。） */
    Meter_SetIcon(METER_ICON_COOLANT_SYMBOL, true);

    /* 【本次新增】水温区域的静态图标：外边框 R43、字母 "C" R42、
     * 字母 "H" R44，三者都是常亮，与水温格柱、高温报警状态无关，
     * 之前一直没有代码驱动它们。跟 R53 一样归为静态图标，LCD 层常亮驱动。 */
    Meter_SetIcon(METER_ICON_COOLANT_BORDER,   true);
    Meter_SetIcon(METER_ICON_COOLANT_LETTER_C, true);
    Meter_SetIcon(METER_ICON_COOLANT_LETTER_H, true);

    for (u8 i = 1u; i <= 9u; i++) {
        bool on = (i <= seg);

        /* T8/T9 高温闪烁：仅闪烁高温区段（需求 8.4） */
        if (on && ct->blink_high && i >= 8u) {
            on = s_blink_1hz_visible;
        }

        //Meter_SetIcon(METER_ICON_COOLANT_BAR(i), on);
        /* 修正:Coolant[] 为 9 元素数组(下标 0~8),段号 i 为 1~9,
         *      故应取 Coolant[i-1]。原代码 Coolant[i] 既越界(i=9 读到数组外,
         *      属未定义行为),又使整条水温柱整体偏移一格、首段 R52 永不点亮。 */
        Meter_SetIcon((meter_icon_t)Coolant[i - 1u], on);
    }
}

/*===========================================================================
 *  胎压显示（4 轮：1 位整数 + 1 位小数，单位 bar）
 *
 *  数据为 kPa（press_*_kpa），显示换算 bar = kPa/100，保留 1 位小数：
 *    bar10 = (kPa + 5) / 10   // 四舍五入到 0.1bar，例 250kPa → 25 → "2.5"
 *  每轮独立小数点图标（R37/R38/R54/R58）；全局单位 "bar" = R49。
 *  本面板无独立“轮胎轮廓”图标，故不驱动轮廓段。
 *  胎压故障图标 R39：任一轮胎异常（show=false 或 blink）时点亮，异常闪烁。
 *===========================================================================*/
void LCD_UpdateTpms(ClusterSystem_t *sys)
{
    const TpmsLcdData_t *tpms = &sys->lcd.tpms;

    const struct {
        bool         show;
        bool         blink;
        u16          press_kpa;
        uint8_t      int_pos;       /* 整数位 position */
        uint8_t      frac_pos;      /* 小数位 position */
        meter_icon_t dp_icon;       /* 该轮小数点图标 */
    } tires[4u] = {
        { tpms->show_lf, tpms->blink_lf, tpms->press_lf_kpa,
          METER_DIGIT_TPMS_LF_INT, METER_DIGIT_TPMS_LF_FRAC, METER_ICON_TPMS_DP_LF },
        { tpms->show_rf, tpms->blink_rf, tpms->press_rf_kpa,
          METER_DIGIT_TPMS_RF_INT, METER_DIGIT_TPMS_RF_FRAC, METER_ICON_TPMS_DP_RF },
        { tpms->show_lr, tpms->blink_lr, tpms->press_lr_kpa,
          METER_DIGIT_TPMS_LR_INT, METER_DIGIT_TPMS_LR_FRAC, METER_ICON_TPMS_DP_LR },
        { tpms->show_rr, tpms->blink_rr, tpms->press_rr_kpa,
          METER_DIGIT_TPMS_RR_INT, METER_DIGIT_TPMS_RR_FRAC, METER_ICON_TPMS_DP_RR },
    };

    /* 全局 "bar" 单位图标常亮 */
    Meter_SetIcon(METER_ICON_TPMS_BAR_UNIT, true);

    bool any_fault = false;   /* 任一轮异常 → 点亮 R39 故障图标 */

    for (u8 i = 0u; i < 4u; i++) {
        if (tires[i].blink || !tires[i].show) {
            any_fault = true;
        }

        if (!tires[i].show) {
            /* 无数据：整数位与小数位显示 '-'，小数点熄灭 */
            Meter_SetDigit(tires[i].int_pos,  '-');
            Meter_SetDigit(tires[i].frac_pos, '-');
            Meter_SetIcon(tires[i].dp_icon,   false);
        } else {
            /* kPa → 0.1bar，四舍五入；钳到 9.9bar */
            u16 bar10 = (u16)((tires[i].press_kpa + 5u) / 10u);
            if (bar10 > 99u) { bar10 = 99u; }
            u8 ip = (u8)(bar10 / 10u);     /* 整数位 0~9 */
            u8 fp = (u8)(bar10 % 10u);     /* 小数位 0~9 */

            Meter_SetDigit(tires[i].int_pos,  (char)('0' + ip));
            Meter_SetDigit(tires[i].frac_pos, (char)('0' + fp));
            Meter_SetIcon(tires[i].dp_icon,   true);   /* 点亮该轮小数点 */
        }
    }

    /* 胎压故障图标 R39：有异常时 1Hz 闪烁，无异常时熄灭 */
    Meter_SetIcon(METER_ICON_TPMS_FAULT, any_fault ? s_blink_1hz_visible : false);
}

/*===========================================================================
 *  里程显示（ODO / TRIP）
 *
 *  布局（用户已确认）：
 *    Digit14~19（pos 13~18）= 6 位整数位
 *    Digit20   （pos 19）   = 1 位小数位
 *    小数点图标 = R34（METER_ICON_ODO_DP）
 *
 *  TRIP（小计）：trip_dm 单位 0.1km。整数部 = trip_dm/10，小数部 = trip_dm%10。
 *               例 trip_dm=3456 → 整数 345，小数 6 → 显示 "   345.6"，小数点亮。
 *  ODO（总计）：odo_km 整 km，无小数。整数部 = odo_km，小数位熄灭，小数点熄灭。
 *===========================================================================*/
void LCD_UpdateOdometer(ClusterSystem_t *sys)
{
    //sys->lcd.odometer.odo_km = 100;
    //sys->lcd.odometer.show_trip = 0; //yc

    const OdometerData_t *odo = &sys->lcd.odometer;

    if (odo->show_trip) {
        sys->lcd.odometer.odo_or_trip = false;
        u16 dm   = odo->trip_dm;                 /* 0.1km 计 */
        u32 ipart = (u32)(dm / 10u);             /* 整数 km   */
        u8  fpart = (u8)(dm % 10u);              /* 小数 0.1km */
        if (ipart > 999999u) { ipart = 999999u; }

        Meter_SetIcon(METER_ICON_TRIP_LABEL, true);
        Meter_SetIcon(METER_ICON_ODO_LABEL,  false);
        Meter_SetIcon(METER_ICON_ODO_DP,     true);   /* 点亮里程小数点 R34 */

        /* 6 位整数右对齐（Digit14~19） */
        Meter_SetNumberField(METER_DIGIT_ODO, METER_ODO_DIGIT_WIDTH, (int32_t)ipart);
        /* 1 位小数（Digit20） */
        Meter_SetDigit((uint8_t)METER_DIGIT_ODO_FRAC, (char)('0' + fpart));
        Meter_SetIcon(METER_ICOM_ODO_UNIT,true);
    } else {
        sys->lcd.odometer.odo_or_trip = true;
        u32 km = odo->odo_km;
        if (km > 999999u) { km = 999999u; }

        Meter_SetIcon(METER_ICON_TRIP_LABEL, false);
        Meter_SetIcon(METER_ICON_ODO_LABEL,  true);
        Meter_SetIcon(METER_ICON_ODO_DP,     false);  /* 熄灭里程小数点 */

        /* 6 位整数右对齐（Digit14~19），小数位熄灭 */
        Meter_SetNumberField(METER_DIGIT_ODO, METER_ODO_DIGIT_WIDTH, (int32_t)km);
        Meter_SetDigit((uint8_t)METER_DIGIT_ODO_FRAC, ' ');
        Meter_SetIcon(METER_ICOM_ODO_UNIT,true);
    }
}

/*===========================================================================
 *  音量显示（Digit21=十位，Digit22=个位，数据来源 UART2 ← SOC，CMD=0x08）
 *
 *  显示规则：
 *    valid=false（上电未收到 SOC 音量帧 / SOC 下发 0xFF）→ 两位全熄灭
 *    valid=true                                          → 显示 0~99
 *
 *  十位为 0 时的显示风格由 VOLUME_SHOW_LEADING_ZERO 控制：
 *    0（默认）：音量 5 → " 5"（十位熄灭，右对齐，与车速风格一致）
 *    1        ：音量 5 → "05"（十位补零）
 *  若面板效果需要补零，只改此宏即可，无需改动其它代码。
 *===========================================================================*/
#define VOLUME_SHOW_LEADING_ZERO   0

void LCD_UpdateVolume(ClusterSystem_t *sys)
{
    const VolumeLcdData_t *v = &sys->lcd.volume;

    if (!v->valid) {
        /* 未收到 SOC 音量数据：两位数码管熄灭（不显示 "00"，避免误导） */
        Meter_SetDigit((uint8_t)METER_DIGIT_VOLUME_TENS, ' ');
        Meter_SetDigit((uint8_t)METER_DIGIT_VOLUME_ONES, ' ');
        return;
    }

    u8 vol = (v->level > 99u) ? 99u : v->level;   /* 面板 2 位，防御性钳位 */
    u8 tens = (u8)(vol / 10u);
    u8 ones = (u8)(vol % 10u);

#if (VOLUME_SHOW_LEADING_ZERO != 0)
    char tens_ch = (char)('0' + tens);
#else
    char tens_ch = (tens == 0u) ? ' ' : (char)('0' + tens);
#endif

    Meter_SetDigit((uint8_t)METER_DIGIT_VOLUME_TENS, tens_ch);
    Meter_SetDigit((uint8_t)METER_DIGIT_VOLUME_ONES, (char)('0' + ones));
}

/*===========================================================================
 *  车外温度显示
 *
 *  注：经确认，本面板无独立外温数字区，外温功能不显示。
 *      保留空函数仅为兼容 LCD_Task 的调用结构，不写任何段。
 *      若后续硬件版本增加外温显示，可在此恢复实现。
 *===========================================================================*/
void LCD_UpdateAmbientTemp(ClusterSystem_t *sys)
{
    (void)sys;   /* 本面板不显示外温，空实现 */
}

#if 0  /* 旧外温实现保留备查（本面板未使用） */
void LCD_UpdateAmbientTemp_legacy(ClusterSystem_t *sys)
{
    const AmbientData_t *amb = &sys->lcd.ambient;

    if (!amb->valid) {
        /* 数据无效 → 负号熄灭，3 位数字全部显示 '-' */
        Meter_SetIcon(METER_ICON_AMBIENT_SIGN, false);
        for (u8 d = 0u; d < METER_AMBIENT_DIGIT_WIDTH; d++) {
            Meter_SetDigit((uint8_t)(METER_DIGIT_AMBIENT + d), '-');
        }
    } else {
        s16 t = amb->temp_c;
        /* 负温 → 点亮负号段；数字以绝对值右对齐显示 */
        Meter_SetIcon(METER_ICON_AMBIENT_SIGN, (t < 0));
        Meter_SetNumberField(METER_DIGIT_AMBIENT,
                             METER_AMBIENT_DIGIT_WIDTH,
                             (int32_t)((t < 0) ? (s16)(-t) : t));
    }
}
#endif /* 旧外温实现保留备查 */