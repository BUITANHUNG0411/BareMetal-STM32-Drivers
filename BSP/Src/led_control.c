/**
 * @file led_control.c
 * @author BUI_TAN_HUNG
 * @brief Register-direct implementation of GPIO LED state control for STM32F1xx.
 * @version 1.0.0
 * @date 2026-06-06
 *
 * @copyright Copyright (c) 2026 BUI_TAN_HUNG
 */

#include "led_control.h"
#include "gpio.h"

void LED_Init(LED_Device *led)
{
    /* Force push-pull output at maximum GPIO speed to guarantee a clean digital drive.
     * The caller's gpio_config is updated so the struct stays consistent with the
     * hardware state; any pre-existing mode/cnf values are intentionally discarded. */
    led->gpio_config.cnf  = 0x0; /* Push-pull: no open-drain, no internal pull-up/down. */
    led->gpio_config.mode = 0x3; /* 50 MHz output speed — prevents slow rise/fall times. */
    GPIO_Init(&led->gpio_config);
}

void LED_SetState(LED_Device *led)
{
    PIN_STATE pin_state = (led->current_state == LED_ON) ? set : reset;
    GPIO_WritePin(&led->gpio_config, pin_state);
}

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

    /* Propagate the updated logical state to the hardware immediately so that
     * led->current_state and the physical pin level never disagree. */
    LED_SetState(led);
}
