#include "ecal_button.h"

void ecal_button_init(Button_t* b, uint8_t initial, uint32_t now_ms) {
    b->stable = initial;
    b->last_raw = initial;
    b->last_change_ms = now_ms;
    b->pressed_event = 0;
    b->released_event = 0;
}

void ecal_button_update(Button_t* b, uint8_t raw, uint32_t now_ms, uint32_t debounce_ms) {
    b->pressed_event = 0;
    b->released_event = 0;

    if (raw != b->last_raw) {
        b->last_raw = raw;
        b->last_change_ms = now_ms;
        return;
    }

    if ((now_ms - b->last_change_ms) >= debounce_ms) {
        if (b->stable != raw) {
            uint8_t prev = b->stable;
            b->stable = raw;

            if (prev == 0 && raw == 1) b->pressed_event = 1;
            if (prev == 1 && raw == 0) b->released_event = 1;
        }
    }
}

uint8_t ecal_button_take_pressed(Button_t* b) {
    uint8_t v = b->pressed_event;
    b->pressed_event = 0;
    return v;
}

uint8_t ecal_button_take_released(Button_t* b) {
    uint8_t v = b->released_event;
    b->released_event = 0;
    return v;
}

uint8_t ecal_button_is_pressed(const Button_t* b) {
    return b->stable;
}