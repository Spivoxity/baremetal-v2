/* common/ubit-v2/device.ld */
/* Copyright (c) 2018-20 J. M. Spivey */

MEMORY {
    FLASH   (rx)  : ORIGIN = 0x00000000,      LENGTH = 512K
    RAM     (rwx) : ORIGIN = 0x20000000,      LENGTH = 124K
    CODERAM (rwx) : ORIGIN = 0x00800000+124K, LENGTH = 4K
}

OUTPUT_FORMAT ("elf32-littlearm")

/* Setting the entry address helps GDB to find the stack bottom */
ENTRY(__reset)

SECTIONS {
    .text ALIGN(16) : SUBALIGN(16) {
        KEEP(*(.vectors))
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
        __etext = .;
    } > FLASH

    .xram : AT (LOADADDR(.text) + SIZEOF(.text)) {
        __xram_start = .;
        *(.xram*)
        . = ALIGN(4);
        __xram_end = .;
    } > CODERAM

    /DISCARD/ : {
        /* Some parts of the GCC library have C++ exception data,
           which is of no use to us. */
        *(.ARM.extab)
        *(.ARM.exidx)
        *(.eh_frame*)
    }

    .data : AT (LOADADDR(.xram) + SIZEOF(.xram)) {
        __data_start = .;
        *(.data*)
        . = ALIGN(4);
        __data_end = .;
    } > RAM

    .bss : {
        __bss_start = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end = .;
        __end = .;
    } > RAM

    /* Set stack top to end of RAM, and move stack limit down by
       size of stack */
    __stack = ORIGIN(RAM) + LENGTH(RAM);
    __stack_limit = __stack - 2048;

    /* Check if data + stack exceeds RAM limit */
    ASSERT(__stack_limit >= __end, "region RAM overlaps with stack")
}

/* Interrupt handlers are aliases for default_handler if not
   otherwise defined */
PROVIDE(nmi_handler = default_handler);
PROVIDE(hardfault_handler = default_handler);
PROVIDE(svc_handler = default_handler);
PROVIDE(debugmon_handler = default_handler);
PROVIDE(pendsv_handler = default_handler);
PROVIDE(systick_handler = default_handler);
PROVIDE(power_clock_handler = default_handler);
PROVIDE(radio_handler = default_handler);
PROVIDE(uart0_handler = default_handler);
PROVIDE(i2c0_spi0_handler = default_handler);
PROVIDE(i2c1_spi1_handler = default_handler);
PROVIDE(nfct_handler = default_handler);
PROVIDE(gpiote_handler = default_handler);
PROVIDE(adc_handler = default_handler);
PROVIDE(timer0_handler = default_handler);
PROVIDE(timer1_handler = default_handler);
PROVIDE(timer2_handler = default_handler);
PROVIDE(rtc0_handler = default_handler);
PROVIDE(temp_handler = default_handler);
PROVIDE(rng_handler = default_handler);
PROVIDE(ecb_handler = default_handler);
PROVIDE(ccm_aar_handler = default_handler);
PROVIDE(wdt_handler = default_handler);
PROVIDE(rtc1_handler = default_handler);
PROVIDE(qdec_handler = default_handler);
PROVIDE(lpcomp_handler = default_handler);
PROVIDE(swi0_handler = default_handler);
PROVIDE(swi1_handler = default_handler);
PROVIDE(swi2_handler = default_handler);
PROVIDE(swi3_handler = default_handler);
PROVIDE(swi4_handler = default_handler);
PROVIDE(swi5_handler = default_handler);
PROVIDE(timer3_handler = default_handler);
PROVIDE(timer4_handler = default_handler);
PROVIDE(pwm0_handler = default_handler);
PROVIDE(pdm_handler = default_handler);
PROVIDE(mwu_handler = default_handler);
PROVIDE(pwm1_handler = default_handler);
PROVIDE(pwm2_handler = default_handler);
PROVIDE(spi2_handler = default_handler);
PROVIDE(rtc2_handler = default_handler);
PROVIDE(i2s_handler = default_handler);
PROVIDE(fpu_handler = default_handler);
PROVIDE(usbd_handler = default_handler);
PROVIDE(uart1_handler = default_handler);
PROVIDE(pwm3_handler = default_handler);
PROVIDE(spi3_handler = default_handler);

/* Addresses for hardware devices */
FICR    = 0x10000000;
UICR    = 0x10001000;
SCB     = 0xe000ed00;
DEBUG   = 0xe000ed00;
NVIC    = 0xe000e000;
SYSTICK = 0xe000e000;
DWT     = 0xe0001000;
POWER   = 0x40000000;
CLOCK   = 0x40000000;
MPU     = 0x40000000;
RADIO   = 0x40001000;
UART    = 0x40002000;
UARTE0  = 0x40002000;
I2C0    = 0x40003000;
SPI0    = 0x40003000;
SPIM0   = 0x40003000;
I2C1    = 0x40004000;
SPI1    = 0x40004000;
SPIM1   = 0x40004000;
NFCT    = 0x40005000;
GPIOTE  = 0x40006000;
ADC     = 0x40007000;
TIMER0  = 0x40008000;
TIMER1  = 0x40009000;
TIMER2  = 0x4000a000;
RTC0    = 0x4000b000;
TEMP    = 0x4000c000;
RNG     = 0x4000d000;
ECB     = 0x4000e000;
CCM     = 0x4000f000;
AAR     = 0x4000f000;
WDT     = 0x40010000;
TIMER3  = 0x4001a000;
TIMER4  = 0x4001b000;
PWM0    = 0x4001c000;
PDM     = 0x4001d000;
NVMC    = 0x4001e000;
PPI     = 0x4001f000;
MWU     = 0x40020000;
PWM1    = 0x40021000;
PWM2    = 0x40022000;
SPI2    = 0x40023000;
SPIM2   = 0x40023000;
RTC2    = 0x40024000;
I2S     = 0x40025000;
UARTE1  = 0x40028000;
PWM3    = 0x4002d000;
SPIM3   = 0x4002f000;
GPIO0   = 0x50000500;
GPIO1   = 0x50000800;
