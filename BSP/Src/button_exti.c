/**
 * @file button_exti.c
 * @author BUI_TAN_HUNG
 * @brief Implements EXTI-based falling-edge interrupt initialization and flag clearing for STM32F1xx buttons.
 * @version 1.0.0
 * @date 2026-09-28
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "button_exti.h"
#include "gpio.h"

#define AFIO_BASE       0x40010000
#define AFIO_EXTICR1    (*(volatile UINT32*)(AFIO_BASE + 0x08))
#define AFIO_EXTICR2    (*(volatile UINT32*)(AFIO_BASE + 0x0C))
#define AFIO_EXTICR3    (*(volatile UINT32*)(AFIO_BASE + 0x10))
#define AFIO_EXTICR4    (*(volatile UINT32*)(AFIO_BASE + 0x14))

#define EXTI_BASE       0x40010400
#define EXTI_IMR        (*(volatile UINT32*)(EXTI_BASE + 0x00))
#define EXTI_FTSR       (*(volatile UINT32*)(EXTI_BASE + 0x0C))
#define EXTI_PR         (*(volatile UINT32*)(EXTI_BASE + 0x14))

/**
 * @brief  Configure the button GPIO pin and arm its EXTI line for falling-edge interrupts.
 * @note   Enables the AFIO clock via RCC_APB2ENR bit 0, then routes the pin to EXTI line 0
 *         by clearing and setting bits [3:0] of AFIO_EXTICR1 (offset 0x08). Configures
 *         a falling-edge trigger via EXTI_FTSR bit 0 (offset 0x0C), and unmasks the line
 *         in EXTI_IMR bit 0 (offset 0x00). The GPIO pin is set to floating input
 *         (CNF=10, MODE=00) via GPIOx_CRL/CRH through GPIO_Init.
 * @param  btn  Pointer to a Button_Device whose gpio_config identifies the port and pin.
 */
void Button_Exti_Init(Button_Device *btn)
{
    //enable GIPIOA-0
    btn->gpio_config.cnf = 0x2;
    btn->gpio_config.mode = 0x0;
    GPIO_Init(&btn->gpio_config);

    RCC_APB2ENR |= (1 << 0);

    AFIO_EXTICR1 &= ~(0xF << 0);
    AFIO_EXTICR1 |= (0x00 << 0);

    EXTI_FTSR &= ~(1U << 0);
    EXTI_FTSR |= (1U << 0);
    EXTI_IMR |= (1U << 0);

}

/**
 * @brief  Clear the pending EXTI interrupt flag for the button's pin.
 * @note   Writes a 1 to bit 0 of EXTI_PR (offset 0x14), which uses write-1-to-clear
 *         semantics on STM32F1xx to acknowledge and dismiss the pending EXTI line 0 request.
 * @param  btn  Pointer to the Button_Device whose EXTI_PR pending bit is to be cleared.
 */
void Button_Exti_ClearFlag(Button_Device *btn)
{
    EXTI_PR |= (1U << 0);
}