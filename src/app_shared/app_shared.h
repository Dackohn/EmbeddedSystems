/*
 * app_shared.h
 *
 * Shared global state between tasks.
 * Because the scheduler runs tasks sequentially (one at a time),
 * data races are reduced, but 'volatile' is used to prevent the compiler
 * from caching values across task boundaries.
 */

#pragma once
#include <stdint.h>

/* Press event produced by Task 1 and consumed by Task 2. */
typedef struct {
    volatile uint32_t last_press_ms;       /* measured press duration in ms */
    volatile uint8_t  last_press_valid;    /* 1 = new event available */
    volatile uint8_t  last_press_is_long;  /* 1 = long press, 0 = short press */
} PressEvent_t;

/* Statistics updated by Task 2 and reported/reset by Task 3. */
typedef struct {
    volatile uint32_t total;       /* total number of presses in the window */
    volatile uint32_t short_count; /* short press count */
    volatile uint32_t long_count;  /* long press count */
    volatile uint32_t sum_ms;      /* sum of durations for average computation */
} Stats_t;

volatile extern PressEvent_t g_press_event;
volatile extern Stats_t g_stats;