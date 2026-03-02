/*
 * board_select.h
 *
 * Compile-time board selection.
 * You must define one of the following macros in your build flags:
 *   - BOARD_UNO
 *   - BOARD_MEGA2560
 *
 * The selected header defines the AVR port registers and bit positions
 * used for button input and the three LEDs.
 */

#pragma once

#if defined(BOARD_UNO)
  #include "board_uno.h"
#elif defined(BOARD_MEGA2560)
  #include "board_mega2560.h"
#elif defined(BOARD_ESP32)
  #include "board_esp32.h"
#else
  #error "Define BOARD_UNO or BOARD_MEGA2560 in compiler flags"
#endif