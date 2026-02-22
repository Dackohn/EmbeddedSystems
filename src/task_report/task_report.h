#pragma once
#include <stdint.h>

typedef struct { uint8_t dummy; } TaskReportCtx;

void task_report_init(TaskReportCtx* ctx);
void task_report_run(void* vctx);