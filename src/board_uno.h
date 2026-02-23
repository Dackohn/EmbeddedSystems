/*
 * board_uno.h
 *
 * Hardware mapping for Arduino UNO (ATmega328P).
 * Defines AVR register + bit for:
 * - button input
 * - green, yellow, red LEDs output
 *
 * IMPORTANT: Wiring must match these mappings.
 */

#pragma once
#include <avr/io.h>

/* Button input. */
#define BTN_DDR   DDRD
#define BTN_PORT  PORTD
#define BTN_PINR  PIND
#define BTN_BIT   PD2

/* Green LED output. */
#define LEDG_DDR  DDRB
#define LEDG_PORT PORTB
#define LEDG_BIT  PB5

/* Red LED output. */
#define LEDR_DDR  DDRB
#define LEDR_PORT PORTB
#define LEDR_BIT  PB4

/* Yellow LED output. */
#define LEDY_DDR  DDRB
#define LEDY_PORT PORTB
#define LEDY_BIT  PB3