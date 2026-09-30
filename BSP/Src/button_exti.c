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
#include "rcc.h"

/* ---------------------------------------------------------------------------
 * AFIO register map  (APB2 bus, RM0008 §9.4)
 * AFIO is responsible for remapping GPIO pins to alternate functions and for
 * selecting which GPIO port drives each EXTI line.
 * --------------------------------------------------------------------------*/
#define AFIO_BASE    0x40010000UL
// #define AFIO_EXTICR1 (*(volatile UINT32 *)(AFIO_BASE + 0x08))
// #define AFIO_EXTICR2 (*(volatile UINT32 *)(AFIO_BASE + 0x0C))
// #define AFIO_EXTICR3 (*(volatile UINT32 *)(AFIO_BASE + 0x10))
// #define AFIO_EXTICR4 (*(volatile UINT32 *)(AFIO_BASE + 0x14))

#define AFIO_EXTICR(x)     (*(volatile UINT32*)(AFIO_BASE + 0x04 * (x+1)))

/* ---------------------------------------------------------------------------
 * EXTI register map  (APB2 bus, RM0008 §10.3)
 * --------------------------------------------------------------------------*/
#define EXTI_BASE 0x40010400UL
#define EXTI_IMR  (*(volatile UINT32 *)(EXTI_BASE + 0x00))
#define EXTI_FTSR (*(volatile UINT32 *)(EXTI_BASE + 0x0C))
#define EXTI_PR   (*(volatile UINT32 *)(EXTI_BASE + 0x14))

void Button_Exti_Init(Button_Device *btn)
{
    /* Configure the GPIO pin as Input with Pull-up/Pull-down mode. 
     * CNF = 0x2 (Input with pull-up/pull-down), MODE = 0x0 (Input mode). */
    btn->gpio_config.cnf  = 0x2; 
    btn->gpio_config.mode = 0x0;
    GPIO_Init(&btn->gpio_config);
    
    /* Write 1 to ODR to select Pull-Up instead of Pull-Down */
    GPIO_WritePin(&btn->gpio_config, set);

    /* AFIO clock must be enabled before its port-select registers are written;
     * writing to an unclocked peripheral is silently discarded by the APB bridge. */
    RCC->APB2ENR |= (1U << 0);

    /* Determine which AFIO_EXTICR register controls the given pin number.
     * EXTICR1 controls pins 0-3, EXTICR2 controls pins 4-7, etc. */
    UINT32 AFIO_EXTICR_Number;
    if (btn->gpio_config.pin_number < 4)        AFIO_EXTICR_Number = 1;
    else if (btn->gpio_config.pin_number < 8)   AFIO_EXTICR_Number = 2;
    else if (btn->gpio_config.pin_number < 12)  AFIO_EXTICR_Number = 3;
    else                                        AFIO_EXTICR_Number = 4;

    /* Route the specific EXTI line to the configured GPIO port. 
     * The 4-bit field for each pin is cleared first, then set to the target port.
     * STM32F103C8T6 primarily uses GPIOA (0x00), GPIOB (0x01), and GPIOC (0x02). */
    AFIO_EXTICR(AFIO_EXTICR_Number) &= ~(0xF << ((btn->gpio_config.pin_number % 4) * 4));
    switch (btn->gpio_config.port_base)
    {
        case (GPIOA_BASE): 
            AFIO_EXTICR(AFIO_EXTICR_Number) |= (0x00 << ((btn->gpio_config.pin_number % 4) * 4));
            break;
        case (GPIOB_BASE):
            AFIO_EXTICR(AFIO_EXTICR_Number) |= (0x01 << ((btn->gpio_config.pin_number % 4) * 4));
            break;
        case (GPIOC_BASE):
            AFIO_EXTICR(AFIO_EXTICR_Number) |= (0x02 << ((btn->gpio_config.pin_number % 4) * 4));
            break;
    }

    /* Arm the configured EXTI line for falling-edge detection, which corresponds
     * to the button press event where the pad transitions from high (idle) to low (pressed). */
    EXTI_FTSR &= ~(1U << btn->gpio_config.pin_number);
    EXTI_FTSR |=  (1U << btn->gpio_config.pin_number);

    /* Unmask the configured EXTI line in the interrupt mask register so the event is
     * forwarded to the NVIC; a masked line generates no interrupt regardless of the trigger. */
    EXTI_IMR |= (1U << btn->gpio_config.pin_number);
}

void Button_Exti_ClearFlag(Button_Device *btn)
{
    /* Acknowledge the interrupt by clearing the pending flag; EXTI_PR is write-1-to-clear,
     * so omitting this step causes the ISR to re-enter in an infinite loop on return. */
    EXTI_PR = (1U << btn->gpio_config.pin_number);
}