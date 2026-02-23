#pragma once
#include <stdio.h>
#include <stdint.h>

/*
 * dev_hw_stdio
 *
 * Hardware is accessed ONLY through this STDIO stream:
 *  - fputc() sends LED commands
 *  - fgetc() reads debounced button edge events (PRESS/RELEASE)
 *
 * Internally, this module uses:
 *  - MCAL button/led drivers for actual GPIO access
 *  - ECAL button for debouncing and event generation
 *
 * This satisfies:
 *  - "hardware communication via standard IO"
 *  - "drivers must exist and be working"
 */

void dev_hw_stdio_init(void);
FILE* dev_hw_stdio_stream(void);

/* LED commands written to the HW stream */
#define HW_LED_GREEN_ON   'g'
#define HW_LED_GREEN_OFF  'G'
#define HW_LED_RED_ON     'r'
#define HW_LED_RED_OFF    'R'
#define HW_LED_YELLOW_ON  'y'
#define HW_LED_YELLOW_OFF 'Y'

/* Button events read from the HW stream */
#define HW_BTN_PRESS_EVT   'P'
#define HW_BTN_RELEASE_EVT 'R'