/* x33-clock/clock.c */
/* Copyright (c) 2020 J. M. Spivey */

#include "microbian.h"
#include "lib.h"
#include "hardware.h"
#include "string.h"

#define RTC_ADDR 0x6f
#define RTC_CONTROL 0x07
#define RTC_SEC 0x00
#define RTC_MIN 0x01
#define RTC_HOUR 0x02
#define RTC_WKDAY 0x03

/* neoframe -- output a neopixel string */
void neoframe(unsigned pin, unsigned *buf, int n);

/* rgb -- assemble a colour from RGB components */
unsigned rgb(unsigned r, unsigned g, unsigned b)
{
    /* Note the awkward GRB colour order of Neopixels */
    return ((g & 0xff) << 16) | ((r & 0xff) << 8) | (b & 0xff);
}

unsigned bcd_decode(unsigned val)
{
    return 10 * (val >> 4) + (val & 0xf);
}

void main_task(int arg)
{
    int secs, mins, hours, weekday;
    byte buf[3];
    unsigned frame[60];

    /* Disable external oscillator */
    i2c_write_reg(I2C_EXTERNAL, RTC_ADDR, RTC_CONTROL, 0x00);

    /* Start the clock oscillator */
    secs = i2c_read_reg(I2C_EXTERNAL, RTC_ADDR, RTC_SEC) & 0x7f;
    i2c_write_reg(I2C_EXTERNAL, RTC_ADDR, RTC_SEC, secs | 0x80);

    /* Enable backup battery */
    weekday = i2c_read_reg(I2C_EXTERNAL, RTC_ADDR, RTC_WKDAY) & 0x07;
    i2c_write_reg(I2C_EXTERNAL, RTC_ADDR, RTC_WKDAY, weekday | 0x08);

    gpio_dir(PAD8, 1);

    timer_pulse(100);

    while (1) {
        receive_msg(PING);

        i2c_read_bytes(I2C_EXTERNAL, RTC_ADDR, 0, buf, 3);
        secs = bcd_decode(buf[RTC_SEC] & 0x7f);
        mins = bcd_decode(buf[RTC_MIN] & 0x7f);
        hours = bcd_decode(buf[RTC_HOUR] & 0x3f);
        if (hours >= 12) hours -= 12;

        printf("%d %d %d\n", hours, mins, secs);

        memset(frame, 0, sizeof(frame));
        frame[5*hours] = rgb(0, 0, 20);
        frame[(5*hours+59)%60] = 
            frame[5*hours+1] = rgb(0, 0, 10);
        frame[mins] = rgb(0, 20, 0);
        frame[secs] = rgb(20, 0, 0);
        intr_disable();
        neoframe(PAD8, frame, 60);
        intr_enable();
    }
}

void init(void)
{
    timer_init();
    i2c_init(I2C_EXTERNAL);
    serial_init();
    start("Main", main_task, 0, STACK);
}
