/*
 * board_esp32.h
 *
 * Hardware mapping for ESP32 (Arduino framework).
 * Defines GPIO numbers for:
 * - button input
 * - green, yellow, red LEDs output
 *
 * NOTE:
 * - Use pinMode()/digitalRead()/digitalWrite() instead of AVR DDR/PORT regs.
 * - Wiring suggestion:
 *   Button between BTN_GPIO and GND, using INPUT_PULLUP.
 */

#pragma once
#include <Arduino.h>

/* Button input GPIO. */
#define BTN_GPIO   27

/* Green LED output GPIO. */
#define LEDG_GPIO  25

/* Red LED output GPIO. */
#define LEDR_GPIO  26

/* Yellow LED output GPIO. */
#define LEDY_GPIO  33