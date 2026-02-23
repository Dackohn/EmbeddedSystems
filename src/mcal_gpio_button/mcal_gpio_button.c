#include "mcal_gpio_button.h"
#include "../board_select.h"
#include "../app_config.h"
#include <avr/io.h>

static inline uint8_t pin_read(volatile uint8_t* pinr, uint8_t bit) {
    return (*pinr & (1u << bit)) ? 1u : 0u;
}

void mcal_button_init(void) {
    /* Input direction */
    BTN_DDR &= (uint8_t)~(1u << BTN_BIT);

#if BUTTON_ACTIVE_LOW
    /* Enable internal pull-up (button to GND). */
    BTN_PORT |= (1u << BTN_BIT);
#else
    /* No pull-up forced for active-high wiring. */
    BTN_PORT &= (uint8_t)~(1u << BTN_BIT);
#endif
}

uint8_t mcal_button_read_pressed(void) {
    uint8_t level = pin_read(&BTN_PINR, BTN_BIT);

#if BUTTON_ACTIVE_LOW
    /* Electrical 0 means pressed. */
    return (level == 0u) ? 1u : 0u;
#else
    /* Electrical 1 means pressed. */
    return (level != 0u) ? 1u : 0u;
#endif
}