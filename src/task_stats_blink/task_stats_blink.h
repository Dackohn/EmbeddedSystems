#pragma once
#include <stdint.h>

typedef struct {
    uint8_t  active;
    uint8_t  state;
    uint16_t toggles_left;
    uint32_t next_toggle_ms;
} TaskStatsBlinkCtx;

void task_stats_blink_init(TaskStatsBlinkCtx* ctx);
void task_stats_blink_run(void* vctx);