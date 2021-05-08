/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <stdio.h>
#include <string.h>
#include "./include/devices/gpio.h"
#include "include/cpu_freq.h"
#include "include/devices/aon.h"

#define PIN		    7
#define RTC_FREQ    32768

void handler();
void wait_timer();
int taken = 0;
int taken2 = 0;


int main (void)
{
    int rc;

    set_freq_320MHz();
    interrupt_enable();
    clint_interrupt_timer_enable();
    plic_interrupt_enable();
    //rtc_enable();

    //GPIO_REG(GPIO_OUTPUT_EN) |= (1 << variant_pin_map[3].bit_pos);
    GPIO_REG(GPIO_OUTPUT_EN) |= (1 << variant_pin_map[5].bit_pos);
    GPIO_REG(GPIO_OUTPUT_EN) |= (1 << variant_pin_map[3].bit_pos);
    GPIO_REG(GPIO_INPUT_EN)  |= (1 << variant_pin_map[PIN].bit_pos);

    unsigned int gpio = variant_pin_map[PIN].bit_pos;
    unsigned int gpio2 = variant_pin_map[9].bit_pos;


    GPIO_REG(GPIO_INPUT_EN)  |= (1 << gpio2);

    irq_functions[gpio + IRQ_GPIO - 1].active = 1;
    irq_functions[gpio + IRQ_GPIO - 1].priority = 7;
    irq_functions[gpio + IRQ_GPIO - 1].irq_handler = handler;

    PLIC_REG(PLIC_ENABLE_OFFSET) |= (1 << (IRQ_GPIO + gpio));
    PLIC_REG(PLIC_PRIORITY_OFFSET + 4 * (IRQ_GPIO + gpio)) = 7;
	GPIO_REG(GPIO_FALL_IE) |= (1 << gpio);

    rtc_next_wake_time(RTC_FREQ);

    while (1)
    {
    	if(taken)
    	{
    		GPIO_REG(GPIO_OUTPUT_VAL) ^= (1 << variant_pin_map[3].bit_pos);
    		taken = 0;
    		//plic_interrupt_enable();
    		//interrupt_enable();
    		PLIC_REG(PLIC_ENABLE_OFFSET) |= (1 << (IRQ_GPIO + gpio));
    		PLIC_REG(PLIC_PRIORITY_OFFSET + 4 * (IRQ_GPIO + gpio)) = 7;
    	    GPIO_REG(GPIO_INPUT_EN)  |= (1 << variant_pin_map[PIN].bit_pos);
    		GPIO_REG(GPIO_FALL_IE) |= (1 << gpio);
    	}
    	wait_timer();
    	/*if(taken2)
    	{
    		GPIO_REG(GPIO_OUTPUT_VAL) ^= (1 << variant_pin_map[3].bit_pos);
    		taken2 = 0;
    		interrupt_enable();
    		plic_interrupt_enable();
    		PLIC_REG(PLIC_ENABLE_OFFSET) |= (1 << (IRQ_GPIO + gpio2));
    		PLIC_REG(PLIC_PRIORITY_OFFSET + 4 * (IRQ_GPIO + gpio2)) = 7;
    		GPIO_REG(GPIO_FALL_IE) |= (1 << gpio2);
    	}*/
    }

    return 0;
}

void wait_timer()
{
	if(taken2)
	{
		GPIO_REG(GPIO_OUTPUT_VAL) ^= (1 << variant_pin_map[5].bit_pos);
	    taken2 = 0;
	}
}

void clint_timer_interrupt_handler()
{
    rtc_next_wake_time(2 * RTC_FREQ);
    taken2 = 1;
}

void handler()
{
	GPIO_REG(GPIO_FALL_IE) &= ~(1 << 23);
    GPIO_REG(GPIO_INPUT_EN)  &= ~(1 << 23);
    PLIC_REG(PLIC_ENABLE_OFFSET) &= ~(1 << (IRQ_GPIO + 23));
	taken = GPIO_REG(GPIO_FALL_IP);
	GPIO_REG(GPIO_FALL_IP) = taken;
    PLIC_REG(PLIC_ENABLE_OFFSET) |= (1 << (IRQ_GPIO + 23));
}

//asm(".global _printf_float");
