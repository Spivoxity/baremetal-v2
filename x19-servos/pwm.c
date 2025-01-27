/* x3500/pwm.c */
/* Copyright (c) 2021 J. M. Spivey */

#include "microbian.h"
#include "hardware.h"

/* Pulse width modulation.  We'll use timer 2 to generate two PWM
   signals, one for each wheel motor.  It counts at 1MHz and resets
   once evey 20000 counts or 20 millisec.

   The timer has four compare-and-capture channels.  We use channel 2
   to set both the output signals high at the beginning of the period,
   and channels 0 and 1 to set them low again at the end of the pulse.

   Events on each counter channel are connected via the "Programmable
   Peripheral Interconnect" (PPI) system to GPIO Tasks in the GPIOTE
   module, and these tasks indirectly toggle the I/O pins. 

      Timer 2         PPI          GPIOTE        GPIO
      COMPARE       EEP TEP         OUT
         0 ----------> 2 -----+
                              +----> 0 --------> PAD1
                +----> 0 -----+
         2 -----+
                +----> 1 -----+
                              +----> 1 --------> PAD2
         1 ----------> 3 -----+

*/

static int PWM_TASK;            /* Process ID */

#define WIDTH 23                /* Message type for pulse width change */

static void pwm_setup(void)
{
    /* Pads 1 and 2 are outputs */
    gpio_dir(PAD1, 1); gpio_dir(PAD2, 1);

    /* GPIOTE channels 0 and 1 toggle the two pins */
    GPIOTE.CONFIG[0] =
        FIELD(GPIOTE_CONFIG_MODE, GPIOTE_MODE_Task)
        | FIELD(GPIOTE_CONFIG_PSEL, PAD1)
        | FIELD(GPIOTE_CONFIG_POLARITY, GPIOTE_POLARITY_Toggle)
        | FIELD(GPIOTE_CONFIG_OUTINIT, 0);
    GPIOTE.CONFIG[1] =
        FIELD(GPIOTE_CONFIG_MODE, GPIOTE_MODE_Task)
        | FIELD(GPIOTE_CONFIG_PSEL, PAD2)
        | FIELD(GPIOTE_CONFIG_POLARITY, GPIOTE_POLARITY_Toggle)
        | FIELD(GPIOTE_CONFIG_OUTINIT, 0);

    /* Set up timer 2 to count at 1MHz and reset at 20000 counts */
    TIMER2.STOP = 1;
    TIMER2.MODE = TIMER_MODE_Timer;
    TIMER2.BITMODE = TIMER_BITMODE_16Bit;
    TIMER2.PRESCALER = 4;       /* 1MHz=16MHz / 2^4 */
    TIMER2.CLEAR = 1;
    TIMER2.CC[0] = 65535;
    TIMER2.CC[1] = 65535;
    TIMER2.CC[2] = 20000;
    TIMER2.SHORTS = BIT(TIMER_COMPARE2_CLEAR);

    /* PPI channels 0 and 2 toggle pin 1; channels 1 and 3 toggle the other */
    PPI.CH[0].EEP = &TIMER2.COMPARE[2];
    PPI.CH[0].TEP = &GPIOTE.OUT[0];
    PPI.CH[1].EEP = &TIMER2.COMPARE[2];
    PPI.CH[1].TEP = &GPIOTE.OUT[1];
    PPI.CH[2].EEP = &TIMER2.COMPARE[0];
    PPI.CH[2].TEP = &GPIOTE.OUT[0];
    PPI.CH[3].EEP = &TIMER2.COMPARE[1];
    PPI.CH[3].TEP = &GPIOTE.OUT[1];
    PPI.CHENSET = BIT(2) | BIT(3);

    /* Start your engines! */
    TIMER2.START = 1;
}

static volatile unsigned width[2];

/* timer2_handler -- special interrupt handler */
void timer2_handler(void)
{
    /* Set the CC registers to their new values */
    TIMER2.CC[0] = width[0];
    TIMER2.CC[1] = width[1];

    /* No more interrupts needed */
    TIMER2.INTENCLR = BIT(TIMER_INT_COMPARE2);

    /* Wake up the driver process */
    interrupt(PWM_TASK);
}

static void set_width(int chan, int wid)
{
    if (wid >= 5) {
        /* Genuine pulses: enable the start-of-pulse transition and set width */
        width[chan] = wid;
        PPI.CHENSET = BIT(chan);
    } else {
        /* No pulses: disable pulse start, and set impossible width; */
        width[chan] = 65535;
        PPI.CHENCLR = BIT(chan);
    }
}

static void pwm_task(int arg)
{
    message m;
    int client;
    
    pwm_setup();

    while (1) {
        receive(ANY, &m);
        switch (m.type) {
        case WIDTH:
            client = m.sender;
            set_width(0, m.int1);
            set_width(1, m.int2);

            /* Enable an interrupt to update the timer */
            TIMER2.COMPARE[2] = 0;
            clear_pending(TIMER2_IRQ);
            TIMER2.INTENSET = BIT(TIMER_INT_COMPARE2);
            enable_irq(TIMER2_IRQ);
            receive_msg(INTERRUPT);
            send_msg(client, REPLY);
            break;

        default:
            badmesg(m.type);
        }
    }
}

void pwm_change(int width1, int width2)
{
    message m;
    m.type = WIDTH;
    m.int1 = width1;
    m.int2 = width2;
    sendrec(PWM_TASK, &m);
}

void pwm_init(void)
{
    PWM_TASK = start("PWM", pwm_task, 0, STACK);    
}
