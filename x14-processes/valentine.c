/* x14-processes/valentine.c */
/* Copyright (c) 2020 J. M. Spivey */

#include "hardware.h"
#include "microbian.h"
#include "lib.h"

/* heart -- filled-in heart image */
const unsigned heart[] =
    IMAGE(0,1,0,1,0,
          1,1,1,1,1,
          1,1,1,1,1,
          0,1,1,1,0,
          0,0,1,0,0);

/* small -- small heart image */
const unsigned small[] =
    IMAGE(0,0,0,0,0,
          0,1,0,1,0,
          0,1,1,1,0,
          0,0,1,0,0,
          0,0,0,0,0);

/* show -- display three rows of a picture n times */
void show(const unsigned *img, int n)
{
    while (n-- > 0) {
        /* Takes 15msec per iteration */
        for (int p = 0; p < 10; p += 2) {
            GPIO0.OUT = img[p];
            GPIO1.OUT = img[p+1];
            timer_delay(3);
        }
    }
}

/* heart_task -- show beating heart */
void heart_task(int n)
{
    GPIO0.DIRSET = LED_MASK0;
    GPIO1.DIRSET = LED_MASK1;

    /* Set row pins to high-drive mode to increase brightness */
    gpio_drive(ROW1, GPIO_DRIVE_S0H1);
    gpio_drive(ROW2, GPIO_DRIVE_S0H1);
    gpio_drive(ROW3, GPIO_DRIVE_S0H1);
    gpio_drive(ROW4, GPIO_DRIVE_S0H1);
    gpio_drive(ROW5, GPIO_DRIVE_S0H1);

    /* priority(P_HIGH); */

    while (1) {
        show(heart, 70);
        show(small, 10);
        show(heart, 10);
        show(small, 10);
    }
}

/* This is a bit lighter than the earlier example, because we use GCC's
builtin modulo operation, rather than repeated subtraction.  That
leaves some CPU time over to look after the blinking lights. */

/* prime -- test for primality */
int prime(int n)
{
    for (int k = 2; k * k <= n; k++) {
        if (n % k == 0)
            return 0;
    }

    return 1;
}

/* prime_task -- print primes on the serial port */
void prime_task(int arg)
{
    int n = 2, count = 0;

    while (1) {
        if (prime(n)) {
            count++;
            printf("prime(%d) = %d\n", count, n);
        }
        n++;
    }
}

void init(void)
{
    serial_init();
    timer_init();
    start("Heart", heart_task, 0, STACK);
    start("Prime", prime_task, 0, STACK);
}
