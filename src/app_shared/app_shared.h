#pragma once
#include <stdint.h>

typedef struct {
    volatile uint32_t last_press_ms;
    volatile uint8_t  last_press_valid;
    volatile uint8_t  last_press_is_long;
} PressEvent_t;

typedef struct {
    volatile uint32_t total;
    volatile uint32_t short_count;
    volatile uint32_t long_count;
    volatile uint32_t sum_ms;
} Stats_t;

extern PressEvent_t g_press_event;
extern Stats_t g_stats;