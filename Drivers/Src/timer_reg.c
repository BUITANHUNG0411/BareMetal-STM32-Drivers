#include "timer_reg.h"

void TIMER_BASE_Init(TIMER_Typedef *TIMx, UINT8 timer_number, TIMER_Config *timer_config)
{
    RCC->APB1ENR |= (1U << (timer_number - 2));
    TIMx->PSC = timer_config->Prescaler;
    TIMx->ARR = timer_config->AutoReload;
    TIMx->DIER |= (1 << 0); //UIE
}

void TIMER_Start(TIMER_Typedef *TIMx)
{
    TIMx->CR1 |= (1 << 0); //CEN = 1
}

void TIMER_Stop(TIMER_Typedef *TIMx)
{
    TIMx->CR1 &= ~(1 << 0); //CEN = 0
    TIMx->CNT = 0;
}

void TIMER_ClearFlag(TIMER_Typedef *TIMx)
{
    TIMx->SR &= ~(1 << 0); //UIF
}

void TIMER_Restart(TIMER_Typedef *TIMx)
{
    TIMER_Stop(TIMx);
    TIMER_Start(TIMx);
}