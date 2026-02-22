#include "srv_scheduler.h"

void srv_scheduler_init(Scheduler_t* s, TaskDesc_t* tasks, uint8_t count, uint32_t now_ms) {
    s->tasks = tasks;
    s->count = count;
    for (uint8_t i = 0; i < count; i++) {
        tasks[i].next_run_ms = now_ms + tasks[i].offset_ms;
    }
}

void srv_scheduler_run_one(Scheduler_t* s, uint32_t now_ms) {
    for (uint8_t i = 0; i < s->count; i++) {
        TaskDesc_t* t = &s->tasks[i];
        if ((int32_t)(now_ms - t->next_run_ms) >= 0) {
            t->fn(t->ctx);
            t->next_run_ms += t->period_ms;
            return;
        }
    }
}