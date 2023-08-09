/* x32-infrared/decode.c *
/* Copyright (c) 2020 J. M. Spivey */

#include "microbian.h"
#include "lib.h"
#include "hardware.h"

#define IR_PIN PAD16
#define CHAN 2

#define IDLE 0
#define START 1
#define PAUSE 2
#define MARK 3
#define SPACE 4
#define STOP 5

static int client = -1;
static unsigned command;

static void reply(void)
{
    message m;

    if (client < 0) return;
    m.int1 = command;
    send(client, REPLY, &m);
    client = -1;
}
 
/* range -- test whether value is within tolerance of a midpoint */
static int range(int x, int mid, int tol)
{
    return (x >= mid-tol && x <= mid+tol);
}

static void ir_edge(unsigned t)
{
    unsigned val = gpio_in(IR_PIN);
    static int state = IDLE;
    static unsigned prev = 1, tprev = 0, tpulse;
    static int nbytes, nbits;

    if (val == prev) return;

    tpulse = t - tprev;

    switch (state) {
    case IDLE:
        if (val == 0) {
            state = START;
            nbytes = nbits = 0;
            command = 0;
        }
        break;

    case START:
        if (range(tpulse, 9000, 100))
            state = PAUSE;
        else
            state = IDLE;
        break;

    case PAUSE:
        if (range(tpulse, 4500, 100))
            state = MARK;
        else
            state = IDLE;
        break;

    case MARK:
        if (range(tpulse, 562, 100))
            state = SPACE;
        else
            state = IDLE;
        break;

    case SPACE:
        if (range(tpulse, 1687, 100))
            command |= BIT(8*(3-nbytes) + nbits);
        else if (! range(tpulse, 562, 100)) {
            state = IDLE;
            break;
        }

        if (++nbits == 8) {
            nbytes++; nbits = 0;
        }

        if (nbytes == 4)
            state = STOP;
        else
            state = MARK;
        break;

    case STOP:
        if (range(tpulse, 562, 100))
            reply();

        state = IDLE;
        break;

    default:
        panic("bad state");
    }

    prev = val; tprev = t;
}

static int IR_TASK;
static volatile unsigned intrtime;

void gpiote_handler(void)
{
    intrtime = timer_micros();
    disable_irq(GPIOTE_IRQ);
    interrupt(IR_TASK);
}

static void ir_task(int arg)
{
    message m;

    gpio_connect(IR_PIN);
    GPIOTE.CONFIG[CHAN] =
        FIELD(GPIOTE_CONFIG_MODE, GPIOTE_MODE_Event)
        | FIELD(GPIOTE_CONFIG_PSEL, IR_PIN)
        | FIELD(GPIOTE_CONFIG_POLARITY, GPIOTE_POLARITY_Toggle);
    GPIOTE.INTENSET = BIT(CHAN);
    connect(GPIOTE_IRQ);
    enable_irq(GPIOTE_IRQ);

    printf("Ready!\n");

    while (1) {
        receive(ANY, &m);
        switch (m.type) {
        case INTERRUPT:
            if (GPIOTE.IN[CHAN]) {
                ir_edge(intrtime);
                GPIOTE.IN[CHAN] = 0;
                clear_pending(GPIOTE_IRQ);
                enable_irq(GPIOTE_IRQ);
            }
            break;

        case READ:
            client = m.sender;
            break;

        default:
            badmesg(m.type);
        }
    }
}

void main_task(int arg)
{
    unsigned cmd;
    message m;

    while (1) {
        sendrec(IR_TASK, READ, &m);
        cmd = m.int1;
        if (GET_BYTE(cmd, 2) != (GET_BYTE(cmd, 3) ^ 0xff))
            printf("wrong address!\n");
        else if (GET_BYTE(cmd, 0) != (GET_BYTE(cmd, 1) ^ 0xff))
            printf("wrong command!\n");
        else {
            char ch = '\0';
            switch (GET_BYTE(cmd, 1)) {
            case 0x19: ch = '0'; break;
            case 0x45: ch = '1'; break;
            case 0x46: ch = '2'; break;
            case 0x47: ch = '3'; break;
            case 0x44: ch = '4'; break;
            case 0x40: ch = '5'; break;
            case 0x43: ch = '6'; break;
            case 0x07: ch = '7'; break;
            case 0x15: ch = '8'; break;
            case 0x09: ch = '9'; break;
            case 0x16: ch = '*'; break;
            case 0x0d: ch = '#'; break;
            case 0x18: ch = 'U'; break;
            case 0x08: ch = 'L'; break;
            case 0x1c: ch = 'K'; break;
            case 0x5a: ch = 'R'; break;
            case 0x52: ch = 'D'; break;
            default: ch = '?';
            }
            printf("%c", ch);
        }
    }
}

void init(void)
{
    timer_init();
    serial_init();
    IR_TASK = start("IR", ir_task, 0, STACK);
    start("Main", main_task, 0, STACK);
}
