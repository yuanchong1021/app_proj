/**
 ******************************************************************************
 * @file    sc5037.h
 * @brief   Sitronix SC5037-G4-1 LCD 控制/驱动 IC —— 底层驱动接口
 *
 *          本驱动针对"仪表面板 + 仅接 COM0 的 1/1 静态驱动"场景编写,
 *          使用 4 线 SPI(IF2=L,IF1=H)与 MCU 通讯。
 *
 * @note    所有段都接在 COM0 上,因此只有亮/灭两种状态,不使用灰度功能。
 ******************************************************************************
 */

#ifndef SC5037_H
#define SC5037_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* =================================================================== */
/*  硬件常量                                                           */
/* =================================================================== */
#define SC5037_NUM_SEGS        320U   /* SEG0 ~ SEG319                */
#define SC5037_NUM_COMS        4U     /* COM0 ~ COM3                  */

/* 本仪表面板只用 COM0(1/1 静态驱动)                                 */
#define SC5037_USED_COMS       1U

/* =================================================================== */
/*  命令字定义(详见 datasheet 第 19 页指令表)                         */
/* =================================================================== */
#define SC5037_CMD_NOP              0x00   /* 空操作                   */
#define SC5037_CMD_SW_RESET         0x01   /* 软件复位                 */
#define SC5037_CMD_SLEEP_IN         0x10   /* 进入低功耗休眠           */
#define SC5037_CMD_SLEEP_OUT        0x11   /* 退出休眠                 */
#define SC5037_CMD_INVERSE_OFF      0x20   /* 关闭反白显示             */
#define SC5037_CMD_INVERSE_ON       0x21   /* 打开反白显示             */
#define SC5037_CMD_ALL_POINTS_OFF   0x22   /* 退出全点亮               */
#define SC5037_CMD_ALL_POINTS_ON    0x23   /* 进入全点亮(测试用)     */
#define SC5037_CMD_DISPLAY_OFF      0x28   /* 关显示(寄存器保留)     */
#define SC5037_CMD_DISPLAY_ON       0x29   /* 开显示                   */
#define SC5037_CMD_COL_ADDR_SET     0x2A   /* 列(SEG)地址窗口设置    */
#define SC5037_CMD_ROW_ADDR_SET     0x2B   /* 行(COM)地址窗口设置    */
#define SC5037_CMD_MEM_WRITE        0x2C   /* 开始向 DDRAM 写数据      */
#define SC5037_CMD_MEM_READ         0x2E   /* 从 DDRAM 读数据          */
#define SC5037_CMD_DUTY_SET         0xB0   /* 占空比设置               */
#define SC5037_CMD_FRAME_FREQ       0xB2   /* 帧频设置                 */
#define SC5037_CMD_GRAY_LEVEL       0xB4   /* 灰度电平设置             */
#define SC5037_CMD_LCD_SCAN         0xB7   /* 扫描方向/主从使能        */
#define SC5037_CMD_READ_MOD_ENTER   0xB8   /* 进入读-修改-写模式       */
#define SC5037_CMD_READ_MOD_EXIT    0xB9   /* 退出读-修改-写模式       */
#define SC5037_CMD_VOP_SET          0xC0   /* LCD 驱动电压 Vop 设置    */
#define SC5037_CMD_BIAS_SET         0xC3   /* 偏压比设置               */
#define SC5037_CMD_POWER_CTRL       0xD2   /* 电源控制                 */

/* 占空比参数(0xB0 的参数值)                                         */
#define SC5037_DUTY_1              0x00   /* 1 duty(本项目:静态驱动)*/
#define SC5037_DUTY_2              0x01
#define SC5037_DUTY_3              0x02
#define SC5037_DUTY_4              0x03

/* 偏压比参数(0xC3 的参数值)                                         */
#define SC5037_BIAS_1_2            0x00   /* 1/2 bias(静态推荐)      */
#define SC5037_BIAS_1_3            0x01   /* 1/3 bias(多路扫描)      */

/* 灰度值 F2 F1 F0 —— DDRAM 中每个像素的 3 bit                         */
#define SC5037_GRAY_WHITE          0x0U  /* 000 → 白(熄灭)           */
#define SC5037_GRAY_LIGHT          0x1U  /* 001 → 浅灰                 */
#define SC5037_GRAY_DARK           0x3U  /* 011 → 深灰                 */
#define SC5037_GRAY_BLACK          0x7U  /* 111 → 黑(点亮)           */

/* 帧频参数(0xB2 的参数值,需 OR 上 0x10)                             */
#define SC5037_FRAME_50HZ          0x00
#define SC5037_FRAME_60HZ          0x01
#define SC5037_FRAME_70HZ          0x02   /* 默认                      */
#define SC5037_FRAME_75HZ          0x03
#define SC5037_FRAME_80HZ          0x04
#define SC5037_FRAME_85HZ          0x05
#define SC5037_FRAME_90HZ          0x06
#define SC5037_FRAME_100HZ         0x07
#define SC5037_FRAME_105HZ         0x08
#define SC5037_FRAME_110HZ         0x09
#define SC5037_FRAME_115HZ         0x0A
#define SC5037_FRAME_120HZ         0x0B
#define SC5037_FRAME_130HZ         0x0C
#define SC5037_FRAME_140HZ         0x0D
#define SC5037_FRAME_150HZ         0x0E

/* 返回码                                                               */
typedef enum {
    SC5037_OK       = 0,    /* 成功                     */
    SC5037_ERR      = -1,   /* 一般错误                 */
    SC5037_ERR_ARG  = -2    /* 参数错误                 */
} sc5037_status_t;

typedef enum {
  GPIO_PIN_RESET = 0u, 
  GPIO_PIN_SET   
} GPIO_PinState;

/* =================================================================== */
/*  HAL 硬件抽象层 —— 由用户按目标 MCU 实现                             */
/*  驱动本身不直接操作 GPIO/SPI,全部通过下面 5 个函数。                 */
/* =================================================================== */

/**
 * @brief  片选 /CS 控制
 * @param  active true = 拉低(选中芯片);false = 拉高(释放)
 */
extern void SC5037_HAL_CS(bool active);

/**
 * @brief  复位 /RST 控制
 * @param  active true = 拉低(进入复位);false = 拉高(退出复位)
 */
extern void SC5037_HAL_RST(bool active);

/**
 * @brief  A0(Data/Command 选择)控制,4 线 SPI 使用
 * @param  data_mode true = 数据(A0=1);false = 命令(A0=0)
 */
extern void SC5037_HAL_A0(bool data_mode);

/**
 * @brief  SPI 阻塞发送一个字节,MSB 先发,时钟模式 0 或 3
 */
extern void SC5037_HAL_SPI_Write(uint8_t byte);

/**
 * @brief  毫秒级阻塞延时
 */
extern void SC5037_HAL_Delay_Ms(uint32_t ms);

/* =================================================================== */
/*  公开 API                                                           */
/* =================================================================== */

/**
 * @brief  硬件复位脉冲(操作 /RST 脚),上电后应调用一次,
 *         随后再调用 SC5037_Init() 进行寄存器初始化。
 */
void SC5037_HardwareReset(void);

/**
 * @brief  写一个命令字节(A0=0)
 */
void SC5037_WriteCmd(uint8_t cmd);

/**
 * @brief  写一个数据/参数字节(A0=1)
 */
void SC5037_WriteData(uint8_t data);

/**
 * @brief  1/1 静态驱动的标准初始化流程:
 *         内部升压 ×3 → 设 Vop ≈ 5V → 1/2 bias → duty = 1 → 70Hz
 *         → 清屏 → 打开显示。
 *         调用前请先调用 SC5037_HardwareReset()。
 */
sc5037_status_t SC5037_Init(void);

/**  打开显示(不改变任何参数)                                         */
void SC5037_DisplayOn(void);

/**  关闭显示(保留寄存器与 DDRAM)                                     */
void SC5037_DisplayOff(void);

/**  进入休眠(整机最低功耗)                                           */
void SC5037_Sleep(void);

/**  退出休眠,图像会被恢复                                             */
void SC5037_Wakeup(void);

/**
 * @brief  运行时重新设置驱动电压 Vop
 *         V0 = 3.0 + vop * 0.04 (V),vop 范围 0~453(9 位)
 *         1 duty 下 V0 建议 3.0V~10.8V。
 */
void SC5037_SetVop(uint16_t vop);

/**  清空本地帧缓冲(只清 RAM 不刷屏,需再调用 SC5037_Flush)            */
void SC5037_ClearBuffer(void);

/**
 * @brief  在本地帧缓冲中置位/清位某一段(按 SEG 物理编号)
 * @param  seg 0 ~ 319
 * @param  on  true = 点亮,false = 熄灭
 */
void SC5037_SetSegment(uint16_t seg, bool on);

/**  查询本地帧缓冲中某段的当前状态                                     */
bool SC5037_GetSegment(uint16_t seg);

/**
 * @brief  把整个本地帧缓冲刷新到 SC5037 的 DDRAM
 *         修改完各段之后调用一次即可,显示保持由芯片自动完成。
 */
void SC5037_Flush(void);

/**
 * @brief  只刷新单个段,省去全屏遍历,适合高速闪烁
 */
void SC5037_FlushSegment(uint16_t seg);

/**  全屏点亮(不改帧缓冲,用于面板自检)                               */
void SC5037_AllPointsOn(void);

/**  退出全屏点亮                                                       */
void SC5037_AllPointsOff(void);

/**  反白显示(黑白对调)                                               */
void SC5037_InverseOn(void);

/**  退出反白显示                                                       */
void SC5037_InverseOff(void);


#ifdef __cplusplus
}
#endif

#endif /* SC5037_H */
