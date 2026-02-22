#include "mcal_uart0_stdio.h"
#include "app_config.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>

#ifndef UBRR0H
  #error "This MCU does not have UART0 registers named UBRR0H/UDR0"
#endif

#define RX_BUF_SZ 128u
static volatile uint8_t rx_buf[RX_BUF_SZ];
static volatile uint8_t rx_w = 0;
static volatile uint8_t rx_r = 0;

static void rb_push(uint8_t b) {
    uint8_t next = (uint8_t)(rx_w + 1u);
    if (next >= RX_BUF_SZ) next = 0;
    if (next == rx_r) return; // overflow drop
    rx_buf[rx_w] = b;
    rx_w = next;
}

static uint8_t rb_pop(uint8_t* out) {
    if (rx_r == rx_w) return 0;
    *out = rx_buf[rx_r];
    uint8_t next = (uint8_t)(rx_r + 1u);
    if (next >= RX_BUF_SZ) next = 0;
    rx_r = next;
    return 1;
}

#if defined(USART0_RX_vect)
ISR(USART0_RX_vect) {
    uint8_t b = UDR0;
    rb_push(b);
}
#elif defined(USART_RX_vect)
ISR(USART_RX_vect) {
    uint8_t b = UDR0;
    rb_push(b);
}
#endif

static int uart_putchar(char c, FILE* stream) {
    (void)stream;
    if (c == '\n') uart_putchar('\r', stream);
    while (!(UCSR0A & (1 << UDRE0))) {}
    UDR0 = (uint8_t)c;
    return 0;
}

static int uart_getchar(FILE* stream) {
    (void)stream;
    uint8_t b;
    if (!rb_pop(&b)) return _FDEV_EOF;   // non-blocking
    return (int)b;
}

static FILE uart_stream;

void mcal_uart0_stdio_init(uint32_t baud) {
    uint16_t ubrr = (uint16_t)((F_CPU / (16UL * baud)) - 1UL);

    cli();

    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)(ubrr & 0xFF);

    UCSR0A = 0;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);  // RX, TX, RX interrupt
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);                // 8N1

    fdev_setup_stream(&uart_stream, uart_putchar, uart_getchar, _FDEV_SETUP_RW);
    stdout = &uart_stream;
    stdin  = &uart_stream;

    sei();
}

/* Non-blocking line read using getchar() (stdio) */
uint8_t mcal_stdio_try_readline(char* out, size_t out_sz) {
    static char buf[96];
    static uint8_t len = 0;

    int ch;
    while ((ch = getchar()) != EOF) {
        char c = (char)ch;
        if (c == '\r') continue;

        if (c == '\n') {
            buf[len] = '\0';
            if (out_sz) {
                strncpy(out, buf, out_sz - 1u);
                out[out_sz - 1u] = '\0';
            }
            len = 0;
            return 1;
        }

        if (len + 1u < sizeof(buf)) buf[len++] = c;
    }
    return 0;
}