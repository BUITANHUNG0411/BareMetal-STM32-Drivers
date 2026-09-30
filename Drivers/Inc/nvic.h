/**
 * @file nvic.h
 * @author BUI_TAN_HUNG
 * @brief Nested Vectored Interrupt Controller (NVIC) register-direct driver for ARM Cortex-M3.
 *
 *        Provides a single generic API to enable any external interrupt by its IRQ number,
 *        without relying on CMSIS or the HAL abstraction layer.
 *
 *        Register reference: ARM Cortex-M3 Technical Reference Manual §3.4 (NVIC registers)
 *                            Base address: 0xE000E100 (Private Peripheral Bus).
 *
 * @version 1.1.0
 * @date 2026-09-29
 *
 * @copyright Copyright (c) 2026 BUI_TAN_HUNG
 */

#pragma once
#include "rcc.h"

/* ---------------------------------------------------------------------------
 * NVIC Interrupt Set-Enable Registers  (ARM Cortex-M3 TRM §3.4.1)
 *
 * The NVIC supports up to 240 external interrupts, partitioned across eight
 * 32-bit ISER registers, each covering 32 consecutive IRQ numbers:
 *
 *   ISER[0]  covers IRQ   0 – 31   (base: 0xE000E100)
 *   ISER[1]  covers IRQ  32 – 63   (base: 0xE000E104)
 *   ISER[n]  covers IRQ n*32 – n*32+31
 *
 * Writing 1 to a bit enables the corresponding IRQ; writing 0 has no effect
 * (use ICER registers to disable). Reads return the current enable state.
 * --------------------------------------------------------------------------*/

/**
 * @brief  Access the nth Interrupt Set-Enable Register by index.
 *
 * @param  x  Register index in [0, 7]; computed as (IRQn / 32).
 * @note   Each ISER occupies one 32-bit word; the stride between registers
 *         is therefore 4 bytes (0x04). Parentheses around (x) are mandatory
 *         to prevent operator-precedence errors when a compound expression
 *         is passed as the argument.
 */
#define NVIC_ISER(x)  (*(volatile UINT32 *)(0xE000E100UL + ((UINT32)(x) * 0x04U)))

/* ---------------------------------------------------------------------------
 * IRQ number definitions  (RM0008 Table 63 — STM32F10xxx vector table)
 *
 * These values are device-specific (assigned by STMicroelectronics, not ARM)
 * and must be consulted from the device reference manual for each target MCU.
 * --------------------------------------------------------------------------*/

/**
 * @brief IRQ position numbers for STM32F103xB peripherals.
 *
 * @note  Values are taken from RM0008 Table 63. They are device-specific:
 *        do not assume these numbers are valid on other Cortex-M3 devices.
 */
typedef enum {
    EXTI0_IRQn = 6,  /**< EXTI Line 0 interrupt,      RM0008 Table 63 position 6.  */
    TIM2_IRQn  = 28  /**< TIM2 global interrupt,       RM0008 Table 63 position 28. */
} IRQ_Number;

/* ---------------------------------------------------------------------------
 * Public API
 * --------------------------------------------------------------------------*/

/**
 * @brief  Enable an external interrupt in the NVIC.
 *
 * @param  IRQn  IRQ position number of the interrupt to enable.
 *               Use a value from the @c IRQ_Number enumeration or any raw
 *               IRQ number in the range [0, 239].
 * @return void
 *
 * @note   The function sets a single bit in the appropriate ISER register;
 *         all other interrupt enables are left unchanged (no read-modify-write
 *         hazard because ISER bits are write-1-to-set).
 *
 *         The corresponding peripheral interrupt must also be enabled at the
 *         peripheral level (e.g., DIER.UIE for a timer, EXTI_IMR for EXTI)
 *         before an interrupt can propagate to the CPU.
 *
 * @warning This function does not configure interrupt priority. If multiple
 *          interrupts share a priority level, preemption behaviour is
 *          determined by the NVIC hardware priority resolution rules
 *          (ARM Cortex-M3 TRM §3.4.7).
 */
void NVIC_Enable(IRQ_Number IRQn);