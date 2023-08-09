/* common/hardware.h */
/* Copyright (c) 2018-20 J. M. Spivey */

#define UBIT 1
#define UBIT_V2 1

/* Hardware register definitions for nRF52833 */

#define BIT(i) (1 << (i))
#define SET_BIT(reg, n) reg |= BIT(n)
#define GET_BIT(reg, n) (((reg) >> (n)) & 0x1)
#define CLR_BIT(reg, n) reg &= ~BIT(n)
#define GET_BYTE(reg, n) (((reg) >> (8*(n))) & 0xff)
#define SET_BYTE(reg, n, v) \
    reg = (reg & ~(0xff << 8*n)) | ((v & 0xff) << 8*n)

/* The macros SET_FIELD, etc., are defined in an indirect way that
permits (because of the timing of CPP macro expansion) the 'field'
argument to be a macro that expands the a 'position, width' pair. */

#define SET_FIELD(reg, field, val) _SET_FIELD(reg, field, val)
#define _SET_FIELD(reg, pos, wd, val) \
    reg = (reg & ~_MASK(pos, wd)) | _FIELD(pos, wd, val)

#define GET_FIELD(reg, field) _GET_FIELD(reg, field)
#define _GET_FIELD(reg, pos, wd)  ((reg >> pos) & _MASK0(wd))

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
#define USB_TX DEVPIN(0, 6)
#define I2C0_SCL DEVPIN(0, 8)
#define PAD9  DEVPIN(0, 9)
#define PAD8  DEVPIN(0, 10)
#define PAD7  DEVPIN(0, 11)
#define   COL2 PAD7
#define PAD12 DEVPIN(0, 12)
#define PAD15 DEVPIN(0, 13)
#define PAD5  DEVPIN(0, 14)
#define   BUTTON_A PAD5
#define ROW3 DEVPIN(0, 15)
#define I2C0_SDA DEVPIN(0, 16)
#define PAD13 DEVPIN(0, 17)
#define ROW5 DEVPIN(0, 19)
#define ROW1 DEVPIN(0, 21)
#define ROW2 DEVPIN(0, 22)
#define PAD11 DEVPIN(0, 23)
#define   BUTTON_B PAD11
#define ROW4 DEVPIN(0, 24)
#define PAD19 DEVPIN(0, 26)
#define   I2C1_SCL PAD19
#define PAD4  DEVPIN(0, 28)
#define   COL1 PAD4
#define PAD10 DEVPIN(0, 30)
#define   COL5 PAD10
#define PAD3  DEVPIN(0, 31)
#define   COL3 PAD3

#define PAD20 DEVPIN(1, 0)
#define   I2C1_SDA PAD20
#define PAD16 DEVPIN(1, 2)
#define PAD6  DEVPIN(1, 5)
#define   COL4 PAD6
#define USB_RX DEVPIN(1, 8)


/* Interrupts */
#define SVC_IRQ    -5
#define PENDSV_IRQ -2
#define SYSTICK_IRQ -1
#define RADIO_IRQ   1
#define UART0_IRQ   2
#define I2C0_IRQ    3
#define I2C1_IRQ    4
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
#define SPI0_IRQ   35
#define UART1_IRQ  40
#define PWM3_IRQ   45
#define SPI1_IRQ   47

#define N_INTERRUPTS 64

/* For compatibility, we allow UART as a synonym for UART0 */
#define UART_IRQ UART0_IRQ
#define uart_handler uart0_handler


/* Device registers */
#define _BASE(addr) ((unsigned volatile *) addr)
#define _REG(ty, addr) (* (ty volatile *) addr)
#define _ARR(ty, addr) ((ty volatile *) addr)

/* Common layout for DMA parameters */
typedef struct {
    void *PTR;
    unsigned MAXCNT;
    unsigned AMOUNT;
} dma_param;


/* System contol block */

#define SCB_BASE                        _BASE(0xe000ed00)
#define SCB_CPUID                       _REG(unsigned, 0xe000ed00)
#define SCB_ICSR                        _REG(unsigned, 0xe000ed04)
#define   SCB_ICSR_PENDSVSET __BIT(28)
#define   SCB_ICSR_VECTACTIVE __FIELD(0, 8)
#define SCB_SCR                         _REG(unsigned, 0xe000ed10)
#define   SCB_SCR_SLEEPONEXIT __BIT(1)
#define   SCB_SCR_SLEEPDEEP __BIT(2)
#define   SCB_SCR_SEVONPEND __BIT(4)
#define SCB_SHPR                        _ARR(unsigned, 0xe000ed18)


/* Debug */

#define DEBUG_BASE                      _BASE(0xe000ed00)
#define DEBUG_DEMCR                     _REG(unsigned, 0xe000edfc)
#define   DEBUG_DEMCR_TRCENA __BIT(24)


/* Nested vectored interupt controller */

#define NVIC_BASE                       _BASE(0xe000e000)
#define NVIC_ISER                       _ARR(unsigned, 0xe000e100)
#define NVIC_ICER                       _ARR(unsigned, 0xe000e180)
#define NVIC_ISPR                       _ARR(unsigned, 0xe000e200)
#define NVIC_ICPR                       _ARR(unsigned, 0xe000e280)
#define NVIC_IPR                        _ARR(unsigned, 0xe000e400)


/* Systick timer */

#define SYSTICK_CLOCK 64000000

#define SYSTICK_BASE                    _BASE(0xe000e000)
#define SYSTICK_CSR                     _REG(unsigned, 0xe000e010)
#define   SYSTICK_CSR_COUNTFLAG __BIT(16)
#define   SYSTICK_CSR_CLKSOURCE __FIELD(2, 1)
#define     SYSTICK_CLKSOURCE_External 0
#define     SYSTICK_CLKSOURCE_Internal 1
#define   SYSTICK_CSR_TICKINT __BIT(1)
#define   SYSTICK_CSR_ENABLE __BIT(0)
#define SYSTICK_RVR                     _REG(unsigned, 0xe000e014)
#define SYSTICK_CVR                     _REG(unsigned, 0xe000e018)
#define SYSTICK_CALIB                   _REG(unsigned, 0xe000e01c)
#define   SYSTICK_CALIB_NOREF __BIT(31)
#define   SYSTICK_CALIB_SKEW __BIT(30)
#define   SYSTICK_CALIB_TENMS __FIELD(0, 24)


/* Data watchpoint and trace */

#define DWT_BASE                        _BASE(0xe0001000)
#define DWT_CTRL                        _REG(unsigned, 0xe0001000)
#define   DWT_CTRL_CYCCNTENA __BIT(0)
#define DWT_CYCCNT                      _REG(unsigned, 0xe0001004)


/* Clock control */

#define CLOCK_BASE                      _BASE(0x40000000)
#define CLOCK_HFCLKSTART                _REG(unsigned, 0x40000000)
#define CLOCK_LFCLKSTART                _REG(unsigned, 0x40000008)
#define CLOCK_HFCLKSTARTED              _REG(unsigned, 0x40000100)
#define CLOCK_LFCLKSTARTED              _REG(unsigned, 0x40000104)
#define CLOCK_LFCLKSRC                  _REG(unsigned, 0x40000518)
#define   CLOCK_LFCLKSRC_RC 0
#define CLOCK_XTALFREQ                  _REG(unsigned, 0x40000550)
#define   CLOCK_XTALFREQ_16MHz 0xFF


/* Memory protection unit */

#define MPU_BASE                        _BASE(0x40000000)
#define MPU_DISABLEINDEBUG              _REG(unsigned, 0x40000608)


/* Factory information */

#define FICR_BASE                       _BASE(0x10000000)
#define FICR_DEVICEID                   _ARR(unsigned, 0x10000060)
#define FICR_DEVICEADDR                 _ARR(unsigned, 0x100000a4)
#define FICR_OVERRIDEEN                 _REG(unsigned, 0x100000a0)
#define   FICR_OVERRIDEEN_NRF __BIT(0)
#define FICR_NRF_1MBIT                  _ARR(unsigned, 0x100000b0)


/* Power management */

/* Interrupts */
#define POWER_INT_POFWARN 2

#define POWER_BASE                      _BASE(0x40000000)
/* Tasks */
#define POWER_CONSTLAT                  _REG(unsigned, 0x40000078)
#define POWER_LOWPWR                    _REG(unsigned, 0x4000007c)
/* Events */
#define POWER_POFWARN                   _REG(unsigned, 0x40000108)
/* Registers */
#define POWER_INTENSET                  _REG(unsigned, 0x40000304)
#define POWER_INTENCLR                  _REG(unsigned, 0x40000308)
#define POWER_RESETREAS                 _REG(unsigned, 0x40000400)
#define   POWER_RESETREAS_RESETPIN __BIT(0)
#define   POWER_RESETREAS_DOG __BIT(1)
#define   POWER_RESETREAS_SREQ __BIT(2)
#define   POWER_RESETREAS_LOCKUP __BIT(3)
#define   POWER_RESETREAS_OFF __BIT(16)
#define   POWER_RESETREAS_LPCOMP __BIT(17)
#define   POWER_RESETREAS_DIF _BIT(18)
#define   POWER_RESETREAS_ALL 0x0007000f
#define POWER_RAMSTATUS                 _REG(unsigned, 0x40000428)
#define POWER_SYSTEMOFF                 _REG(unsigned, 0x40000500)
#define POWER_POFCON                    _REG(unsigned, 0x40000510)
#define   POWER_POFCON_POF __BIT(1)
#define   POWER_POFCON_TRESHOLD _FIELD(1, 2)
#define     POWER_THRESHOLD_V21 0
#define     POWER_THRESHOLD_V23 1
#define     POWER_THRESHOLD_V25 2
#define     POWER_THRESHOLD_V27 3
#define POWER_GPREGRET                  _REG(unsigned, 0x4000051c)
#define POWER_GPREGRET2                 _REG(unsigned, 0x40000520)
#define POWER_DCDCEN                    _REG(unsigned, 0x40000578)


/* Watchdog timer */

/* Interrupts */
#define WDT_INT_TIMEOUT 0

#define WDT_BASE                        _BASE(0x40010000)
/* Tasks */
#define WDT_START                       _REG(unsigned, 0x40010000)
/* Events */
#define WDT_TIMEOUT                     _REG(unsigned, 0x40010100)
/* Registers */
#define WDT_INTENSET                    _REG(unsigned, 0x40010304)
#define WDT_INTENCLR                    _REG(unsigned, 0x40010308)
#define WDT_RUNSTATUS                   _REG(unsigned, 0x40010400)
#define WDT_REQSTATUS                   _REG(unsigned, 0x40010404)
#define WDT_CRV                         _REG(unsigned, 0x40010504)
#define   WDT_HERTZ 32768
#define WDT_RREN                        _REG(unsigned, 0x40010508)
#define WDT_CONFIG                      _REG(unsigned, 0x4001050c)
#define   WDT_CONFIG_SLEEP __BIT(0)
#define   WDT_CONFIG_HALT __BIT(3)
#define WDT_RR                          _ARR(unsigned, 0x40010600)
#define   WDT_MAGIC 0x6e524635


/* Non-Volatile Memory Controller */

#define NVMC_BASE                       _BASE(0x4001e000)
#define NVMC_READY                      _REG(unsigned, 0x4001e400)
#define NVMC_CONFIG                     _REG(unsigned, 0x4001e504)
#define   NVMC_CONFIG_WEN __BIT(0)
#define   NVMC_CONFIG_EEN __BIT(1)
#define NVMC_ERASEPAGE                  _REG(void*, 0x4001e508)
#define NVMC_ICACHECONF                 _REG(unsigned, 0x4001e540)
#define   NVMC_ICACHECONF_CACHEEN __BIT(0)


/* GPIO */

#define GPIO0_BASE                      _BASE(0x50000500)
/* Registers */
#define GPIO0_OUT                       _REG(unsigned, 0x50000504)
#define GPIO0_OUTSET                    _REG(unsigned, 0x50000508)
#define GPIO0_OUTCLR                    _REG(unsigned, 0x5000050c)
#define GPIO0_IN                        _REG(unsigned, 0x50000510)
#define GPIO0_DIR                       _REG(unsigned, 0x50000514)
#define GPIO0_DIRSET                    _REG(unsigned, 0x50000518)
#define GPIO0_DIRCLR                    _REG(unsigned, 0x5000051c)
#define GPIO0_PINCNF                    _ARR(unsigned, 0x50000700)
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

#define GPIO1_BASE                      _BASE(0x50000800)
#define GPIO1_OUT                       _REG(unsigned, 0x50000804)
#define GPIO1_OUTSET                    _REG(unsigned, 0x50000808)
#define GPIO1_OUTCLR                    _REG(unsigned, 0x5000080c)
#define GPIO1_IN                        _REG(unsigned, 0x50000810)
#define GPIO1_DIR                       _REG(unsigned, 0x50000814)
#define GPIO1_DIRSET                    _REG(unsigned, 0x50000818)
#define GPIO1_DIRCLR                    _REG(unsigned, 0x5000081c)
#define GPIO1_PINCNF                    _ARR(unsigned, 0x50000a00)


/* GPIOTE */

/* Interrupts */
#define GPIOTE_INT_IN0 0
#define GPIOTE_INT_IN1 1
#define GPIOTE_INT_IN2 2
#define GPIOTE_INT_IN3 3
#define GPIOTE_INT_PORT 31

#define GPIOTE_BASE                     _BASE(0x40006000)
/* Tasks */
#define GPIOTE_OUT                      _ARR(unsigned, 0x40006000)
#define GPIOTE_SET                      _ARR(unsigned, 0x40006030)
#define GPIOTE_CLR                      _ARR(unsigned, 0x40006060)
/* Events */
#define GPIOTE_IN                       _ARR(unsigned, 0x40006100)
#define GPIOTE_PORT                     _REG(unsigned, 0x4000617c)
/* Registers */
#define GPIOTE_INTENSET                 _REG(unsigned, 0x40006304)
#define GPIOTE_INTENCLR                 _REG(unsigned, 0x40006308)
#define GPIOTE_CONFIG                   _ARR(unsigned, 0x40006510)
#define   GPIOTE_CONFIG_MODE __FIELD(0, 2)
#define     GPIOTE_MODE_Event 1
#define     GPIOTE_MODE_Task 3
#define   GPIOTE_CONFIG_PSEL __FIELD(8, 6) // Note 6 bits inc port number
#define   GPIOTE_CONFIG_POLARITY __FIELD(16, 2)
#define     GPIOTE_POLARITY_LoToHi 1
#define     GPIOTE_POLARITY_HiToLo 2
#define     GPIOTE_POLARITY_Toggle 3
#define   GPIOTE_CONFIG_OUTINIT __FIELD(20, 1)


/* PPI */
typedef struct { unsigned EN, DIS; } ppi_chg;

typedef struct { unsigned volatile *EEP, *TEP; } ppi_chan;


#define PPI_BASE                        _BASE(0x4001f000)
/* Tasks */
#define PPI_CHG                         _ARR(ppi_chg, 0x4001f000)
/* Registers */
#define PPI_CHEN                        _REG(unsigned, 0x4001f500)
#define PPI_CHENSET                     _REG(unsigned, 0x4001f504)
#define PPI_CHENCLR                     _REG(unsigned, 0x4001f508)
#define PPI_CH                          _ARR(ppi_chan, 0x4001f510)
#define PPI_CHGRP                       _ARR(unsigned, 0x4001f800)


/* Radio */

/* Interrupts */
#define RADIO_INT_READY 0
#define RADIO_INT_END 3
#define RADIO_INT_DISABLED 4

#define RADIO_BASE                      _BASE(0x40001000)
/* Tasks */
#define RADIO_TXEN                      _REG(unsigned, 0x40001000)
#define RADIO_RXEN                      _REG(unsigned, 0x40001004)
#define RADIO_START                     _REG(unsigned, 0x40001008)
#define RADIO_STOP                      _REG(unsigned, 0x4000100c)
#define RADIO_DISABLE                   _REG(unsigned, 0x40001010)
#define RADIO_RSSISTART                 _REG(unsigned, 0x40001014)
#define RADIO_RSSISTOP                  _REG(unsigned, 0x40001018)
#define RADIO_BCSTART                   _REG(unsigned, 0x4000101c)
#define RADIO_BCSTOP                    _REG(unsigned, 0x40001020)
/* Events */
#define RADIO_READY                     _REG(unsigned, 0x40001100)
#define RADIO_ADDRESS                   _REG(unsigned, 0x40001104)
#define RADIO_PAYLOAD                   _REG(unsigned, 0x40001108)
#define RADIO_END                       _REG(unsigned, 0x4000110c)
#define RADIO_DISABLED                  _REG(unsigned, 0x40001110)
#define RADIO_DEVMATCH                  _REG(unsigned, 0x40001114)
#define RADIO_DEVMISS                   _REG(unsigned, 0x40001118)
#define RADIO_RSSIEND                   _REG(unsigned, 0x4000111c)
#define RADIO_BCMATCH                   _REG(unsigned, 0x40001128)
/* Registers */
#define RADIO_SHORTS                    _REG(unsigned, 0x40001200)
#define RADIO_INTENSET                  _REG(unsigned, 0x40001304)
#define RADIO_INTENCLR                  _REG(unsigned, 0x40001308)
#define RADIO_CRCSTATUS                 _REG(unsigned, 0x40001400)
#define RADIO_RXMATCH                   _REG(unsigned, 0x40001408)
#define RADIO_RXCRC                     _REG(unsigned, 0x4000140c)
#define RADIO_DAI                       _REG(unsigned, 0x40001410)
#define RADIO_PACKETPTR                 _REG(void*, 0x40001504)
#define RADIO_FREQUENCY                 _REG(unsigned, 0x40001508)
#define RADIO_TXPOWER                   _REG(unsigned, 0x4000150c)
#define RADIO_MODE                      _REG(unsigned, 0x40001510)
#define   RADIO_MODE_NRF_1Mbit 0
#define RADIO_PCNF0                     _REG(unsigned, 0x40001514)
#define   RADIO_PCNF0_LFLEN __FIELD(0, 4)
#define   RADIO_PCNF0_S0LEN __FIELD(8, 1)
#define   RADIO_PCNF0_S1LEN __FIELD(16, 4)
#define RADIO_PCNF1                     _REG(unsigned, 0x40001518)
#define   RADIO_PCNF1_MAXLEN __FIELD(0, 8)
#define   RADIO_PCNF1_STATLEN __FIELD(8, 8)
#define   RADIO_PCNF1_BALEN __FIELD(16, 3)
#define   RADIO_PCNF1_ENDIAN __FIELD(24, 1)
#define     RADIO_ENDIAN_Little 0
#define     RADIO_ENDIAN_Big 1
#define   RADIO_PCNF1_WHITEEN __BIT(25)
#define RADIO_BASE0                     _REG(unsigned, 0x4000151c)
#define RADIO_BASE1                     _REG(unsigned, 0x40001520)
#define RADIO_PREFIX0                   _REG(unsigned, 0x40001524)
#define RADIO_PREFIX1                   _REG(unsigned, 0x40001528)
#define RADIO_TXADDRESS                 _REG(unsigned, 0x4000152c)
#define RADIO_RXADDRESSES               _REG(unsigned, 0x40001530)
#define RADIO_CRCCNF                    _REG(unsigned, 0x40001534)
#define RADIO_CRCPOLY                   _REG(unsigned, 0x40001538)
#define RADIO_CRCINIT                   _REG(unsigned, 0x4000153c)
#define RADIO_TEST                      _REG(unsigned, 0x40001540)
#define RADIO_TIFS                      _REG(unsigned, 0x40001544)
#define RADIO_RSSISAMPLE                _REG(unsigned, 0x40001548)
#define RADIO_STATE                     _REG(unsigned, 0x40001550)
#define RADIO_DATAWHITEIV               _REG(unsigned, 0x40001554)
#define RADIO_BCC                       _REG(unsigned, 0x40001560)
#define RADIO_DAB                       _ARR(unsigned, 0x40001600)
#define RADIO_DAP                       _ARR(unsigned, 0x40001620)
#define RADIO_DACNF                     _REG(unsigned, 0x40001640)
#define RADIO_OVERRIDE                  _ARR(unsigned, 0x40001724)
#define RADIO_POWER                     _REG(unsigned, 0x40001ffc)


/* TIMERS: Timers are all 8/16/24/32 bit; timers 3 and 4 have 6 CC registers
   instead of 4. */

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

#define TIMER0_BASE                     _BASE(0x40008000)
/* Tasks */
#define TIMER0_START                    _REG(unsigned, 0x40008000)
#define TIMER0_STOP                     _REG(unsigned, 0x40008004)
#define TIMER0_COUNT                    _REG(unsigned, 0x40008008)
#define TIMER0_CLEAR                    _REG(unsigned, 0x4000800c)
#define TIMER0_SHUTDOWN                 _REG(unsigned, 0x40008010)
#define TIMER0_CAPTURE                  _ARR(unsigned, 0x40008040)
/* Events */
#define TIMER0_COMPARE                  _ARR(unsigned, 0x40008140)
/* Registers */
#define TIMER0_SHORTS                   _REG(unsigned, 0x40008200)
#define TIMER0_INTENSET                 _REG(unsigned, 0x40008304)
#define TIMER0_INTENCLR                 _REG(unsigned, 0x40008308)
#define TIMER0_MODE                     _REG(unsigned, 0x40008504)
#define   TIMER_MODE_Timer 0
#define   TIMER_MODE_Counter 1
#define TIMER0_BITMODE                  _REG(unsigned, 0x40008508)
#define   TIMER_BITMODE_16Bit 0
#define   TIMER_BITMODE_8Bit 1
#define   TIMER_BITMODE_24Bit 2
#define   TIMER_BITMODE_32Bit 3
#define TIMER0_PRESCALER                _REG(unsigned, 0x40008510)
#define TIMER0_CC                       _ARR(unsigned, 0x40008540)

#define TIMER1_BASE                     _BASE(0x40009000)
#define TIMER1_START                    _REG(unsigned, 0x40009000)
#define TIMER1_STOP                     _REG(unsigned, 0x40009004)
#define TIMER1_COUNT                    _REG(unsigned, 0x40009008)
#define TIMER1_CLEAR                    _REG(unsigned, 0x4000900c)
#define TIMER1_SHUTDOWN                 _REG(unsigned, 0x40009010)
#define TIMER1_CAPTURE                  _ARR(unsigned, 0x40009040)
#define TIMER1_COMPARE                  _ARR(unsigned, 0x40009140)
#define TIMER1_SHORTS                   _REG(unsigned, 0x40009200)
#define TIMER1_INTENSET                 _REG(unsigned, 0x40009304)
#define TIMER1_INTENCLR                 _REG(unsigned, 0x40009308)
#define TIMER1_MODE                     _REG(unsigned, 0x40009504)
#define TIMER1_BITMODE                  _REG(unsigned, 0x40009508)
#define TIMER1_PRESCALER                _REG(unsigned, 0x40009510)
#define TIMER1_CC                       _ARR(unsigned, 0x40009540)

#define TIMER2_BASE                     _BASE(0x4000a000)
#define TIMER2_START                    _REG(unsigned, 0x4000a000)
#define TIMER2_STOP                     _REG(unsigned, 0x4000a004)
#define TIMER2_COUNT                    _REG(unsigned, 0x4000a008)
#define TIMER2_CLEAR                    _REG(unsigned, 0x4000a00c)
#define TIMER2_SHUTDOWN                 _REG(unsigned, 0x4000a010)
#define TIMER2_CAPTURE                  _ARR(unsigned, 0x4000a040)
#define TIMER2_COMPARE                  _ARR(unsigned, 0x4000a140)
#define TIMER2_SHORTS                   _REG(unsigned, 0x4000a200)
#define TIMER2_INTENSET                 _REG(unsigned, 0x4000a304)
#define TIMER2_INTENCLR                 _REG(unsigned, 0x4000a308)
#define TIMER2_MODE                     _REG(unsigned, 0x4000a504)
#define TIMER2_BITMODE                  _REG(unsigned, 0x4000a508)
#define TIMER2_PRESCALER                _REG(unsigned, 0x4000a510)
#define TIMER2_CC                       _ARR(unsigned, 0x4000a540)

#define TIMER3_BASE                     _BASE(0x4001a000)
#define TIMER3_START                    _REG(unsigned, 0x4001a000)
#define TIMER3_STOP                     _REG(unsigned, 0x4001a004)
#define TIMER3_COUNT                    _REG(unsigned, 0x4001a008)
#define TIMER3_CLEAR                    _REG(unsigned, 0x4001a00c)
#define TIMER3_SHUTDOWN                 _REG(unsigned, 0x4001a010)
#define TIMER3_CAPTURE                  _ARR(unsigned, 0x4001a040)
#define TIMER3_COMPARE                  _ARR(unsigned, 0x4001a140)
#define TIMER3_SHORTS                   _REG(unsigned, 0x4001a200)
#define TIMER3_INTENSET                 _REG(unsigned, 0x4001a304)
#define TIMER3_INTENCLR                 _REG(unsigned, 0x4001a308)
#define TIMER3_MODE                     _REG(unsigned, 0x4001a504)
#define TIMER3_BITMODE                  _REG(unsigned, 0x4001a508)
#define TIMER3_PRESCALER                _REG(unsigned, 0x4001a510)
#define TIMER3_CC                       _ARR(unsigned, 0x4001a540)

#define TIMER4_BASE                     _BASE(0x4001b000)
#define TIMER4_START                    _REG(unsigned, 0x4001b000)
#define TIMER4_STOP                     _REG(unsigned, 0x4001b004)
#define TIMER4_COUNT                    _REG(unsigned, 0x4001b008)
#define TIMER4_CLEAR                    _REG(unsigned, 0x4001b00c)
#define TIMER4_SHUTDOWN                 _REG(unsigned, 0x4001b010)
#define TIMER4_CAPTURE                  _ARR(unsigned, 0x4001b040)
#define TIMER4_COMPARE                  _ARR(unsigned, 0x4001b140)
#define TIMER4_SHORTS                   _REG(unsigned, 0x4001b200)
#define TIMER4_INTENSET                 _REG(unsigned, 0x4001b304)
#define TIMER4_INTENCLR                 _REG(unsigned, 0x4001b308)
#define TIMER4_MODE                     _REG(unsigned, 0x4001b504)
#define TIMER4_BITMODE                  _REG(unsigned, 0x4001b508)
#define TIMER4_PRESCALER                _REG(unsigned, 0x4001b510)
#define TIMER4_CC                       _ARR(unsigned, 0x4001b540)


/* Random Number Generator */

/* Interrupts */
#define RNG_INT_VALRDY 0

#define RNG_BASE                        _BASE(0x4000d000)
/* Tasks */
#define RNG_START                       _REG(unsigned, 0x4000d000)
#define RNG_STOP                        _REG(unsigned, 0x4000d004)
/* Events */
#define RNG_VALRDY                      _REG(unsigned, 0x4000d100)
/* Registers */
#define RNG_SHORTS                      _REG(unsigned, 0x4000d200)
#define RNG_INTEN                       _REG(unsigned, 0x4000d300)
#define RNG_INTENSET                    _REG(unsigned, 0x4000d304)
#define RNG_INTENCLR                    _REG(unsigned, 0x4000d308)
#define RNG_CONFIG                      _REG(unsigned, 0x4000d504)
#define RNG_CONFIG_DERCEN __BIT(0)
#define RNG_VALUE                       _REG(unsigned, 0x4000d508)


/* Temperature sensor */

/* Interrupts */
#define TEMP_INT_DATARDY 0

#define TEMP_BASE                       _BASE(0x4000c000)
/* Tasks */
#define TEMP_START                      _REG(unsigned, 0x4000c000)
#define TEMP_STOP                       _REG(unsigned, 0x4000c004)
/* Events */
#define TEMP_DATARDY                    _REG(unsigned, 0x4000c100)
/* Registers */
#define TEMP_INTEN                      _REG(unsigned, 0x4000c300)
#define TEMP_INTENSET                   _REG(unsigned, 0x4000c304)
#define TEMP_INTENCLR                   _REG(unsigned, 0x4000c308)
#define TEMP_VALUE                      _REG(unsigned, 0x4000c508)


/* I2C */
#define N_I2C 2
#define I2C_INTERNAL 0
#define I2C_EXTERNAL 1


/* Interrupts */
#define I2C_INT_STOPPED 1
#define I2C_INT_RXDREADY 2
#define I2C_INT_TXDSENT 7
#define I2C_INT_ERROR 9
#define I2C_INT_BB 14
/* Shortcuts */
#define I2C_BB_SUSPEND 0
#define I2C_BB_STOP 1

#define I2C0_BASE                       _BASE(0x40003000)
/* Tasks */
#define I2C0_STARTRX                    _REG(unsigned, 0x40003000)
#define I2C0_STARTTX                    _REG(unsigned, 0x40003008)
#define I2C0_STOP                       _REG(unsigned, 0x40003014)
#define I2C0_SUSPEND                    _REG(unsigned, 0x4000301c)
#define I2C0_RESUME                     _REG(unsigned, 0x40003020)
/* Events */
#define I2C0_STOPPED                    _REG(unsigned, 0x40003104)
#define I2C0_RXDREADY                   _REG(unsigned, 0x40003108)
#define I2C0_TXDSENT                    _REG(unsigned, 0x4000311c)
#define I2C0_ERROR                      _REG(unsigned, 0x40003124)
#define I2C0_BB                         _REG(unsigned, 0x40003138)
#define I2C0_SUSPENDED                  _REG(unsigned, 0x40003148)
/* Registers */
#define I2C0_SHORTS                     _REG(unsigned, 0x40003200)
#define I2C0_INTEN                      _REG(unsigned, 0x40003300)
#define I2C0_INTENSET                   _REG(unsigned, 0x40003304)
#define I2C0_INTENCLR                   _REG(unsigned, 0x40003308)
#define I2C0_ERRORSRC                   _REG(unsigned, 0x400034c4)
#define   I2C_ERRORSRC_OVERRUN __BIT(0)
#define   I2C_ERRORSRC_ANACK __BIT(1)
#define   I2C_ERRORSRC_DNACK __BIT(2)
#define   I2C_ERRORSRC_All 0x7
#define I2C0_ENABLE                     _REG(unsigned, 0x40003500)
#define   I2C_ENABLE_Disabled 0
#define   I2C_ENABLE_Enabled 5
#define I2C0_PSELSCL                    _REG(unsigned, 0x40003508)
#define I2C0_PSELSDA                    _REG(unsigned, 0x4000350c)
#define I2C0_RXD                        _REG(unsigned, 0x40003518)
#define I2C0_TXD                        _REG(unsigned, 0x4000351c)
#define I2C0_FREQUENCY                  _REG(unsigned, 0x40003524)
#define   I2C_FREQUENCY_100kHz 0x01980000
#define I2C0_ADDRESS                    _REG(unsigned, 0x40003588)
#define I2C0_POWER                      _REG(unsigned, 0x40003ffc)

#define I2C1_BASE                       _BASE(0x40004000)
#define I2C1_STARTRX                    _REG(unsigned, 0x40004000)
#define I2C1_STARTTX                    _REG(unsigned, 0x40004008)
#define I2C1_STOP                       _REG(unsigned, 0x40004014)
#define I2C1_SUSPEND                    _REG(unsigned, 0x4000401c)
#define I2C1_RESUME                     _REG(unsigned, 0x40004020)
#define I2C1_STOPPED                    _REG(unsigned, 0x40004104)
#define I2C1_RXDREADY                   _REG(unsigned, 0x40004108)
#define I2C1_TXDSENT                    _REG(unsigned, 0x4000411c)
#define I2C1_ERROR                      _REG(unsigned, 0x40004124)
#define I2C1_BB                         _REG(unsigned, 0x40004138)
#define I2C1_SUSPENDED                  _REG(unsigned, 0x40004148)
#define I2C1_SHORTS                     _REG(unsigned, 0x40004200)
#define I2C1_INTEN                      _REG(unsigned, 0x40004300)
#define I2C1_INTENSET                   _REG(unsigned, 0x40004304)
#define I2C1_INTENCLR                   _REG(unsigned, 0x40004308)
#define I2C1_ERRORSRC                   _REG(unsigned, 0x400044c4)
#define I2C1_ENABLE                     _REG(unsigned, 0x40004500)
#define I2C1_PSELSCL                    _REG(unsigned, 0x40004508)
#define I2C1_PSELSDA                    _REG(unsigned, 0x4000450c)
#define I2C1_RXD                        _REG(unsigned, 0x40004518)
#define I2C1_TXD                        _REG(unsigned, 0x4000451c)
#define I2C1_FREQUENCY                  _REG(unsigned, 0x40004524)
#define I2C1_ADDRESS                    _REG(unsigned, 0x40004588)
#define I2C1_POWER                      _REG(unsigned, 0x40004ffc)


/* UART */

/* Interrupts */
#define UART_INT_RXDRDY 2
#define UART_INT_TXDRDY 7

#define UART_BASE                       _BASE(0x40002000)
/* Tasks */
#define UART_STARTRX                    _REG(unsigned, 0x40002000)
#define UART_STARTTX                    _REG(unsigned, 0x40002008)
/* Events */
#define UART_RXDRDY                     _REG(unsigned, 0x40002108)
#define UART_TXDRDY                     _REG(unsigned, 0x4000211c)
/* Registers */
#define UART_INTENSET                   _REG(unsigned, 0x40002304)
#define UART_INTENCLR                   _REG(unsigned, 0x40002308)
#define UART_ENABLE                     _REG(unsigned, 0x40002500)
#define   UART_ENABLE_Disabled 0
#define   UART_ENABLE_Enabled 4
#define UART_PSELTXD                    _REG(unsigned, 0x4000250c)
#define UART_PSELRXD                    _REG(unsigned, 0x40002514)
#define UART_RXD                        _REG(unsigned, 0x40002518)
#define UART_TXD                        _REG(unsigned, 0x4000251c)
#define UART_BAUDRATE                   _REG(unsigned, 0x40002524)
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
#define UART_CONFIG                     _REG(unsigned, 0x4000256c)
#define   UART_CONFIG_HWFC __BIT(0)
#define   UART_CONFIG_PARITY __FIELD(1, 3)
#define     UART_PARITY_None 0
#define     UART_PARITY_Even 7


/* UARTE -- UART with EasyDMA */

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

#define UARTE0_BASE                     _BASE(0x40002000)
/* Tasks */
#define UARTE0_STARTRX                  _REG(unsigned, 0x40002000)
#define UARTE0_STOPRX                   _REG(unsigned, 0x40002004)
#define UARTE0_STARTTX                  _REG(unsigned, 0x40002008)
#define UARTE0_STOPTX                   _REG(unsigned, 0x4000200c)
#define UARTE0_FLUSHRX                  _REG(unsigned, 0x4000202c)
/* Events */
#define UARTE0_CTS                      _REG(unsigned, 0x40002100)
#define UARTE0_NTCS                     _REG(unsigned, 0x40002104)
#define UARTE0_RXDRDY                   _REG(unsigned, 0x40002108)
#define UARTE0_ENDRX                    _REG(unsigned, 0x40002110)
#define UARTE0_TXDRDY                   _REG(unsigned, 0x4000211c)
#define UARTE0_ENDTX                    _REG(unsigned, 0x40002120)
#define UARTE0_ERROR                    _REG(unsigned, 0x40002124)
#define UARTE0_RXTO                     _REG(unsigned, 0x40002144)
#define UARTE0_RXSTARTED                _REG(unsigned, 0x4000214c)
#define UARTE0_TXSTARTED                _REG(unsigned, 0x40002150)
#define UARTE0_TXSTOPPED                _REG(unsigned, 0x40002158)
/* Registers */
#define UARTE0_SHORTS                   _REG(unsigned, 0x40002200)
#define UARTE0_INTEN                    _REG(unsigned, 0x40002300)
#define UARTE0_INTENSET                 _REG(unsigned, 0x40002304)
#define UARTE0_INTENCLR                 _REG(unsigned, 0x40002308)
#define UARTE0_ERRORSRC                 _REG(unsigned, 0x40002480)
#define   UARTE_ERROR_OVERRUN __BIT(0)
#define   UARTE_ERROR_PARITY __BIT(1)
#define   UARTE_ERROR_FRAMING __BIT(2)
#define   UARTE_ERROR_BREAK __BIT(3)
#define UARTE0_ENABLE                   _REG(unsigned, 0x40002500)
#define   UARTE_ENABLE_Disabled 0
#define   UARTE_ENABLE_Enabled 8
#define UARTE0_PSELRTS                  _REG(unsigned, 0x40002508)
#define UARTE0_PSELTXD                  _REG(unsigned, 0x4000250c)
#define UARTE0_PSELCTS                  _REG(unsigned, 0x40002510)
#define UARTE0_PSELRXD                  _REG(unsigned, 0x40002514)
#define UARTE0_BAUDRATE                 _REG(unsigned, 0x40002524)
#define   UARTE_BAUDRATE_1200   0x0004f000
#define   UARTE_BAUDRATE_2400   0x0009d000
#define   UARTE_BAUDRATE_4800   0x0013b000
#define   UARTE_BAUDRATE_9600   0x00275000
#define   UARTE_BAUDRATE_14400  0x003af000
#define   UARTE_BAUDRATE_19200  0x004ea000
#define   UARTE_BAUDRATE_28800  0x0075c000
#define   UARTE_BAUDRATE_31250  0x00800000
#define   UARTE_BAUDRATE_38400  0x009d0000
#define   UARTE_BAUDRATE_56000  0x00e50000
#define   UARTE_BAUDRATE_57600  0x00eb0000
#define   UARTE_BAUDRATE_76800  0x013a9000
#define   UARTE_BAUDRATE_115200 0x01d60000
#define   UARTE_BAUDRATE_230400 0x03b00000
#define   UARTE_BAUDRATE_250000 0x04000000
#define   UARTE_BAUDRATE_460800 0x07400000
#define   UARTE_BAUDRATE_921600 0x0f000000
#define   UARTE_BAUDRATE_1M     0x10000000
#define UARTE0_RXD                      _REG(dma_param, 0x40002534)
#define UARTE0_TXD                      _REG(dma_param, 0x40002544)
#define UARTE0_CONFIG                   _REG(unsigned, 0x4000256c)
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
#define UARTE1_BASE                     _BASE(0x40028000)
#define UARTE1_STARTRX                  _REG(unsigned, 0x40028000)
#define UARTE1_STOPRX                   _REG(unsigned, 0x40028004)
#define UARTE1_STARTTX                  _REG(unsigned, 0x40028008)
#define UARTE1_STOPTX                   _REG(unsigned, 0x4002800c)
#define UARTE1_FLUSHRX                  _REG(unsigned, 0x4002802c)
#define UARTE1_CTS                      _REG(unsigned, 0x40028100)
#define UARTE1_NTCS                     _REG(unsigned, 0x40028104)
#define UARTE1_RXDRDY                   _REG(unsigned, 0x40028108)
#define UARTE1_ENDRX                    _REG(unsigned, 0x40028110)
#define UARTE1_TXDRDY                   _REG(unsigned, 0x4002811c)
#define UARTE1_ENDTX                    _REG(unsigned, 0x40028120)
#define UARTE1_ERROR                    _REG(unsigned, 0x40028124)
#define UARTE1_RXTO                     _REG(unsigned, 0x40028144)
#define UARTE1_RXSTARTED                _REG(unsigned, 0x4002814c)
#define UARTE1_TXSTARTED                _REG(unsigned, 0x40028150)
#define UARTE1_TXSTOPPED                _REG(unsigned, 0x40028158)
#define UARTE1_SHORTS                   _REG(unsigned, 0x40028200)
#define UARTE1_INTEN                    _REG(unsigned, 0x40028300)
#define UARTE1_INTENSET                 _REG(unsigned, 0x40028304)
#define UARTE1_INTENCLR                 _REG(unsigned, 0x40028308)
#define UARTE1_ERRORSRC                 _REG(unsigned, 0x40028480)
#define UARTE1_ENABLE                   _REG(unsigned, 0x40028500)
#define UARTE1_PSELRTS                  _REG(unsigned, 0x40028508)
#define UARTE1_PSELTXD                  _REG(unsigned, 0x4002850c)
#define UARTE1_PSELCTS                  _REG(unsigned, 0x40028510)
#define UARTE1_PSELRXD                  _REG(unsigned, 0x40028514)
#define UARTE1_BAUDRATE                 _REG(unsigned, 0x40028524)
#define UARTE1_RXD                      _REG(dma_param, 0x40028534)
#define UARTE1_TXD                      _REG(dma_param, 0x40028544)
#define UARTE1_CONFIG                   _REG(unsigned, 0x4002856c)


/* SAADC -- Successive approximation ADC */
typedef struct {
    unsigned PSELP;
    unsigned PSELN;
    unsigned CONFIG;
    unsigned LIMIT;
} adc_chan;


/* Interrupts */
#define ADC_INT_STARTED 0
#define ADC_INT_END 1
#define ADC_INT_DONE 2
#define ADC_INT_RESULTDONE 3
#define ADC_INT_CALDONE 4
#define ADC_INT_STOPPED 5

#define ADC_BASE                        _BASE(0x40007000)
/* Tasks */
#define ADC_START                       _REG(unsigned, 0x40007000)
#define ADC_SAMPLE                      _REG(unsigned, 0x40007004)
#define ADC_STOP                        _REG(unsigned, 0x40007008)
#define ADC_CALIBRATE                   _REG(unsigned, 0x4000700c)
/* Events */
#define ADC_STARTED                     _REG(unsigned, 0x40007100)
#define ADC_END                         _REG(unsigned, 0x40007104)
#define ADC_DONE                        _REG(unsigned, 0x40007108)
#define ADC_RESULTDONE                  _REG(unsigned, 0x4000710c)
#define ADC_CALDONE                     _REG(unsigned, 0x40007110)
#define ADC_STOPPED                     _REG(unsigned, 0x40007114)
/* Registers */
#define ADC_INTEN                       _REG(unsigned, 0x40007300)
#define ADC_INTENSET                    _REG(unsigned, 0x40007304)
#define ADC_INTENCLR                    _REG(unsigned, 0x40007308)
#define ADC_BUSY                        _REG(unsigned, 0x40007400)
#define ADC_ENABLE                      _REG(unsigned, 0x40007500)
#define ADC_CHAN                        _ARR(adc_chan, 0x40007510)
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
#define ADC_RESOLUTION                  _REG(unsigned, 0x400075f0)
#define   ADC_RESOLUTION_8bit 0
#define   ADC_RESOLUTION_10bit 1
#define   ADC_RESOLUTION_12bit 2
#define   ADC_RESOLUTION_14bit 3
#define ADC_OVERSAMPLE                  _REG(unsigned, 0x400075f4)
#define ADC_SAMPLERATE                  _REG(unsigned, 0x400075f8)
#define ADC_RESULT                      _REG(dma_param, 0x4000762c)


/* PWM */
typedef struct {
    void *PTR;
    unsigned CNT;
    unsigned REFRESH;
    unsigned ENDDELAY;
    unsigned char filler[16];
} pwm_sequence;


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

#define PWM0_BASE                       _BASE(0x4001c000)
/* Tasks */
#define PWM0_STOP                       _REG(unsigned, 0x4001c004)
#define PWM0_SEQSTART                   _ARR(unsigned, 0x4001c008)
#define PWM0_NEXTSTEP                   _REG(unsigned, 0x4001c010)
/* Events */
#define PWM0_STOPPED                    _REG(unsigned, 0x4001c104)
#define PWM0_SEQSTARTED                 _ARR(unsigned, 0x4001c108)
#define PWM0_SEQEND                     _ARR(unsigned, 0x4001c110)
#define PWM0_PWMPERIODEND               _REG(unsigned, 0x4001c118)
#define PWM0_LOOPSDONE                  _REG(unsigned, 0x4001c11c)
/* Registers */
#define PWM0_SHORTS                     _REG(unsigned, 0x4001c200)
#define PWM0_INTEN                      _REG(unsigned, 0x4001c300)
#define PWM0_INTENSET                   _REG(unsigned, 0x4001c304)
#define PWM0_INTENCLR                   _REG(unsigned, 0x4001c308)
#define PWM0_ENABLE                     _REG(unsigned, 0x4001c500)
#define PWM0_MODE                       _REG(unsigned, 0x4001c504)
#define   PWM_MODE_Up 0
#define   PWM_MODE_UpAndDown 1
#define PWM0_COUNTERTOP                 _REG(unsigned, 0x4001c508)
#define PWM0_PRESCALER                  _REG(unsigned, 0x4001c50c)
#define PWM0_DECODER                    _REG(unsigned, 0x4001c510)
#define   PWM_DECODER_LOAD __FIELD(0, 2)
#define     PWM_LOAD_Common 0
#define     PWM_LOAD_Grouped 1
#define     PWM_LOAD_Individual 2
#define     PWM_LOAD_Waveform 3
#define   PWM_DECODER_MODE __FIELD(8, 1)
#define     PWM_MODE_RefreshCount 0
#define     PWM_MODE_NextStep 1
#define PWM0_LOOP                       _REG(unsigned, 0x4001c514)
#define PWM0_SEQ                        _ARR(pwm_sequence, 0x4001c520)
#define   PWM_SEQ_COMPARE __FIELD(0, 15)
#define   PWM_SEQ_POLARITY __FIELD(15, 1)
#define     PWM_POLARITY_RisingEdge 0
#define     PWM_POLARITY_FallingEdge 1
#define PWM0_PSEL                       _ARR(unsigned, 0x4001c560)
#define PWM1_BASE                       _BASE(0x40021000)
#define PWM1_STOP                       _REG(unsigned, 0x40021004)
#define PWM1_SEQSTART                   _ARR(unsigned, 0x40021008)
#define PWM1_NEXTSTEP                   _REG(unsigned, 0x40021010)
#define PWM1_STOPPED                    _REG(unsigned, 0x40021104)
#define PWM1_SEQSTARTED                 _ARR(unsigned, 0x40021108)
#define PWM1_SEQEND                     _ARR(unsigned, 0x40021110)
#define PWM1_PWMPERIODEND               _REG(unsigned, 0x40021118)
#define PWM1_LOOPSDONE                  _REG(unsigned, 0x4002111c)
#define PWM1_SHORTS                     _REG(unsigned, 0x40021200)
#define PWM1_INTEN                      _REG(unsigned, 0x40021300)
#define PWM1_INTENSET                   _REG(unsigned, 0x40021304)
#define PWM1_INTENCLR                   _REG(unsigned, 0x40021308)
#define PWM1_ENABLE                     _REG(unsigned, 0x40021500)
#define PWM1_MODE                       _REG(unsigned, 0x40021504)
#define PWM1_COUNTERTOP                 _REG(unsigned, 0x40021508)
#define PWM1_PRESCALER                  _REG(unsigned, 0x4002150c)
#define PWM1_DECODER                    _REG(unsigned, 0x40021510)
#define PWM1_LOOP                       _REG(unsigned, 0x40021514)
#define PWM1_SEQ                        _ARR(pwm_sequence, 0x40021520)
#define PWM1_PSEL                       _ARR(unsigned, 0x40021560)
#define PWM2_BASE                       _BASE(0x40022000)
#define PWM2_STOP                       _REG(unsigned, 0x40022004)
#define PWM2_SEQSTART                   _ARR(unsigned, 0x40022008)
#define PWM2_NEXTSTEP                   _REG(unsigned, 0x40022010)
#define PWM2_STOPPED                    _REG(unsigned, 0x40022104)
#define PWM2_SEQSTARTED                 _ARR(unsigned, 0x40022108)
#define PWM2_SEQEND                     _ARR(unsigned, 0x40022110)
#define PWM2_PWMPERIODEND               _REG(unsigned, 0x40022118)
#define PWM2_LOOPSDONE                  _REG(unsigned, 0x4002211c)
#define PWM2_SHORTS                     _REG(unsigned, 0x40022200)
#define PWM2_INTEN                      _REG(unsigned, 0x40022300)
#define PWM2_INTENSET                   _REG(unsigned, 0x40022304)
#define PWM2_INTENCLR                   _REG(unsigned, 0x40022308)
#define PWM2_ENABLE                     _REG(unsigned, 0x40022500)
#define PWM2_MODE                       _REG(unsigned, 0x40022504)
#define PWM2_COUNTERTOP                 _REG(unsigned, 0x40022508)
#define PWM2_PRESCALER                  _REG(unsigned, 0x4002250c)
#define PWM2_DECODER                    _REG(unsigned, 0x40022510)
#define PWM2_LOOP                       _REG(unsigned, 0x40022514)
#define PWM2_SEQ                        _ARR(pwm_sequence, 0x40022520)
#define PWM2_PSEL                       _ARR(unsigned, 0x40022560)
#define PWM3_BASE                       _BASE(0x4002d000)
#define PWM3_STOP                       _REG(unsigned, 0x4002d004)
#define PWM3_SEQSTART                   _ARR(unsigned, 0x4002d008)
#define PWM3_NEXTSTEP                   _REG(unsigned, 0x4002d010)
#define PWM3_STOPPED                    _REG(unsigned, 0x4002d104)
#define PWM3_SEQSTARTED                 _ARR(unsigned, 0x4002d108)
#define PWM3_SEQEND                     _ARR(unsigned, 0x4002d110)
#define PWM3_PWMPERIODEND               _REG(unsigned, 0x4002d118)
#define PWM3_LOOPSDONE                  _REG(unsigned, 0x4002d11c)
#define PWM3_SHORTS                     _REG(unsigned, 0x4002d200)
#define PWM3_INTEN                      _REG(unsigned, 0x4002d300)
#define PWM3_INTENSET                   _REG(unsigned, 0x4002d304)
#define PWM3_INTENCLR                   _REG(unsigned, 0x4002d308)
#define PWM3_ENABLE                     _REG(unsigned, 0x4002d500)
#define PWM3_MODE                       _REG(unsigned, 0x4002d504)
#define PWM3_COUNTERTOP                 _REG(unsigned, 0x4002d508)
#define PWM3_PRESCALER                  _REG(unsigned, 0x4002d50c)
#define PWM3_DECODER                    _REG(unsigned, 0x4002d510)
#define PWM3_LOOP                       _REG(unsigned, 0x4002d514)
#define PWM3_SEQ                        _ARR(pwm_sequence, 0x4002d520)
#define PWM3_PSEL                       _ARR(unsigned, 0x4002d560)


/* NVIC stuff */

/* irq_priority -- set priority of an IRQ from 0 (highest) to 255 */
void irq_priority(int irq, unsigned priority);

/* enable_irq -- enable interrupts from an IRQ */
#define enable_irq(irq)  NVIC_ISER[0] = BIT(irq)

/* disable_irq -- disable interrupts from a specific IRQ */
#define disable_irq(irq)  NVIC_ICER[0] = BIT(irq)

/* clear_pending -- clear pending interrupt from an IRQ */
#define clear_pending(irq)  NVIC_ICPR[0] = BIT(irq)

/* reschedule -- request PendSV interrupt */
#define reschedule()  SCB_ICSR = BIT(SCB_ICSR_PENDSVSET)

/* active_irq -- find active interrupt: returns -16 to 31 */
#define active_irq()  (GET_FIELD(SCB_ICSR, SCB_ICSR_VECTACTIVE) - 16)

/* delay_loop -- timed delay */
void delay_loop(unsigned usec);


/* GPIO convenience */

#ifndef INLINE
#define INLINE inline
#endif

/* Some C language tricks are needed for us to refer to the two GPIO
   ports uniformly. */

extern unsigned volatile * const gpio_base[2];

#define _GPIO_REG(port, reg) \
    (* (gpio_base[port] + (&GPIO0_##reg - GPIO0_BASE)))

#define _GPIO_PINCNF(pin) \
    ((gpio_base[PORT(pin)] + (GPIO0_PINCNF - GPIO0_BASE))[PIN(pin)])

/* gpio_dir -- set GPIO direction */
INLINE void gpio_dir(unsigned pin, unsigned dir) {
    if (dir)
        _GPIO_REG(PORT(pin), DIRSET) = BIT(PIN(pin));
    else
        _GPIO_REG(PORT(pin), DIRCLR) = BIT(PIN(pin));
}

/* gpio_connect -- connect pin for input */
INLINE void gpio_connect(unsigned pin) {
    SET_FIELD(_GPIO_PINCNF(pin), GPIO_PINCNF_INPUT, GPIO_INPUT_Connect);
}

/* gpio_drive -- set GPIO drive strength */
INLINE void gpio_drive(unsigned pin, unsigned mode) {
    SET_FIELD(_GPIO_PINCNF(pin), GPIO_PINCNF_DRIVE, mode);
}

/* gpio_out -- set GPIO output value */
INLINE void gpio_out(unsigned pin, unsigned value) {
    if (value)
        _GPIO_REG(PORT(pin), OUTSET) = BIT(PIN(pin));
    else
        _GPIO_REG(PORT(pin), OUTCLR) = BIT(PIN(pin));
}

/* gpio_in -- get GPIO input bit */
INLINE unsigned gpio_in(unsigned pin) {
    return GET_BIT(_GPIO_REG(PORT(pin), IN), PIN(pin));
}


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

#define led_init()  GPIO0_DIRSET = LED_MASK0, GPIO1_DIRSET = LED_MASK1
#define led_dot()   GPIO0_OUTSET = LED_DOT0, GPIO1_OUTSET = LED_DOT1
#define led_off()   GPIO0_OUTCLR = LED_MASK0, GPIO1_OUTCLR = LED_MASK1


/* CODERAM -- mark function for copying to RAM */
#define CODERAM  __attribute((noinline, section(".xram")))

/* A few assembler macros for single instructions. */
#define intr_disable()  asm volatile ("cpsid i")
#define intr_enable()   asm volatile ("cpsie i")
#define get_primask()   ({ unsigned x;                                   \
                           asm volatile ("mrs %0, primask" : "=r"(x)); x; })
#define set_primask(x)  asm volatile ("msr primask, %0" : : "r"(x))
#define nop()           asm volatile ("nop")
#define syscall(op)     asm volatile ("svc %0" : : "i"(op))

/* pause() -- disabled on V2 owing to long wakeup time */
#define pause()         /* asm volatile ("wfe") */
