#include "ecal_leds.h"
#include "../board_select.h"
#include "../app_config.h"

static inline void pin_output(volatile uint8_t* ddr, uint8_t bit) { *ddr |= (1 << bit); }
static inline void pin_write(volatile uint8_t* port, uint8_t bit, uint8_t v) {
    if (v) *port |= (1 << bit);
    else   *port &= (uint8_t)~(1 << bit);
}

void ecal_leds_init(void) {
    pin_output(&LEDG_DDR, LEDG_BIT);
    pin_output(&LEDR_DDR, LEDR_BIT);
    pin_output(&LEDY_DDR, LEDY_BIT);

    ecal_led_green(0);
    ecal_led_red(0);
    ecal_led_yellow(0);
}



void ecal_yellow_blink_start(YellowBlink_t* y, uint8_t blinks, uint32_t now_ms) {
    if (!blinks) return;

    y->active = 1;

    // We turn ON immediately, so we already performed 1 toggle
    y->state = 1;
    ecal_led_yellow(1);

    // Total toggles for N blinks is 2*N (ON+OFF per blink).
    // Since we already did the first ON, remaining toggles are:
    y->toggles_left = (uint16_t)(blinks * 2u - 1u);

    // Next toggle (to OFF) happens after the interval
    y->next_toggle_ms = now_ms + YELLOW_TOGGLE_INTERVAL_MS;
}

void ecal_yellow_blink_update(YellowBlink_t* y, uint32_t now_ms, uint32_t toggle_interval_ms) {
    if (!y->active) return;

    if ((int32_t)(now_ms - y->next_toggle_ms) >= 0) {
        y->state ^= 1u;
        ecal_led_yellow(y->state);

        if (y->toggles_left) y->toggles_left--;
        y->next_toggle_ms = now_ms + toggle_interval_ms;

        if (y->toggles_left == 0) {
            y->active = 0;
            y->state = 0;
            ecal_led_yellow(0);
        }
    }
}