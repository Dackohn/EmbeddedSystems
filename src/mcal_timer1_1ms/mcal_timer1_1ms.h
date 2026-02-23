#pragma once
#include <stdint.h>

/* Initialize Timer1 to generate a 1ms interrupt timebase. */
void mcal_timer1_init_1ms(void);

/* Return the current millisecond counter value. */
uint32_t mcal_millis(void);

/* Consume the "tick happened" flag (1 if tick occurred since last call). */
uint8_t mcal_consume_tick(void);