#pragma once

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void thermistor_stdio_init(void);
FILE* thermistor_stdio_stream(void);

#ifdef __cplusplus
}
#endif