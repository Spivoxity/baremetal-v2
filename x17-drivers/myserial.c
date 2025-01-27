/* x17-drivers/myserial.c */
/* Copyright (c) 2020 J. M. Spivey */

#include "microbian.h"
#include "hardware.h"

#define TX USB_TX
#define RX USB_RX

static int SERIAL_TASK;

/* Message types for serial task */
#define PUTC 16

/* NBUF -- size of output buffer.  Should be a power of 2. */
#define NBUF 128

/* Output buffer */
static char txbuf[NBUF];        /* Circular buffer for output */
static int bufin = 0;           /* In pointer */
static int bufout = 0;          /* Out pointer */
static int n_tx = 0;            /* Character count */

static int txidle = 1;          /* True if transmitter is idle */

/* The clear_pending() call below is needed because the UART interrupt
   handler disables the IRQ for the UART in the NVIC, but doesn't
   disable the UART itself from sending interrupts.  The pending bit
   is cleared on return from the interrupt handler, but that doesn't
   stop the UART from setting it again. */

/* serial_interrupt -- handle serial interrupt */
static void serial_interrupt(void)
{
    if (UART.TXDRDY) {
        txidle = 1;
        UART.TXDRDY = 0;
    }

    clear_pending(UART_IRQ);
    enable_irq(UART_IRQ);
}

/* serial_starttx -- start transmitter if possible */
static void serial_starttx(void)
{
    /* Can we start transmitting a character? */
    if (txidle && n_tx > 0) {
        UART.TXD = txbuf[bufout];
        bufout = (bufout+1) % NBUF;
        n_tx--;
        txidle = 0;
    }
}

/* serial_enqueue -- add character to output queue */
static void serial_enqueue(char ch)
{
    while (n_tx == NBUF) {
        /* The buffer is full -- wait for a space to appear */
        receive_msg(INTERRUPT);
        serial_interrupt();
        serial_starttx();
    }

    txbuf[bufin] = ch;
    bufin = (bufin+1) % NBUF;
    n_tx++;
}

/* serial_task -- driver process for UART */
static void serial_task(int arg)
{
    message m;
    char ch;

    UART.ENABLE = UART_ENABLE_Disabled;
    UART.BAUDRATE = UART_BAUDRATE_9600; /* 9600 baud */
    UART.CONFIG = FIELD(UART_CONFIG_PARITY, UART_PARITY_None);
                                        /* format 8N1 */
    UART.PSELTXD = TX;                  /* choose pins */
    UART.PSELRXD = RX;
    UART.ENABLE = UART_ENABLE_Enabled;
    UART.TXDRDY = 0;
    UART.STARTTX = 1;
    UART.INTENSET = BIT(UART_INT_TXDRDY);
    connect(UART_IRQ);
    enable_irq(UART_IRQ);
    txidle = 1;

    while (1) {
        receive(ANY, &m);

        switch (m.type) {
        case INTERRUPT:
            serial_interrupt();
            break;

        case PUTC:
            ch = m.int1;
            serial_enqueue(ch);
            break;

        default:
            badmesg(m.type);
        }
          
        serial_starttx();
    }
}

/* myserial_init -- start the serial driver task */
void myserial_init(void)
{
    SERIAL_TASK = start("Serial", serial_task, 0, 256);
}

/* myserial_putc -- queue a character for output */
void myserial_putc(char ch)
{
    send_int(SERIAL_TASK, PUTC, ch);
}

/* print_buf -- output routine for use by printf */
void print_buf(char *buf, int n)
{
    for (int i = 0; i < n; i++) {
        char c = buf[i];
        if (c == '\n') myserial_putc('\r');
        myserial_putc(c);
    }
}
