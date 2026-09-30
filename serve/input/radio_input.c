/**
 * @file    radio_input.c
 * @brief   SOC → MCU 下行数据接收解析服务（UART2 DMA 接收 + 空闲中断分帧）
 *
 * 接收方案（基于云途 UART_DRV 的 DMA 收发，已核对驱动源码）：
 *   - uart_config2 配置为 UART_USING_DMA、rxDMAChannel=0；dma_config0 已把
 *     DMA_REQ_UART2_RX 绑定到 DMA 通道0。调用 UART_DRV_ReceiveData() 时，驱动
 *     内部执行：
 *         DMA_DRV_ConfigMultiBlockTransfer(通道0, 外设→内存,
 *                                          &UART2->DATA, 缓冲区, 1B, N次)
 *       + DMA_DRV_StartChannel(通道0)
 *     即“配源/目的地址 + 启动通道”这一整套由驱动完成，应用层无需手写 DMA_DRV_*。
 *
 *   - 一次性挂起 DMA_RX_BUF_SIZE 字节的 DMA 接收。SOC 发完一帧后线路空闲，
 *     UART 触发空闲中断。本 SDK 没有独立的空闲事件——驱动在 ISR 中以
 *     UART_EVENT_ERROR 上报，并把 receiveStatus 置为 STATUS_UART_IDLE_ERROR。
 *     回调据此判定“帧结束”，用 UART_DRV_GetReceiveStatus() 取得 DMA 剩余量，
 *     算出本帧已收字节数，整帧一次性推入环形缓冲区，再重挂 DMA。
 *
 *   - 相比原“每字节一次 DMA 完成中断”，现在一帧只中断一次，CPU 中断开销大降。
 *
 * 前提：uart_config2.idleErrorIntEnable = true（已在配置工具中使能）。
 *       DMA 接收启动时，UART_DRV_SetErrorInterrupts() 会据此自动打开空闲中断。
 *
 * 帧格式（协议 V1.1 §2.1）：
 *   [0xAA][0x55][CMD][DATA_LEN/DATA][...][CHECKSUM]
 */

#include "radio_input.h"
#include "rtc_input.h"
#include "uart_driver.h"
#include "odo.h"          /* 【新增】ODO_RequestClearTotal()：CMD=0x09 总里程清零 */

/* =====================================================================
 *  编译常量
 * ===================================================================== */
#define SOC_UART_INST        2u
#define SOC_MAX_DATA_LEN     8u

/* 环形缓冲区：大小必须为2的幂 */
#define RING_BUF_SIZE        64u
#define RING_MASK            (RING_BUF_SIZE - 1u)

/* DMA 接收缓冲区大小（字节）
 * 一次挂起的 DMA 接收长度。协议最长帧 13 字节，取 64 留足余量，
 * 使连续多帧也能在一次 DMA 里缓冲（正常情况下每帧靠空闲中断即时取走）。 */
#define DMA_RX_BUF_SIZE      64u

/* 帧头 */
#define SOC_HEAD1            0xAAu
#define SOC_HEAD2            0x55u

/* 命令字 */
#define CMD_TRIP_SWITCH       0x02u
#define CMD_TRIP_RESET        0x03u
#define CMD_BT_STATUS         0x04u
#define CMD_TIME_SYNC         0x05u
#define CMD_RADIO_STATUS      0x06u
#define CMD_AUDIO_SOURCE_TYPE 0x07u
#define CMD_VOLUME            0x08u   /* 音量值（短帧，DATA=0~99，0xFF=熄灭） */
#define CMD_ODO_RESET         0x09u   /* 总里程(ODO)清零（短帧，DATA 必须=0x5A） */

/* ---------------------------------------------------------------------
 *  CMD=0x09 总里程清零 —— 安全策略
 *
 *  SOC 侧触发条件：上一曲+下一曲+功能切换 三键同时长按约 3 秒；
 *  一次触发背靠背连发两帧完全相同的命令：
 *      AA 55 09 5A 53  AA 55 09 5A 53      （10 字节，115200bps 下约 868us）
 *  SOC 内部有自锁，同一次按住只发一组；下一次最快也要
 *  「松手 + 重按 + 3 秒」，即两次真实清零间隔 >= 3 秒。
 *
 *  总里程不可逆，这里设三道闸：
 *    闸1  异或校验正确 —— 由 feed_byte() 的 ST_CHECKSUM 保证，
 *                        校验错的帧根本不会进 process_frame()，不计入确认
 *    闸2  DATA == 0x5A 魔术字 —— 本协议只有 8 位异或校验，随机误码约有
 *                        1/256 概率蒙对；用非平凡固定值再压低两个数量级
 *    闸3  ODO_CONFIRM_MS 窗口内累计 ODO_CONFIRM_FRAMES 帧才执行
 *
 *  两帧确认与本文件对 CMD=0x03 的既有约定一致：抗误码最强，要连错两次
 *  才会误清。代价是丢一帧则不清零 —— 用户松手重按即可，比误清总里程
 *  好得多。若改为收到一帧即执行，把 ODO_CONFIRM_FRAMES 改成 1 即可，
 *  第二帧会被去重窗口自动吞掉，不会清两次。
 * --------------------------------------------------------------------- */
#define ODO_RESET_MAGIC       0x5Au  /* CMD=0x09 的 DATA 魔术字，必须校验 */
#define ODO_CONFIRM_FRAMES    2u     /* 需要几帧一致才执行（1 或 2）      */
#define ODO_CONFIRM_MS        200u   /* 收齐这几帧的时间窗口(ms)          */
#define ODO_DEDUP_MS          1000u  /* 执行后多久内忽略重复帧(ms)        */

/* =====================================================================
 *  环形缓冲区
 * ===================================================================== */
typedef struct {
    uint8_t           buf[RING_BUF_SIZE];
    volatile uint16_t head;   /* 写指针（中断上下文写） */
    uint16_t          tail;   /* 读指针（Task 上下文读） */
} RingBuf_t;

static RingBuf_t s_ring;

/* DMA 接收缓冲：驱动把 UART2->DATA 的字节 DMA 搬运到此。
 * 必须是静态存储（生命周期覆盖整个 DMA 传输），不能是局部变量。 */
static uint8_t   s_dma_rx_buf[DMA_RX_BUF_SIZE];

/* 入队：仅在中断上下文调用，缓冲区满时静默丢弃 */
static inline void ring_push(uint8_t b)
{
    uint16_t next = (uint16_t)((s_ring.head + 1u) & RING_MASK);
    if (next != s_ring.tail) {
        s_ring.buf[s_ring.head] = b;
        s_ring.head = next;
    }
    /* 满时丢弃：可在此处增加溢出计数器用于诊断 */
}

/* 出队：仅在 Task 上下文调用 */
static inline bool ring_pop(uint8_t *out)
{
    if (s_ring.head == s_ring.tail) {
        return false;
    }
    *out = s_ring.buf[s_ring.tail];
    s_ring.tail = (uint16_t)((s_ring.tail + 1u) & RING_MASK);
    return true;
}

/* =====================================================================
 *  帧解析状态机
 * ===================================================================== */
typedef enum {
    ST_IDLE = 0,
    ST_HEAD2,
    ST_CMD,
    ST_LEN,
    ST_DATA,
    ST_DATA_N,
    ST_CHECKSUM
} ParseState_t;

/* 【新增】CMD=0x09 总里程清零的确认/去重状态 */
static uint8_t      s_odo_cnt        = 0u;    /* 当前窗口内已收到的合法 0x09 帧数 */
static uint32_t     s_odo_first_ms   = 0u;    /* 当前确认窗口的起点             */
static uint32_t     s_odo_done_ms    = 0u;    /* 上次执行清零的时刻             */
static bool         s_odo_done_valid = false; /* s_odo_done_ms 是否有效         */

static ParseState_t s_state     = ST_IDLE;
static uint8_t      s_cmd       = 0u;
static uint8_t      s_len       = 0u;
static uint8_t      s_data_cnt  = 0u;
static uint8_t      s_data[SOC_MAX_DATA_LEN];
static uint8_t      s_xor_acc   = 0u;

/* =====================================================================
 *  判断是否为长帧（含 LEN 字段）
 * ===================================================================== */
static inline bool is_long_frame(uint8_t cmd)
{
    return (cmd == CMD_TIME_SYNC      /* 0x05: len=2, data=hh mm     */
         || cmd == CMD_RADIO_STATUS); /* 0x06: len=3, data=type fH fL */
}

/* =====================================================================
 *  DMA 接收缓冲 → 环形缓冲区
 *  把 s_dma_rx_buf[0, count) 推入 ring（仅中断上下文调用）
 * ===================================================================== */
static inline void rx_flush(uint16_t count)
{
    uint16_t i;
    for (i = 0u; i < count; i++) {
        ring_push(s_dma_rx_buf[i]);
    }
}

/* 重新挂起一次 DMA 接收（缓冲区复位到起点，长度 DMA_RX_BUF_SIZE） */
static inline void rx_arm(void)
{
    (void)UART_DRV_ReceiveData(SOC_UART_INST, s_dma_rx_buf, DMA_RX_BUF_SIZE);
}

/* =====================================================================
 *  UART2 RX 回调（DMA 模式，中断上下文）
 *
 *  事件来源（已核对 uart_driver.c）：
 *    UART_EVENT_ERROR        —— 线路空闲（帧尾）或硬件错误都走这里。
 *                               GetReceiveStatus 的返回状态区分二者：
 *                                 STATUS_UART_IDLE_ERROR → 空闲=帧结束（正常）
 *                                 其它错误码            → 帧错误/溢出/噪声等
 *    UART_EVENT_RX_FULL      —— DMA 缓冲收满 DMA_RX_BUF_SIZE 字节（连续流场景）
 *    UART_EVENT_END_TRANSFER —— 接收停止（未提供新缓冲时）
 * ===================================================================== */
static void uart2_rx_callback(void *driverState, uart_event_t event, void *userData)
{
    (void)driverState;
    (void)userData;

    uint32_t remaining = 0u;
    status_t st;
    uint16_t filled;

    switch (event) {

    case UART_EVENT_ERROR:
        st = UART_DRV_GetReceiveStatus(SOC_UART_INST, &remaining);
        if (st == STATUS_UART_IDLE_ERROR) {
            /* 线路空闲 = 一帧结束。此时 DMA 仍处于挂起态，remaining 为
             * DMA 尚未搬运的字节数，本帧已收 = 缓冲长度 - remaining。 */
            filled = (uint16_t)(DMA_RX_BUF_SIZE - (uint16_t)remaining);
            rx_flush(filled);
        }
        /* else：真正的硬件错误（帧错误/溢出/噪声）——丢弃本段残缺数据，
         *       状态机会在下一个 0xAA 处重新同步。 */

        /* 中止当前传输并重挂，使 DMA 目的指针复位到缓冲起点 */
        (void)UART_DRV_AbortReceivingData(SOC_UART_INST);
        rx_arm();
        break;

    case UART_EVENT_RX_FULL:
        /* 缓冲被填满（SOC 连续发送、期间无空闲间隙）：整块取走，
         * 并把同一缓冲交还驱动以无间隙续传。必须在本回调内先取走数据，
         * 因为回调返回后驱动会立即从缓冲起点覆写。 */
        rx_flush(DMA_RX_BUF_SIZE);
        (void)UART_DRV_SetRxBuffer(SOC_UART_INST, s_dma_rx_buf, DMA_RX_BUF_SIZE);
        break;

    case UART_EVENT_END_TRANSFER:
        /* 接收已停止且未续传：重新挂起接收 */
        rx_arm();
        break;

    default:
        break;
    }
}

/* =====================================================================
 *  复位状态机
 * ===================================================================== */
static void parser_reset(void)
{
    s_state = ST_IDLE;
}

/* =====================================================================
 *  帧处理（解析完整帧后写入 sys）
 * ===================================================================== */
static void process_frame(ClusterSystem_t *sys)
{
    switch (s_cmd) {
        
    case CMD_TRIP_SWITCH:
        if (s_data[0] == 0x01u || s_data[0] == 0x02u) 
        {
            if (sys->lcd.odometer.odo_or_trip == true)
            {
                 sys->lcd.odometer.show_trip = true;
            }
            else
            {
                sys->lcd.odometer.show_trip = false;
            }
        }
        break;

    case CMD_TRIP_RESET:
        if (s_data[0] == 0x01u || s_data[0] == 0x02u) {
            sys->lcd.odometer.trip_dm = 0u;
        }
        break;

    case CMD_BT_STATUS:
        sys->lcd.radio.band         = RADIO_BAND_BT;
        sys->lcd.radio.bt_connected = (s_data[0] == 0x01u);
        sys->lcd.radio.freq_raw     = 0u;
        sys->lcd.radio.valid        = true;
        break;

    case CMD_TIME_SYNC:
        if (s_len == 2u) {
            uint8_t hh = s_data[0];
            uint8_t mm = s_data[1];
            if (hh <= 23u && mm <= 59u) {
                (void)RTC_SetTime(hh, mm);
                sys->lcd.clock.hour   = hh;
                sys->lcd.clock.minute = mm;
                sys->lcd.clock.valid  = true;
            }
        }
        break;

    case CMD_RADIO_STATUS: {
        if (s_len == 3u) {
            uint8_t  type = s_data[0];
            uint16_t freq = (uint16_t)(((uint16_t)s_data[1] << 8u) | s_data[2]);
            if (type == 0x00u) {
                sys->lcd.radio.band         = RADIO_BAND_FM;
                sys->lcd.radio.freq_raw     = freq;
                sys->lcd.radio.bt_connected = false;
                sys->lcd.radio.valid        = true;
            } else if (type == 0x01u) {
                sys->lcd.radio.band         = RADIO_BAND_AM;
                sys->lcd.radio.freq_raw     = freq;
                sys->lcd.radio.bt_connected = false;
                sys->lcd.radio.valid        = true;
            }
        }
        break;
    }

    /* ---------------------------------------------------------------
     * CMD=0x08 音量值（短帧）
     *   DATA = 0x00~0x63(0~99) → 显示到 Digit21(十位) / Digit22(个位)
     *   DATA = 0xFF            → 熄灭音量显示（可选，SOC 不用可忽略）
     *   其它值（0x64~0xFE）    → 钳位到 99，仍显示
     * SOC 只需在「初始化」和「音量变化」两个时机各发一帧，MCU 自行保持。
     * --------------------------------------------------------------- */
    case CMD_VOLUME: {
        uint8_t vol = s_data[0];
        if (vol == 0xFFu) {
            sys->lcd.volume.valid = false;          /* 请求熄灭音量区 */
        } else {
            if (vol > 99u) { vol = 99u; }           /* 面板仅 2 位，钳位 */
            sys->lcd.volume.level = vol;
            sys->lcd.volume.valid = true;
        }
        break;
    }

    /* ---------------------------------------------------------------
     * CMD=0x09 总里程(ODO)清零（短帧）
     *   帧：AA 55 09 5A 53，一次触发连发两帧
     *   DATA 必须 == 0x5A，且窗口内累计满 ODO_CONFIRM_FRAMES 帧才执行
     *
     * 这里只置"清零请求"，真正的清零 + 落盘放在 ODO_Task 里做。原因：
     * NVM_SaveOdometer() 底层 EFM 擦写会关中断并轮询等待数毫秒，
     * 不适合放在 10ms 周期的 UART 解析路径上。ODO_Task 周期 200ms，
     * 用户完全感知不到这点延迟。
     * --------------------------------------------------------------- */
    case CMD_ODO_RESET: {
        uint32_t now = sys->system_tick_ms;

        /* 闸2：魔术字。不匹配则静默丢弃，且不计入确认帧数 */
        if (s_data[0] != ODO_RESET_MAGIC) {
            break;
        }

        /* 去重：刚清过就忽略。SOC 侧两次真实清零间隔 >= 3s，
         * 1s 窗口既能吞掉同一次触发的重复帧，又不会误吞下一次操作。 */
        if (s_odo_done_valid && ((uint32_t)(now - s_odo_done_ms) < ODO_DEDUP_MS)) {
            break;
        }

        /* 闸3：确认窗口累计。窗口超时则重新开窗，避免
         * 两次相隔很久的"单帧误码"被凑成一次有效确认。 */
        if ((s_odo_cnt == 0u) ||
            ((uint32_t)(now - s_odo_first_ms) > ODO_CONFIRM_MS)) {
            s_odo_cnt      = 1u;
            s_odo_first_ms = now;
        } else {
            s_odo_cnt++;
        }

        if (s_odo_cnt >= ODO_CONFIRM_FRAMES) {
            s_odo_cnt        = 0u;
            s_odo_done_ms    = now;
            s_odo_done_valid = true;
            ODO_RequestClearTotal();     /* ★ 请求清零，由 ODO_Task 执行 */
        }
        break;
    }

    case CMD_AUDIO_SOURCE_TYPE:
        switch (s_data[0]) {
        case 0u:
            sys->lcd.radio.band = RADIO_BAND_BT;
            break;
        case 1u:
            sys->lcd.radio.band = RADIO_BAND_FM;
            break;
        case 2u:
            sys->lcd.radio.band = RADIO_BAND_AM;
            break;
        default:
            break;
        }
        sys->lcd.radio.valid = true;
        break;

    default:
        break;
    }
}

/* =====================================================================
 *  状态机逐字节驱动
 * ===================================================================== */
static bool feed_byte(uint8_t b, ClusterSystem_t *sys)
{
    bool frame_done = false;

    switch (s_state) {
    case ST_IDLE:
        if (b == SOC_HEAD1) {
            s_state = ST_HEAD2;
        }
        break;

    case ST_HEAD2:
        if (b == SOC_HEAD2) {
            s_state = ST_CMD;
        } else {
            /* 连续两个 HEAD1 时保持 ST_HEAD2，避免 AA AA 55 ... 漏帧 */
            s_state = (b == SOC_HEAD1) ? ST_HEAD2 : ST_IDLE;
        }
        break;

    case ST_CMD:
        s_cmd      = b;
        s_xor_acc  = b;          /* XOR 累积从 CMD 字节开始 */
        s_data_cnt = 0u;
        s_state    = is_long_frame(s_cmd) ? ST_LEN : ST_DATA;
        break;

    case ST_LEN:
        s_len     = b;
        s_xor_acc = (uint8_t)(s_xor_acc ^ b);
        if (s_len == 0u || s_len > SOC_MAX_DATA_LEN) {
            parser_reset();
        } else {
            s_state = ST_DATA_N;
        }
        break;

    case ST_DATA:
        /* 短帧：固定1字节数据 */
        s_data[0]  = b;
        s_xor_acc  = (uint8_t)(s_xor_acc ^ b);
        s_data_cnt = 1u;
        s_state    = ST_CHECKSUM;
        break;

    case ST_DATA_N:
        /* 长帧：收满 s_len 字节 */
        s_data[s_data_cnt] = b;
        s_xor_acc          = (uint8_t)(s_xor_acc ^ b);
        s_data_cnt++;
        if (s_data_cnt >= s_len) {
            s_state = ST_CHECKSUM;
        }
        break;

    case ST_CHECKSUM:
        if (s_xor_acc == b) {
            process_frame(sys);
            frame_done = true;
        }
        parser_reset();
        break;

    default:
        parser_reset();
        break;
    }

    return frame_done;
}

/* =====================================================================
 *  公开 API
 * ===================================================================== */

void RADIO_Init(ClusterSystem_t *sys)
{
    /* 初始化环形缓冲区 */
    s_ring.head = 0u;
    s_ring.tail = 0u;

    /* 初始化状态机 */
    parser_reset();

    /* 【新增】复位 CMD=0x09 总里程清零的确认/去重状态 */
    s_odo_cnt        = 0u;
    s_odo_first_ms   = 0u;
    s_odo_done_ms    = 0u;
    s_odo_done_valid = false;

    /* 初始化 LCD 字段 */
    sys->lcd.radio.band         = RADIO_BAND_NONE;
    sys->lcd.radio.freq_raw     = 0u;
    sys->lcd.radio.valid        = false;
    sys->lcd.radio.bt_connected = false;

    /* 音量显示：上电默认熄灭（Digit21/22 空白），
     * 等待 SOC 初始化时下发的第一帧 CMD=0x08 后才点亮。 */
    sys->lcd.volume.level = 0u;
    sys->lcd.volume.valid = false;

    /* 安装回调并挂起首次 DMA 接收。
     * UART_DRV_ReceiveData 内部完成 DMA 通道0 的“配地址 + 启动通道”，
     * 并（因 idleErrorIntEnable=true）使能空闲中断用于帧尾检测。 */
    (void)UART_DRV_InstallRxCallback(SOC_UART_INST, uart2_rx_callback, NULL);
    rx_arm();
}

void RADIO_Task(ClusterSystem_t *sys)
{
    uint8_t b;
    /* 消费环形缓冲区中所有字节，交给状态机解析（逻辑不变） */
    while (ring_pop(&b)) {
        (void)feed_byte(b, sys);
    }

    /* 【新增】CMD=0x09 确认窗口超时收尾：
     * 窗口内没凑够 ODO_CONFIRM_FRAMES 帧（例如丢了一帧，或某帧是误码
     * 恰好通过了校验），本次计数作废，不允许跨窗口累积。 */
    if ((s_odo_cnt > 0u) &&
        ((uint32_t)(sys->system_tick_ms - s_odo_first_ms) > ODO_CONFIRM_MS)) {
        s_odo_cnt = 0u;
    }
}