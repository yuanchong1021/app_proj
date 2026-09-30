/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file flash_hw_access.h
 * @version 1.4.0
 */

#ifndef FLASH_HW_ACCESS_H
#define FLASH_HW_ACCESS_H

#include <stdbool.h>
#include "flash_driver.h"


/*!
 * @{
 */

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif


/*!
 * @brief Get the flash idle status.
 *
 * @param[in] efmBase - efm base pointer
 * @return true - flash is idle
 *         false - flash is busy
 */
static inline bool FLASH_GetIdleStatus(EFM_Type *efmBase)
{
    return (bool)(EFM_STS_IDLE_MASK == (efmBase->STS & EFM_STS_IDLE_MASK));
}

/*!
 * @brief Get the flash error status.
 *
 * @param[in] efmBase - efm base pointer
 * @return true - flash error occurred
 *         false - flash no error
 */
static inline bool FLASH_GetErrorStatus(EFM_Type *efmBase)
{
    return (bool)(0U != (efmBase->STS & FEATURE_EFM_CMD_ERROR_MASK));
}

/*!
 * @brief Clear the flash busy status.
 *
 * @param[in] efmBase - efm base pointer
 */
static inline void FLASH_ClearErrorFlags(EFM_Type *efmBase)
{
    efmBase->STS = FEATURE_EFM_CMD_ERROR_MASK;
}

/*!
 * @brief Get the flash done status.
 * 
 * @param[in] efmBase - efm base pointer
 * @return true - flash command done
 *         false - flash command is in progress
 */
static inline bool FLASH_GetDoneStatus(EFM_Type *efmBase)
{
    return (bool)(EFM_STS_DONE_MASK == (efmBase->STS & EFM_STS_DONE_MASK));
}

/*!
 * @brief Clear the flash done status.
 * 
 * @param[in] efmBase - efm base pointer
 */
static inline void FLASH_ClearDoneStatusFlag(EFM_Type *efmBase)
{
    efmBase->STS = EFM_STS_DONE_MASK;
}

/*!
 * @brief Set the flash command code.
 * 
 * @param[in] efmBase - efm base pointer
 */
static inline void FLASH_SetCmdCode(EFM_Type *efmBase, uint32_t cmdCode)
{
    efmBase->CMD = EFM_CMD_CMD(cmdCode);
}

/*!
 * @brief Check the read collision error flag is detected or not.
 *
 * @param[in] efmBase - efm base pointer
 * @return the read collision error flag
 *        - true: Collision error detected.
 *        - false: No collision error detected.
 */
static inline bool FLASH_GetReadCollisionFlag(EFM_Type *efmBase)
{
    return (EFM_STS_ACCERR_MASK == (efmBase->STS & EFM_STS_ACCERR_MASK));
}

/*!
 * @brief Clear the read collision error flag.
 *
 * @param[in] efmBase - efm base pointer
 */
static inline void FLASH_ClearReadCollisionFlag(EFM_Type *efmBase)
{
    efmBase->STS = EFM_STS_ACCERR_MASK;
}

/*!
 * @brief Check the Single bit fault flag is detected during a valid
 * flash read access from the platform flash controller
 *
 * @param[in] efmBase - efm base pointer
 * @return the platform flash error status
 * @retval true: Single bit fault detected.
 * @retval false: No Single bit fault detected.
 */
static inline bool FLASH_GetSingleBitFaultFlag(EFM_Type *efmBase)
{
    return (EFM_STS_RECOVERR_MASK == (efmBase->STS & EFM_STS_RECOVERR_MASK));
}

/*!
 * @brief Clear the platform Flash Single bit fault detect flag.
 *
 * @param[in] efmBase - efm base pointer
 */
static inline void FLASH_ClearSingleBitFaultFlag(EFM_Type *efmBase)
{
    efmBase->STS = EFM_STS_RECOVERR_MASK;
}

/*!
 * @brief Check the double bit fault flag is detected during a valid
 * flash read access from the platform flash controller
 *
 * @param[in] efmBase - efm base pointer
 * @return the platform flash error status
 * @retval true: Double bit fault detected.
 * @retval false: No double bit fault detected.
 */
static inline bool FLASH_GetDoubleBitFaultFlag(EFM_Type *efmBase)
{
    return (EFM_STS_UNRECOVERR_MASK == (efmBase->STS & EFM_STS_UNRECOVERR_MASK));
}

/*!
 * @brief Clear the platform Flash double bit fault detect flag.
 *
 * @param[in] efmBase - efm base pointer
 */
static inline void FLASH_ClearDoubleBitFaultFlag(EFM_Type *efmBase)
{
    efmBase->STS = EFM_STS_UNRECOVERR_MASK;
}

#ifdef __cplusplus
}
#endif

/*! @}*/

#endif /* FLASH_HW_ACCESS_H */

/*******************************************************************************
 * EOF
 ******************************************************************************/
