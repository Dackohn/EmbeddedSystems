#include "button_stdio.h"

#include "../../app_config.h"
#include <Arduino_FreeRTOS.h>
#include <task.h>
#include <avr/io.h>
#include <stdio.h>

static int button_getchar(FILE* stream);

static FILE g_button_file = FDEV_SETUP_STREAM(NULL, button_getchar, _FDEV_SETUP_READ);

static uint8_t g_stable_level = 1U;
static uint8_t g_last_raw_level = 1U;
static TickType_t g_change_tick = 0;

void button_stdio_init(void)
{
    /* input pull-up */
    BTN_DDR &= (uint8_t)~(1U << BTN_BIT);
    BTN_PORT |= (1U << BTN_BIT);

    g_stable_level = 1U;
    g_last_raw_level = 1U;
    g_change_tick = xTaskGetTickCount();
}

FILE* button_stdio_stream(void)
{
    return &g_button_file;
}

static int button_getchar(FILE* stream)
{
    uint8_t raw;
    TickType_t now;
    TickType_t debounce_ticks;

    (void)stream;

    raw = ((BTN_PINR & (1U << BTN_BIT)) != 0U) ? 1U : 0U;
    now = xTaskGetTickCount();
    debounce_ticks = pdMS_TO_TICKS(APP_BUTTON_DEBOUNCE_MS);

    if (raw != g_last_raw_level) {
        g_last_raw_level = raw;
        g_change_tick = now;
    }

    if ((now - g_change_tick) >= debounce_ticks) {
        if (raw != g_stable_level) {
            g_stable_level = raw;

            /* active low: confirmed press */
            if (g_stable_level == 0U) {
                return 'P';
            }
        }
    }

    return _FDEV_EOF;
}