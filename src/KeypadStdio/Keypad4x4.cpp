#include "Keypad4x4.h"

/**
 * Construct keypad helper and copy pin/map configuration.
 */
Keypad4x4::Keypad4x4(const uint8_t rowPins[4], const uint8_t colPins[4], const char map[4][4]) {
  for (uint8_t i = 0; i < 4; i++) { rows[i] = rowPins[i]; cols[i] = colPins[i]; }
  for (uint8_t r = 0; r < 4; r++)
    for (uint8_t c = 0; c < 4; c++)
      keymap[r][c] = map[r][c];
}

/**
 * Configure pin modes: rows as outputs (idle HIGH), columns as inputs with pull-ups.
 */
void Keypad4x4::begin() {
  for (uint8_t r = 0; r < 4; r++) {
    pinMode(rows[r], OUTPUT);
    digitalWrite(rows[r], HIGH);
  }
  for (uint8_t c = 0; c < 4; c++) {
    pinMode(cols[c], INPUT_PULLUP);
  }
}

/**
 * Scan the matrix once by driving each row low and reading columns.
 * Returns the mapped character for the first detected key, or 0 if none.
 */
char Keypad4x4::scanOnce() {
  for (uint8_t r = 0; r < 4; r++) {
    digitalWrite(rows[r], LOW);
    for (uint8_t c = 0; c < 4; c++) {
      if (digitalRead(cols[c]) == LOW) {
        digitalWrite(rows[r], HIGH);
        return keymap[r][c];
      }
    }
    digitalWrite(rows[r], HIGH);
  }
  return 0;
}

/**
 * Wait for a keypress, perform debounce check, and wait for release.
 * Returns the confirmed key character.
 */
char Keypad4x4::getKeyBlocking() {
  while (true) {
    char k = 0;
    while ((k = scanOnce()) == 0) delay(5);

    delay(25); // debounce
    char k2 = scanOnce();
    if (k2 != k) {
      // unstable press; restart without recursion (prevents stack growth)
      continue;
    }

    while (scanOnce() != 0) delay(5); // wait release
    return k;
  }
}