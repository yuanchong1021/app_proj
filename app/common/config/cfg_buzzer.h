/**
 * @file    cfg_buzzer.h
 * @brief   蜂鸣器报警参数配置 (Section 7)
 *
 * 修改影响范围：app/ui/buzzer.c
 *
 * 报警时序示意：
 *   安全带：1Hz × 6 声 → 静音 9s → 循环 5 次后停止
 *   燃气泄漏：2Hz × 12 声后停止（一次性）
 */

#ifndef CLUSTER_CFG_BUZZER_H
#define CLUSTER_CFG_BUZZER_H

/*===========================================================================
 *  安全带未系报警参数
 *===========================================================================*/
#define CFG_BUZZER_SEATBELT_FREQ_HZ     1u      /**< 鸣叫频率（Hz）             */
#define CFG_BUZZER_SEATBELT_BEEPS       6u      /**< 单轮鸣叫次数               */
#define CFG_BUZZER_SEATBELT_PAUSE_MS    9000u   /**< 两轮间静音时长（ms）       */
#define CFG_BUZZER_SEATBELT_CYCLES      5u      /**< 总循环轮数                  */

/** IGN 下电后关闭安全带报警音的去抖时间（ms）
 *
 *  ★本次新增★ 需求：IGN 下电，安全带报警音关闭。
 *
 *  hw_input.ign_on 是裸读 GPIO（无去抖），钥匙触点抖动和起机（START）拖动
 *  时 IGN 线的瞬时跌落都会被 1ms 采样完整看到。IGN 电平必须连续保持这么久
 *  才被 buzzer.c 采纳，避免一次抖动就把正在进行的安全带报警打断、
 *  并把 seatbelt_latched 闭锁解除（解除后条件再满足会重新响 5 轮）。
 *
 *  与 cfg_selfcheck.h::CFG_SELFCHECK_IGN_DEBOUNCE_MS 是两套独立的去抖，
 *  互不影响：那个决定"什么时候重新自检"，这个只决定"什么时候闭嘴"。 */
#define CFG_BUZZER_IGN_DEBOUNCE_MS      20u

/*===========================================================================
 *  燃气泄漏报警参数
 *===========================================================================*/
#define CFG_BUZZER_GAS_FREQ_HZ          2u      /**< 鸣叫频率（Hz）             */
#define CFG_BUZZER_GAS_BEEPS            12u     /**< 鸣叫次数（一次性，无循环） */

#endif /* CLUSTER_CFG_BUZZER_H */