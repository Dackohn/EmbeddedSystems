#include "app_config.h"
#include "board_select.h"
#include "dev_hw_stdio/dev_hw_stdio.h"

#include "mcal_timer1_1ms/mcal_timer1_1ms.h"
#include "mcal_uart0_stdio/mcal_uart0_stdio.h"

#include "srv_scheduler/srv_scheduler.h"

#include "task_press_measure/task_press_measure.h"
#include "task_stats_blink/task_stats_blink.h"
#include "task_report/task_report.h"

#include <avr/interrupt.h>
#include <stdio.h>

static TaskPressMeasureCtx g_t1;
static TaskStatsBlinkCtx   g_t2;
static TaskReportCtx       g_t3;

static void hw_init(void) {
    cli();

    /* Serial monitor via STDIO */
    mcal_uart0_stdio_init(UART_BAUD);

    /* 1ms timebase */
    mcal_timer1_init_1ms();

    /*
     * Hardware access via STDIO:
     * - fputc() -> LED control
     * - fgetc() -> debounced button edge events
     *
     * Internally uses MCAL button/led drivers + ECAL debouncing.
     */
    dev_hw_stdio_init();

    sei();
}

int main(void) {
    hw_init();

    printf("READY: Button press monitor\n");
    printf("Commands: stats | reset\n");

    uint32_t now = mcal_millis();

    task_press_measure_init(&g_t1, now);
    task_stats_blink_init(&g_t2);
    task_report_init(&g_t3);

    TaskDesc_t tasks[] = {
        { TASK1_PERIOD_MS, TASK1_OFFSET_MS, 0, task_press_measure_run, &g_t1 },
        { TASK2_PERIOD_MS, TASK2_OFFSET_MS, 0, task_stats_blink_run,   &g_t2 },
        { TASK3_PERIOD_MS, TASK3_OFFSET_MS, 0, task_report_run,        &g_t3 },
    };

    Scheduler_t sch;
    srv_scheduler_init(&sch, tasks, (uint8_t)(sizeof(tasks)/sizeof(tasks[0])), now);

    while (1) {
        if (mcal_consume_tick()) {
            uint32_t t = mcal_millis();
            srv_scheduler_run_one(&sch, t);   /* ONE task per tick */
        }
    }
}