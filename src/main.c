/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <stdio.h>
#include <string.h>
#include "./include/devices/gpio.h"
#include "./include/interrupt.h"
#include "./include/cpu_freq.h"

#define PIN		    7

void handler();

int main (void)
{
    int rc;

    set_freq_320MHz();

    rc = interrupt_enable();
    gpio_interrupt_enable(PIN, handler, 5);

    rc = gpio_init(PIN, OUTPUT);

    while (1)
    {
        
	}

    return 0;
}

void handler()
{
    printf("Interrupt taken");
}

//asm(".global _printf_float");
