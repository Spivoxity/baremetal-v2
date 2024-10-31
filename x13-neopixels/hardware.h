/* common/hardware.h */
/* Copyright (c) 2018-20 J. M. Spivey */

#define UBIT 1
#define UBIT_V2 1

/* Hardware register definitions for nRF52833 */

#define BIT(i) (1 << (i))
#define SET_BIT(r, n) r |= BIT(n)
#define GET_BIT(r, n) (((r) >> (n)) & 0x1)
#define CLR_BIT(r, n) r &= ~BIT(n)
#define GET_BYTE(r, n) (((r) >> (8*(n))) & 0xff)
#define SET_BYTE(r, n, v) \
    r = (r & ~(0xff << 8*n)) | ((v & 0xff) << 8*n)

/* The macros SET_FIELD, etc., are defined in an indirect way that
permits (because of the timing of CPP macro expansion) the 'field'
argument to be a macro that expands the a 'position, width' pair. */

#define SET_FIELD(r, field, val) _SET_FIELD(r, field, val)
#define _SET_FIELD(r, pos, wd, val) \
    r = (r & ~_MASK(pos, wd)) | _FIELD(pos, wd, val)

#define GET_FIELD(r, field) _GET_FIELD(r, field)
#define _GET_FIELD(r, pos, wd)  ((r >> pos) & _MASK0(wd))

#define FIELD(field, val) _FIELD(field, val)
#define _FIELD(pos, wd, val)  (((val) & _MASK0(wd)) << pos)

#define MASK(field) _MASK(field)
#define _MASK(pos, wd)  (_MASK0(wd) << pos)

#define _MASK0(wd)  (~((-2) << (wd-1)))

#define __BIT(pos) pos
#define __FIELD(pos, wd) pos, wd


/* Device pins as encoded (port, pin) pairs */ 
#define DEVPIN(p, i) ((p<<5) + i)
#define PORT(x) ((x)>>5)
#define PIN(x) ((x)&0x1f)

#define PAD14 DEVPIN(0, 1) 
#define PAD0  DEVPIN(0, 2)
#define PAD1  DEVPIN(0, 3)
#define PAD2  DEVPIN(0, 4)
#define PAD9  DEVPIN(0, 9)
#define PAD8  DEVPIN(0, 10)
#define PAD7  DEVPIN(0, 11)
#define PAD12 DEVPIN(0, 12)
#define PAD15 DEVPIN(0, 13)
#define PAD5  DEVPIN(0, 14)
#define ROW3  DEVPIN(0, 15)
#define PAD13 DEVPIN(0, 17)
#define ROW5  DEVPIN(0, 19)
#define ROW1  DEVPIN(0, 21)
#define ROW2  DEVPIN(0, 22)
#define PAD11 DEVPIN(0, 23)
#define ROW4  DEVPIN(0, 24)
#define PAD19 DEVPIN(0, 26)
#define PAD4  DEVPIN(0, 28)
#define PAD10 DEVPIN(0, 30)
#define PAD3  DEVPIN(0, 31)

#define PAD20 DEVPIN(1, 0)
#define PAD16 DEVPIN(1, 2)
#define PAD6  DEVPIN(1, 5)

#define COL1 PAD4
#define COL2 PAD7
#define COL3 PAD3
#define COL4 PAD6
#define COL5 PAD10

#define USB_TX DEVPIN(0, 6)
#define USB_RX DEVPIN(1, 8)

#define BUTTON_A PAD5
#define BUTTON_B PAD11

#define I2C0_SDA DEVPIN(0, 16)
#define I2C0_SCL DEVPIN(0, 8)
#define I2C1_SCL PAD19
#define I2C1_SDA PAD20

#define SPI_SCK PAD13
#define SPI_MISO PAD14
#define SPI_MOSI PAD15

/* Two I2C busses, one for internal chips (I2C0_SCL, I2C0_SDA),
   another external (I2C1_SCL, I2C1_SDA); use SPI2 for SPI */
#define N_I2C 2
#define I2C_INTERNAL 0
#define I2C_EXTERNAL 1
#define SPI_CHAN 2


/* Interrupts */
#define SVC_IRQ    -5
#define PENDSV_IRQ -2
#define SYSTICK_IRQ -1
#define RADIO_IRQ   1
#define UART0_IRQ   2
#define I2C0_IRQ    3
#define SPI0_IRQ    3
#define I2C1_IRQ    4
#define SPI1_IRQ    4
#define GPIOTE_IRQ  6
#define ADC_IRQ     7
#define TIMER0_IRQ  8
#define TIMER1_IRQ  9
#define TIMER2_IRQ 10
#define RTC0_IRQ   11
#define TEMP_IRQ   12
#define RNG_IRQ    13
#define RTC1_IRQ   17
#define TIMER3_IRQ 26
#define TIMER4_IRQ 27
#define PWM0_IRQ   28
#define PWM1_IRQ   33
#define PWM2_IRQ   34
#define SPI2_IRQ   35
#define UART1_IRQ  40
#define PWM3_IRQ   45
#define SPI3_IRQ   47

#define N_INTERRUPTS 64

/* For compatibility, we allow UART as a synonym for UART0 */
#define UART_IRQ UART0_IRQ
#define uart_handler uart0_handler

/* Interrupts 3 and 4 are shared between I2C and SPI: we can
define a handler with either name */
#define i2c0_handler i2c0_spi0_handler
#define spi0_handler i2c0_spi0_handler
#define i2c1_handler i2c1_spi1_handler
#define spi1_handler i2c1_spi1_handler


/* Device registers */
#define _PADDING(n) unsigned char _PAD(__LINE__)[n];
#define _PAD(lnum) _JOIN(_pad, lnum)
#define _JOIN(x, y) x##y


/* Common layout for DMA parameters */
typedef struct {
    void *PTR;
    unsigned MAXCNT;
    unsigned AMOUNT;
    unsigned LIST;      /* Used for SPIM */
} dma_param;


/* System contol block */
struct _scb {
    unsigned CPUID;                     // 0x00
    unsigned ICSR;                      // 0x04
#define   SCB_ICSR_PENDSVSET __BIT(28)
#define   SCB_ICSR_VECTACTIVE __FIELD(0, 8)
    _PADDING(4)
    unsigned AIRCR;                     // 0x0c
#define   SCB_AIRCR_VECTRESET __BIT(0)
#define   SCB_AIRCR_VECTCLRACTIVE __BIT(1)
#define   SCB_AIRCR_SYSRESETREQ __BIT(2)
#define   SCB_AIRCR_PRIGROUP __FIELD(8, 3)
#define   SCB_AIRCR_ENDIANESS __FIELD(15, 1)
#define   SCB_AIRCR_VECTKEY __FIELD(16, 16)
    unsigned SCR;                       // 0x10
#define   SCB_SCR_SLEEPONEXIT __BIT(1)
#define   SCB_SCR_SLEEPDEEP __BIT(2)
#define   SCB_SCR_SEVONPEND __BIT(4)
    _PADDING(4)
    unsigned SHPR[3];                   // 0x18
};

extern volatile struct _scb SCB;


/* Debug */
struct _debug {
    _PADDING(252)
    unsigned DEMCR;                     // 0xfc
#define   DEBUG_DEMCR_TRCENA __BIT(24)
};

extern volatile struct _debug DEBUG;


/* Nested vectored interupt controller */
struct _nvic {
    _PADDING(256)
    unsigned ISER[8];                   // 0x100
    _PADDING(96)
    unsigned ICER[8];                   // 0x180
    _PADDING(96)
    unsigned ISPR[8];                   // 0x200
    _PADDING(96)
    unsigned ICPR[8];                   // 0x280
    _PADDING(352)
    unsigned IPR[60];                   // 0x400
};

extern volatile struct _nvic NVIC;


/* Systick timer */
struct _systick {
    _PADDING(16)
    unsigned CSR;                       // 0x010
#define   SYSTICK_CSR_COUNTFLAG __BIT(16)
#define   SYSTICK_CSR_CLKSOURCE __FIELD(2, 1)
#define     SYSTICK_CLKSOURCE_External 0
#define     SYSTICK_CLKSOURCE_Internal 1
#define   SYSTICK_CSR_TICKINT __BIT(1)
#define   SYSTICK_CSR_ENABLE __BIT(0)
    unsigned RVR;                       // 0x014
    unsigned CVR;                       // 0x018
    unsigned CALIB;                     // 0x1c
#define   SYSTICK_CALIB_NOREF __BIT(31)
#define   SYSTICK_CALIB_SKEW __BIT(30)
#define   SYSTICK_CALIB_TENMS __FIELD(0, 24)
};

#define SYSTICK_CLOCK 64000000

extern volatile struct _systick SYSTICK;


/* Data watchpoint and trace */
struct _dwt {
    unsigned CTRL;                      // 0x000
#define   DWT_CTRL_CYCCNTENA __BIT(0)
    unsigned CYCCNT;                    // 0x004
};

extern volatile struct _dwt DWT;


/* Clock control */
struct _clock {
    unsigned HFCLKSTART;                // 0x000
    _PADDING(4)
    unsigned LFCLKSTART;                // 0x008
    _PADDING(244)
    unsigned HFCLKSTARTED;              // 0x100
    unsigned LFCLKSTARTED;              // 0x104
    _PADDING(1040)
    unsigned LFCLKSRC;                  // 0x518
#define   CLOCK_LFCLKSRC_RC 0
    _PADDING(52)
    unsigned XTALFREQ;                  // 0x550
#define   CLOCK_XTALFREQ_16MHz 0xFF
};

extern volatile struct _clock CLOCK;


/* Memory protection unit */
struct _mpu {
    _PADDING(1544)
    unsigned DISABLEINDEBUG;            // 0x608
};

extern volatile struct _mpu MPU;


/* Factory information */
struct _ficr {
    _PADDING(96)
    unsigned DEVICEID[2];               // 0x060
    _PADDING(60)
    unsigned DEVICEADDR[2];             // 0x0a4
    unsigned OVERRIDEEN;                // 0x0a0
#define   FICR_OVERRIDEEN_NRF __BIT(0)
    _PADDING(12)
    unsigned NRF_1MBIT[5];              // 0x0b0
};

extern volatile struct _ficr FICR;


/* Power management */
struct _power {
/* Tasks */
    _PADDING(120)
    unsigned CONSTLAT;                  // 0x078
    unsigned LOWPWR;                    // 0x07c
/* Events */
    _PADDING(136)
    unsigned POFWARN;                   // 0x108
/* Registers */
    _PADDING(504)
    unsigned INTENSET;                  // 0x304
    unsigned INTENCLR;                  // 0x308
    _PADDING(244)
    unsigned RESETREAS;                 // 0x400
#define   POWER_RESETREAS_RESETPIN __BIT(0)
#define   POWER_RESETREAS_DOG __BIT(1)
#define   POWER_RESETREAS_SREQ __BIT(2)
#define   POWER_RESETREAS_LOCKUP __BIT(3)
#define   POWER_RESETREAS_OFF __BIT(16)
#define   POWER_RESETREAS_LPCOMP __BIT(17)
#define   POWER_RESETREAS_DIF _BIT(18)
#define   POWER_RESETREAS_ALL 0x0007000f
    _PADDING(36)
    unsigned RAMSTATUS;                 // 0x428
    _PADDING(212)
    unsigned SYSTEMOFF;                 // 0x500
    _PADDING(12)
    unsigned POFCON;                    // 0x510
#define   POWER_POFCON_POF __BIT(1)
#define   POWER_POFCON_TRESHOLD _FIELD(1, 2)
#define     POWER_THRESHOLD_V21 0
#define     POWER_THRESHOLD_V23 1
#define     POWER_THRESHOLD_V25 2
#define     POWER_THRESHOLD_V27 3
    _PADDING(8)
    unsigned GPREGRET;                  // 0x51c
    unsigned GPREGRET2;                 // 0x520
    _PADDING(84)
    unsigned DCDCEN;                    // 0x578
};

/* Interrupts */
#define POWER_INT_POFWARN 2

extern volatile struct _power POWER;


/* Watchdog timer */
struct _wdt {
/* Tasks */
    unsigned START;                     // 0x000
/* Events */
    _PADDING(252)
    unsigned TIMEOUT;                   // 0x100
/* Registers */
    _PADDING(512)
    unsigned INTENSET;                  // 0x304
    unsigned INTENCLR;                  // 0x308
    _PADDING(244)
    unsigned RUNSTATUS;                 // 0x400
    unsigned REQSTATUS;                 // 0x404
    _PADDING(252)
    unsigned CRV;                       // 0x504
#define   WDT_HERTZ 32768
    unsigned RREN;                      // 0x508
    unsigned CONFIG;                    // 0x50c
#define   WDT_CONFIG_SLEEP __BIT(0)
#define   WDT_CONFIG_HALT __BIT(3)
    _PADDING(240)
    unsigned RR[8];                     // 0x600
#define   WDT_MAGIC 0x6e524635
};

/* Interrupts */
#define WDT_INT_TIMEOUT 0

extern volatile struct _wdt WDT;


/* Non-Volatile Memory Controller */
struct _nvmc {
    _PADDING(1024)
    unsigned READY;                     // 0x400
    _PADDING(256)
    unsigned CONFIG;                    // 0x504
#define   NVMC_CONFIG_Ronly 0
#define   NVMC_CONFIG_Wen 1
#define   NVMC_CONFIG_Een 2
    void *ERASEPAGE;                    // 0x508
    _PADDING(52)
    unsigned ICACHECONF;                // 0x540
#define   NVMC_ICACHECONF_CACHEEN __BIT(0)
};

extern volatile struct _nvmc NVMC;


/* UICR */
struct _uicr {
    _PADDING(20)
    unsigned NRFFW[13];                 // 0x014
    _PADDING(8)
    unsigned NRFHW[12];                 // 0x050
    unsigned CUSTOMER[31];              // 0x080
    _PADDING(260)
    unsigned PSELRESET[2];              // 0x200
    unsigned APPROTECT;                 // 0x208
    unsigned NFCPINS;                   // 0x20c
    unsigned DEBUGCTRL;                 // 0x210
    _PADDING(240)
    unsigned REGOUT0;                   // 0x304
};

extern volatile struct _uicr UICR;


/* GPIO */
struct _gpio {
/* Registers */
    _PADDING(4)
    unsigned OUT;                       // 0x004
    unsigned OUTSET;                    // 0x008
    unsigned OUTCLR;                    // 0x00c
    unsigned IN;                        // 0x010
    unsigned DIR;                       // 0x014
    unsigned DIRSET;                    // 0x018
    unsigned DIRCLR;                    // 0x01c
    _PADDING(480)
    unsigned PINCNF[32];                // 0x200
#define   GPIO_PINCNF_DIR __FIELD(0, 1)
#define     GPIO_DIR_Input 0
#define     GPIO_DIR_Output 1
#define   GPIO_PINCNF_INPUT __FIELD(1, 1)
#define     GPIO_INPUT_Connect 0
#define     GPIO_INPUT_Disconnect 1
#define   GPIO_PINCNF_PULL __FIELD(2, 2)
#define     GPIO_PULL_Disabled 0
#define     GPII_PULL_Pulldown 1
#define     GPIO_PULL_Pullup 3
#define   GPIO_PINCNF_DRIVE __FIELD(8, 3)
#define     GPIO_DRIVE_S0S1 0
#define     GPIO_DRIVE_H0S1 1
#define     GPIO_DRIVE_S0H1 2
#define     GPIO_DRIVE_H0H1 3
#define     GPIO_DRIVE_D0S1 4
#define     GPIO_DRIVE_D0H1 5
#define     GPIO_DRIVE_S0D1 6 /* Open drain */
#define     GPIO_DRIVE_H0D1 7
#define   GPIO_PINCNF_SENSE __FIELD(16, 2)
#define     GPIO_SENSE_Disabled 0
#define     GPIO_SENSE_High 2
#define     GPIO_SENSE_Low 3
};

extern volatile struct _gpio GPIO0, GPIO1;
extern volatile struct _gpio * const GPIO[];


/* GPIOTE */
struct _gpiote {
/* Tasks */
    unsigned OUT[4];                    // 0x000
    _PADDING(32)
    unsigned SET[4];                    // 0x030
    _PADDING(32)
    unsigned CLR[4];                    // 0x060
/* Events */
    _PADDING(144)
    unsigned IN[4];                     // 0x100
    _PADDING(108)
    unsigned PORT;                      // 0x17c
/* Registers */
    _PADDING(388)
    unsigned INTENSET;                  // 0x304
    unsigned INTENCLR;                  // 0x308
    _PADDING(516)
    unsigned CONFIG[4];                 // 0x510
#define   GPIOTE_CONFIG_MODE __FIELD(0, 2)
#define     GPIOTE_MODE_Event 1
#define     GPIOTE_MODE_Task 3
#define   GPIOTE_CONFIG_PSEL __FIELD(8, 6) // Note 6 bits inc port number
#define   GPIOTE_CONFIG_POLARITY __FIELD(16, 2)
#define     GPIOTE_POLARITY_LoToHi 1
#define     GPIOTE_POLARITY_HiToLo 2
#define     GPIOTE_POLARITY_Toggle 3
#define   GPIOTE_CONFIG_OUTINIT __FIELD(20, 1)
};

/* Interrupts */
#define GPIOTE_INT_IN0 0
#define GPIOTE_INT_IN1 1
#define GPIOTE_INT_IN2 2
#define GPIOTE_INT_IN3 3
#define GPIOTE_INT_PORT 31

extern volatile struct _gpiote GPIOTE;


/* PPI */
typedef struct { unsigned EN, DIS; } ppi_chg;

typedef struct { unsigned volatile *EEP, *TEP; } ppi_chan;

struct _ppi {
/* Tasks */
    ppi_chg CHG[6];                     // 0x000
/* Registers */
    _PADDING(1232)
    unsigned CHEN;                      // 0x500
    unsigned CHENSET;                   // 0x504
    unsigned CHENCLR;                   // 0x508
    _PADDING(4)
    ppi_chan CH[20];                    // 0x510
    _PADDING(592)
    unsigned CHGRP[6];                  // 0x800
};

extern volatile struct _ppi PPI;


/* Radio */
struct _radio {
/* Tasks */
    unsigned TXEN;                      // 0x000
    unsigned RXEN;                      // 0x004
    unsigned START;                     // 0x008
    unsigned STOP;                      // 0x00c
    unsigned DISABLE;                   // 0x010
    unsigned RSSISTART;                 // 0x014
    unsigned RSSISTOP;                  // 0x018
    unsigned BCSTART;                   // 0x01c
    unsigned BCSTOP;                    // 0x020
/* Events */
    _PADDING(220)
    unsigned READY;                     // 0x100
    unsigned ADDRESS;                   // 0x104
    unsigned PAYLOAD;                   // 0x108
    unsigned END;                       // 0x10c
    unsigned DISABLED;                  // 0x110
    unsigned DEVMATCH;                  // 0x114
    unsigned DEVMISS;                   // 0x118
    unsigned RSSIEND;                   // 0x11c
    _PADDING(8)
    unsigned BCMATCH;                   // 0x128
/* Registers */
    _PADDING(212)
    unsigned SHORTS;                    // 0x200
    _PADDING(256)
    unsigned INTENSET;                  // 0x304
    unsigned INTENCLR;                  // 0x308
    _PADDING(244)
    unsigned CRCSTATUS;                 // 0x400
    _PADDING(4)
    unsigned RXMATCH;                   // 0x408
    unsigned RXCRC;                     // 0x40c
    unsigned DAI;                       // 0x410
    _PADDING(240)
    void *PACKETPTR;                    // 0x504
    unsigned FREQUENCY;                 // 0x508
    unsigned TXPOWER;                   // 0x50c
    unsigned MODE;                      // 0x510
#define   RADIO_MODE_NRF_1Mbit 0
    unsigned PCNF0;                     // 0x514
#define   RADIO_PCNF0_LFLEN __FIELD(0, 4)
#define   RADIO_PCNF0_S0LEN __FIELD(8, 1)
#define   RADIO_PCNF0_S1LEN __FIELD(16, 4)
    unsigned PCNF1;                     // 0x518
#define   RADIO_PCNF1_MAXLEN __FIELD(0, 8)
#define   RADIO_PCNF1_STATLEN __FIELD(8, 8)
#define   RADIO_PCNF1_BALEN __FIELD(16, 3)
#define   RADIO_PCNF1_ENDIAN __FIELD(24, 1)
#define     RADIO_ENDIAN_Little 0
#define     RADIO_ENDIAN_Big 1
#define   RADIO_PCNF1_WHITEEN __BIT(25)
    unsigned BASE0;                     // 0x51c
    unsigned BASE1;                     // 0x520
    unsigned PREFIX0;                   // 0x524
    unsigned PREFIX1;                   // 0x528
    unsigned TXADDRESS;                 // 0x52c
    unsigned RXADDRESSES;               // 0x530
    unsigned CRCCNF;                    // 0x534
    unsigned CRCPOLY;                   // 0x538
    unsigned CRCINIT;                   // 0x53c
    unsigned TEST;                      // 0x540
    unsigned TIFS;                      // 0x544
    unsigned RSSISAMPLE;                // 0x548
    _PADDING(4)
    unsigned STATE;                     // 0x550
    unsigned DATAWHITEIV;               // 0x554
    _PADDING(8)
    unsigned BCC;                       // 0x560
    _PADDING(156)
    unsigned DAB[8];                    // 0x600
    unsigned DAP[8];                    // 0x620
    unsigned DACNF;                     // 0x640
    _PADDING(224)
    unsigned OVERRIDE[5];               // 0x724
    _PADDING(2244)
    unsigned POWER;                     // 0xffc
};

/* Interrupts */
#define RADIO_INT_READY 0
#define RADIO_INT_END 3
#define RADIO_INT_DISABLED 4

extern volatile struct _radio RADIO;


/* TIMERS: Timers are all 8/16/24/32 bit; timers 3 and 4 have 6 CC registers
   instead of 4. */
struct _timer {
/* Tasks */
    unsigned START;                     // 0x000
    unsigned STOP;                      // 0x004
    unsigned COUNT;                     // 0x008
    unsigned CLEAR;                     // 0x00c
    unsigned SHUTDOWN;                  // 0x010
    _PADDING(44)
    unsigned CAPTURE[6];                // 0x040
/* Events */
    _PADDING(232)
    unsigned COMPARE[6];                // 0x140
/* Registers */
    _PADDING(168)
    unsigned SHORTS;                    // 0x200
    _PADDING(256)
    unsigned INTENSET;                  // 0x304
    unsigned INTENCLR;                  // 0x308
    _PADDING(504)
    unsigned MODE;                      // 0x504
#define   TIMER_MODE_Timer 0
#define   TIMER_MODE_Counter 1
    unsigned BITMODE;                   // 0x508
#define   TIMER_BITMODE_16Bit 0
#define   TIMER_BITMODE_8Bit 1
#define   TIMER_BITMODE_24Bit 2
#define   TIMER_BITMODE_32Bit 3
    _PADDING(4)
    unsigned PRESCALER;                 // 0x510
    _PADDING(44)
    unsigned CC[6];                     // 0x540
};

/* Interrupts */
#define TIMER_INT_COMPARE0 16
#define TIMER_INT_COMPARE1 17
#define TIMER_INT_COMPARE2 18
#define TIMER_INT_COMPARE3 19
#define TIMER_INT_COMPARE4 20
#define TIMER_INT_COMPARE5 21
/* Shortcuts */
#define TIMER_COMPARE0_CLEAR 0
#define TIMER_COMPARE1_CLEAR 1
#define TIMER_COMPARE2_CLEAR 2
#define TIMER_COMPARE3_CLEAR 3
#define TIMER_COMPARE4_CLEAR 4
#define TIMER_COMPARE5_CLEAR 5
#define TIMER_COMPARE0_STOP 8
#define TIMER_COMPARE1_STOP 9
#define TIMER_COMPARE2_STOP 10
#define TIMER_COMPARE3_STOP 11
#define TIMER_COMPARE4_STOP 12
#define TIMER_COMPARE5_STOP 13

extern volatile struct _timer TIMER0, TIMER1, TIMER2, TIMER3, TIMER4;
extern volatile struct _timer * const TIMER[];


/* Random Number Generator */
struct _rng {
/* Tasks */
    unsigned START;                     // 0x000
    unsigned STOP;                      // 0x004
/* Events */
    _PADDING(248)
    unsigned VALRDY;                    // 0x100
/* Registers */
    _PADDING(252)
    unsigned SHORTS;                    // 0x200
    _PADDING(252)
    unsigned INTEN;                     // 0x300
    unsigned INTENSET;                  // 0x304
    unsigned INTENCLR;                  // 0x308
    _PADDING(504)
    unsigned CONFIG;                    // 0x504
#define RNG_CONFIG_DERCEN __BIT(0)
    unsigned VALUE;                     // 0x508
};

/* Interrupts */
#define RNG_INT_VALRDY 0

extern volatile struct _rng RNG;


/* Temperature sensor */
struct _temp {
/* Tasks */
    unsigned START;                     // 0x000
    unsigned STOP;                      // 0x004
/* Events */
    _PADDING(248)
    unsigned DATARDY;                   // 0x100
/* Registers */
    _PADDING(508)
    unsigned INTEN;                     // 0x300
    unsigned INTENSET;                  // 0x304
    unsigned INTENCLR;                  // 0x308
    _PADDING(508)
    unsigned VALUE;                     // 0x508
};

/* Interrupts */
#define TEMP_INT_DATARDY 0

extern volatile struct _temp TEMP;


/* I2C */
struct _i2c {
/* Tasks */
    unsigned STARTRX;                   // 0x000
    _PADDING(4)
    unsigned STARTTX;                   // 0x008
    _PADDING(8)
    unsigned STOP;                      // 0x014
    _PADDING(4)
    unsigned SUSPEND;                   // 0x01c
    unsigned RESUME;                    // 0x020
/* Events */
    _PADDING(224)
    unsigned STOPPED;                   // 0x104
    unsigned RXDREADY;                  // 0x108
    _PADDING(16)
    unsigned TXDSENT;                   // 0x11c
    _PADDING(4)
    unsigned ERROR;                     // 0x124
    _PADDING(16)
    unsigned BB;                        // 0x138
    _PADDING(12)
    unsigned SUSPENDED;                 // 0x148
/* Registers */
    _PADDING(180)
    unsigned SHORTS;                    // 0x200
    _PADDING(252)
    unsigned INTEN;                     // 0x300
    unsigned INTENSET;                  // 0x304
    unsigned INTENCLR;                  // 0x308
    _PADDING(440)
    unsigned ERRORSRC;                  // 0x4c4
#define   I2C_ERRORSRC_OVERRUN __BIT(0)
#define   I2C_ERRORSRC_ANACK __BIT(1)
#define   I2C_ERRORSRC_DNACK __BIT(2)
#define   I2C_ERRORSRC_All 0x7
    _PADDING(56)
    unsigned ENABLE;                    // 0x500
#define   I2C_ENABLE_Disabled 0
#define   I2C_ENABLE_Enabled 5
    _PADDING(4)
    unsigned PSELSCL;                   // 0x508
    unsigned PSELSDA;                   // 0x50c
    _PADDING(8)
    unsigned RXD;                       // 0x518
    unsigned TXD;                       // 0x51c
    _PADDING(4)
    unsigned FREQUENCY;                 // 0x524
#define   I2C_FREQUENCY_100kHz 0x01980000
    _PADDING(96)
    unsigned ADDRESS;                   // 0x588
    _PADDING(2672)
    unsigned POWER;                     // 0xffc
};

/* Interrupts */
#define I2C_INT_STOPPED 1
#define I2C_INT_RXDREADY 2
#define I2C_INT_TXDSENT 7
#define I2C_INT_ERROR 9
#define I2C_INT_BB 14
/* Shortcuts */
#define I2C_BB_SUSPEND 0
#define I2C_BB_STOP 1

extern volatile struct _i2c I2C0, I2C1;
extern volatile struct _i2c * const I2C[];


/* Legacy SPI */
struct _spi {
    _PADDING(264)
    unsigned READY;                     // 0x108
    _PADDING(500)
    unsigned INTEN;                     // 0x300
    unsigned INTENSET;                  // 0x304
    unsigned INTENCLR;                  // 0x308
    _PADDING(500)
    unsigned ENABLE;                    // 0x500
#define   SPI_ENABLE_Enabled 1
#define   SPI_ENABLE_Disabled 0
    _PADDING(4)
    unsigned PSELSCK;                   // 0x508
    unsigned PSELMOSI;                  // 0x50c
    unsigned PSELMISO;                  // 0x510
    _PADDING(4)
    unsigned RXD;                       // 0x518
    unsigned TXD;                       // 0x51c
    _PADDING(4)
    unsigned FREQUENCY;                 // 0x524
#define   SPI_FREQUENCY_125kHz 0x02000000
#define   SPI_FREQUENCY_250kHz 0x04000000
#define   SPI_FREQUENCY_500kHz 0x08000000
#define   SPI_FREQUENCY_1MHz   0x10000000
#define   SPI_FREQUENCY_2MHz   0x20000000
#define   SPI_FREQUENCY_4MHz   0x40000000
#define   SPI_FREQUENCY_8MHz   0x80000000
    _PADDING(44)
    unsigned CONFIG;                    // 0x554
#define   SPI_CONFIG_ORDER __FIELD(0, 1)
#define     SPI_ORDER_MsbFirst 0
#define     SPI_ORDER_LsbFirst 1
#define   SPI_CONFIG_CPHASE __FIELD(1, 1)
#define     SPI_CPHASE_Leading 0
#define     SPI_CPHASE_Trailing 1
#define   SPI_CONFIG_CPOLARITY __FIELD(2, 1)
#define     SPI_CPOLARITY_ActiveHigh 0
#define     SPI_CPOLARITY_ActiveLow 1
};

#define SPI_INT_READY 2

extern volatile struct _spi SPI0, SPI1, SPI2;
extern volatile struct _spi * const SPI[];


/* SPI with EasyDMA */
struct _spim {
/* Tasks */
    _PADDING(16)
    unsigned START;                     // 0x010
    unsigned STOP;                      // 0x014
    _PADDING(4)
    unsigned SUSPEND;                   // 0x01c
    unsigned RESUME;                    // 0x020
/* Events */
    _PADDING(224)
    unsigned STOPPED;                   // 0x104
    _PADDING(8)
    unsigned ENDRX;                     // 0x110
    _PADDING(4)
    unsigned END;                       // 0x118
    _PADDING(48)
    unsigned STARTED;                   // 0x14c
/* Registers */
    _PADDING(176)
    unsigned SHORTS;                    // 0x200
    _PADDING(256)
    unsigned INTENSET;                  // 0x304
    unsigned INTENCLR;                  // 0x308
    _PADDING(244)
    unsigned STALLSTAT;                 // 0x400
#define   SPIM_STALLSTAT_TX __BIT(0)
#define   SPIM_STALLSTAT_RX __BIT(1)
    _PADDING(252)
    unsigned ENABLE;                    // 0x500
#define   SPIM_ENABLE_Disabled 0
#define   SPIM_ENABLE_Enabled 7
    _PADDING(4)
    unsigned PSELSCK;                   // 0x508
    unsigned PSELMOSI;                  // 0x50c
    unsigned PSELMISO;                  // 0x510
    unsigned PSELCSN;                   // 0x514
    _PADDING(12)
    unsigned FREQUENCY;                 // 0x524
    /* Use same FREQUENCY settings as plain SPI, plus (if brave) */
#define   SPI_FREQUENCY_16MHz 0x0a000000
#define   SPI_FREQUENCY_32MHz 0x14000000
    _PADDING(12)
    dma_param RXD;                      // 0x534
    dma_param TXD;                      // 0x544
    unsigned CONFIG;                    // 0x554
    /* Use same CONFIG fields as plain SPI */
    _PADDING(8)
    unsigned RXDELAY;                   // 0x560
    unsigned CSNDUR;                    // 0x564
    unsigned CSNPOL;                    // 0x568
#define   SPIM_CSNPOL_ActiveLow 0
#define   SPIM_CSNPOL_ActiveHigh 1
    unsigned PSELDCX;                   // 0x56c
    _PADDING(80)
    unsigned ORC;                       // 0x5c0
};

/* Interrupts */
#define SPIM_INT_STOPPED 1
#define SPIM_INT_ENDRX 4
#define SPIM_INT_END 6
#define SPIM_INT_ENDTX 8
#define SPIM_INT_STARTED 19

/* Shortcuts */
#define SPIM_END_START 17

extern volatile struct _spim SPIM0, SPIM1, SPIM2, SPIM3;
extern volatile struct _spim * const SPIM[];


/* UART */
struct _uart {
/* Tasks */
    unsigned STARTRX;                   // 0x000
    _PADDING(4)
    unsigned STARTTX;                   // 0x008
/* Events */
    _PADDING(252)
    unsigned RXDRDY;                    // 0x108
    _PADDING(16)
    unsigned TXDRDY;                    // 0x11c
/* Registers */
    _PADDING(484)
    unsigned INTENSET;                  // 0x304
    unsigned INTENCLR;                  // 0x308
    _PADDING(500)
    unsigned ENABLE;                    // 0x500
#define   UART_ENABLE_Disabled 0
#define   UART_ENABLE_Enabled 4
    _PADDING(8)
    unsigned PSELTXD;                   // 0x50c
    _PADDING(4)
    unsigned PSELRXD;                   // 0x514
    unsigned RXD;                       // 0x518
    unsigned TXD;                       // 0x51c
    _PADDING(4)
    unsigned BAUDRATE;                  // 0x524
#define   UART_BAUDRATE_1200   0x0004f000
#define   UART_BAUDRATE_2400   0x0009d000
#define   UART_BAUDRATE_4800   0x0013b000
#define   UART_BAUDRATE_9600   0x00275000
#define   UART_BAUDRATE_14400  0x003af000
#define   UART_BAUDRATE_19200  0x004ea000
#define   UART_BAUDRATE_28800  0x0075c000
#define   UART_BAUDRATE_31250  0x00800000
#define   UART_BAUDRATE_38400  0x009d0000
#define   UART_BAUDRATE_56000  0x00e50000
#define   UART_BAUDRATE_57600  0x00eb0000
#define   UART_BAUDRATE_76800  0x013a9000
#define   UART_BAUDRATE_115200 0x01d60000
#define   UART_BAUDRATE_230400 0x03b00000
#define   UART_BAUDRATE_250000 0x04000000
#define   UART_BAUDRATE_460800 0x07400000
#define   UART_BAUDRATE_921600 0x0f000000
#define   UART_BAUDRATE_1M     0x10000000
    _PADDING(68)
    unsigned CONFIG;                    // 0x56c
#define   UART_CONFIG_HWFC __BIT(0)
#define   UART_CONFIG_PARITY __FIELD(1, 3)
#define     UART_PARITY_None 0
#define     UART_PARITY_Even 7
};

/* Interrupts */
#define UART_INT_RXDRDY 2
#define UART_INT_TXDRDY 7

extern volatile struct _uart UART;


/* UARTE -- UART with EasyDMA */
struct _uarte {
/* Tasks */
    unsigned STARTRX;                   // 0x000
    unsigned STOPRX;                    // 0x004
    unsigned STARTTX;                   // 0x008
    unsigned STOPTX;                    // 0x00c
    _PADDING(28)
    unsigned FLUSHRX;                   // 0x02c
/* Events */
    _PADDING(208)
    unsigned CTS;                       // 0x100
    unsigned NTCS;                      // 0x104
    unsigned RXDRDY;                    // 0x108
    _PADDING(4)
    unsigned ENDRX;                     // 0x110
    _PADDING(8)
    unsigned TXDRDY;                    // 0x11c
    unsigned ENDTX;                     // 0x120
    unsigned ERROR;                     // 0x124
    _PADDING(28)
    unsigned RXTO;                      // 0x144
    _PADDING(4)
    unsigned RXSTARTED;                 // 0x14c
    unsigned TXSTARTED;                 // 0x150
    _PADDING(4)
    unsigned TXSTOPPED;                 // 0x158
/* Registers */
    _PADDING(164)
    unsigned SHORTS;                    // 0x200
    _PADDING(252)
    unsigned INTEN;                     // 0x300
    unsigned INTENSET;                  // 0x304
    unsigned INTENCLR;                  // 0x308
    _PADDING(372)
    unsigned ERRORSRC;                  // 0x480
#define   UARTE_ERROR_OVERRUN __BIT(0)
#define   UARTE_ERROR_PARITY __BIT(1)
#define   UARTE_ERROR_FRAMING __BIT(2)
#define   UARTE_ERROR_BREAK __BIT(3)
    _PADDING(124)
    unsigned ENABLE;                    // 0x500
#define   UARTE_ENABLE_Disabled 0
#define   UARTE_ENABLE_Enabled 8
    _PADDING(4)
    unsigned PSELRTS;                   // 0x508
    unsigned PSELTXD;                   // 0x50c
    unsigned PSELCTS;                   // 0x510
    unsigned PSELRXD;                   // 0x514
    _PADDING(12)
    unsigned BAUDRATE;                  // 0x524
    _PADDING(12)
    dma_param RXD;                      // 0x534
    dma_param TXD;                      // 0x544
    _PADDING(24)
    unsigned CONFIG;                    // 0x56c
#define   UARTE_CONFIG_HWFC __BIT(0)
#define   UARTE_CONFIG_PARITY __FIELD(1, 3)
#define     UARTE_PARITY_Disabled 0
#define     UARTE_PARITY_Enabled 7
#define   UARTE_CONFIG_STOP __FIELD(4, 1)
#define     UARTE_STOP_1 0
#define     UARTE_STOP_2 1
#define   UARTE_CONFIG_PARITYTYPE __FIELD(8, 1)
#define     UARTE_PARITYTYPE_Even 0
#define     UARTE_PARITYTYPE_Odd 1
};

/* Interrupts */
#define UARTE_INT_CTS 0
#define UARTE_INT_NCTS 1
#define UARTE_INT_RXDRDY 2
#define UARTE_INT_ENDRX 4
#define UARTE_INT_TXDRDY 7
#define UARTE_INT_ENDTX 8
#define UARTE_INT_ERROR 9
#define UARTE_INT_RXTO 17
#define UARTE_INT_RXSTARTED 19
#define UARTE_INT_TXSTARTED 20
#define UARTE_INT_TXSTOPPED 22

/* Shortcuts */
#define UARTE_ENDRX_STARTRX 5
#define UARTE_ENDRX_STOPRX 6

extern volatile struct _uarte UARTE0, UARTE1;
extern volatile struct _uarte * const UARTE[];
       

/* SAADC -- Successive approximation ADC */
typedef struct {
    unsigned PSELP;
    unsigned PSELN;
    unsigned CONFIG;
    unsigned LIMIT;
} adc_chan;

struct _adc {
/* Tasks */
    unsigned START;                     // 0x000
    unsigned SAMPLE;                    // 0x004
    unsigned STOP;                      // 0x008
    unsigned CALIBRATE;                 // 0x00c
/* Events */
    _PADDING(240)
    unsigned STARTED;                   // 0x100
    unsigned END;                       // 0x104
    unsigned DONE;                      // 0x108
    unsigned RESULTDONE;                // 0x10c
    unsigned CALDONE;                   // 0x110
    unsigned STOPPED;                   // 0x114
/* Registers */
    _PADDING(488)
    unsigned INTEN;                     // 0x300
    unsigned INTENSET;                  // 0x304
    unsigned INTENCLR;                  // 0x308
    _PADDING(244)
    unsigned BUSY;                      // 0x400
    _PADDING(252)
    unsigned ENABLE;                    // 0x500
    _PADDING(12)
    adc_chan CHAN[8];                   // 0x510
#define   ADC_CONFIG_RESP __FIELD(0, 2)
#define   ADC_CONFIG_RESN __FIELD(4, 2)
#define     ADC_RES_Bypass 0
#define     ADC_RES_Pulldown 1
#define     ADC_RES_Pullup 2
#define     ADC_RES_VDD_1_2 3
#define   ADC_CONFIG_GAIN __FIELD(8, 3)
#define     ADC_GAIN_1_6 0
#define     ADC_GAIN_1_5 1
#define     ADC_GAIN_1_4 2
#define     ADC_GAIN_1_3 3
#define     ADC_GAIN_1_2 4
#define     ADC_GAIN_1 5
#define     ADC_GAIN_2 6
#define     ADC_GAIN_3 7
#define   ADC_CONFIG_REFSEL __FIELD(12, 1)
#define     ADC_REFSEL_Internal 0
#define     ADC_REFSEL_VDD_1_4 1
#define   ADC_CONFIG_TACQ __FIELD(16, 3)
#define     ADC_TACQ_3us 0
#define     ADC_TACQ_5us 1
#define     ADC_TACQ_10us 2
#define     ADC_TACQ_15us 3
#define     ADC_TACQ_20us 4
#define     ADC_TACQ_40us 5
#define  ADC_CONFIG_MODE __FIELD(20, 1)
#define     ADC_MODE_SE 0
#define     ADC_MODE_Diff 1
#define  ADC_CONFIG_BURST __FIELD(24, 1)
#define     ADC_BURST_Disabled 0
#define     ADC_BURST_Enabled 1
    _PADDING(96)
    unsigned RESOLUTION;                // 0x5f0
#define   ADC_RESOLUTION_8bit 0
#define   ADC_RESOLUTION_10bit 1
#define   ADC_RESOLUTION_12bit 2
#define   ADC_RESOLUTION_14bit 3
    unsigned OVERSAMPLE;                // 0x5f4
    unsigned SAMPLERATE;                // 0x5f8
    _PADDING(48)
    dma_param RESULT;                   // 0x62c
};

/* Interrupts */
#define ADC_INT_STARTED 0
#define ADC_INT_END 1
#define ADC_INT_DONE 2
#define ADC_INT_RESULTDONE 3
#define ADC_INT_CALDONE 4
#define ADC_INT_STOPPED 5

extern volatile struct _adc ADC;


/* PWM */
typedef struct {
    void *PTR;
    unsigned CNT;
    unsigned REFRESH;
    unsigned ENDDELAY;
    unsigned char filler[16];
} pwm_sequence;

struct _pwm {
/* Tasks */
    _PADDING(4)
    unsigned STOP;                      // 0x004
    unsigned SEQSTART[2];               // 0x008
    unsigned NEXTSTEP;                  // 0x010
/* Events */
    _PADDING(240)
    unsigned STOPPED;                   // 0x104
    unsigned SEQSTARTED[2];             // 0x108
    unsigned SEQEND[2];                 // 0x110
    unsigned PWMPERIODEND;              // 0x118
    unsigned LOOPSDONE;                 // 0x11c
/* Registers */
    _PADDING(224)
    unsigned SHORTS;                    // 0x200
    _PADDING(252)
    unsigned INTEN;                     // 0x300
    unsigned INTENSET;                  // 0x304
    unsigned INTENCLR;                  // 0x308
    _PADDING(500)
    unsigned ENABLE;                    // 0x500
    unsigned MODE;                      // 0x504
#define   PWM_MODE_Up 0
#define   PWM_MODE_UpAndDown 1
    unsigned COUNTERTOP;                // 0x508
    unsigned PRESCALER;                 // 0x50c
    unsigned DECODER;                   // 0x510
#define   PWM_DECODER_LOAD __FIELD(0, 2)
#define     PWM_LOAD_Common 0
#define     PWM_LOAD_Grouped 1
#define     PWM_LOAD_Individual 2
#define     PWM_LOAD_Waveform 3
#define   PWM_DECODER_MODE __FIELD(8, 1)
#define     PWM_MODE_RefreshCount 0
#define     PWM_MODE_NextStep 1
    unsigned LOOP;                      // 0x514
    _PADDING(8)
    pwm_sequence SEQ[2];                // 0x520
#define   PWM_SEQ_COMPARE __FIELD(0, 15)
#define   PWM_SEQ_POLARITY __FIELD(15, 1)
#define     PWM_POLARITY_RisingEdge 0
#define     PWM_POLARITY_FallingEdge 1
    unsigned PSEL[4];                   // 0x560
};

/* Interrupts */
#define PWM_INT_STOPPED 1
#define PWM_INT_SEQSTARTED0 2
#define PWM_INT_SEQSTARTED1 3
#define PWM_INT_SEQEND0 4
#define PWM_INT_SEQEND1 5
#define PWM_INT_PWMPERIODEND 6
#define PWM_INT_LOOPSDONE 7

/* Shortcuts */
#define PWM_SEQEND0_STOP 0
#define PWM_SEQEND1_STOP 1
#define PWM_LOOPSDONE_SEQSTART0 2
#define PWM_LOOPSDONE_SEQSTART1 3
#define PWM_LOOPSDONE_STOP 4

/* Sample format */
#define PWM_SAMPLE(c, p) \
    (FIELD(PWM_SEQ_COMPARE, c) | FIELD(PWM_SEQ_POLARITY, p))
#define RISING PWM_POLARITY_RisingEdge
#define FALLING PWM_POLARITY_FallingEdge

extern volatile struct _pwm PWM0, PWM1, PWM2, PWM3;
extern volatile struct _pwm * const PWM[];


/* NVIC stuff */

/* irq_priority -- set priority of an IRQ from 0 (highest) to 255 */
void irq_priority(int irq, unsigned priority);

/* enable_irq -- enable interrupts from an IRQ */
void enable_irq(int irq);

/* disable_irq -- disable interrupts from a specific IRQ */
void disable_irq(int irq);

/* clear_pending -- clear pending interrupt from an IRQ */
void clear_pending(int irq);

/* reschedule -- request PendSV interrupt */
#define reschedule()  SCB.ICSR = BIT(SCB_ICSR_PENDSVSET)

/* active_irq -- find active interrupt: returns -16 to 31 */
#define active_irq()  (GET_FIELD(SCB.ICSR, SCB_ICSR_VECTACTIVE) - 16)

/* sysreset -- request a system reset */
#define sysreset()  SCB.AIRCR = FIELD(SCB_AIRCR_VECTKEY, 0x05fa) \
                                | BIT(SCB_AIRCR_SYSRESETREQ)

/* delay_loop -- timed delay */
void delay_loop(unsigned usec);


/* GPIO convenience */

/* gpio_dir -- set GPIO direction */
void gpio_dir(unsigned pin, unsigned dir);

/* gpio_connect -- connect pin for input */
void gpio_connect(unsigned pin);

/* gpio_drive -- set GPIO drive strength */
void gpio_drive(unsigned pin, unsigned mode);

/* gpio_out -- set GPIO output value */
void gpio_out(unsigned pin, unsigned value);

/* gpio_in -- get GPIO input bit */
unsigned gpio_in(unsigned pin);


/* Image constants */

#define NIMG 10

typedef unsigned image[NIMG];

#define LED_MASK0 0xd1688800
#define LED_MASK1 0x00000020
#define LED_DOT0  0x50008800
#define LED_DOT1  0x00000020

#define _ROW(r, c1, c2, c3, c4, c5) \
    BIT(r) | (!c1<<28) | (!c2<<11) | (!c3<<31) | (!c5<<30), (!c4<<5)

#define IMAGE(x11, x12, x13, x14, x15, \
              x21, x22, x23, x24, x25, \
              x31, x32, x33, x34, x35, \
              x41, x42, x43, x44, x45, \
              x51, x52, x53, x54, x55) \
    { _ROW(ROW1, x11, x12, x13, x14, x15), \
      _ROW(ROW2, x21, x22, x23, x24, x25), \
      _ROW(ROW3, x31, x32, x33, x34, x35), \
      _ROW(ROW4, x41, x42, x43, x44, x45), \
      _ROW(ROW5, x51, x52, x53, x54, x55) }

#define led_init()  GPIO0.DIRSET = LED_MASK0, GPIO1.DIRSET = LED_MASK1
#define led_dot()   GPIO0.OUTSET = LED_DOT0, GPIO1.OUTSET = LED_DOT1
#define led_off()   GPIO0.OUTCLR = LED_MASK0, GPIO1.OUTCLR = LED_MASK1


/* CODERAM -- mark function for copying to RAM */
#define CODERAM  __attribute((noinline, section(".xram")))

/* A few assembler macros for single instructions. */
#define pause()         asm volatile ("wfe")
#define intr_disable()  asm volatile ("cpsid i")
#define intr_enable()   asm volatile ("cpsie i")
#define get_primask()   ({ unsigned x;                                   \
                           asm volatile ("mrs %0, primask" : "=r"(x)); x; })
#define set_primask(x)  asm volatile ("msr primask, %0" : : "r"(x))
#define nop()           asm volatile ("nop")
