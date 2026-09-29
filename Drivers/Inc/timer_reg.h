#pragma once
#include "gpio.h"

#define TIM2_BASE 0x40000000
#define TIM3_BASE 0x40000400
#define TIM4_BASE 0x40000800
#define TIM5_BASE 0x40000C00

#define TIM2 (TIMER_Typedef *)TIM2_BASE
#define TIM3 (TIMER_Typedef *)TIM3_BASE
#define TIM4 (TIMER_Typedef *)TIM4_BASE
#define TIM5 (TIMER_Typedef *)TIM5_BASE

typedef struct {
    volatile UINT32 CR1;
    volatile UINT32 CR2;
    volatile UINT32 SMCR;
    volatile UINT32 DIER;
    volatile UINT32 SR;
    volatile UINT32 EGR;
    volatile UINT32 CCMR1;
    volatile UINT32 CCMR2;
    volatile UINT32 CCER;
    volatile UINT32 CNT;
    volatile UINT32 PSC;
    volatile UINT32 ARR;
    volatile UINT32 CCR1;
    volatile UINT32 CCR2;
    volatile UINT32 CCR3;
    volatile UINT32 CCR4;
    volatile UINT32 DRC;
    volatile UINT32 DMAR;
} TIMER_Typedef;

typedef struct {
    UINT32 Prescaler;
    UINT32 AutoReload;
} TIMER_Config;

void TIMER_BASE_Init(TIMER_Typedef *TIMx, UINT8 timer_number, TIMER_Config *timer_config);
void TIMER_Start(TIMER_Typedef *TIMx);
void TIMER_Stop(TIMER_Typedef *TIMx);
void TIMER_Restart(TIMER_Typedef *TIMx);
void TIMER_ClearFlag(TIMER_Typedef *TIMx);