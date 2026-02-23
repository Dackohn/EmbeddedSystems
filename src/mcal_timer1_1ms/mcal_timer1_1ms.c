/*
 * mcal_timer1_1ms.c
 *
 * Timer1-based 1ms timebase:
 * - ISR increments a millisecond counter
 * - ISR sets a tick flag that the main loop consumes
 *
 * The atomic sections prevent tearing when reading shared ISR data.
 */

#include "mcal_timer1_1ms.h"
#include <avr/io.h>
#include <avr/interrupt.h>

static volatile uint32_t g_ms = 0;  /* monotonic ms counter */
static volatile uint8_t  g_tick = 0;/* "tick occurred" flag */

void mcal_timer1_init_1ms(void) {
    cli();

    TCCR1A = 0;
    TCCR1B = 0;

    /* CTC mode: counter resets when reaching OCR1A. */
    TCCR1B |= (1 << WGM12);

    /* Prescaler 64. */
    TCCR1B |= (1 << CS11) | (1 << CS10);

    /* 16MHz / 64 = 250kHz -> 250 cycles per 1ms => OCR1A = 249. */
    OCR1A = 249;

    /* Enable compare match A interrupt. */
    TIMSK1 |= (1 << OCIE1A);

    sei();
}

/* ISR executes every 1ms. */
ISR(TIMER1_COMPA_vect) {
    g_ms++;
    g_tick = 1;
}

/* Atomic read of millisecond counter. */
uint32_t mcal_millis(void) {
    uint32_t v;
    uint8_t sreg = SREG;
    cli();
    v = g_ms;
    SREG = sreg;
    return v;
}

/* Atomic consume/reset of tick flag. */
uint8_t mcal_consume_tick(void) {
    uint8_t v;
    uint8_t sreg = SREG;
    cli();
    v = g_tick;
    g_tick = 0;
    SREG = sreg;
    return v;
}