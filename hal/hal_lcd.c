/**
 * @file  hal_lcd.c
 * @brief LCD HAL 实现（对接 SC5037 段码屏驱动）
 *
 * 逻辑地址 → SC5037 物理 SEG 映射：
 *   0x00~0x08  燃油格 F1~F9     SEG0~SEG8
 *   0x09       油泵图标          SEG9
 *   0x0A       CNG 图标          SEG10
 *   0x10~0x18  水温格 T1~T9     SEG16~SEG24
 *   0x20~0x22  左前胎压(3位数字) SEG32起(每位7SEG)
 *   0x23~0x25  右前胎压          SEG53起
 *   0x26~0x28  左后胎压          SEG74起
 *   0x29~0x2B  右后胎压          SEG95起
 *   0x2C~0x2F  轮胎图标          SEG116~SEG119
 *   0x40~0x45  里程6位数字       SEG128起(每位7SEG)
 *   0x46       ODO标签           SEG170
 *   0x47       TRIP标签          SEG171
 *   0x50~0x52  外温3位数字       SEG176起
 *   0x53       外温负号          SEG197
 *   0x54       °C图标            SEG198
 *
 * 7段编码(bit6=a顶,bit5=b右上,bit4=c右下,bit3=d底,bit2=e左下,bit1=f左上,bit0=g中)
 */
#include "hal_lcd.h"
#include "sc5037.h"

/* 图标类：逻辑0x00~0x0A顺序映射到物理SEG */
static const uint16_t k_icon_seg[11u] = { 0u,1u,2u,3u,4u,5u,6u,7u,8u,9u,10u };

#define SEG_COOLANT_BASE   16u
#define SEG_TPMS_ICON_BASE 116u
static const uint16_t k_tpms_num_base[4u] = {32u, 53u, 74u, 95u};
#define SEG_ODO_BASE       128u
#define SEG_ODO_LABEL      170u
#define SEG_TRIP_LABEL     171u
#define SEG_AMB_BASE       176u
#define SEG_AMB_SIGN       197u
#define SEG_AMB_UNIT       198u
#define SEGS_PER_DIGIT     7u

static const uint8_t k_seg7[10u] = {
    0x7Eu,0x30u,0x6Du,0x79u,0x33u,0x5Bu,0x5Fu,0x70u,0x7Fu,0x7Bu
};
#define SEG7_DASH  0x01u

static void write_digit_seg(uint16_t seg_start, uint8_t code)
{
    for (uint8_t b = 0u; b < 7u; b++) {
        SC5037_SetSegment(seg_start + b, (code & (1u << (6u - b))) != 0u);
    }
}

static void write_number(uint16_t base_seg, uint32_t value, uint8_t digits)
{
    bool     invalid = (value == 0xFFFFFFFFu);
    uint8_t  d[6u]   = {0u};
    if (!invalid) {
        uint32_t v = value;
        for (uint8_t i = 0u; i < digits; i++) { d[i] = (uint8_t)(v % 10u); v /= 10u; }
    }
    bool leading = true;
    for (int8_t i = (int8_t)(digits - 1u); i >= 0; i--) {
        uint16_t seg  = base_seg + (uint16_t)((uint8_t)(digits-1u)-(uint8_t)i) * SEGS_PER_DIGIT;
        uint8_t  code;
        if (invalid)                             { code = SEG7_DASH; }
        else if (leading && d[i]==0u && i!=0)    { code = 0x00u; }
        else                                     { leading = false; code = k_seg7[d[i]]; }
        write_digit_seg(seg, code);
    }
}

void HAL_LCD_Clear(void)
{
    SC5037_HardwareReset();
    SC5037_Init();
}

void HAL_LCD_WriteSeg(u16 seg_addr, u8 seg_data)
{
    bool on = (seg_data != 0u);
    if (seg_addr <= 0x0Au) {
        SC5037_SetSegment(k_icon_seg[seg_addr], on);
        SC5037_FlushSegment(k_icon_seg[seg_addr]);
        return;
    }
    if (seg_addr >= 0x10u && seg_addr <= 0x18u) {
        uint16_t s = SEG_COOLANT_BASE + (seg_addr - 0x10u);
        SC5037_SetSegment(s, on); SC5037_FlushSegment(s); return;
    }
    if (seg_addr >= 0x2Cu && seg_addr <= 0x2Fu) {
        uint16_t s = SEG_TPMS_ICON_BASE + (seg_addr - 0x2Cu);
        SC5037_SetSegment(s, on); SC5037_FlushSegment(s); return;
    }
    if (seg_addr == 0x46u) { SC5037_SetSegment(SEG_ODO_LABEL,  on); SC5037_FlushSegment(SEG_ODO_LABEL);  return; }
    if (seg_addr == 0x47u) { SC5037_SetSegment(SEG_TRIP_LABEL, on); SC5037_FlushSegment(SEG_TRIP_LABEL); return; }
    if (seg_addr == 0x53u) { SC5037_SetSegment(SEG_AMB_SIGN,   on); SC5037_FlushSegment(SEG_AMB_SIGN);   return; }
    if (seg_addr == 0x54u) { SC5037_SetSegment(SEG_AMB_UNIT,   on); SC5037_FlushSegment(SEG_AMB_UNIT);   return; }
}

void HAL_LCD_WriteNum(u16 base_addr, u16 value)
{
    uint32_t v32 = (value == LCD_NUM_INVALID) ? 0xFFFFFFFFu : (uint32_t)value;
    if (base_addr >= 0x20u && base_addr <= 0x2Bu) {
        uint8_t wheel = (uint8_t)((base_addr - 0x20u) / 3u);
        write_number(k_tpms_num_base[wheel], v32, 3u);
        SC5037_Flush(); return;
    }
    if (base_addr == 0x50u) {
        write_number(SEG_AMB_BASE, v32, 3u);
        SC5037_Flush(); return;
    }
}

void HAL_LCD_WriteNum32(u16 base_addr, u32 value)
{
    if (base_addr != 0x40u) { return; }
    if (value > 999999u) { value = 999999u; }
    write_number(SEG_ODO_BASE, value, 6u);
    SC5037_Flush();
}

void HAL_LCD_WriteDecimalPoint(u16 base_addr, u8 pos)
{
    if (base_addr != 0x40u) { return; }
    for (uint8_t i = 0u; i < 6u; i++) {
        SC5037_SetSegment(SEG_ODO_BASE + i * SEGS_PER_DIGIT + 6u, false);
    }
    if (pos == 1u) {
        SC5037_SetSegment(SEG_ODO_BASE + 4u * SEGS_PER_DIGIT + 6u, true);
    }
    SC5037_Flush();
}

void HAL_LCD_SetBacklight(u8 pct)
{
    PINS_DRV_WritePin(LCD_BACKLIGHT_PORT, LCD_BACKLIGHT_PIN, (pct > 0u) ? 1u : 0u);
}
