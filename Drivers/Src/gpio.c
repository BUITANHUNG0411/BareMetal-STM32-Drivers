/**
 * @file gpio.c
 * @author BUI_TAN_HUNG
 * @brief Implements low-level GPIO clock enable and pin configuration/write for STM32F1xx.
 * @version 1.0.0
 * @date 2026-09-28
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "gpio.h"

/**
 * @brief  Enable the GPIO port clock and configure the pin mode and CNF bits.
 * @note   Sets the corresponding IOPxEN bit in RCC_APB2ENR (offset 0x18) to enable
 *         the port clock. Then writes the 4-bit CNF[1:0]:MODE[1:0] field into either
 *         GPIOx_CRL (offset 0x00, pins 0–7) or GPIOx_CRH (offset 0x04, pins 8–15),
 *         at bit position (pin_number % 8) * 4.
 * @param  gpio_config  Pointer to a GPIO_PinConfig structure carrying port_base,
 *                      pin_number, mode, and cnf fields.
 */
void GPIO_Init(GPIO_PinConfig *gpio_config)
{
    switch (gpio_config->port_base)
    {
        /* Enable GPIOx peripheral clock via RCC->APB2ENR (RM0008 §7.3.7). */

        case GPIOA_BASE: RCC->APB2ENR |= (1U << 2); break;
        case GPIOB_BASE: RCC->APB2ENR |= (1U << 3); break;
        case GPIOC_BASE: RCC->APB2ENR |= (1U << 4); break;
        case GPIOD_BASE: RCC->APB2ENR |= (1U << 5); break;
        case GPIOE_BASE: RCC->APB2ENR |= (1U << 6); break;
        case GPIOF_BASE: RCC->APB2ENR |= (1U << 7); break;
        case GPIOG_BASE: RCC->APB2ENR |= (1U << 8); break;
    }

    UINT32 cnf_mode_value = (gpio_config->cnf << 2) | gpio_config->mode;
    if (gpio_config->pin_number < 8)
    {
        (*(volatile UINT32*)(gpio_config->port_base + 0x00)) &= ~(0xF << (gpio_config->pin_number * 4));        /* Clear 4-bit field in GPIOx_CRL. */
        (*(volatile UINT32*)(gpio_config->port_base + 0x00)) |= (cnf_mode_value << (gpio_config->pin_number * 4));         /* Set MODE=11, CNF=00. */
        return;
    }
    else
    {
        (*(volatile UINT32*)(gpio_config->port_base + 0x04)) &= ~(0xF << ((gpio_config->pin_number-8) * 4));
        (*(volatile UINT32*)(gpio_config->port_base + 0x04)) |= (cnf_mode_value << ((gpio_config->pin_number-8) * 4));
        return;
    }
}

/**
 * @brief  Drive a GPIO pin high or low by writing to the port's BSRR register.
 * @note   Accesses GPIOx_BSRR (offset 0x10): writing a 1 to BSy (bit y, lower half)
 *         atomically sets the pin; writing a 1 to BRy (bit y+16, upper half)
 *         atomically resets the pin. No read-modify-write is needed.
 * @param  gpio_config  Pointer to a GPIO_PinConfig structure identifying the port and pin.
 * @param  state        Desired pin level: set (drive high) or reset (drive low).
 */
void GPIO_WritePin(GPIO_PinConfig *gpio_config, PIN_STATE state)
{
    switch (state)
    {
        /* GPIOx_BSRR BRy — reset pin to 0. */
        case reset: (*(volatile UINT32*)(gpio_config->port_base + 0x10)) |= (1U << (gpio_config->pin_number + 16)); return; 
        /* GPIOx_BSRR BSy — set pin to 1. */
        case set: (*(volatile UINT32*)(gpio_config->port_base + 0x10)) |= (1U << gpio_config->pin_number); break;
    }
}