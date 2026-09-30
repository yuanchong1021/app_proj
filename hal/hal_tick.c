/**
 * @file  hal_tick.c
 * @brief 系统时基 HAL 实现（LPTMR0 1ms 中断）
 *
 * 修复：
 *   原 lptmrIsrCnt 为 uint8_t，255ms 溢出 → 改为 uint32_t。
 *   原中断里调用 PRINTF → 已删除。
 *   LPTMR_Config: clockSource=FXOSC(24MHz), prescaler=/2→12MHz,
 *                 compareValue=11999 → 周期=1ms。
 */
#include "hal_tick.h"
#include "lptmr_driver.h"
#include "interrupt_manager.h"

static volatile uint32_t s_tick_ms = 0u;

void lpTMR0_IRQHandler(void)
{
    lpTMR_DRV_ClearCompareFlag(0u);
    s_tick_ms++;
}

void HAL_Tick_Init(void)
{
    INT_SYS_EnableIRQ(lpTMR0_IRQn);
    lpTMR_DRV_StartCounter(0u);
}

u32 HAL_GetTickMs(void) { return s_tick_ms; }

/*---------------------------------------------------------------------------
 *  低功耗支持：暂停 / 恢复时基
 *
 *  只停计数器 + 关中断，不清 s_tick_ms，因此恢复后从冻结值继续累加。
 *  说明见 hal_tick.h 中 HAL_Tick_Suspend() 的注释。
 *-------------------------------------------------------------------------*/
void HAL_Tick_Suspend(void)
{
    lpTMR_DRV_StopCounter(0u);
    INT_SYS_DisableIRQ(lpTMR0_IRQn);
    lpTMR_DRV_ClearCompareFlag(0u);      /* 清掉可能挂着的标志，避免恢复瞬间补一次中断 */
}

void HAL_Tick_Resume(void)
{
    lpTMR_DRV_ClearCompareFlag(0u);
    INT_SYS_EnableIRQ(lpTMR0_IRQn);
    lpTMR_DRV_StartCounter(0u);
}
