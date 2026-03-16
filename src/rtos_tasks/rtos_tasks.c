#include "rtos_tasks.h"

#include "../app_config.h"
#include "../app_shared/app_shared.h"
#include "../drivers/button_stdio/button_stdio.h"
#include "../drivers/thermistor_stdio/thermistor_stdio.h"
#include "../drivers/led_stdio/led_stdio.h"
#include "../drivers/lcd_stdio/lcd_stdio.h"
#include "../drivers/uart_stdio/uart_stdio.h"

#include <Arduino_FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <string.h>

#define APP_ALERT_LOW_dC    240
#define APP_ALERT_HIGH_dC   260
#define APP_ALERT_BLINK_MS  250U
#define APP_BUTTON_PULSE_HOLD_TICKS 4U

static int16_t clamp_temp(int16_t t_dC)
{
    if (t_dC < APP_TEMP_MIN_dC) {
        return (int16_t)APP_TEMP_MIN_dC;
    }
    if (t_dC > APP_TEMP_MAX_dC) {
        return (int16_t)APP_TEMP_MAX_dC;
    }
    return t_dC;
}

static int16_t median3(int16_t a, int16_t b, int16_t c)
{
    if (a > b) { int16_t t = a; a = b; b = t; }
    if (b > c) { int16_t t = b; b = c; c = t; }
    if (a > b) { int16_t t = a; a = b; b = t; }
    return b;
}

static FILE* g_btn = NULL;
static FILE* g_sensor = NULL;
static FILE* g_led = NULL;
static FILE* g_lcd = NULL;
static FILE* g_uart = NULL;

static alert_state_t g_state = ALERT_STATE_NORMAL;
static TickType_t g_state_since = 0;

static const char* state_str(alert_state_t s)
{
    switch (s) {
        case ALERT_STATE_NORMAL:       return "NORMAL";
        case ALERT_STATE_PENDING_HIGH: return "PEND_H";
        case ALERT_STATE_ACTIVE:       return "ALERT";
        case ALERT_STATE_PENDING_LOW:  return "PEND_L";
        default:                       return "UNK";
    }
}

static const char* warn_str(uint8_t w)
{
    if (w & APP_WARN_OPEN)  return "OPEN";
    if (w & APP_WARN_SHORT) return "SHORT";
    if (w & APP_WARN_RANGE) return "RANGE";
    if (w & APP_WARN_NOISY) return "NOISY";
    return "OK";
}

static void format_temp(char* out, int16_t t_dC)
{
    int16_t abs_t = t_dC;
    int16_t whole;
    int16_t frac;

    if (abs_t < 0) {
        abs_t = (int16_t)(-abs_t);
        whole = (int16_t)(abs_t / 10);
        frac = (int16_t)(abs_t % 10);
        snprintf(out, 17, "-%d.%dC", whole, frac);
    } else {
        whole = (int16_t)(abs_t / 10);
        frac = (int16_t)(abs_t % 10);
        snprintf(out, 17, "%d.%dC", whole, frac);
    }
}

static void format_age(char* out, uint32_t age_ms)
{
    if (age_ms < 1000UL) {
        snprintf(out, 17, "%lums", (unsigned long)age_ms);
    } else {
        unsigned long s = (unsigned long)(age_ms / 1000UL);
        snprintf(out, 17, "%lus", s);
    }
}

static uint8_t temp_is_critical(int16_t t_dC)
{
    return (uint8_t)((t_dC < APP_ALERT_LOW_dC) || (t_dC > APP_ALERT_HIGH_dC));
}

static const char* alert_dir_str(int16_t t_dC)
{
    if (t_dC < APP_ALERT_LOW_dC) {
        return "LOW";
    }
    if (t_dC > APP_ALERT_HIGH_dC) {
        return "HIGH";
    }
    return "OK";
}

static void task_button(void* arg)
{
    TickType_t last = xTaskGetTickCount();
    static uint32_t total_raw_presses = 0UL;
    static uint32_t total_filtered_presses = 0UL;
    static uint32_t display_toggles = 0UL;
    static uint32_t last_press_ms = 0UL;
    static uint32_t last_accept_ms = 0UL;
    static uint8_t stretched_pulse = 0U;
    static uint8_t pulse_ticks_left = 0U;
    (void)arg;

    for (;;) {
        int ch;
        uint32_t now_ms;
        uint8_t raw_event = 0U;
        uint8_t accepted_event = 0U;
        button_state_t bs;

        ch = fgetc(g_btn);
        now_ms = (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);

        if (ch == 'P') {
            raw_event = 1U;
            total_raw_presses++;
            last_press_ms = now_ms;

            if ((last_accept_ms == 0UL) ||
                ((now_ms - last_accept_ms) >= APP_BUTTON_DEBOUNCE_MS)) {
                accepted_event = 1U;
                total_filtered_presses++;
                display_toggles++;
                last_accept_ms = now_ms;

                stretched_pulse = 1U;
                pulse_ticks_left = APP_BUTTON_PULSE_HOLD_TICKS;
            }
        }

        if (stretched_pulse != 0U) {
            if (pulse_ticks_left > 0U) {
                pulse_ticks_left--;
            } else {
                stretched_pulse = 0U;
            }
        }

        bs.raw_event = raw_event;
        bs.median_event = stretched_pulse;
        bs.weighted_event = stretched_pulse;
        bs.weighted_permille = (stretched_pulse != 0U) ? 1000U : 0U;
        bs.total_raw_presses = total_raw_presses;
        bs.total_filtered_presses = total_filtered_presses;
        bs.display_toggles = display_toggles;
        bs.last_press_ms = last_press_ms;

        app_shared_set_button(&bs);

        vTaskDelayUntil(&last, pdMS_TO_TICKS(APP_TASK_BUTTON_MS));
    }
}

static void task_sensor(void* arg)
{
    TickType_t last = xTaskGetTickCount();
    (void)arg;

    for (;;) {
        sensor_sample_t s;
        unsigned int raw_u;
        int temp_dC;
        unsigned long res_ohms;
        unsigned int warn_u;

        raw_u = 0U;
        temp_dC = 0;
        res_ohms = 0UL;
        warn_u = 0U;

        fscanf(g_sensor, "%u %d %lu %u", &raw_u, &temp_dC, &res_ohms, &warn_u);

        s.raw_adc = (uint16_t)raw_u;
        s.temp_dC = (int16_t)temp_dC;
        s.resistance_ohms = (uint32_t)res_ohms;
        s.warnings = (uint8_t)warn_u;
        s.timestamp_ms = (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);

        app_shared_set_sample(&s);

        vTaskDelayUntil(&last, pdMS_TO_TICKS(APP_TASK_SENSOR_MS));
    }
}

static void task_condition(void* arg)
{
    TickType_t last = xTaskGetTickCount();
    static int16_t sat_hist[4] = {0, 0, 0, 0};
    static int16_t med_hist[4] = {0, 0, 0, 0};
    static uint8_t sat_count = 0U;
    static uint8_t med_count = 0U;
    static int16_t prev_sat = 0;
    (void)arg;

    for (;;) {
        sensor_sample_t s;
        conditioned_state_t c;
        uint8_t faulty;
        uint8_t critical_now;
        uint8_t safe_now;
        TickType_t now;
        int16_t raw_t;
        int16_t sat_t;
        int16_t med_t;
        int16_t filt_t;

        app_shared_get_sample(&s);
        now = xTaskGetTickCount();

        raw_t = s.temp_dC;
        sat_t = clamp_temp(raw_t);

        if ((sat_count != 0U) &&
            (((sat_t - prev_sat) > APP_MAX_STEP_dC) || ((prev_sat - sat_t) > APP_MAX_STEP_dC))) {
            s.warnings |= APP_WARN_NOISY;
        }
        prev_sat = sat_t;

        sat_hist[3] = sat_hist[2];
        sat_hist[2] = sat_hist[1];
        sat_hist[1] = sat_hist[0];
        sat_hist[0] = sat_t;
        if (sat_count < 4U) {
            sat_count++;
        }

        if (sat_count >= 3U) {
            med_t = median3(sat_hist[0], sat_hist[1], sat_hist[2]);
        } else {
            med_t = sat_t;
        }

        med_hist[3] = med_hist[2];
        med_hist[2] = med_hist[1];
        med_hist[1] = med_hist[0];
        med_hist[0] = med_t;
        if (med_count < 4U) {
            med_count++;
        }

        if (med_count >= 4U) {
            filt_t = (int16_t)((med_hist[0] * 4 + med_hist[1] * 3 + med_hist[2] * 2 + med_hist[3]) / 10);
        } else if (med_count == 3U) {
            filt_t = (int16_t)((med_hist[0] * 3 + med_hist[1] * 2 + med_hist[2]) / 6);
        } else if (med_count == 2U) {
            filt_t = (int16_t)((med_hist[0] * 2 + med_hist[1]) / 3);
        } else {
            filt_t = med_t;
        }

        if ((raw_t < APP_TEMP_MIN_dC) || (raw_t > APP_TEMP_MAX_dC)) {
            s.warnings |= APP_WARN_RANGE;
        }

        s.temp_dC = filt_t;

        faulty = (s.warnings != 0U) ? 1U : 0U;
        critical_now = (uint8_t)((faulty == 0U) && temp_is_critical(s.temp_dC));
        safe_now = (uint8_t)((faulty == 0U) && !temp_is_critical(s.temp_dC));

        switch (g_state) {
            case ALERT_STATE_NORMAL:
                if (critical_now != 0U) {
                    g_state = ALERT_STATE_PENDING_HIGH;
                    g_state_since = now;
                }
                break;

            case ALERT_STATE_PENDING_HIGH:
                if (critical_now == 0U) {
                    g_state = ALERT_STATE_NORMAL;
                    g_state_since = now;
                } else if ((now - g_state_since) >= pdMS_TO_TICKS(APP_ALERT_CONFIRM_MS)) {
                    g_state = ALERT_STATE_ACTIVE;
                    g_state_since = now;
                }
                break;

            case ALERT_STATE_ACTIVE:
                if (safe_now != 0U) {
                    g_state = ALERT_STATE_PENDING_LOW;
                    g_state_since = now;
                }
                break;

            case ALERT_STATE_PENDING_LOW:
                if (critical_now != 0U) {
                    g_state = ALERT_STATE_ACTIVE;
                    g_state_since = now;
                } else if ((now - g_state_since) >= pdMS_TO_TICKS(APP_ALERT_CONFIRM_MS)) {
                    g_state = ALERT_STATE_NORMAL;
                    g_state_since = now;
                }
                break;

            default:
                g_state = ALERT_STATE_NORMAL;
                g_state_since = now;
                break;
        }

        c.sample = s;
        c.state = g_state;
        c.alert_active = (g_state == ALERT_STATE_ACTIVE) ? 1U : 0U;
        c.stable_ms = (uint32_t)((now - g_state_since) * portTICK_PERIOD_MS);
        c.raw_temp_dC = raw_t;
        c.saturated_temp_dC = sat_t;
        c.median_temp_dC = med_t;
        c.weighted_temp_dC = filt_t;

        app_shared_set_conditioned(&c);

        vTaskDelayUntil(&last, pdMS_TO_TICKS(APP_TASK_CONDITION_MS));
    }
}

static void task_alert(void* arg)
{
    TickType_t last = xTaskGetTickCount();
    static uint8_t led_state = 0U;
    (void)arg;

    for (;;) {
        conditioned_state_t c;
        app_shared_get_conditioned(&c);

        if (c.alert_active != 0U) {
            led_state = (led_state == 0U) ? 1U : 0U;
            fputc((led_state != 0U) ? '1' : '0', g_led);
            vTaskDelayUntil(&last, pdMS_TO_TICKS(APP_ALERT_BLINK_MS));
        } else {
            led_state = 0U;
            fputc('0', g_led);
            vTaskDelayUntil(&last, pdMS_TO_TICKS(APP_TASK_ALERT_MS));
        }
    }
}

static void task_ui(void* arg)
{
    TickType_t last = xTaskGetTickCount();
    static uint8_t lcd_page = 0U;
    static uint32_t last_page_toggle_count = 0UL;
    static uint8_t lcd_initialized = 0U;
    (void)arg;

    for (;;) {
        conditioned_state_t c;
        button_state_t b;
        ui_state_t ui;
        char temp_buf[17];
        char raw_buf[17];
        char sat_buf[17];
        char med_buf[17];
        char age_buf[17];
        char line1[17];
        char line2[17];
        uint32_t now_ms;

        app_shared_get_conditioned(&c);
        app_shared_get_button(&b);
        app_shared_get_ui(&ui);

        now_ms = (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);

        format_temp(temp_buf, c.sample.temp_dC);
        format_temp(raw_buf, c.raw_temp_dC);
        format_temp(sat_buf, c.saturated_temp_dC);
        format_temp(med_buf, c.median_temp_dC);

        if (b.last_press_ms != 0UL) {
            format_age(age_buf, now_ms - b.last_press_ms);
        } else {
            snprintf(age_buf, sizeof(age_buf), "never");
        }

        fprintf(g_uart,
                "raw_adc=%u raw_t=%s sat=%s med=%s filt=%s R=%lu warn=%s state=%s alert=%u dir=%s stable=%lu\n",
                (unsigned int)c.sample.raw_adc,
                raw_buf,
                sat_buf,
                med_buf,
                temp_buf,
                (unsigned long)c.sample.resistance_ohms,
                warn_str(c.sample.warnings),
                state_str(c.state),
                (unsigned int)c.alert_active,
                alert_dir_str(c.sample.temp_dC),
                (unsigned long)c.stable_ms);

        fprintf(g_uart,
                "button raw=%u med=%u w=%u wpermille=%u total=%lu filt_total=%lu page_toggles=%lu last_ms=%lu\n",
                (unsigned int)b.raw_event,
                (unsigned int)b.median_event,
                (unsigned int)b.weighted_event,
                (unsigned int)b.weighted_permille,
                (unsigned long)b.total_raw_presses,
                (unsigned long)b.total_filtered_presses,
                (unsigned long)b.display_toggles,
                (unsigned long)b.last_press_ms);

        fprintf(g_uart,
                "%u,%d,%d,%d,%d,%u,%u,%u,%u\n",
                (unsigned int)c.sample.raw_adc,
                (int)c.raw_temp_dC,
                (int)c.saturated_temp_dC,
                (int)c.median_temp_dC,
                (int)c.weighted_temp_dC,
                (unsigned int)b.raw_event,
                (unsigned int)b.median_event,
                (unsigned int)b.weighted_event,
                (unsigned int)b.weighted_permille);

        if (b.display_toggles != last_page_toggle_count) {
            lcd_page = (lcd_page == 0U) ? 1U : 0U;
            last_page_toggle_count = b.display_toggles;
        }

        if (ui.display_enabled != 0U) {
            if (lcd_page == 0U) {
                snprintf(line1, sizeof(line1), "%-16s", temp_buf);

                if (c.sample.warnings != 0U) {
                    char tmp[17];
                    snprintf(tmp, sizeof(tmp), "WARN:%s", warn_str(c.sample.warnings));
                    snprintf(line2, sizeof(line2), "%-16s", tmp);
                } else if (c.alert_active != 0U) {
                    char tmp[17];
                    snprintf(tmp, sizeof(tmp), "AL:%s Btn:%lu",
                             alert_dir_str(c.sample.temp_dC),
                             (unsigned long)b.total_filtered_presses);
                    snprintf(line2, sizeof(line2), "%-16s", tmp);
                } else {
                    char tmp[17];
                    snprintf(tmp, sizeof(tmp), "Btn:%lu Last:%s",
                             (unsigned long)b.total_filtered_presses,
                             age_buf);
                    snprintf(line2, sizeof(line2), "%-16s", tmp);
                }
            } else {
                char tmp1[17];
                char tmp2[17];
                snprintf(tmp1, sizeof(tmp1), "Raw:%lu F:%lu",
                         (unsigned long)b.total_raw_presses,
                         (unsigned long)b.total_filtered_presses);
                snprintf(tmp2, sizeof(tmp2), "Ago:%s",
                         age_buf);
                snprintf(line1, sizeof(line1), "%-16s", tmp1);
                snprintf(line2, sizeof(line2), "%-16s", tmp2);
            }

            if (lcd_initialized == 0U) {
                fputc('\f', g_lcd);
                lcd_initialized = 1U;
            }

            /* Home/update both lines without clearing every cycle */
            fputc('\v', g_lcd);
            fprintf(g_lcd, "%-16s\n%-16s", line1, line2);
        }

        vTaskDelayUntil(&last, pdMS_TO_TICKS(APP_TASK_UI_MS));
    }
}

void rtos_tasks_create(void)
{
    g_btn = button_stdio_stream();
    g_sensor = thermistor_stdio_stream();
    g_led = led_stdio_stream();
    g_lcd = lcd_stdio_stream();
    g_uart = uart_stdio_stream();

    g_state = ALERT_STATE_NORMAL;
    g_state_since = xTaskGetTickCount();

    xTaskCreate(task_button,    "btn",    APP_STACK_BUTTON,    NULL, 2, NULL);
    xTaskCreate(task_sensor,    "sens",   APP_STACK_SENSOR,    NULL, 2, NULL);
    xTaskCreate(task_condition, "cond",   APP_STACK_CONDITION, NULL, 2, NULL);
    xTaskCreate(task_alert,     "alert",  APP_STACK_ALERT,     NULL, 1, NULL);
    xTaskCreate(task_ui,        "ui",     APP_STACK_UI,        NULL, 1, NULL);
}