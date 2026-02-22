#include "dev_hw_stdio.h"
#include "../board_select.h"
#include "../app_config.h"
#include <avr/io.h>

/* helpers */
static inline void pin_output(volatile uint8_t* ddr, uint8_t bit) { *ddr |= (1u << bit); }
static inline void pin_input(volatile uint8_t* ddr, uint8_t bit)  { *ddr &= (uint8_t)~(1u << bit); }
static inline void pin_write(volatile uint8_t* port, uint8_t bit, uint8_t v) {
    if (v) *port |= (1u << bit);
    else   *port &= (uint8_t)~(1u << bit);
}
static inline uint8_t pin_read(volatile uint8_t* pinr, uint8_t bit) {
    return (*pinr & (1u << bit)) ? 1u : 0u;
}

static inline uint8_t read_pressed_level(void) {
    uint8_t level = pin_read(&BTN_PINR, BTN_BIT);
#if BUTTON_ACTIVE_LOW
    return (level == 0u) ? 1u : 0u;
#else
    return (level != 0u) ? 1u : 0u;
#endif
}

/* putc: LEDs controlled by printing characters */
static int hw_putchar(char c, FILE* stream) {
    (void)stream;

    switch (c) {
        case HW_LED_GREEN_ON:   pin_write(&LEDG_PORT, LEDG_BIT, 1u); break;
        case HW_LED_GREEN_OFF:  pin_write(&LEDG_PORT, LEDG_BIT, 0u); break;

        case HW_LED_RED_ON:     pin_write(&LEDR_PORT, LEDR_BIT, 1u); break;
        case HW_LED_RED_OFF:    pin_write(&LEDR_PORT, LEDR_BIT, 0u); break;

        case HW_LED_YELLOW_ON:  pin_write(&LEDY_PORT, LEDY_BIT, 1u); break;
        case HW_LED_YELLOW_OFF: pin_write(&LEDY_PORT, LEDY_BIT, 0u); break;

        default: break;
    }
    return 0;
}

/* getc: returns raw level updates only when raw level changes, otherwise EOF (non-blocking)
   - First call returns current raw level so Task1 can initialize.
   - After that, returns EOF if unchanged.
*/
static int hw_getchar(FILE* stream) {
    (void)stream;

    static uint8_t last_pressed = 0;
    static uint8_t initialized = 0;

    uint8_t pressed = read_pressed_level();

    if (!initialized) {
        initialized = 1;
        last_pressed = pressed;
        return pressed ? (int)HW_BTN_RAW_PRESSED : (int)HW_BTN_RAW_RELEASED;
    }

    if (pressed != last_pressed) {
        last_pressed = pressed;
        return pressed ? (int)HW_BTN_RAW_PRESSED : (int)HW_BTN_RAW_RELEASED;
    }

    return _FDEV_EOF;
}

static FILE g_hw_stream;

void dev_hw_stdio_init(void) {
    /* Button input */
    pin_input(&BTN_DDR, BTN_BIT);
#if BUTTON_ACTIVE_LOW
    BTN_PORT |= (1u << BTN_BIT); /* pull-up */
#endif

    /* LED outputs */
    pin_output(&LEDG_DDR, LEDG_BIT);
    pin_output(&LEDR_DDR, LEDR_BIT);
    pin_output(&LEDY_DDR, LEDY_BIT);

    /* default off */
    pin_write(&LEDG_PORT, LEDG_BIT, 0u);
    pin_write(&LEDR_PORT, LEDR_BIT, 0u);
    pin_write(&LEDY_PORT, LEDY_BIT, 0u);

    fdev_setup_stream(&g_hw_stream, hw_putchar, hw_getchar, _FDEV_SETUP_RW);
}

FILE* dev_hw_stdio_stream(void) {
    return &g_hw_stream;
}