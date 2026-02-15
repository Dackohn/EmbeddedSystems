#include "Stdio.h"
#include "KeypadStdio/Keypad4x4.h"
#include "LcdHd44780/Lcd.h"

FILE Stdio::inFile;
FILE Stdio::outFile;

Keypad4x4* Stdio::keypad = nullptr;
Lcd* Stdio::lcd = nullptr;

void Stdio::begin() {
  /**
   * Initialize FILE streams for stdin/stdout redirection using fdev hooks.
   */
  fdev_setup_stream(&inFile, nullptr, inGet, _FDEV_SETUP_READ);
  fdev_setup_stream(&outFile, outPut, nullptr, _FDEV_SETUP_WRITE);
}

void Stdio::bindKeypad(Keypad4x4* kp) { keypad = kp; }
void Stdio::bindLcd(Lcd* l) { lcd = l; }

void Stdio::useKeypadIn() { stdin = &inFile; }
void Stdio::useLcdOut() { stdout = &outFile; }

int Stdio::inGet(FILE*) {
  // Return -1 (EOF) if no keypad bound.
  if (!keypad) return -1;

  // Block until a confirmed keypress is available from the keypad.
  char k = keypad->getKeyBlocking();

  // Map special keypad keys to input delimiters/newline expected by app.
  if (k == '#') return '\n';   // submit
  if (k == '*') return '*';    // delimiter

  return (int)k;
}

int Stdio::outPut(char c, FILE*) {
  // If no LCD bound, silently consume output.
  if (!lcd) return 0;

  // Prevent printing non-printable control characters as garbage.
  // Allow newline handling inside Lcd::put().
  if ((uint8_t)c < 0x20 && c != '\n' && c != '\r') return 0;

  lcd->put(c);
  return 0;
}