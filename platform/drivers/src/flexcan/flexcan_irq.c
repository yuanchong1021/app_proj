/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file flexcan_irq.c
 * @version 1.4.0
 */

#include "flexcan_irq.h"
#if (FEATURE_CAN_HAS_SELF_WAKE_UP && FEATURE_CAN_HAS_WAKE_UP_IRQ)
#include "flexcan_hw_access.h"
#endif

#if defined (YTM32B1L_SERIES)

/*******************************************************************************
 * Code
 ******************************************************************************/
#if defined(CAN_INSTANCE_COUNT) && (CAN_INSTANCE_COUNT > 0U)
/* Implementation of CAN0 IRQ handler for OR'ed interrupts (Bus Off,
Transmit Warning, Receive Warning, Error, Wakeup). */
void CAN0_ORed_Err_Wakeup_IRQHandler(void)
{
#if (FEATURE_CAN_HAS_SELF_WAKE_UP && FEATURE_CAN_HAS_WAKE_UP_IRQ)
    if(FLEXCAN_GetWAKINT(CAN0) != 0U)
    {
        FLEXCAN_WakeUpHandler(0U);
    }
    else
#endif
    {
        FLEXCAN_Error_IRQHandler(0U);
    }     
}

/* Implementation of CAN0 IRQ handler for interrupts indicating a successful
transmission or reception for Message Buffers 0-31. */
void CAN0_ORed_0_15_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U, 0U, 15U);
}
void CAN0_ORed_16_31_MB_IRQHandler(void)
{
    FLEXCAN_IRQHandler(0U, 16U, 31U);
}
#endif /* (CAN_INSTANCE_COUNT > 0U) */

#elif (defined (YTM32B1M_SERIES) || defined (YTM32B1H_SERIES))
void CAN0_ORed_IRQHandler(void)                                        /* CAN0_ORed Handler*/
{
    FLEXCAN_ORed_IRQHandler(0);
}
void CAN0_Error_IRQHandler(void)                                       /* CAN0_Error Handler*/
{
    FLEXCAN_Error_IRQHandler(0);
}
void CAN0_Wake_Up_IRQHandler(void)                                     /* CAN0_Wake_Up Handler*/
{
    FLEXCAN_WakeUpHandler(0);
}
void CAN0_ORed_0_15_MB_IRQHandler(void)                                /* CAN0_ORed_0_15_MB Handler*/
{
    FLEXCAN_IRQHandler(0, 0, 15);
}
void CAN0_ORed_16_31_MB_IRQHandler(void)                               /* CAN0_ORed_16_31_MB Handler*/
{
    FLEXCAN_IRQHandler(0, 16, 31);
}
#if defined(FEATURE_CAN0_MAX_MB_NUM) && (FEATURE_CAN0_MAX_MB_NUM >= 32U)
void CAN0_ORed_32_47_MB_IRQHandler(void)                               /* CAN0_ORed_32_47_MB Handler*/
{
    FLEXCAN_IRQHandler(0, 32, 47);
}
void CAN0_ORed_48_63_MB_IRQHandler(void)                               /* CAN0_ORed_48_63_MB Handler*/
{
    FLEXCAN_IRQHandler(0, 48, 63);
}
#endif
#if defined(FEATURE_CAN0_MAX_MB_NUM) && (FEATURE_CAN0_MAX_MB_NUM >= 64U)
void CAN0_ORed_64_79_MB_IRQHandler(void)                               /* CAN0_ORed_64_79_MB Handler*/
{
    FLEXCAN_IRQHandler(0, 64, 79);
}
void CAN0_ORed_80_95_MB_IRQHandler(void)                               /* CAN0_ORed_80_95_MB Handler*/
{
    FLEXCAN_IRQHandler(0, 80, 95);
}
void CAN0_ORed_96_111_MB_IRQHandler(void)                               /* CAN0_ORed_96_111_MB Handler*/
{
    FLEXCAN_IRQHandler(0, 96, 111);
}
void CAN0_ORed_112_127_MB_IRQHandler(void)                               /* CAN0_ORed_112_127_MB Handler*/
{
    FLEXCAN_IRQHandler(0, 112, 127);
}
#endif 
#if defined(CAN_INSTANCE_COUNT) && (CAN_INSTANCE_COUNT >= 1U)
void CAN1_ORed_IRQHandler(void)                                        /* CAN1_ORed Handler*/
{
    FLEXCAN_ORed_IRQHandler(1);
}
void CAN1_Error_IRQHandler(void)                                       /* CAN1_Error Handler*/
{
    FLEXCAN_Error_IRQHandler(1);
}
void CAN1_Wake_Up_IRQHandler(void)                                     /* CAN1_Wake_Up Handler*/
{
    FLEXCAN_WakeUpHandler(1);
}
void CAN1_ORed_0_15_MB_IRQHandler(void)                                /* CAN1_ORed_0_15_MB Handler*/
{
    FLEXCAN_IRQHandler(1, 0, 15);
}
void CAN1_ORed_16_31_MB_IRQHandler(void)                               /* CAN1_ORed_16_31_MB Handler*/
{
    FLEXCAN_IRQHandler(1, 16, 31);
}
#if defined(FEATURE_CAN1_MAX_MB_NUM) && (FEATURE_CAN1_MAX_MB_NUM >= 32U)
void CAN1_ORed_32_47_MB_IRQHandler(void)                               /* CAN1_ORed_32_47_MB Handler*/
{
    FLEXCAN_IRQHandler(1, 32, 47);
}
void CAN1_ORed_48_63_MB_IRQHandler(void)                               /* CAN1_ORed_48_63_MB Handler*/
{
    FLEXCAN_IRQHandler(1, 48, 63);
}
#endif
#if defined(FEATURE_CAN1_MAX_MB_NUM) && (FEATURE_CAN1_MAX_MB_NUM >= 64U)
void CAN1_ORed_64_79_MB_IRQHandler(void)                               /* CAN1_ORed_64_79_MB Handler*/
{
    FLEXCAN_IRQHandler(1, 64, 79);
}
void CAN1_ORed_80_95_MB_IRQHandler(void)                               /* CAN1_ORed_80_95_MB Handler*/
{
    FLEXCAN_IRQHandler(1, 80, 95);
}
void CAN1_ORed_96_111_MB_IRQHandler(void)                              /* CAN1_ORed_96_111_MB Handler*/
{
    FLEXCAN_IRQHandler(1, 96, 111);
}
void CAN1_ORed_112_127_MB_IRQHandler(void)                             /* CAN1_ORed_112_127_MB Handler*/
{
    FLEXCAN_IRQHandler(1, 112, 127);
}
#endif
#endif
#if defined(CAN_INSTANCE_COUNT) && (CAN_INSTANCE_COUNT >= 2U)  
void CAN2_ORed_IRQHandler(void)                                        /* CAN2_ORed Handler*/
{
    FLEXCAN_ORed_IRQHandler(2);
}
void CAN2_Error_IRQHandler(void)                                       /* CAN2_Error Handler*/
{
    FLEXCAN_Error_IRQHandler(2);
}
void CAN2_Wake_Up_IRQHandler(void)                                     /* CAN2_Wake_Up Handler*/
{
    FLEXCAN_WakeUpHandler(2);
}
void CAN2_ORed_0_15_MB_IRQHandler(void)                                /* CAN2_ORed_0_15_MB Handler*/
{
    FLEXCAN_IRQHandler(2, 0, 15);
}
void CAN2_ORed_16_31_MB_IRQHandler(void)                               /* CAN2_ORed_16_31_MB Handler*/
{
    FLEXCAN_IRQHandler(2, 16, 31);
}
#if defined(FEATURE_CAN2_MAX_MB_NUM) && (FEATURE_CAN2_MAX_MB_NUM >= 32U)
void CAN2_ORed_32_47_MB_IRQHandler(void)                               /* CAN2_ORed_32_47_MB Handler*/
{
    FLEXCAN_IRQHandler(2, 32, 47);
}
void CAN2_ORed_48_63_MB_IRQHandler(void)                               /* CAN2_ORed_48_63_MB Handler*/
{
    FLEXCAN_IRQHandler(2, 48, 63);
}
#endif
#endif
#if defined(CAN_INSTANCE_COUNT) && (CAN_INSTANCE_COUNT >= 3U)  
void CAN3_ORed_IRQHandler(void)                                        /* CAN3_ORed Handler*/
{
    FLEXCAN_ORed_IRQHandler(3);
}
void CAN3_Error_IRQHandler(void)                                       /* CAN3_Error Handler*/
{
    FLEXCAN_Error_IRQHandler(3);
}
void CAN3_Wake_Up_IRQHandler(void)                                     /* CAN3_Wake_Up Handler*/
{
    FLEXCAN_WakeUpHandler(3);
}
void CAN3_ORed_0_15_MB_IRQHandler(void)                                /* CAN3_ORed_0_15_MB Handler*/
{
    FLEXCAN_IRQHandler(3, 0, 15);
}
void CAN3_ORed_16_31_MB_IRQHandler(void)                               /* CAN3_ORed_16_31_MB Handler*/
{
    FLEXCAN_IRQHandler(3, 16, 31);
}
#if defined(FEATURE_CAN3_MAX_MB_NUM) && (FEATURE_CAN3_MAX_MB_NUM >= 32U)
void CAN3_ORed_32_47_MB_IRQHandler(void)                               /* CAN3_ORed_32_47_MB Handler*/
{
    FLEXCAN_IRQHandler(3, 32, 47);
}
void CAN3_ORed_48_63_MB_IRQHandler(void)                               /* CAN3_ORed_48_63_MB Handler*/
{
    FLEXCAN_IRQHandler(3, 48, 63);
}
#endif
#endif
#if defined(CAN_INSTANCE_COUNT) && (CAN_INSTANCE_COUNT >= 4U) 
void CAN4_ORed_IRQHandler(void)                                        /* CAN4_ORed Handler*/
{
    FLEXCAN_ORed_IRQHandler(4);
}
void CAN4_Error_IRQHandler(void)                                       /* CAN4_Error Handler*/
{
    FLEXCAN_Error_IRQHandler(4);
}
void CAN4_Wake_Up_IRQHandler(void)                                     /* CAN4_Wake_Up Handler*/
{
    FLEXCAN_WakeUpHandler(4);
}
void CAN4_ORed_0_15_MB_IRQHandler(void)                                /* CAN4_ORed_0_15_MB Handler*/
{
    FLEXCAN_IRQHandler(4, 0, 15);
}
void CAN4_ORed_16_31_MB_IRQHandler(void)                               /* CAN4_ORed_16_31_MB Handler*/
{
    FLEXCAN_IRQHandler(4, 16, 31);
}
#if defined(FEATURE_CAN4_MAX_MB_NUM) && (FEATURE_CAN4_MAX_MB_NUM >= 32U)
void CAN4_ORed_32_47_MB_IRQHandler(void)                               /* CAN4_ORed_32_47_MB Handler*/
{
    FLEXCAN_IRQHandler(4, 32, 47);
}
void CAN4_ORed_48_63_MB_IRQHandler(void)                               /* CAN4_ORed_48_63_MB Handler*/
{
    FLEXCAN_IRQHandler(4, 48, 63);
}
#endif
#endif
#if defined(CAN_INSTANCE_COUNT) && (CAN_INSTANCE_COUNT >= 5U) 
void CAN5_ORed_IRQHandler(void)                                        /* CAN5_ORed Handler*/
{
    FLEXCAN_ORed_IRQHandler(5);
}
void CAN5_Error_IRQHandler(void)                                       /* CAN5_Error Handler*/
{
    FLEXCAN_Error_IRQHandler(5);
}
void CAN5_Wake_Up_IRQHandler(void)                                     /* CAN5_Wake_Up Handler*/
{
    FLEXCAN_WakeUpHandler(5);
}
void CAN5_ORed_0_15_MB_IRQHandler(void)                                /* CAN5_ORed_0_15_MB Handler*/
{
    FLEXCAN_IRQHandler(5, 0, 15);
}
void CAN5_ORed_16_31_MB_IRQHandler(void)                               /* CAN5_ORed_16_31_MB Handler*/
{
    FLEXCAN_IRQHandler(5, 16, 31);
}
#endif
#if defined(CAN_INSTANCE_COUNT) && (CAN_INSTANCE_COUNT >= 6U) 
void CAN6_ORed_IRQHandler(void)                                        /* CAN6_ORed Handler*/
{
    FLEXCAN_ORed_IRQHandler(6);
}
void CAN6_Error_IRQHandler(void)                                       /* CAN6_Error Handler*/
{
    FLEXCAN_Error_IRQHandler(6);
}
void CAN6_Wake_Up_IRQHandler(void)                                     /* CAN6_Wake_Up Handler*/
{
    FLEXCAN_WakeUpHandler(6);
}
void CAN6_ORed_0_15_MB_IRQHandler(void)                                /* CAN6_ORed_0_15_MB Handler*/
{
    FLEXCAN_IRQHandler(6, 0, 15);
}
void CAN6_ORed_16_31_MB_IRQHandler(void)                               /* CAN6_ORed_16_31_MB Handler*/
{
    FLEXCAN_IRQHandler(6, 16, 31);
}
#endif
#if defined(CAN_INSTANCE_COUNT) && (CAN_INSTANCE_COUNT >= 7U) 
void CAN7_ORed_IRQHandler(void)                                        /* CAN7_ORed Handler*/
{
    FLEXCAN_ORed_IRQHandler(7);
}
void CAN7_Error_IRQHandler(void)                                       /* CAN7_Error Handler*/
{
    FLEXCAN_Error_IRQHandler(7);
}
void CAN7_Wake_Up_IRQHandler(void)                                     /* CAN7_Wake_Up Handler*/
{
    FLEXCAN_WakeUpHandler(7);
}
void CAN7_ORed_0_15_MB_IRQHandler(void)                                /* CAN7_ORed_0_15_MB Handler*/
{
    FLEXCAN_IRQHandler(7, 0, 15);
}
void CAN7_ORed_16_31_MB_IRQHandler(void)                               /* CAN7_ORed_16_31_MB Handler*/
{
    FLEXCAN_IRQHandler(7, 16, 31);
}
#endif
#else
    #error "No valid CPU defined!"
#endif

/*******************************************************************************
 * EOF
 ******************************************************************************/
