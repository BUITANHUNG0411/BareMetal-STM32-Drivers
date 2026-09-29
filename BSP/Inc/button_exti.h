/**
 * @file button_exti.h
 * @author BUI_TAN_HUNG
 * @brief EXTI-based falling-edge interrupt driver for a push-button on STM32F1xx.
 *        Provides GPIO floating-input configuration and EXTI line arming/acknowledgement
 *        at the register level, with no dependency on the HAL library.
 * @version 1.0.0
 * @date 2026-09-28
 *
 * @copyright Copyright (c) 2026 BUI_TAN_HUNG
 */

#pragma once
#include "gpio.h"

/**
 * @brief Represents a physical push-button mapped to a single GPIO pin.
 *
 * @note  The gpio_config member is configured for floating input inside
 *        Button_Exti_Init(); the caller must supply only the port and pin
 *        fields — mode and cnf are overridden automatically.
 */
typedef struct {
    GPIO_PinConfig gpio_config; /**< Port and pin assignment for this button. */
} Button_Device;

/**
 * @brief  Configure the button GPIO as floating input and arm its EXTI line
 *         for falling-edge interrupt detection.
 *
 * @param  btn  Pointer to a Button_Device with gpio_config.port_base and
 *              gpio_config.pin_number set by the caller. Must not be NULL.
 *              Currently supports pin 0 (routed to EXTI0) only.
 * @return void
 * @note   Enables the AFIO clock via RCC->APB2ENR so that the EXTI port-select
 *         registers (EXTICRx) are accessible before being programmed.
 *         An external pull-up resistor is required because floating input mode
 *         leaves the pad high-impedance — without it the line drifts and
 *         generates spurious interrupts.
 */
void Button_Exti_Init(Button_Device *btn);

/**
 * @brief  Acknowledge the pending EXTI interrupt for the button's pin.
 *
 * @param  btn  Pointer to a Button_Device that has been initialised via
 *              Button_Exti_Init(). Must not be NULL.
 * @return void
 * @note   EXTI_PR uses write-1-to-clear semantics. This must be called at
 *         the start of the EXTI ISR; leaving the flag set causes the handler
 *         to re-enter immediately after returning.
 */
void Button_Exti_ClearFlag(Button_Device *btn);