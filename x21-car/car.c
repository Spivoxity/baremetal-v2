/* x21-car/car.c */
/* Copyright (c) 2021 J. M. Spivey */

#include "microbian.h"
#include "hardware.h"
#include "pwm.h"

int ADAPTER, CONTROL;

/* RADIO ADAPTER PROCESS */

/* Message type for remote command */
#define CMD 24

/* adapter_task -- poll the radio and sends commands to CONTROL */
void adapter_task(int dummy)
{
    int n;
    byte buf[32];

    while (1) {
        n = radio_receive(buf);
        if (n >= 2 && buf[0] == '@')
            send_int(CONTROL, CMD, buf[1]);
    }
}


/* CONTROL PROCESS */

/* The controller waits for commands over the radio, and responds to a
driving command -- forward, left or right -- by starting the motors
and running them for a fixed time.  If further commands arrive, the
counter quantum is reset, and motion continues.  If there are no
further commands, motion stops when the qunatum expires.  This scheme
helps to keep the motion smooth when occasional radio messages are
lost. */

#define REST 1500               /* PWM setting for stopped motor */
#define SPEED 300               /* PWM increment for forward motion */
#define TURN 150                /* PWM incerement for turning */

static int quantum = 0;         /* Time left before stopping */
static int lspeed = 0, rspeed = 0; /* Speeds for left and right wheel */

/* drive -- start car moving */
void drive(int lsp, int rsp, int t)
{
    if (lsp != lspeed || rsp != rspeed) {
        lspeed = lsp; rspeed = rsp;
        pwm_change(REST-lspeed, REST+rspeed);
    }
    quantum = t;
}

/* stop -- stop the car */
void stop(void)
{
    /* We disconnect the motors to avoid the problem of creep */
    lspeed = rspeed = 0;
    pwm_change(0, 0);
}

/* control_task -- monitor and respond to commands */
void control_task(int dummy)
{
    message m;
    char cmd;

    stop();
    timer_pulse(20);

    while (1) {
        receive(ANY, &m);
        switch (m.type) {
        case PING:
            /* Note passage of time */
            if (quantum > 0) {
                quantum--;
                if (quantum == 0) stop();
            }
            break;

        case CMD:
            /* Respond to radio command */
            cmd = m.int1;
            if (cmd == 'B')
                drive(SPEED, SPEED, 25);
            else if (cmd == 'L')
                drive(TURN, -TURN, 10);
            else if (cmd == 'R')
                drive(-TURN, TURN, 10);
            break;

        default:
            panic("Controller got bad command");
        }
    }
}

void init(void)
{
    timer_init();
    radio_init();
    pwm_init();
    ADAPTER = start("Adapter", adapter_task, 0, STACK);
    CONTROL = start("Control", control_task, 0, STACK);
}
