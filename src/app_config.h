#pragma once

#include <stdint.h>

/* UART */
#define APP_UART_BAUD 9600UL

/* LCD */
#define APP_LCD_I2C_ADDR               0x27
#define APP_LCD_COLS                   16
#define APP_LCD_ROWS                   2

/* Periods */
#define APP_TASK_BUTTON_MS             20U
#define APP_TASK_SENSOR_MS             50U
#define APP_TASK_CONDITION_MS          20U
#define APP_TASK_ALERT_MS              20U
#define APP_TASK_UI_MS                 250U

/* Logic */
#define APP_BUTTON_DEBOUNCE_MS         50U
#define APP_ALERT_CONFIRM_MS           120U

/* Critical temperature band in deci-Celsius */
#define APP_ALERT_LOW_dC               240
#define APP_ALERT_HIGH_dC              260

/* Thermistor constants */
#define APP_THERM_SERIES_OHMS          10000.0f
#define APP_THERM_NOMINAL_OHMS         10000.0f
#define APP_THERM_NOMINAL_TEMP_C       25.0f
#define APP_THERM_BETA                 3950.0f

/* Sensor sanity */
#define APP_ADC_OPEN_THRESHOLD         1015U
#define APP_ADC_SHORT_THRESHOLD        8U
#define APP_TEMP_MIN_dC                (240)
#define APP_TEMP_MAX_dC                (260)
#define APP_MAX_STEP_dC                80

/* Task stack sizes */
#define APP_STACK_BUTTON               256U
#define APP_STACK_SENSOR               384U
#define APP_STACK_CONDITION            256U
#define APP_STACK_ALERT                192U
#define APP_STACK_UI                   512U

/* Warning flags */
#define APP_WARN_OPEN                  (1u << 0)
#define APP_WARN_SHORT                 (1u << 1)
#define APP_WARN_RANGE                 (1u << 2)
#define APP_WARN_NOISY                 (1u << 3)

/* Supported boards */
#if defined(__AVR_ATmega328P__)
  #define APP_BOARD_NAME "UNO"

  /* Button D7 -> PD7 */
  #define BTN_DDR   DDRD
  #define BTN_PORT  PORTD
  #define BTN_PINR  PIND
  #define BTN_BIT   PD7

  /* LED D13 -> PB5 */
  #define LED_DDR   DDRB
  #define LED_PORT  PORTB
  #define LED_BIT   PB5

#elif defined(__AVR_ATmega2560__)
  #define APP_BOARD_NAME "MEGA2560"

  /* Button D12 -> PB6 */
  #define BTN_DDR   DDRB
  #define BTN_PORT  PORTB
  #define BTN_PINR  PINB
  #define BTN_BIT   PB6

  /* LED D13 -> PB7 */
  #define LED_DDR   DDRB
  #define LED_PORT  PORTB
  #define LED_BIT   PB7

#else
  #error "Unsupported AVR board. Add pin mapping in app_config.h"
#endif