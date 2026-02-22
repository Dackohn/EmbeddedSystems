#pragma once
#include <stdint.h>
#include <stddef.h>

void mcal_uart0_stdio_init(uint32_t baud);

/* non-blocking console helper */
uint8_t mcal_stdio_try_readline(char* out, size_t out_sz);