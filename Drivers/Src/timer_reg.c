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

void TIMER_BASE_Init(TIMER_Typedef *TIMx, UINT8 timer_number, TIMER_Config *timer_config)
{
    /* TIM2–TIM5 share consecutive APB1ENR enable bits starting at bit 0 (TIM2).
     * Subtracting 2 from the logical timer number maps it directly to the correct
     * bit position without a lookup table. */
    RCC->APB1ENR |= (1U << (timer_number - 2));

    TIMx->PSC  = timer_config->Prescaler;
    TIMx->ARR  = timer_config->AutoReload;

    /* Enabling UIE arms the timer to assert its interrupt line on each counter overflow,
     * which is the hardware signal the debounce middleware uses as a timeout notification. */
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