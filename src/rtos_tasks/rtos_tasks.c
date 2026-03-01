#ifdef USE_FREERTOS

#include "rtos_tasks.h"

#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <stdio.h>

#include "app_config.h"
#include "app_shared/app_shared.h"

#include "task_press_measure/task_press_measure.h"
#include "task_stats_blink/task_stats_blink.h"
#include "task_report/task_report.h"

/* -----------------------------------------------------------------------
 * Shared state
 * ----------------------------------------------------------------------- */

static TaskPressMeasureCtx g_t1;
static TaskStatsBlinkCtx   g_t2;
static TaskReportCtx       g_t3;

/*
 * Binary semaphore: Task 1 gives it once a complete press is recorded.
 * Task 2 blocks on it, so it only wakes when there is actually work to do.
 */
static SemaphoreHandle_t g_sem_press;

/* -----------------------------------------------------------------------
 * Task 1 — button polling + press measurement
 * ----------------------------------------------------------------------- */
static void task_button(void* arg) {
    (void)arg;

    task_press_measure_init(&g_t1, 0);

    for (;;) {
        vTaskDelay(1);   /* yield one tick (~15 ms at 64 Hz) */

        uint8_t before = g_press_event.last_press_valid;

        task_press_measure_run(&g_t1);

        uint8_t after = g_press_event.last_press_valid;

        if (!before && after) {
            xSemaphoreGive(g_sem_press);
        }
    }
}

/* -----------------------------------------------------------------------
 * Task 2 — statistics + yellow blink engine
 * ----------------------------------------------------------------------- */
static void task_stats(void* arg) {
    (void)arg;

    task_stats_blink_init(&g_t2);

    for (;;) {
        xSemaphoreTake(g_sem_press, portMAX_DELAY);

        do {
            task_stats_blink_run(&g_t2);
            if (g_t2.active) {
                vTaskDelay(1);
            }
        } while (g_t2.active);
    }
}

/* -----------------------------------------------------------------------
 * Task 3 — responsive console poll + periodic report
 *
 * Runs often (20ms) so commands respond immediately,
 * while the 10s report is timed inside task_report_run().
 * ----------------------------------------------------------------------- */
static void task_report_rtos(void* arg) {
    (void)arg;

    task_report_init(&g_t3);

    TickType_t lastWake = xTaskGetTickCount();

    for (;;) {
        task_report_run(&g_t3);
        vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(20));
    }
}

/* -----------------------------------------------------------------------
 * Create tasks + sync objects. Call once from setup().
 * Stack sizes in words (2 bytes each on AVR).
 * Tuned smaller for Arduino UNO (2KB SRAM).
 * ----------------------------------------------------------------------- */
void rtos_tasks_create(void) {
    g_sem_press = xSemaphoreCreateBinary();

    xTaskCreate(task_button,      "btn",    120, NULL, 3, NULL);
    xTaskCreate(task_stats,       "stats",  150, NULL, 2, NULL);
    xTaskCreate(task_report_rtos, "report", 180, NULL, 1, NULL);
}

#endif /* USE_FREERTOS */