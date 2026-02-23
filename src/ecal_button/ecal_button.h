#pragma once
#include <stdint.h>

typedef struct {
    uint8_t  stable;
    uint8_t  last_raw;
    uint32_t last_change_ms;

    uint8_t  pressed_event;
    uint8_t  released_event;
} Button_t;

void ecal_button_init(Button_t* b, uint8_t initial, uint32_t now_ms);
void ecal_button_update(Button_t* b, uint8_t raw, uint32_t now_ms, uint32_t debounce_ms);

uint8_t ecal_button_take_pressed(Button_t* b);
uint8_t ecal_button_take_released(Button_t* b);
uint8_t ecal_button_is_pressed(const Button_t* b);