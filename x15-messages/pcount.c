/* x15-messages/pcount.c */
/* Copyright (c) 2018-20 J. M. Spivey */

#include "microbian.h"
#include "lib.h"
#include <string.h>

int PRODUCER, CONSUMER;

#define PRIME 42

/* prime -- test for primality */
int prime(int n)
{
    for (int k = 2; k * k <= n; k++) {
        if (n % k == 0)
            return 0;
    }

    return 1;
}

/* producer -- process that sends primes to consumer */
void producer(int arg)
{
    int n = 2;
    message m1;

    while (1) {
        if (prime(n)) {
            m1.type = PRIME;
            m1.int1 = n;
            send(CONSUMER, &m1);
        }
        n++;
    }
}

/* consumer -- receive primes and count them */
void consumer(int arg)
{
    int count = 0, limit = 1000;
    message m2;

    while (1) {
        receive(PRIME, &m2);
        while (m2.int1 >= limit) {
            printf("There are %d primes less than %d\n", count, limit);
            limit += 1000;
        }
        count++;
    }
}

void init(void)
{
    serial_init();
    PRODUCER = start("Produce", producer, 0, STACK);
    CONSUMER = start("Consume", consumer, 0, STACK);
}
