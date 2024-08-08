/* x10-serial/primes.c */
/* Copyright (c) 2018 J. M. Spivey */

#include "hardware.h"
#include "lib.h"

/* Pins to use for serial communication */
#define TX USB_TX
#define RX USB_RX

int txinit;              /* UART ready to transmit first char */

/* serial_init -- set up UART connection to host */
void serial_init(void)
{
    UART.ENABLE = UART_ENABLE_Disabled;
    UART.BAUDRATE = UART_BAUDRATE_9600; /* 9600 baud */
    UART.CONFIG = FIELD(UART_CONFIG_PARITY, UART_PARITY_None);
                                        /* format 8N1 */
    UART.PSELTXD = TX;                  /* choose pins */
    UART.PSELRXD = RX;
    UART.ENABLE = UART_ENABLE_Enabled;
    UART.TXDRDY = 0;
    UART.STARTTX = 1;
    txinit = 1;
}

/* serial_putc -- send output character */
void serial_putc(char ch)
{
    if (! txinit) {
        while (! UART.TXDRDY) { }
    }
    txinit = 0;
    UART.TXDRDY = 0;
    UART.TXD = ch;
}

/* print_buf -- output routine for use by printf */
void print_buf(char *buf, int n)
{
    for (int i = 0; i < n; i++) {
        char c = buf[i];
        if (c == '\n') serial_putc('\r');
        serial_putc(c);
    }
}

/* modulo -- (very slow) remainder operation */
int modulo(int a, int b)
{
    int r = a;
    while (r >= b) r -= b;
    return r;
}

/* prime -- test if an integer is prime */
int prime(int n)
{
    for (int k = 2; k * k <= n; k++) {
        if (modulo(n, k) == 0)
            return 0;
    }

    return 1;
}

/* start_timer -- light an LED and start a timer */
void start_timer(void)
{
    led_dot();

    TIMER0.MODE = TIMER_MODE_Timer;
    TIMER0.BITMODE = TIMER_BITMODE_32Bit;
    TIMER0.PRESCALER = 4; /* Count at 1MHz */
    TIMER0.START = 1;
}

/* stop_timer -- turn off LED and print timer result */
void stop_timer(void)
{
    led_off();

    TIMER0.CAPTURE[0] = 1;
    unsigned time1 = TIMER0.CC[0];
    printf("%d millisec\n", (time1+500)/1000);
}

void init(void)
{
    int n = 2, count = 0;

    led_init();
    serial_init();
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
