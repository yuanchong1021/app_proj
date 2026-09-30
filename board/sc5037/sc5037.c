/**
 ******************************************************************************
 * @file    sc5037.c
 * @brief   Sitronix SC5037-G4-1 低层驱动实现 (修复版)
 *
 *  芯片 DDRAM 数据排布(重要,务必与原版区分):
 *  ------------------------------------------------------------------
 *  DDRAM 总容量 = 4 行(COM) x 320 列(SEG) x 3 bit/像素。
 *
 *  !! 关键点 !!
 *  行(COM) 由 "Row Address Set (0x2B)" 单独选择,与 Memory Write 的
 *  数据字节无关。
 *
 *  列地址 "Column Address Set (0x2A)" 的有效范围是 0 ~ 159(不是 0~319!),
 *  因为每个列地址对应 *同一行里相邻的 2 个 SEG*。
 *  发送 "Memory Write (0x2C)" 后,每写 1 字节,数据格式为:
 *
 *      D7(无用) D6 D5 D4 = 高位 SEG(列地址*2+1) 的灰度 F2F1F0
 *      D3(无用) D2 D1 D0 = 低位 SEG(列地址*2  ) 的灰度 F2F1F0
 *
 *  每写完 1 字节,列地址自动 +1,指向下一对 SEG。
 *  (参考 datasheet 第15、17页 RAM Write 示例 与 Figure 7/8)
 *
 *  半字节低 3 位 F2F1F0:
 *       000 → 白(OFF)    001 → 浅灰
 *       011 → 深灰        111 → 黑(ON)
 *
 *  本项目只使用 COM0(1 duty 静态驱动),所以:
 *      - Row Address Set 永远是 0x00 / 0x00(只选 COM0)
 *      - Column Address Set 范围是 0x00 ~ 0x9F(159),覆盖全部 320 个 SEG
 *      - 每个数据字节里的 2 个半字节只会是 0x0(白/灭)或 0x7(黑/亮)
 ******************************************************************************
 */

#include "sc5037.h"
#include <stdint.h>
#include <string.h>

/* =================================================================== */
/*  本地帧缓冲                                                         */
/*  每段只用 1 bit(因为仅 COM0 使用,且不需要灰度)                    */
/*  320 段 = 40 字节                                                   */
/* =================================================================== */
#define SC5037_BUF_BYTES   ((SC5037_NUM_SEGS + 7U) / 8U)  /*向上取整 = 40 每段只用一个bit表示亮灭状态*/

/* DDRAM 一行的列地址个数:320 段 / 2 段每列地址 = 160 (0x00~0x9F) */
#define SC5037_NUM_COLS     (SC5037_NUM_SEGS / 2U)         /* = 160       */

static uint8_t s_fb[SC5037_BUF_BYTES];

/* =================================================================== */
/*  底层辅助函数                                                       */
/* =================================================================== */

void SC5037_WriteCmd(uint8_t cmd)
{
    uint8_t i = 35;
    SC5037_HAL_A0(false);       /* 命令模式:A0 = 0 */
    SC5037_HAL_CS(true);        /* /CS 拉低         */
    SC5037_HAL_SPI_Write(cmd);
    while (i--);
    SC5037_HAL_CS(false);       /* /CS 拉高         */
}

void SC5037_WriteData(uint8_t data)
{
    uint8_t i = 35;
    SC5037_HAL_A0(true);        /* 数据模式:A0 = 1 */
    SC5037_HAL_CS(true);
    SC5037_HAL_SPI_Write(data);
    while (i--);
    SC5037_HAL_CS(false);
}

/* =================================================================== */
/*  复位与初始化                                                       */
/* =================================================================== */

void SC5037_HardwareReset(void)
{
    SC5037_HAL_CS(false);        /* /CS 拉高          */
    SC5037_HAL_RST(false);       /* /RST 拉高(待机)*/
    SC5037_HAL_Delay_Ms(5);
    SC5037_HAL_RST(true);        /* /RST 拉低(复位)*/
    SC5037_HAL_Delay_Ms(5);
    SC5037_HAL_RST(false);       /* 退出复位          */
    SC5037_HAL_Delay_Ms(20);     /* 等待芯片内部稳定  */
}

/**
 * 内部电源模式下的典型上电序列(datasheet 第 26 页):
 *   1. Sleep out
 *   2. Power control = 全部 ON
 *   3. Bias 选择
 *   4. Duty 设置 = 1
 *   5. Vop 设置
 *   6. 帧频
 *   7. 清 DDRAM
 *   8. 开显示
 */
sc5037_status_t SC5037_Init(void)
{
    /* 1) 退出休眠 */
    SC5037_WriteCmd(SC5037_CMD_SLEEP_OUT);                   /* 0x11 */
    SC5037_HAL_Delay_Ms(5);

    /* 2) 打开全部电源模块(OSC/BOOSTER/FOLLOWER/V0/VREF)   */
    SC5037_WriteCmd(SC5037_CMD_POWER_CTRL);                  /* 0xD2 */
    SC5037_WriteData(0x00);

    /* 3) 1/2 bias —— 静态驱动推荐值                          */
    SC5037_WriteCmd(SC5037_CMD_BIAS_SET);                    /* 0xC3 */
    SC5037_WriteData(SC5037_BIAS_1_2);

    /* 4) Duty = 1(静态),只驱动 COM0                         */
    SC5037_WriteCmd(SC5037_CMD_DUTY_SET);                    /* 0xB0 */
    SC5037_WriteData(SC5037_DUTY_1);

    /* 5) 设置 Vop。
     *    !! 重要修复 !!
     *    原代码写的是 0x08 → V0 = 3 + 8*0.04 = 3.32V,电压过低,
     *    几乎不足以产生黑白对比,这很可能是"背光亮但屏不亮"的
     *    直接原因之一。
     *    这里先按原注释意图改为 0x32(V0 = 3 + 50*0.04 = 5.0V)。
     *    如果点亮后对比度仍然偏弱,可在 5V~9V 范围内逐步上调
     *    (调用 SC5037_SetVop 实测调整,不同面板最佳值不同)。
     */
    SC5037_WriteCmd(SC5037_CMD_VOP_SET);                     /* 0xC0 */
    SC5037_WriteData(0x0c);     /* vop 低字节 = 0x32(50)        */
    SC5037_WriteData(0x00);     /* vop 高位(bit8)               */

    /* 6) 帧频 70Hz                                             */
    SC5037_WriteCmd(SC5037_CMD_FRAME_FREQ);                  /* 0xB2 */
    SC5037_WriteData(0x10 | SC5037_FRAME_70HZ);

    /* 7) 清空本地帧缓冲 + 清空芯片 DDRAM                      */
    SC5037_ClearBuffer();
    SC5037_Flush();

    /* 8) 正常显示方向(非反白),打开显示                     */
    SC5037_WriteCmd(SC5037_CMD_INVERSE_OFF);
    SC5037_WriteCmd(SC5037_CMD_DISPLAY_ON);
    SC5037_HAL_Delay_Ms(10);

    return SC5037_OK;
}

/* =================================================================== */
/*  简单命令封装                                                       */
/* =================================================================== */
void SC5037_DisplayOn(void)     { SC5037_WriteCmd(SC5037_CMD_DISPLAY_ON);    }
void SC5037_DisplayOff(void)    { SC5037_WriteCmd(SC5037_CMD_DISPLAY_OFF);   }
void SC5037_Sleep(void)         { SC5037_WriteCmd(SC5037_CMD_SLEEP_IN);      }
void SC5037_Wakeup(void)        { SC5037_WriteCmd(SC5037_CMD_SLEEP_OUT);
                                  SC5037_HAL_Delay_Ms(5); }
void SC5037_AllPointsOn(void)   { SC5037_WriteCmd(SC5037_CMD_ALL_POINTS_ON); }
void SC5037_AllPointsOff(void)  { SC5037_WriteCmd(SC5037_CMD_ALL_POINTS_OFF);}
void SC5037_InverseOn(void)     { SC5037_WriteCmd(SC5037_CMD_INVERSE_ON);    }
void SC5037_InverseOff(void)    { SC5037_WriteCmd(SC5037_CMD_INVERSE_OFF);   }

void SC5037_SetVop(uint16_t vop)
{
    SC5037_WriteCmd(SC5037_CMD_VOP_SET);
    SC5037_WriteData((uint8_t)(vop & 0xFFU));          /* 低 8 位      */
    SC5037_WriteData((uint8_t)((vop >> 8) & 0x01U));   /* 最高位 bit8  */
}

/* =================================================================== */
/*  帧缓冲操作(按 SEG 物理编号,逐位存储,这部分原本没有问题)        */
/* =================================================================== */

void SC5037_ClearBuffer(void)
{
    memset(s_fb, 0, sizeof(s_fb));
}

void SC5037_SetSegment(uint16_t seg, bool on)
{
    /* 越界保护:非法 SEG 编号静默忽略,避免缓冲越界              */
    if (seg >= SC5037_NUM_SEGS) return;

    uint16_t byte_idx = seg >> 3;                  /* seg / 8       */
    uint8_t  bit_mask = (uint8_t)(1U << (seg & 0x7U));

    if (on) {
        s_fb[byte_idx] |=  bit_mask;               /* 置位 → 点亮   */
    } else {
        s_fb[byte_idx] &= (uint8_t)~bit_mask;      /* 清位 → 熄灭   */
    }
}

bool SC5037_GetSegment(uint16_t seg)
{
    if (seg >= SC5037_NUM_SEGS) return false;
    return (s_fb[seg >> 3] & (1U << (seg & 0x7U))) != 0U;
}

/**
 *  把 DDRAM 的访问窗口设置为整个面板:
 *     行:COM0 单独一行(Row 0 ~ 0)
 *     列:列地址 0 ~ 159(对应 SEG0~SEG319,每个列地址覆盖 2 个 SEG)
 *
 *  !! 重要修复 !!
 *  原代码列结束地址写的是 0x3F(63),只能覆盖 SEG0~SEG127,
 *  正确的结束地址应为 0x9F(159),才能覆盖全部 320 个 SEG。
 */
static void SC5037_SetFullWindow(void)
{
    /* Row 窗口:只取 COM0                                        */
    SC5037_WriteCmd(SC5037_CMD_ROW_ADDR_SET);
    SC5037_WriteData(0x00);                 /* row start = 0      */
    SC5037_WriteData(0x00);                 /* row end   = 0      */

    /* Col 窗口:列地址 0 ~ 159(0x9F),对应 SEG0 ~ SEG319        */
    SC5037_WriteCmd(SC5037_CMD_COL_ADDR_SET);
    SC5037_WriteData(0x00);
    SC5037_WriteData((uint8_t)(SC5037_NUM_COLS - 1U));   /* 0x9F */
}

/**
 *  整屏刷新。
 *
 *  !! 重要修复 !!
 *  原代码错误地认为"每个 SEG 对应 2 字节(编码 COM0~COM3)",
 *  实际上协议是"每个字节编码同一行里相邻的 2 个 SEG"。
 *  另外原代码用 seg(0~319)直接索引 40 字节的 s_fb 数组,存在
 *  严重的数组越界访问(seg 最大到 319,但数组只有 40 字节)。
 *
 *  这里改为按"列地址"(0~159)循环,每次取一对 SEG 的状态,
 *  正确拼成 1 个字节后写入,总共只需写 160 字节。
 *
 *  注:原来的"影子缓存局部刷新"逻辑在这种地址自增的协议下并不安全
 *  (跳过某个字节会导致后续所有数据错位到错误的列),这里直接去掉,
 *  改为每次整屏刷新(只有 160 字节,开销很小)。需要局部刷新时请用
 *  SC5037_FlushSegment()。
 */
void SC5037_Flush(void)
{
    SC5037_SetFullWindow();

    /* 发送 Memory Write 命令                                      */
    SC5037_HAL_A0(false);
    SC5037_HAL_CS(true);
    SC5037_HAL_SPI_Write(SC5037_CMD_MEM_WRITE);

    SC5037_HAL_A0(true);
    for (uint16_t col = 0; col < SC5037_NUM_COLS; col++) {

        uint16_t seg_lo = (uint16_t)(col * 2U);       /* 低位 SEG(D3:0) 低字节对应的seg号*/
        uint16_t seg_hi = (uint16_t)(col * 2U + 1U);  /* 高位 SEG(D7:4) 高字节对应的seg号*/

        bool on_lo = SC5037_GetSegment(seg_lo);
        bool on_hi = SC5037_GetSegment(seg_hi);

        uint8_t gray_lo = on_lo ? SC5037_GRAY_BLACK : SC5037_GRAY_WHITE;
        uint8_t gray_hi = on_hi ? SC5037_GRAY_BLACK : SC5037_GRAY_WHITE;

        uint8_t byte = (uint8_t)((gray_hi << 4) | gray_lo);
        SC5037_HAL_SPI_Write(byte);
    }

    SC5037_HAL_CS(false);
}

/**
 *  只刷新单个 SEG。
 *
 *  !! 重要修复 !!
 *  原代码直接把 seg(0~319)当作列地址传给 0x2A(有效范围只有
 *  0~159),并且只写 1 字节、没有考虑该列地址实际覆盖 2 个 SEG。
 *  这里改为:
 *    1. 列地址 = seg / 2
 *    2. 从帧缓冲里同时取出这一对 SEG(seg 本身 + 它的"邻居")的
 *       状态,拼成 1 个字节后写入,避免把邻居的显示状态写错。
 */
void SC5037_FlushSegment(uint16_t seg)
{
    if (seg >= SC5037_NUM_SEGS) return;

    uint16_t col     = (uint16_t)(seg / 2U);
    uint16_t seg_lo  = (uint16_t)(col * 2U);
    uint16_t seg_hi  = (uint16_t)(col * 2U + 1U);

    /* 行窗口:只刷 COM0                                          */
    SC5037_WriteCmd(SC5037_CMD_ROW_ADDR_SET);
    SC5037_WriteData(0x00);
    SC5037_WriteData(0x00);

    /* 列窗口:单个列地址(start = end = col)                     */
    SC5037_WriteCmd(SC5037_CMD_COL_ADDR_SET);
    SC5037_WriteData((uint8_t)(col & 0xFFU));
    SC5037_WriteData((uint8_t)(col & 0xFFU));

    bool on_lo = SC5037_GetSegment(seg_lo);
    bool on_hi = SC5037_GetSegment(seg_hi);

    uint8_t gray_lo = on_lo ? SC5037_GRAY_BLACK : SC5037_GRAY_WHITE;
    uint8_t gray_hi = on_hi ? SC5037_GRAY_BLACK : SC5037_GRAY_WHITE;
    uint8_t byte    = (uint8_t)((gray_hi << 4) | gray_lo);

    /* 写 Memory Write + 1 字节数据                                */
    SC5037_HAL_A0(false);
    SC5037_HAL_CS(true);
    SC5037_HAL_SPI_Write(SC5037_CMD_MEM_WRITE);

    SC5037_HAL_A0(true);
    SC5037_HAL_SPI_Write(byte);

    SC5037_HAL_CS(false);
}
