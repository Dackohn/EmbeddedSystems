#include "mcal_timer1_1ms.h"
#include <avr/io.h>
#include <avr/interrupt.h>

static volatile uint32_t g_ms = 0;
static volatile uint8_t g_tick = 0;

void mcal_timer1_init_1ms(void) {
    cli();

    TCCR1A = 0;
    TCCR1B = 0;

    // CTC mode: TOP = OCR1A
    TCCR1B |= (1 << WGM12);

    // Prescaler 64
    TCCR1B |= (1 << CS11) | (1 << CS10);

    // 16MHz / 64 = 250kHz -> 250 counts per 1ms -> OCR1A = 249
    OCR1A = 249;

    // Enable compare match A interrupt
    TIMSK1 |= (1 << OCIE1A);

    sei();
}

ISR(TIMER1_COMPA_vect) {
    g_ms++;
    g_tick = 1;
}

uint32_t mcal_millis(void) {
    uint32_t v;
    uint8_t sreg = SREG;
    cli();
    v = g_ms;
    SREG = sreg;
    return v;
}

uint8_t mcal_consume_tick(void) {
    uint8_t v;
    uint8_t sreg = SREG;
    cli();
    v = g_tick;
    g_tick = 0;
    SREG = sreg;
    return v;
}