/**
 * @file    cfg_can.h
 * @brief   CAN 总线参数与报文 ID 配置
 *
 * 来源：长安跨越星V5(100V1) 仪表硬件规格书 Section 1.2 + 整车 CAN 矩阵
 * 修改影响范围：CAN 服务层（service/can/）、HAL CAN 实现
 *
 * 命名约定：
 *   - CFG_CAN_xxx     : 总线物理参数与超时阈值
 *   - CAN_ID_xxx_yyy  : CAN 报文 ID（yyy = 16 进制 ID 的十六进制字面量）
 */

#ifndef CLUSTER_CFG_CAN_H
#define CLUSTER_CFG_CAN_H

/*===========================================================================
 *  CAN 物理层参数
 *===========================================================================*/
#define CFG_CAN_BAUD_RATE               500000u /**< 波特率：500 kbps        */
#define CFG_CAN_TERM_RESISTANCE         120u    /**< 终端电阻 120 Ω          */
#define CFG_CAN_FRAME_FORMAT            0u      /**< 0=标准帧, 1=扩展帧       */
#define CFG_CAN_BYTE_ORDER              0u      /**< 0=Intel(LE), 1=Motorola */

/*===========================================================================
 *  CAN 报文超时阈值
 *
 *  CFG_CAN_TIMEOUT_MULTIPLIER 是允许丢失的报文倍数。
 *  即：超时阈值 = 报文周期 × MULTIPLIER。
 *  例：周期 100ms 的报文，连续 10 个周期未收到（共 1000ms）则判定超时。
 *===========================================================================*/
#define CFG_CAN_TIMEOUT_MULTIPLIER      10u     /**< 允许连续丢失的周期数     */

/**
 * CAN 上线宽限期（ms）：自检结束后再等这么久。
 *
 * 用于满足"自检结束后系统正常则指示灯熄灭、有故障才点亮"的要求：
 *   - 报文已收到过 → 用各自的超时阈值正常判定；
 *   - 报文从未收到过 → 在 自检时长 + 本宽限期 之内不判超时（避免自检刚结束
 *     整车其它 ECU 还没上电就误报一片故障灯）；超过之后仍未上线才判超时
 *     （避免整条 CAN 断线时反而什么都不报）。
 */
#define CFG_CAN_STARTUP_GRACE_MS        1000u
#define CFG_CAN_TIMEOUT_20MS            (20u  * CFG_CAN_TIMEOUT_MULTIPLIER)  /**< 20ms 周期报文超时阈值  */
#define CFG_CAN_TIMEOUT_100MS           (100u * CFG_CAN_TIMEOUT_MULTIPLIER)  /**< 100ms 周期报文超时阈值 */
#define CFG_CAN_TIMEOUT_200MS           (200u * CFG_CAN_TIMEOUT_MULTIPLIER)  /**< 200ms 周期报文超时阈值 */

/*===========================================================================
 *  CAN 报文 ID 定义
 *
 *  接收方向：仪表订阅整车其它 ECU 发送的报文
 *  发送方向：仪表向整车发布自身状态（车速、里程、燃油量、转向等）
 *===========================================================================*/

/* --- 接收报文 --- */
#define CAN_ID_EMS_306          0x306u  /**< EMS 发动机状态 1（水温/故障，100ms） */

#define CAN_ID_ABS_330          0x330u  /**< ABS 车速与故障状态（20ms）           */
#define CAN_ID_EMS_303          0x303u  /**< GPF 颗粒捕集器再生状态（10ms）       */
#define CAN_ID_GAS_305          0x305u  /**< CNG 燃气控制器状态（200ms）          */
#define CAN_ID_TPMS_390         0x390u  /**< TPMS 胎压监测（200ms）               */
#define CAN_ID_EMS_360          0x360u  /**< 仪表接收：EMS 发动机状态 2（MIL，100ms）   */
#define CAN_ID_ECU_302          0x302u  /**< 仪表接收：发动机转速 / 转速故障（10ms）    */

/* --- 发送报文 --- */
#define CAN_ID_TX_380           0x380u  /**< 仪表外发：燃油百分比/里程/安全带      */
#define CAN_ID_TX_381           0x381u  /**< 仪表外发：扩展状态/外温/转向          */

#endif /* CLUSTER_CFG_CAN_H */
