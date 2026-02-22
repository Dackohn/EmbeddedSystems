#pragma once
#include <avr/io.h>

#define BTN_DDR   DDRD
#define BTN_PORT  PORTD
#define BTN_PINR  PIND
#define BTN_BIT   PD2

#define LEDG_DDR  DDRB
#define LEDG_PORT PORTB
#define LEDG_BIT  PB5   // D13

#define LEDR_DDR  DDRB
#define LEDR_PORT PORTB
#define LEDR_BIT  PB4   // D12

#define LEDY_DDR  DDRB
#define LEDY_PORT PORTB
#define LEDY_BIT  PB3   // D11