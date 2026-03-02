/*
 * app_shared.c
 *
 * Defines shared globals used across task modules.
 */

#include "app_shared.h"

/* Last press event (Task 1 -> Task 2). */
volatile PressEvent_t g_press_event = {0};

/* Statistics (Task 2 updates; Task 3 prints and resets). */
volatile Stats_t g_stats = {0};