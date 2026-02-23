#include "mcal_uart0_stdio.h"
#include <avr/io.h>
#include <stdio.h>

/*
 * UART putchar: blocking transmit (safe for printf()).
 */
static int uart_putchar(char c, FILE* stream) {
    (void)stream;
    while (!(UCSR0A & (1 << UDRE0))) { }
    UDR0 = (uint8_t)c;
    return 0;
}

/*
 * UART getchar: NON-BLOCKING.
 * Returns:
 *  - received byte (0..255) if available
 *  - EOF if no byte is available
 *
 * This allows polling stdin via fgetc(stdin) without stalling the CPU.
 */
static int uart_getchar(char c, FILE* stream) {
    (void)c;
    (void)stream;

    if (!(UCSR0A & (1 << RXC0))) {
        return _FDEV_EOF; /* no data */
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

    /* Bind UART to STDIO */
    fdev_setup_stream(&g_uart_stream, uart_putchar, (int (*)(FILE*))uart_getchar, _FDEV_SETUP_RW);
    stdout = &g_uart_stream;
    stdin  = &g_uart_stream;
}

/*
 * Non-blocking STDIO line reader.
 *
 * Implementation rules:
 * - Only uses STDIO input (fgetc(stdin)) to comply with the requirement.
 * - Because uart_getchar returns EOF when no data exists, this function returns quickly.
 * - A line is considered complete at '\n' or '\r'.
 */
uint8_t mcal_stdio_try_readline(char* out, uint8_t out_sz) {
    static char buf[48];
    static uint8_t idx = 0;

    int ch = fgetc(stdin);
    if (ch == EOF) return 0u;

    char c = (char)ch;

    if (c == '\n' || c == '\r') {
        if (idx >= sizeof(buf)) idx = (uint8_t)(sizeof(buf) - 1u);
        buf[idx] = '\0';

        uint8_t n = (idx < (uint8_t)(out_sz - 1u)) ? idx : (uint8_t)(out_sz - 1u);
        for (uint8_t i = 0; i < n; i++) out[i] = buf[i];
        out[n] = '\0';

        idx = 0;
        return 1u;
    }

    if (idx < (uint8_t)(sizeof(buf) - 1u)) {
        buf[idx++] = c;
    }
    return 0u;
}