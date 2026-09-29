/**
 * @file rcc.c
 * @author BUI_TAN_HUNG
 * @brief RCC driver implementation for STM32F1xx.
 * @version 1.0.0
 * @date 2026-09-29
 *
 * @copyright Copyright (c) 2026 BUI_TAN_HUNG
 *
 * @note  The RCC peripheral is currently accessed directly through the RCC pointer
 *        macro defined in rcc.h, so no function implementations are required here.
 *        This file is reserved for future clock configuration routines such as
 *        PLL setup, HSE oscillator enable, and APB/AHB bus prescaler programming.
 */
 #include "rcc.h"

void RCC_SystemClock_72MHz(void)
{
    RCC->CR |= (1 << 16);                        
    while ( !(RCC->CR & (1 << 17)) ) {}           

   
    FLASH_ACR = (1 << 4) | (2 << 0);                

    RCC->CFGR |= (4 << 8) | (1 << 16) | (7 << 18);

    RCC->CR |= (1 << 24);                           
    while ( !(RCC->CR & (1 << 25)) ) {}             

    RCC->CFGR |= (2 << 0);                       
    while ( (RCC->CFGR & (3 << 2)) != (2 << 2) ) {} 
}