/* x13-neopixels/rainbow.c */
/* Copyright (c) 2019 J. M. Spivey */

#include "hardware.h"

#define NPIX 12                 /* Number of pixels in string */
#define STEP 2                  /* Colour step between pixels */
#define NEO PAD0                /* Pin for output signal */

/* neoframe -- output a neopixel string */
void neoframe(unsigned pin, unsigned *buf, int n);

/* rgb -- assemble a colour from RGB components */
unsigned rgb(unsigned r, unsigned g, unsigned b)
{
    /* Note the awkward GRB colour order of Neopixels */
    return ((g & 0xff) << 16) | ((r & 0xff) << 8) | (b & 0xff);
}

/* getR, getG, get B -- access R, G, B components of colour */
unsigned getR(unsigned rgb) { return (rgb >> 8) & 0xff; }
unsigned getG(unsigned rgb) { return (rgb >> 16) & 0xff; }
unsigned getB(unsigned rgb) { return rgb & 0xff; }

#define ISTEP 32                /* Number of steps between two colours */

/* interp -- interpolate between two colours */
unsigned interp(int i, unsigned x, unsigned y)
{
    int r = (ISTEP-i) * getR(x) + i * getR(y);
    int g = (ISTEP-i) * getG(x) + i * getG(y);
    int b = (ISTEP-i) * getB(x) + i * getB(y);
    return rgb(r>>5, g>>5, b>>5);
}

#define INTEN 31

/* hue -- find colour on cyclic colour triangle */
unsigned hue(int i)
{
    int j = i & 0x1f;

    switch ((i >> 5) % 3) {
    case 0:
        /* Colours 0 to 31, 96 to 127, ... from red to green */
        return interp(j, rgb(INTEN, 0, 0), rgb(0, INTEN, 0));
    case 1:
        /* Colours 32 to 63, 128 to 159, ... from green to blue */
        return interp(j, rgb(0, INTEN, 0), rgb(0, 0, INTEN));
    case 2:
        /* Colours 64 to 95, 160 to 191, ... from blue to red */
        return interp(j, rgb(0, 0, INTEN), rgb(INTEN, 0, 0));
    default:
        return 0;
    }
}

/* pix -- buffer for a frame of pixels */
unsigned pix[NPIX];

void init(void)
{
    int u = 0;

    /* Set up pin NEO for output */
    gpio_dir(NEO, 1);
    gpio_out(NEO, 0);

    /* Initialise to all pixels off */
    neoframe(NEO, pix, NPIX);

    while (1) {
        /* Compute shifted rainbow pattern */
        for (int i = 0; i < NPIX; i++)
            pix[i] = hue(u + STEP * i);

        /* Display it */
        neoframe(NEO, pix, NPIX);

        /* Shift next frame a bit further */
        u++;

        /* Delay before next frame */
        delay_loop(100000);
    }
}
