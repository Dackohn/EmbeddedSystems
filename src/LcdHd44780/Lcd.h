#pragma once
#include <Arduino.h>
#include <LiquidCrystal.h>

class Lcd {
public:
  Lcd(uint8_t rs, uint8_t en, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);

  /** Create Lcd adapter using 4-bit LiquidCrystal pins. */
  Lcd(uint8_t rs, uint8_t en, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);

  /** Initialize the display with given columns and rows. */
  void begin(uint8_t cols, uint8_t rows);
  /** Clear the display and reset cursor tracking. */
  void clear();
  /** Set cursor position (wraps within configured size). */
  void setCursor(uint8_t col, uint8_t row);

  /**
   * Output a single character to the display.
   * Handles newline and carriage-return semantics.
   */
  void put(char c);
  /** Print a NUL-terminated string via `put`. */
  void printStr(const char* s);

private:
  LiquidCrystal lcd;
  uint8_t cols;
  uint8_t rows;
  uint8_t col;
  uint8_t row;

  void newline();
};