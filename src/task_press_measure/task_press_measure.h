#pragma once
#include <stdint.h>

/* Task 1 context:
 * stores debounce state and timing of the current press.
 */
typedef struct {
    uint32_t press_start_ms;   /* timestamp when press started */
    uint8_t  pressed;          /* debounced state: currently pressed */

    uint8_t  raw_level;        /* last observed raw value */
    uint8_t  stable_level;     /* debounced stable value */
    uint32_t last_change_ms;   /* last time raw input changed */
} TaskPressMeasureCtx;

/* Initialize Task 1 context/state. */
void task_press_measure_init(TaskPressMeasureCtx* ctx, uint32_t now_ms);

/* Task 1 periodic function. */
void task_press_measure_run(void* vctx);