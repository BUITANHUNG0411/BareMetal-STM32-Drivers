/**
 * @file rcc.h
 * @author BUI_TAN_HUNG
 * @brief Register map and fundamental integer-type definitions for the STM32F1xx Reset
 *        and Clock Control (RCC) peripheral. Acts as the root header included by all
 *        driver layers that require UINT32/UINT8 or direct RCC register access.
 * @version 1.0.0
 * @date 2026-09-29
 *
 * @copyright Copyright (c) 2026 BUI_TAN_HUNG
 */

#pragma once

/* ---------------------------------------------------------------------------
 * Fundamental integer types
 * All driver layers obtain UINT32 and UINT8 from this header so that the
 * definitions are never duplicated across translation units.
 * --------------------------------------------------------------------------*/
typedef unsigned int  UINT32; /**< 32-bit unsigned word, matches ARM Cortex-M register width. */
typedef unsigned char UINT8;  /**< 8-bit unsigned byte.                                       */

/* ---------------------------------------------------------------------------
 * RCC register block  (RM0008 §7.3, base address 0x4002_1000)
 * --------------------------------------------------------------------------*/

/**
 * @brief Memory-mapped register layout for the STM32F1xx RCC peripheral.
 *
 * @note  Field order matches the hardware register map exactly. Any reordering
 *        would silently corrupt peripheral access because the struct is cast
 *        directly onto the bus address.
 */
typedef struct {
    volatile UINT32 CR;        /**< Clock control register                (offset 0x00) */
    volatile UINT32 CFGR;      /**< Clock configuration register          (offset 0x04) */
    volatile UINT32 CIR;       /**< Clock interrupt register              (offset 0x08) */
    volatile UINT32 APB2RSTR;  /**< APB2 peripheral reset register        (offset 0x0C) */
    volatile UINT32 APB1RSTR;  /**< APB1 peripheral reset register        (offset 0x10) */
    volatile UINT32 AHBENR;    /**< AHB peripheral clock enable register  (offset 0x14) */
    volatile UINT32 APB2ENR;   /**< APB2 peripheral clock enable register (offset 0x18) */
    volatile UINT32 APB1ENR;   /**< APB1 peripheral clock enable register (offset 0x1C) */
    volatile UINT32 BDCR;      /**< Backup domain control register        (offset 0x20) */
    volatile UINT32 CSR;       /**< Control/status register               (offset 0x24) */
} RCC_Typedef;

/** @brief Physical base address of the RCC register block on the AHB bus. */
#define RCC_BASE 0x40021000UL

/**
 * @brief Pointer to the RCC register block.
 *
 * @note  Usage:  RCC->APB2ENR |= (1U << 2);   // enable GPIOA clock
 */
#define RCC ((RCC_Typedef *)RCC_BASE)

/* ---------------------------------------------------------------------------
 * Flash memory interface  (RM0008 §3.3, base address 0x4002_2000)
 * The Flash ACR must be programmed with the correct wait-state count before
 * the system clock is raised above the Flash read-access frequency limit.
 * --------------------------------------------------------------------------*/

/** @brief Physical base address of the Flash memory interface register block. */
#define FLASH_BASE 0x40022000UL

/**
 * @brief Flash access control register (ACR).
 *
 *        Relevant fields:
 *          - LATENCY [2:0] — number of wait states (0–2) required for the
 *            target SYSCLK frequency (RM0008 Table 11).
 *          - PRFTBE  [4]   — prefetch buffer enable; improves sustained
 *            instruction fetch throughput at high clock frequencies.
 */
#define FLASH_ACR (*(volatile UINT32 *)(FLASH_BASE + 0x00))

/* ---------------------------------------------------------------------------
 * Clock configuration API
 * --------------------------------------------------------------------------*/

/**
 * @brief  Switch the system clock source to the PLL and configure it to
 *         produce a 72 MHz SYSCLK from the 8 MHz HSE oscillator.
 *
 * @return void
 *
 * @note   Call this function once at startup, before initialising any
 *         peripheral that depends on SYSCLK, HCLK, or PCLK1/PCLK2.
 *
 *         Clock tree after this function returns:
 *           - HSE  =   8 MHz  (external crystal)
 *           - PLL  =  72 MHz  (HSE × 9, PLLMUL = 7 → ×9)
 *           - SYSCLK = 72 MHz (PLL selected as clock source)
 *           - HCLK   = 72 MHz (AHB prescaler = 1)
 *           - PCLK1  = 36 MHz (APB1 prescaler = 2, max 36 MHz)
 *           - PCLK2  = 72 MHz (APB2 prescaler = 1)
 *
 *         Flash wait states are set to 2 cycles and the prefetch buffer is
 *         enabled before raising SYSCLK, as required by RM0008 §3.4.
 *
 * @warning Blocking waits are used for HSE ready (HSERDY) and PLL locked
 *          (PLLRDY) flags. If HSE fails to start (e.g., no crystal fitted),
 *          this function will hang indefinitely.
 */
void RCC_SystemClock_72MHz(void);