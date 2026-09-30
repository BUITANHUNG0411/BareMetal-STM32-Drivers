/**
 * @file timer_reg.c
 * @author BUI_TAN_HUNG
 * @brief Register-direct implementation of TIM2–TIM5 base-timer control for STM32F1xx.
 * @version 1.0.0
 * @date 2026-09-28
 *
 * @copyright Copyright (c) 2026 BUI_TAN_HUNG
 */

#include "timer_reg.h"

void TIMER_BASE_Init(TIMER_Typedef *TIMx, TIMER_Config *timer_config)
{
    /* Gate the APB1 peripheral clock for the target timer before accessing its
     * registers. TIM2–TIM5 occupy consecutive enable bits starting at bit 0;
     * the correct bit is selected by switching on the peripheral base address.
     * Accessing an unclocked peripheral is silently ignored by the APB bridge,
     * which would leave PSC/ARR at their reset values without any error signal. */
    switch ((UINT32)TIMx)
    {
        case TIM2_BASE: RCC->APB1ENR |= (1U << 0); break;
        case TIM3_BASE: RCC->APB1ENR |= (1U << 1); break;
        case TIM4_BASE: RCC->APB1ENR |= (1U << 2); break;
        case TIM5_BASE: RCC->APB1ENR |= (1U << 3); break;
    }

    TIMx->PSC  = timer_config->Prescaler;
    TIMx->ARR  = timer_config->AutoReload;

    /* Enabling UIE arms the timer to assert its interrupt line on each counter
     * overflow, which is the hardware signal the debounce middleware uses as a
     * timeout notification. The counter is left stopped; the caller must invoke
     * TIMER_Start() when ready to begin the debounce window. */
    TIMx->DIER |= (1U << 0);
}

void TIMER_Start(TIMER_Typedef *TIMx)
{
    /* Setting CEN allows the counter to increment; the timer was previously halted
     * or has never run, so no residual state needs to be cleared here. */
    TIMx->CR1 |= (1U << 0);
}

void TIMER_Stop(TIMER_Typedef *TIMx)
{
    /* Halt the counter first to prevent an ARR overflow from generating an update event
     * between the CEN clear and the subsequent CNT reset. */
    TIMx->CR1 &= ~(1U << 0);
    TIMx->CNT  = 0;
}

void TIMER_ClearFlag(TIMER_Typedef *TIMx)
{
    /* UIF uses rc_w0 semantics: writing 0 to the bit clears the pending flag.
     * Writing 1 is explicitly ignored by the hardware, so a clear-mask operation
     * (rather than a set operation) is the correct method. */
    TIMx->SR &= ~(1U << 0);
}

void TIMER_Restart(TIMER_Typedef *TIMx)
{
    TIMER_Stop(TIMx);
    TIMER_Start(TIMx);
}