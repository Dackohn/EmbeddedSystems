#include "led_stdio.h"

#include "../../app_config.h"
#include <avr/io.h>
#include <stdio.h>

static int led_putchar(char c, FILE* stream);

static FILE g_led_file = FDEV_SETUP_STREAM(led_putchar, NULL, _FDEV_SETUP_WRITE);

void led_stdio_init(void)
{
    LED_DDR |= (1U << LED_BIT);
    LED_PORT &= (uint8_t)~(1U << LED_BIT);
}

FILE* led_stdio_stream(void)
{
    return &g_led_file;
}

static int led_putchar(char c, FILE* stream)
{
    (void)stream;

    if (c == '1') {
        LED_PORT |= (1U << LED_BIT);
    } else if (c == '0') {
        LED_PORT &= (uint8_t)~(1U << LED_BIT);
    }

    return 0;
}