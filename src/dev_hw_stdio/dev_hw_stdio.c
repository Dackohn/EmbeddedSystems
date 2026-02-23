#include "dev_hw_stdio.h"
#include "../app_config.h"
#include "../mcal_timer1_1ms/mcal_timer1_1ms.h"
#include "../mcal_gpio_button/mcal_gpio_button.h"
#include "../mcal_gpio_leds/mcal_gpio_leds.h"
#include "../ecal_button/ecal_button.h"
#include <avr/io.h>

/*
 * This module exposes a FILE stream for hardware access.
 *
 * Write side (fputc):
 *   - receives LED control characters and calls MCAL LED driver.
 *
 * Read side (fgetc):
 *   - samples MCAL button driver (raw pressed level)
 *   - debounces using ECAL button component
 *   - returns only debounced PRESS/RELEASE events
 *   - returns EOF if no event is available (non-blocking)
 */

static Button_t g_btn;        /* ECAL debounce state */
static uint8_t  g_btn_inited; /* one-time init guard */

/* putc: LEDs controlled by printing characters */
static int hw_putchar(char c, FILE* stream) {
    (void)stream;

    switch (c) {
        case HW_LED_GREEN_ON:   mcal_led_green(1u); break;
        case HW_LED_GREEN_OFF:  mcal_led_green(0u); break;

        case HW_LED_RED_ON:     mcal_led_red(1u); break;
        case HW_LED_RED_OFF:    mcal_led_red(0u); break;

        case HW_LED_YELLOW_ON:  mcal_led_yellow(1u); break;
        case HW_LED_YELLOW_OFF: mcal_led_yellow(0u); break;

        default:
            /* Ignore anything not meant for hardware control. */
            break;
    }
    return 0;
}

/* getc: returns only DEBOUNCED edge events, otherwise EOF (non-blocking) */
static int hw_getchar(FILE* stream) {
    (void)stream;

    uint32_t now = mcal_millis();
    uint8_t raw_pressed = mcal_button_read_pressed(); /* raw logical level: 1 pressed */

    /* Initialize debouncer the first time we are ever asked for input */
    if (!g_btn_inited) {
        ecal_button_init(&g_btn, raw_pressed, now);
        g_btn_inited = 1u;
        return _FDEV_EOF;
    }

    /* Update debounce state and event flags */
    ecal_button_update(&g_btn, raw_pressed, now, DEBOUNCE_MS);

    if (ecal_button_take_pressed(&g_btn))  return (int)HW_BTN_PRESS_EVT;
    if (ecal_button_take_released(&g_btn)) return (int)HW_BTN_RELEASE_EVT;

    return _FDEV_EOF;
}

static FILE g_hw_stream;

void dev_hw_stdio_init(void) {
    /*
     * Initialize real drivers first:
     * - Button (MCAL)
     * - LEDs (MCAL)
     */
    mcal_button_init();
    mcal_leds_init();

    /* Reset debouncer state from current raw button level */
    uint32_t now = mcal_millis();
    uint8_t raw_pressed = mcal_button_read_pressed();
    ecal_button_init(&g_btn, raw_pressed, now);
    g_btn_inited = 1u;

    /* Bind FILE handlers (read + write) */
    fdev_setup_stream(&g_hw_stream, hw_putchar, hw_getchar, _FDEV_SETUP_RW);
}

FILE* dev_hw_stdio_stream(void) {
    return &g_hw_stream;
}