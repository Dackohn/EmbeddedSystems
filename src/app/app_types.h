#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    SENSOR_WARN_NONE   = 0,
    SENSOR_WARN_OPEN   = 1 << 0,
    SENSOR_WARN_SHORT  = 1 << 1,
    SENSOR_WARN_RANGE  = 1 << 2,
    SENSOR_WARN_NOISY  = 1 << 3
} sensor_warning_flags_t;

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
    float voltage_v;
    float resistance_ohms;
    float temperature_c;
    uint8_t warnings;
    uint32_t timestamp_ms;
} sensor_sample_t;

typedef struct
{
    sensor_sample_t sample;
    alert_state_t state;
    bool alert_active;
    uint32_t stable_ms;
} conditioned_state_t;

typedef struct
{
    bool display_enabled;
} ui_state_t;