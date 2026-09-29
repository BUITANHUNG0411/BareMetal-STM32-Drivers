/**
 * @file gpio.c
 * @author BUI_TAN_HUNG
 * @brief Register-direct implementation of GPIO clock enable, pin configuration,
 *        and atomic output control for STM32F1xx.
 * @version 1.0.0
 * @date 2026-09-28
 *
 * @copyright Copyright (c) 2026 BUI_TAN_HUNG
 */

#include "gpio.h"

void GPIO_Init(GPIO_PinConfig *gpio_config)
{
    switch (gpio_config->port_base)
    {
        /* Gate the APB2 peripheral clock for the target port before touching its registers.
         * Accessing an unclocked peripheral is silently ignored by the bus fabric, which
         * would leave the pin in its reset (floating input) state without any error signal. */
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
        /* Pins 0–7 reside in CRL; each occupies a 4-bit field at bit position pin*4.
         * The field is cleared first so that writing a new value is always deterministic,
         * regardless of the hardware power-on state of those bits. */
        (*(volatile UINT32*)(gpio_config->port_base + 0x00)) &= ~(0xF << (gpio_config->pin_number * 4));
        (*(volatile UINT32*)(gpio_config->port_base + 0x00)) |=  (cnf_mode_value << (gpio_config->pin_number * 4));
        return;
    }
    else
    {
        /* Pins 8–15 reside in CRH; the pin index is rebased to [0, 7] to address
         * the correct 4-bit field within the higher control register. */
        (*(volatile UINT32*)(gpio_config->port_base + 0x04)) &= ~(0xF << ((gpio_config->pin_number - 8) * 4));
        (*(volatile UINT32*)(gpio_config->port_base + 0x04)) |=  (cnf_mode_value << ((gpio_config->pin_number - 8) * 4));
        return;
    }
}

void GPIO_WritePin(GPIO_PinConfig *gpio_config, PIN_STATE state)
{
    switch (state)
    {
        /* Writing to the BR half of BSRR pulls the line low without a read cycle,
         * eliminating the read-modify-write window where an interrupt could corrupt ODR. */
        case reset:
            (*(volatile UINT32*)(gpio_config->port_base + 0x10)) |= (1U << (gpio_config->pin_number + 16));
            return;

        /* Writing to the BS half of BSRR drives the line high with the same atomicity. */
        case set:
            (*(volatile UINT32*)(gpio_config->port_base + 0x10)) |= (1U << gpio_config->pin_number);
            break;
    }
}