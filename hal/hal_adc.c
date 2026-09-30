/**
 * @file  hal_adc.c
 * @brief ADC HAL 实现（DMA 循环模式 + 中断回调）
 *
 * 工作原理：
 *   ADC0 以连续序列模式循环采样，每个 FIFO 结果触发 DMA_REQ_ADC0，
 *   DMA 以循环模式将结果搬到 s_adc_raw[]，每轮搬完触发 DMA 完成中断，
 *   回调把结果拷到 s_result[]，供主循环零等待读取。
 *
 * 通道映射（index = s_result[] 下标）：
 *   0 = ADC_SE4 → 12V 电源分压（整车已确认）
 *   1 = ADC_SE5 → 燃油传感器
 *
 * 本次新增：HAL_ADC_ReadVbatMv() / HAL_ADC_ReadRaw()
 *
 * ADC0 FIFO 寄存器地址：ADC0_BASE(0x40040000) + 0x004C = 0x4004004C
 */
#include "hal_adc.h"
#include "cfg_power.h"
#include "dma_config.h"
#include "dma_driver.h"
#include "adc_driver.h"
#include "adc_config.h"      /* extern adc_config0，见下面 HAL_ADC_Init() 的说明 */
#include "interrupt_manager.h"
#include "YTM32B1MC0.h"
#include "hal_tick.h"
#include <string.h>

/**
 * s_ready 长时间不为 true 的自愈超时（ms）。
 *
 * 正常情况下 HAL_ADC_Init() 之后最多几个 ADC 转换周期（us~低个位 ms）
 * DMA 就应该完成第一轮循环、置位 s_ready。500ms 已经是几十倍的余量。
 *
 * 若实测发现"IGN 重新上电、电压跌到 9V 以下"时 s_ready 长期停在 0、
 * 且怎么等都不恢复：大概率是 ADC/DMA 在这个边界条件下没能正常触发
 * 第一次转换或中断（具体是触发源没建立、还是中断没使能，需要结合
 * adc_driver.c / dma_driver.c 的实现继续查，这两个是厂商 SDK，
 * 本次未包含在你发我的代码里）。在没有确认底层驱动具体行为之前，
 * 这里先加一层应用层兜底：超时就重新跑一遍 HAL_ADC_Init()，
 * 而不是任由 s_ready 停在 0 直到下次整机重新上电才恢复。
 */
#define ADC_READY_TIMEOUT_MS    500u

#define ADC_INST            0u
#define ADC_CHANNEL_COUNT   2u
#define ADC_FULLSCALE       4095u
#define ADC_RPULL_OHM       117u      /* 燃油上拉电阻 Ω，按原理图修改 */
#define ADC0_FIFO_ADDR      (ADC0_BASE + 0x004CUL)

/* 12V 分压比分子/分母，编译期常量 */
#define VBAT_DIV_NUM        (CFG_VBAT_DIV_R1_OHM + CFG_VBAT_DIV_R2_OHM)
#define VBAT_DIV_DEN        (CFG_VBAT_DIV_R2_OHM)

/* DMA 目标缓冲（中断回调中写入） */
static volatile uint16_t s_adc_raw[ADC_CHANNEL_COUNT];
/* 主循环安全读取副本 */
static volatile uint16_t s_result[ADC_CHANNEL_COUNT];
static volatile bool     s_ready = false;

/* s_ready 自愈超时计时用（仅主循环访问，非中断上下文） */
static u32               s_wait_start_ms   = 0u;
static bool              s_wait_start_set  = false;

dma_loop_transfer_config_t adcLoopTransferConfig = { .triggerLoopIterationCount = 1,
                                                     .dstOffsetEnable = true,
                                                     .triggerLoopOffset = -(ADC_CHANNEL_COUNT * 2) };

/* DMA 完成中断回调 */
static void adc_dma_callback(void *param, dma_chn_status_t status)
{
    (void)param;
    if (status == DMA_CHN_NORMAL) {
        for (uint8_t i = 0u; i < ADC_CHANNEL_COUNT; i++) {
            s_result[i] = s_adc_raw[i];
        }
        s_ready = true;
    }
    /* DMA 循环模式自动重置，无需手动重启 */
}

static void adc_dma_setup(void)
{
    dma_transfer_config_t cfg;
    memset(&cfg, 0, sizeof(cfg));

    cfg.srcAddr               = ADC0_FIFO_ADDR;
    cfg.destAddr              = (uint32_t)s_adc_raw;
    cfg.srcTransferSize       = DMA_TRANSFER_SIZE_2B;
    cfg.destTransferSize      = DMA_TRANSFER_SIZE_2B;
    cfg.srcOffset             = 0;
    cfg.destOffset            = 0x02u;
    cfg.srcLastAddrAdjust     = 0;
    cfg.destLastAddrAdjust    = -(ADC_CHANNEL_COUNT*2);
    cfg.transferLoopByteCount = ADC_CHANNEL_COUNT*2;
    cfg.loopTransferConfig    = &adcLoopTransferConfig,
    cfg.interruptEnable       = true;

    DMA_DRV_InstallCallback(ADC_DMA_CHANNEL, adc_dma_callback, NULL);
    DMA_DRV_ConfigLoopTransfer(ADC_DMA_CHANNEL, &cfg);
    DMA_DRV_StartChannel(ADC_DMA_CHANNEL);
}

void HAL_ADC_Init(void)
{
    memset((void *)s_adc_raw, 0, sizeof(s_adc_raw));
    memset((void *)s_result,  0, sizeof(s_result));
    s_ready          = false;
    s_wait_start_set = false;   /* HAL_ADC_Task() 里重新起算超时窗口 */

    /* 【修复】原来这里只做 DMA 配置 + Enable + Start，没有重新下发
     * ADC_DRV_ConfigConverter()。这份配置（触发方式、序列通道、
     * DMA watermark、采样/启动时间……）在原实现里只在 main.c 的
     * Board_Init() 里下发过一次。
     *
     * main.c 的 HAL_ADC_Init() 调用永远跟在 Board_Init() 后面，凑巧没
     * 暴露问题；但 hal_lowpower.c 的 lp_periph_on()（IGN 唤醒路径）和
     * 本文件新增的 HAL_ADC_Task() 自愈重试，都是单独调 HAL_ADC_Init()、
     * 不会再走一遍 Board_Init()。如果 ADC 的配置寄存器在这期间因为
     * 时钟异常（见 clock_config.c 里 FXOSC 时钟监控）或电源纹波被清空/
     * 破坏，之后不管重新唤醒多少次、自愈重试多少次，下发的都只有
     * Enable+Start，转换器本身一直停留在没有正确配置序列/触发方式的
     * 状态，自然永远等不到第一次转换完成——只有真正整机断电重上电、
     * 重新走一遍 Board_Init()，配置才会被重新下发。这与你反馈的
     * "只有重新下电上电12V才会恢复"完全对应。
     *
     * 现在把 ADC_DRV_ConfigConverter() 挪到这里、每次 HAL_ADC_Init()
     * 都重新下发一遍，三条路径（冷启动 / 唤醒 / 自愈重试）就都不再
     * 依赖"配置寄存器本来就没丢"这个假设了。 */
    ADC_DRV_ConfigConverter(ADC_INST, &adc_config0);

    adc_dma_setup();
    ADC_DRV_Enable(ADC_INST);
    ADC_DRV_Start(ADC_INST);
}

/**
 * @brief ADC 就绪自愈轮询
 *
 * 建议以几十~100ms 周期挂进调度表（main.c 里 HAL_ADC_Init() 一次性调用，
 * 不受调度器管理；这个函数是给调度器周期调用的，两者不冲突）。
 *
 * 逻辑：s_ready 已经是 true → 什么都不做，顺便清一下计时状态。
 *       s_ready 还是 false → 记录/检查已经等了多久，超过
 *       ADC_READY_TIMEOUT_MS 就认为 DMA/ADC 卡住了，重新跑一遍
 *       HAL_ADC_Init() 尝试拉起来。
 */
void HAL_ADC_Task(void)
{
    u32 now;

    if (s_ready) {
        s_wait_start_set = false;
        return;
    }

    now = HAL_GetTickMs();

    if (!s_wait_start_set) {
        s_wait_start_ms  = now;
        s_wait_start_set = true;
        return;
    }

    if ((u32)(now - s_wait_start_ms) >= ADC_READY_TIMEOUT_MS) {
        HAL_ADC_Init();   /* 重新配置 DMA 循环传输 + 重启 ADC，重新起算超时 */
    }
}

u16 HAL_ADC_ReadRaw(u8 channel)
{
    if (channel >= ADC_CHANNEL_COUNT) { return 0u; }
    return (u16)s_result[channel];
}

u16 HAL_ADC_ReadOhms(u8 channel)
{
    if (channel >= ADC_CHANNEL_COUNT) { return 0xFFFFu; }
    uint16_t raw = s_result[channel];
    if (raw == 0u)            { return 0u;      }
    if (raw >= ADC_FULLSCALE) { return 0xFFFFu; }
    uint32_t r = (uint32_t)ADC_RPULL_OHM * raw / (ADC_FULLSCALE - raw);
    return (r > 0xFFFFu) ? 0xFFFFu : (u16)r;
}

s16 HAL_ADC_ReadTempRaw(u8 channel)
{
    if (channel >= ADC_CHANNEL_COUNT) { return 0; }
    return (s16)s_result[channel];
}

/*---------------------------------------------------------------------------
 *  12V 电源电压
 *
 *  原理图：BAT_12V ─[R60 100K]─[R61 33K]─┬─ R64 33K ─ GND
 *                                         └─[R62 10K]─ 12V_AD ─[C60 1nF]─ GND
 *  R62/C60 为 ADC 前抗混叠 RC，不参与直流分压。
 *
 *    V_adc = raw × VREF / 4095
 *    VBAT  = V_adc × (R1 + R2) / R2= V_adc × 166 / 33
 *
 *  两步均做四舍五入，避免连续截断。
 *
 *  溢出核算（u32 上限 4.29e9，全程安全）：
 *    raw × VREF        ≤ 4095 × 5000   = 20,475,000
 *    mv_adc × (R1+R2)  ≤ 5000 × 166000 = 830,000,000
 *
 *  对应关系（Vref=5V）：
 *     6.5V → raw 1058     9.0V → raw 1465
 *    13.5V → raw 2198    18.0V → raw 2930    满量程 4095 → 25.2V
 *-------------------------------------------------------------------------*/
u16 HAL_ADC_ReadVbatMv(void)
{
    uint32_t raw;
    uint32_t mv_adc;
    uint32_t vbat;

    if (!s_ready) { return 0u; }

    raw = (uint32_t)s_result[ADC_CH_POWER_12V];
    if (raw > ADC_FULLSCALE) { raw = ADC_FULLSCALE; }

    mv_adc = (raw * (uint32_t)CFG_ADC_VREF_MV + (ADC_FULLSCALE / 2u)) / ADC_FULLSCALE;
    vbat   = (mv_adc * (uint32_t)VBAT_DIV_NUM + (VBAT_DIV_DEN / 2u)) / (uint32_t)VBAT_DIV_DEN;

    return (vbat > 0xFFFFu) ? 0xFFFFu : (u16)vbat;
}

bool HAL_ADC_IsReady(void) { return s_ready; }