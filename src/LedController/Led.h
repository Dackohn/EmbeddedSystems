#pragma once
#include <Arduino.h>

class Led {
public:
  /** Construct LED controller for given digital pin. */
  Led(uint8_t pin);
  /** Configure pin as output and ensure LED is off. */
  void begin();
  /** Turn LED on (drive HIGH). */
  void on();
  /** Turn LED off (drive LOW). */
  void off();

private:
  uint8_t pin;
};