/**
 * @file timer_reg.h
 * @author BUI_TAN_HUNG
 * @brief Register-direct driver interface for STM32F1xx general-purpose timers TIM2–TIM5.
 *        Provides base-timer initialisation, start/stop/restart control, and interrupt
 *        flag management without relying on the HAL abstraction layer.
 * @version 1.0.0
 * @date 2026-09-28
 *
 * @copyright Copyright (c) 2026 BUI_TAN_HUNG
 */

#pragma once
#include "gpio.h"

/* ---------------------------------------------------------------------------
 * Timer base addresses  (APB1 bus, RM0008 §2.3)
 * --------------------------------------------------------------------------*/
#define TIM2_BASE 0x40000000UL
#define TIM3_BASE 0x40000400UL
#define TIM4_BASE 0x40000800UL
#define TIM5_BASE 0x40000C00UL

/** @brief Convenience pointer macros that cast the base address to the register struct. */
#define TIM2 ((TIMER_Typedef *)TIM2_BASE)
#define TIM3 ((TIMER_Typedef *)TIM3_BASE)
#define TIM4 ((TIMER_Typedef *)TIM4_BASE)
#define TIM5 ((TIMER_Typedef *)TIM5_BASE)

/* ---------------------------------------------------------------------------
 * Register layout
 * --------------------------------------------------------------------------*/

/**
 * @brief Memory-mapped register layout for STM32F1xx general-purpose timers (RM0008 §15.4).
 *
 * @note  This layout is identical for TIM2, TIM3, TIM4, and TIM5. Fields that do
 *        not exist on a specific timer (e.g., CCR4 on TIM3) read as zero and should
 *        not be written; consulting RM0008 §15 before extending this driver is advised.
 */
typedef struct {
    volatile UINT32 CR1;   /**< Control register 1            (offset 0x00) */
    volatile UINT32 CR2;   /**< Control register 2            (offset 0x04) */
    volatile UINT32 SMCR;  /**< Slave mode control register   (offset 0x08) */
    volatile UINT32 DIER;  /**< DMA/interrupt enable register (offset 0x0C) */
    volatile UINT32 SR;    /**< Status register               (offset 0x10) */
    volatile UINT32 EGR;   /**< Event generation register     (offset 0x14) */
    volatile UINT32 CCMR1; /**< Capture/compare mode 1        (offset 0x18) */
    volatile UINT32 CCMR2; /**< Capture/compare mode 2        (offset 0x1C) */
    volatile UINT32 CCER;  /**< Capture/compare enable        (offset 0x20) */
    volatile UINT32 CNT;   /**< Counter value                 (offset 0x24) */
    volatile UINT32 PSC;   /**< Prescaler                     (offset 0x28) */
    volatile UINT32 ARR;   /**< Auto-reload register          (offset 0x2C) */
    volatile UINT32 CCR1;  /**< Capture/compare register 1    (offset 0x34) */
    volatile UINT32 CCR2;  /**< Capture/compare register 2    (offset 0x38) */
    volatile UINT32 CCR3;  /**< Capture/compare register 3    (offset 0x3C) */
    volatile UINT32 CCR4;  /**< Capture/compare register 4    (offset 0x40) */
    volatile UINT32 DRC;   /**< DMA control register          (offset 0x48) */
    volatile UINT32 DMAR;  /**< DMA address for burst         (offset 0x4C) */
} TIMER_Typedef;

/**
 * @brief Timer initialisation parameters expressed in raw hardware register units.
 *
 * @note  Effective overflow period: T = (Prescaler + 1) * (AutoReload + 1) / f_PCLK1.
 *        With PCLK1 = 8 MHz, Prescaler = 7 and AutoReload = 19999 yields T ≈ 20 ms.
 */
typedef struct {
    UINT32 Prescaler;  /**< PSC register value; divides the APB1 timer clock by (Prescaler + 1). */
    UINT32 AutoReload; /**< ARR register value; counter wraps after (AutoReload + 1) ticks.      */
} TIMER_Config;

/* ---------------------------------------------------------------------------
 * Public API
 * --------------------------------------------------------------------------*/

/**
 * @brief  Enable the timer APB1 clock, program PSC/ARR, and arm the update interrupt.
 *
 * @param  TIMx          Pointer to the target timer register block (use TIM2–TIM5 macros).
 * @param  timer_number  Logical timer index in [2, 5]; determines the APB1ENR clock-gate bit.
 * @param  timer_config  Pointer to a TIMER_Config with the desired Prescaler and AutoReload.
 *                       Must not be NULL.
 * @return void
 * @note   Does not start the counter; call TIMER_Start() to begin counting.
 *         The update interrupt (UIE) is armed so the ISR fires on each ARR overflow,
 *         which is the mechanism used to signal the end of the debounce window.
 */
void TIMER_BASE_Init(TIMER_Typedef *TIMx, UINT8 timer_number, TIMER_Config *timer_config);

/**
 * @brief  Start the timer counter.
 *
 * @param  TIMx  Pointer to the target timer register block.
 * @return void
 * @note   Sets the CEN bit in CR1; the counter begins incrementing on the very next
 *         APB1 clock edge after the write propagates through the APB bus.
 */
void TIMER_Start(TIMER_Typedef *TIMx);

/**
 * @brief  Stop the timer counter and reset the count to zero.
 *
 * @param  TIMx  Pointer to the target timer register block.
 * @return void
 * @note   CEN is cleared before CNT is zeroed to prevent a spurious update event
 *         that would otherwise fire if CNT wrapped to ARR during the reset write.
 */
void TIMER_Stop(TIMER_Typedef *TIMx);

/**
 * @brief  Stop, reset, and immediately restart the timer in one sequence.
 *
 * @param  TIMx  Pointer to the target timer register block.
 * @return void
 * @note   Intended for debounce re-arm: guarantees that the full debounce window
 *         restarts cleanly from zero, discarding any partial count accumulated
 *         from a preceding bounce edge.
 */
void TIMER_Restart(TIMER_Typedef *TIMx);

/**
 * @brief  Clear the timer update interrupt flag.
 *
 * @param  TIMx  Pointer to the target timer register block.
 * @return void
 * @note   SR bits are rc_w0 (read, clear by writing 0). This must be called at the
 *         beginning of the timer ISR; leaving the flag set causes the handler to
 *         re-enter the moment it returns, creating an infinite interrupt loop.
 */
void TIMER_ClearFlag(TIMER_Typedef *TIMx);