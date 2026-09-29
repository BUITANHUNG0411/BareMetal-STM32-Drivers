/**
 * @file gpio.h
 * @author BUI_TAN_HUNG
 * @brief Register-direct GPIO driver for STM32F1xx — port clock enable, pin
 *        configuration (MODE/CNF), atomic output control, and input sampling.
 * @version 1.0.0
 * @date 2026-09-28
 *
 * @copyright Copyright (c) 2026 BUI_TAN_HUNG
 */

#pragma once

#include "rcc.h"

/* ---------------------------------------------------------------------------
 * Port base addresses  (APB2 bus, RM0008 §3.3)
 * --------------------------------------------------------------------------*/

/**
 * @brief Physical base addresses for all GPIO ports on STM32F1xx.
 *
 * @note  Each port occupies a 1 KB region on the APB2 bus. The enum is used as
 *        a typed address rather than a raw integer to prevent accidental misuse.
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

/**
 * @brief Logical output level for a GPIO pin driven through BSRR.
 */
typedef enum {
    reset = 0, /**< Drive the output line low  (assert BR bit in BSRR). */
    set        /**< Drive the output line high (assert BS bit in BSRR). */
} PIN_STATE;

/**
 * @brief Aggregates the hardware parameters needed to configure a single GPIO pin.
 *
 * @note  MODE and CNF encodings follow RM0008 Table 20 (output) and Table 21 (input).
 *        Caller is responsible for choosing a combination that is valid for the
 *        intended function (e.g., MODE=00 is mandatory for any input configuration).
 */
typedef struct {
    PORT_BASE port_base;  /**< Base address of the target GPIO port.                    */
    UINT8     pin_number; /**< Pin index within the port, range [0, 15].               */
    UINT8     mode;       /**< 2-bit MODE field: input (00) or output speed (01/10/11). */
    UINT8     cnf;        /**< 2-bit CNF field: input/output sub-type (RM0008 §9.2.1). */
} GPIO_PinConfig;

/* ---------------------------------------------------------------------------
 * Public API
 * --------------------------------------------------------------------------*/

/**
 * @brief  Enable the GPIO port clock and program the pin's MODE/CNF fields.
 *
 * @param  gpio_config  Pointer to a fully populated GPIO_PinConfig. Must not be NULL.
 *                      @c pin_number must be in the range [0, 15].
 * @return void
 * @note   Enables the IOPxEN bit in RCC->APB2ENR before accessing CRL/CRH so the
 *         peripheral bus is clocked; writing to an unclocked peripheral register is
 *         silently ignored on Cortex-M3 and produces undefined hardware state.
 */
void GPIO_Init(GPIO_PinConfig *gpio_config);

/**
 * @brief  Atomically set or reset a GPIO output pin via the BSRR register.
 *
 * @param  gpio_config  Pointer to a configured GPIO_PinConfig. Must not be NULL.
 * @param  state        Desired output level: @c set (high) or @c reset (low).
 * @return void
 * @note   BSRR is a write-only, split-word register; each half-word targets either
 *         the set or reset function without a read-modify-write cycle, making this
 *         call safe from interrupt context with no risk of race conditions on the ODR.
 */
void GPIO_WritePin(GPIO_PinConfig *gpio_config, PIN_STATE state);

/**
 * @brief  Sample the current logical level of a GPIO pin via the IDR register.
 *
 * @param  port  Base address of the GPIO port to read.
 * @param  pin   Pin index to sample, range [0, 15].
 * @return UINT8  1 if the pad is driven high, 0 if driven low, as captured by IDR.
 * @note   IDR reflects the actual pad voltage regardless of the ODR content, so this
 *         function is valid for both input and output pins (e.g., verifying drive level).
 */
UINT8 GPIO_ReadPin(PORT_BASE port, UINT8 pin);