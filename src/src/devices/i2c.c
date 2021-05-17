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

int i2c_send(struct i2c *isc, uint8_t *tx, unsigned int size, uint8_t addr)
{
    if (isc->active == 0)
    {
        return -I2C_ERR_NV;
    }

    uint32_t i;

    I2C_REG(I2C_TXR) = (addr << 1);
    I2C_REG(I2C_CR) |= I2C_CR_STA | I2C_CR_WR;

    while((I2C_REG(I2C_SR) & I2C_SR_TIP) == 0)
        ;
    if(I2C_REG(I2C_SR) & I2C_SR_ACK)
    {
        return -I2C_ERR_ACK;
    }  
    for (i = 0; i < size; i++)
    {
        I2C_REG(I2C_TXR) = tx[i];

        while ((I2C_REG(I2C_SR) & I2C_SR_TIP) == 0)
            ;
    }

    I2C_REG(I2C_CR) |= I2C_CR_STO | I2C_CR_WR;
    while ((I2C_REG(I2C_SR) & I2C_SR_TIP) == 0)
        ;
    if (I2C_REG(I2C_SR) & I2C_SR_ACK)
    {
        return -I2C_ERR_ACK;
    }

    return I2C_OK;
}

int i2c_receive(struct i2c *isc, uint8_t *rx, unsigned int size, uint8_t addr)
{
    if (isc->active == 0)
    {
        return -I2C_ERR_NV;
    }

    uint32_t i;

    I2C_REG(I2C_TXR) = (addr << 1) + 0b1;
    I2C_REG(I2C_CR) |= I2C_CR_STA | I2C_CR_WR;

    while ((I2C_REG(I2C_SR) & I2C_SR_TIP) == 0)
        ;

    if (I2C_REG(I2C_SR) & I2C_SR_ACK)
    {
        return -I2C_ERR_ACK;
    }

    I2C_REG(I2C_CR) &= ~I2C_CR_WR;
    I2C_REG(I2C_CR) |= I2C_CR_RD;
    for (i = 0; i < size; i++)
    {
        rx[i] = I2C_REG(I2C_TXR);

        I2C_REG(I2C_CR) |= I2C_CR_ACK;
    }

    I2C_REG(I2C_CR) |= I2C_CR_STO | I2C_CR_WR;

    return I2C_OK;
}

int i2c_transmit(struct i2c *isc, uint8_t *rx, unsigned int rx_size, uint8_t *tx, unsigned int tx_size, uint8_t addr)
{
    int rc;

    rc = i2c_send(isc, tx, tx_size, addr);
    if(rc)
    {
        return rc;
    }

    return i2c_receive(isc, rx, rx_size, addr);

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

int i2c_interrupt_enable(struct i2c *isc, void *isr, unsigned int prio)
{
    if (isc->active == 0)
    {
        return -I2C_ERR_NV;
    }

    isc->int_active = 1;

    irq_functions[INT_I2C_BASE].irq_handler = isr;
    irq_functions[INT_I2C_BASE].priority    = prio;
    irq_functions[INT_I2C_BASE].active      = 1;

    PLIC_REG(PLIC_PRIORITY_OFFSET + 4 * (INT_I2C_BASE)) = prio;
    PLIC_REG(PLIC_ENABLE_OFFSET) |= (1 << (INT_I2C_BASE));

    I2C_REG(I2C_CTR) |= I2C_CTR_IE;

    return I2C_OK;
}

int i2c_interrupt_disable(struct i2c *isc)
{
    if (isc->active == 0)
    {
        return -I2C_ERR_NV;
    }

    isc->int_active = 0;

    PLIC_REG(PLIC_ENABLE_OFFSET) &= ~(1 << (INT_I2C_BASE));

    I2C_REG(I2C_CTR) &= ~I2C_CTR_IE;

    irq_functions[INT_I2C_BASE].irq_handler = NULL;
    irq_functions[INT_I2C_BASE].priority    = 0;
    irq_functions[INT_I2C_BASE].active      = 0;

    return I2C_OK;
}