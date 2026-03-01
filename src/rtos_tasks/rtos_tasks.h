#pragma once

#ifdef USE_FREERTOS

/* Create all three FreeRTOS tasks and the synchronisation objects.
 * Call once from setup() before the scheduler starts. */
void rtos_tasks_create(void);

#endif