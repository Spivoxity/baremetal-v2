/* timer.c */
/* Copyright (c) 2018-2020 J. M. Spivey */

#include "microbian.h"
#include "hardware.h"

static int TIMER_TASK;

#define DELAY 33
#define PULSE 34

#define TICK 5                  /* Interval between ticks (ms) */

#define MAX_TIMERS 8

static int max = 0;             /* Max timers actually in use */

/* Millis will overflow in about 46 days, but that's long enough. */

/* millis -- milliseconds since boot */
static unsigned millis = 0;

/* timer -- array of data for pending timer messages */
static struct {
    int client;      /* Process that receives message, or -1 if empty */
    int msgtype;     /* Message to send */
    unsigned next;   /* Next time to send a message */
    unsigned period; /* Interval between messages if msgtype = PING */
} timer[MAX_TIMERS];

/* check_timers is called by the timer task and sends messages
   directly to clients.  We assume that each client is waiting to
   receive the message: otherwise the progress of the entire system
   will be held up, possibly leading to deadlock. */

/* check_timers -- send any messages that are due */
static void check_timers(void)
{
    int i;

    for (i = 0; i < max; i++) {
        if (timer[i].client >= 0 && millis >= timer[i].next) {
            send_int(timer[i].client, timer[i].msgtype, timer[i].next);
            timer[i].next += timer[i].period;
            if (timer[i].msgtype != PING)
                timer[i].client = -1;
        }
    }
}

/* create -- create a new timer */
static void create(int client, int msgtype, int delay)
{
    int i = 0;

    while (i < MAX_TIMERS && timer[i].client >= 0)
        i++;

    if (i == MAX_TIMERS)
        panic("Too many timers");

    if (i == max) max++;

    /* If we are between ticks when the timer is created, then the
       timer will go off up to one tick early.  We could add on a tick
       to compensate for this, but most applications work better
       without it.  Effectively, the delay is counted from the
       previous timer tick, and if it is created as a response to that
       tick, then the effect is what is usually wanted. */

    timer[i].client = client;
    timer[i].msgtype = msgtype;
    timer[i].next = millis + delay;
    timer[i].period = delay;
}

/* timer1_handler -- interrupt handler */
void timer1_handler(void)
{
    /* Update the time here so it is accessible to timer_micros */
    if (TIMER1.COMPARE[0]) {
        millis += TICK;
        TIMER1.COMPARE[0] = 0;
        interrupt(TIMER_TASK);
    }
}

static void timer_task(int n)
{
    message m;

    /* We use Timer 1 because its 16-bit mode is adequate for a clock
       with up to 1us resolution and 1ms period, leaving the 32-bit
       Timer 0 for other purposes. */
    TIMER1.STOP = 1;
    TIMER1.MODE = TIMER_MODE_Timer;
    TIMER1.BITMODE = TIMER_BITMODE_16Bit;
    TIMER1.PRESCALER = 4;      /* 1MHz = 16MHz / 2^4 */
    TIMER1.CLEAR = 1;
    TIMER1.CC[0] = 1000 * TICK;
    TIMER1.SHORTS = BIT(TIMER_COMPARE0_CLEAR);
    TIMER1.INTENSET = BIT(TIMER_INT_COMPARE0);
    TIMER1.START = 1;
    enable_irq(TIMER1_IRQ);
    priority(P_HANDLER);

    while (1) {
        receive(ANY, &m);

        switch (m.type) {
        case INTERRUPT:
            check_timers();
            break;

        case DELAY:
            create(m.sender, REPLY, m.int1);
            break;

        case PULSE:
            create(m.sender, PING, m.int1);
            break;

        default:
            badmesg(m.type);
        }
    }
}

/* timer_init -- start the timer task */
void timer_init(void)
{
    int i;

    for (i = 0; i < MAX_TIMERS; i++)
        timer[i].client = -1;

    TIMER_TASK = start("Timer", timer_task, 0, 256);
}

/* timer_now -- return current time in milliseconds since startup */
unsigned timer_now(void)
{
    return millis;
}

/* The result of timer_micros will overflow after 71 minutes, but even
if it does overflow, shorter durations can be measured by taking the
difference of two readings with unsigned subtraction. */

/* timer_micros -- return microseconds since startup */
unsigned timer_micros(void)
{
    unsigned my_millis, ticks1, ticks2, extra;
    
    /* The timer resets itself when it reaches the limit, then
       requests an interupt, and the interrupt handler increments the
       value of millis.  So we must allow for the possibility that the
       timer has reset itself but the interrupt has not yet been
       handled. Worse, the timer expiry could happen between looking
       at the timer and looking at the interrupt flag.  Our approach
       is to take two readings with interrupts disabled, one before
       and one after checking the flag.  If the flag is set, but the
       value has not gone down between the two readings, that
       indicates the reset after disabling interrupts but before the
       first reading, so an extra tick should be added. */

    intr_disable();
    TIMER1.CAPTURE[1] = 1;      /* Capture count before testing event */
    extra = TIMER1.COMPARE[0];  /* Inspect the expiry event */
    TIMER1.CAPTURE[2] = 1;      /* Capture count afterwards */
    ticks1 = TIMER1.CC[1];
    ticks2 = TIMER1.CC[2];
    my_millis = millis;
    intr_enable();

    /* Correct my_millis if the timer expired */
    if (extra && ticks1 <= ticks2) my_millis += TICK;

    return 1000 * my_millis + ticks1;
}

/* timer_delay -- one-shot delay */
void timer_delay(int msec)
{
    message m;
    m.type = DELAY;
    m.int1 = msec;
    sendrec(TIMER_TASK, &m);
}

/* timer_pulse -- regular pulse */
void timer_pulse(int msec)
{
    send_int(TIMER_TASK, PULSE, msec);
}

/* timer_wait -- sleep until next timer pulse */
void timer_wait(void)
{
    receive_msg(PING);
}
