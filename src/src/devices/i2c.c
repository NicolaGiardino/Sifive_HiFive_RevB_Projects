#include "../../include/devices/i2c.h"

int i2c_init(struct i2c *isc, i2c_baud_t baud)
{

    int prescaler;
    prescaler = I2C_CALC_BAUD(baud);

    I2C_REG(I2C_PRERlo) = (prescaler & 0xFF);
    I2C_REG(I2C_PRERhi) = (prescaler >> 8) & 0xFF;

    GPIO_REG(GPIO_IOF_EN) |= GPIO_I2C_SDA;
    GPIO_REG(GPIO_IOF_EN) |= GPIO_I2C_SCL;

    GPIO_REG(GPIO_IOF_SEL) &= ~GPIO_I2C_SDA;
    GPIO_REG(GPIO_IOF_SEL) &= ~GPIO_I2C_SCL;

    I2C_REG(I2C_CTR) = I2C_CTR_EN;

    isc->active     = 1;
    isc->baud       = baud;
    isc->int_active = 0;

    return I2C_OK;
}



int i2c_close(struct i2c *isc)
{
    if(isc->active == 0)
    {
        return -I2C_ERR_NV;
    }

    I2C_REG(I2C_PRERlo) = 0xFF;
    I2C_REG(I2C_PRERhi) = 0xFF;

    GPIO_REG(GPIO_IOF_EN) &= ~GPIO_I2C_SDA;
    GPIO_REG(GPIO_IOF_EN) &= ~GPIO_I2C_SCL;

    GPIO_REG(GPIO_IOF_SEL) &= ~GPIO_I2C_SDA;
    GPIO_REG(GPIO_IOF_SEL) &= ~GPIO_I2C_SCL;

    I2C_REG(I2C_CTR) &= ~I2C_CTR_EN;

    isc->active     = 0;
    isc->baud       = 0;
    isc->int_active = 0;

    return I2C_OK;
}