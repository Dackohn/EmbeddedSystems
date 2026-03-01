#include "mcal_timer1_1ms.h"

#ifndef USE_FREERTOS

#include <avr/io.h>
#include <avr/interrupt.h>

static volatile uint32_t g_ms = 0;
static volatile uint8_t  g_tick = 0;

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

#else /* USE_FREERTOS */

/*
 * FreeRTOS owns Timer1 for its scheduler tick — we must not touch it.
 *
 * mcal_millis() uses Arduino's millis() which is driven by Timer0,
 * completely independent of FreeRTOS. This gives real wall-clock ms
 * regardless of what tick rate the FreeRTOS library was compiled with.
 *
 * This is the only correct approach: do NOT compute millis from
 * xTaskGetTickCount() * portTICK_PERIOD_MS because the feilipu library
 * compiles its kernel with its own FreeRTOSConfig.h (64 Hz tick), while
 * our source files may see a different configTICK_RATE_HZ — causing
 * portTICK_PERIOD_MS to disagree and making mcal_millis() wrong.
 */
#include <Arduino.h>

void mcal_timer1_init_1ms(void) { /* noop: FreeRTOS owns Timer1 */ }
uint32_t mcal_millis(void)       { return (uint32_t)millis(); }
uint8_t  mcal_consume_tick(void) { return 0; /* unused in RTOS build */ }

#endif