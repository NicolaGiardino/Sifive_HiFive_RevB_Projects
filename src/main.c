/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <stdio.h>
#include <metal/cpu.h>
#include <metal/machine.h>
#include <metal/clock.h>
#include "include/Servo.h"

#ifndef RTC_FREQ
#define RTC_FREQ    32768
#endif


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

    //This is for HCSR04
    int echo = 7;
    int trigger = 6;
    int cpu_freq = 16000000;

    int echo_pin    = variant_pin_map[echo].bit_pos;
    int trigger_pin = variant_pin_map[trigger].bit_pos;

    GPIO_REG(GPIO_INPUT_EN)     &= ~(1 << trigger_pin);
    GPIO_REG(GPIO_OUTPUT_EN)    |= 1 << trigger_pin;
    GPIO_REG(GPIO_PULLUP_EN)    &= ~(1 << trigger_pin);
    GPIO_REG(GPIO_IOF_EN)       |= 1 << trigger_pin;
    GPIO_REG(GPIO_IOF_SEL)      |= 1 << trigger_pin; //Selecting IOF1 for PMW
    GPIO_REG(GPIO_OUTPUT_XOR)   |= 1 << trigger_pin;

    GPIO_REG(GPIO_INPUT_EN)     |= 1 << echo_pin;
    GPIO_REG(GPIO_OUTPUT_EN)    &= ~(1 << echo_pin);
    GPIO_REG(GPIO_PULLUP_EN)    &= ~(1 << echo_pin);
    GPIO_REG(GPIO_IOF_EN)       &= ~(1 << echo_pin);
    GPIO_REG(GPIO_IOF_SEL)      &= ~(1 << echo_pin); //Deselecting IOF1 for PMW
    GPIO_REG(GPIO_OUTPUT_XOR)   |= 1 << echo_pin;
    GPIO_REG(GPIO_RISE_IE)		|= 1 << echo_pin;

    unsigned char scale = 4; //1MHz
    unsigned int pulse  = (0.00001 * cpu_freq) / 16;
    unsigned int period = (0.01 * cpu_freq) / 16;
    uint16_t passed;
    uint16_t t2;


	while(1)
	{
		PWM1_REG(PWM_CFG)   = 0;
		PWM1_REG(PWM_COUNT) = 0;
		PWM1_REG(PWM_CMP0)  = period;
		PWM1_REG(PWM_CMP3)  = pulse;
		PWM1_REG(PWM_CFG)   = scale | PWM_CFG_ZEROCMP | PWM_CFG_ENALWAYS;


		passed = PWM1_REG(PWM_S);

		while(!(GPIO_REG(GPIO_RISE_IP) & (1 << echo_pin)))
            ;

		t2 = PWM1_REG(PWM_S);
		passed = t2 - passed;

		PWM1_REG(PWM_CFG)   = 0;
		GPIO_REG(GPIO_RISE_IP) |= 1 << echo_pin;

		printf("Distance: %x\n", passed);

		wait_for_timer();
	}


    return 0;
}


//asm(".global _printf_float");
