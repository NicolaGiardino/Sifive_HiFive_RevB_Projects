// See LICENSE for license details.

#ifndef _SIFIVE_GPIO_H
#define _SIFIVE_GPIO_H

#include "../interrupt.h"

#define GPIO_INPUT_VAL      (0x00)
#define GPIO_INPUT_EN       (0x04)
#define GPIO_OUTPUT_EN      (0x08)
#define GPIO_OUTPUT_VAL     (0x0C)
#define GPIO_PULLUP_EN      (0x10)
#define GPIO_DRIVE          (0x14)
#define GPIO_RISE_IE        (0x18)
#define GPIO_RISE_IP        (0x1C)
#define GPIO_FALL_IE        (0x20)
#define GPIO_FALL_IP        (0x24)
#define GPIO_HIGH_IE        (0x28)
#define GPIO_HIGH_IP        (0x2C)
#define GPIO_LOW_IE         (0x30)
#define GPIO_LOW_IP         (0x34)
#define GPIO_IOF_EN         (0x38)
#define GPIO_IOF_SEL        (0x3C)
#define GPIO_OUTPUT_XOR     (0x40)

#define GPIO_OK             0
#define GPIO_ERR_NV         1
#define GPIO_ERR_NOTOUT     2
#define GPIO_ERR_NOTIN      3

typedef enum
{
    GPIO_OUT_XOR_EN  = 1,
    GPIO_OUT_XOR_DIS = 0,
} gpio_out_xor_t;

typedef enum
{
    OUTPUT = 1,
    INPUT  = 0,
} gpio_mode_t;

typedef enum
{
    GPIO_PUP_EN  = 1,
    GPIO_PUP_DIS = 0,
} gpio_pullup_t;

typedef enum
{
	GPIO_RISE_EN = 0,
	GPIO_FALL_EN = 1,
	GPIO_HIGH_EN = 2,
	GPIO_LOW_EN  = 3,
} gpio_int_t;

int gpio_init(unsigned int pin, gpio_mode_t mode);

int gpio_init_output(unsigned int pin, gpio_out_xor_t xor);

int gpio_init_input(unsigned int pin, gpio_pullup_t pup);

int gpio_setoutput(unsigned int pin, unsigned int val);

int gpio_getinput(unsigned int pin);

int gpio_interrupt_enable(unsigned int pin, void *isr, unsigned int prio, gpio_int_t in);

int gpio_init(unsigned int pin, gpio_mode_t mode)
{

    if (mode == INPUT)
    {
        return gpio_init_input(pin, GPIO_PUP_DIS);
    }
    else if (mode == OUTPUT)
    {
        return gpio_init_output(pin, GPIO_OUT_XOR_EN);
    }
    else
    {
        return -GPIO_ERR_NV;
    }
}

int gpio_init_output(unsigned int pin, gpio_out_xor_t xor)
{
    if (pin > MAXPIN - 1)
    {
        return -GPIO_ERR_NV;
    }
    GPIO_REG(GPIO_INPUT_EN) &= ~(1 << variant_pin_map[pin].bit_pos);
    GPIO_REG(GPIO_OUTPUT_EN) |= (1 << variant_pin_map[pin].bit_pos);
    GPIO_REG(GPIO_PULLUP_EN) &= ~(1 << variant_pin_map[pin].bit_pos);

    if (xor == GPIO_OUT_XOR_EN)
    {
        GPIO_REG(GPIO_OUTPUT_XOR) |= (1 << variant_pin_map[pin].bit_pos);
    }
    else
    {
        GPIO_REG(GPIO_OUTPUT_XOR) &= ~(1 << variant_pin_map[pin].bit_pos);
    }

    return GPIO_OK;
}

int gpio_init_input(unsigned int pin, gpio_pullup_t pup)
{
    if (pin > MAXPIN - 1)
    {
        return -GPIO_ERR_NV;
    }

    unsigned int gpio;
    gpio = variant_pin_map[pin].bit_pos;

    GPIO_REG(GPIO_INPUT_EN) |= (1 << gpio);
    GPIO_REG(GPIO_INPUT_VAL) &= ~(1 << gpio);
    GPIO_REG(GPIO_OUTPUT_EN) &= ~(1 << gpio);
    GPIO_REG(GPIO_OUTPUT_XOR) &= ~(1 << gpio);

    if (pup == GPIO_PUP_EN)
    {
        GPIO_REG(GPIO_PULLUP_EN) |= (1 << gpio);
    }
    else if (pup == GPIO_PUP_DIS)
    {
        GPIO_REG(GPIO_PULLUP_EN) &= ~(1 << gpio);
    }
    else
    {
        return -GPIO_ERR_NV;
    }

    return GPIO_OK;
}

int gpio_setoutput(unsigned int pin, unsigned int val)
{
    if (pin > MAXPIN - 1)
    {
        return -GPIO_ERR_NV;
    }
    if (GPIO_REG(GPIO_OUTPUT_EN) & (1 << variant_pin_map[pin].bit_pos) == 0)
    {
        return -GPIO_ERR_NOTOUT;
    }

    if (val)
    {
        GPIO_REG(GPIO_OUTPUT_VAL) |= (1 << variant_pin_map[pin].bit_pos);
    }
    else
    {
        GPIO_REG(GPIO_OUTPUT_VAL) &= ~(1 << variant_pin_map[pin].bit_pos);
    }
    return GPIO_OK;
}

int gpio_getinput(unsigned int pin)
{
    if (pin > MAXPIN - 1)
    {
        return -GPIO_ERR_NV;
    }
    if (GPIO_REG(GPIO_INPUT_EN) & (1 << variant_pin_map[pin].bit_pos) == 0)
    {
        return -GPIO_ERR_NOTIN;
    }

    if (GPIO_REG(GPIO_INPUT_VAL) & (1 << variant_pin_map[pin].bit_pos))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int gpio_interrupt_enable(unsigned int pin, void *isr, unsigned int prio, gpio_int_t in)
{
    if (pin > MAXPIN - 1)
    {
        return -GPIO_ERR_NV;
    }
    if (prio > PLIC_ENABLE_SHIFT_PER_TARGET)
    {
        return -GPIO_ERR_NV;
    }

    unsigned int gpio = variant_pin_map[pin].bit_pos;

    irq_functions[IRQ_GPIO + gpio].irq_handler = isr;
    irq_functions[IRQ_GPIO + gpio].active = 1;
    irq_functions[IRQ_GPIO + gpio].priority = prio;

    if ((IRQ_GPIO + gpio) > PLIC_ENABLE_OFFSET_MAX)
    {
        PLIC_REG(PLIC_ENABLE_OFFSET_2) |= (1 << (31 -IRQ_GPIO + gpio));
    }
    else
    {
        PLIC_REG(PLIC_ENABLE_OFFSET) |= (1 << (IRQ_GPIO + gpio));
    }

    PLIC_REG(PLIC_PRIORITY_OFFSET + 4 * (IRQ_GPIO + gpio)) = prio;

    switch(in)
    {
    case GPIO_RISE_EN:
    	GPIO_REG(GPIO_RISE_IE) |= (1 << gpio);
    	break;

    case GPIO_FALL_EN:
        	GPIO_REG(GPIO_FALL_IE) |= (1 << gpio);
        	break;

    case GPIO_HIGH_EN:
        	GPIO_REG(GPIO_HIGH_IE) |= (1 << gpio);
        	break;

    case GPIO_LOW_EN:
        	GPIO_REG(GPIO_LOW_IE) |= (1 << gpio);
        	break;
    }

    return 0;
}

#endif /* _SIFIVE_GPIO_H */
