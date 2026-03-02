#include "mcal_uart0_stdio.h"
#include <avr/io.h>
#include <stdio.h>

/* ---------------------------------------------------- */
/* UART low level                                       */
/* ---------------------------------------------------- */

static int uart_putchar(char c, FILE* stream) {
    (void)stream;
    while (!(UCSR0A & (1 << UDRE0))) { }
    UDR0 = (uint8_t)c;
    return 0;
}

static int uart_getchar(FILE* stream) {
    (void)stream;

    if (!(UCSR0A & (1 << RXC0))) {
        return _FDEV_EOF;
    }
    return (int)UDR0;
}

static FILE g_uart_stream;

void mcal_uart0_stdio_init(uint32_t baud) {

    uint16_t ubrr = (uint16_t)((F_CPU / (16UL * baud)) - 1UL);

    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)(ubrr & 0xFFu);

    UCSR0B = (1 << TXEN0) | (1 << RXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); /* 8N1 */

    fdev_setup_stream(&g_uart_stream,
                      uart_putchar,
                      uart_getchar,
                      _FDEV_SETUP_RW);

    stdout = &g_uart_stream;
    stdin  = &g_uart_stream;
}

/* ---------------------------------------------------- */
/* Non-blocking line reader (NO STATIC BUFFER)         */
/* ---------------------------------------------------- */

uint8_t mcal_stdio_try_readline(char* out, uint8_t out_sz) {

    /* Persistent index per stream */
    static uint8_t idx = 0;

    int ch = fgetc(stdin);
    if (ch == EOF) {
        return 0u;
    }

    char c = (char)ch;

    /* Line complete */
    if (c == '\n' || c == '\r') {

        if (idx >= out_sz)
            idx = (uint8_t)(out_sz - 1u);

        out[idx] = '\0';
        idx = 0;
        return 1u;
    }

    /* Normal character */
    if (idx < (uint8_t)(out_sz - 1u)) {
        out[idx++] = c;
    }

    return 0u;
}