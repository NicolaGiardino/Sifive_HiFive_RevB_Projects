/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <stdio.h>
#include <metal/cpu.h>
#include <metal/machine.h>
#include <metal/clock.h>
#include "include/devices/spi.h"
#include "include/platform.h"

#ifndef RTC_FREQ
#define RTC_FREQ    32768
#endif


#define CS_ESP01	2

struct metal_cpu *cpu;
struct metal_interrupt *cpu_intr, *tmr_intr;
int tmr_id;
volatile uint32_t timer_isr_flag;


void timer_isr (int id, void *data) {

    // Disable Timer interrupt
    metal_interrupt_disable(tmr_intr, tmr_id);

    // Flag showing we hit timer isr
    timer_isr_flag = 1;
}

void wait_for_timer() {

    // clear global timer isr flag
    timer_isr_flag = 0;

    // Set timer
    metal_cpu_set_mtimecmp(cpu, (metal_cpu_get_mtime(cpu) + RTC_FREQ));
    // Enable Timer interrupt
    metal_interrupt_enable(tmr_intr, tmr_id);

    // wait till timer triggers and isr is hit
    while (timer_isr_flag == 0){};

    timer_isr_flag = 0;
}

int main (void)
{
    int rc, push;
    struct metal_led *led0_red, *led0_green, *led0_blue;


    /* This code
 		wait_for_timer(); is used to initialize the cpu and its interrupts */
    cpu = metal_cpu_get(metal_cpu_get_current_hartid());
    if (cpu == NULL) {
        printf("CPU null.\n");
        return 2;
    }
    cpu_intr = metal_cpu_interrupt_controller(cpu);
    if (cpu_intr == NULL) {
        printf("CPU interrupt controller is null.\n");
        return 3;
    }
    metal_interrupt_init(cpu_intr);


    // Setup Timer and its interrupt so we can toggle LEDs on 1s cadence
    tmr_intr = metal_cpu_timer_interrupt_controller(cpu);
    if (tmr_intr == NULL) {
        printf("TIMER interrupt controller is  null.\n");
        return 4;
    }
    metal_interrupt_init(tmr_intr);
    tmr_id = metal_cpu_timer_get_interrupt_id(cpu);
    rc = metal_interrupt_register_handler(tmr_intr, tmr_id, timer_isr, cpu);
    if (rc < 0) {
        printf("TIMER interrupt handler registration failed\n");
        return (rc * -1);
    }

    // Lastly CPU interrupt
    if (metal_interrupt_enable(cpu_intr, 0) == -1) {
        printf("CPU interrupt enable failed\n");
        return 6;
    }

    struct spi s;
    struct spi_config config = {
    		80000,
			0,
			0,
			0,
			0,
			0,
			8,
    };

    rc = spi_init(&s, 1, config);
    if(rc)
    {
    	printf("SPI Init error\n");
    	return 7;
    }



	while(1)
	{



	}

    return 0;
}


//asm(".global _printf_float");
