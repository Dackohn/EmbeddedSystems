#pragma once
#include <stdint.h>

typedef struct {
    uint32_t press_start_ms;
    uint8_t  pressed;

    uint8_t  raw_level;
    uint8_t  stable_level;
    uint32_t last_change_ms;
} TaskPressMeasureCtx;

void task_press_measure_init(TaskPressMeasureCtx* ctx, uint32_t now_ms);
void task_press_measure_run(void* vctx);    