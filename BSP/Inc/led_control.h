/**
 * @file led_control.h
 * @author BUI_TAN_HUNG
 * @brief GPIO-based LED driver providing logical state management and hardware output
 *        control for a single LED on STM32F1xx platforms.
 * @version 1.0.0
 * @date 2026-06-06
 *
 * @copyright Copyright (c) 2026 BUI_TAN_HUNG
 */

#pragma once
#include "gpio.h"

/**
 * @brief Logical output state of an LED.
 *
 * @note  The mapping between LED_ON/LED_OFF and the physical pin level depends on
 *        the board circuit (active-high vs. active-low). This driver assumes active-high
 *        (LED_ON drives the pin high to source current through the LED).
 */
typedef enum {
    LED_OFF = 0, /**< LED de-energised; GPIO output is driven low.  */
    LED_ON       /**< LED energised;    GPIO output is driven high. */
} LED_State;

/**
 * @brief Represents a single LED bound to a GPIO output pin.
 */
typedef struct {
    GPIO_PinConfig gpio_config;   /**< Port and pin assignment for this LED.         */
    LED_State      current_state; /**< Tracks the intended logical state of the LED. */
} LED_Device;

/**
 * @brief  Configure the GPIO pin associated with an LED for push-pull output.
 *
 * @param  led  Pointer to an LED_Device with gpio_config.port_base and
 *              gpio_config.pin_number set by the caller. Must not be NULL.
 * @return void
 * @note   Overrides the mode and cnf fields to enforce push-pull output at 50 MHz
 *         (MODE=11, CNF=00) regardless of any values previously stored in the struct,
 *         then delegates clock enable and register programming to GPIO_Init().
 */
void LED_Init(LED_Device *led);

/**
 * @brief  Apply led->current_state to the physical GPIO output.
 *
 * @param  led  Pointer to an initialised LED_Device. Must not be NULL.
 * @return void
 * @note   The GPIO line is driven through BSRR (atomic set/reset), so this function
 *         is safe to call from interrupt context without disabling interrupts.
 */
void LED_SetState(LED_Device *led);

/**
 * @brief  Invert the LED logical state and immediately drive it to the hardware.
 *
 * @param  led  Pointer to an initialised LED_Device. Must not be NULL.
 * @return void
 * @note   Updates led->current_state in-place before calling LED_SetState(), ensuring
 *         that the struct always reflects the actual physical output level.
 */
void LED_Toggle(LED_Device *led);