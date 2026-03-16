#pragma once

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void led_stdio_init(void);
FILE* led_stdio_stream(void);

#ifdef __cplusplus
}
#endif