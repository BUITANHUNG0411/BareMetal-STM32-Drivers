#include "button_manager.h"

int main(void) 
{
    static Button_Device s_btn = {.gpio_config = {.port_base = GPIOA_BASE,
                                                 .pin_number = 0,
                                                 .mode = 00,
                                                 .cnf = 01}};

    static LED_Device s_led = {.gpio_config = {.port_base =  GPIOA_BASE,
                                             .pin_number = 5,
                                             .mode = 10,
                                             .cnf = 00},
                                             .current_state = LED_OFF};

    static TIMER_Config s_tim_cfg = {.Prescaler = 7, .AutoReload = 19999};

    static ButtonManager_Config s_btn_cfg = {.btn = &s_btn,
                                             .tim = TIM2,
                                             .timer_number = 2,
                                             .tim_cfg = &s_tim_cfg,
                                             .led = &s_led};
    ButtonManager_Init(&s_btn_cfg);
    while (1)
    {
    
    }
}