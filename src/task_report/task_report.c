#include "task_report.h"

#include "../app_shared/app_shared.h"
#include "../mcal_uart0_stdio/mcal_uart0_stdio.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef USE_FREERTOS
#include <Arduino_FreeRTOS.h>
#endif

static void reset_stats(void) {
    g_stats.total = 0;
    g_stats.short_count = 0;
    g_stats.long_count = 0;
    g_stats.sum_ms = 0;
}

/* ---------- tiny printing helpers (avoid printf-heavy formatting) ---------- */

static void put_str(const char* s) {
    fputs(s, stdout);
}

static void put_u32(uint32_t v) {
    char buf[11];
    ultoa((unsigned long)v, buf, 10);
    put_str(buf);
}

static void print_stats_compact(const Stats_t* s, const char* tag) {
    uint32_t avg = 0;
    if (s->total) avg = s->sum_ms / s->total;

    /* Force start at column 0 */
    put_str("\r\n");

    put_str("[");
    put_str(tag);
    put_str("]\r\n");

    put_str("t=");
    put_u32(s->total);
    put_str(" sh=");
    put_u32(s->short_count);
    put_str(" lo=");
    put_u32(s->long_count);
    put_str(" avg=");
    put_u32(avg);
    put_str("ms\r\n");
}

/* ---------- command parsing (kept minimal) ---------- */

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

static void console_poll(void) {
    char line[48];
    if (!mcal_stdio_try_readline(line, sizeof(line))) return;

    char cmd[16];
    read_token(line, cmd, sizeof(cmd));
    if (cmd[0] == '\0') return;

    if (strcmp(cmd, "stats") == 0) {
        Stats_t snap = g_stats;
        print_stats_compact(&snap, "manual");
    } else if (strcmp(cmd, "reset") == 0) {
        reset_stats();
        put_str("OK: reset\n");
    } else {
        put_str("Commands: stats | reset\n");
    }
}

/* ---------- time source ---------- */

static uint32_t now_ms(void) {
#ifdef USE_FREERTOS
    /* Use RTOS tick time (stable), avoids Timer1 conflicts */
    TickType_t t = xTaskGetTickCount();
    return (uint32_t)((uint32_t)t * (uint32_t)portTICK_PERIOD_MS);
#else
    /* Bare-metal path (your timer-based millis) */
    return mcal_millis();
#endif
}

/* ---------- public API ---------- */

void task_report_init(TaskReportCtx* ctx) {
    if (!ctx) return;
    ctx->started = 0u;
    ctx->last_report_ms = 0u;
}

void task_report_run(void* vctx) {
    TaskReportCtx* ctx = (TaskReportCtx*)vctx;

    /* keep console responsive */
    console_poll();

    if (!ctx) return;

    uint32_t now = now_ms();

    if (!ctx->started) {
        ctx->started = 1u;
        ctx->last_report_ms = now;
        return;
    }

    if ((uint32_t)(now - ctx->last_report_ms) >= 10000u) {
        Stats_t snap = g_stats;
        print_stats_compact(&snap, "10s");
        reset_stats();
        ctx->last_report_ms = now;
    }
}