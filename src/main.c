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

    interrupt_enable();
    clint_interrupt_timer_enable();

    rtc_enable();

    GPIO_REG(GPIO_OUTPUT_EN) |= (1 << variant_pin_map[3].bit_pos);

    rtc_next_wake_time(RTC_FREQ);

    while (1)
    {
    	if(taken)
    	{
    		GPIO_REG(GPIO_OUTPUT_VAL) ^= (1 << variant_pin_map[3].bit_pos);
    		taken = 0;
    	}
	}

    return 0;
}

void clint_timer_interrupt_handler()
{
    rtc_next_wake_time(2 * RTC_FREQ);
    taken = 1;
}

//asm(".global _printf_float");
