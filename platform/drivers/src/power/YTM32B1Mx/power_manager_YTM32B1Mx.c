/*
 * Copyright 2020-2025 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file power_manager_YTM32B1Mx.c
 * @version 1.4.0
 */

#include "power_manager.h"
#include "clock_manager.h"

/*! @brief Power manager internal structure. */
power_manager_state_t gPowerManagerState;

/*******************************************************************************
 * INTERNAL FUNCTIONS
 ******************************************************************************/
static void POWER_SYS_SwitchToLowPowerMode(const power_manager_user_config_t * const configPtr);

/*******************************************************************************
 * Code
 ******************************************************************************/

/*******************************************************************************
 *
 * The following is an example of how to set up two power modes and one
 * callback, and initialize the Power manager with structures containing their settings.
 * The example shows two possible ways the configuration structures can be stored
 * (ROM or RAM), although it is expected that they will be placed in the read-only
 * memory to save the RAM space. (Note: In the example it is assumed that the programmed chip
 * doesn't support any optional power options described in the power_manager_user_config_t)
 * :
 * @code
 *
 *  power_manager_user_config_t sleepConfig = {   sleep power mode configuration
 *     .powerMode = POWER_MANAGER_SLEEP,
 *     .sleepOnExitValue = false,
 *  };
 *
 *  power_manager_user_config_t deepsleepConfig = {   deepsleep power mode configuration
 *     .powerMode = POWER_MANAGER_DEEPSLEEP,
 *     .sleepOnExitValue = false,
 *  };
 *
 *  power_manager_user_config_t const * powerConfigsArr[] = {    Power mode configurations array
 *     &sleepConfig,
 *     &deepsleepConfig
 *  };
 *
 *  power_manager_callback_user_config_t callbackCfg0 = {  Callback configuration structure callbackCfg0
 *     .callbackFunction                     = &callback0,
 *     .callbackType                         = POWER_MANAGER_CALLBACK_BEFORE_AFTER,
 *     .callbackData                         = (void *)0,
 *  };
 *
 *  power_manager_callback_user_config_t const * callbacksConfigsArr[] = {  Callback configuration structures array
 *     &callbackCfg0
 *  };
 *
 *  status_t callback0(power_manager_notify_struct_t * notify,   Definition of power manager callback
 *                                      power_manager_callback_data_t * dataPtr)
 *  {
 *   status_t ret = STATUS_SUCCESS;
 *   ...
 *   return ret;
 *  }
 *
 *  int main(void) Main function
 *  {
 *   status_t ret = STATUS_SUCCESS;
 *
 *   Calling of init method
 *   POWER_SYS_Init(&powerConfigsArr, 2U, &powerStaticCallbacksConfigsArr, 1U);
 *
 *   Switch to DEEPSLEEP mode
 *   ret = POWER_SYS_SetMode(MODE_DEEPSLEEP,POWER_MANAGER_POLICY_AGREEMENT);
 *
 *   if (ret != STATUS_SUCCESS)
 *   {
 *     return -1;
 *   }
 *   return 0;
 *  }
 *
 * @endcode
 *
 *END**************************************************************************/
status_t POWER_SYS_DoInit(void)
{
    return STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : POWER_SYS_DoDeinit
 * Description   : This function performs the actual implementation-specific de-initialization.
 *
 *
 *END**************************************************************************/
status_t POWER_SYS_DoDeinit(void)
{
    /* Biasing disabled, core logic can run in full performance */
    status_t status = STATUS_SUCCESS;

    return status;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : POWER_SYS_DoSetMode
 * Description   : This function performs the actual implementation-specific logic to switch
 * to one of the defined power modes.
 *
 *
 *END**************************************************************************/
status_t POWER_SYS_DoSetMode(const power_manager_user_config_t * const configPtr)
{
    status_t returnCode = STATUS_SUCCESS; /* Function return */

    POWER_SYS_SwitchToLowPowerMode(configPtr);

    return returnCode;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : POWER_SYS_GetCurrentMode
 * Description   : Returns currently running power mode.
 *
 * Implements POWER_SYS_GetCurrentMode_Activity
 *
 *END**************************************************************************/
power_mode_stat_t POWER_SYS_GetCurrentMode(power_manager_user_config_t *configPtr)
{
    power_mode_stat_t retVal;

    switch (configPtr->powerMode)
    {
        /* Run mode */
        case POWER_MANAGER_RUN:
            retVal = STAT_RUN;
            break;
        /* Sleep mode */
        case POWER_MANAGER_SLEEP:
            retVal = STAT_SLEEP;
            break;
        case POWER_MANAGER_DEEPSLEEP:
            retVal = STAT_DEEPSLEEP;
            break;
        /* This should never happen - core has to be in some run mode to execute code */
        default:
            retVal = STAT_INVALID;
            break;
    }

    return retVal;
}


/*FUNCTION**********************************************************************
 *
 * Function Name : POWER_SYS_SwitchToSleepingPowerMode
 * Description   :Internal function used by POWER_SYS_SetMode function to switch to a sleeping power mode
 * configPtr   pointer to the requested user-defined power mode configuration
 *
 *END**************************************************************************/
static void POWER_SYS_SwitchToLowPowerMode(const power_manager_user_config_t * const configPtr)
{
    uint32_t systickCSR;
#if (defined(FEATURE_SOC_HAS_SEPARATE_CMU_AND_CLKOUT) && (FEATURE_SOC_HAS_SEPARATE_CMU_AND_CLKOUT==1))
    uint32_t cmuCtrl = CMU0->CTRL;
#else
    uint32_t cmuCtrl = SCU->CMU_CTRL;
#endif
    /* Configure the hardware layer */
    switch (configPtr->powerMode)
    {
        case POWER_MANAGER_RUN:
            /* No operation needed */
            SCB->SCR &= ~(SCB_SCR_SLEEPONEXIT_Msk);
            break;
        case POWER_MANAGER_SLEEP:
            SCB->SCR &= ~(SCB_SCR_SLEEPDEEP_Msk);
            break;
#if defined(CPU_YTM32B1MC0) || defined(CPU_YTM32B1MD2)
        case POWER_MANAGER_DEEPSLEEP:
            SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
            PCU->CSR &= ~(PCU_CSR_PMCLPEN_MASK);
            break;
        case POWER_MANAGER_STANDBY:
            SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
            PCU->CSR |= (PCU_CSR_PMCLPEN_MASK);
            break;
#elif defined(CPU_YTM32B1ME1)
        case POWER_MANAGER_DEEPSLEEP:
            SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
            PCU->CTRL0 &= ~(PCU_CTRL0_PWR_MODE_MASK);
            break;
        case POWER_MANAGER_STANDBY:
            SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
            PCU->CTRL0 &= ~(PCU_CTRL0_PWR_MODE_MASK);
            PCU->CTRL0 |= (PCU_CTRL0_PWR_MODE(0x1));
            break;
        case POWER_MANAGER_POWERDOWN:
            SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
            PCU->CTRL0 &= ~(PCU_CTRL0_PWR_MODE_MASK);
            PCU->CTRL0 |= (PCU_CTRL0_PWR_MODE(0x2));
            break;
        case POWER_MANAGER_DEEPPOWERDOWN:
            SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
            PCU->CTRL0 &= ~(PCU_CTRL0_PWR_MODE_MASK);
            PCU->CTRL0 |= (PCU_CTRL0_PWR_MODE(0x3));
            break;
#else
        case POWER_MANAGER_DEEPSLEEP:
            SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
            PCU->CTRL &= ~(PCU_CTRL_RPMEN_MASK | PCU_CTRL_STANDBYEN_MASK);
            break;
        case POWER_MANAGER_STANDBY:
            SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
            PCU->CTRL |= PCU_CTRL_RPMEN_MASK | PCU_CTRL_STANDBYEN_MASK;
            break;
        case POWER_MANAGER_POWERDOWN:
            SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
            PCU->CTRL &= ~(PCU_CTRL_RPMEN_MASK | PCU_CTRL_STANDBYEN_MASK);
            PCU->CTRL |= PCU_CTRL_RPMEN_MASK;
            break;
#endif
        default:
            SCB->SCR &= ~(SCB_SCR_SLEEPDEEP_Msk);
            break;
    }

    if ((configPtr->powerMode == POWER_MANAGER_SLEEP) ||
        (configPtr->powerMode == POWER_MANAGER_DEEPSLEEP) ||
        #if defined(CPU_YTM32B1MD1) || defined(CPU_YTM32B1ME0) || defined(CPU_YTM32B1ME1)
        (configPtr->powerMode == POWER_MANAGER_POWERDOWN) ||
        #endif
        #if defined(CPU_YTM32B1ME1)
        (configPtr->powerMode == POWER_MANAGER_DEEPPOWERDOWN) ||
        #endif
        (configPtr->powerMode == POWER_MANAGER_STANDBY))
    {
        /* Disable CMU before entering low power mode */
    #if defined(CPU_YTM32B1MD2)
        CMU0->CTRL = 0;
        CMU0->STS = CMU0->STS;
    #else
        SCU->CMU_CTRL = 0;
        SCU->CMUSTS = SCU->CMUSTS;
    #endif
        /* Disable systick before entering low power mode */
        systickCSR = SysTick->CTRL;
        SysTick->CTRL = 0x00;
        /* Clear systick pending, to prevent immediate wake-up after sleep. */
        SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;
    
        if (configPtr->sleepOnExitValue)
        {
            SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;
        }
        else
        {
            SCB->SCR &= ~(SCB_SCR_SLEEPONEXIT_Msk);
        }

        /* ERRATA_E220008, this error only occurs with YTM32B1MD1.
           But for robustness, it's better to wait for flash idle before entering low power mode. */
        while(0U == (EFM->STS & EFM_STS_IDLE_MASK))
        {
            // wait for flash idle
        }
        STANDBY();
    
        /* Clear SLEEPDEEP bit to avoid RTOS tickless idle getting into deepsleep mode */
        SCB->SCR &= ~(SCB_SCR_SLEEPDEEP_Msk);
    
        /* Restore systick */
        SysTick->CTRL = systickCSR;
    
        if ((SCU->FXOSC_CTRL & SCU_FXOSC_CTRL_FXOSC_EN_MASK) != 0U)
        {
            (void)CLOCK_SYS_WaitFXOSCValid();
        }
        /* Restore CMU control register */
    #if defined(CPU_YTM32B1MD2)
        CMU0->CTRL = cmuCtrl;
    #else
        SCU->CMU_CTRL = cmuCtrl;
    #endif
    }
}

/*! @}*/
/*******************************************************************************
 * EOF
 ******************************************************************************/
