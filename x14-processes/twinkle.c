/* x14-processes/twinkle.c */
/* Copyright (c) 2020 J. M. Spivey */

#include "hardware.h"
#include "microbian.h"

/* period -- coprime periods for the LEDs */
const unsigned period[] = {
    11, 13, 14, 15, 17, 19, 23
};

#define JIFFY 25 /* milliseconds */

/* blink_task -- blink an LED */
void blink_task(int j)
{
    while (1) {
        timer_delay(period[j] * JIFFY);
        GPIO_OUTCLR = BIT(j+4);
        timer_delay(period[j] * JIFFY);
        GPIO_OUTSET = BIT(j+4);
    }
}

void init(void)
{
    GPIO_DIR = 0xfff0;
    GPIO_OUT = 0x5ff0;

    timer_init();
    for (int i = 0; i < 7; i++) {
        start("Blink", blink_task, i, 256);
    }
}
        
    
