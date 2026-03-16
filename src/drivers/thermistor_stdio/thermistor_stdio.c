#include "thermistor_stdio.h"

#include "../../app_config.h"

#include <Arduino_FreeRTOS.h>
#include <task.h>
#include <avr/io.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

static int thermistor_getchar(FILE* stream);

static FILE g_sensor_file = FDEV_SETUP_STREAM(NULL, thermistor_getchar, _FDEV_SETUP_READ);

static char g_line_buf[40];
static uint8_t g_line_idx = 0U;
static int16_t g_prev_temp_dC = 250;
static uint8_t g_have_prev = 0U;

static uint16_t adc_read_channel0(void)
{
    ADMUX = (1U << REFS0); /* AVcc ref, ADC0 */
    ADCSRA |= (1U << ADSC);
    while ((ADCSRA & (1U << ADSC)) != 0U) {}
    return ADC;
}

static void fill_sensor_line(void)
{
    uint32_t sum = 0UL;
    uint8_t i;
    uint16_t raw;
    uint8_t warnings = 0U;
    float resistance;
    float steinhart;
    float temp_c;
    int16_t temp_dC;
    int16_t step_dC;
    uint32_t resistance_u32;

    for (i = 0U; i < 8U; ++i) {
        sum += adc_read_channel0();
    }

    raw = (uint16_t)(sum / 8UL);

    if (raw >= APP_ADC_OPEN_THRESHOLD) {
        warnings |= APP_WARN_OPEN;
    }
    if (raw <= APP_ADC_SHORT_THRESHOLD) {
        warnings |= APP_WARN_SHORT;
    }

    if (raw == 0U) {
        raw = 1U;
    }
    if (raw >= 1023U) {
        raw = 1022U;
    }

    resistance = APP_THERM_SERIES_OHMS / ((1023.0f / (float)raw) - 1.0f);

    steinhart = resistance / APP_THERM_NOMINAL_OHMS;
    steinhart = log(steinhart);
    steinhart /= APP_THERM_BETA;
    steinhart += 1.0f / (APP_THERM_NOMINAL_TEMP_C + 273.15f);
    steinhart = 1.0f / steinhart;
    temp_c = steinhart - 273.15f;

    temp_dC = (int16_t)(temp_c * 10.0f);

    if ((temp_dC < APP_TEMP_MIN_dC) || (temp_dC > APP_TEMP_MAX_dC)) {
        warnings |= APP_WARN_RANGE;
    }

    if (g_have_prev != 0U) {
        step_dC = (int16_t)(temp_dC - g_prev_temp_dC);
        if (step_dC < 0) {
            step_dC = (int16_t)(-step_dC);
        }
        if (step_dC > APP_MAX_STEP_dC) {
            warnings |= APP_WARN_NOISY;
        }
    }

    g_prev_temp_dC = temp_dC;
    g_have_prev = 1U;
    resistance_u32 = (uint32_t)resistance;

    snprintf(g_line_buf, sizeof(g_line_buf), "%u %d %lu %u\n",
             raw,
             temp_dC,
             (unsigned long)resistance_u32,
             (unsigned int)warnings);

    g_line_idx = 0U;
}

void thermistor_stdio_init(void)
{
    /* AVcc reference, ADC enabled, prescaler 128 */
    ADMUX = (1U << REFS0);
    ADCSRA = (1U << ADEN) | (1U << ADPS2) | (1U << ADPS1) | (1U << ADPS0);
    ADCSRB = 0U;
    DIDR0 |= (1U << ADC0D);

    g_have_prev = 0U;
    g_prev_temp_dC = 250;
    g_line_idx = 0U;
    g_line_buf[0] = '\0';
}

FILE* thermistor_stdio_stream(void)
{
    return &g_sensor_file;
}

static int thermistor_getchar(FILE* stream)
{
    (void)stream;

    if (g_line_buf[g_line_idx] == '\0') {
        fill_sensor_line();
    }

    if (g_line_buf[g_line_idx] == '\0') {
        return _FDEV_EOF;
    }

    return g_line_buf[g_line_idx++];
}