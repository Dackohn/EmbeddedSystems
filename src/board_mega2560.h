/*
 * board_mega2560.h
 *
 * Wiring (as in your picture):
 *  - Green LED  -> Arduino D13
 *  - Red LED    -> Arduino D12
 *  - Yellow LED -> Arduino D11
 *  - Button     -> Arduino D4 (to GND, active-low, use pull-up)
 */

#pragma once
#include <avr/io.h>

/* Button input (Arduino D4 = PG5). */
#define BTN_DDR   DDRG
#define BTN_PORT  PORTG
#define BTN_PINR  PING
#define BTN_BIT   PG5

/* Green LED output (Arduino D13 = PB7). */
#define LEDG_DDR  DDRB
#define LEDG_PORT PORTB
#define LEDG_BIT  PB7

/* Red LED output (Arduino D12 = PB6). */
#define LEDR_DDR  DDRB
#define LEDR_PORT PORTB
#define LEDR_BIT  PB6

/* Yellow LED output (Arduino D11 = PB5). */
#define LEDY_DDR  DDRB
#define LEDY_PORT PORTB
#define LEDY_BIT  PB5