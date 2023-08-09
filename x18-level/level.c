/* x18-level/level.c */
/* Copyright (c) 2019 J. M. Spivey */

#include "microbian.h"
#include "hardware.h"
#include "lib.h"
#include "accel.h"

/* light -- show one pixel */
void light(int x, int y)
{
    image screen;
    image_clear(screen);
    image_set(x, y, screen);
    display_show(screen);
}

/* scale -- map acceleration to coordinate */
static int scale(int x)
{
    if (x < -20) return 4;
    if (x < -10) return 3;
    if (x <= 10) return 2;
    if (x <= 20) return 1;
    return 0;
}

static void i2c_map(void)
{
    static char *hex = "0123456789abcdef";

    printf("I2C bus map:\n");
    printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n");
    for (int a = 0; a < 8; a++) {
        printf("%c0:", hex[a]);
        for (int b = 0; b < 16; b++) {
            int addr = (a<<4) + b;
            if (addr < 0x08 || addr >= 0x78)
                /* Reserved addresses */
                printf("   ");
            else {
                int status = i2c_probe(I2C_INTERNAL, addr);
                if (status == OK)
                    printf(" %c%c", hex[a], hex[b]);
                else
                    printf(" --");
            }
        }
        printf("\n");
    }
}

/* main_task -- show the spirit level */
static void main(int n)
{
    int x, y, z;

    printf("Hello\n\n");
    i2c_map();
    printf("\n");
    timer_delay(1000);
    accel_start();

    while (1) {
        timer_delay(200);
        accel_reading(&x, &y, &z);
        printf("x=%d y=%d z=%d\n", x, y, z);
        x = scale(x); y = scale(y);
        light(x, y);
    }
}

void init(void)
{
    serial_init();
    timer_init();
    i2c_init(I2C_INTERNAL);
    display_init();
    start("Main", main, 0, STACK);
}
