#pragma once
#include <stdint.h>

typedef void (*TaskFn)(void*);

typedef struct {
    uint32_t period_ms;
    uint32_t offset_ms;
    uint32_t next_run_ms;
    TaskFn fn;
    void* ctx;
} TaskDesc_t;

typedef struct {
    TaskDesc_t* tasks;
    uint8_t count;
} Scheduler_t;

void srv_scheduler_init(Scheduler_t* s, TaskDesc_t* tasks, uint8_t count, uint32_t now_ms);

/* Run at most ONE ready task per tick */
void srv_scheduler_run_one(Scheduler_t* s, uint32_t now_ms);