#include "uart_stdio.h"

#include <avr/io.h>
#include <stdio.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

static int uart_putchar(char c, FILE* stream);
static int uart_getchar(FILE* stream);

static FILE g_uart_file = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

void uart_stdio_init(uint32_t baud)
{
    uint16_t ubrr = (uint16_t)((F_CPU / (16UL * baud)) - 1UL);

    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)(ubrr & 0xFF);

    UCSR0A = 0;
    UCSR0B = (1U << RXEN0) | (1U << TXEN0);
    UCSR0C = (1U << UCSZ01) | (1U << UCSZ00);

    stdout = &g_uart_file;
    stdin  = &g_uart_file;
    stderr = &g_uart_file;
}

FILE* uart_stdio_stream(void)
{
    return &g_uart_file;
}

static int uart_putchar(char c, FILE* stream)
{
    (void)stream;

    if (c == '\n') {
        while ((UCSR0A & (1U << UDRE0)) == 0U) {}
        UDR0 = '\r';
    }

    while ((UCSR0A & (1U << UDRE0)) == 0U) {}
    UDR0 = (uint8_t)c;
    return 0;
}

static int uart_getchar(FILE* stream)
{
    (void)stream;
    while ((UCSR0A & (1U << RXC0)) == 0U) {}
    return UDR0;
}