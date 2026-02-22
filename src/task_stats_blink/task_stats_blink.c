#include "task_stats_blink.h"
#include "../app_shared/app_shared.h"
#include "../mcal_timer1_1ms/mcal_timer1_1ms.h"
#include "../app_config.h"
#include "../dev_hw_stdio/dev_hw_stdio.h"
#include <stdio.h>

static inline void yellow_set(uint8_t on) {
    FILE* hw = dev_hw_stdio_stream();
    fputc(on ? HW_LED_YELLOW_ON : HW_LED_YELLOW_OFF, hw);
}

static void blink_start(TaskStatsBlinkCtx* ctx, uint8_t blinks, uint32_t now) {
    if (!blinks) return;

    ctx->active = 1u;

    /* visible immediately */
    ctx->state = 1u;
    yellow_set(1u);

    /* remaining toggles (we already turned ON once) */
    ctx->toggles_left = (uint16_t)(blinks * 2u - 1u);
    ctx->next_toggle_ms = now + (uint32_t)YELLOW_TOGGLE_INTERVAL_MS;
}

void task_stats_blink_init(TaskStatsBlinkCtx* ctx) {
    ctx->active = 0u;
    ctx->state = 0u;
    ctx->toggles_left = 0u;
    ctx->next_toggle_ms = 0u;
    yellow_set(0u);
}

void task_stats_blink_run(void* vctx) {
    TaskStatsBlinkCtx* ctx = (TaskStatsBlinkCtx*)vctx;
    uint32_t now = mcal_millis();

    if (g_press_event.last_press_valid) {
        uint32_t dur = g_press_event.last_press_ms;
        uint8_t is_long = g_press_event.last_press_is_long;

        g_stats.total++;
        g_stats.sum_ms += dur;

        if (is_long) {
            g_stats.long_count++;
            blink_start(ctx, 10u , now);
        } else {
            g_stats.short_count++;
            blink_start(ctx, 5u , now);
        }

        g_press_event.last_press_valid = 0u;
    }

    if (ctx->active) {
        if ((int32_t)(now - ctx->next_toggle_ms) >= 0) {
            ctx->state ^= 1u;
            yellow_set(ctx->state);

            if (ctx->toggles_left) ctx->toggles_left--;

            ctx->next_toggle_ms = now + (uint32_t)YELLOW_TOGGLE_INTERVAL_MS;

            if (ctx->toggles_left == 0u) {
                ctx->active = 0u;
                ctx->state = 0u;
                yellow_set(0u);
            }
        }
    }
}