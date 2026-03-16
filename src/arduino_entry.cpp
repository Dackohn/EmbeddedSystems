#include <Arduino.h>
#include <Arduino_FreeRTOS.h>

extern "C" {
#include "app_config.h"
#include "app_shared/app_shared.h"
#include "drivers/uart_stdio/uart_stdio.h"
#include "drivers/button_stdio/button_stdio.h"
#include "drivers/led_stdio/led_stdio.h"
#include "drivers/thermistor_stdio/thermistor_stdio.h"
#include "drivers/lcd_stdio/lcd_stdio.h"
#include "rtos_tasks/rtos_tasks.h"
}

void setup(void)
{
    uart_stdio_init(APP_UART_BAUD);
    button_stdio_init();
    led_stdio_init();
    thermistor_stdio_init();
    lcd_stdio_init(APP_LCD_I2C_ADDR, APP_LCD_COLS, APP_LCD_ROWS);

    app_shared_init();

    fprintf(stdout, "READY board=%s\n", APP_BOARD_NAME);
    fprintf(stdout, "Button toggles LCD display\n");

    rtos_tasks_create();
    /* Scheduler starts automatically after setup() returns in Arduino_FreeRTOS */
}

void loop(void)
{
}