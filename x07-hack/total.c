/* x07-hack/total.c */
/* Copyright (c) 2021 J. M. Spivey */

#include "lib.h"
#include "hardware.h"

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
    UART.STARTTX = 1;
    UART.STARTRX = 1;
    UART.RXDRDY = 0;
    txinit = 1;
}

/* serial_getc -- wait for input character and return it */
int serial_getc(void)
{
    while (! UART.RXDRDY) { }
    char ch = UART.RXD;
    UART.RXDRDY = 0;
    return ch;
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

/* serial_puts -- send a string character by character */
void serial_puts(const char *s)
{
    while (*s != '\0')
        serial_putc(*s++);
}

/* getline -- input a line of text with line editing */
void getline(char *buf)
{
    char *p = buf;

    serial_puts("> ");

    while (1) {
        char x = serial_getc();

        switch (x) {
        case '\b':
        case 0177:
            if (p > buf) {
                p--;
                serial_puts("\b \b");
            }
            break;

        case '\r':
        case '\n':
            *p = '\0';
            serial_puts("\r\n");
            return;

        default:
            /* Ignore other non-printing characters */
            if (x >= 040 && x < 0177) {
                *p++ = x;
                serial_putc(x);
            }
        }
    }
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

/* getnum -- read a line of input and convert to a number */
int getnum(void)
{
    char buf[64];
    getline(buf);
    return atoi(buf);
}

void init(void)
{
    int n = 0, total;
    int data[10];

    serial_init();

    printf("Enter numbers, 0 to finish\n");
    while (1) {
        int x = getnum();
        if (x == 0) break;
        data[n++] = x;
    }

    total = 0;
    for (int i = 0; i < n; i++)
        total += data[i];
    
    printf("Total = %d\n", total);    
}   
