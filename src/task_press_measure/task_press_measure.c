#include "task_press_measure.h"
#include "../app_config.h"
#include "../mcal_timer1_1ms/mcal_timer1_1ms.h"
#include "../dev_hw_stdio/dev_hw_stdio.h"
#include "../app_shared/app_shared.h"
#include <stdint.h>
#include <stdio.h>

/*
 * Task 1 - Button press duration measurement
 *
 * Hardware accessed only via STDIO stream:
 *   - fgetc() -> debounced press/release events
 *   - fputc() -> LED control commands
 */

void task_press_measure_init(TaskPressMeasureCtx* ctx, uint32_t now_ms) {
    (void)now_ms;
    ctx->press_start_ms = 0;
    ctx->pressed = 0;

    /* Clear LEDs at startup */
    fputc(HW_LED_GREEN_OFF, dev_hw_stdio_stream());
    fputc(HW_LED_RED_OFF,   dev_hw_stdio_stream());
}

static uint8_t poll_button_event(uint8_t* released) {
    int ch = fgetc(dev_hw_stdio_stream());
    if (ch == EOF) return 0;

    if (ch == HW_BTN_PRESS_EVT) {
        *released = 0;
        return 1;
    }

    if (ch == HW_BTN_RELEASE_EVT) {
        *released = 1;
        return 1;
    }

    return 0;
}

void task_press_measure_run(void* vctx) {
    TaskPressMeasureCtx* ctx = (TaskPressMeasureCtx*)vctx;
    uint32_t now = mcal_millis();

    uint8_t released;

    if (!poll_button_event(&released))
        return;

    /* Press detected → start timing */
    if (!released) {
        ctx->press_start_ms = now;
        ctx->pressed = 1;

        /* Clear indicators while button is held */
        fputc(HW_LED_GREEN_OFF, dev_hw_stdio_stream());
        fputc(HW_LED_RED_OFF,   dev_hw_stdio_stream());
        return;
    }

    /* Release detected → compute duration */
    if (released && ctx->pressed) {
        ctx->pressed = 0;

        uint32_t dur = now - ctx->press_start_ms;
        if (dur < 5) return;

        uint8_t is_long = (dur >= SHORT_PRESS_THRESHOLD_MS);

        g_press_event.last_press_ms = dur;
        g_press_event.last_press_is_long = is_long;
        g_press_event.last_press_valid = 1;

        /* LED classification feedback via STDIO */
        if (is_long) {
            fputc(HW_LED_GREEN_OFF, dev_hw_stdio_stream());
            fputc(HW_LED_RED_ON,    dev_hw_stdio_stream());
        } else {
            fputc(HW_LED_RED_OFF,   dev_hw_stdio_stream());
            fputc(HW_LED_GREEN_ON,  dev_hw_stdio_stream());
        }
    }
}