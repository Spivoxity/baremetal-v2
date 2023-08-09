/* x31-adc/lights.c */
/* Copyright (c) 2020 J. M. Spivey */

#include "microbian.h"
#include "lib.h"
#include "hardware.h"

#define CHAN PAD2

#define PATTERN(a, b, c, d, e, f, g, h, i) \
    IMAGE(a, b, c, d, e, \
          b, c, d, e, f, \
          c, d, e, f, g, \
          d, e, f, g, h, \
          e, f, g, h, i)

image pattern[19] = {
    PATTERN(0, 0, 0, 0, 0, 0, 0, 0, 0),
    PATTERN(1, 0, 0, 0, 0, 0, 0, 0, 0),
    PATTERN(1, 1, 0, 0, 0, 0, 0, 0, 0),
    PATTERN(1, 1, 1, 0, 0, 0, 0, 0, 0),
    PATTERN(1, 1, 1, 1, 0, 0, 0, 0, 0),
    PATTERN(1, 1, 1, 1, 1, 0, 0, 0, 0),
    PATTERN(1, 1, 1, 1, 1, 1, 0, 0, 0),
    PATTERN(1, 1, 1, 1, 1, 1, 1, 0, 0),
    PATTERN(1, 1, 1, 1, 1, 1, 1, 1, 0),
    PATTERN(1, 1, 1, 1, 1, 1, 1, 1, 1),
    PATTERN(0, 1, 1, 1, 1, 1, 1, 1, 1),
    PATTERN(0, 0, 1, 1, 1, 1, 1, 1, 1),
    PATTERN(0, 0, 0, 1, 1, 1, 1, 1, 1),
    PATTERN(0, 0, 0, 0, 1, 1, 1, 1, 1),
    PATTERN(0, 0, 0, 0, 0, 1, 1, 1, 1),
    PATTERN(0, 0, 0, 0, 0, 0, 1, 1, 1),
    PATTERN(0, 0, 0, 0, 0, 0, 0, 1, 1),
    PATTERN(0, 0, 0, 0, 0, 0, 0, 0, 1),
    PATTERN(0, 0, 0, 0, 0, 0, 0, 0, 0)
}; 

int adc_reading(int chan);
void adc_init(void);

void light_task(int arg)
{
    int x;

    timer_pulse(100);

    while (1) {
        receive(PING, NULL);
        x = adc_reading(CHAN);
        display_show(pattern[x/54]);
    }
}

void init(void)
{
    timer_init();
    display_init();
    adc_init();
    start("Main", light_task, 0, STACK);
}
