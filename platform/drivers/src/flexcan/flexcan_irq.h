/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file flexcan_irq.h
 * @version 1.4.0
 */

#ifndef FLEXCAN_IRQ_H
#define FLEXCAN_IRQ_H

#include "device_registers.h"

/*!
 * @brief Interrupt handler for a FlexCAN instance.
 *
 * @param   instance        The FlexCAN instance number.
 * @param   mb_idx_start    The FlexCAN Start MB index.
 * @param   mb_idx_end      The FlexCAN End MB index.
 */
void FLEXCAN_IRQHandler(uint8_t instance, uint8_t mb_idx_start, uint8_t mb_idx_end);

/*!
 * @brief Error interrupt handler for a FlexCAN instance.
 *
 * @param   instance    The FlexCAN instance number.
 */
void FLEXCAN_Error_IRQHandler(uint8_t instance);

/*!
 * @brief Error interrupt handler for a FlexCAN instance.
 *
 * @param   instance    The FlexCAN instance number.
 */
void FLEXCAN_ORed_IRQHandler(uint8_t instance);

#if FEATURE_CAN_HAS_WAKE_UP_IRQ

/*!
 * @brief Wake up handler for a FlexCAN instance.
 *
 * @param   instance    The FlexCAN instance number.
 */
void FLEXCAN_WakeUpHandler(uint8_t instance);

#endif /* FEATURE_CAN_HAS_WAKE_UP_IRQ */

#if defined (YTM32B1L_SERIES)

/*******************************************************************************
 * Default interrupt handlers signatures
 ******************************************************************************/
void CAN0_ORed_Err_Wakeup_IRQHandler(void);
void CAN0_ORed_0_15_MB_IRQHandler(void);
void CAN0_ORed_16_31_MB_IRQHandler(void);

#elif (defined (YTM32B1M_SERIES) || defined (YTM32B1H_SERIES))
/*******************************************************************************
 * Default interrupt handlers signatures
 ******************************************************************************/
void CAN0_ORed_IRQHandler(void);                                        /* CAN0_ORed Handler*/
void CAN0_Error_IRQHandler(void);                                       /* CAN0_Error Handler*/
void CAN0_Wake_Up_IRQHandler(void);                                     /* CAN0_Wake_Up Handler*/
void CAN0_ORed_0_15_MB_IRQHandler(void);                                /* CAN0_ORed_0_15_MB Handler*/
void CAN0_ORed_16_31_MB_IRQHandler(void);                               /* CAN0_ORed_16_31_MB Handler*/
#if defined (FEATURE_CAN0_MAX_MB_NUM)&&(FEATURE_CAN0_MAX_MB_NUM >= 32U)
void CAN0_ORed_32_47_MB_IRQHandler(void);                               /* CAN0_ORed_32_47_MB Handler*/
void CAN0_ORed_48_63_MB_IRQHandler(void);                               /* CAN0_ORed_48_63_MB Handler*/
#endif
#if defined (FEATURE_CAN0_MAX_MB_NUM)&&(FEATURE_CAN0_MAX_MB_NUM >= 64U)
void CAN0_ORed_64_79_MB_IRQHandler(void);                               /* CAN0_ORed_64_79_MB Handler*/
void CAN0_ORed_80_95_MB_IRQHandler(void);                               /* CAN0_ORed_80_95_MB Handler*/
void CAN0_ORed_96_111_MB_IRQHandler(void);                              /* CAN0_ORed_96_111_MB Handler*/
void CAN0_ORed_112_127_MB_IRQHandler(void);                             /* CAN0_ORed_112_127_MB Handler*/
#endif 
#if defined (CAN_INSTANCE_COUNT)&&(CAN_INSTANCE_COUNT >= 1U)
void CAN1_ORed_IRQHandler(void);                                        /* CAN1_ORed Handler*/
void CAN1_Error_IRQHandler(void);                                       /* CAN1_Error Handler*/
void CAN1_Wake_Up_IRQHandler(void);                                     /* CAN1_Wake_Up Handler*/
void CAN1_ORed_0_15_MB_IRQHandler(void);                                /* CAN1_ORed_0_15_MB Handler*/
void CAN1_ORed_16_31_MB_IRQHandler(void);                               /* CAN1_ORed_16_31_MB Handler*/
#if defined (FEATURE_CAN1_MAX_MB_NUM)&&(FEATURE_CAN1_MAX_MB_NUM >= 32U)
void CAN1_ORed_32_47_MB_IRQHandler(void);                               /* CAN1_ORed_32_47_MB Handler*/
void CAN1_ORed_48_63_MB_IRQHandler(void);                               /* CAN1_ORed_48_63_MB Handler*/
#endif
#if defined (FEATURE_CAN1_MAX_MB_NUM)&&(FEATURE_CAN1_MAX_MB_NUM >= 64U)
void CAN1_ORed_64_79_MB_IRQHandler(void);                               /* CAN1_ORed_64_79_MB Handler*/
void CAN1_ORed_80_95_MB_IRQHandler(void);                               /* CAN1_ORed_80_95_MB Handler*/
void CAN1_ORed_96_111_MB_IRQHandler(void);                              /* CAN1_ORed_96_111_MB Handler*/
void CAN1_ORed_112_127_MB_IRQHandler(void);                             /* CAN1_ORed_112_127_MB Handler*/
#endif
#endif
#if defined (CAN_INSTANCE_COUNT)&&(CAN_INSTANCE_COUNT >= 2U)  
void CAN2_ORed_IRQHandler(void);                                        /* CAN2_ORed Handler*/
void CAN2_Error_IRQHandler(void);                                       /* CAN2_Error Handler*/
void CAN2_Wake_Up_IRQHandler(void);                                     /* CAN2_Wake_Up Handler*/
void CAN2_ORed_0_15_MB_IRQHandler(void);                                /* CAN2_ORed_0_15_MB Handler*/
void CAN2_ORed_16_31_MB_IRQHandler(void);                               /* CAN2_ORed_16_31_MB Handler*/
#if defined (FEATURE_CAN2_MAX_MB_NUM)&&(FEATURE_CAN2_MAX_MB_NUM >= 32U)
void CAN2_ORed_32_47_MB_IRQHandler(void);                               /* CAN2_ORed_32_47_MB Handler*/
void CAN2_ORed_48_63_MB_IRQHandler(void);                               /* CAN2_ORed_48_63_MB Handler*/
#endif
#endif
#if defined (CAN_INSTANCE_COUNT)&&(CAN_INSTANCE_COUNT >= 3U)  
void CAN3_ORed_IRQHandler(void);                                        /* CAN3_ORed Handler*/
void CAN3_Error_IRQHandler(void);                                       /* CAN3_Error Handler*/
void CAN3_Wake_Up_IRQHandler(void);                                     /* CAN3_Wake_Up Handler*/
void CAN3_ORed_0_15_MB_IRQHandler(void);                                /* CAN3_ORed_0_15_MB Handler*/
void CAN3_ORed_16_31_MB_IRQHandler(void);                               /* CAN3_ORed_16_31_MB Handler*/
#if defined (FEATURE_CAN3_MAX_MB_NUM)&&(FEATURE_CAN3_MAX_MB_NUM >= 32U)
void CAN3_ORed_32_47_MB_IRQHandler(void);                               /* CAN3_ORed_32_47_MB Handler*/
void CAN3_ORed_48_63_MB_IRQHandler(void);                               /* CAN3_ORed_48_63_MB Handler*/
#endif
#endif
#if defined (CAN_INSTANCE_COUNT)&&(CAN_INSTANCE_COUNT >= 4U) 
void CAN4_ORed_IRQHandler(void);                                        /* CAN4_ORed Handler*/
void CAN4_Error_IRQHandler(void);                                       /* CAN4_Error Handler*/
void CAN4_Wake_Up_IRQHandler(void);                                     /* CAN4_Wake_Up Handler*/
void CAN4_ORed_0_15_MB_IRQHandler(void);                                /* CAN4_ORed_0_15_MB Handler*/
void CAN4_ORed_16_31_MB_IRQHandler(void);                               /* CAN4_ORed_16_31_MB Handler*/
#if defined (FEATURE_CAN4_MAX_MB_NUM)&&(FEATURE_CAN4_MAX_MB_NUM >= 32U)
void CAN4_ORed_32_47_MB_IRQHandler(void);                               /* CAN4_ORed_32_47_MB Handler*/
void CAN4_ORed_48_63_MB_IRQHandler(void);                               /* CAN4_ORed_48_63_MB Handler*/
#endif
#endif
#if defined (CAN_INSTANCE_COUNT)&&(CAN_INSTANCE_COUNT >= 5U) 
void CAN5_ORed_IRQHandler(void);                                        /* CAN5_ORed Handler*/
void CAN5_Error_IRQHandler(void);                                       /* CAN5_Error Handler*/
void CAN5_Wake_Up_IRQHandler(void);                                     /* CAN5_Wake_Up Handler*/
void CAN5_ORed_0_15_MB_IRQHandler(void);                                /* CAN5_ORed_0_15_MB Handler*/
void CAN5_ORed_16_31_MB_IRQHandler(void);                               /* CAN5_ORed_16_31_MB Handler*/
#endif
#if defined (CAN_INSTANCE_COUNT)&&(CAN_INSTANCE_COUNT >= 6U) 
void CAN6_ORed_IRQHandler(void);                                        /* CAN6_ORed Handler*/
void CAN6_Error_IRQHandler(void);                                       /* CAN6_Error Handler*/
void CAN6_Wake_Up_IRQHandler(void);                                     /* CAN6_Wake_Up Handler*/
void CAN6_ORed_0_15_MB_IRQHandler(void);                                /* CAN6_ORed_0_15_MB Handler*/
void CAN6_ORed_16_31_MB_IRQHandler(void);                               /* CAN6_ORed_16_31_MB Handler*/
#endif
#if defined (CAN_INSTANCE_COUNT)&&(CAN_INSTANCE_COUNT >= 7U) 
void CAN7_ORed_IRQHandler(void);                                        /* CAN7_ORed Handler*/
void CAN7_Error_IRQHandler(void);                                       /* CAN7_Error Handler*/
void CAN7_Wake_Up_IRQHandler(void);                                     /* CAN7_Wake_Up Handler*/
void CAN7_ORed_0_15_MB_IRQHandler(void);                                /* CAN7_ORed_0_15_MB Handler*/
void CAN7_ORed_16_31_MB_IRQHandler(void);                               /* CAN7_ORed_16_31_MB Handler*/
#endif
#else
    #error "No valid CPU defined!"
#endif

#endif /* FLEXCAN_IRQ_H */

/*******************************************************************************
 * EOF
 ******************************************************************************/
