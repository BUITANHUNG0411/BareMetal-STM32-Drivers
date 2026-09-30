/**
 * @file nvic.c
 * @author BUI_TAN_HUNG
 * @brief Register-direct NVIC driver implementation for ARM Cortex-M3.
 *
 *        Implements NVIC_Enable() by computing the correct ISER register index
 *        and bit position from the supplied IRQ number, then asserting the enable
 *        bit using a write-1-to-set operation — no read-modify-write required.
 *
 * @version 1.1.0
 * @date 2026-09-29
 *
 * @copyright Copyright (c) 2026 BUI_TAN_HUNG
 */

#include "nvic.h"

void NVIC_Enable(IRQ_Number IRQn)
{
    /* The 240 possible IRQ lines are partitioned across eight ISER registers,
     * each covering 32 consecutive IRQ numbers. Integer division by 32 maps
     * IRQn to the index of the register that owns its enable bit:
     *
     *   IRQn  0-31  → ISER[0]   IRQn 32-63 → ISER[1]   ... */
    UINT32 iser_index = (UINT32)IRQn / 32U;

    /* The bit position within the selected ISER register is the remainder
     * after dividing by 32. Bit 0 corresponds to the lowest IRQ covered by
     * that register (e.g., bit 0 of ISER[1] controls IRQ 32). */
    UINT32 bit_position = (UINT32)IRQn % 32U;

    /* ISER bits use write-1-to-set semantics: writing 1 enables the IRQ and
     * writing 0 has no effect. A simple OR-assignment is therefore atomic with
     * respect to other enable bits — no masking of existing bits is needed. */
    NVIC_ISER(iser_index) |= (1U << bit_position);
}