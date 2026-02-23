#pragma once
#include <stdint.h>

/*
 * MCAL GPIO LEDs Driver
 *
 * Low-level control of Green/Red/Yellow LEDs.
 * Responsibilities:
 *  - configure LED pins as outputs
 *  - set each LED ON/OFF
 *
 * Application logic must NOT touch registers directly.
 */

void mcal_leds_init(void);

void mcal_led_green(uint8_t on);
void mcal_led_red(uint8_t on);
void mcal_led_yellow(uint8_t on);