/* x16-sync/order.c */
/* Copyright (c) 2018-20 J. M. Spivey */

#include "microbian.h"
#include "lib.h"
#include <string.h>

int MAY, FARAGE, HUMPHRYS;

#define SPEAK 23

char *slogan[] = {
    "no deal is better than a bad deal",
    "BREXIT MEANS BREXIT!"
};

void host(int n)
{
    int speaker;
    char *line;
    message m;

    while (1) {
        receive(SPEAK, &m);
        speaker = m.sender;
        line = m.ptr1;
        printf("%s\n", line);
        send_msg(speaker, REPLY);
    }
}

void guest(int n)
{
    message m;

    while (1) {
        m.type = SPEAK;
        m.ptr1 = slogan[n];
        sendrec(HUMPHRYS, &m);
    }
}

void init(void)
{
    serial_init();
    MAY = start("May", guest, 0, STACK);
    FARAGE = start("Farage", guest, 1, STACK);
    HUMPHRYS = start("Humphrys", host, 0, STACK);
}
