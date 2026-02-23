/*
 * srv_scheduler.h
 *
 * Cooperative scheduler service:
 * - tasks stored in an array (period, offset, next_run time, function, context)
 * - run_one() executes at most one eligible task per call
 */

#pragma once
#include <stdint.h>

typedef void (*TaskFn_t)(void* ctx);

typedef struct {
    uint32_t period_ms;    /* recurrence period */
    uint32_t offset_ms;    /* initial delay before first run */
    uint32_t next_run_ms;  /* absolute time when task becomes eligible */
    TaskFn_t  fn;          /* task entry function */
    void*     ctx;         /* pointer to task state/context */
} TaskDesc_t;

typedef struct {
    TaskDesc_t* tasks;
    uint8_t     count;
} Scheduler_t;

/* Initialize scheduler and compute each task's first next_run_ms. */
void srv_scheduler_init(Scheduler_t* s, TaskDesc_t* tasks, uint8_t count, uint32_t now_ms);

/* Run only the first eligible task (one task per tick requirement). */
void srv_scheduler_run_one(Scheduler_t* s, uint32_t now_ms);