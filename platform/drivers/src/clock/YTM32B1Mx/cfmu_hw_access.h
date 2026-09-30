/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file cfmu_hw_access.h
 * @version 1.4.0
 */

#ifndef CFMU_HW_ACCESS_H
#define CFMU_HW_ACCESS_H

#include "device_registers.h"
#include <stdbool.h>
#include <stddef.h>



/*!
 * @ingroup cfmu_hw_access
 * @defgroup cfmu_hw_access
 * @{
 */

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus*/

/*!
 * @brief Enable CFMU clock out enable or not.
 *
 * This function enable/disable to-be-measured clock out.
 *
 * @param[in] base  Base address for current CFMU instance.
 * @param[in] enable    CFMU clock out enable or disable
 *            true - CFMU clock out enabled
 *            false - CFMU clock out disabled
 */
static inline void CFMU_SetClkOutEnable(CFMU_Type *base, const bool enable)
{
#ifdef CFMU_CTRL_CLK_OUTEN_SHIFT
    uint32_t regValue = base->CTRL;
    regValue &= ~CFMU_CTRL_CLK_OUTEN_MASK;
    base->CTRL = regValue | ((uint32_t)(enable ? 1U : 0U) << CFMU_CTRL_CLK_OUTEN_SHIFT);
#else
    (void) base;
    (void) enable;
#endif
}

/*!
 * @brief Select clock out source.
 *
 * This function select the to-be-measured clock out source.
 *
 * @param[in] base  Base address for current CFMU instance.
 * @param[in] clkoutSrc  clockout source.
 */
static inline void CFMU_SetClkOutSrc(CFMU_Type* const base, const uint8_t src)
{
    uint32_t regValue = base->CTRL;
    regValue &= ~(CFMU_CTRL_CLK_SEL_MASK);
    regValue |= CFMU_CTRL_CLK_SEL(src);
    base->CTRL = regValue;
}

/*!
 * @brief Set SCU clock output divider
 *
 * This function set to-be-measured clock output divider.
 *
 * @param[in] base  Base address for current CFMU instance.
 * @param[in] clkDiv Clock output divider. clock output frequency = clock input frequency / (clkDiv + 1).
 *           clkDiv must be in range [0, 255].
 */
static inline void CFMU_SetClkOutDiv(CFMU_Type *base, const uint8_t clkDiv)
{
    uint32_t regValue = base->CTRL;
    regValue &= (uint32_t)(~CFMU_CTRL_CLK_DIV_MASK);
    regValue |= (uint32_t)CFMU_CTRL_CLK_DIV(clkDiv);
    base->CTRL = regValue;
}

#if defined(__cplusplus)
}
#endif /* __cplusplus*/

/*! @}*/

#endif /* CFMU_HW_ACCESS_H */
/*******************************************************************************
 * EOF
 ******************************************************************************/
