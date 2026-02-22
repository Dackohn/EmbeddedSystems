#pragma once
#include <avr/io.h>

#define BTN_DDR   DDRE
#define BTN_PORT  PORTE
#define BTN_PINR  PINE
#define BTN_BIT   PE4   // D2 on Mega2560

#define LEDG_DDR  DDRB
#define LEDG_PORT PORTB
#define LEDG_BIT  PB7   // D13

#define LEDR_DDR  DDRB
#define LEDR_PORT PORTB
#define LEDR_BIT  PB6   // D12

#define LEDY_DDR  DDRB
#define LEDY_PORT PORTB
#define LEDY_BIT  PB5   // D11