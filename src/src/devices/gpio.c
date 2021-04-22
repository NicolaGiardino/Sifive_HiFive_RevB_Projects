#include "../../include/devices/gpio.h"

int gpio_set(unsigned int pin, gpio_mode_t mode)
{

    if(mode == INPUT)
    {
        return gpio_setinput(pin, GPIO_PUP_DIS);
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
    GPIO_REG(GPIO_PULLUP_EN) &= ~(1 << variant_pin_map[pin].bit_pos);

    if(xor == GPIO_OUT_XOR_EN)
    {
        GPIO_REG(GPIO_OUTPUT_XOR) |= (1 << variant_pin_map[pin].bit_pos);
    }
    else 
    {
        GPIO_REG(GPIO_OUTPUT_XOR) &= ~(1 << variant_pin_map[pin].bit_pos);
    }

    return GPIO_OK;
}

int gpio_setinput(unsigned int pin, gpio_pullup_t pup)
{
    if (pin > MAXPIN - 1)
    {
        return -GPIO_ERR_NV;
    }
    GPIO_REG(GPIO_INPUT_EN)   |= (1 << variant_pin_map[pin].bit_pos);
    GPIO_REG(GPIO_OUTPUT_EN)  &= ~(1 << variant_pin_map[pin].bit_pos);
    GPIO_REG(GPIO_OUTPUT_XOR) &= ~(1 << variant_pin_map[pin].bit_pos);

    if(pup == GPIO_PUP_EN)
    {
        GPIO_REG(GPIO_PULLUP_EN) |= (1 << variant_pin_map[pin].bit_pos);
    }
    else if(pup == GPIO_PUP_DIS)
    {
        GPIO_REG(GPIO_PULLUP_EN) &= ~(1 << variant_pin_map[pin].bit_pos);
    }
    else 
    {
        return -GPIO_ERR_NV;
    }


    return GPIO_OK;
}

int gpio_getoutput(unsigned int pin)
{
    if(pin > MAXPIN - 1)
    {
        return -GPIO_ERR_NV;
    }
    if(GPIO_REG(GPIO_OUTPUT_EN) & (1 << variant_pin_map[pin].bit_pos) == 0)
    {
        return -GPIO_ERR_NOTOUT;
    }

    if (GPIO_REG(GPIO_OUTPUT_VAL) & (1 << variant_pin_map[pin].bit_pos))
    {
        return 1;
    }
    else
    {
        return 0;
    }
    
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
