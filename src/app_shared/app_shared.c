#include "app_shared.h"

#include <Arduino_FreeRTOS.h>
#include <semphr.h>

static sensor_sample_t g_sample;
static conditioned_state_t g_conditioned;
static button_state_t g_button;
static ui_state_t g_ui;

static SemaphoreHandle_t g_mtx_sample;
static SemaphoreHandle_t g_mtx_conditioned;
static SemaphoreHandle_t g_mtx_button;
static SemaphoreHandle_t g_mtx_ui;

void app_shared_init(void)
{
    g_mtx_sample = xSemaphoreCreateMutex();
    g_mtx_conditioned = xSemaphoreCreateMutex();
    g_mtx_button = xSemaphoreCreateMutex();
    g_mtx_ui = xSemaphoreCreateMutex();

    g_sample.raw_adc = 0U;
    g_sample.temp_dC = 0;
    g_sample.resistance_ohms = 0UL;
    g_sample.warnings = 0U;
    g_sample.timestamp_ms = 0UL;

    g_conditioned.sample = g_sample;
    g_conditioned.state = ALERT_STATE_NORMAL;
    g_conditioned.alert_active = 0U;
    g_conditioned.stable_ms = 0UL;
    g_conditioned.raw_temp_dC = 0;
    g_conditioned.saturated_temp_dC = 0;
    g_conditioned.median_temp_dC = 0;
    g_conditioned.weighted_temp_dC = 0;

    g_button.raw_event = 0U;
    g_button.median_event = 0U;
    g_button.weighted_event = 0U;
    g_button.weighted_permille = 0U;
    g_button.total_raw_presses = 0UL;
    g_button.total_filtered_presses = 0UL;
    g_button.display_toggles = 0UL;
    g_button.last_press_ms = 0UL;
    g_button.last_press_duration_ms = 0UL;

    g_ui.display_enabled = 1U;
}

void app_shared_set_sample(const sensor_sample_t* sample)
{
    xSemaphoreTake(g_mtx_sample, portMAX_DELAY);
    g_sample = *sample;
    xSemaphoreGive(g_mtx_sample);
}

void app_shared_get_sample(sensor_sample_t* sample)
{
    xSemaphoreTake(g_mtx_sample, portMAX_DELAY);
    *sample = g_sample;
    xSemaphoreGive(g_mtx_sample);
}

void app_shared_set_conditioned(const conditioned_state_t* state)
{
    xSemaphoreTake(g_mtx_conditioned, portMAX_DELAY);
    g_conditioned = *state;
    xSemaphoreGive(g_mtx_conditioned);
}

void app_shared_get_conditioned(conditioned_state_t* state)
{
    xSemaphoreTake(g_mtx_conditioned, portMAX_DELAY);
    *state = g_conditioned;
    xSemaphoreGive(g_mtx_conditioned);
}

void app_shared_set_button(const button_state_t* state)
{
    xSemaphoreTake(g_mtx_button, portMAX_DELAY);
    g_button = *state;
    xSemaphoreGive(g_mtx_button);
}

void app_shared_get_button(button_state_t* state)
{
    xSemaphoreTake(g_mtx_button, portMAX_DELAY);
    *state = g_button;
    xSemaphoreGive(g_mtx_button);
}

void app_shared_toggle_display(void)
{
    xSemaphoreTake(g_mtx_ui, portMAX_DELAY);
    g_ui.display_enabled = (g_ui.display_enabled == 0U) ? 1U : 0U;
    xSemaphoreGive(g_mtx_ui);
}

void app_shared_get_ui(ui_state_t* ui)
{
    xSemaphoreTake(g_mtx_ui, portMAX_DELAY);
    *ui = g_ui;
    xSemaphoreGive(g_mtx_ui);
}