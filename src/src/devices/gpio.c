#include "../../include/devices/gpio.h"

int gpio_set(unsigned int pin, gpio_mode_t mode)
{

    if(mode == INPUT)
    {
        return gpio_setinput(pin);
    }
    else if(mode == OUTPUT)
    {
        return gpio_setoutput(pin, GPIO_OUT_XOR_EN);
    }
    else
    {
        return -GPIO_ERR_NV;
    }

}

int gpio_setoutput(unsigned int pin, gpio_out_xor_t xor)
{
    if(pin > MAXPIN - 1)
    {
        return -GPIO_ERR_NV;
    }
    GPIO_REG(GPIO_INPUT_EN)  &= ~(1 << variant_pin_map[pin].bit_pos);
    GPIO_REG(GPIO_OUTPUT_EN) |= (1 << variant_pin_map[pin].bit_pos);

    if(xor == GPIO_OUT_XOR_EN)
    {
        GPIO_REG(GPIO_OUTPUT_XOR) |= (1 << variant_pin_map[pin].bit_pos);
    }
    else 
    {
        GPIO_REG(GPIO_OUTPUT_XOR) |= (1 << variant_pin_map[pin].bit_pos);
    }

    return GPIO_OK;
}

int gpio_setinput(unsigned int pin)
{
    if (pin > MAXPIN - 1)
    {
        return -GPIO_ERR_NV;
    }
    GPIO_REG(GPIO_INPUT_EN)  |= (1 << variant_pin_map[pin].bit_pos);
    GPIO_REG(GPIO_OUTPUT_EN) &= ~(1 << variant_pin_map[pin].bit_pos);

    return GPIO_OK;
}

