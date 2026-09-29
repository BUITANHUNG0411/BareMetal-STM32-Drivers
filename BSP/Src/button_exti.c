/**
 * @file button_exti.c
 * @author BUI_TAN_HUNG
 * @brief Register-direct EXTI falling-edge interrupt initialisation and flag management
 *        for STM32F1xx push-buttons.
 * @version 1.0.0
 * @date 2026-09-28
 *
 * @copyright Copyright (c) 2026 BUI_TAN_HUNG
 */

#include "button_exti.h"
#include "gpio.h"

/* ---------------------------------------------------------------------------
 * AFIO register map  (APB2 bus, RM0008 §9.4)
 * AFIO is responsible for remapping GPIO pins to alternate functions and for
 * selecting which GPIO port drives each EXTI line.
 * --------------------------------------------------------------------------*/
#define AFIO_BASE    0x40010000UL
#define AFIO_EXTICR1 (*(volatile UINT32 *)(AFIO_BASE + 0x08))
#define AFIO_EXTICR2 (*(volatile UINT32 *)(AFIO_BASE + 0x0C))
#define AFIO_EXTICR3 (*(volatile UINT32 *)(AFIO_BASE + 0x10))
#define AFIO_EXTICR4 (*(volatile UINT32 *)(AFIO_BASE + 0x14))

/* ---------------------------------------------------------------------------
 * EXTI register map  (APB2 bus, RM0008 §10.3)
 * --------------------------------------------------------------------------*/
#define EXTI_BASE 0x40010400UL
#define EXTI_IMR  (*(volatile UINT32 *)(EXTI_BASE + 0x00))
#define EXTI_FTSR (*(volatile UINT32 *)(EXTI_BASE + 0x0C))
#define EXTI_PR   (*(volatile UINT32 *)(EXTI_BASE + 0x14))

void Button_Exti_Init(Button_Device *btn)
{
    /* Set floating input mode so the GPIO pad is high-impedance; the board's
     * external pull-up holds the line high until the button grounds it. */
    btn->gpio_config.cnf  = 0x2;
    btn->gpio_config.mode = 0x0;
    GPIO_Init(&btn->gpio_config);

    /* AFIO clock must be enabled before its port-select registers are written;
     * writing to an unclocked peripheral is silently discarded by the APB bridge. */
    RCC->APB2ENR |= (1U << 0);

    /* Route GPIOA pin 0 to EXTI line 0 by writing the port identifier (0x0 = GPIOA)
     * into the first 4-bit field of EXTICR1. The field is cleared first to remove
     * any previous routing left by a prior configuration or reset state. */
    AFIO_EXTICR1 &= ~(0xF << 0);
    AFIO_EXTICR1 |=  (0x00 << 0);

    /* Arm EXTI line 0 for falling-edge detection, which corresponds to the button
     * press event where the pad transitions from high (idle) to low (pressed). */
    EXTI_FTSR &= ~(1U << 0);
    EXTI_FTSR |=  (1U << 0);

    /* Unmask EXTI line 0 in the interrupt mask register so the event is forwarded
     * to the NVIC; a masked line generates no interrupt regardless of the trigger. */
    EXTI_IMR |= (1U << 0);
}

void Button_Exti_ClearFlag(Button_Device *btn)
{
    /* Acknowledge the interrupt by clearing the pending flag; EXTI_PR is write-1-to-clear,
     * so omitting this step causes the ISR to re-enter in an infinite loop on return. */
    EXTI_PR |= (1U << 0);
}