/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <stdio.h>
#include <metal/cpu.h>
#include <metal/machine.h>
#include <metal/clock.h>
#include "bsp/env/hifive1.h"
#include "bsp/env/freedom-e300-hifive1/platform.h"
#include "gpio_to_pin.h"

#define RTC_FREQ    32768
#define FREQ 50
#define PERIOD 20 //ms
#define DUTY_MIN 90
#define DUTY_MAX 95
#define ERR 1
#define SERVO PIN6
#define MINPWM 544
#define MAXPWM 2400
#define DEFPWM 1472
#define ERR 1

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
    metal_cpu_set_mtimecmp(cpu, (metal_cpu_get_mtime(cpu) + 0.5 * RTC_FREQ));
    // Enable Timer interrupt
    metal_interrupt_enable(tmr_intr, tmr_id);

    // wait till timer triggers and isr is hit
    while (timer_isr_flag == 0){};

    timer_isr_flag = 0;
}

unsigned int map(int deg, int max, int pmin, int pmax)
{

	return deg * pmax / max + pmin / (max / deg);

}

void InitServo(unsigned int pin)
{
    GPIO_REG(GPIO_OUTPUT_XOR) |= 0<<pin;
    GPIO_REG(GPIO_IOF_EN)     |= 0<<pin;
    GPIO_REG(GPIO_INPUT_EN)   |= 0<<pin;
    GPIO_REG(GPIO_OUTPUT_EN)  |= 1<<pin;
    GPIO_REG(GPIO_PULLUP_EN)  |= 0<<pin;
}

int MoveServo(unsigned int pin, int pos)
{
//The PIN used here is the arduino relative PIN, for semplicity
	/*
	 * This piece of code enables the PWM0 interface
	 * the OR is used to set to one the relevant bit (the one in position pwmpin)
	 */

	if(pos > 180 || pos < 0)
	{
		return -ERR;
	}

	if(pin > MAXPIN - 1)
	{
		return - ERR;
	}

	int cpu_freq;
	cpu_freq = 16000000; //Assuming a 16MHz frequency, can be changed later
	unsigned int gpio;
	gpio = variant_pin_map[pin].bit_pos;

	GPIO_REG(GPIO_IOF_EN)     |= 1<<gpio;
	GPIO_REG(GPIO_IOF_SEL)    |= 1<<gpio; //Selecting IOF1 for PMW
	GPIO_REG(GPIO_OUTPUT_XOR) |= 1<<gpio;

	unsigned char pwmscale = 10; //2^10 = 1024, giving a 16MHz/1024 = 15'625Hz Frequency
	unsigned int  period = (0.020 * cpu_freq) / 1024;  // 20ms period or freq=50Hz
	unsigned int  pulse =  (map(pos, 180, MINPWM, MAXPWM) * (cpu_freq / 1000000)) / 1000; //  pulse width

	switch(variant_pin_map[pin].pwm_num)
	{
		case 0: //This is the case of PWM0, 8bits, not enough for a counter
			return -ERR;

		case 1:
			PWM1_REG(PWM_CFG) = 0;			//Disable PWM
			PWM1_REG(PWM_COUNT) = 0;		//Set count to zero incrementing at cpu_freq speed
			switch(variant_pin_map[pin].pwm_cmp_num)
			{
				case 0:
					PWM1_REG(PWM_CMP1) = period; 	//Set the reset point as soon as period is reached (this can be used as a timer)
					PWM1_REG(PWM_CMP0) = pulse;
					PWM1_REG(PWM_CFG) |= pwmscale | PWM_CFG_ENALWAYS | PWM_CFG_ZEROCMP;
					break;
				case 1:
					PWM1_REG(PWM_CMP0) = period; 	//Set the reset point as soon as period is reached (this can be used as a timer)
					PWM1_REG(PWM_CMP1) = pulse;
					PWM1_REG(PWM_CFG) |= pwmscale | PWM_CFG_ENALWAYS | PWM_CFG_ZEROCMP;
					break;
				case 2:
					PWM1_REG(PWM_CMP0) = period; 	//Set the reset point as soon as period is reached (this can be used as a timer)
					PWM1_REG(PWM_CMP2) = pulse;
					PWM1_REG(PWM_CFG) |= pwmscale | PWM_CFG_ENALWAYS | PWM_CFG_ZEROCMP;
					break;
				case 3:
					PWM1_REG(PWM_CMP0) = period; 	//Set the reset point as soon as period is reached (this can be used as a timer)
					PWM1_REG(PWM_CMP3) = pulse;
					PWM1_REG(PWM_CFG) |= pwmscale | PWM_CFG_ENALWAYS | PWM_CFG_ZEROCMP;
					break;
				default:
					return -ERR;
			}

		case 2:
			PWM2_REG(PWM_CFG) = 0;			//Disable PWM
			PWM2_REG(PWM_COUNT) = 0;		//Set count to zero incrementing at cpu_freq speed
			switch(variant_pin_map[pin].pwm_cmp_num)
			{
				case 0:
					PWM2_REG(PWM_CMP1) = period; 	//Set the reset point as soon as period is reached (this can be used as a timer)
					PWM2_REG(PWM_CMP0) = pulse;
					PWM2_REG(PWM_CFG) |= pwmscale | PWM_CFG_ENALWAYS | PWM_CFG_ZEROCMP;
					break;
				case 1:
					PWM2_REG(PWM_CMP0) = period; 	//Set the reset point as soon as period is reached (this can be used as a timer)
					PWM2_REG(PWM_CMP1) = pulse;
					PWM2_REG(PWM_CFG) |= pwmscale | PWM_CFG_ENALWAYS | PWM_CFG_ZEROCMP;
					break;
				case 2:
					PWM2_REG(PWM_CMP0) = period; 	//Set the reset point as soon as period is reached (this can be used as a timer)
					PWM2_REG(PWM_CMP2) = pulse;
					PWM2_REG(PWM_CFG) |= pwmscale | PWM_CFG_ENALWAYS | PWM_CFG_ZEROCMP;
					break;
				case 3:
					PWM2_REG(PWM_CMP0) = period; 	//Set the reset point as soon as period is reached (this can be used as a timer)
					PWM2_REG(PWM_CMP3) = pulse;
					PWM2_REG(PWM_CFG) |= pwmscale | PWM_CFG_ENALWAYS | PWM_CFG_ZEROCMP;
					break;
				default:
					return -ERR;
			}

			default:
				return -ERR;

	}
	return 0;
}

int EndServo(unsigned int pin)
{
	switch(variant_pin_map[pin].pwm_num)
	{
		case 0:
			return -ERR;
		case 1:
			PWM1_REG(PWM_CFG) = 0;
			break;
		case 2:
			PWM2_REG(PWM_CFG) = 0;
			break;
		default:
			return -ERR;
	}
	GPIO_REG(GPIO_OUTPUT_EN)  |= 0<<variant_pin_map[pin].bit_pos;
	return 0;
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


	while(1)
	{

	}


    return 0;
}


asm(".global _printf_float");
