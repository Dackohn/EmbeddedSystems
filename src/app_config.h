/*
 * app_config.h
 *
 * Central configuration for timing and thresholds:
 * - CPU clock (F_CPU) for AVR timing math
 * - scheduler tick resolution
 * - debounce window and press classification threshold
 * - task periods and offsets (cooperative schedule)
 * - yellow LED blink timing
 * - UART baud rate
 * - button electrical polarity definition
 */

#pragma once
#include <stdint.h>

/* AVR CPU frequency (used by UART baud calculations and timer setup). */
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

/* Scheduler tick resolution (system timebase step). */
#define TICK_MS                     1u

/* Debounce window for mechanical switch bounce filtering (ms). */
#define DEBOUNCE_MS                 5u

/* Short/Long classification threshold (ms). */
#define SHORT_PRESS_THRESHOLD_MS    500u

/* press detection + duration measurement (ms). */
#define TASK1_PERIOD_MS             10u
#define TASK1_OFFSET_MS             0u

/* statistics update + yellow blink engine (ms). */
#define TASK2_PERIOD_MS             10u
#define TASK2_OFFSET_MS             1u

/* periodic report polling interval (ms) - task uses internal 10s timer for actual report. */
#define TASK3_PERIOD_MS             20u
#define TASK3_OFFSET_MS             50u

/* Yellow LED blink toggle interval (ms). */
#define YELLOW_TOGGLE_INTERVAL_MS   50u

/* UART speed for STDIO communication (baud). */
#define UART_BAUD                   9600UL

/* Button electrical convention:
 * 0 -> active-high (pressed reads 1)
 * 1 -> active-low  (pressed reads 0)
 */
#define BUTTON_ACTIVE_LOW           1u