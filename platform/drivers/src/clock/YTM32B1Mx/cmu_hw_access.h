/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file cmu_hw_access.h
 * @version 1.4.0
 */

#ifndef CMU_HW_ACCESS_H
#define CMU_HW_ACCESS_H

#include "device_registers.h"
#include <stdbool.h>
#include <stddef.h>


/*!
 * @ingroup cmu_hw_access
 * @defgroup cmu_hw_access
 * @{
 */

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus*/

/*!
 * @brief Enable CMU channel monitor enable or not.
 *
 * This function enable/disable CMU channel monitor.
 *
 * @param[in] base  Base address for current CMU instance.
 * @param[in] ch    CMU channel monitor channel
 * @param[in] enable    CMU channel monitor enable or disable
 *            true - CMU channel monitor enabled
 *            false - CMU channel monitor disabled
 */
static inline void CMU_SetChEnable(CMU_Type *base, const uint8_t ch, const bool enable)
{
    uint32_t regValue = base->CTRL;
    regValue &= ~((uint32_t)0x01U << (CMU_CTRL_EN_SHIFT + ch));
    base->CTRL = regValue | ((uint32_t)(enable ? 1U : 0U) << (CMU_CTRL_EN_SHIFT + ch));
}

/*!
 * @brief Enable CMU channel monitor reset enable or not.
 *
 * This function enable/disable CMU channel monitor reset function.
 *
 * @param[in] base  Base address for current CMU instance.
 * @param[in] ch    CMU channel monitor channel
 * @param[in] enable    CMU channel monitor enable or disable
 *            true - CMU channel monitor will reset MCU
 *            false - CMU channel monitor won't reset MCU
 */
static inline void CMU_SetChResetEnable(CMU_Type *base, const uint8_t ch, const bool enable)
{
    uint32_t regValue = base->CTRL;
    regValue &= ~((uint32_t)0x01U << (CMU_CTRL_RE_SHIFT + ch));
    base->CTRL = regValue | ((uint32_t)(enable ? 1U : 0U) << (CMU_CTRL_RE_SHIFT + ch));
}

/*!
 * @brief Select CMU channel reference clock
 *
 * This function set CMU channel reference clock
 *
 * @param[in] base  Base address for current CMU instance.
 * @param[in] ch    CMU channel monitor channel
 * @param[in] refClock CMU channel monitor reference clock
 *            0 - SIRC as reference clock
 *            1 - FXOSC as reference clock
 */
static inline void CMU_SetChRefClock(CMU_Type *base, const uint8_t ch, const uint8_t refClock)
{
    uint32_t regValue = base->CTRL;
    regValue &= ~((uint32_t)0x01U << (CMU_CTRL_REFS_SHIFT + ch));
    base->CTRL = regValue | ((uint32_t)refClock << (CMU_CTRL_REFS_SHIFT + ch));
}

/*!
 * @brief Set CMU channel compare high value
 *
 * This function set CMU channel compare high value.
 *
 * @param[in] base  Base address for current CMU instance.
 * @param[in] ch    CMU channel monitor channel
 * @param[in] value CMU channel compare high value
 */
static inline void CMU_SetChCmpHigh(CMU_Type *base, const uint8_t ch, const uint16_t value)
{
    base->CMP[ch].HIGH = value;
}
/*!
 * @brief Set CMU channel compare low value
 *
 * This function set CMU channel compare low value.
 *
 * @param[in] base  Base address for current CMU instance.
 * @param[in] ch    CMU channel monitor channel
 * @param[in] value CMU channel compare low value
 */
static inline void CMU_SetChCmpLow(CMU_Type *base, const uint8_t ch, const uint16_t value)
{
    base->CMP[ch].LOW = value;
}

#if defined(__cplusplus)
}
#endif /* __cplusplus*/

/*! @}*/

#endif /* CMU_HW_ACCESS_H */
/*******************************************************************************
 * EOF
 ******************************************************************************/
