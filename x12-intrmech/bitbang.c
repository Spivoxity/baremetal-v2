/* x12-intrmech/bitbang.c */
/* Copyright (c) 2020 J. M. Spivey */

#include "hardware.h"

/* rng_init -- initialise hardware random number generator */
void rng_init(void)
{
    /* Setting the DERCEN bit enables the bias elimination algorithm,
       and makes the intervals between random bytes irregular. */
    SET_BIT(RNG_CONFIG, RNG_CONFIG_DERCEN);
    RNG_VALRDY = 0;
    RNG_INTENSET = BIT(RNG_INT_VALRDY);
    enable_irq(RNG_IRQ);
    RNG_START = 1;
}

/* rng_handler -- interrupt handler for random number generator */
void rng_handler(void)
{
    if (RNG_VALRDY) {
        gpio_out(PAD1, 1);
        RNG_VALRDY = 0;         /* Just acknowledge the interrupt */
        gpio_out(PAD1, 0);
    }
}

#define nop5()   nop(); nop(); nop(); nop(); nop()
#define nop10()  nop5(); nop5()

/* square_out -- generate square wave by bit-banging */
CODERAM void square_out(void)
{
    while (1) {
        gpio_out(PAD0, 0);
        nop10(); nop10(); nop10(); nop();
        gpio_out(PAD0, 1);
        nop10(); nop10(); nop5(); nop(); nop(); nop(); nop();
    }
}    

void init(void)
{
    gpio_dir(PAD0, 1);
    gpio_dir(PAD1, 1);

    rng_init();
    square_out();
}
