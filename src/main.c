/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <stdio.h>
#include <string.h>
#include "./include/devices/gpio.h"
#include "include/cpu_freq.h"

#define PIN		    3

void handler();

int main (void)
{
    int rc;

    set_freq_320MHz();

    rc = interrupt_enable();

    gpio_interrupt_enable(PIN, handler, 5, GPIO_FALL_EN);

    rc = gpio_init_input(PIN, GPIO_PUP_EN);

    for(int i = 0; i < 100000; i++)
    	;

    __asm__ volatile ("wfi");

    while (1)
    {
        
	}

    return 0;
}

void handler()
{
    printf("Interrupt taken");
    uint32_t in = GPIO_REG(GPIO_FALL_IP);

    GPIO_REG(GPIO_FALL_IP) = in;
}

//asm(".global _printf_float");
