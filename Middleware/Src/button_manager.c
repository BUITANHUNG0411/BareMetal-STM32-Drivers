#include "button_manager.h"
#include "button_exti.h"
#include "led_control.h"
#include "stm32f103xb.h"
#include "timer_reg.h"

static Button_State s_state;
static ButtonManager_Config s_cfg;

void ButtonManager_Init(ButtonManager_Config *config)
{
    Button_Exti_Init(config->btn);
    TIMER_BASE_Init(config->tim, config->timer_number, config->tim_cfg);
    LED_Init(config->led);
    s_state = BUTTON_IDLE;
    s_cfg = *config;
}

void ButtonManager_EXTI_Handler()
{
    Button_Exti_ClearFlag(s_cfg.btn);
    if (s_state != BUTTON_IDLE) return;
    s_state = BUTTON_DEBOUNCING;
    TIMER_Restart(s_cfg.tim);
}

void ButtonManager_TIM_Handler()
{
    TIMER_Stop(s_cfg.tim);
    TIMER_ClearFlag(s_cfg.tim);
    LED_Toggle(s_cfg.led);
    s_state = BUTTON_IDLE;
}