#pragma once
#include <stdint.h>

/*
 * UART0 STDIO Driver
 *
 * stdout/stderr -> UART0 (printf)
 * stdin         -> UART0 (scanf / fgetc)
 *
 * IMPORTANT:
 * getchar is implemented as NON-BLOCKING (returns EOF if no RX byte),
 * so application tasks can poll input without stalling the scheduler.
 */

void mcal_uart0_stdio_init(uint32_t baud);

/* STDIO-based non-blocking line reader. Returns 1 if a full line is available. */
uint8_t mcal_stdio_try_readline(char* out, uint8_t out_sz);