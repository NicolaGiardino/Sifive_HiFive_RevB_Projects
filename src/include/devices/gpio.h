// See LICENSE for license details.

#ifndef _SIFIVE_GPIO_H
#define _SIFIVE_GPIO_H

#include "../platform.h"

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

int gpio_set(unsigned int pin, gpio_mode_t mode);

int gpio_setoutput(unsigned int pin, gpio_out_xor_t xor);

int gpio_setinput(unsigned int pin, gpio_pullup_t pup);

int gpio_getoutput(unsigned int pin);

int gpio_getinput(unsigned int pin);

int gpio_interrupt_enable(unsigned int pin, void *isr, unsigned int prio);

#endif /* _SIFIVE_GPIO_H */
