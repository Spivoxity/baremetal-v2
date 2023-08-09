/* x19-servos/buggy.c */
/* Copyright (c) 2020 J. M. Spivey */

#include "microbian.h"
#include "pwm.h"

#define REST 1500               /* PWM setting for stationary wheel */
#define SPEED 500               /* PWM increment for motion */

/* drive -- set PWM to drive or rest */
void drive(int lspeed, int rspeed)
{
    pwm_change(REST-lspeed, REST+rspeed);
}

/* control_task -- repeat fixed sequence of motions */
void control_task(int dummy)
{
    timer_delay(1000);
    while (1) {
        drive(SPEED, SPEED);    /* Drive forward */
        timer_delay(1000);      /* ... for 1 second */
        drive(-SPEED, SPEED);   /* Turn left */
        timer_delay(1000);      /* ... for 1 second */
    }
}

void init(void)
{
    timer_init();
    pwm_init();
    start("Control", control_task, 0, STACK);
}
