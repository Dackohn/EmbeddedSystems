#include <Arduino.h>
#include <stdio.h>
#include <string.h>

#include "LcdHd44780/Lcd.h"
#include "KeypadStdio/Keypad4x4.h"
#include "LedController/Led.h"
#include "SerialStdio/Stdio.h"

// LCD 16x2 (4-bit)
static const uint8_t LCD_RS = 12;
static const uint8_t LCD_EN = 11;
static const uint8_t LCD_D4 = 5;
static const uint8_t LCD_D5 = 4;
static const uint8_t LCD_D6 = 3;
static const uint8_t LCD_D7 = 2;

// Keypad pins
static const uint8_t ROWS[4] = {A0, A1, A2, A3};
static const uint8_t COLS[4] = {6, 7, 8, 9};

static const char MAP[4][4] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

// LEDs
static const uint8_t LED_GREEN_PIN = 10;
static const uint8_t LED_RED_PIN   = 13;

Lcd lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
Keypad4x4 keypad(ROWS, COLS, MAP);
Led ledGreen(LED_GREEN_PIN);
Led ledRed(LED_RED_PIN);

static bool isLocked = true;
static char password[17] = "1234";   // current password (can be extended)

/** Update LCD and LEDs to display current lock state. */
static void showLockState() {
  lcd.clear();
  if (isLocked) {
    ledRed.on(); ledGreen.off();
    printf("State: LOCKED\n");
    printf("(* for options)");
  } else {
    ledGreen.on(); ledRed.off();
    printf("State: UNLOCK\n");
    printf("(* for options)");
  }
}

/** Show the top-level command options on the LCD. */
static void showRootOptions() {
  lcd.clear();
  printf("Commands:\n");
  printf("*0 *1 *2 *3");
}

/** Display help/usage for a specific command code. */
static void showOptionsForCmd(char cmd) {
  lcd.clear();
  if (cmd == '0') {
    printf("*0#\n");
    printf("Lock");
  } else if (cmd == '1') {
    printf("*1*PASS#\n");
    printf("Unlock");
  } else if (cmd == '2') {
    printf("*2*OLD*NEW#\n");
    printf("Change pass");
  } else if (cmd == '3') {
    printf("*3#\n");
    printf("Show state");
  } else {
    printf("Invalid cmd\n");
    printf("(* for opts)");
  }
}

// Reads one token until '*' or '#'. The found delimiter is stored in outDelim.
// The token can be empty (e.g., immediately after '*').
/**
 * Read a token from stdin until '*' or '#' is seen.
 * Stores the token (NUL-terminated) into `out` and the delimiter into `outDelim`.
 * The token may be empty.
 */
static void readToken(char* out, size_t outSize, char* outDelim) {
  if (outSize == 0) return;

  size_t i = 0;
  char ch = 0;

  out[0] = '\0';
  *outDelim = 0;

  while (true) {
    if (scanf("%c", &ch) != 1) continue;

    if (ch == '\r') continue;

    if (ch == '#') {
      *outDelim = '#';
      out[i] = '\0';
      return;
    }

    if (ch == '\n') {
      *outDelim = '#';
      out[i] = '\0';
      return;
    }

    if (ch == '*') {
      *outDelim = '*';
      out[i] = '\0';
      return;
    }

    if (i + 1 < outSize) {
      out[i++] = ch;
      out[i] = '\0';
    }
  }
}

/**
 * Execute a parsed command `cmd` with optional operands `op1` and `op2`.
 * Updates lock state, LEDs and prints results to the LCD.
 */
static void execCommand(const char* cmd, const char* op1, const char* op2) {
  lcd.clear();

  if (strcmp(cmd, "0") == 0) {
    isLocked = true;
    ledRed.on(); ledGreen.off();
    printf("OK: LOCKED\n");
    printf("Door locked");
    return;
  }

  if (strcmp(cmd, "1") == 0) {
    if (op1[0] == '\0') {
      printf("Error:\n");
      printf("Missing pass");
      return;
    }
    if (strcmp(op1, password) == 0) {
      isLocked = false;
      ledGreen.on(); ledRed.off();
      printf("OK: UNLOCK\n");
      printf("Access granted");
    } else {
      isLocked = true;
      ledRed.on(); ledGreen.off();
      printf("FAIL\n");
      printf("Wrong pass");
    }
    return;
  }

  if (strcmp(cmd, "2") == 0) {
    if (op1[0] == '\0' || op2[0] == '\0') {
      printf("Error:\n");
      printf("Format *2*O*N#");
      return;
    }
    if (strcmp(op1, password) != 0) {
      printf("FAIL\n");
      printf("Old pass bad");
      return;
    }

    strncpy(password, op2, sizeof(password) - 1);
    password[sizeof(password) - 1] = '\0';

    printf("OK: Changed\n");
    printf("New password");
    return;
  }

  if (strcmp(cmd, "3") == 0) {
    showLockState();
    return;
  }

  printf("Command?\n");
  printf("Invalid");
}

void setup() {
  keypad.begin();
  lcd.begin(16, 2);
  ledGreen.begin();
  ledRed.begin();

  Stdio::begin();
  Stdio::bindKeypad(&keypad);
  Stdio::bindLcd(&lcd);

  Stdio::useLcdOut();
  Stdio::useKeypadIn();

  showRootOptions();
  delay(1200);
  showLockState();
  delay(1200);
}

void loop() {
  // General format: * <cmd> [* op1] [* op2] #
  // Rules: '*' delimiter, '#' execute
  char token[17];
  char cmd[4] = {0};
  char op1[17] = {0};
  char op2[17] = {0};
  char delim = 0;

  // 1) Wait for the first '*'
  do {
    readToken(token, sizeof(token), &delim);
  } while (delim != '*');

  // On every '*': show options
  showRootOptions();
  delay(700);

  // 2) Read CMD until next '*' or '#'
  readToken(cmd, sizeof(cmd), &delim);

  if (cmd[0] == '\0') {
    lcd.clear();
    printf("Error:\n");
    printf("Missing cmd");
    delay(1200);
    return;
  }

  if (delim == '*') {
    showOptionsForCmd(cmd[0]);
    delay(800);
  }

  // 3) If ended with '#', execute (no-operand commands: *0#, *3#)
  if (delim == '#') {
    execCommand(cmd, "", "");
    delay(1500);
    return;
  }

  // 4) Read op1 until '*' or '#'
  readToken(op1, sizeof(op1), &delim);

  if (delim == '*') {
    showOptionsForCmd(cmd[0]);
    delay(800);
  }

  if (delim == '#') {
    execCommand(cmd, op1, "");
    delay(1500);
    return;
  }

  // 5) Read op2 until '#'
  readToken(op2, sizeof(op2), &delim);

  execCommand(cmd, op1, op2);
  delay(1500);
}