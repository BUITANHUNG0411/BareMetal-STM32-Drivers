/**
 * @file nvic.h
 * @author BUI_TAN_HUNG
 * @brief Nested Vectored Interrupt Controller (NVIC) register definitions and initialization.
 * @version 1.0.0
 * @date 2026-09-29
 *
 * @copyright Copyright (c) 2026 BUI_TAN_HUNG
 */

#pragma once

/** @brief Unsigned 32-bit integer type used for register access. */
#define UINT32 unsigned int

/** @brief Interrupt Set-Enable Register 0 (ISER0) base address. */
#define NVIC_ISER0 (*(volatile UINT32 *)(0xE000E100UL))

/** @brief IRQ number for EXTI Line 0. */
#define EXTI0_IRQn  6

/** @brief IRQ number for TIM2 global interrupt. */
#define TIM2_IRQn   28

/**
 * @brief Initializes the NVIC by enabling necessary interrupts.
 *
 * @return void
 * @note Enables EXTI0 and TIM2 interrupts in the ISER0 register.
 */
void NVIC_Init();