#pragma once
#include <Arduino.h>
#include <stdio.h>

class Keypad4x4;
class Lcd;

class Stdio {
public:
  static void begin();

  /** Bind a keypad instance used for stdin reads. */
  static void bindKeypad(Keypad4x4* kp);
  /** Bind an LCD instance used for stdout writes. */
  static void bindLcd(Lcd* lcd);

  /** Redirect `stdin` to the keypad input stream. */
  static void useKeypadIn();  // stdin -> keypad (scanf)
  /** Redirect `stdout` to the LCD output stream. */
  static void useLcdOut();    // stdout -> lcd (printf)
  
  /** Enable or disable local echo to the LCD. */
  static void setEcho(bool enabled);
  /** Set position where echoed input will start on the LCD. */
  static void setEchoPos(uint8_t col, uint8_t row);
  /** Clear `count` characters in the echo area. */
  static void clearEchoArea(uint8_t count);

private:
  static int inGet(FILE* f);
  static int outPut(char c, FILE* f);

  static FILE inFile;
  static FILE outFile;

  static Keypad4x4* keypad;
  static Lcd* lcd;
};