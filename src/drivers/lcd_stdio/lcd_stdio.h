#pragma once

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void lcd_stdio_init(unsigned char addr, unsigned char cols, unsigned char rows);
FILE* lcd_stdio_stream(void);

#ifdef __cplusplus
}
#endif