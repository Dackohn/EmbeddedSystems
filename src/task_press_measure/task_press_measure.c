#include "task_press_measure.h"
#include "../app_config.h"
#include "../mcal_timer1_1ms/mcal_timer1_1ms.h"
#include "../app_shared/app_shared.h"
#include "../dev_hw_stdio/dev_hw_stdio.h"
#include <stdio.h>

static inline uint8_t hw_read_raw(FILE* hw, uint8_t current_raw) {
    int ch = fgetc(hw);
    if (ch == EOF) return current_raw;
    if (ch == (int)HW_BTN_RAW_PRESSED)  return 1u;
    if (ch == (int)HW_BTN_RAW_RELEASED) return 0u;
    return current_raw;
}

void task_press_measure_init(TaskPressMeasureCtx* ctx, uint32_t now_ms) {
    FILE* hw = dev_hw_stdio_stream();

    ctx->press_start_ms = 0;
    ctx->pressed = 0;

    /* initialize raw/stable from first HW read */
    ctx->raw_level = hw_read_raw(hw, 0u);
    ctx->stable_level = ctx->raw_level;
    ctx->last_change_ms = now_ms;

    /* clear indicators at start (stdio -> HW) */
    fputc(HW_LED_GREEN_OFF, hw);
    fputc(HW_LED_RED_OFF, hw);
}

void task_press_measure_run(void* vctx) {
    TaskPressMeasureCtx* ctx = (TaskPressMeasureCtx*)vctx;
    uint32_t now = mcal_millis();
    FILE* hw = dev_hw_stdio_stream();

    /* 1) Read raw level changes via HW stdio stream */
    uint8_t new_raw = hw_read_raw(hw, ctx->raw_level);
    if (new_raw != ctx->raw_level) {
        ctx->raw_level = new_raw;
        ctx->last_change_ms = now;
    }

    /* 2) Debounce: accept change only if raw stayed stable for DEBOUNCE_MS */
    if (ctx->stable_level != ctx->raw_level) {
        if ((uint32_t)(now - ctx->last_change_ms) >= (uint32_t)DEBOUNCE_MS) {
            ctx->stable_level = ctx->raw_level;

            /* debounced edge event */
            if (ctx->stable_level) {
                /* Pressed (debounced) */
                ctx->press_start_ms = now;
                ctx->pressed = 1u;

                /* while holding: show no previous result */
                fputc(HW_LED_GREEN_OFF, hw);
                fputc(HW_LED_RED_OFF, hw);
            } else {
                /* Released (debounced) */
                if (ctx->pressed) {
                    ctx->pressed = 0u;

                    uint32_t dur = now - ctx->press_start_ms;
                    uint8_t is_long = (dur >= (uint32_t)SHORT_PRESS_THRESHOLD_MS) ? 1u : 0u;

                    g_press_event.last_press_ms = dur;
                    g_press_event.last_press_is_long = is_long;
                    g_press_event.last_press_valid = 1u;

                    /* indicate result only when NOT pressed */
                    if (is_long) {
                        fputc(HW_LED_GREEN_OFF, hw);
                        fputc(HW_LED_RED_ON, hw);
                    } else {
                        fputc(HW_LED_RED_OFF, hw);
                        fputc(HW_LED_GREEN_ON, hw);
                    }
                }
            }
        }
    }
}