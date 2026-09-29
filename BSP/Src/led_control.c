/**
 * @file led_control.c
 * @author BUI_TAN_HUNG
 * @brief Implementation of low-level GPIO LED driver for STM32F1xx platforms.
 * @version 1.0.0
 * @date 2026-06-06
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "led_control.h"
#include "gpio.h"

/**
 * @brief  Initialize the GPIO pin associated with the given LED device.
 * @note   Enables the corresponding GPIO port clock via RCC_APB2ENR, then
 *         configures the pin as output push-pull at 50 MHz (MODE=11, CNF=00).
 * @param  led  Pointer to a fully populated LED_Device structure.
 */
void LED_Init(LED_Device *led)
{
    led->gpio_config.cnf = 0x0; //00
    led->gpio_config.mode = 0x3; //10
    GPIO_Init(&led->gpio_config);
}

/**
 * @brief  Set the physical output level of the LED according to led->current_state.
 * @note   Uses the GPIO BSRR register (offset 0x10): BS bits to set, BR bits to reset.
 * @param  led  Pointer to the LED_Device whose current_state will be applied.
 */
void LED_SetState(LED_Device *led)
{
    PIN_STATE pin_state = (led->current_state == LED_ON) ? set : reset;
    GPIO_WritePin(&led->gpio_config, pin_state);
}

/**
 * @brief  Toggle the LED state between LED_ON and LED_OFF, then apply it.
 * @note   Internally flips led->current_state and calls LED_SetState().
 * @param  led  Pointer to the LED_Device to toggle.
 */
void LED_Toggle(LED_Device *led)
{
    switch (led->current_state)
    {
        case LED_ON:
            led->current_state = LED_OFF;
            break;
        default:
            led->current_state = LED_ON;
            break;
    }
    LED_SetState(led);
}
