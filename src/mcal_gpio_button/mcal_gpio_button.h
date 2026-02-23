#pragma once
#include <stdint.h>

/*
 * MCAL GPIO Button Driver
 *
 * Low-level (register-level) access to the pushbutton.
 * Responsibilities:
 *  - configure the pin as input
 *  - enable pull-up if BUTTON_ACTIVE_LOW is used
 *  - return the logical "pressed" state (1 pressed, 0 released)
 *
 * Higher-level behavior (debounce, edge events) is handled in ECAL / dev_hw_stdio.
 */

void mcal_button_init(void);

/* Returns 1 if logically pressed, otherwise 0. */
uint8_t mcal_button_read_pressed(void);