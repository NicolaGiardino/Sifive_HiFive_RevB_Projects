/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <stdio.h>
#include <string.h>
#include "include/devices/gpio.h"
#include "include/cpu_freq.h"
#include "include/devices/spi.h"

#define PIN		    7
#define RTC_FREQ    32768

void handler();
void wait_timer();
int taken = 0;


int main (void)
{

    set_freq_320MHz();

    int rc;
    struct spi s;

    struct spi_config config =
    {
        8,
        80000,
        0,
        0,
        0,
        0,
        0
    };

    while (1)
    {
        
    }

    return 0;
}

void wait_timer()
{
	if(taken)
	{
		GPIO_REG(GPIO_OUTPUT_VAL) ^= (1 << variant_pin_map[5].bit_pos);
	    taken = 0;
	}
}

void clint_timer_interrupt_handler()
{
    //rtc_next_wake_time(2 * RTC_FREQ);
    taken = 1;
}

void handler()
{
	taken = GPIO_REG(GPIO_FALL_IP);
	GPIO_REG(GPIO_FALL_IP) = taken;
}

//asm(".global _printf_float");
