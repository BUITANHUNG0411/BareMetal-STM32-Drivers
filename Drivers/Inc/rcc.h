#pragma once

/**
 * @file rcc.h
 * @author BUI_TAN_HUNG
 * @brief RCC register map and base-type definitions for STM32F1xx.
 * @version 1.0.0
 * @date 2026-09-29
 *
 * @copyright Copyright (c) 2026
 */

/* Common integer types used across all driver layers. */
typedef unsigned int  UINT32;
typedef unsigned char UINT8;

/**
 * @brief RCC register block layout (RM0008 §7.3).
 */
typedef struct {
    volatile UINT32 CR;        /**< Clock control register               (offset 0x00) */
    volatile UINT32 CFGR;      /**< Clock configuration register         (offset 0x04) */
    volatile UINT32 CIR;       /**< Clock interrupt register             (offset 0x08) */
    volatile UINT32 APB2RSTR;  /**< APB2 peripheral reset register       (offset 0x0C) */
    volatile UINT32 APB1RSTR;  /**< APB1 peripheral reset register       (offset 0x10) */
    volatile UINT32 AHBENR;    /**< AHB peripheral clock enable register (offset 0x14) */
    volatile UINT32 APB2ENR;   /**< APB2 peripheral clock enable register(offset 0x18) */
    volatile UINT32 APB1ENR;   /**< APB1 peripheral clock enable register(offset 0x1C) */
    volatile UINT32 BDCR;      /**< Backup domain control register       (offset 0x20) */
    volatile UINT32 CSR;       /**< Control/status register              (offset 0x24) */
} RCC_Typedef;

#define RCC_BASE 0x40021000UL
#define RCC      ((RCC_Typedef *)RCC_BASE)