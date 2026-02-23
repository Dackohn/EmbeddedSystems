#include "mcal_gpio_leds.h"
#include "../board_select.h"
#include <avr/io.h>

static inline void pin_output(volatile uint8_t* ddr, uint8_t bit) { *ddr |= (1u << bit); }

static inline void pin_write(volatile uint8_t* port, uint8_t bit, uint8_t v) {
    if (v) *port |= (1u << bit);
    else   *port &= (uint8_t)~(1u << bit);
}

void mcal_leds_init(void) {
    pin_output(&LEDG_DDR, LEDG_BIT);
    pin_output(&LEDR_DDR, LEDR_BIT);
    pin_output(&LEDY_DDR, LEDY_BIT);

    mcal_led_green(0);
    mcal_led_red(0);
    mcal_led_yellow(0);
}

void mcal_led_green(uint8_t on)  { pin_write(&LEDG_PORT, LEDG_BIT, on); }
void mcal_led_red(uint8_t on)    { pin_write(&LEDR_PORT, LEDR_BIT, on); }
void mcal_led_yellow(uint8_t on) { pin_write(&LEDY_PORT, LEDY_BIT, on); }