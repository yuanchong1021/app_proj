/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file flash_driver.c
 * @version 1.4.0
 */

/*!
 * @page misra_violations MISRA-C:2012 violations list
 *
 * PRQA S 0311 Rule 11.8: Dangerous pointer cast results in loss of const qualification.
 *
 * PRQA S 0316 Rule 11.5: [I] Cast from a pointer to void to a pointer to object type.
 *
 * PRQA S 0488 Rule 18.4: Performing pointer arithmetic.
 *
 */

#include "flash_driver.h"
#include "flash_hw_access.h"
#include "interrupt_manager.h"
#include "clock_manager.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* Table of base addresses for EFM instances. */
static EFM_Type * const s_efmBase[EFM_INSTANCE_COUNT] = EFM_BASE_PTRS;

/* Pointer to runtime state structure.*/
static flash_state_t * s_FlashStatePtr[EFM_INSTANCE_COUNT] = {NULL};

/* Table for EFM IRQ numbers */
static const IRQn_Type s_efmIrqId[EFM_INSTANCE_COUNT] = EFM_IRQS;

#ifdef EFM_READ_COLLISION_IRQS
static const IRQn_Type s_efmReadCollisionIrqId[EFM_READ_COLLISION_IRQS_CH_COUNT] = EFM_READ_COLLISION_IRQS;
#endif /* EFM_READ_COLLISION_IRQS */

static flash_syncCallback_t s_SyncCallBackFunction = NULL_SYNCCALLBACK;

static uint32_t s_tempData = 0x12345678U;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/* Declaration of EFM_IRQHandler. Needed just to avoid MISRA violation. */
void EFM_IRQHandler(void);

/* Declaration of EFM_Error_IRQHandler. Needed just to avoid MISRA violation. */
void EFM_Error_IRQHandler(void);

/*******************************************************************************
 * Internal Functions
 ******************************************************************************/
/*!
* @brief Internal flash command to execute flash commands.
*        This function will be copy to RAM section when system boot.
*
* @param[in] command Target flash command.
*/
START_FUNCTION_DECLARATION_RAMSECTION
static status_t FLASH_LaunchCommandSequence(uint32_t instance) __attribute__((noinline))
END_FUNCTION_DECLARATION_RAMSECTION

static uint32_t FLASH_GetSectorSize(uint32_t dest);

static void FLASH_DoneIRQHandler(uint32_t instance);

#if defined(EFM_READ_COLLISION_IRQS_CH_COUNT) && (EFM_READ_COLLISION_IRQS_CH_COUNT > 0U)
static void FLASH_ReadCollisionIRQHandler(uint32_t instance);
#endif /* EFM_READ_COLLISION_IRQS_CH_COUNT */

#ifdef FEATURE_EFM_BOOT_SWAP_CMD_CODE
static status_t FLASH_BootSwap(uint32_t instance);
#endif /* FEATURE_EFM_BOOT_SWAP_CMD_CODE */

#if defined(FEATURE_EFM_UNLOCK_CMD_COMPLEX) && (FEATURE_EFM_UNLOCK_CMD_COMPLEX == 1)
static void FLASH_Pflash_UnlockCmd(EFM_Type *base, uint32_t cmd, uint32_t addr);
static void FLASH_Dflash_UnlockCmd(EFM_Type *base, uint32_t cmd, uint32_t addr);
#endif /* FEATURE_EFM_UNLOCK_CMD_COMPLEX */

static void FLASH_EventCallback(EFM_Type *base, flash_state_t *state, flash_event_t event);

static uint32_t FLASH_StateCalcShiftSum(const flash_state_t *state);
/*******************************************************************************
 * Code
 ******************************************************************************/
#if defined(FEATURE_EFM_UNLOCK_CMD_COMPLEX) && (FEATURE_EFM_UNLOCK_CMD_COMPLEX == 1)
/*!
 * @brief Internal pflash command to unlock flash command.
 *
 * @param[in] base Target efm base.
 * @param[in] cmd Target flash command.
 * @param[in] addr Target destination address.
*/
static void FLASH_Pflash_UnlockCmd(EFM_Type *base, uint32_t cmd, uint32_t addr)
{
    uint32_t blockUnlockKeys[] = FEATURE_EFM_PFLASH_BLOCK_UNLOCK_KEY_VALUES;
    uint32_t arrayUnlockKeys[] = FEATURE_EFM_PFLASH_ARRAY_UNLOCK_KEY_VALUES;
    uint32_t arrayIndex = FEATURE_EFM_PFLASH_ARRAY_COUNT_MAX;
    uint32_t isNVR = 0;
    uint32_t sectorId = 0;
    FEATURE_EFM_PFLASH_GET_SECTOR_ARRAY_ID(addr, arrayIndex, isNVR);
    DEV_ASSERT(arrayIndex < FEATURE_EFM_PFLASH_ARRAY_COUNT_MAX);

    switch(cmd)
    {
        case FEATURE_EFM_ERASE_SECTOR_CMD_CODE:
        case FEATURE_EFM_ERASE_SECTOR_RETRY_CMD_CODE:
        case FEATURE_EFM_ERASE_SECTOR_VREAD_RETRY_CMD_CODE:
        case FEATURE_EFM_PROGRAM_CMD_CODE:
        case FEATURE_EFM_BOOT_SWAP_CMD_CODE:
            sectorId = FEATURE_EFM_PFLASH_ARRAY_IDX(arrayIndex) | FEATURE_EFM_PFLASH_NVR_IDX(isNVR);
            sectorId |= isNVR ? FEATURE_EFM_PFLASH_NVR_ADDR(addr) : FEATURE_EFM_PFLASH_MIAN_ADDR(addr);
            base->CMD_UNLOCK = FEATURE_EFM_PFLASH_SECTOR_UNLOCK_KEY(FEATURE_EFM_PFLASH_SECTOR_UNLOCK_KEY_VALUE) |
                               FEATURE_EFM_PFLASH_SECTOR_ID_INV(~sectorId) | sectorId;
            break;
        case FEATURE_EFM_ERASE_BLOCK_CMD_CODE:
            base->CMD_UNLOCK = blockUnlockKeys[FEATURE_EFM_PFLASH_BLOCK_IDX(addr)];
            break;
        case FEATURE_EFM_ERASE_ARRAY_CMD_CODE:
            base->CMD_UNLOCK = arrayUnlockKeys[arrayIndex];
            break;
        default:
            break;
    }
}

/*!
 * @brief Internal Dflash command to unlock flash command.
 *
 * @param[in] base Target efm base.
 * @param[in] cmd Target flash command.
 * @param[in] addr Target destination address.
*/
static void FLASH_Dflash_UnlockCmd(EFM_Type *base, uint32_t cmd, uint32_t addr)
{
    uint32_t blockUnlockKeys[] = FEATURE_EFM_PFLASH_BLOCK_UNLOCK_KEY_VALUES;
    uint32_t isNVR = 0xFF;
    uint32_t sectorId = 0;
    if(addr >= FEATURE_EFM_DFLASH_MAIN_START_ADDRESS && addr <= FEATURE_EFM_DFLASH_MAIN_END_ADDRESS)
    {
        isNVR = 0;
    }else if (addr >= FEATURE_EFM_DFLASH_NVR_START_ADDRESS && addr <= FEATURE_EFM_DFLASH_NVR_END_ADDRESS)
    {
        isNVR = 1;
    }else{
        /* Invalid address */
        DevAssert(0);
    }

    switch(cmd)
    {
        case FEATURE_EFM_ERASE_SECTOR_CMD_CODE:
        case FEATURE_EFM_PROGRAM_DATA_CMD_CODE:
            sectorId = FEATURE_EFM_DFLASH_NVR_IDX(isNVR);
            sectorId |= isNVR ? FEATURE_EFM_DFLASH_NVR_ADDR(addr) : FEATURE_EFM_DFLASH_MIAN_ADDR(addr);
            base->CMD_UNLOCK = FEATURE_EFM_DFLASH_SECTOR_UNLOCK_KEY(FEATURE_EFM_DFLASH_SECTOR_UNLOCK_KEY_VALUE) |
                               FEATURE_EFM_DFLASH_SECTOR_ID_INV(~sectorId) | sectorId;
            break;
        case FEATURE_EFM_ERASE_BLOCK_CMD_CODE:
            base->CMD_UNLOCK = blockUnlockKeys[FEATURE_EFM_DFLASH_BLOCK_IDX(addr)];
            break;
        default:
            break;
    }
}

/* Use instance to choose the cmd unlock function for pflash and dflash. */
typedef void (*FLASH_UnlockCmds_t)(EFM_Type *base, uint32_t cmd, uint32_t addr);
static FLASH_UnlockCmds_t s_FlashUnlockCmd[EFM_INSTANCE_COUNT] = { FLASH_Pflash_UnlockCmd, FLASH_Dflash_UnlockCmd};
#endif /* FEATURE_EFM_UNLOCK_CMD_COMPLEX */

/*FUNCTION**********************************************************************
 *
 * Function Name : FLASH_LaunchCommandSequence
 * Description   : Perform command write sequence on Flash.
 * It is internal function, called by driver APIs only.
 *
 *END**************************************************************************/
START_FUNCTION_DEFINITION_RAMSECTION
DISABLE_CHECK_RAMSECTION_FUNCTION_CALL
static status_t FLASH_LaunchCommandSequence(uint32_t instance)
{
    DEV_ASSERT(instance < EFM_INSTANCE_COUNT);
    EFM_Type *base = s_efmBase[instance];
    flash_state_t *state = s_FlashStatePtr[instance];
    DEV_ASSERT(state != NULL);
    status_t status = STATUS_SUCCESS; /* Return code variable */
    uint32_t primask_bit;

    /* Enter critical section: Disable interrupts to avoid any interruption during the command launch */
    primask_bit = __get_PRIMASK();
    __disable_irq();

    if((state->cmdParam.cmdCode == FEATURE_EFM_ERASE_SECTOR_CMD_CODE)
#ifdef FEATURE_EFM_ERASE_SECTOR_RETRY_CMD_CODE
       || (state->cmdParam.cmdCode == FEATURE_EFM_ERASE_SECTOR_RETRY_CMD_CODE)
#endif
    )
    {
        /* The cmd param dest must be checked to ensure the erase function is not called abnormally. */
        if ((((uint32_t)state->cmdParam.dest & (FEATURE_EFM_FLASH_MIN_SECTOR_SIZE - 1U)) != 0U)
            || (((uint32_t)state->leftSize & (FEATURE_EFM_FLASH_MIN_SECTOR_SIZE - 1U)) != 0U)
            || (state->leftSize <= 0)) 
        {
            status = STATUS_InvalidArgument;
        }
    }

    if (state->shiftsum != FLASH_StateCalcShiftSum(state))
    {
        status = STATUS_InvalidArgument;
    }

    if (status == STATUS_SUCCESS)
    {
#ifndef EFM_CTRL_WE_MASK
        /* Add NOP command to clear write buffer */
        base->CMD = 0;
#endif

#if defined(FEATURE_EFM_HAS_READONLY_BIT) && (FEATURE_EFM_HAS_READONLY_BIT == 1)
        /* Flash can be read, erased and programmed */
        EFM->CTRL &= ~EFM_CTRL_READONLY_MASK;
#endif
    /* clear pending status */
    /* static inline functions are risks due to "-o0" optimization not inlined,
       So operate registers directly in ram function */
    base->STS = FEATURE_EFM_CMD_STS_CLEAR_MASK;

        EFM_ENABLE_WE_COMMAND(base);
        /* Passing parameter to the command */
        for (uint32_t i = 0; i < state->cmdParam.word_size; i++)
        {
            state->cmdParam.dest[i] = state->cmdParam.pdata[i];
#ifdef EFM_STS_ARRAY_SELECTED_MASK
            while (EFM_STS_ARRAY_SELECTED_MASK != (base->STS & EFM_STS_ARRAY_SELECTED_MASK))
            {
                /* Wait until the array is selected */
            }
#endif /* EFM_STS_SET_ADDR_MASK */
        }
#ifdef EFM_STS_SET_ADDR_MASK
        while (EFM_STS_SET_ADDR_MASK != (base->STS & EFM_STS_SET_ADDR_MASK))
        {
            /* Wait until the address is set */
        }
#endif /* EFM_STS_SET_ADDR_MASK */
        EFM_DISABLE_WE_COMMAND(base);

#if defined(FEATURE_EFM_UNLOCK_CMD_COMPLEX) && (FEATURE_EFM_UNLOCK_CMD_COMPLEX == 1)
        s_FlashUnlockCmd[instance](base, state->cmdParam.cmdCode, (uint32_t)state->cmdParam.dest);
#else
        EFM_UNLOCK_CMD_REGISTER(base);
#endif /* EFM_UNLOCK_CMD_REGISTER */

        /* Write command register to launch command */
        if (state->cmdParam.cmdCode == 0xFEU)
        {
            /* Quick erase sector by split erase sector time */
            base->CMD = FEATURE_EFM_ERASE_SECTOR_CMD_CODE;
        }
        else
        {
            base->CMD = EFM_CMD_CMD(state->cmdParam.cmdCode);
        }

        if (false == state->async)
        {
            while ((EFM_STS_DONE_MASK != (base->STS & EFM_STS_DONE_MASK)) &&
                   (EFM_STS_ACCERR_MASK != (base->STS & EFM_STS_ACCERR_MASK)))
            {
                /* Wait untill Done bit is set
             * Serve callback function as often as possible
             */
                if (NULL_SYNCCALLBACK != s_SyncCallBackFunction)
                {
                    /* Temporarily disable compiler's check for ROM access call from within a ram function.
                 * The use of a function pointer type makes this check irrelevant.
                 * Nevertheless, it is imperative that the user-provided callback be defined in RAM SECTION */
                    s_SyncCallBackFunction();
                }
            }
        }
        if (0U != (base->STS & FEATURE_EFM_CMD_ERROR_MASK))
        {
            status = STATUS_ERROR;
        }

#if defined(FEATURE_EFM_HAS_READONLY_BIT) && (FEATURE_EFM_HAS_READONLY_BIT == 1)
        /* Read only flash array */
        EFM->CTRL |= EFM_CTRL_READONLY_MASK;
#endif
    }
    else
    {
        if (state->async)
        {
            FLASH_EventCallback(base, state, FLASH_EVENT_ERROR);
        }else{
            /* Do nothing, just return error status for sync mode. */
        }
    }
    /* Exit critical section: restore previous priority mask */
    __set_PRIMASK(primask_bit);

    return status;
}
ENABLE_CHECK_RAMSECTION_FUNCTION_CALL
END_FUNCTION_DEFINITION_RAMSECTION

/*FUNCTION**********************************************************************
 *
 * Function Name : FLASH_GetSectorSize
 * Description   : Get flash sector size based on the destination address.
 * It is internal function, called by driver APIs only.
 *
 *END**************************************************************************/
static uint32_t FLASH_GetSectorSize(uint32_t dest)
{
    uint32_t sectorSize = 0;
    if ((dest < FEATURE_EFM_MAIN_ARRAY_END_ADDRESS)
#if defined(FEATURE_EFM_MAIN_ARRAY_START_ADDRESS) && (FEATURE_EFM_MAIN_ARRAY_START_ADDRESS != 0)
            && (dest >= FEATURE_EFM_MAIN_ARRAY_START_ADDRESS)
#endif /* FEATURE_EFM_MAIN_ARRAY_START_ADDRESS */
        )
    {
        /* Flash main array */
        sectorSize = FEATURE_EFM_MAIN_ARRAY_SECTOR_SIZE;
    }
#if FEATURE_EFM_HAS_DATA_FLASH
#ifdef FEATURE_EFM_DATA_ARRAY_START_ADDRESS
    else if ((dest < FEATURE_EFM_DATA_ARRAY_END_ADDRESS)
            && (dest >= FEATURE_EFM_DATA_ARRAY_START_ADDRESS))
    {
        /* Flash data array */
        sectorSize = FEATURE_EFM_DATA_ARRAY_SECTOR_SIZE;
    }
#endif
#ifdef FEATURE_EFM_DATA_ARRAY0_START_ADDRESS
    else if ((dest < FEATURE_EFM_DATA_ARRAY0_END_ADDRESS)
            && (dest >= FEATURE_EFM_DATA_ARRAY0_START_ADDRESS))
    {
        /* Flash data array 0 */
        sectorSize = FEATURE_EFM_DATA_ARRAY_SECTOR_SIZE;
    }
#endif
#ifdef FEATURE_EFM_DATA_ARRAY1_START_ADDRESS
    else if ((dest < FEATURE_EFM_DATA_ARRAY1_END_ADDRESS)
            && (dest >= FEATURE_EFM_DATA_ARRAY1_START_ADDRESS))
    {
        /* Flash data array 1 */
        sectorSize = FEATURE_EFM_DATA_ARRAY_SECTOR_SIZE;
    }
#endif
#endif
#if FEATURE_EFM_HAS_NVR_FLASH
    else if ((dest < FEATURE_EFM_NVR_ARRAY_END_ADDRESS)
             && (dest >= FEATURE_EFM_NVR_ARRAY_START_ADDRESS))
    {
        /* Flash NVR array */
        sectorSize = FEATURE_EFM_NVR_ARRAY_SECTOR_SIZE;
    }
#endif
    else{
        sectorSize = 0;
    }
    return sectorSize;
}

#if (defined(FEATURE_EFM_PREREAD_CHECK_FOR_PROGRAM) && (FEATURE_EFM_PREREAD_CHECK_FOR_PROGRAM == 1)) || \
    (defined(FEATURE_EFM_SW_READ_VERIFY) && (FEATURE_EFM_SW_READ_VERIFY == 1))
/*FUNCTION**********************************************************************
*
* Function Name : FLASH_DRV_SoftwareVerifyMemory
* Description   : Flash software verification. 
*                 Check if memory matches the expected data.
* 
*END**************************************************************************/
static status_t FLASH_DRV_SoftwareVerifyMemory(uint32_t address, volatile const uint32_t *expectedData, uint32_t wordCount)
{
    status_t status = STATUS_SUCCESS;
    if (expectedData == NULL) 
    {
        /* Erase verification or pre-read check */
        for (uint32_t i = 0; i < wordCount; i++)
        {
            volatile uint32_t *currentAddr = (volatile uint32_t *)(address + (i * sizeof(uint32_t)));
            if (*currentAddr != 0xFFFFFFFFU)
            {
                status = STATUS_ERROR;
                break;
            }
        }
    }
    else 
    {
        /* Programming verification */
        for (uint32_t i = 0; i < wordCount; i++)
        {
            volatile uint32_t *currentAddr = (volatile uint32_t *)(address + (i * sizeof(uint32_t)));
            uint32_t tmpData = expectedData[i];
            if (*currentAddr != tmpData)
            {
                status = STATUS_ERROR;
                break;
            }
        }
    }
    return status;
}
#endif

/*FUNCTION**********************************************************************
 *
 * Function Name : FLASH_StateCalcChecksum
 * Description   : Calculate checksum value for flash state structure.
 *
 *END**************************************************************************/
static uint32_t FLASH_StateCalcShiftSum(const flash_state_t *state)
{
    uint32_t sum = 0;
    sum = (sum << 1) + state->cmdParam.cmdCode;
    sum = (sum << 1) + (uint32_t)state->cmdParam.dest;
    sum = (sum << 1) + (uint32_t)state->cmdParam.pdata;
    sum = (sum << 1) + state->cmdParam.word_size;
    sum = (sum << 1) + (uint32_t)state->leftSize;
    return sum;
}

/*FUNCTION**********************************************************************
*
* Function Name : FLASH_EventCallback
* Description   : Callback function for flash module.
*                 Used only for internal functions.
*
*END**************************************************************************/
static void FLASH_EventCallback(EFM_Type *base, flash_state_t *state, flash_event_t event)
{
    (void) base;
    if((event == FLASH_EVENT_ERROR) || (event == FLASH_EVENT_COMPLETE) || (event == FLASH_EVENT_ACCESS_ERROR))
    {
        state->driverBusy = false;
    }
#if (defined(FEATURE_EFM_ERASE_SECTOR_RETRY_CMD_CODE) && defined(EFM_CTRL_CMD_VERIFY_EN_MASK))
#ifdef FEATURE_EFM_ERASE_SECTOR_VREAD_RETRY_CMD_CODE
    if ((state->cmdParam.cmdCode == FEATURE_EFM_ERASE_SECTOR_RETRY_CMD_CODE) || (state->cmdParam.cmdCode == FEATURE_EFM_ERASE_SECTOR_VREAD_RETRY_CMD_CODE))
#else
    if (state->cmdParam.cmdCode == FEATURE_EFM_ERASE_SECTOR_RETRY_CMD_CODE)
#endif /* FEATURE_EFM_ERASE_SECTOR_VREAD_RETRY_CMD_CODE */
    {
        if (true == state->readVerify)
        {
            base->CTRL |= EFM_CTRL_CMD_VERIFY_EN_MASK;
        }
        else
        {
            base->CTRL &= ~EFM_CTRL_CMD_VERIFY_EN_MASK;
        }
    }
#endif
    if (NULL_CALLBACK != state->callback)
    {
        state->callback(event);
    }
}

/*FUNCTION**********************************************************************
*
* Function Name : FLASH_DoneIRQHandler
* Description   : Interrupt handler for flash module done event.
*
*END**************************************************************************/
static void FLASH_DoneIRQHandler(uint32_t instance)
{
    DEV_ASSERT(instance < EFM_INSTANCE_COUNT);
    status_t status = STATUS_SUCCESS;
    EFM_Type *base = s_efmBase[instance];
    flash_state_t *state = s_FlashStatePtr[instance];
    DEV_ASSERT(state != NULL);
#ifdef FEATURE_EFM_PROGRAM_NVR_CMD_CODE
    uint32_t efm_data[FEATURE_EFM_WRITE_UNIT_WORD_SIZE + 1U];
#endif /* FEATURE_EFM_PROGRAM_NVR_CMD_CODE */

#ifdef EFM_CTRL_RETRY_CFG_MASK
    uint32_t retry_cfg;
#endif /* EFM_CTRL_RETRY_CFG_MASK */

    FLASH_ClearDoneStatusFlag(base);
    if (state->async)
    {
        if(state->shiftsum != FLASH_StateCalcShiftSum(state))
        {
            FLASH_EventCallback(base, state, FLASH_EVENT_ERROR);
        }
        else if (FLASH_GetErrorStatus(base))
        {
#ifdef FEATURE_EFM_ERASE_SECTOR_VREAD_RETRY_CMD_CODE
            if (FEATURE_EFM_ERASE_SECTOR_VREAD_RETRY_CMD_CODE == state->cmdParam.cmdCode)
            {
                state->cmdParam.cmdCode = FEATURE_EFM_ERASE_SECTOR_RETRY_CMD_CODE;
            }
#endif /* FEATURE_EFM_ERASE_SECTOR_VREAD_RETRY_CMD_CODE */
#ifdef FEATURE_EFM_ERASE_SECTOR_RETRY_CMD_CODE
            if (FEATURE_EFM_ERASE_SECTOR_RETRY_CMD_CODE == state->cmdParam.cmdCode)
            {
                if (state->retryCount < FLASH_RETRY_MAX_COUNT)
                {
                    state->retryCount += 1U;
#ifdef EFM_CTRL_RETRY_CFG_MASK
                    if ((FLASH_RETRY_CFG_START + state->retryCount) <= 0x7U)
                    {
                        retry_cfg = base->CTRL & ~EFM_CTRL_RETRY_CFG_MASK;
                        retry_cfg |= EFM_CTRL_RETRY_CFG(FLASH_RETRY_CFG_START + state->retryCount); /* IAR Warning[Pa082] */
                        base->CTRL = retry_cfg;
                    }
#endif /* EFM_CTRL_RETRY_CFG_MASK */
                    state->shiftsum = FLASH_StateCalcShiftSum(state);
                    status = FLASH_LaunchCommandSequence(instance);
                    if (status != STATUS_SUCCESS)
                    {
                        FLASH_EventCallback(base, state, FLASH_EVENT_ERROR);
                    }
                }
                else
                {
                    FLASH_EventCallback(base, state, FLASH_EVENT_ERROR);
                }
            }
            else
#endif /* FEATURE_EFM_ERASE_SECTOR_RETRY_CMD_CODE */
            {
                FLASH_EventCallback(base, state, FLASH_EVENT_ERROR);
            }
        }
        else
        {
            switch (state->cmdParam.cmdCode)
            {
#ifdef FEATURE_EFM_ERASE_BLOCK_CMD_CODE
                case FEATURE_EFM_ERASE_BLOCK_CMD_CODE:
#ifdef FEATURE_EFM_ERASE_BLOCK_VERIFY_CMD_CODE
                case FEATURE_EFM_ERASE_BLOCK_VERIFY_CMD_CODE:
#endif /* FEATURE_EFM_ERASE_BLOCK_VERIFY_CMD_CODE */
#ifdef FEATURE_EFM_ERASE_ARRAY_CMD_CODE
                case FEATURE_EFM_ERASE_ARRAY_CMD_CODE:
#endif /* FEATURE_EFM_ERASE_ARRAY_CMD_CODE */
                    state->leftSize = 0;
                    break;
#endif /* FEATURE_EFM_ERASE_BLOCK_CMD_CODE */

                case FEATURE_EFM_ERASE_SECTOR_CMD_CODE:
#ifdef FEATURE_EFM_ERASE_SECTOR_VERIFY_CMD_CODE
                case FEATURE_EFM_ERASE_SECTOR_VERIFY_CMD_CODE:
#endif /* FEATURE_EFM_ERASE_SECTOR_VERIFY_CMD_CODE */
                    state->leftSize -= (int32_t)FLASH_GetSectorSize((uint32_t)state->cmdParam.dest);
                    state->cmdParam.dest += FLASH_GetSectorSize((uint32_t)state->cmdParam.dest) >> 2U; /* PRQA S 0488 */
                    break;

#ifdef FEATURE_EFM_ERASE_SECTOR_RETRY_CMD_CODE
                case FEATURE_EFM_ERASE_SECTOR_RETRY_CMD_CODE:
#ifdef FEATURE_EFM_ERASE_SECTOR_VREAD_RETRY_CMD_CODE
                    state->cmdParam.cmdCode = FEATURE_EFM_ERASE_SECTOR_VREAD_RETRY_CMD_CODE;
                    break;
                case FEATURE_EFM_ERASE_SECTOR_VREAD_RETRY_CMD_CODE:
                    state->cmdParam.cmdCode = FEATURE_EFM_ERASE_SECTOR_RETRY_CMD_CODE;
#endif /* FEATURE_EFM_ERASE_SECTOR_VREAD_RETRY_CMD_CODE */
#ifdef EFM_CTRL_RETRY_CFG_MASK
                    retry_cfg = (base->CTRL & ~EFM_CTRL_RETRY_CFG_MASK);
                    retry_cfg |= EFM_CTRL_RETRY_CFG(0x7U);
#endif /* EFM_CTRL_RETRY_CFG_MASK */
                    /* Append one more erase after finish to ensure erase thoroughly */
                    if (state->retryCount != 0xFFFFFFFFU)
                    {
                        state->retryCount = 0xFFFFFFFFU;
                    }
                    else
                    {
                        state->retryCount = 0U;
#ifdef EFM_CTRL_RETRY_CFG_MASK
                        retry_cfg |= EFM_CTRL_RETRY_CFG(FLASH_RETRY_CFG_START);
#endif /* EFM_CTRL_RETRY_CFG_MASK */
                        state->leftSize -= (int32_t)FLASH_GetSectorSize((uint32_t)state->cmdParam.dest);
                        state->cmdParam.dest += FLASH_GetSectorSize((uint32_t)state->cmdParam.dest) >> 2U; /* PRQA S 0488 */
                    }
#ifdef EFM_CTRL_RETRY_CFG_MASK
                    base->CTRL = retry_cfg;
#endif /* EFM_CTRL_RETRY_CFG_MASK */
                    break;
#endif /* FEATURE_EFM_ERASE_SECTOR_RETRY_CMD_CODE */

                case FEATURE_EFM_PROGRAM_CMD_CODE:
#ifdef FEATURE_EFM_PROGRAM_VERIFY_CMD_CODE
                case FEATURE_EFM_PROGRAM_VERIFY_CMD_CODE:
#endif /* FEATURE_EFM_PROGRAM_VERIFY_CMD_CODE */
                    state->leftSize -= (int32_t)FEATURE_EFM_WRITE_UNIT_SIZE;
                    state->cmdParam.dest += FEATURE_EFM_WRITE_UNIT_WORD_SIZE; /* PRQA S 0488 */
                    state->cmdParam.pdata += FEATURE_EFM_WRITE_UNIT_WORD_SIZE; /* PRQA S 0488 */
                    break;

#ifdef FEATURE_EFM_PROGRAM_DATA_CMD_CODE
                case FEATURE_EFM_PROGRAM_DATA_CMD_CODE:
                    state->leftSize -= (int32_t)FEATURE_EFM_WRITE_DATA_UNIT_SIZE;
                    state->cmdParam.dest += FEATURE_EFM_WRITE_DATA_UNIT_WORD_SIZE; /* PRQA S 0488 */
                    state->cmdParam.pdata += FEATURE_EFM_WRITE_DATA_UNIT_WORD_SIZE; /* PRQA S 0488 */
                    break;
#endif /* FEATURE_EFM_PROGRAM_DATA_CMD_CODE */

#ifdef FEATURE_EFM_ERASE_NVR_CMD_CODE
                case FEATURE_EFM_ERASE_NVR_CMD_CODE:
                    /* Only Erase one sector, finish it */
                    break;
#endif /* FEATURE_EFM_ERASE_NVR_CMD_CODE */

#ifdef FEATURE_EFM_PROGRAM_NVR_CMD_CODE
                case FEATURE_EFM_PROGRAM_NVR_CMD_CODE:
                    state->leftSize -= (int32_t)FEATURE_EFM_WRITE_UNIT_SIZE;
                    state->cmdParam.pdata = efm_data;
                    state->nvr_addr += FEATURE_EFM_WRITE_UNIT_SIZE;
                    state->nvr_data += FEATURE_EFM_WRITE_UNIT_WORD_SIZE; /* PRQA S 0488 */

                    efm_data[0] = state->nvr_addr;
                    for (uint32_t i = 0U; i < FEATURE_EFM_WRITE_UNIT_WORD_SIZE; i++)
                    {
                        efm_data[i + 1U] = state->nvr_data[i];
                    }
                    break;
#endif /* FEATURE_EFM_PROGRAM_NVR_CMD_CODE */

                default:
                    /* No more commands to execute */
                    break;
            }
            if (state->leftSize > 0)
            {
                state->shiftsum = FLASH_StateCalcShiftSum(state);
                status = FLASH_LaunchCommandSequence(instance);
                if (status != STATUS_SUCCESS)
                {
                    FLASH_EventCallback(base, state, FLASH_EVENT_ERROR);
                }
            }
            else
            {
                FLASH_EventCallback(base, state, FLASH_EVENT_COMPLETE);
            }
        }
    }
    else /* sync mode */
    {
        if (FLASH_GetErrorStatus(base))
        {
            FLASH_EventCallback(base, state, FLASH_EVENT_ERROR);
        }
        else
        {
            FLASH_EventCallback(base, state, FLASH_EVENT_COMPLETE);
        }
    }
}

/*FUNCTION**********************************************************************
*
* Function Name : FLASH_DRV_GetDoneStatus
* Description   : Get the done status of the flash module.
*
*END**************************************************************************/
bool FLASH_DRV_GetDoneStatus(uint32_t instance)
{
    DEV_ASSERT(instance < EFM_INSTANCE_COUNT);
    return FLASH_GetDoneStatus(s_efmBase[instance]);
}

/*FUNCTION**********************************************************************
*
* Function Name : FLASH_DRV_GetReadCollisionFlag
* Description   : Get the read collision flag of the flash module.
*
*END**************************************************************************/
bool FLASH_DRV_GetReadCollisionFlag(uint32_t instance)
{
    DEV_ASSERT(instance < EFM_INSTANCE_COUNT);
    return FLASH_GetReadCollisionFlag(s_efmBase[instance]);
}

#if defined(EFM_READ_COLLISION_IRQS_CH_COUNT) && (EFM_READ_COLLISION_IRQS_CH_COUNT > 0U)
/*FUNCTION**********************************************************************
*
* Function Name : FLASH_ReadCollisionIRQHandler
* Description   : Interrupt handler for flash module access error event.
*
*END**************************************************************************/
static void FLASH_ReadCollisionIRQHandler(uint32_t instance)
{
    DEV_ASSERT(instance < EFM_INSTANCE_COUNT);
    flash_state_t *state = s_FlashStatePtr[instance];
    DEV_ASSERT(state != NULL);
    EFM_Type *base = s_efmBase[instance];

    FLASH_ClearReadCollisionFlag(base);
    FLASH_EventCallback(base, state, FLASH_EVENT_ACCESS_ERROR);
}
#endif /* EFM_READ_COLLISION_IRQS_CH_COUNT */

/*FUNCTION**********************************************************************
*
* Function Name : EFM_IRQHandler
* Description   : Interrupt handler for flash module.
* This handler is used when flash driver is configured for async mode.
*
*END**************************************************************************/
#if (EFM_IRQS_CH_COUNT > 0U)
/* Implementation of EFM handler named in startup code. */
void EFM_IRQHandler(void)
{
    if (FLASH_DRV_GetDoneStatus(0))
    {
        FLASH_DoneIRQHandler(0);
    }
}
#endif

#if (EFM_IRQS_CH_COUNT > 1U)
void EFM_D_IRQHandler(void)
{
    if (FLASH_DRV_GetDoneStatus(1))
    {
        FLASH_DoneIRQHandler(1);
    }
    if (FLASH_DRV_GetReadCollisionFlag(1))
    {
        FLASH_ReadCollisionIRQHandler(1);
    }
}
#endif

#if defined(EFM_READ_COLLISION_IRQS_CH_COUNT) && (EFM_READ_COLLISION_IRQS_CH_COUNT > 0U)
/* Implementation of EFM Error handler named in startup code. */
void EFM_Error_IRQHandler(void)
{
    if (FLASH_DRV_GetReadCollisionFlag(0))
    {
        FLASH_ReadCollisionIRQHandler(0);
    }
}
#endif

/*FUNCTION**********************************************************************
*
* Function Name : FLASH_DRV_Init
* Description   : Initialize the Flash module.
*
* Implements    : FLASH_DRV_Init_Activity
*END**************************************************************************/
status_t FLASH_DRV_Init(uint32_t instance, const flash_user_config_t * userConfigPtr, flash_state_t * state)
{
    DEV_ASSERT(instance < EFM_INSTANCE_COUNT);
    DEV_ASSERT(s_FlashStatePtr[instance] == NULL);
    DEV_ASSERT(userConfigPtr != NULL);
    DEV_ASSERT(state != NULL);

    EFM_Type *base = s_efmBase[instance];
    status_t status = STATUS_SUCCESS;    /* Return code variable */
#ifdef EFM_WDG_WDG_EN_MASK
    uint32_t slowBusClockFrq;
#endif /* EFM_WDG_WDG_EN_MASK */

    if (FLASH_GetIdleStatus(base)){
        FLASH_ClearErrorFlags(base);
        FLASH_ClearDoneStatusFlag(base);
        s_FlashStatePtr[instance] = state;
        state->async = userConfigPtr->async;
        state->callback = userConfigPtr->callback;
        state->disGlobalInt = userConfigPtr->disGlobalInt;
        state->driverBusy = false;

#if defined(FEATURE_EFM_TIMING_MAX) && (FEATURE_EFM_TIMING_MAX == 1U)
        base->TIMING1 = FEATURE_EFM_TIMING1_MAX;
        base->TIMING2 = FEATURE_EFM_TIMING2_MAX;
#endif

#ifdef EFM_CTRL_CMD_VERIFY_EN_MASK
        if (true == userConfigPtr->readVerify)
        {
            base->CTRL |= EFM_CTRL_CMD_VERIFY_EN_MASK;
        }
        else
        {
            base->CTRL &= ~EFM_CTRL_CMD_VERIFY_EN_MASK;
        }
#endif
        state->readVerify = userConfigPtr->readVerify;

#ifdef EFM_WDG_WDG_EN_MASK
        if (true == userConfigPtr->wdgEnable)
        {
            /* Get slow bus clock freq for WDG module */
            status = CLOCK_SYS_GetFreq(SLOW_BUS_CLK, &slowBusClockFrq);
            if (STATUS_SUCCESS == status){
                base->WDG = EFM_WDG_WDG_PRESCALER(slowBusClockFrq / 1e6);
                base->WDG |= EFM_WDG_WDG_EN_MASK;
            }
        }
        else
        {
            base->WDG &= ~EFM_WDG_WDG_EN_MASK;
        }

#endif /* EFM_WDG_WDG_EN_MASK */

        /* Enable flash interrupt */
        if (state->async)
        {
            FLASH_DRV_EnableCmdCompleteInterrupt(instance);
            INT_SYS_EnableIRQ(s_efmIrqId[instance]);
            FLASH_DRV_EnableReadCollisionInterrupt(instance);
#ifdef EFM_READ_COLLISION_IRQS
            INT_SYS_EnableIRQ(s_efmReadCollisionIrqId[instance]);
#endif /* EFM_READ_COLLISION_IRQS */
        }

    }else{
        status = STATUS_EFM_BUSY;
    }

#if defined(FEATURE_EFM_HAS_READONLY_BIT) && (FEATURE_EFM_HAS_READONLY_BIT == 1)
    /* Read only flash array */
    EFM->CTRL |= EFM_CTRL_READONLY_MASK;
#endif
    return status;
}

/*FUNCTION**********************************************************************
*
* Function Name : FLASH_DRV_Deinit
* Description   : De-initialize the Flash module.
*
* Implements    : FLASH_DRV_Deinit_Activity
*END**************************************************************************/
status_t FLASH_DRV_Deinit(uint32_t instance)
{
    DEV_ASSERT(instance < EFM_INSTANCE_COUNT);
    EFM_Type *base = s_efmBase[instance];
    flash_state_t const *state = s_FlashStatePtr[instance];
    status_t status = STATUS_SUCCESS;    /* Return code variable */
    DEV_ASSERT(state != NULL);

    if (FLASH_GetIdleStatus(base)){
        FLASH_ClearErrorFlags(base);
        FLASH_ClearDoneStatusFlag(base);
        /* Disable flash interrupt */
        if (state->async)
        {
            FLASH_DRV_DisableCmdCompleteInterrupt(instance);
            INT_SYS_DisableIRQ(EFM_IRQn);
        }
        s_FlashStatePtr[instance] = NULL;
    }else{
        status = STATUS_EFM_BUSY;
    }
    return status;
}

/*FUNCTION**********************************************************************
*
* Function Name : FLASH_DRV_GetDefaultConfig
* Description   : Gets the default configuration structure for the Flash module.
*
* Implements    : FLASH_DRV_GetDefaultConfig_Activity
*END**************************************************************************/
void FLASH_DRV_GetDefaultConfig(flash_user_config_t * const userConfigPtr)
{
    DEV_ASSERT(userConfigPtr != NULL);

    /* Set default configuration for flash module */
    userConfigPtr->async = false;
    userConfigPtr->disGlobalInt = true;
    userConfigPtr->readVerify = true;
    userConfigPtr->callback = NULL_CALLBACK;
}

/*FUNCTION**********************************************************************
*
* Function Name : FLASH_DRV_GetBusyStatus
* Description   : Get the Flash module busy status.
*
* Implements    : FLASH_DRV_GetBusyStatus_Activity
*END**************************************************************************/
bool FLASH_DRV_GetBusyStatus(uint32_t instance)
{
    DEV_ASSERT(instance < EFM_INSTANCE_COUNT);
    flash_state_t const *state = s_FlashStatePtr[instance];
    DEV_ASSERT(state != NULL);

    return state->driverBusy;
}

/*FUNCTION**********************************************************************
*
* Function Name : FLASH_DRV_SetAsyncMode
* Description   : Enable or disable async mode.
*
* Implements    : FLASH_DRV_SetAsyncMode_Activity
*END**************************************************************************/
status_t FLASH_DRV_SetAsyncMode(uint32_t instance, bool async)
{
    DEV_ASSERT(instance < EFM_INSTANCE_COUNT);
    EFM_Type *base = s_efmBase[instance];
    flash_state_t *state = s_FlashStatePtr[instance];
    DEV_ASSERT(state != NULL);
    status_t status = STATUS_SUCCESS;

    if ((false == FLASH_GetIdleStatus(base)) || (state->driverBusy))
    {
        status = STATUS_EFM_BUSY;
    }
    else
    {
        state->async = async;
        if (true == async)
        {
            state->driverBusy = false;
            FLASH_DRV_EnableCmdCompleteInterrupt(instance);
            INT_SYS_EnableIRQ(s_efmIrqId[instance]);
            FLASH_DRV_EnableReadCollisionInterrupt(instance);
#ifdef EFM_READ_COLLISION_IRQS
            INT_SYS_EnableIRQ(s_efmReadCollisionIrqId[instance]);
#endif /* EFM_READ_COLLISION_IRQS */
        }
    }
    return status;
}

/*FUNCTION**********************************************************************
*
* Function Name : FLASH_DRV_SetDisableGlobalInt
* Description   : Enable or disable global interrupt in sync mode.
*
* Implements    : FLASH_DRV_SetDisableGlobalInt_Activity
*END**************************************************************************/
void FLASH_DRV_SetDisableGlobalInt(uint32_t instance, bool disGlobalInt)
{
    DEV_ASSERT(instance < EFM_INSTANCE_COUNT);
    flash_state_t *state = s_FlashStatePtr[instance];
    DEV_ASSERT(state != NULL);

    state->disGlobalInt = disGlobalInt;
}

/*FUNCTION**********************************************************************
*
* Function Name : FLASH_DRV_SetReadVerify
* Description   : Enable or disable read verify during program/erase operation.
*
* Implements    : FLASH_DRV_SetReadVerify_Activity
*END**************************************************************************/
void FLASH_DRV_SetReadVerify(uint32_t instance, bool readVerify)
{
    DEV_ASSERT(instance < EFM_INSTANCE_COUNT);
    flash_state_t *state = s_FlashStatePtr[instance];
    DEV_ASSERT(state != NULL);

    state->readVerify = readVerify;

#ifdef EFM_CTRL_CMD_VERIFY_EN_MASK
    EFM_Type *base = s_efmBase[instance];
    if (true == readVerify)
    {
        base->CTRL |= EFM_CTRL_CMD_VERIFY_EN_MASK;
    }
    else
    {
        base->CTRL &= ~EFM_CTRL_CMD_VERIFY_EN_MASK;
    }
#endif
}

#ifdef FEATURE_EFM_ERASE_ARRAY_CMD_CODE
/*FUNCTION**********************************************************************
 *
 * Function Name : FLASH_DRV_EraseArray
 * Description   : Erases a array of Flash memory.
 * This API always returns STATUS_SUCCESS if size provided by the user is
 * zero regardless of the input validation.
 *
 * Implements    : FLASH_DRV_EraseArray_Activity
 *END**************************************************************************/
status_t FLASH_DRV_EraseArray(uint32_t instance, uint32_t dest)
{
    DEV_ASSERT(instance < EFM_INSTANCE_COUNT);
    EFM_Type *base = s_efmBase[instance];
    flash_state_t *state = s_FlashStatePtr[instance];
    DEV_ASSERT(state != NULL);
    status_t status; /* Return code variable */
    DEV_ASSERT((dest & (FEATURE_EFM_FLASH_MIN_SECTOR_SIZE - 1)) == 0U);

    state->cmdParam.cmdCode = FEATURE_EFM_ERASE_ARRAY_CMD_CODE;
    state->cmdParam.dest = (uint32_t *)dest;
    state->cmdParam.pdata = &s_tempData;
    state->cmdParam.word_size = 1U;

    if (state->async)
    {
        if ((false == FLASH_GetIdleStatus(base)) || (state->driverBusy))
        {
            status = STATUS_EFM_BUSY;
        }
        else
        {
            state->driverBusy = true;
            state->leftSize = 0;
            state->shiftsum = FLASH_StateCalcShiftSum(state);
            status = FLASH_LaunchCommandSequence(instance);
        }
    }
    else
    {
        /* Check IDLE to verify the previous command is completed */
        if (false == FLASH_GetIdleStatus(base))
        {
            status = STATUS_EFM_BUSY;
        }
        else
        {
            /* Calling flash command sequence function to execute the command */
            state->shiftsum = FLASH_StateCalcShiftSum(state);
            status = FLASH_LaunchCommandSequence(instance);
        }
    }
    return status;
}
#endif /* FEATURE_EFM_ERASE_ARRAY_CMD_CODE */

#ifdef FEATURE_EFM_ERASE_BLOCK_CMD_CODE
/*FUNCTION**********************************************************************
 *
 * Function Name : FLASH_DRV_EraseBlock
 * Description   : Erases a block of Flash memory.
 * This API always returns STATUS_SUCCESS if size provided by the user is
 * zero regardless of the input validation.
 *
 * Implements    : FLASH_DRV_EraseBlock_Activity
 *END**************************************************************************/
status_t FLASH_DRV_EraseBlock(uint32_t instance, uint32_t dest)
{
    DEV_ASSERT(instance < EFM_INSTANCE_COUNT);
    EFM_Type *base = s_efmBase[instance];
    flash_state_t *state = s_FlashStatePtr[instance];
    DEV_ASSERT(state != NULL);
    status_t status; /* Return code variable */
    DEV_ASSERT((dest & (FEATURE_EFM_FLASH_MIN_SECTOR_SIZE - 1)) == 0U);

#if defined(FEATURE_EFM_HAS_ERASE_TIMING_UNION) && (FEATURE_EFM_HAS_ERASE_TIMING_UNION == 1U)
    base->TIMING2 = FEATURE_EFM_BLOCK_ERASE_TIMING;
#endif
    state->cmdParam.cmdCode = FEATURE_EFM_ERASE_BLOCK_CMD_CODE;
#ifdef FEATURE_EFM_ERASE_BLOCK_VERIFY_CMD_CODE
    if (true == state->readVerify)
    {
        state->cmdParam.cmdCode = FEATURE_EFM_ERASE_BLOCK_VERIFY_CMD_CODE;
    }
#endif
    state->cmdParam.dest = (uint32_t *)dest;
    state->cmdParam.pdata = &s_tempData;
    state->cmdParam.word_size = 1U;

    if (state->async)
    {
        if ((false == FLASH_GetIdleStatus(base)) || (state->driverBusy))
        {
            status = STATUS_EFM_BUSY;
        }
        else
        {
            state->driverBusy = true;
            state->leftSize = 0;
            state->shiftsum = FLASH_StateCalcShiftSum(state);
            status = FLASH_LaunchCommandSequence(instance);
        }
    }
    else
    {
        /* Check IDLE to verify the previous command is completed */
        if (false == FLASH_GetIdleStatus(base))
        {
            status = STATUS_EFM_BUSY;
        }
        else
        {
            /* Calling flash command sequence function to execute the command */
            state->shiftsum = FLASH_StateCalcShiftSum(state);
            status = FLASH_LaunchCommandSequence(instance);
        }
    }
    return status;
}
#endif /* FEATURE_EFM_ERASE_BLOCK_CMD_CODE */

/*FUNCTION**********************************************************************
 *
 * Function Name : FLASH_DRV_EraseSector
 * Description   : Erases one or more sectors in P-Flash or D-Flash memory.
 * This API always returns STATUS_SUCCESS if size provided by the user is
 * zero regardless of the input validation.
 *
 * Implements    : FLASH_DRV_EraseSector_Activity
 *END**************************************************************************/
status_t FLASH_DRV_EraseSector(uint32_t instance, uint32_t dest, uint32_t size)
{
    DEV_ASSERT(instance < EFM_INSTANCE_COUNT);
    EFM_Type *base = s_efmBase[instance];
    flash_state_t *state = s_FlashStatePtr[instance];
    DEV_ASSERT(state != NULL);
    status_t status = STATUS_SUCCESS; /* Return code variable */
    uint32_t sectorSize;              /* Size of one sector   */
    sectorSize = FLASH_GetSectorSize(dest);
    DEV_ASSERT((dest & (sectorSize - 1)) == 0U);
    DEV_ASSERT((size & (sectorSize - 1)) == 0U);
    DEV_ASSERT(size >= sectorSize);

#if defined(FEATURE_EFM_HAS_ERASE_TIMING_UNION) && (FEATURE_EFM_HAS_ERASE_TIMING_UNION == 1U)
    base->TIMING2 = FEATURE_EFM_SECTOR_ERASE_TIMING;
#endif

    state->cmdParam.cmdCode = FEATURE_EFM_ERASE_SECTOR_CMD_CODE;
#ifdef FEATURE_EFM_ERASE_SECTOR_VERIFY_CMD_CODE
    if (true == state->readVerify)
    {
        state->cmdParam.cmdCode = FEATURE_EFM_ERASE_SECTOR_VERIFY_CMD_CODE;
    }
#endif
    state->cmdParam.dest = (uint32_t *)dest;
    state->cmdParam.pdata = &s_tempData;
    state->cmdParam.word_size = 1U;
    state->leftSize = (int32_t)size;

    if (state->async)
    {
        if ((false == FLASH_GetIdleStatus(base)) || (state->driverBusy))
        {
            status = STATUS_EFM_BUSY;
        }
        else
        {
            state->driverBusy = true;
            state->shiftsum = FLASH_StateCalcShiftSum(state);
            status = FLASH_LaunchCommandSequence(instance);
        }
    }
    else
    {
        while ((state->leftSize > 0) && (STATUS_SUCCESS == status) && (sectorSize > 0U))
        {
            /* Check IDLE to verify the previous command is completed */
            if (false == FLASH_GetIdleStatus(base))
            {
                status = STATUS_EFM_BUSY;
            }
            else
            {
                /* Calling flash command sequence function to execute the command */
                state->shiftsum = FLASH_StateCalcShiftSum(state);
                status = FLASH_LaunchCommandSequence(instance);
            }
#if defined(FEATURE_EFM_SW_READ_VERIFY) && (FEATURE_EFM_SW_READ_VERIFY == 1)
            if ((status == STATUS_SUCCESS) && (true == state->readVerify))
            {
                /* Software verify for erase operation */
                status = FLASH_DRV_SoftwareVerifyMemory((uint32_t)state->cmdParam.dest, NULL, 
                                    sectorSize / sizeof(uint32_t));
            }
#endif
            state->leftSize -= (int32_t)sectorSize;
            state->cmdParam.dest += sectorSize >> 2U; /* PRQA S 0488 */
            sectorSize = FLASH_GetSectorSize((uint32_t)state->cmdParam.dest);
        }
    }

    return status;
}
#ifdef FEATURE_EFM_ERASE_SECTOR_RETRY_CMD_CODE
/*FUNCTION**********************************************************************
 *
 * Function Name : FLASH_DRV_EraseSectorQuick
 * Description   : Erases one or more sectors in P-Flash or D-Flash memory.
 * This API always returns STATUS_SUCCESS if size provided by the user is
 * zero regardless of the input validation.
 * This API is used for the case that the flash need to be erased quickly.
 *
 * Implements    : FLASH_DRV_EraseSector_Activity
 *END**************************************************************************/
status_t FLASH_DRV_EraseSectorQuick(uint32_t instance, uint32_t dest, uint32_t size)
{
    DEV_ASSERT(instance < EFM_INSTANCE_COUNT);
    EFM_Type *base = s_efmBase[instance];
    flash_state_t *state = s_FlashStatePtr[instance];
    DEV_ASSERT(state != NULL);
    status_t status = STATUS_SUCCESS; /* Return code variable */
    uint32_t sectorSize;              /* Size of one sector   */
    uint32_t i = 0;
    sectorSize = FLASH_GetSectorSize(dest);
    DEV_ASSERT((dest & (sectorSize - 1)) == 0U);
    DEV_ASSERT((size & (sectorSize - 1)) == 0U);
    DEV_ASSERT(size >= sectorSize);

#if defined(FEATURE_EFM_HAS_ERASE_TIMING_UNION) && (FEATURE_EFM_HAS_ERASE_TIMING_UNION == 1U)
    base->TIMING2 = FEATURE_EFM_SECTOR_ERASE_RETRY_TIMING;
#endif

#ifdef EFM_CTRL_CMD_VERIFY_EN_MASK
#ifdef FEATURE_EFM_ERASE_SECTOR_VREAD_RETRY_CMD_CODE
    base->CTRL &= ~EFM_CTRL_CMD_VERIFY_EN_MASK;
#else
    base->CTRL |= EFM_CTRL_CMD_VERIFY_EN_MASK;
#endif
#endif
    state->cmdParam.cmdCode = FEATURE_EFM_ERASE_SECTOR_RETRY_CMD_CODE;
    state->cmdParam.dest = (uint32_t *)dest;
    state->cmdParam.pdata = &s_tempData;
    state->cmdParam.word_size = 1U;
    state->leftSize = (int32_t)size;

    if (state->async)
    {
        if ((false == FLASH_GetIdleStatus(base)) || (state->driverBusy))
        {
            status = STATUS_EFM_BUSY;
        }
        else
        {
            state->driverBusy = true;
            state->retryCount = 0U;
#ifdef EFM_CTRL_RETRY_CFG_MASK
            base->CTRL &= ~EFM_CTRL_RETRY_CFG_MASK;
            base->CTRL |= EFM_CTRL_RETRY_CFG(FLASH_RETRY_CFG_START);
#endif /* EFM_CTRL_RETRY_CFG_MASK */
            state->shiftsum = FLASH_StateCalcShiftSum(state);
            status = FLASH_LaunchCommandSequence(instance);
        }
    }
    else
    {
        while ((state->leftSize > 0) && (STATUS_SUCCESS == status) && (sectorSize > 0U))
        {
            /* Check IDLE to verify the previous command is completed */
            if (false == FLASH_GetIdleStatus(base))
            {
                status = STATUS_EFM_BUSY;
            }
            else
            {
                for (i = 0; i < FLASH_RETRY_MAX_COUNT; i++)
                {
#if defined(EFM_CTRL_RETRY_CFG_MASK)
                    if ((FLASH_RETRY_CFG_START + i) <= 0x7U)
                    {
                        base->CTRL &= ~EFM_CTRL_RETRY_CFG_MASK;
                        base->CTRL |= EFM_CTRL_RETRY_CFG(FLASH_RETRY_CFG_START + i);
                    }
#endif /* EFM_CTRL_RETRY_CFG_MASK */
                    state->cmdParam.cmdCode = FEATURE_EFM_ERASE_SECTOR_RETRY_CMD_CODE;
                    state->shiftsum = FLASH_StateCalcShiftSum(state);
                    status = FLASH_LaunchCommandSequence(instance);
                    if (STATUS_SUCCESS == status)
                    {
#ifdef FEATURE_EFM_ERASE_SECTOR_VREAD_RETRY_CMD_CODE
                        state->cmdParam.cmdCode = FEATURE_EFM_ERASE_SECTOR_VREAD_RETRY_CMD_CODE;
                        state->shiftsum = FLASH_StateCalcShiftSum(state);
                        status = FLASH_LaunchCommandSequence(instance);
                        if (STATUS_SUCCESS != status)
                        {
                            continue;
                        }
#endif /* FEATURE_EFM_ERASE_SECTOR_VREAD_RETRY_CMD_CODE */
                        break;
                    }
                }
                if (i >= FLASH_RETRY_MAX_COUNT)
                {
                    status = STATUS_ERROR;
                }
                else
                /* Append 0x7 RETRY_CFG retry after finish to ensure erase thoroughly */
                {
#if defined(EFM_CTRL_RETRY_CFG_MASK)
                    base->CTRL &= ~EFM_CTRL_RETRY_CFG_MASK;
                    base->CTRL |= EFM_CTRL_RETRY_CFG(0x7);
#endif /* EFM_CTRL_RETRY_CFG_MASK */
                    state->cmdParam.cmdCode = FEATURE_EFM_ERASE_SECTOR_RETRY_CMD_CODE;
                    state->shiftsum = FLASH_StateCalcShiftSum(state);
                    status = FLASH_LaunchCommandSequence(instance);
                    if (STATUS_SUCCESS == status)
                    {
#ifdef FEATURE_EFM_ERASE_SECTOR_VREAD_RETRY_CMD_CODE
                        state->cmdParam.cmdCode = FEATURE_EFM_ERASE_SECTOR_VREAD_RETRY_CMD_CODE;
                        state->shiftsum = FLASH_StateCalcShiftSum(state);
                        status = FLASH_LaunchCommandSequence(instance);
#endif /* FEATURE_EFM_ERASE_SECTOR_VREAD_RETRY_CMD_CODE */
                    }
                    else
                    {
                        status = STATUS_ERROR;
                    }
                }
            }

            state->leftSize -= (int32_t)sectorSize;
            state->cmdParam.dest += sectorSize >> 2U; /* PRQA S 0488 */
            sectorSize = FLASH_GetSectorSize((uint32_t)state->cmdParam.dest);
        }
#ifdef EFM_CTRL_CMD_VERIFY_EN_MASK
        if (true == state->readVerify)
        {
            base->CTRL |= EFM_CTRL_CMD_VERIFY_EN_MASK;
        }
        else
        {
            base->CTRL &= ~EFM_CTRL_CMD_VERIFY_EN_MASK;
        }
#endif
    }

    return status;
}
#endif /* FEATURE_EFM_ERASE_SECTOR_RETRY_CMD_CODE */

/*FUNCTION**********************************************************************
 *
 * Function Name : FLASH_DRV_Program
 * Description   : Program command on flash
 * This API always returns STATUS_SUCCESS if size provided by user is
 * zero regardless of the input validation.
 *
 * Implements    : FLASH_DRV_Program_Activity
 *END**************************************************************************/
status_t FLASH_DRV_Program(uint32_t instance, uint32_t dest, uint32_t size, const void * pData)
{
    DEV_ASSERT(instance < EFM_INSTANCE_COUNT);
    EFM_Type *base = s_efmBase[instance];
    flash_state_t *state = s_FlashStatePtr[instance];
    DEV_ASSERT(state != NULL);
    status_t status = STATUS_SUCCESS;    /* Return code variable */

    DEV_ASSERT((dest & (FEATURE_EFM_WRITE_UNIT_SIZE - 1U)) == 0U);
    DEV_ASSERT((size & (FEATURE_EFM_WRITE_UNIT_SIZE - 1U)) == 0U);
    DEV_ASSERT(size >= FEATURE_EFM_WRITE_UNIT_SIZE);
    DEV_ASSERT(((uint32_t)pData & 0x03U) == 0U);

    state->cmdParam.cmdCode = FEATURE_EFM_PROGRAM_CMD_CODE;
#ifdef FEATURE_EFM_PROGRAM_VERIFY_CMD_CODE
    if (true == state->readVerify)
    {
        state->cmdParam.cmdCode = FEATURE_EFM_PROGRAM_VERIFY_CMD_CODE;
    }
#endif
    state->cmdParam.dest = (uint32_t *) dest;
    state->cmdParam.pdata = (const uint32_t *) pData; /* PRQA S 0316 */
    state->cmdParam.word_size = FEATURE_EFM_WRITE_UNIT_WORD_SIZE;
    state->leftSize = (int32_t)size;
#if defined(FEATURE_EFM_PREREAD_CHECK_FOR_PROGRAM) && (FEATURE_EFM_PREREAD_CHECK_FOR_PROGRAM == 1)
    /* Pre-check before program, Check if destination flash area is erased */
    status = FLASH_DRV_SoftwareVerifyMemory(dest, NULL, size / sizeof(uint32_t));
#endif
    if (state->async)
    {
        if ((false == FLASH_GetIdleStatus(base)) || (state->driverBusy))
        {
            status = STATUS_EFM_BUSY;
        }
        else
        {
            state->driverBusy = true;
            state->shiftsum = FLASH_StateCalcShiftSum(state);
            status = FLASH_LaunchCommandSequence(instance);
        }
    }
    else
    {
        while ((state->leftSize > 0) && (STATUS_SUCCESS == status))
        {
            /* Check IDLE to verify the previous command is completed */
            if (false == FLASH_GetIdleStatus(base))
            {
                status = STATUS_EFM_BUSY;
            }
            else
            {
                /* Calling flash command sequence function to execute the command */
                state->shiftsum = FLASH_StateCalcShiftSum(state);
                status = FLASH_LaunchCommandSequence(instance);
#if defined(FEATURE_EFM_SW_READ_VERIFY) && (FEATURE_EFM_SW_READ_VERIFY == 1)
                if ((status == STATUS_SUCCESS) && (true == state->readVerify))
                {
                    /* Software verify for program operation */
                    status = FLASH_DRV_SoftwareVerifyMemory((uint32_t)state->cmdParam.dest, 
                                (volatile const uint32_t *)(state->cmdParam.pdata), state->cmdParam.word_size);
                }
#endif
                /* Update destination address for next iteration */
                state->cmdParam.dest += FEATURE_EFM_WRITE_UNIT_WORD_SIZE; /* PRQA S 0488 */
                /* Update size for next iteration */
                state->leftSize -= (int32_t)FEATURE_EFM_WRITE_UNIT_SIZE;
                /* Increment the source address by unit word size */
                state->cmdParam.pdata += FEATURE_EFM_WRITE_UNIT_WORD_SIZE; /* PRQA S 0488 */
            }
        }
    }

    return status;
}

#ifdef FEATURE_EFM_PROGRAM_DATA_CMD_CODE
/*FUNCTION**********************************************************************
 *
 * Function Name : FLASH_DRV_ProgramDFlash
 * Description   : Program command on DFlash
 * This API always returns STATUS_SUCCESS if size provided by user is
 * zero regardless of the input validation.
 *
 * Implements    : FLASH_DRV_Program_Activity
 *END**************************************************************************/
status_t FLASH_DRV_ProgramDFlash(uint32_t instance, uint32_t dest, uint32_t size, const void * pData)
{
    DEV_ASSERT(instance < EFM_INSTANCE_COUNT);
    EFM_Type *base = s_efmBase[instance];
    flash_state_t *state = s_FlashStatePtr[instance];
    DEV_ASSERT(state != NULL);
    status_t status = STATUS_SUCCESS;    /* Return code variable */

    DEV_ASSERT((dest & (FEATURE_EFM_WRITE_DATA_UNIT_SIZE - 1)) == 0U);
    DEV_ASSERT(((uint32_t)pData & 0x03U) == 0U);
    DEV_ASSERT((size & (FEATURE_EFM_WRITE_DATA_UNIT_SIZE - 1)) == 0U);
    DEV_ASSERT(size >= FEATURE_EFM_WRITE_DATA_UNIT_SIZE);

    state->cmdParam.cmdCode = FEATURE_EFM_PROGRAM_DATA_CMD_CODE;
    state->cmdParam.dest = (uint32_t *) dest;
    state->cmdParam.pdata = (const uint32_t *) pData; /* PRQA S 0316 */
    state->cmdParam.word_size = FEATURE_EFM_WRITE_DATA_UNIT_WORD_SIZE;
    state->leftSize = (int32_t)size;



    if (state->async)
    {
        if ((false == FLASH_GetIdleStatus(base)) || (state->driverBusy))
        {
            status = STATUS_EFM_BUSY;
        }
        else
        {
            state->driverBusy = true;
            state->shiftsum = FLASH_StateCalcShiftSum(state);
            status = FLASH_LaunchCommandSequence(instance);
        }
    }
    else
    {
        while ((state->leftSize > 0) && (STATUS_SUCCESS == status))
        {
            /* Check IDLE to verify the previous command is completed */
            if (false == FLASH_GetIdleStatus(base))
            {
                status = STATUS_EFM_BUSY;
            }
            else
            {
                /* Calling flash command sequence function to execute the command */
                state->shiftsum = FLASH_StateCalcShiftSum(state);
                status = FLASH_LaunchCommandSequence(instance);

                /* Update destination address for next iteration */
                state->cmdParam.dest += FEATURE_EFM_WRITE_DATA_UNIT_WORD_SIZE; /* PRQA S 0488 */
                /* Update size for next iteration */
                state->leftSize -= (int32_t)FEATURE_EFM_WRITE_DATA_UNIT_SIZE;
                /* Increment the source address by unit word size */
                state->cmdParam.pdata += FEATURE_EFM_WRITE_DATA_UNIT_WORD_SIZE; /* PRQA S 0488 */
            }
        }
    }

    return status;
}
#endif /* FEATURE_EFM_WRITE_DATA_UNIT_SIZE */


/*FUNCTION**********************************************************************
 *
 * Function Name : FLASH_DRV_CheckSum
 * Description   : Performs 32 bit sum of each byte data over a specified Flash
 * memory range without carry which provides rapid method for checking data integrity.
 * The callback time period of this API is determined via FLASH_CALLBACK_CS macro in the
 * flash_driver.h which is used as a counter value for the CallBack() function calling in
 * this API. This value can be changed as per the user requirement. User can change this value
 * to obtain the maximum permissible callback time period.
 * This API always returns STATUS_SUCCESS if size provided by user is zero regardless of the input
 * validation.
 *
 * Implements    : FLASH_DRV_CheckSum_Activity
 *END**************************************************************************/
status_t FLASH_DRV_CheckSum(uint32_t instance, uint32_t dest, uint32_t size, uint32_t * pSum)
{
    DEV_ASSERT(instance < EFM_INSTANCE_COUNT);
    EFM_Type *base = s_efmBase[instance];
    flash_state_t *state = s_FlashStatePtr[instance];
    DEV_ASSERT(state != NULL);
    DEV_ASSERT(pSum != NULL);
    status_t status = STATUS_SUCCESS;      /* Return code variable           */
    uint32_t counter = 0U;              /* Counter for callback operation */
    uint32_t data;                      /* Data read from Flash address   */
    uint32_t tempSize = size;           /* Temporary of size variation    */
    uint32_t destAddr = dest;           /* Temporary of destination address */

    *pSum = 0U;
    /* Doing sum operation */
    while (tempSize > 0U)
    {
        data = *(uint8_t *)(destAddr);
        *pSum += data;
        destAddr += 1U;
        tempSize -= 1U;
        ++counter;

        /* Check if flash need to serve callback function */
        if (counter >= FLASH_CALLBACK_CS)
        {
            /* Serve callback function if counter reaches limitation */
            FLASH_EventCallback(base, state, FLASH_EVENT_CHECKSUM);

            /* Reset counter */
            counter = 0U;
        }
    }

    return status;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLASH_DRV_EnableCmdCompleteInterrupt
 * Description   : Enable the command complete interrupt is generated when
 * an EFM command completes.
 *
 * Implements    : FLASH_DRV_EnableCmdCompleteInterrupt_Activity
 *END**************************************************************************/
void FLASH_DRV_EnableCmdCompleteInterrupt(uint32_t instance)
{
    DEV_ASSERT(instance < EFM_INSTANCE_COUNT);
    EFM_Type *base = s_efmBase[instance];

    /* Enable the command complete interrupt */
    base->CTRL |= EFM_CTRL_DONEIE_MASK;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLASH_DRV_DisableCmdCompleteInterrupt
 * Description   : Disable the command complete interrupt.
 *
 * Implements    : FLASH_DRV_DisableCmdCompleteInterrupt_Activity
 *END**************************************************************************/
void FLASH_DRV_DisableCmdCompleteInterrupt(uint32_t instance)
{
    DEV_ASSERT(instance < EFM_INSTANCE_COUNT);
    EFM_Type *base = s_efmBase[instance];

    /* Disable the command complete interrupt */
    base->CTRL &= ~EFM_CTRL_DONEIE_MASK;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLASH_DRV_EnableReadCollisionInterrupt
 * Description   : Enable the read collision error interrupt generation when an
 * EFM read collision error occurs.
 *
 * Implements    : FLASH_DRV_EnableReadCollisionInterrupt_Activity
 *END**************************************************************************/
void FLASH_DRV_EnableReadCollisionInterrupt(uint32_t instance)
{
    DEV_ASSERT(instance < EFM_INSTANCE_COUNT);
    EFM_Type *base = s_efmBase[instance];

    /* Enable the read collision error interrupt */
    base->CTRL |= EFM_CTRL_ACCERRIE_MASK;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLASH_DRV_DisableReadCollisionInterrupt
 * Description   : Disable the read collision error interrupt
 *
 * Implements    : FLASH_DRV_DisableReadCollisionInterrupt_Activity
 *END**************************************************************************/
void FLASH_DRV_DisableReadCollisionInterrupt(uint32_t instance)
{
    DEV_ASSERT(instance < EFM_INSTANCE_COUNT);
    EFM_Type *base = s_efmBase[instance];

    /* Disable the read collision error interrupt */
    base->CTRL &= ~EFM_CTRL_ACCERRIE_MASK;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLASH_DRV_EnableSingleBitFaultInterrupt
 * Description   : Enable the platform Flash single bit fault detect interrupt 
 * generation when an recovery ECC fault is detected during a valid flash 
 * read access from the platform flash controller.
 *
 * Implements    : FLASH_DRV_EnableSingleBitFaultInterrupt
 *END**************************************************************************/
void FLASH_DRV_EnableSingleBitFaultInterrupt(uint32_t instance)
{
    DEV_ASSERT(instance < EFM_INSTANCE_COUNT);
    EFM_Type *base = s_efmBase[instance];

    /* Enable the single bit fault detect interrupt */
    base->CTRL |= EFM_CTRL_RECOVERRIE_MASK;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLASH_DRV_DisableSingleBitFaultInterrupt
 * Description   : Disable the platform Flash single bit fault detect interrupt
 *
 * Implements    : FLASH_DRV_DisableSingleBitFaultInterrupt
 *END**************************************************************************/
void FLASH_DRV_DisableSingleBitFaultInterrupt(uint32_t instance)
{
    DEV_ASSERT(instance < EFM_INSTANCE_COUNT);
    EFM_Type *base = s_efmBase[instance];

    /* Disable the single bit fault detect interrupt */
    base->CTRL &= ~EFM_CTRL_RECOVERRIE_MASK;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLASH_DRV_EnableDoubleBitFaultInterrupt
 * Description   : Enable the platform Flash double bit fault detect interrupt 
 * generation when an uncorrectable ECC fault is detected during a valid flash 
 * read access from the platform flash controller.
 *
 * Implements    : FLASH_DRV_EnableDoubleBitFaultInterrupt_Activity
 *END**************************************************************************/
void FLASH_DRV_EnableDoubleBitFaultInterrupt(uint32_t instance)
{
    DEV_ASSERT(instance < EFM_INSTANCE_COUNT);
    EFM_Type *base = s_efmBase[instance];

    /* Enable the double bit fault detect interrupt */
    base->CTRL |= EFM_CTRL_UNRECOVERRIE_MASK;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLASH_DRV_DisableDoubleBitFaultInterrupt
 * Description   : Disable the platform Flash double bit fault detect interrupt
 *
 * Implements    : FLASH_DRV_DisableDoubleBitFaultInterrupt_Activity
 *END**************************************************************************/
void FLASH_DRV_DisableDoubleBitFaultInterrupt(uint32_t instance)
{
    DEV_ASSERT(instance < EFM_INSTANCE_COUNT);
    EFM_Type *base = s_efmBase[instance];
    
    /* Disable the double bit fault detect interrupt */
    base->CTRL &= ~EFM_CTRL_UNRECOVERRIE_MASK;
}

#ifdef FEATURE_EFM_ERASE_NVR_CMD_CODE
/*FUNCTION**********************************************************************
 *
 * Function Name : FLASH_DRV_EraseNVR
 * Description   : Erase NVR one sector on flash
 * This API always returns STATUS_SUCCESS even if the address passed 
 * in is incorrect.
 *
 * Implements    : FLASH_DRV_EraseNVR_Activity
 *END**************************************************************************/
status_t FLASH_DRV_EraseNVR(uint32_t instance, uint32_t dest)
{
    DEV_ASSERT(instance < EFM_INSTANCE_COUNT);
    EFM_Type *base = s_efmBase[instance];
    flash_state_t *state = s_FlashStatePtr[instance];
    DEV_ASSERT(state != NULL);
    status_t status;    /* Return code variable */

    s_tempData = dest;
    state->cmdParam.cmdCode = FEATURE_EFM_ERASE_NVR_CMD_CODE;
    state->cmdParam.dest = &base->NVR_ADDR;
    state->cmdParam.pdata = &s_tempData;
    state->cmdParam.word_size = 1U;

    if (state->async)
    {
        if ((false == FLASH_GetIdleStatus(base)) || (state->driverBusy))
        {
            status = STATUS_EFM_BUSY;
        }
        else
        {
            state->driverBusy = true;
            state->leftSize = 0;
            state->shiftsum = FLASH_StateCalcShiftSum(state);
            status = FLASH_LaunchCommandSequence(instance);
        }
    }
    else
    {
        if (false == FLASH_GetIdleStatus(base))
        {
            status = STATUS_EFM_BUSY;
        }
        else
        {
            state->shiftsum = FLASH_StateCalcShiftSum(state);
            status = FLASH_LaunchCommandSequence(instance);
        }
    }

    return status;
}
#endif /* FEATURE_EFM_ERASE_NVR_CMD_CODE */

#ifdef FEATURE_EFM_PROGRAM_NVR_CMD_CODE
/*FUNCTION**********************************************************************
 *
 * Function Name : FLASH_DRV_Program_NVR
 * Description   : Program NVR command on flash
 * This API always returns STATUS_SUCCESS if size provided by user is
 * zero regardless of the input validation.
 *
 * Implements    : FLASH_DRV_ProgramNVR_Activity
 *END**************************************************************************/
status_t FLASH_DRV_ProgramNVR(uint32_t instance, uint32_t dest, uint32_t size, const void *pData)
{
    DEV_ASSERT(instance < EFM_INSTANCE_COUNT);
    EFM_Type *base = s_efmBase[instance];
    flash_state_t *state = s_FlashStatePtr[instance];
    DEV_ASSERT(state != NULL);
    status_t status = STATUS_SUCCESS; /* Return code variable */

    DEV_ASSERT((dest & (FEATURE_EFM_WRITE_UNIT_SIZE - 1U)) == 0U);
    DEV_ASSERT((size & (FEATURE_EFM_WRITE_UNIT_SIZE - 1U)) == 0U);
    DEV_ASSERT(size > FEATURE_EFM_WRITE_UNIT_SIZE);
    DEV_ASSERT(((uint32_t)pData & 0x03U) == 0U);

    uint32_t efm_data[FEATURE_EFM_WRITE_UNIT_WORD_SIZE + 1U];

    state->cmdParam.cmdCode = FEATURE_EFM_PROGRAM_NVR_CMD_CODE;
    state->cmdParam.dest = &base->NVR_ADDR;
    state->cmdParam.pdata = (uint32_t *)efm_data;
    state->cmdParam.word_size = FEATURE_EFM_WRITE_UNIT_WORD_SIZE + 1U;
    state->leftSize = (int32_t)size;
    state->nvr_addr = dest;
    state->nvr_data = (uint32_t *)pData; /* PRQA S 0311 */ /* PRQA S 0316 */

    if (state->async)
    {
        if ((false == FLASH_GetIdleStatus(base)) || (state->driverBusy))
        {
            status = STATUS_EFM_BUSY;
        }
        else
        {
            state->driverBusy = true;
            efm_data[0] = state->nvr_addr;
            for (uint32_t i = 0U; i < FEATURE_EFM_WRITE_UNIT_WORD_SIZE; i++)
            {
                efm_data[i + 1U] = state->nvr_data[i];
            }
            state->shiftsum = FLASH_StateCalcShiftSum(state);
            status = FLASH_LaunchCommandSequence(instance);
        }
    }
    else
    {
        while ((state->leftSize > 0) && (STATUS_SUCCESS == status))
        {
            /* Check IDLE to verify the previous command is completed */
            if (false == FLASH_GetIdleStatus(base))
            {
                status = STATUS_EFM_BUSY;
            }
            else
            {
                efm_data[0] = state->nvr_addr;
                for (uint8_t i = 0U; i < FEATURE_EFM_WRITE_UNIT_WORD_SIZE; i++)
                {
                    efm_data[i + 1U] = state->nvr_data[i];
                }
                /* Calling flash command sequence function to execute the command */
                state->shiftsum = FLASH_StateCalcShiftSum(state);
                status = FLASH_LaunchCommandSequence(instance);

                /* Update size for next iteration */
                state->leftSize -= (int32_t)FEATURE_EFM_WRITE_UNIT_SIZE;
                /* Update address for next iteration */
                state->nvr_addr += FEATURE_EFM_WRITE_UNIT_SIZE;
                /* Increment the source address by unit word size */
                state->nvr_data += FEATURE_EFM_WRITE_UNIT_WORD_SIZE; /* PRQA S 0488 */
            }
        }
    }
    return status;
}
#endif /* FEATURE_EFM_PROGRAM_NVR_CMD_CODE */

#ifdef FEATURE_EFM_READ_NVR_CMD_CODE
/*FUNCTION**********************************************************************
 *
 * Function Name : FLASH_DRV_ReadNVR
 * Description   : Read NVR command on flash
 * This API always returns STATUS_SUCCESS if size provided by user is
 * zero regardless of the input validation.
 *
 * Implements    : FLASH_DRV_ReadNVR_Activity
 *END**************************************************************************/
status_t FLASH_DRV_ReadNVR(uint32_t instance, uint32_t address, uint32_t size, void * dest)
{
    DEV_ASSERT(instance < EFM_INSTANCE_COUNT);
    EFM_Type *base = s_efmBase[instance];
    flash_state_t *state = s_FlashStatePtr[instance];
    DEV_ASSERT(state != NULL);
    status_t status = STATUS_SUCCESS;    /* Return code variable */

    DEV_ASSERT((address & (FEATURE_EFM_WRITE_UNIT_SIZE - 1U)) == 0U);
    DEV_ASSERT((size & (FEATURE_EFM_WRITE_UNIT_SIZE - 1U)) == 0U);
    DEV_ASSERT(size >= FEATURE_EFM_WRITE_UNIT_SIZE);

    bool pre_state = state->async;

    s_tempData = address;
    state->cmdParam.cmdCode = FEATURE_EFM_READ_NVR_CMD_CODE;
    state->cmdParam.dest = &base->NVR_ADDR ;
    state->cmdParam.pdata = &s_tempData;
    state->cmdParam.word_size = 1U;
    state->leftSize = (int32_t)size;
    state->nvr_addr = address;
    state->nvr_data = (uint32_t *)dest; /* PRQA S 0316 */

    /* Read the NVR synchronously, due to very short time */
    state->async = false;
    base->CTRL &= ~EFM_CTRL_DONEIE_MASK;
    while ((state->leftSize > 0) && (STATUS_SUCCESS == status))
    {
        /* Check IDLE to verify the previous command is completed */
        if (false == FLASH_GetIdleStatus(base))
        {
            status = STATUS_EFM_BUSY;
        }
        else
        {
            /* Calling flash command sequence function to execute the command */
            state->shiftsum = FLASH_StateCalcShiftSum(state);
            status = FLASH_LaunchCommandSequence(instance);
            if (STATUS_SUCCESS != status){
                break;
            }
            /* Passing parameter to the command */
            for (uint32_t i = 0U; i < FEATURE_EFM_WRITE_UNIT_WORD_SIZE; i++)
            {
                state->nvr_data[i] = base->NVR_DATA[i];
            }

            /* Update size for next iteration */
            state->leftSize -= (int32_t)FEATURE_EFM_WRITE_UNIT_SIZE;
            /* Update address for next iteration */
            state->nvr_addr += FEATURE_EFM_WRITE_UNIT_SIZE;
            /* Increment the source address by unit word size */
            state->nvr_data += FEATURE_EFM_WRITE_UNIT_WORD_SIZE; /* PRQA S 0488 */
        }
    }
    state->async = pre_state;
    if (state->async == true)
    {
        base->CTRL |= EFM_CTRL_DONEIE_MASK;
    }
    return status;
}
#endif /* FEATURE_EFM_READ_NVR_CMD_CODE */

#ifdef FEATURE_EFM_BOOT_SWAP_CMD_CODE
/*!
 * @brief Internal flash command to execute bootswap.
 *        It is called by FLASH_DRV_BootSwap.
 *
 * @param[in] instance Target flash instance.
 */
static status_t FLASH_BootSwap(uint32_t instance)
{
    DEV_ASSERT(instance < EFM_INSTANCE_COUNT);
    EFM_Type *base = s_efmBase[instance];
    flash_state_t *state = s_FlashStatePtr[instance];
    DEV_ASSERT(state != NULL);
    status_t status;    /* Return code variable */

    /* Check IDLE to verify the previous command is completed */
    if (false == FLASH_GetIdleStatus(base))
    {
        status = STATUS_EFM_BUSY;
    }
    else
    {
        state->cmdParam.cmdCode = FEATURE_EFM_BOOT_SWAP_CMD_CODE;
        state->cmdParam.pdata = &s_tempData;
        state->cmdParam.word_size = 0U;
#if defined(FEATURE_EFM_BOOT_SWAP_TAG_ADDR)
        state->cmdParam.dest = (uint32_t *)FEATURE_EFM_BOOT_SWAP_TAG_ADDR;
        state->cmdParam.word_size = 1U;
#endif
        /* Calling flash command sequence function to execute the command */
        state->shiftsum = FLASH_StateCalcShiftSum(state);
        status = FLASH_LaunchCommandSequence(instance);
    }
    return status;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLASH_DRV_BootSwap
 * Description   : Swap MCU boot flash blocks
 *
 * Implements    : FLASH_DRV_BootSwap_Activity
 *END**************************************************************************/
status_t FLASH_DRV_BootSwap(uint32_t instance)
{
    status_t status = STATUS_SUCCESS;    /* Return code variable */
    EFM_Type *base = s_efmBase[instance];
    flash_state_t *state = s_FlashStatePtr[instance];
    bool pre_state = state->async;

    INT_SYS_DisableIRQGlobal();
    /* Boot swap only synchronously */
    state->async = false;
    base->CTRL &= ~EFM_CTRL_DONEIE_MASK;

    status = FLASH_BootSwap(instance);
    if (STATUS_SUCCESS != status)
    {
        /* Erase Boot NVR and try again */
#ifdef EFM_BOOT_SWAP_UNLOCK
        EFM_BOOT_SWAP_UNLOCK();
#endif /* EFM_BOOT_SWAP_UNLOCK */

#ifdef FEATURE_EFM_ERASE_NVR_CMD_CODE
        status = FLASH_DRV_EraseNVR(instance, FEATURE_EFM_BOOT_NVR_ADDR);
#else
        status = FLASH_DRV_EraseSector(instance, FEATURE_EFM_BOOT_SWAP_TAG_ADDR, FEATURE_EFM_NVR_ARRAY_SECTOR_SIZE);
#endif /* FEATURE_EFM_ERASE_NVR_CMD_CODE */

#ifdef EFM_BOOT_SWAP_LOCK
        EFM_BOOT_SWAP_LOCK();
#endif /* EFM_BOOT_SWAP_LOCK */

        if (STATUS_SUCCESS == status)
        {
            status = FLASH_BootSwap(instance);
        }   
    }

    state->async = pre_state;
    if (state->async == true)
    {
        base->CTRL |= EFM_CTRL_DONEIE_MASK;
    }
    INT_SYS_EnableIRQGlobal();
    return status;
}
#endif

#ifdef FEATURE_EFM_LOAD_AES_KEY_CMD_CODE
/*FUNCTION**********************************************************************
 *
 * Function Name : FLASH_DRV_LoadAESKey
 * Description   : Load AES key for HCU
 *
 * Implements    : FLASH_DRV_LoadAESKey_Activity
 *END**************************************************************************/
status_t FLASH_DRV_LoadAESKey(uint32_t instance, uint32_t address)
{
    DEV_ASSERT(instance < EFM_INSTANCE_COUNT);
    EFM_Type *base = s_efmBase[instance];
    flash_state_t *state = s_FlashStatePtr[instance];
    DEV_ASSERT(state != NULL);
    bool pre_state = state->async;
    status_t status;    /* Return code variable */

    /* Check address if align to FEATURE_EFM_AES_KEY_SIZE bytes */
    if (0U != (address % FEATURE_EFM_AES_KEY_SIZE))
    {
        status = STATUS_ERROR;
    }
    /* Check IDLE to verify the previous command is completed */
    else if (false == FLASH_GetIdleStatus(base))
    {
        status = STATUS_EFM_BUSY;
    }
    else
    {
        state->cmdParam.cmdCode = FEATURE_EFM_LOAD_AES_KEY_CMD_CODE;
#if defined(CPU_YTM32B1ME0)
        base->CTRL &= ~EFM_CTRL_AES_KEY_SEL_MASK;
        base->CTRL |= EFM_CTRL_AES_KEY_SEL(((uint32_t)(address) - 0x10000000U) / 0x20U);
        state->cmdParam.word_size = 0U;
#elif defined(CPU_YTM32B1MD1) || defined(CPU_PTM32B1MD2)
        s_tempData = address;
        state->cmdParam.dest = &base->NVR_ADDR;
        state->cmdParam.pdata = &s_tempData;
        state->cmdParam.word_size = 1U;
#else
        state->cmdParam.dest = (uint32_t *)address;
        state->cmdParam.pdata = &s_tempData;
        state->cmdParam.word_size = 1U;
#endif
        /* Load AES key synchronously, due to very short time */
        state->async = false;
        base->CTRL &= ~EFM_CTRL_DONEIE_MASK;
        /* Calling flash command sequence function to execute the command */
        state->shiftsum = FLASH_StateCalcShiftSum(state);
        status = FLASH_LaunchCommandSequence(instance);
        state->async = pre_state;
        if (state->async == true)
        {
            base->CTRL |= EFM_CTRL_DONEIE_MASK;
        }
    }
    return status;
}
#endif /* FEATURE_EFM_LOAD_AES_KEY_CMD_CODE */

#ifdef FEATURE_EFM_LOAD_RSA_KEY_CMD_CODE
/*FUNCTION**********************************************************************
 *
 * Function Name : FLASH_DRV_LoadRSAKey
 * Description   : Load RSA key for HCU
 *
 * Implements    : FLASH_DRV_LoadRSAKey_Activity
 *END**************************************************************************/
status_t FLASH_DRV_LoadRSAKey(uint32_t instance, uint32_t address, uint8_t keyLen)
{
    DEV_ASSERT(instance < EFM_INSTANCE_COUNT);
    EFM_Type *base = s_efmBase[instance];
    flash_state_t *state = s_FlashStatePtr[instance];
    DEV_ASSERT(state != NULL);
    bool pre_state = state->async;
    status_t status;    /* Return code variable */

    /* Check address if align to FEATURE_EFM_RSA_KEY_SIZE bytes */
    if (0U != (address % FEATURE_EFM_RSA_KEY_SIZE))
    {
        status = STATUS_ERROR;
    }
    else
    {
        /* Check IDLE to verify the previous command is completed */
        if (false == FLASH_GetIdleStatus(base))
        {
            status = STATUS_EFM_BUSY;
        }
        else
        {
            state->cmdParam.cmdCode = FEATURE_EFM_LOAD_RSA_KEY_CMD_CODE;
            state->cmdParam.dest = (uint32_t *)address;
            state->cmdParam.pdata = &s_tempData;
            state->cmdParam.word_size = 1U;
    
            /* Set RSA key length */
            base->RSA_KEY_CFG = keyLen;
            /* Load AES key synchronously, due to very short time */
            state->async = false;
            base->CTRL &= ~EFM_CTRL_DONEIE_MASK;
            /* Calling flash command sequence function to execute the command */
            state->shiftsum = FLASH_StateCalcShiftSum(state);
            status = FLASH_LaunchCommandSequence(instance);
            state->async = pre_state;
            if (state->async == true)
            {
                base->CTRL |= EFM_CTRL_DONEIE_MASK;
            }
        }
    }
    return status;
}
#endif /* FEATURE_EFM_LOAD_RSA_KEY_CMD_CODE */

/*******************************************************************************
* EOF
*******************************************************************************/
