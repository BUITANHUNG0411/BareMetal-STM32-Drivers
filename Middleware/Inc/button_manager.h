/**
 * @file button_manager.h
 * @author BUI_TAN_HUNG
 * @brief Middleware combining EXTI button detection, timer-based hardware debounce,
 *        and LED output into a single event-driven subsystem for STM32F1xx.
 *
 *        Architecture:
 *          - EXTI fires on the falling edge of the button press.
 *          - A general-purpose timer measures a fixed debounce window.
 *          - If the timer expires without a subsequent EXTI edge, the press is
 *            validated and the LED is toggled.
 *          - Any edges arriving during the debounce window are discarded.
 *
 * @version 1.0.0
 * @date 2026-09-28
 *
 * @copyright Copyright (c) 2026 BUI_TAN_HUNG
 */

#pragma once

#include "button_manager.h"
#include "button_exti.h"
#include "led_control.h"
#include "timer_reg.h"
#include "nvic.h"

/**
 * @brief States of the internal debounce finite state machine.
 */
typedef enum {
    BUTTON_IDLE       = 0, /**< Subsystem is idle; EXTI is armed and waiting for a press.  */
    BUTTON_DEBOUNCING      /**< Debounce timer is running; additional EXTI edges are ignored. */
} Button_State;

/**
 * @brief Aggregates all hardware resources managed by the ButtonManager subsystem.
 *
 * @note  All pointer members must remain valid for the lifetime of the subsystem.
 *        Passing stack-allocated objects that go out of scope leads to undefined behaviour
 *        when the ISR handlers dereference the stale pointers.
 */
typedef struct {
    Button_Device  *btn;          /**< Button device to monitor for press events.           */
    TIMER_Typedef  *tim;          /**< Timer peripheral used to measure the debounce window. */
    UINT8           timer_number; /**< Logical timer index [2–5] for APB1 clock gate setup. */
    TIMER_Config   *tim_cfg;      /**< Prescaler and auto-reload defining the debounce period. */
    LED_Device     *led;          /**< LED device toggled on each validated button press.    */
} ButtonManager_Config;

/**
 * @brief  Initialise all hardware resources owned by the ButtonManager subsystem.
 *
 * @param  config  Pointer to a fully populated ButtonManager_Config. Must not be NULL.
 *                 All nested pointers (btn, tim, tim_cfg, led) must also be valid.
 * @return void
 * @note   Must be called once before enabling the EXTI and timer interrupts in the NVIC.
 *         Calling this function again re-initialises all peripherals and resets the
 *         debounce state machine to BUTTON_IDLE.
 */
void ButtonManager_Init(ButtonManager_Config *config);

/**
 * @brief  EXTI interrupt service routine entry point for button press detection.
 *
 * @return void
 * @note   Must be called from EXTI0_IRQHandler (or the EXTIx handler matching the
 *         button pin). Clears the EXTI pending flag, ignores edges during debounce,
 *         and starts the debounce timer on the first edge detected in the idle state.
 */
void ButtonManager_EXTI_Handler(void);

/**
 * @brief  Timer update interrupt service routine entry point for debounce timeout.
 *
 * @return void
 * @note   Must be called from the TIMx_IRQHandler matching the timer passed in
 *         ButtonManager_Config. Stops the timer, clears the update flag, toggles
 *         the LED to acknowledge the validated press, and returns to BUTTON_IDLE.
 */
void ButtonManager_TIM_Handler(void);