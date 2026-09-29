/**
 * @file button_exti.h
 * @author BUI_TAN_HUNG
 * @brief Declares the Button_Device type and EXTI-based button driver API for STM32F1xx.
 * @version 1.0.0
 * @date 2026-09-28
 *
 * @copyright Copyright (c) 2026
 *
 */

#pragma once
#include "gpio.h"
typedef unsigned int    UINT32;

typedef struct{
    GPIO_PinConfig gpio_config;
} Button_Device;

/**
 * @brief  Configure the button GPIO pin and arm its EXTI line for falling-edge interrupts.
 * @note   Enables the AFIO clock via RCC_APB2ENR bit 0, maps the pin to the EXTI line
 *         through AFIO_EXTICR1 (offset 0x08), sets the falling-edge trigger in EXTI_FTSR
 *         (offset 0x0C), and unmasks the line in EXTI_IMR (offset 0x00).
 * @param  btn  Pointer to a Button_Device whose gpio_config identifies the port and pin.
 */
void Button_Exti_Init(Button_Device *btn);

/**
 * @brief  Clear the pending EXTI interrupt flag for the button's pin.
 * @note   Writes a 1 to the corresponding bit in EXTI_PR (offset 0x14); on STM32F1xx,
 *         writing 1 to a PR bit clears the pending flag (write-1-to-clear semantics).
 * @param  btn  Pointer to the Button_Device whose EXTI pending bit is to be cleared.
 */
void Button_Exti_ClearFlag(Button_Device *btn);