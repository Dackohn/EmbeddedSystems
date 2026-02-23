#pragma once

#include <stdint.h>

/* Task 3 context (currently unused but kept for symmetry and future extensions). */
typedef struct {
    uint8_t reserved;
} TaskReportCtx;

void task_report_init(TaskReportCtx* ctx);
void task_report_run(void* vctx);