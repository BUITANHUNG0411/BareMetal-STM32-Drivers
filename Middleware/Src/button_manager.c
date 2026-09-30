/**
 * @file button_manager.c
 * @author BUI_TAN_HUNG
 * @brief Event-driven button debounce middleware for STM32F1xx.
 *        Coordinates EXTI press detection, timer-based debounce, and LED output
 *        through a two-state FSM driven entirely from interrupt context.
 * @version 1.0.0
 * @date 2026-09-28
 *
 * @copyright Copyright (c) 2026 BUI_TAN_HUNG
 */

#include "button_manager.h"
#include "nvic.h"

/* ---------------------------------------------------------------------------
 * Module-private state
 * Both variables are written exclusively through the public API so that
 * external code cannot bypass the debounce FSM or corrupt the hardware config.
 * --------------------------------------------------------------------------*/
static Button_State        s_state;
static ButtonManager_Config s_cfg;

void ButtonManager_Init(ButtonManager_Config *config)
{
    Button_Exti_Init(config->btn);
    TIMER_BASE_Init(config->tim, config->timer_number, config->tim_cfg);
    LED_Init(config->led);

    /* Start in IDLE so the very first button press is captured without needing
     * an explicit reset call from the application layer. */
    s_state = BUTTON_IDLE;

    /* Copy the caller's config by value so this module holds its own reference.
     * This prevents a dangling-pointer scenario if the caller's struct goes out
     * of scope (e.g., a local variable in main() before the infinite loop). */
    s_cfg = *config;

    /* Enable button external and debounce timer interrupt in the NVIC */
    NVIC_Enable(TIM2_IRQn);
    NVIC_Enable(EXTI0_IRQn);
}

void ButtonManager_EXTI_Handler(void)
{
    /* Acknowledge the EXTI event immediately so the pending flag is cleared before
     * the handler returns; leaving it set would cause continuous re-entry. */
    Button_Exti_ClearFlag(s_cfg.btn);

    /* Discard edges that arrive while the debounce window is open — contact bounce
     * produces multiple transitions within microseconds of the initial press, and
     * only the first one should launch the debounce timer. */
    if (s_state != BUTTON_IDLE) return;

    s_state = BUTTON_DEBOUNCING;

    /* Restart the timer to begin measuring a clean debounce window from this edge,
     * ensuring a previous partial count cannot shorten the intended timeout. */
    TIMER_Restart(s_cfg.tim);
}

void ButtonManager_TIM_Handler(void)
{
    /* The full debounce period has elapsed with no additional edges, confirming
     * that the initial transition was a genuine button press, not contact bounce. */
    TIMER_Stop(s_cfg.tim);
    TIMER_ClearFlag(s_cfg.tim);

    /* Toggle the LED to provide visible feedback for each validated press event. */
    LED_Toggle(s_cfg.led);

    /* Return to IDLE to re-arm the subsystem for the next button interaction. */
    s_state = BUTTON_IDLE;
}