/*
 * Copyright 2020-2025 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 * @file vector_table_copy.c
 * @brief 
 * 
 */

#include <stdint.h>
#include "device_registers.h"

static volatile uint32_t * const s_vectors[NUMBER_OF_CORES] = FEATURE_INTERRUPT_INT_VECTORS;
extern uint32_t IVT_RAM_start[];
extern uint32_t IVT_start[];
extern uint32_t IVT_end[];


void VectorTableCopy(){
    uint8_t coreId = (uint8_t)GET_CORE_ID();
    uint32_t* ramStart = (uint32_t*)IVT_RAM_start;
    const uint32_t* romStart = (const uint32_t*)IVT_start;
    uint32_t size=((uint32_t)IVT_end-(uint32_t)IVT_start)/sizeof(uint32_t);
    *s_vectors[coreId] = (uint32_t)IVT_RAM_start;

    for (uint32_t n = 0; n < size; n++)
    {
        ramStart[n] = romStart[n];
    }
}


void DefaultISR(void)
{
   while(1){};
}

__attribute__ ((weak)) void NMI_Handler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void HardFault_Handler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void MemManage_Handler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void BusFault_Handler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void UsageFault_Handler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void SVC_Handler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void DebugMon_Handler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void PendSV_Handler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void SysTick_Handler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void DMA0_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void DMA1_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void DMA2_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void DMA3_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void DMA_Error_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void EFM_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void EFM_Error_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void PCU_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void EFM_Ecc_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void WDG0_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void I2C0_Master_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void I2C0_Slave_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void SPI0_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void SPI1_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void SPI2_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void I2C1_Master_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void UART0_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void UART1_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void UART2_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void ADC0_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void ACMP0_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void EMU0_SB_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void EMU0_DB_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void pTMR0_Ch0_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void pTMR0_Ch1_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void pTMR0_Ch2_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void pTMR0_Ch3_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void SCU_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void lpTMR0_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void GPIOA_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void GPIOB_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void GPIOC_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void GPIOD_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void GPIOE_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void CAN0_ORed_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void CAN0_Error_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void CAN0_Wake_Up_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void CAN0_ORed_0_15_MB_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void CAN0_ORed_16_31_MB_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void CAN0_ORed_32_47_MB_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void CAN0_ORed_48_63_MB_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void CAN1_ORed_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void CAN1_Error_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void CAN1_Wake_Up_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void CAN1_ORed_0_15_MB_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void CAN1_ORed_16_31_MB_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void CAN1_ORed_32_47_MB_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void CAN1_ORed_48_63_MB_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void eTMR0_Ch0_Ch1_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void eTMR0_Ch2_Ch3_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void eTMR0_Ch4_Ch5_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void eTMR0_Ch6_Ch7_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void eTMR0_Fault_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void eTMR0_Ovf_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void eTMR1_Ch0_Ch1_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void eTMR1_Ch2_Ch3_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void eTMR1_Ch4_Ch5_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void eTMR1_Ch6_Ch7_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void eTMR1_Fault_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void eTMR1_Ovf_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void MPWM0_Ch0_Ovf_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void MPWM0_Ch1_Ovf_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void MPWM0_Ch2_Ovf_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void MPWM0_Ch3_Ovf_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void MPWM0_Ch4_Ovf_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void MPWM0_Ch5_Ovf_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void MPWM0_Ch6_Ovf_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void MPWM0_Ch7_Ovf_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void MPWM0_Ch0_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void MPWM0_Ch1_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void MPWM0_Ch2_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void MPWM0_Ch3_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void MPWM0_Ch4_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void MPWM0_Ch5_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void MPWM0_Ch6_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void MPWM0_Ch7_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void TRNG_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void HCU_IRQHandler(void) {
    DefaultISR();
}
