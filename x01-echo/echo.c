/* x01-echo/echo.c */
/* Copyright (c) 2018 J. M. Spivey */

#include "hardware.h"

void serial_init(void);
void serial_puts(const char *s);
void serial_getline(const char *prompt, char *buf, int len);

#define NBUF 80

char linebuf[NBUF];

void init(void)
{
    serial_init();
    serial_puts("\r\nHello micro:world!\r\n");

    while (1) {
        serial_getline("> ", linebuf, NBUF);
        serial_puts("--> ");
        serial_puts(linebuf);
        serial_puts("\r\n");
    }
}

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

/* serial_getline -- input a line of text into buf with line editing */
void serial_getline(const char *prompt, char *buf, int nbuf)
{
    char *p = buf;

    serial_puts(prompt);

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
            *p = '\0';
            serial_puts("\r\n");
            return;

        default:
            /* Ignore other non-printing characters */
            if (x >= 040 && x < 0177 && p < &buf[nbuf]) {
                *p++ = x;
                serial_putc(x);
            }
        }
    }
}



