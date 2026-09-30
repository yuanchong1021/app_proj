/**
 ******************************************************************************
 * @file    meter_display.c
 * @brief   仪表段码 LCD 上层显示 —— 段映射表与高层绘制函数实现
 *
 *  段映射来源:从 CAD 原理图截取的 PIN/COM 对照表(30 位数码管 + 77 个 R 图标)
 *  所有段都接在 COM0 上,属于 1/1 静态驱动。
 ******************************************************************************
 */

#include "meter_display.h"
#include "sc5037.h"

#include <stddef.h>

/* =================================================================== */
/*  一、图标段 → 物理 SEG 编号 映射表                                  */
/*                                                                      */
/*  表项顺序必须与 meter_display.h 中 meter_icon_t 枚举顺序一致。       */
/*  这些数据完全对应 CAD 原理图截图。                                   */
/* =================================================================== */
static const uint16_t g_meter_icon_table[ICON__COUNT] = {
    /* ---------- R1 ~ R15 ---------- */
    [ICON_R1 ] =  30,   /* PIN=SEG30  / COM=R1   */
    [ICON_R2 ] =  45,   /* PIN=SEG45  / COM=R2   */
    [ICON_R3 ] =  46,
    [ICON_R4 ] =  47,
    [ICON_R5 ] =  48,
    [ICON_R6 ] =  49,
    [ICON_R7 ] =  50,
    [ICON_R8 ] =  51,
    [ICON_R9 ] =  52,
    [ICON_R10] =  60,
    [ICON_R11] =  68,
    [ICON_R12] =  69,
    [ICON_R13] =  70,
    [ICON_R14] =  71,
    [ICON_R15] =  72,

    /* ---------- R16 拆成 a/b 两段 ---------- */
    [ICON_R16A] = 81,   /* PIN=SEG81 / COM=R16a */
    [ICON_R16B] = 80,   /* PIN=SEG80 / COM=R16b */

    /* ---------- R17 ~ R44 ---------- */
    [ICON_R17] =  79,
    [ICON_R18] =  78,
    [ICON_R19] =  77,
    [ICON_R20] =  76,
    [ICON_R21] =  75,
    [ICON_R22] =  74,
    [ICON_R23] =  73,
    [ICON_R24] =  82,
    [ICON_R25] =  83,
    [ICON_R26] =  84,
    [ICON_R27] =  85,
    [ICON_R28] = 107,
    [ICON_R29] = 122,
    [ICON_R30] = 123,
    [ICON_R31] = 124,
    [ICON_R32] = 133,   /* 注意:CAD 图中 R32 对应 SEG133,R33 对应 SEG132 */
    [ICON_R33] = 132,
    [ICON_R34] = 176,
    [ICON_R35] = 191,   /* 修正:CAD/SEG 表中 R35 对应 SEG191(原误填 SEG189=13G) */
    [ICON_R36] = 192,   /* 修正:CAD/SEG 表中 R36 对应 SEG192(原误填 SEG190=13B) */
    [ICON_R37] = 214,
    [ICON_R38] = 229,
    [ICON_R39] = 237,
    /* SEG238 在 CAD 中标记为 "--",未接任何段,不出现在表中 */
    [ICON_R40] = 239,
    [ICON_R41] = 240,
    [ICON_R42] = 241,
    [ICON_R43] = 242,
    [ICON_R44] = 243,

    /* ---------- R45 拆成 a/b 两段 ---------- */
    [ICON_R45A] = 245,  /* PIN=SEG245 / COM=R45a */
    [ICON_R45B] = 244,  /* PIN=SEG244 / COM=R45b */

    /* ---------- R46 ~ R75 ---------- */
    [ICON_R46] = 246,
    [ICON_R47] = 247,
    [ICON_R48] = 248,
    [ICON_R49] = 249,
    [ICON_R50] = 250,
    [ICON_R51] = 251,
    [ICON_R52] = 252,
    [ICON_R53] = 253,
    [ICON_R54] = 261,
    [ICON_R55] = 269,
    [ICON_R56] = 270,
    [ICON_R57] = 271,
    [ICON_R58] = 279,
    [ICON_R59] = 287,
    [ICON_R60] = 288,
    [ICON_R61] = 289,
    [ICON_R62] = 290,
    [ICON_R63] = 291,
    [ICON_R64] = 292,
    [ICON_R65] = 293,
    [ICON_R66] = 294,
    [ICON_R67] = 295,
    [ICON_R68] = 296,
    [ICON_R69] = 297,
    [ICON_R70] = 298,
    [ICON_R71] = 299,
    [ICON_R72] = 300,
    [ICON_R73] = 301,
    [ICON_R74] = 302,
    [ICON_R75] = 303,
};

/* =================================================================== */
/*  二、数码管位 → 7 段 SEG 映射表                                     */
/*                                                                      */
/*  7 段布局:                                                          */
/*        aaa                                                           */
/*       f   b                                                          */
/*       f   b                                                          */
/*        ggg                                                           */
/*       e   c                                                          */
/*       e   c                                                          */
/*        ddd                                                           */
/*                                                                      */
/*  数组顺序:{a, b, c, d, e, f, g}                                     */
/*  数组下标 = position,其中 position 0 对应面板 Digit 1,              */
/*           position 29 对应 Digit 30。                                 */
/* =================================================================== */
typedef struct {
    uint16_t seg_a;
    uint16_t seg_b;
    uint16_t seg_c;
    uint16_t seg_d;
    uint16_t seg_e;
    uint16_t seg_f;
    uint16_t seg_g;
} digit_map_t;

static const digit_map_t g_digit_seg_table[METER_NUM_DIGITS] = {
    /* Digit 1  (position 0 ) */  { 16,  22,  20,  19,  18,  17,  21 },
    /* Digit 2  (position 1 ) */  { 23,  29,  27,  26,  25,  24,  28 },
    /* Digit 3  (position 2 ) */  { 31,  37,  35,  34,  33,  32,  36 },
    /* Digit 4  (position 3 ) */  { 38,  44,  42,  41,  40,  39,  43 },
    /* Digit 5  (position 4 ) */  { 53,  59,  57,  56,  55,  54,  58 },
    /* Digit 6  (position 5 ) */  { 61,  67,  65,  64,  63,  62,  66 },
    /* Digit 7  (position 6 ) */  { 86,  92,  90,  89,  88,  87,  91 },
    /* Digit 8  (position 7 ) */  { 93,  99,  97,  96,  95,  94,  98 },
    /* Digit 9  (position 8 ) */  { 100, 106, 104, 103, 102, 101, 105 },
    /* Digit 10 (position 9 ) */  { 108, 114, 112, 111, 110, 109, 113 },
    /* Digit 11 (position 10) */  { 115, 121, 119, 118, 117, 116, 120 },
    /* Digit 12 (position 11) */  { 125, 131, 129, 128, 127, 126, 130 },
    /* Digit 13 (position 12) */  { 184, 190, 188, 187, 186, 185, 189 }, /* 修正:b=SEG190(13B),g=SEG189(13G);原误填 192/191 占用了 R36/R35 段 */
    /* Digit 14 (position 13) */  { 134, 140, 138, 137, 136, 135, 139 },
    /* Digit 15 (position 14) */  { 141, 147, 145, 144, 143, 142, 146 },
    /* Digit 16 (position 15) */  { 148, 154, 152, 151, 150, 149, 153 },
    /* Digit 17 (position 16) */  { 155, 161, 159, 158, 157, 156, 160 },
    /* Digit 18 (position 17) */  { 162, 168, 166, 165, 164, 163, 167 },
    /* Digit 19 (position 18) */  { 169, 175, 173, 172, 171, 170, 174 },
    /* Digit 20 (position 19) */  { 177, 183, 181, 180, 179, 178, 182 },
    /* Digit 21 (position 20) */  { 193, 199, 197, 196, 195, 194, 198 },
    /* Digit 22 (position 21) */  { 200, 206, 204, 203, 202, 201, 205 },
    /* Digit 23 (position 22) */  { 207, 213, 211, 210, 209, 208, 212 },
    /* Digit 24 (position 23) */  { 215, 221, 219, 218, 217, 216, 220 },
    /* Digit 25 (position 24) */  { 222, 228, 226, 225, 224, 223, 227 },
    /* Digit 26 (position 25) */  { 230, 236, 234, 233, 232, 231, 235 },
    /* Digit 27 (position 26) */  { 254, 260, 258, 257, 256, 255, 259 },
    /* Digit 28 (position 27) */  { 262, 268, 266, 265, 264, 263, 267 },
    /* Digit 29 (position 28) */  { 272, 278, 276, 275, 274, 273, 277 },
    /* Digit 30 (position 29) */  { 280, 286, 284, 283, 282, 281, 285 },
};

/* =================================================================== */
/*  三、7 段字形表(字符 → 7 段 bit 位)                                */
/*                                                                      */
/*  bit0 = a, bit1 = b, bit2 = c, bit3 = d, bit4 = e, bit5 = f, bit6 = g */
/* =================================================================== */

#define SEG_A_BIT (1U<<0)
#define SEG_B_BIT (1U<<1)
#define SEG_C_BIT (1U<<2)
#define SEG_D_BIT (1U<<3)
#define SEG_E_BIT (1U<<4)
#define SEG_F_BIT (1U<<5)
#define SEG_G_BIT (1U<<6)

static uint8_t char_to_7seg(char c)
{
    switch (c) {
        /* 阿拉伯数字 */
        case '0': return SEG_A_BIT|SEG_B_BIT|SEG_C_BIT|SEG_D_BIT|SEG_E_BIT|SEG_F_BIT;
        case '1': return SEG_B_BIT|SEG_C_BIT;
        case '2': return SEG_A_BIT|SEG_B_BIT|SEG_D_BIT|SEG_E_BIT|SEG_G_BIT;
        case '3': return SEG_A_BIT|SEG_B_BIT|SEG_C_BIT|SEG_D_BIT|SEG_G_BIT;
        case '4': return SEG_B_BIT|SEG_C_BIT|SEG_F_BIT|SEG_G_BIT;
        case '5': return SEG_A_BIT|SEG_C_BIT|SEG_D_BIT|SEG_F_BIT|SEG_G_BIT;
        case '6': return SEG_A_BIT|SEG_C_BIT|SEG_D_BIT|SEG_E_BIT|SEG_F_BIT|SEG_G_BIT;
        case '7': return SEG_A_BIT|SEG_B_BIT|SEG_C_BIT;
        case '8': return 0x7F;       /* 全部 7 段点亮 */
        case '9': return SEG_A_BIT|SEG_B_BIT|SEG_C_BIT|SEG_D_BIT|SEG_F_BIT|SEG_G_BIT;

        /* 十六进制 A-F(大小写都支持) */
        case 'A': case 'a':
            return SEG_A_BIT|SEG_B_BIT|SEG_C_BIT|SEG_E_BIT|SEG_F_BIT|SEG_G_BIT;
        case 'b': case 'B':
            return SEG_C_BIT|SEG_D_BIT|SEG_E_BIT|SEG_F_BIT|SEG_G_BIT;
        case 'C':
            return SEG_A_BIT|SEG_D_BIT|SEG_E_BIT|SEG_F_BIT;
        case 'c':
            return SEG_D_BIT|SEG_E_BIT|SEG_G_BIT;
        case 'd': case 'D':
            return SEG_B_BIT|SEG_C_BIT|SEG_D_BIT|SEG_E_BIT|SEG_G_BIT;
        case 'E': case 'e':
            return SEG_A_BIT|SEG_D_BIT|SEG_E_BIT|SEG_F_BIT|SEG_G_BIT;
        case 'F': case 'f':
            return SEG_A_BIT|SEG_E_BIT|SEG_F_BIT|SEG_G_BIT;

        /* 特殊字符 */
        case '-': return SEG_G_BIT;   /* 减号 = 仅中间一段 */
        case ' ': return 0x00;        /* 空格 = 整位熄灭   */
        default:  return 0x00;        /* 未知字符 → 熄灭   */
    }
}

/* =================================================================== */
/*  四、公开 API 实现                                                  */
/* =================================================================== */

void Meter_Init(void)
{
    SC5037_HardwareReset();     /* 先做硬件复位              */
    SC5037_Init();              /* 再初始化寄存器            */
    SC5037_ClearBuffer();       /* 清空本地帧缓冲            */
    SC5037_Flush();             /* 同步到 DDRAM(全熄灭)    */
}

void Meter_ClearAll(void)
{
    SC5037_ClearBuffer();
}

void Meter_SetIcon(meter_icon_t icon, bool on)
{
    /* 非法图标编号静默忽略                                   */
    if ((unsigned)icon >= ICON__COUNT) return;

    uint16_t seg = g_meter_icon_table[icon];
    SC5037_SetSegment(seg, on);
}

void Meter_SetIconR(uint8_t r_num, bool on)
{
    /* 把 R 编号翻译成 enum 索引:
     *   R1..R15     → enum  0..14
     *   R16         → 未定义(需用 ICON_R16A 或 ICON_R16B)
     *   R17..R44    → enum 17..44
     *   R45         → 未定义(需用 ICON_R45A 或 ICON_R45B)
     *   R46..R75    → enum 47..76                               */
    meter_icon_t icon;
    if (r_num >= 1U && r_num <= 15U) {
        icon = (meter_icon_t)(r_num - 1U);
    } else if (r_num >= 17U && r_num <= 44U) {
        icon = (meter_icon_t)r_num;                 /* R17 → 17 */
    } else if (r_num >= 46U && r_num <= 75U) {
        icon = (meter_icon_t)(r_num + 1U);          /* R46 → 47 */
    } else {
        /* 包括 r_num = 0、16、45 或 > 75,均视为非法 */
        return;
    }
    Meter_SetIcon(icon, on);
}

void Meter_SetDigit(uint8_t position, char c)
{
    if (position >= METER_NUM_DIGITS) return;

    uint8_t bits = char_to_7seg(c);
    const digit_map_t *m = &g_digit_seg_table[position];

    /* 逐段把字符对应的位映射到面板上                        */
    SC5037_SetSegment(m->seg_a, (bits & SEG_A_BIT) != 0U);
    SC5037_SetSegment(m->seg_b, (bits & SEG_B_BIT) != 0U);
    SC5037_SetSegment(m->seg_c, (bits & SEG_C_BIT) != 0U);
    SC5037_SetSegment(m->seg_d, (bits & SEG_D_BIT) != 0U);
    SC5037_SetSegment(m->seg_e, (bits & SEG_E_BIT) != 0U);
    SC5037_SetSegment(m->seg_f, (bits & SEG_F_BIT) != 0U);
    SC5037_SetSegment(m->seg_g, (bits & SEG_G_BIT) != 0U);
}

void Meter_SetNumberField(uint8_t start_pos, uint8_t width, int32_t value)
{
    if (width == 0U) return;
    if ((uint16_t)start_pos + width > METER_NUM_DIGITS) return;

    bool negative = (value < 0);
    uint32_t v = (uint32_t)(negative ? -value : value);

    /* 先准备一个右对齐的字符缓冲,其余位置先填0           */
    char buf[METER_NUM_DIGITS];
    for (uint8_t i = 0; i < width; i++) buf[i] = '0';

    int pos = (int)width - 1;
    if (v == 0U) {
        buf[pos--] = '0';
    } else {
        while (v != 0U && pos >= 0) {
            buf[pos--] = (char)('0' + (v % 10U));
            v /= 10U;
        }
    }
    /* 处理溢出:数字太大,宽度不够 → 整段显示 '-' 作为提示   */
    if (v != 0U) {
        for (uint8_t i = 0; i < width; i++) buf[i] = '-';
    } else if (negative) {
        if (pos >= 0) {
            buf[pos] = '-';     /* 负号放在最前面的空位       */
        } else {
            /* 没有空位容纳负号 → 整段 '-' 提示溢出           */
            for (uint8_t i = 0; i < width; i++) buf[i] = '-';
        }
    }

    /* 把字符逐位写入面板对应数字位                          */
    for (uint8_t i = 0; i < width; i++) {
        Meter_SetDigit((uint8_t)(start_pos + i), buf[i]);
    }
}

void Meter_SetNumber(uint8_t width, int32_t value)
{
    Meter_SetNumberField(0U, width, value);
}

void Meter_SetBatteryLevel(uint8_t level_0_to_4)
{
    /* 示例:假设 R1=电池外框,R2..R5 = 4 格电量条
     * 具体 R 编号请根据 CAD 图中真实图标位置调整。          */
    Meter_SetIcon(ICON_R1, true);
    Meter_SetIcon(ICON_R2, level_0_to_4 >= 1U);
    Meter_SetIcon(ICON_R3, level_0_to_4 >= 2U);
    Meter_SetIcon(ICON_R4, level_0_to_4 >= 3U);
    Meter_SetIcon(ICON_R5, level_0_to_4 >= 4U);
}

void Meter_Refresh(void)
{
    SC5037_Flush();
}

void Meter_SelfTest(void)
{
    /* 遍历图标表 + 数字段表,所有 SEG 都置位,刷屏             */
    SC5037_ClearBuffer();

    for (int i = 0; i < ICON__COUNT; i++) {
        SC5037_SetSegment(g_meter_icon_table[i], true);
    }
    for (int i = 0; i < METER_NUM_DIGITS; i++) {
        const digit_map_t *m = &g_digit_seg_table[i];
        SC5037_SetSegment(m->seg_a, true);
        SC5037_SetSegment(m->seg_b, true);
        SC5037_SetSegment(m->seg_c, true);
        SC5037_SetSegment(m->seg_d, true);
        SC5037_SetSegment(m->seg_e, true);
        SC5037_SetSegment(m->seg_f, true);
        SC5037_SetSegment(m->seg_g, true);
    }
    SC5037_Flush();
}

void Meter_SetSegmentRaw(uint16_t seg, bool on)
{
    /* 调试用:绕过映射表直接操作物理 SEG                    */
    SC5037_SetSegment(seg, on);
}
