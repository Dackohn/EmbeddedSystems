#include "task_report.h"
#include "../app_shared/app_shared.h"
#include "../mcal_uart0_stdio/mcal_uart0_stdio.h"
#include <stdio.h>
#include <string.h>

static void reset_stats(void) {
    g_stats.total = 0;
    g_stats.short_count = 0;
    g_stats.long_count = 0;
    g_stats.sum_ms = 0;
}

static void print_stats(const Stats_t* s, const char* tag) {
    uint32_t avg = 0;
    if (s->total) avg = s->sum_ms / s->total;

    printf("\n[%s]\n", tag);
    printf("total=%lu short=%lu long=%lu avg=%lu ms\n",
           (unsigned long)s->total,
           (unsigned long)s->short_count,
           (unsigned long)s->long_count,
           (unsigned long)avg);
}

static void console_poll(void) {
    char line[48];
    if (!mcal_stdio_try_readline(line, sizeof(line))) return;

    if (strcmp(line, "stats") == 0) {
        Stats_t snap = g_stats;
        print_stats(&snap, "manual stats");
    } else if (strcmp(line, "reset") == 0) {
        reset_stats();
        printf("OK: reset\n");
    } else if (line[0]) {
        printf("Commands: stats | reset\n");
    }
}

void task_report_init(TaskReportCtx* ctx) { (void)ctx; }

void task_report_run(void* vctx) {
    (void)vctx;

    Stats_t snap = g_stats;
    print_stats(&snap, "10s report");
    reset_stats();

    console_poll();
}