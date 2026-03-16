#include "lcd_stdio.h"

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <stdio.h>

static LiquidCrystal_I2C* g_lcd = 0;
static uint8_t g_cols = 16;
static uint8_t g_rows = 2;
static uint8_t g_col = 0;
static uint8_t g_row = 0;
static FILE g_lcd_file;

static int lcd_putchar(char c, FILE* stream)
{
    (void)stream;

    if (g_lcd == 0) {
        return 0;
    }

    switch (c) {
        case '\f': /* clear + home */
            g_lcd->clear();
            g_col = 0;
            g_row = 0;
            g_lcd->setCursor(0, 0);
            break;

        case '\v': /* home = row 0, col 0 */
            g_col = 0;
            g_row = 0;
            g_lcd->setCursor(0, 0);
            break;

        case '\n': /* next row */
            g_row = 1;
            g_col = 0;
            g_lcd->setCursor(0, 1);
            break;

        case '\a': /* backlight toggle off if you still want it */
            g_lcd->noBacklight();
            break;

        case '\r': /* optional explicit carriage return/home */
            g_col = 0;
            g_lcd->setCursor(0, g_row);
            break;

        default:
            g_lcd->write((uint8_t)c);
            if (g_col + 1 < g_cols) {
                g_col++;
            }
            break;
    }

    return 0;
}

extern "C" {

void lcd_stdio_init(unsigned char addr, unsigned char cols, unsigned char rows)
{
    if (g_lcd == 0) {
        g_cols = cols;
        g_rows = rows;
        g_lcd = new LiquidCrystal_I2C(addr, cols, rows);
        g_lcd->init();
        g_lcd->backlight();
        g_lcd->clear();
        g_lcd->setCursor(0, 0);
        g_col = 0;
        g_row = 0;

        fdev_setup_stream(&g_lcd_file, lcd_putchar, NULL, _FDEV_SETUP_WRITE);
    }
}

FILE* lcd_stdio_stream(void)
{
    return &g_lcd_file;
}

}