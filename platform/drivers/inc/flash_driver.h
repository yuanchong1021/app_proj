/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file flash_driver.h
 * @version 1.4.0
 */

#ifndef FLASH_DRIVER_H
#define FLASH_DRIVER_H

#include <stdbool.h>
#include <stddef.h>
#include "status.h"
#include "device_registers.h"

/*!
 * @defgroup flash_driver Flash Memory (Flash)
 * @ingroup flash
 * @details This section describes the programming interface of the Flash Peripheral Driver.
 * @{
 */

/*! @brief Max Count for Flash Erase Retry */
#define FLASH_RETRY_MAX_COUNT      (10U)

/*! @brief Start Number of Flash Erase Retry Configuration
*   @details This macro defines the start number of EFM_CTRL_RETRY_CFG.
*   When an erase error occurs, retry cfg will automatically increase by 1 until the value reaches 7.
*   The minimum value is 4, but it is recommended to start with the "0x7" value due to more stable.
*/
#define FLASH_RETRY_CFG_START      (0x7U)

/*******************************************************************************
* CallBack function period
*******************************************************************************/
#ifndef FLASH_CALLBACK_CS
/*! @brief  Callback period count for FlashCheckSum
 *
 * This value is only relevant for FlashCheckSum operation, where a high rate of
 * calling back can impair performance. The rest of the flash operations invoke
 * the callback as often as possible while waiting for the flash controller
 * to finish the requested operation.
 */
#define FLASH_CALLBACK_CS 0x0AU
#endif

/*******************************************************************************
* Null Callback function definition
*******************************************************************************/
/*!
 * @name Null Callback function definition
 * @{
 */
/*! @brief  Null callback */
#define NULL_CALLBACK          ((flash_callback_t)0x0U)
#define NULL_SYNCCALLBACK      ((flash_syncCallback_t)0x0U)

/*@}*/

typedef enum
{
    FLASH_EVENT_CHECKSUM         = 0x01U,    /*!< Check sum reach the FLASH_CALLBACK_CS */
    FLASH_EVENT_COMPLETE         = 0x02U,    /*!< Erase/program operation completed */
    FLASH_EVENT_ACCESS_ERROR     = 0x03U,    /*!< An access error occured when command launch */
    FLASH_EVENT_ERROR            = 0x04U,    /*!< An error occured during erase/program operation */
} flash_event_t;

/*******************************************************************************
* Callback function prototype
*******************************************************************************/
/*! @brief Call back function pointer data type for launch command in sync mode
 *
 *   If using callback with sync mode, any code reachable from this function
 *   must not be placed in a Flash block targeted for a program/erase operation.
 *   Functions can be placed in RAM section by using the
 *   START/END_FUNCTION_DEFINITION/DECLARATION_RAMSECTION macros.
 */
typedef void (*flash_syncCallback_t)(void);

/*! @brief Call back function pointer data type for flash event */
typedef void (*flash_callback_t)(flash_event_t event);

/*******************************************************************************
* Function Prototypes for Flash SSD
*******************************************************************************/
/*!
 * @name Flash driver APIs
 * @{
 */

#if defined(__cplusplus)
extern "C" {
#endif

/*! @brief Flash driver configuration */
typedef struct {
    bool async;                /*!< true: asynchronous (Only support for RWW), false: synchronous */
    bool disGlobalInt;         /*!< true: disable global interrupt during flash operation, only for sync mode */
    bool readVerify;           /*!< true: enable read verify after erase or program, false: disable read verify */
#ifdef EFM_WDG_WDG_EN_MASK
    bool wdgEnable;              /*!< true: enable WDG, false: disable WDG */
#endif /* EFM_WDG_WDG_EN_MASK */
    flash_callback_t callback; /*!< Callback function pointer */
} flash_user_config_t;

/*! @cond DRIVER_INTERNAL_USE_ONLY */
typedef struct{
    volatile uint32_t cmdCode;
    volatile uint32_t *dest;
    volatile const uint32_t *pdata;
    volatile uint32_t word_size;
} flash_cmd_handle_t;
/*! @endcond */

/*! @brief Flash driver state */
typedef struct {
/*! @cond DRIVER_INTERNAL_USE_ONLY */
    bool async;                     /*!< true: asynchronous, false: synchronous */
    bool disGlobalInt;              /*!< true: disable global interrupt, only for sync mode */
    bool readVerify;                /*!< true: enable read verify after erase or program, false: disable read verify */
    flash_callback_t callback;      /*!< Callback function pointer */
    volatile bool driverBusy;       /*!< Driver is busy with an operation */
    flash_cmd_handle_t cmdParam;    /*!< Command parameter */
    volatile int32_t leftSize;      /*!< Left size of erase or program operation */
    volatile uint32_t retryCount;   /*!< Retry count for erase retry manual */
#ifdef FEATURE_EFM_PROGRAM_NVR_CMD_CODE
    volatile uint32_t nvr_addr;
    volatile uint32_t *nvr_data;
#endif /* FEATURE_EFM_PROGRAM_NVR_CMD_CODE */
    volatile uint32_t shiftsum;     /*!< Shiftsum value to protect cmdParam and leftSize */
/*! @endcond */
} flash_state_t;

/*!
 * @brief FLASH init
 *
 * This function initializes the Flash module.
 *
 * @param[in] instance Flash instance number.
 * @return operation status
 *        - STATUS_SUCCESS:   Operation was successful.
 *        - STATUS_ERROR:     Operation failure was occurred.
 *        - STATUS_BUSY:      Operation was busy.
 */
status_t FLASH_DRV_Init(uint32_t instance, const flash_user_config_t * userConfigPtr, flash_state_t * state);

/*!
 * @brief FLASH deinit
 *
 * This function de-initializes the Flash module.
 *
 * @param[in] instance Flash instance number.
 * @return operation status
 *        - STATUS_SUCCESS:   Operation was successful.
 *        - STATUS_ERROR:     Operation failure was occurred.
 *        - STATUS_BUSY:      Operation was busy.
 */
status_t FLASH_DRV_Deinit(uint32_t instance);

/*!
 * @brief FLASH get default configuration
 *
 * This function gets the default configuration structure for the Flash module
 * user configuration.
 *
 * @param[in] instance Flash instance number.
 * @param[out] userConfigPtr Pointer to the user configuration structure.
 */
void FLASH_DRV_GetDefaultConfig(flash_user_config_t * const userConfigPtr);

/*!
 * @brief FLASH get busy status
 *
 * This function gets the busy status of the Flash module.
 *
 * @param[in] instance Flash instance number.
 * @return busy status
 *        - true: Flash is busy with an operation.
 *        - false: Flash is idle.
 */
bool FLASH_DRV_GetBusyStatus(uint32_t instance);

/*!
 * @brief FLASH set async mode
 *
 * This function sets the async mode.
 *
 * @param[in] instance Flash instance number.
 * @param[in] readVerify true: async mode, 
 *                       false: sync mode.
 * @return operation status
 *        - STATUS_SUCCESS:   Operation was successful.
 *        - STATUS_ERROR:     Operation failure was occurred.
 *        - STATUS_EFM_BUSY:  Operation was busy.
 */
status_t FLASH_DRV_SetAsyncMode(uint32_t instance, bool async);

/*!
 * @brief FLASH set disable or enable global interrupt
 *
 * This function sets disable or enable global interrupt in sync mode.
 *
 * @param[in] instance Flash instance number.
 * @param[in] readVerify true: async mode, 
                         false: sync mode.
 */
void FLASH_DRV_SetDisableGlobalInt(uint32_t instance, bool disGlobalInt);

/*!
 * @brief FLASH set read verify
 *
 * This function sets the read verify after erase or program.
 *
 * @param[in] instance Flash instance number.
 * @param[in] readVerify true: enable read verify after erase or program,
                         false: disable read verify
 */
void FLASH_DRV_SetReadVerify(uint32_t instance, bool readVerify);

#ifdef FEATURE_EFM_ERASE_ARRAY_CMD_CODE
/*!
 * @brief Flash Array Erase
 *
 * This API is used to erase a array of flash memory.
 *
 * @param[in] instance Flash instance number.
 * @param[in] dest Start address for the target flash array.
                   This address should be aligned to flash array margin.
 * @return operation status
 *        - STATUS_SUCCESS:         Operation was successful.
 *        - STATUS_ERROR:           Operation failure was occurred.
 *        - STATUS_BUSY:            Operation was busy.
 */
status_t FLASH_DRV_EraseArray(uint32_t instance, uint32_t dest);
#endif /* FEATURE_EFM_ERASE_ARRAY_CMD_CODE */

/*!
 * @brief Flash Block Erase
 *
 * This API is used to erase a block of flash memory.
 *
 * @param[in] instance Flash instance number.
 * @param[in] dest Start address for the target flash block.
                   This address should be aligned to flash block margin.
 * @return operation status
 *        - STATUS_SUCCESS:         Operation was successful.
 *        - STATUS_ERROR:           Operation failure was occurred.
 *        - STATUS_BUSY:            Operation was busy.
 */
status_t FLASH_DRV_EraseBlock(uint32_t instance, uint32_t dest);

/*!
 * @brief Flash erase sector.
 *
 * This API erases one or more sectors in main flash or data flash memory.
 * This API always returns STATUS_SUCCESS if size provided by the user is
 * zero regardless of the input validation.
 *
 * @param[in] instance Flash instance number.
 * @param[in] dest Address in the first sector to be erased.
 *            User need to make sure the dest address in of main flash or data flash block.
 *            This address should be aligned to sector size:
 *            FEATURE_EFM_MAIN_ARRAY_SECTOR_SIZE or FEATURE_EFM_DATA_ARRAY_SECTOR_SIZE.
 *
 * @param[in] size Size to be erased in bytes. It is used to determine number of sectors to be erased.
 *            This size should be aligned to bytes:
 *            FEATURE_EFM_MAIN_ARRAY_SECTOR_SIZE or FEATURE_EFM_DATA_ARRAY_SECTOR_SIZE.
 *
 * @return operation status
 *        - STATUS_SUCCESS:         Operation was successful.
 *        - STATUS_ERROR:           Operation failure was occurred.
 *        - STATUS_BUSY:            Operation was busy.
 */
status_t FLASH_DRV_EraseSector(uint32_t instance, uint32_t dest, uint32_t size);

/*!
 * @brief Flash erase sector quick.
 *
 * This API erases one or more sectors in main flash or data flash memory.
 * This API always returns STATUS_SUCCESS if size provided by the user is
 * zero regardless of the input validation.
 * This API is used for the case that the flash need to be erased quickly.
 *
 * @param[in] instance Flash instance number.
 * @param[in] dest Address in the first sector to be erased.
 *            User need to make sure the dest address in of main flash or data flash block.
 *            This address should be aligned to sector size:
 *            FEATURE_EFM_MAIN_ARRAY_SECTOR_SIZE or FEATURE_EFM_DATA_ARRAY_SECTOR_SIZE.
 *
 * @param[in] size Size to be erased in bytes. It is used to determine number of sectors to be erased.
 *            This size should be aligned to bytes:
 *            FEATURE_EFM_MAIN_ARRAY_SECTOR_SIZE or FEATURE_EFM_DATA_ARRAY_SECTOR_SIZE.
 *
 * @return operation status
 *        - STATUS_SUCCESS:         Operation was successful.
 *        - STATUS_ERROR:           Operation failure was occurred.
 *        - STATUS_BUSY:            Operation was busy.
 */
status_t FLASH_DRV_EraseSectorQuick(uint32_t instance, uint32_t dest, uint32_t size);

/*!
 * @brief Flash program
 *
 * This API is used to program multiple consecutive bytes on P-Flash
 * or D-Flash block. This API always returns EFM_OK if size provided
 * by user is zero regardless of the input validation
 *
 * @param[in] instance Flash instance number.
 * @param[in] dest Start address for the intended program operation.
 *                 This address should be aligned to bytes:
 *                 FEATURE_EFM_WRITE_UNIT_SIZE
 *                  
 * @param[in] size Size in byte to be programmed.
 *                 This size should be aligned to bytes:
 *.                FEATURE_EFM_WRITE_UNIT_SIZE
 *
 * @param[in] pData Pointer of source address from which data has to
 *                  be taken for program operation. pData should be **aligned to
 *                  32-bit boundary**.
 * @return operation status
 *        - STATUS_SUCCESS:         Operation was successful.
 *        - STATUS_ERROR:           Operation failure was occurred.
 *        - STATUS_BUSY:            Operation was busy.
 */
status_t FLASH_DRV_Program(uint32_t instance, uint32_t dest, uint32_t size, const void *pData);

#ifdef FEATURE_EFM_PROGRAM_DATA_CMD_CODE
/*!
 * @brief Flash Program DFlash
 *
 * This API is used to program multiple consecutive bytes on P-Flash
 * or D-Flash block. This API always returns EFM_OK if size provided
 * by user is zero regardless of the input validation.
 *
 * @param[in] instance Flash instance number.
 * @param[in] dest Start address for the intended program operation.
 *                 This address should be aligned to bytes:
 *                 FEATURE_EFM_WRITE_DATA_UNIT_SIZE.
 *                  
 * @param[in] size Size in byte to be programmed.
 *                 This size should be aligned to bytes:
 *                 FEATURE_EFM_WRITE_DATA_UNIT_SIZE.
 *
 * @param[in] pData Pointer of source address from which data has to
 *                  be taken for program operation. pData should be **aligned to
 *                  32-bit boundary**.
 * @return operation status
 *        - STATUS_SUCCESS:         Operation was successful.
 *        - STATUS_ERROR:           Operation failure was occurred.
 *        - STATUS_BUSY:            Operation was busy.
 */
status_t FLASH_DRV_ProgramDFlash(uint32_t instance, uint32_t dest, uint32_t size, const void *pData);
#endif /* FEATURE_EFM_PROGRAM_DATA_CMD_CODE */

/*!
 * @brief Calculates check sum.
 *
 * This API performs 32 bit sum of each byte data over a specified Flash
 * memory range without carry which provides rapid method for checking data integrity.
 * The callback time period of this API is determined via FLASH_CALLBACK_CS macro in
 * flash_driver.h which is used as a counter value for the CallBack() function calling in
 * this API. This value can be changed as per the user requirement. User can change this value
 * to obtain the maximum permissible callback time period.
 * This API always returns STATUS_SUCCESS if size provided by user is zero regardless of the input
 * validation.
 *
 * @param[in] instance Flash instance number.
 * @param[in] dest Start address of the Flash range to be summed.
 * @param[in] size Size in byte of the Flash range to be summed.
 * @param[in] pSum To return the sum value.
 * @return operation status
 *        - STATUS_SUCCESS:         Operation was successful.
 *        - STATUS_ERROR:           Operation failure was occurred.
 */
status_t FLASH_DRV_CheckSum(uint32_t instance, uint32_t dest, uint32_t size, uint32_t *pSum);

/*!
 * @brief Enable the command complete interrupt.
 *
 * This function will enable the command complete interrupt is generated when
 * an EFM command completes.
 *
 * @param[in] instance Flash instance number.
 */
void FLASH_DRV_EnableCmdCompleteInterrupt(uint32_t instance);

/*!
 * @brief Disable the command complete interrupt.
 *
 * @param[in] instance Flash instance number.
 */
void FLASH_DRV_DisableCmdCompleteInterrupt(uint32_t instance);

/*!
 * @brief Enable the read collision error interrupt.
 *
 * This function will enable the read collision error interrupt generation when an
 * EFM read collision error occurs.
 *
 * @param[in] instance Flash instance number.
 */
void FLASH_DRV_EnableReadCollisionInterrupt(uint32_t instance);

/*!
 * @brief Disable the read collision error interrupt.
 *
 * @param[in] instance Flash instance number.
 */
void FLASH_DRV_DisableReadCollisionInterrupt(uint32_t instance);

/*!
 * @brief Enable the single bit fault detect interrupt.
 *
 * This function will enable the single bit fault detect interrupt generation when
 * an recovery ECC fault is detected during a valid flash read access from
 * the platform flash controller.
 *
 * @param[in] instance Flash instance number.
 */
void FLASH_DRV_EnableSingleBitFaultInterrupt(uint32_t instance);

/*!
 * @brief Disable the single bit fault detect interrupt.
 *
 * @param[in] instance Flash instance number.
 */
void FLASH_DRV_DisableSingleBitFaultInterrupt(uint32_t instance);

/*!
 * @brief Enable the double bit fault detect interrupt.
 *
 * This function will enable the double bit fault detect interrupt generation when
 * an uncorrectable ECC fault is detected during a valid flash read access from
 * the platform flash controller.
 *
 * @param[in] instance Flash instance number.
 */
void FLASH_DRV_EnableDoubleBitFaultInterrupt(uint32_t instance);

/*!
 * @brief Disable the double bit fault detect interrupt.
 *
 * @param[in] instance Flash instance number.
 */
void FLASH_DRV_DisableDoubleBitFaultInterrupt(uint32_t instance);

/*!
 * @brief Get the command complete interrupt status.*
 *
 * @param[in] instance Flash instance number.
 */
bool FLASH_DRV_GetDoneStatus(uint32_t instance);

/*!
 * @brief Get the read collision error interrupt status.
 *
 * @param[in] instance Flash instance number.
 */
bool FLASH_DRV_GetReadCollisionFlag(uint32_t instance);

#ifdef FEATURE_EFM_ERASE_NVR_CMD_CODE
/*!
 * @brief Flash NVR Sector Erase
 *
 * This API is used to erase an NVR sector of flash memory.
 * THis API won't support multi-sector erase for safety consideration.
 *
 * \note User have to unlock the flash NVR sector before programming.
 *
 * @param[in] instance Flash instance number.
 * @param[in] dest Start address for the target flash NVR sector.
                   This address should be aligned to flash block margin.
 * @return operation status
 *        - STATUS_SUCCESS:         Operation was successful.
 *        - STATUS_ERROR:           Operation failure was occurred.
 *        - STATUS_BUSY:            Operation was busy.
 */
status_t FLASH_DRV_EraseNVR(uint32_t instance, uint32_t dest);

/*!
 * @brief Flash NVR Program
 *
 * This API is used to program flash NVR sectors.
 * User can get flash NVR partition information from chip reference manual.
 * This API can be used to program AES key, customer NVR region, etc.
 * \note User have to unlock the flash NVR sector before programming.
 * \note THis API won't support multi-sector program for safety consideration.
 *
 * @param[in] instance Flash instance number.
 * @param[in] dest Start address for the target flash NVR sector.
                   This address should be aligned to flash block margin.
 * @param[in] size Size in byte to be programmed.
 *                 This size should be aligned to bytes following below table.
 * @param[in] pData Pointer of source address from which data has to
 *                  be taken for program operation. **pData should be aligned to
 *                  32-bit boundary**.
 * @return operation status
 *        - STATUS_SUCCESS:         Operation was successful.
 *        - STATUS_ERROR:           Operation failure was occurred.
 *        - STATUS_BUSY:            Operation was busy.
 */
status_t FLASH_DRV_ProgramNVR(uint32_t instance, uint32_t dest, uint32_t size, const void *pData);

/*!
 * @brief Flash NVR Read
 *
 * This API is used to read data from flash NVR region.
 * This API won't support AES key read for safety consideration.
 *
 * \note User have to unlock the flash NVR sector before programming.
 *
 * @param[in] instance Flash instance number.
 * @param[in] address Start address for the target flash NVR sector.
 *                 This address should be aligned to flash block margin.
 * @param[in] size Total read data length in byte.
 *                 This read length should be aligned to flash program margin.
 * @param[out] dest Read data destination address.
 *                 This address should be aligned to 32-bit boundary.
 * @return operation status
 *        - STATUS_SUCCESS:         Operation was successful.
 *        - STATUS_ERROR:           Operation failure was occurred.
 *        - STATUS_BUSY:            Operation was busy.
 */
status_t FLASH_DRV_ReadNVR(uint32_t instance, uint32_t address, uint32_t size, void *dest);

#endif /* FEATURE_EFM_ERASE_NVR_CMD_CODE */

#ifdef FEATURE_EFM_BOOT_SWAP_CMD_CODE

/*!
 * @brief Flash boot swap
 *
 * This API is used swap the boot flash block with the current flash block.
 *
 * @param[in] instance Flash instance number.
 * @return operation status
 *        - STATUS_SUCCESS:         Operation was successful.
 *        - STATUS_ERROR:           Operation failure was occurred.
 *        - STATUS_BUSY:            Operation was busy.
 */

status_t FLASH_DRV_BootSwap(uint32_t instance);

#endif

#ifdef FEATURE_EFM_LOAD_AES_KEY_CMD_CODE
/*!
 * @brief Flash load AES key
 *
 * This API is used for load AES key for HCU.
 *
 * @param[in] instance Flash instance number.
 * @param[in] address Start address for the target flash NVR sector.
 *                 This address should be aligned to flash block margin.
 * 
 * @return operation status
 *        - STATUS_SUCCESS:         Operation was successful.
 *        - STATUS_ERROR:           Operation failure was occurred.
 *        - STATUS_BUSY:            Operation was busy.
 */

status_t FLASH_DRV_LoadAESKey(uint32_t instance, uint32_t address);
#endif /* FEATURE_EFM_LOAD_AES_KEY_CMD_CODE */

#ifdef FEATURE_EFM_LOAD_RSA_KEY_CMD_CODE
/*!
 * @brief Flash load RSA key
 *
 * This API is used for load RSA key for HCU.
 *
 * @param[in] instance Flash instance number.
 * @param[in] address Start address for the target flash NVR sector.
 *                 This address should be aligned to flash block margin.
 * @param[in] keyLen RSA key length, for (keyLen+1)*32 bit
 *
 * @return operation status
 *        - STATUS_SUCCESS:         Operation was successful.
 *        - STATUS_ERROR:           Operation failure was occurred.
 *        - STATUS_BUSY:            Operation was busy.
 */
status_t FLASH_DRV_LoadRSAKey(uint32_t instance, uint32_t address, uint8_t keyLen);
#endif /* FEATURE_EFM_LOAD_RSA_KEY_CMD_CODE */

#if defined(__cplusplus)
}
#endif

/*@}*/ /* End of Flash driver APIs*/
/*! @}*/ /* End of addtogroup flash_driver */

#endif /* FLASH_DRIVER_H */
