/* x07-hack/squirt.c */
/* Copyright (c) J. M. Spivey 2020 */

/* Squirt a text file at the micro:bit */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define DEVICE "/dev/ttyACM0"   /* TTY device for the micro:bit */

int main(int argc, char **argv)
{
    FILE *fin, *fout;

    if (argc != 2) {
        fprintf(stderr, "Usage: squirt file\n");
        exit(2);
    }

    fin = fopen(argv[1], "r");
    if (fin == NULL) {
        fprintf(stderr, "squirt: couldn't open %s for reading\n", argv[1]);
        exit(1);
    }

    fout = fopen(DEVICE, "wb");
    if (fout == NULL) {
        fprintf(stderr, "squirt: couldn't open %s for writing\n", DEVICE);
        exit(1);
    }

    while (1) {
        int ch = fgetc(fin);

        if (ch == EOF) break;

        if (ch == '\n')
            fputc('\r', fout);
        else
            fputc(ch, fout);

        fflush(fout);
        usleep(10000);
    }
}


       
        
