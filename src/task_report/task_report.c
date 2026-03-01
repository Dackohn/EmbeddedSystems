#include "task_report.h"

#include "../app_shared/app_shared.h"
#include "../mcal_uart0_stdio/mcal_uart0_stdio.h"
#include "../mcal_timer1_1ms/mcal_timer1_1ms.h"

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

/* Minimal token parser (no sscanf) */
static const char* skip_spaces(const char* p) {
    while (*p == ' ' || *p == '\t') p++;
    return p;
}

static void read_token(const char* line, char* out, uint8_t out_sz) {
    if (out_sz == 0) return;
    out[0] = '\0';

    const char* p = skip_spaces(line);
    uint8_t i = 0;

    while (*p && *p != ' ' && *p != '\t' && i < (uint8_t)(out_sz - 1u)) {
        out[i++] = *p++;
    }
    out[i] = '\0';
}

/*
 * Poll console using STDIO only:
 * - mcal_stdio_try_readline uses fgetc(stdin) internally (non-blocking)
 * - parse command without scanf-family to reduce UNO flash/stack
 */
static void console_poll(void) {
    char line[48];
    if (!mcal_stdio_try_readline(line, sizeof(line))) return;

    char cmd[16];
    read_token(line, cmd, sizeof(cmd));
    if (cmd[0] == '\0') return;

    if (strcmp(cmd, "stats") == 0) {
        Stats_t snap = g_stats;
        print_stats(&snap, "manual stats");
    } else if (strcmp(cmd, "reset") == 0) {
        reset_stats();
        printf("OK: reset\n");
    } else {
        printf("Commands: stats | reset\n");
    }
}

void task_report_init(TaskReportCtx* ctx) {
    if (!ctx) return;
    ctx->started = 0u;
    ctx->last_report_ms = 0u;
}

/*
 * In RTOS: call this often (ex: every 20ms) to keep console responsive.
 * It will print the periodic report every 10 seconds based on mcal_millis().
 *
 * In bare-metal: it can still be called every 10s by your scheduler, and it
 * will still work correctly.
 */
void task_report_run(void* vctx) {
    TaskReportCtx* ctx = (TaskReportCtx*)vctx;

    console_poll();

    if (!ctx) return;

    uint32_t now = mcal_millis();

    if (!ctx->started) {
        ctx->started = 1u;
        ctx->last_report_ms = now;
        return;
    }

    /* periodic 10s report */
    if ((uint32_t)(now - ctx->last_report_ms) >= 10000u) {
        Stats_t snap = g_stats;
        print_stats(&snap, "10s report");
        reset_stats();

        /* avoid drift if loop timing jitters */
        ctx->last_report_ms = now;
    }
}