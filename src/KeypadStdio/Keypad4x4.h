#pragma once
#include <Arduino.h>

class Keypad4x4 {
public:
  /**
   * Construct a 4x4 keypad helper.
   * @param rowPins Array of 4 pins used as row outputs.
   * @param colPins Array of 4 pins used as column inputs (with pull-ups).
   * @param map 4x4 character mapping for keys.
   */
  Keypad4x4(const uint8_t rowPins[4], const uint8_t colPins[4], const char map[4][4]);

  /** Initialize pins and set rows to idle (HIGH). */
  void begin();

  /**
   * Block until a stable keypress is detected and released.
   * Returns the pressed key character.
   */
  char getKeyBlocking();

private:
  uint8_t rows[4];
  uint8_t cols[4];
  char keymap[4][4];

  /**
   * Perform a single non-blocking scan of the matrix.
   * Returns the pressed key character or 0 if none.
   */
  char scanOnce();
};