#ifdef USE_FREERTOS

#include <Arduino.h>
#include <Arduino_FreeRTOS.h>

#include <avr/interrupt.h>
#include <util/delay.h>

/*
 * Distinct blink patterns on LED13 so you can instantly identify the reason:
 *  - Stack overflow: FAST blink (100ms on/off)
 *  - Malloc failed:  SLOW blink (400ms on/off)
 *
 * IMPORTANT:
 * - Do NOT printf inside these hooks (it can re-crash).
 * - Disable interrupts to prevent further corruption.
 */

static void panic_blink(uint16_t on_ms, uint16_t off_ms) {
    cli();
    pinMode(13, OUTPUT);

    for (;;) {
        digitalWrite(12, HIGH);
        _delay_ms(on_ms);
        digitalWrite(13, LOW);
        _delay_ms(off_ms);
    }
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    (void)xTask;
    (void)pcTaskName;
    panic_blink(100, 100);   /* FAST = stack overflow */
}

void vApplicationMallocFailedHook(void) {
    panic_blink(400, 400);   /* SLOW = heap/malloc failure */
}

#endif /* USE_FREERTOS */