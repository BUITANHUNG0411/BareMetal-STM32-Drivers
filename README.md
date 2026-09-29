# BareMetal-STM32-Drivers

Register-direct peripheral driver library for **STM32F103xB (Blue Pill)**, written in C without the HAL abstraction layer.  
Built as a learning project to demonstrate bare-metal embedded development — from RCC clock gating to interrupt-driven debounce.

---

## Hardware Target

| Item | Value |
|---|---|
| MCU | STM32F103C8T6 |
| Core | ARM Cortex-M3 @ 8 MHz (HSI) |
| Flash / RAM | 64 KB / 20 KB |
| Button | PA0 — external pull-up, active-low, EXTI0 |
| LED | PA5 — push-pull output |

---

## Project Structure

```
BareMetal-STM32-Drivers/
│
├── Drivers/                  # Chip-level, register-direct, board-agnostic
│   ├── Inc/
│   │   ├── rcc.h             # RCC register map + UINT32/UINT8 base types
│   │   ├── gpio.h            # GPIO port/pin configuration & atomic write API
│   │   └── timer_reg.h       # TIM2–TIM5 base-timer register map & control API
│   └── Src/
│       ├── rcc.c
│       ├── gpio.c
│       └── timer_reg.c
│
├── BSP/                      # Board-specific peripheral wrappers
│   ├── Inc/
│   │   ├── button_exti.h     # Push-button EXTI falling-edge driver
│   │   └── led_control.h     # GPIO LED state & toggle driver
│   └── Src/
│       ├── button_exti.c
│       └── led_control.c
│
└── Middleware/               # Application logic built on top of BSP + Drivers
    ├── Inc/
    │   └── button_manager.h  # Debounce FSM coordinating EXTI, Timer, and LED
    └── Src/
        └── button_manager.c
```

---

## Architecture

```
┌──────────────────────────────────┐
│        Application  (main.c)     │
├──────────────────────────────────┤
│  Middleware    button_manager    │  ← debounce FSM, interrupt handlers
├─────────────────┬────────────────┤
│  BSP            │  BSP           │  ← board-specific wiring
│  button_exti    │  led_control   │
├─────────────────┴────────────────┤
│  Drivers    gpio │ timer │ rcc   │  ← register-direct, chip-level
├──────────────────────────────────┤
│  CMSIS  (core_cm3, stm32f103xb)  │
└──────────────────────────────────┘
```

### Debounce Flow

```
[Button Press]
     │  EXTI0_IRQHandler fires
     ▼
ButtonManager_EXTI_Handler()
     │  Clear EXTI_PR flag
     │  State == IDLE? → start TIM2, go to DEBOUNCING
     │  State == DEBOUNCING? → discard (bounce suppression)
     ▼
[TIM2 overflows after ~20 ms]
     │  TIM2_IRQHandler fires
     ▼
ButtonManager_TIM_Handler()
     │  Stop & clear TIM2
     │  Toggle LED
     └→ Return to IDLE
```

---

## Build

**Prerequisites:** `arm-none-eabi-gcc`, `CMake ≥ 3.22`, `Ninja`, `STM32CubeProgrammer`

```bash
# Configure
cmake -B build/Debug -G Ninja -DCMAKE_BUILD_TYPE=Debug

# Build
cmake --build build/Debug

# Flash (via STM32CubeProgrammer CLI)
STM32_Programmer_CLI -c port=SWD -w build/Debug/led_blinking_register.hex -rst
```

---

## Key Design Decisions

| Decision | Rationale |
|---|---|
| No HAL | Forces understanding of register-level hardware behaviour |
| BSRR for GPIO output | Atomic set/reset — safe to call from ISR without disabling interrupts |
| Timer debounce (not `delay`) | Non-blocking; CPU remains responsive during the debounce window |
| Layered architecture | Each layer is independently testable and reusable across boards |
| `rcc.h` as root header | Single source of truth for `UINT32`/`UINT8` — avoids typedef duplication |

---

## Implemented Drivers

| Module | Layer | Peripheral | Status |
|---|---|---|---|
| `rcc.h` | Driver | RCC register map | ✅ Done |
| `gpio.c` | Driver | GPIO (CRL/CRH/BSRR/IDR) | ✅ Done |
| `timer_reg.c` | Driver | TIM2–TIM5 base timer | ✅ Done |
| `button_exti.c` | BSP | EXTI falling-edge | ✅ Done |
| `led_control.c` | BSP | GPIO output LED | ✅ Done |
| `button_manager.c` | Middleware | Debounce FSM | ✅ Done |

---

## Author

**BUI TAN HUNG** — Embedded Systems Engineering Student
