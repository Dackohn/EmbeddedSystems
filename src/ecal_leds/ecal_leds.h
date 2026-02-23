#pragma once
#include <stdint.h>

typedef struct {
    uint8_t  active;
    uint16_t toggles_left;
    uint32_t next_toggle_ms;
    uint8_t  state;
} YellowBlink_t;

void ecal_leds_init(void);

void ecal_led_green(uint8_t on);
void ecal_led_red(uint8_t on);
void ecal_led_yellow(uint8_t on);

void ecal_yellow_blink_start(YellowBlink_t* y, uint8_t blinks, uint32_t now_ms);
void ecal_yellow_blink_update(YellowBlink_t* y, uint32_t now_ms, uint32_t toggle_interval_ms);