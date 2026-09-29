#pragma once

#include "button_exti.h"
#include "led_control.h"
#include "timer_reg.h"

typedef enum {
    BUTTON_IDLE = 0,
    BUTTON_DEBOUNCING
} Button_State;

typedef struct {
    Button_Device  *btn;
    TIMER_Typedef  *tim;
    UINT8           timer_number;
    TIMER_Config   *tim_cfg;
    LED_Device     *led;
} ButtonManager_Config;

void ButtonManager_Init(ButtonManager_Config *config);
void ButtonManager_EXTI_Handler();
void ButtonManager_TIM_Handler();