#pragma once
#include <stdint.h>

void mcal_timer1_init_1ms(void);
uint32_t mcal_millis(void);

uint8_t mcal_consume_tick(void);