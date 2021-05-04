/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <stdio.h>
#include <string.h>
#include "./include/devices/gpio.h"
#include "include/cpu_freq.h"
#include "include/devices/aon.h"

#define PIN		    3
#define RTC_FREQ    32768

void handler();

int taken = 0;

int main (void)
{
    int rc;

    set_freq_320MHz();

    clint_interrupt_enable();
    interrupt_enable();

    AON_REG(AON_RTCCFG) |= AON_RTCCFG_ENALWAYS;

    CLINT_REG(CLINT_MTIMECMP) = CLINT_REG(CLINT_MTIME) + RTC_FREQ;

    while (1)
    {

	}

    return 0;
}

void clint_timer_interrupt_handler()
{
    printf("Interrupt taken\n");
    taken = 1;
}

//asm(".global _printf_float");
