/**
 * @file rcc.c
 * @author BUI_TAN_HUNG
 * @brief Register-direct RCC clock configuration implementation for STM32F1xx.
 *
 *        Configures the system clock to 72 MHz by enabling the external HSE
 *        oscillator, programming the PLL, setting Flash wait states, and
 *        switching the SYSCLK mux — all at the register level without HAL.
 *
 * @version 1.1.0
 * @date 2026-09-29
 *
 * @copyright Copyright (c) 2026 BUI_TAN_HUNG
 */

#include "rcc.h"

/* ---------------------------------------------------------------------------
 * RCC_CR register bit positions  (RM0008 §7.3.1)
 * --------------------------------------------------------------------------*/
#define RCC_CR_HSEON   16U /**< HSE oscillator enable.                      */
#define RCC_CR_HSERDY  17U /**< HSE oscillator ready flag (hardware-set).   */
#define RCC_CR_PLLON   24U /**< PLL enable.                                 */
#define RCC_CR_PLLRDY  25U /**< PLL locked flag (hardware-set).             */

/* ---------------------------------------------------------------------------
 * RCC_CFGR register field values  (RM0008 §7.3.2)
 * --------------------------------------------------------------------------*/
/** SW[1:0] = 10 — select PLL as SYSCLK source. */
#define RCC_CFGR_SW_PLL      (2U << 0U)
/** SWS[1:0] = 10 — read-back: PLL is active SYSCLK (hardware-set). */
#define RCC_CFGR_SWS_PLL     (2U << 2U)
/** SWS field mask [3:2]. */
#define RCC_CFGR_SWS_MASK    (3U << 2U)
/** PPRE1[2:0] = 100 — APB1 clock = HCLK / 2 (must not exceed 36 MHz). */
#define RCC_CFGR_PPRE1_DIV2  (4U << 8U)
/** PLLSRC = 1 — HSE oscillator drives the PLL input. */
#define RCC_CFGR_PLLSRC_HSE  (1U << 16U)
/** PLLMUL[3:0] = 0111 — multiply PLL input by 9 (encoding: 7 + 2 = 9). */
#define RCC_CFGR_PLLMUL_9    (7U << 18U)

/* ---------------------------------------------------------------------------
 * Flash ACR bit positions  (RM0008 §3.3.3)
 * --------------------------------------------------------------------------*/
/** LATENCY[2:0] = 010 — two CPU wait states required for SYSCLK 48–72 MHz. */
#define FLASH_ACR_LATENCY_2  (2U << 0U)
/** PRFTBE = 1 — prefetch buffer enable; pipelines Flash reads to hide latency. */
#define FLASH_ACR_PRFTBE     (1U << 4U)

void RCC_SystemClock_72MHz(void)
{
    /* ------------------------------------------------------------------
     * Step 1 — Enable the HSE oscillator and wait for it to stabilise.
     *
     * HSE is driven by an external crystal (typically 8 MHz on Blue Pill
     * boards). It provides a more accurate and temperature-stable reference
     * than the internal HSI RC oscillator, which is mandatory when the PLL
     * must produce a precise 72 MHz output.
     *
     * HSERDY (CR bit 17) is set by hardware once the oscillator amplitude
     * has reached its steady-state value. Polling is required because the
     * PLL input must be stable before the PLL is enabled; starting the PLL
     * on an unstable clock produces an unpredictable output frequency.
     * ------------------------------------------------------------------ */
    RCC->CR |= (1U << RCC_CR_HSEON);
    while ( !(RCC->CR & (1U << RCC_CR_HSERDY)) ) {}

    /* ------------------------------------------------------------------
     * Step 2 — Set Flash read latency BEFORE raising SYSCLK.
     *
     * Flash memory has a fixed read-access time determined by the silicon
     * process. As SYSCLK rises, more CPU clock cycles elapse during a single
     * Flash read; LATENCY inserts wait states so that data is valid before
     * the CPU samples it on the next rising edge.
     *
     * RM0008 Table 11 wait-state requirements:
     *   0 wait states : SYSCLK ≤ 24 MHz
     *   1 wait state  : 24 MHz < SYSCLK ≤ 48 MHz
     *   2 wait states : 48 MHz < SYSCLK ≤ 72 MHz  ← this configuration
     *
     * The prefetch buffer (PRFTBE) pipelines sequential Flash word fetches,
     * hiding the wait-state penalty during sustained instruction execution.
     *
     * Critical ordering: latency must be programmed BEFORE the clock switch.
     * Increasing SYSCLK first creates a window where the CPU reads Flash
     * without sufficient wait states, producing silent instruction corruption.
     * ------------------------------------------------------------------ */
    FLASH_ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY_2;

    /* ------------------------------------------------------------------
     * Step 3 — Configure PLL source, multiplier, and APB1 prescaler.
     *
     * Clock tree after this step (values not active until Step 5):
     *   HSE         =  8 MHz  (crystal)
     *   PLLSRC      =  HSE    (CFGR bit 16)
     *   PLLMUL      =  ×9    (CFGR bits [21:18] = 0111, encoding 7 means 7+2=9)
     *   PLL output  = 72 MHz
     *   SYSCLK      = 72 MHz  (after Step 5)
     *   HCLK        = 72 MHz  (AHB prescaler = 1, default)
     *   PCLK1       = 36 MHz  (APB1 prescaler = 2; hardware maximum is 36 MHz)
     *   PCLK2       = 72 MHz  (APB2 prescaler = 1, default)
     *
     * The PLL must be configured while PLLON = 0; writing PLL fields after
     * the PLL is running is silently ignored by the hardware (RM0008 §7.2.3).
     * ------------------------------------------------------------------ */
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2 | RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMUL_9;

    /* ------------------------------------------------------------------
     * Step 4 — Enable the PLL and wait for it to lock.
     *
     * PLLRDY (CR bit 25) is set by hardware once the PLL output frequency
     * has settled within its operating specification. The PLL must not be
     * selected as the clock source until this flag is asserted; doing so
     * routes an unstable frequency to the entire chip clock tree.
     * ------------------------------------------------------------------ */
    RCC->CR |= (1U << RCC_CR_PLLON);
    while ( !(RCC->CR & (1U << RCC_CR_PLLRDY)) ) {}

    /* ------------------------------------------------------------------
     * Step 5 — Switch SYSCLK source to the PLL.
     *
     * SW[1:0] (CFGR bits [1:0]) selects the clock source; after writing,
     * SWS[1:0] (CFGR bits [3:2]) reflects the source actually used by the
     * hardware mux. Polling SWS guarantees the switch has propagated before
     * any downstream peripheral is accessed on the new 72 MHz domain.
     * ------------------------------------------------------------------ */
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ( (RCC->CFGR & RCC_CFGR_SWS_MASK) != RCC_CFGR_SWS_PLL ) {}
}