#pragma once
#include <stdint.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#define TICK_MS                     1u
#define DEBOUNCE_MS                 5u
#define SHORT_PRESS_THRESHOLD_MS    500u

#define TASK1_PERIOD_MS             10u
#define TASK1_OFFSET_MS             0u

#define TASK2_PERIOD_MS             10u
#define TASK2_OFFSET_MS             1u

#define TASK3_PERIOD_MS             10000u
#define TASK3_OFFSET_MS             50u

#define YELLOW_TOGGLE_INTERVAL_MS   50u

#define UART_BAUD                   9600UL

#define BUTTON_ACTIVE_LOW           0u