#include "Lcd.h"

/** Construct Lcd adapter and initialize internal cursor state. */
Lcd::Lcd(uint8_t rs, uint8_t en, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
  : lcd(rs, en, d4, d5, d6, d7), cols(16), rows(2), col(0), row(0) {}

/** Initialize the LiquidCrystal instance and clear display. */
void Lcd::begin(uint8_t c, uint8_t r) {
  cols = c; rows = r;
  lcd.begin(cols, rows);
  clear();
}

/** Clear display and reset cursor tracking to 0,0. */
void Lcd::clear() {
  lcd.clear();
  col = 0; row = 0;
  lcd.setCursor(0, 0);
}

/** Set the tracked cursor position and update display cursor. */
void Lcd::setCursor(uint8_t c, uint8_t r) {
  col = c % cols;
  row = r % rows;
  lcd.setCursor(col, row);
}

/** Move to the start of the next line (wraps). */
void Lcd::newline() {
  col = 0;
  row = (row + 1) % rows;
  lcd.setCursor(col, row);
}

/** Write a character, handle newlines and wrapping. */
void Lcd::put(char c) {
  if (c == '\r') return;
  if (c == '\n') { newline(); return; }

  lcd.write((uint8_t)c);
  col++;
  if (col >= cols) newline();
}

/** Print a NUL-terminated string using `put`. */
void Lcd::printStr(const char* s) {
  while (*s) put(*s++);
}