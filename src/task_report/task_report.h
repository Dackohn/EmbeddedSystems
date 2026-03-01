#pragma once

#include <stdint.h>

/* Task 3 context used for periodic timing (works in both bare-metal and RTOS). */
typedef struct {
    uint32_t last_report_ms;
    uint8_t  started;
} TaskReportCtx;

void task_report_init(TaskReportCtx* ctx);
void task_report_run(void* vctx);