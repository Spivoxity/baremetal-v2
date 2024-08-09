/* x17-drivers/myprimes.c */
/* Copyright (c) 2020 J. M. Spivey */

#include "microbian.h"
#include "lib.h"
#include "hardware.h"

void myserial_init(void);

int modulo(int a, int b)
{
    int r = a;
    while (r >= b) r -= b;
    return r;
}

/* prime -- test for primality */
int prime(int n)
{
    for (int k = 2; k * k <= n; k++) {
        if (modulo(n, k) == 0)
            return 0;
    }

    return 1;
}

void start_timer(void)
{
    /* Light an LED */
    led_dot();

    /* Start a timer */
    TIMER0.MODE = TIMER_MODE_Timer;
    TIMER0.BITMODE = TIMER_BITMODE_32Bit;
    TIMER0.PRESCALER = 4; /* Count at 1MHz */
    TIMER0.START = 1;
}

void stop_timer(void)
{
    /* Turn LED off */
    led_off();

    /* Fetch timer result */
    TIMER0.CAPTURE[0] = 1;
    unsigned time = TIMER0.CC[0];
    printf("%d millisec\n", (time+500)/1000);
}

/* prime_task -- print primes on the serial port */
void prime_task(int arg)
{
    int n = 2, count = 0;

    led_init();
    delay_loop(10000);
    start_timer();

    while (count < 500) {
        if (prime(n)) {
            count++;
            printf("prime(%d) = %d\n", count, n);
        }
        n++;
    }

    stop_timer();
}

void init(void)
{
    myserial_init();
    start("Prime", prime_task, 0, STACK);
}
