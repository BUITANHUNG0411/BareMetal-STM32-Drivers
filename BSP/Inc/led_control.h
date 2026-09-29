/**
 * @file led_control.h
 * @author BUI_TAN_HUNG
 * @brief Low-level driver for controlling GPIO LEDs on STM32F1xx platforms.
 * @version 1.0.0
 * @date 2026-06-06
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#pragma once
#include "gpio.h"

typedef enum {
    LED_OFF = 0,
    LED_ON
} LED_State;

typedef struct {
    GPIO_PinConfig gpio_config;
    LED_State      current_state;
} LED_Device;

/**
 * @brief  Initialize the GPIO pin associated with the given LED device.
 * @note   Enables the corresponding GPIO port clock via RCC_APB2ENR, then
 *         configures the pin as output push-pull at 50 MHz (MODE=11, CNF=00).
 * @param  led  Pointer to a fully populated LED_Device structure.
 */
void LED_Init(LED_Device *led);

/**
 * @brief  Set the physical output level of the LED according to led->current_state.
 * @note   Uses the GPIO BSRR register (offset 0x10): BS bits to set, BR bits to reset.
 * @param  led  Pointer to the LED_Device whose current_state will be applied.
 */
void LED_SetState(LED_Device *led);

/**
 * @brief  Toggle the LED state between LED_ON and LED_OFF, then apply it.
 * @note   Internally flips led->current_state and calls LED_SetState().
 * @param  led  Pointer to the LED_Device to toggle.
 */
void LED_Toggle(LED_Device *led);