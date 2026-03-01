#ifdef USE_FREERTOS

#include <Arduino.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "app_config.h"
#include "mcal_uart0_stdio/mcal_uart0_stdio.h"
#include "dev_hw_stdio/dev_hw_stdio.h"
#include <Arduino_FreeRTOS.h>
#include "rtos_tasks/rtos_tasks.h"

void setup(void) {
    /* UART first so printf works immediately. */
    mcal_uart0_stdio_init(UART_BAUD);

    /* Hardware stream: button debounce + LED control via fputc/fgetc. */
    dev_hw_stdio_init();

    printf("READY: Button press monitor\n");
    printf("Commands: stats | reset\n");

    /* Create the three FreeRTOS tasks.
     * The scheduler starts automatically after setup() returns
     * (Arduino_FreeRTOS framework handles this). */
    rtos_tasks_create();
}

void loop(void) {
    /* Never called — FreeRTOS takes over after setup(). */
}

#endif /* USE_FREERTOS */