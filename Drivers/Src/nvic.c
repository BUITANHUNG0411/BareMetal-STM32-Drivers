/**
 * @file nvic.c
 * @author BUI_TAN_HUNG
 * @brief Implementation of the NVIC initialization.
 * @version 1.0.0
 * @date 2026-09-29
 *
 * @copyright Copyright (c) 2026 BUI_TAN_HUNG
 */

#include "nvic.h"

void NVIC_Init()
{
    /* Enable EXTI0 and TIM2 interrupts by setting their respective bits in ISER0 */
    NVIC_ISER0 |= (1 << EXTI0_IRQn) | (1 << TIM2_IRQn);
}