#include "../include/Servo.h"

unsigned int map(int deg, int max, int pmin, int pmax)
{

	return deg * pmax / max + pmin / (max / deg);

}

int InitServo(unsigned int pin)
{
	if(variant_pin_map[pin].pwm_num == 0xF)
	{
		return -ERR;
	}
	else
	{
		pin = variant_pin_map[pin].bit_pos;
		GPIO_REG(GPIO_OUTPUT_XOR) 	&= ~(1 << pin);
		GPIO_REG(GPIO_IOF_EN) 		&= ~(1 << pin);
		GPIO_REG(GPIO_INPUT_EN) 	&= ~(1 << pin);
		GPIO_REG(GPIO_OUTPUT_EN) 	|= 1 << pin;
		GPIO_REG(GPIO_PULLUP_EN) 	&= ~(1 << pin);
		return OK;
	}
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

	GPIO_REG(GPIO_IOF_EN) 		|= 1 << gpio;
	GPIO_REG(GPIO_IOF_SEL) 		|= 1 << gpio; //Selecting IOF1 for PMW
	GPIO_REG(GPIO_OUTPUT_XOR) 	|= 1 << gpio;

	unsigned char pwmscale = 10; //2^10 = 1024, giving a 16MHz/1024 = 15'625Hz Frequency
	unsigned int  period   = (0.020 * cpu_freq) / 1024;  // 20ms period or freq=50Hz
	unsigned int pulse     = (map(pos, 180, MINPWM, MAXPWM) * (cpu_freq / 1000000)) / 1000; //  pulse width

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
	return OK;
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
	GPIO_REG(GPIO_OUTPUT_EN) &= ~(1 << variant_pin_map[pin].bit_pos);
	return OK;
}
