#pragma once
#include <stdint.h>

/* Task 2 context:
 * maintains the yellow LED blink state machine.
 */
typedef struct {
    uint8_t  active;          /* blink sequence running */
    uint8_t  state;           /* current LED state (0/1) */
    uint16_t toggles_left;    /* remaining toggles until done */
    uint32_t next_toggle_ms;  /* next scheduled toggle time */
} TaskStatsBlinkCtx;

/* Initialize Task 2 context/state. */
void task_stats_blink_init(TaskStatsBlinkCtx* ctx);

/* Task 2 periodic function. */
void task_stats_blink_run(void* vctx);