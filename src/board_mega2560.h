/*
 * board_mega2560.h
 *
 * Hardware mapping for Arduino Mega2560 (ATmega2560).
 * Defines AVR register + bit for:
 * - button input
 * - green, yellow, red LEDs output
 *
 * IMPORTANT: Wiring must match these mappings.
 */

#pragma once
#include <avr/io.h>

/* Button input (PE4). */
#define BTN_DDR   DDRE
#define BTN_PORT  PORTE
#define BTN_PINR  PINE
#define BTN_BIT   PE4

/* Green LED output (PB7). */
#define LEDG_DDR  DDRB
#define LEDG_PORT PORTB
#define LEDG_BIT  PB7

/* Red LED output (PB6). */
#define LEDR_DDR  DDRB
#define LEDR_PORT PORTB
#define LEDR_BIT  PB6

/* Yellow LED output (PB5). */
#define LEDY_DDR  DDRB
#define LEDY_PORT PORTB
#define LEDY_BIT  PB5   