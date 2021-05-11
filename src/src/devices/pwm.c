#include "../../include/devices/pwm.h"

int pwm_init(struct pwm p, unsigned int pin, unsigned char scale)
{

    if (pin > MAXPIN - 1)
    {
        return -PWM_ERR_NV;
    }

    if (p.pwm_num != variant_pin_map[pin].pwm_num || variant_pin_map[pin].pwm_cmp_num == 0)
    {
        return -PWM_ERR_NV;
    }

    if (scale < 0b001 || scale > 0b111)
    {
        return -PWM_ERR_SCALE;
    }

    unsigned int gpio;

    p.pwm_num = variant_pin_map[pin].pwm_num;
    p.scale = scale;

    return PWM_OK;
}

int pwm_set_period(struct pwm p, unsigned int period, pwm_zerocmp_t zerocmp, pwm_mode_t mode)
{
    //Period must be in seconds
    if (p.scale == 0b0000)
    {
        return -PWM_ERR_NV;
    }

    unsigned int gpio;
    unsigned int scale;
    unsigned int period_scaled;
    scale = (unsigned int)p.scale;

    if (period > (cpu_freq / scale))
    {
        return -PWM_ERR_PERIOD;
    }

    period_scaled = (period * cpu_freq) / scale;

    switch (p.pwm_num)
    {
    case 0:                                 //This is the case of PWM0, 8bits, not enough for a counter
        PWM0_REG(PWM_CFG) = 0;              //Disable PWM
        PWM0_REG(PWM_COUNT) = 0;            //Set count to zero incrementing at cpu_freq speed
        PWM0_REG(PWM_CMP0) = period_scaled; //Set the reset point as soon as period is reached (this can be used as a timer)
        if (mode == PWM_EN_ALWAYS)
        {
            if (zerocmp == PWM_ZEROCMP_EN)
            {
                PWM0_REG(PWM_CFG) |= p.scale | PWM_CFG_ENALWAYS | PWM_CFG_ZEROCMP;
            }
            else
            {
                PWM0_REG(PWM_CFG) |= p.scale | PWM_CFG_ENALWAYS;
            }
        }
        else
        {
            if (zerocmp == PWM_ZEROCMP_EN)
            {
                PWM0_REG(PWM_CFG) |= p.scale | PWM_CFG_ONESHOT | PWM_CFG_ZEROCMP;
            }
            else
            {
                PWM0_REG(PWM_CFG) |= p.scale | PWM_CFG_ONESHOT;
            }
        }
        break;

    case 1:
        PWM1_REG(PWM_CFG) = 0;              //Disable PWM
        PWM1_REG(PWM_COUNT) = 0;            //Set count to zero incrementing at cpu_freq speed
        PWM1_REG(PWM_CMP0) = period_scaled; //Set the reset point as soon as period is reached (this can be used as a timer)
        if (mode == PWM_EN_ALWAYS)
        {
            if (zerocmp == PWM_ZEROCMP_EN)
            {
                PWM1_REG(PWM_CFG) |= p.scale | PWM_CFG_ENALWAYS | PWM_CFG_ZEROCMP;
            }
            else
            {
                PWM1_REG(PWM_CFG) |= p.scale | PWM_CFG_ENALWAYS;
            }
        }
        else
        {
            if (zerocmp == PWM_ZEROCMP_EN)
            {
                PWM1_REG(PWM_CFG) |= p.scale | PWM_CFG_ONESHOT | PWM_CFG_ZEROCMP;
            }
            else
            {
                PWM1_REG(PWM_CFG) |= p.scale | PWM_CFG_ONESHOT;
            }
        }
        break;

    case 2:
        PWM2_REG(PWM_CFG) = 0;              //Disable PWM
        PWM2_REG(PWM_COUNT) = 0;            //Set count to zero incrementing at cpu_freq speed
        PWM2_REG(PWM_CMP0) = period_scaled; //Set the reset point as soon as period is reached (this can be used as a timer)
        if (mode == PWM_EN_ALWAYS)
        {
            if (zerocmp == PWM_ZEROCMP_EN)
            {
                PWM2_REG(PWM_CFG) |= p.scale | PWM_CFG_ENALWAYS | PWM_CFG_ZEROCMP;
            }
            else
            {
                PWM2_REG(PWM_CFG) |= p.scale | PWM_CFG_ENALWAYS;
            }
        }
        else
        {
            if (zerocmp == PWM_ZEROCMP_EN)
            {
                PWM2_REG(PWM_CFG) |= p.scale | PWM_CFG_ONESHOT | PWM_CFG_ZEROCMP;
            }
            else
            {
                PWM2_REG(PWM_CFG) |= p.scale | PWM_CFG_ONESHOT;
            }
        }
        break;

    default:
        return -PWM_ERR_NV;
    }

    return PWM_OK;
}

int pwm_set_pulsewidth(struct pwm p, unsigned int pin, unsigned int pulse)
{
    /* Pulse, as period, must be in seconds */
    if (p.scale == 0b0000)
    {
        return -PWM_ERR_NV;
    }

    if (pulse > p.period)
    {
        return -PWM_ERR_PULSE;
    }

    unsigned int gpio;

    gpio = variant_pin_map[pin].bit_pos;

    GPIO_REG(GPIO_OUTPUT_XOR) &= ~(1 << gpio);
    GPIO_REG(GPIO_IOF_EN) &= ~(1 << gpio);
    GPIO_REG(GPIO_INPUT_EN) &= ~(1 << gpio);
    GPIO_REG(GPIO_OUTPUT_EN) |= 1 << gpio;
    GPIO_REG(GPIO_PULLUP_EN) &= ~(1 << gpio);

    unsigned int pulse_scaled;
    unsigned int scale;
    scale = (unsigned int)p.scale;

    pulse_scaled = (pulse * cpu_freq) / scale;

    GPIO_REG(GPIO_IOF_EN)     |= 1 << gpio;
    GPIO_REG(GPIO_IOF_SEL)    |= 1 << gpio; //Selecting IOF1 for PMW
    GPIO_REG(GPIO_OUTPUT_XOR) |= 1 << gpio;

    switch (p.pwm_num)
    {
    case 0: //This is the case of PWM0, 8bits, not enough for a counter
        switch (variant_pin_map[pin].pwm_cmp_num)
        {
        case 0:
            return -PWM_ERR_NV;
        case 1:
            PWM0_REG(PWM_CMP1) = pulse_scaled;
            p.pulse[0] = pulse;
            break;
        case 2:
            PWM0_REG(PWM_CMP2) = pulse_scaled;
            p.pulse[0] = pulse;
            break;
        case 3:
            PWM0_REG(PWM_CMP3) = pulse_scaled;
            p.pulse[0] = pulse;
            break;
        default:
            return -PWM_ERR_NV;
        }

    case 1:
        switch (variant_pin_map[pin].pwm_cmp_num)
        {
        case 0:
            return -PWM_ERR_NV;
        case 1:
            PWM1_REG(PWM_CMP1) = pulse_scaled;
            p.pulse[0] = pulse;
            break;
        case 2:
            PWM1_REG(PWM_CMP2) = pulse_scaled;
            p.pulse[0] = pulse;
            break;
        case 3:
            PWM1_REG(PWM_CMP3) = pulse_scaled;
            p.pulse[0] = pulse;
            break;
        default:
            return -PWM_ERR_NV;
        }

    case 2:
        switch (variant_pin_map[pin].pwm_cmp_num)
        {
        case 0:
            return -PWM_ERR_NV;
        case 1:
            PWM2_REG(PWM_CMP1) = pulse_scaled;
            p.pulse[0] = pulse;
            break;
        case 2:
            PWM2_REG(PWM_CMP2) = pulse_scaled;
            p.pulse[0] = pulse;
            break;
        case 3:
            PWM2_REG(PWM_CMP3) = pulse_scaled;
            p.pulse[0] = pulse;
            break;
        default:
            return -PWM_ERR_NV;
        }

    default:
        return -PWM_ERR_NV;
    }

    return PWM_OK;
}

int pwm_set_duty(struct pwm p, unsigned int pin, unsigned int duty)
{
    /* Duty must be in percentage, duty on */
    if (duty > 100)
    {
        return -PWM_ERR_DUTY;
    }

    unsigned int pulse;
    pulse = (duty * p.period) / 100;

    return pwm_set_pulsewidth(p, pin, pulse);
}

unsigned int pwm_get_period(struct pwm p)
{
    if(p.scale == 0b0000)
    {
        return -PWM_ERR_NV;
    }

    return p.period;
}

unsigned int pwm_get_pulse(struct pwm p, unsigned int pin)
{
    if(p.scale == 0b0000)
    {
        return PWM_ERR_GET;
    }

    if (variant_pin_map[pin].pwm_num != p.pwm_num || variant_pin_map[pin].pwm_cmp_num == 0) 
    {
        return PWM_ERR_GET;
    }

    return p.pulse[variant_pin_map[pin].pwm_cmp_num - 1];
}

unsigned int pwm_get_duty(struct pwm p, unsigned int pin)
{
    unsigned int pulse;
    pulse = pwm_get_pulse(p, pin);

    if(pulse == PWM_ERR_GET)
    {
        return pulse;
    }

    return (pulse * 100) / p.period;
}

int pwm_interrupt_enable(struct pwm p, unsigned int pin, void *isr, unsigned int prio)
{
    /* Pulse, as period, must be in seconds */
    if (p.scale == 0b0000)
    {
        return -PWM_ERR_NV;
    }
    if (p.pwm_num != variant_pin_map[pin].pwm_num || variant_pin_map[pin].pwm_cmp_num == 0)
    {
        return -PWM_ERR_NV;
    }

    unsigned int source = PWM0_INT + 4 * p.pwm_num + variant_pin_map[pin].pwm_cmp_num;

    irq_functions[source].handler = isr;
    irq_functions[source].priority = prio;
    irq_functions[source].active = 1;

    PLIC_REG(PLIC_PRIORITY_OFFSET + 4 * source) = prio;

    PLIC_REG(PLIC_ENABLE_OFFSET2) |= (1 << (31 - source));
}