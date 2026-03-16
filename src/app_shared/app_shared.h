#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    ALERT_STATE_NORMAL = 0,
    ALERT_STATE_PENDING_HIGH,
    ALERT_STATE_ACTIVE,
    ALERT_STATE_PENDING_LOW
} alert_state_t;

typedef struct
{
    uint16_t raw_adc;
    int16_t  temp_dC;
    uint32_t resistance_ohms;
    uint8_t  warnings;
    uint32_t timestamp_ms;
} sensor_sample_t;

typedef struct
{
    sensor_sample_t sample;
    alert_state_t state;
    uint8_t alert_active;
    uint32_t stable_ms;
    int16_t raw_temp_dC;
    int16_t saturated_temp_dC;
    int16_t median_temp_dC;
    int16_t weighted_temp_dC;
} conditioned_state_t;

typedef struct
{
    uint8_t raw_event;
    uint8_t median_event;
    uint8_t weighted_event;
    uint16_t weighted_permille;
    uint32_t total_raw_presses;
    uint32_t total_filtered_presses;
    uint32_t display_toggles;
    uint32_t last_press_ms;
    uint32_t last_press_duration_ms;
} button_state_t;

typedef struct
{
    uint8_t display_enabled;
} ui_state_t;

void app_shared_init(void);

void app_shared_set_sample(const sensor_sample_t* sample);
void app_shared_get_sample(sensor_sample_t* sample);

void app_shared_set_conditioned(const conditioned_state_t* state);
void app_shared_get_conditioned(conditioned_state_t* state);

void app_shared_set_button(const button_state_t* state);
void app_shared_get_button(button_state_t* state);

void app_shared_toggle_display(void);
void app_shared_get_ui(ui_state_t* ui);

#ifdef __cplusplus
}
#endif