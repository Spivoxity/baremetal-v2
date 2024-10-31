/* x12-intrmech/bitbang.c */
/* Copyright (c) 2020 J. M. Spivey */

#include "hardware.h"

/* Square wave outputs on pads 0 (bit-banged in software) and 1
   (hardware control).  Meanwhile, the random number generator is
   causing interrupts at irregular intervals, with a pulse on pad 2. */

#define nop5()   nop(); nop(); nop(); nop(); nop()
#define nop10()  nop5(); nop5()

/* square_out -- generate square wave by bit-banging */
CODERAM void square_out(void)
{
    while (1) {
        GPIO0.OUTSET = BIT(PAD0);
        nop10(); nop10(); nop10(); nop(); // 31 cycles
        GPIO0.OUTCLR = BIT(PAD0);
        nop10(); nop10(); nop5(); nop(); nop(); nop(); nop(); // 29 cycles
    }
}    

/* sqwave_init -- configure PPI to output a square wave on PAD1 */
void sqwave_init(void)
{
    /* Set up timer 0 to reset once every 500ns */
    TIMER0.STOP = 1;
    TIMER0.MODE = TIMER_MODE_Timer;
    TIMER0.BITMODE = TIMER_BITMODE_16Bit;
    TIMER0.PRESCALER = 0;       // Clocked at 16MHz
    TIMER0.CLEAR = 1;
    TIMER0.CC[0] = 8;           // Reset once every 8 counts
    TIMER0.SHORTS = BIT(TIMER_COMPARE0_CLEAR);
    
    /* Set up "GPIO Tasks and Events" channel 0 to toggle PAD1 */
    GPIOTE.CONFIG[0] =
        FIELD(GPIOTE_CONFIG_MODE, GPIOTE_MODE_Task)
        | FIELD(GPIOTE_CONFIG_PSEL, PAD1)
        | FIELD(GPIOTE_CONFIG_POLARITY, GPIOTE_POLARITY_Toggle)
        | FIELD(GPIOTE_CONFIG_OUTINIT, 0);
        
    /* Set up "Programmable Peripheral Interconnect" channel 0 
       to link timer and GPIOTE */
    PPI.CH[0].EEP = &TIMER0.COMPARE[0];
    PPI.CH[0].TEP = &GPIOTE.OUT[0];
    PPI.CHENSET = BIT(0);
    
    /* Start the timer */
    TIMER0.START = 1;
}

/* rng_init -- initialise hardware random number generator */
void rng_init(void)
{
    /* Setting the DERCEN bit enables the bias elimination algorithm,
       and makes the intervals between random bytes irregular. */
    SET_BIT(RNG.CONFIG, RNG_CONFIG_DERCEN);
    RNG.VALRDY = 0;
    RNG.INTENSET = BIT(RNG_INT_VALRDY);
    enable_irq(RNG_IRQ);
    RNG.START = 1;
}

/* rng_handler -- interrupt handler for random number generator */
void rng_handler(void)
{
    GPIO0.OUTSET = BIT(PAD2);
    RNG.VALRDY = 0;             /* Just acknowledge the interrupt */
    GPIO0.OUTCLR = BIT(PAD2);
}

void init(void)
{
    gpio_dir(PAD0, 1);
    gpio_dir(PAD1, 1);
    gpio_dir(PAD2, 1);

    rng_init();
    sqwave_init();
    square_out();
}
