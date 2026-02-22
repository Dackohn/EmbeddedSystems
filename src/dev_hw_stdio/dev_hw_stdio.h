#pragma once
#include <stdio.h>
#include <stdint.h>

void dev_hw_stdio_init(void);
FILE* dev_hw_stdio_stream(void);

/* LED commands written to the HW stream */
#define HW_LED_GREEN_ON   'g'
#define HW_LED_GREEN_OFF  'G'
#define HW_LED_RED_ON     'r'
#define HW_LED_RED_OFF    'R'
#define HW_LED_YELLOW_ON  'y'
#define HW_LED_YELLOW_OFF 'Y'

/* Button raw level updates read from the HW stream */
#define HW_BTN_RAW_PRESSED   '1'
#define HW_BTN_RAW_RELEASED  '0'