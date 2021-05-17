#ifndef I2C_H
#define I2C_H

#include "gpio.h"

#define I2C_PRERlo  0x00
#define I2C_PRERhi  0x04
#define I2C_CTR     0x08
#define I2C_TXR     0x0c
#define I2C_RXR     0x0c
#define I2C_CR      0x10
#define I2C_SR      0x10

#define I2C_OK      0
#define I2C_ERR_NV  1
#define I2C_ERR_ACK 2

#define I2C_CTR_EN  (1 << 7)
#define I2C_CTR_IE  (1 << 6)

#define I2C_CR_IACK (1 << 0)
#define I2C_CR_ACK  (1 << 3)
#define I2C_CR_WR   (1 << 4)
#define I2C_CR_RD   (1 << 5)
#define I2C_CR_STO  (1 << 6)
#define I2C_CR_STA  (1 << 7)

#define I2C_SR_IF   (1 << 0)
#define I2C_SR_TIP  (1 << 1)
#define I2C_SR_AL   (1 << 5)
#define I2C_SR_BUSY (1 << 6)
#define I2C_SR_ACK  (1 << 7)

#define I2C_CALC_BAUD(baud) ((cpu_freq / (5 * baud)) - 1)
#define I2C_PRER_RESET      0xFFFF

typedef enum
{
    I2C_BAUD_100K = 100000UL,
    I2C_BAUD_400K = 400000UL
} i2c_baud_t;

struct i2c
{
    unsigned int active;
    i2c_baud_t baud;
    unsigned int int_active;
};

int i2c_init(struct i2c *isc, i2c_baud_t baud);

int i2c_send(struct i2c *isc, uint8_t *tx, unsigned int size, uint8_t addr);

int i2c_receive(struct i2c *isc, uint8_t *rx, unsigned int size, uint8_t addr);

int i2c_transmit(struct i2c *isc, uint8_t *rx, unsigned int rx_size, uint8_t *tx, unsigned int tx_size, uint8_t addr);

int i2c_close(struct i2c *isc);

int i2c_interrupt_enable(struct i2c *isc, void *isr, unsigned int prio);

int i2c_interrupt_disable(struct i2c *isc);

#endif