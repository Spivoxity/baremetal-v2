/* x16-sync/order.c */
/* Copyright (c) 2018-20 J. M. Spivey */

#include "microbian.h"
#include "lib.h"
#include <string.h>

int MAY, FARAGE;

char *slogan[] = {
    "no deal is better than a bad deal",
    "BREXIT MEANS BREXIT!"
};

void guest(int n)
{
    while (1) {
        printf("%s\n", slogan[n]);
    }
}

void init(void)
{
    serial_init();
    MAY = start("May", guest, 0, STACK);
    FARAGE = start("Farage", guest, 1, STACK);
}
