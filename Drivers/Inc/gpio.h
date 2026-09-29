#pragma once

#include "rcc.h"

/**
 * @brief Definition BASE ADDRESS of GPIO.
 */
typedef enum {
    GPIOA_BASE = 0x40010800,
    GPIOB_BASE = 0x40010C00,
    GPIOC_BASE = 0x40011000,
    GPIOD_BASE = 0x40011400,
    GPIOE_BASE = 0x40011800,
    GPIOF_BASE = 0x40011C00,
    GPIOG_BASE = 0x40012000
} PORT_BASE;

typedef enum {
    reset = 0,
    set
} PIN_STATE;

typedef struct {
    PORT_BASE port_base;
    UINT8 pin_number;
    UINT8 mode;
    UINT8 cnf;
} GPIO_PinConfig;

/**
 * @brief  Enable the GPIO port clock and configure the pin mode and CNF bits.
 * @note   Sets the corresponding IOPxEN bit in RCC_APB2ENR (offset 0x18), then writes
 *         the 4-bit CNF[1:0]:MODE[1:0] field into GPIOx_CRL (offset 0x00, pins 0–7)
 *         or GPIOx_CRH (offset 0x04, pins 8–15) at position (pin_number % 8) * 4.
 * @param  gpio_config  Pointer to a GPIO_PinConfig carrying port_base, pin_number, mode, and cnf.
 */
void GPIO_Init(GPIO_PinConfig *gpio_config);

/**
 * @brief  Drive a GPIO pin high or low via the port's BSRR register.
 * @note   Writes to GPIOx_BSRR (offset 0x10): BSy bit (y = pin_number) sets the pin;
 *         BRy bit (y+16) resets the pin — both operations are atomic, no read needed.
 * @param  gpio_config  Pointer to a GPIO_PinConfig identifying the target port and pin.
 * @param  state        Desired output level: set (high) or reset (low).
 */
void GPIO_WritePin(GPIO_PinConfig *gpio_config, PIN_STATE state);

/**
 * @brief  Read the current logical level of a GPIO input pin.
 * @note   Samples GPIOx_IDR (offset 0x08) and extracts bit at position pin_number
 *         to return the current input data register value for that pin.
 * @param  port  Base address of the GPIO port to read from.
 * @param  pin   Pin number (0–15) to sample.
 */
UINT8 GPIO_ReadPin(PORT_BASE port, UINT8 pin);