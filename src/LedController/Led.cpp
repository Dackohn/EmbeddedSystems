#include "Led.h"

/** Create LED controller bound to `pin`. */
Led::Led(uint8_t pin) : pin(pin) {}

/** Configure the pin as output and ensure LED is off. */
void Led::begin() {
  pinMode(pin, OUTPUT);
  off();
}

/** Drive the LED pin HIGH. */
void Led::on() { digitalWrite(pin, HIGH); }

/** Drive the LED pin LOW. */
void Led::off() { digitalWrite(pin, LOW); }