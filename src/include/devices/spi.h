// See LICENSE for license details.

#ifndef SPI_H
#define SPI_H

#include "../platform.h"
#include "./gpio.h"
/* Register offsets */

#define SPI_REG_SCKDIV          0x00
#define SPI_REG_SCKMODE         0x04
#define SPI_REG_CSID            0x10
#define SPI_REG_CSDEF           0x14
#define SPI_REG_CSMODE          0x18

#define SPI_REG_DCSSCK          0x28
#define SPI_REG_DSCKCS          0x2a
#define SPI_REG_DINTERCS        0x2c
#define SPI_REG_DINTERXFR       0x2e

#define SPI_REG_FMT             0x40
#define SPI_REG_TXFIFO          0x48
#define SPI_REG_RXFIFO          0x4c
#define SPI_REG_TXCTRL          0x50
#define SPI_REG_RXCTRL          0x54

#define SPI_REG_FCTRL           0x60
#define SPI_REG_FFMT            0x64

#define SPI_REG_IE              0x70
#define SPI_REG_IP              0x74

/* Fields */

#define SPI_SCK_POL             0x1
#define SPI_SCK_PHA             0x2

#define SPI_FMT_PROTO(x)        ((x) & 0x3)
#define SPI_FMT_ENDIAN(x)       (((x) & 0x1) << 2)
#define SPI_FMT_DIR(x)          (((x) & 0x1) << 3)
#define SPI_FMT_LEN(x)          (((x) & 0xf) << 16)

/* TXCTRL register */
#define SPI_TXWM(x)             ((x) & 0xffff)
/* RXCTRL register */
#define SPI_RXWM(x)             ((x) & 0xffff)

#define SPI_IP_TXWM             0x1
#define SPI_IP_RXWM             0x2

#define SPI_FCTRL_EN            0x1

#define SPI_INSN_CMD_EN         0x1
#define SPI_INSN_ADDR_LEN(x)    (((x) & 0x7) << 1)
#define SPI_INSN_PAD_CNT(x)     (((x) & 0xf) << 4)
#define SPI_INSN_CMD_PROTO(x)   (((x) & 0x3) << 8)
#define SPI_INSN_ADDR_PROTO(x)  (((x) & 0x3) << 10)
#define SPI_INSN_DATA_PROTO(x)  (((x) & 0x3) << 12)
#define SPI_INSN_CMD_CODE(x)    (((x) & 0xff) << 16)
#define SPI_INSN_PAD_CODE(x)    (((x) & 0xff) << 24)

#define SPI_TXFIFO_FULL  (1 << 31)   
#define SPI_RXFIFO_EMPTY (1 << 31)   

#define SPI_FREQ_MAX    100000

#define SPI_OK          0
#define SPI_ERR_NV      1
#define SPI_ERR_CS      2
#define SPI_ERR_BAUD    3
#define SPI_ERR_LEN     4

/* Values */

typedef enum
{
    SPI_DIR_RX = 0,
    SPI_DIR_TX = 1,
} spi_dir_t;

typedef enum
{
    SPI_PROTO_S = 0,
    SPI_PROTO_D = 1,
    SPI_PROTO_Q = 2,
} spi_proto_t;

typedef enum
{
    SPI_ENDIAN_MSB = 0,
    SPI_ENDIAN_LSB = 1,
} spi_endian_t;

typedef uint8_t spi_len_t;

/*
 * If enabled, inactive state of SCK is logical 1
 */
typedef enum
{
    SPI_SCKPOL_EN  = 1,
    SPI_SCKPOL_DIS = 0,
} spi_sckpol_t;

/* 
 * If enabled, data shifted on leading edge, 
 * sampled on trailing edge of SKC 
 */
typedef enum
{
    SPI_SCKPHA_EN  = 1,
    SPI_SCKPHA_DIS = 1,
} spi_sckpha_t;

typedef enum
{
    SPI_CSMODE_AUTO = 0,
    SPI_CSMODE_HOLD = 2,
    SPI_CSMODE_OFF  = 3,
} spi_csmode_t;

typedef enum
{
    SPI_CSDEF_EN  = 1,
    SPI_CSDEF_DIS = 0,
} spi_csdef_t;

typedef enum
{
    SPI_INT_TX = 1,
    SPI_INT_RX = 2
} spi_int_t;

struct spi_config
{
	spi_len_t    len;
    unsigned int baud;
    spi_sckpha_t phase;
    spi_sckpol_t polarity;
    spi_proto_t  protocol;
    spi_endian_t endianness;
    spi_dir_t    direction;
}__attribute__ ((aligned (16)));

struct spi
{
    unsigned int spi_num;
    struct spi_config config;
    unsigned int spi_int_active[2];
}__attribute__ ((aligned (16)));

void delay(uint32_t counter)
{
    volatile uint32_t i = 0;
    while (i < counter) {
        i++;
    }
}

int spi_init(struct spi *s, unsigned int spi_num, struct spi_config config);

int spi_set_cs(struct spi *s, unsigned int cspin, spi_csmode_t csmode, spi_csdef_t csdef);

int spi_transmit(struct spi *s, unsigned int cs, uint8_t *tx, uint8_t *rx, unsigned int size);

int spi_send(struct spi *s, unsigned int cs, const uint8_t tx);

int spi_send_multiple(struct spi *s, unsigned int cs, uint8_t *tx, unsigned int size);

int spi_receive(struct spi *s, unsigned int cs, uint8_t *rx);

int spi_receive_multiple(struct spi *s, unsigned int cs, uint8_t *rx, unsigned int size);

int spi_close(struct spi *s);

int spi_interrupt_enable(struct spi *s, spi_int_t type, void *isr, unsigned int prio);

int spi_interrupt_disable(struct spi *s, spi_int_t type);







int spi_init(struct spi *s, unsigned int spi_num, struct spi_config config)
{
    if (spi_num != 1)
    {
        return -SPI_ERR_NV;
    }
    if (config.baud > SPI_FREQ_MAX)
    {
        return -SPI_ERR_BAUD;
    }
    if (config.len > 8)
    {
        return -SPI_ERR_LEN;
    }
    uint16_t sckdiv;
    sckdiv = cpu_freq / (2 * config.baud) - 1;

    s->spi_num = spi_num;
    s->config.baud = config.baud;
    s->config.direction = config.direction;
    s->config.endianness = config.endianness;
    s->config.phase = config.phase;
    s->config.polarity = config.polarity;
    s->config.protocol = config.protocol;
    s->config.len = config.len;

    //GPIO_REG(GPIO_OUTPUT_XOR)   &= ~(1 << GPIO_SPI1_MISO);
    GPIO_REG(GPIO_IOF_EN) |= (1 << GPIO_SPI1_MISO);
    //GPIO_REG(GPIO_INPUT_EN)     |= (1 << GPIO_SPI1_MISO);
    //GPIO_REG(GPIO_OUTPUT_EN)    &= ~(1 << GPIO_SPI1_MISO);
    //GPIO_REG(GPIO_PULLUP_EN)    &= ~(1 << GPIO_SPI1_MISO);
    GPIO_REG(GPIO_IOF_SEL) &= ~(1 << GPIO_SPI1_MISO);

    //GPIO_REG(GPIO_OUTPUT_XOR)   |= (1 << GPIO_SPI1_MOSI);
    GPIO_REG(GPIO_IOF_EN) |= (1 << GPIO_SPI1_MOSI);
    //GPIO_REG(GPIO_INPUT_EN)     &= ~(1 << GPIO_SPI1_MOSI);
    //GPIO_REG(GPIO_OUTPUT_EN)    |= 1 << GPIO_SPI1_MOSI;
    //GPIO_REG(GPIO_PULLUP_EN)    &= ~(1 << GPIO_SPI1_MOSI);
    GPIO_REG(GPIO_IOF_SEL) &= ~(1 << GPIO_SPI1_MOSI);

    //GPIO_REG(GPIO_OUTPUT_XOR)   |= (1 << GPIO_SPI1_SCK);
    GPIO_REG(GPIO_IOF_EN) |= (1 << GPIO_SPI1_SCK);
    //GPIO_REG(GPIO_INPUT_EN)     &= ~(1 << GPIO_SPI1_SCK);
    //GPIO_REG(GPIO_OUTPUT_EN)    |= 1 << GPIO_SPI1_SCK;
    //GPIO_REG(GPIO_PULLUP_EN)    &= ~(1 << GPIO_SPI1_SCK);
    GPIO_REG(GPIO_IOF_SEL) &= ~(1 << GPIO_SPI1_SCK);

    SPI1_REG(SPI_REG_SCKDIV) = sckdiv;
    SPI1_REG(SPI_REG_SCKMODE) = (config.phase ? 1 << SPI_SCK_PHA : 0 << SPI_SCK_PHA) | (config.polarity ? 1 << SPI_SCK_PHA : 0 << SPI_SCK_PHA);
    SPI1_REG(SPI_REG_FMT) = SPI_FMT_PROTO(config.protocol) | SPI_FMT_ENDIAN(config.endianness) | SPI_FMT_DIR(config.direction) | SPI_FMT_LEN(config.len);

    SPI1_REG(SPI_REG_CSDEF) = 0xFFFFFFFFUL;

    delay(1000);

    return SPI_OK;
}

unsigned int cs_active;

int spi_set_cs(struct spi *s, unsigned int cspin, spi_csmode_t csmode, spi_csdef_t csdef)
{
    if (s->spi_num != 1)
    {
        return -SPI_ERR_NV;
    }
    if (SS_PIN_TO_CS_ID(cspin) > 3)
    {
        return -SPI_ERR_CS;
    }

    unsigned int gpio;
    unsigned int csid;
    gpio = variant_pin_map[cspin].bit_pos;
    csid = SS_PIN_TO_CS_ID(cspin);

    //GPIO_REG(GPIO_OUTPUT_XOR)   |= (1 << gpio);
    GPIO_REG(GPIO_IOF_EN) |= (1 << gpio);
    //GPIO_REG(GPIO_INPUT_EN)     &= ~(1 << gpio);
    //GPIO_REG(GPIO_OUTPUT_EN)    |= (1 << gpio);
    //GPIO_REG(GPIO_PULLUP_EN)    &= ~(1 << gpio);
    GPIO_REG(GPIO_IOF_SEL) &= ~(1 << gpio);

    SPI1_REG(SPI_REG_CSID) = csid;
    SPI1_REG(SPI_REG_CSMODE) = csmode;

    if (csdef == SPI_CSDEF_EN)
    {
        SPI1_REG(SPI_REG_CSDEF) |= (1 << csid);
    }
    else if (csdef == SPI_CSDEF_DIS)
    {
        SPI1_REG(SPI_REG_CSDEF) &= ~(1 << csid);
    }
    else
    {
        return -SPI_ERR_CS;
    }

    SPI1_REG(SPI_REG_FMT) = SPI_FMT_PROTO(s->config.protocol) | SPI_FMT_ENDIAN(s->config.endianness) | SPI_FMT_DIR(s->config.direction) | SPI_FMT_LEN(s->config.len) | SPI_FMT_DIR(SPI_DIR_RX);

    return SPI_OK;
}

int spi_transmit(struct spi *s, unsigned int cs, uint8_t *tx, uint8_t *rx, unsigned int size)
{

    if (cs < 3 && s->spi_num == 1)
    {
        uint32_t i;

        for (i = 0; i < size; i++)
        {
            while (SPI1_REG(SPI_REG_TXFIFO) > 0xFF)
            {
            }
            SPI1_REG(SPI_REG_TXFIFO) = tx[i];

            while (SPI1_REG(SPI_REG_RXFIFO) <= 0xFF)
                ;
            rx[i] = SPI1_REG(SPI_REG_RXFIFO);
        }
    }
    else
    {
        return -SPI_ERR_CS;
    }
}

int spi_send(struct spi *s, unsigned int cspin, const uint8_t tx)
{

    if (s->spi_num != 1)
    {
        return -SPI_ERR_NV;
    }
    if (SS_PIN_TO_CS_ID(cspin) > 3)
    {
        return -SPI_ERR_CS;
    }

    while (SPI1_REG(SPI_REG_TXFIFO) > 0xFF)
    {
    }
    SPI1_REG(SPI_REG_TXFIFO) = tx;

    while (SPI1_REG(SPI_REG_RXFIFO) <= 0xFF)
        ;

    return SPI_OK;
}

int spi_send_multiple(struct spi *s, unsigned int cspin, uint8_t *tx, unsigned int size)
{

    if (size == 0)
    {
        return -SPI_ERR_NV;
    }
    uint16_t i;
    int rc;
    for (i = 0; i < size; i++)
    {
        while (SPI1_REG(SPI_REG_TXFIFO) > 0xFF)
        {
        }
        SPI1_REG(SPI_REG_TXFIFO) = tx[i];

        while (SPI1_REG(SPI_REG_RXFIFO) <= 0xFF)
            ;
    }

    return rc;
}

int spi_receive(struct spi *s, unsigned int cspin, uint8_t *rx)
{

    if (s->spi_num != 1)
    {
        return -SPI_ERR_NV;
    }
    if (SS_PIN_TO_CS_ID(cspin) > 3)
    {
        return -SPI_ERR_CS;
    }

    while (SPI1_REG(SPI_REG_TXFIFO) > 0xFF)
    {
    }
    SPI1_REG(SPI_REG_TXFIFO) = 0x00;

    *rx = SPI1_REG(SPI_REG_RXFIFO);

    return SPI_OK;
}

int spi_receive_multiple(struct spi *s, unsigned int cspin, uint8_t *rx, unsigned int size)
{

    if (size == 0)
    {
        return -SPI_ERR_NV;
    }

    uint16_t i;
    int rc;
    for (i = 0; i < size; i++)
    {
        while (SPI1_REG(SPI_REG_TXFIFO) > 0xFF)
        {
        }
        SPI1_REG(SPI_REG_TXFIFO) = 0x00;

        rx[0] = SPI1_REG(SPI_REG_RXFIFO);
    }
    delay(10000);

    return rc;
}

int spi_close(struct spi *s)
{
    if (s->spi_num != 1)
    {
        return -SPI_ERR_NV;
    }
    GPIO_REG(GPIO_OUTPUT_XOR) &= ~(1 << GPIO_SPI1_MISO);
    GPIO_REG(GPIO_IOF_EN) &= ~(1 << GPIO_SPI1_MISO);
    GPIO_REG(GPIO_INPUT_EN) &= ~(1 << GPIO_SPI1_MISO);
    GPIO_REG(GPIO_OUTPUT_EN) &= ~(1 << GPIO_SPI1_MISO);
    GPIO_REG(GPIO_PULLUP_EN) &= ~(1 << GPIO_SPI1_MISO);
    GPIO_REG(GPIO_IOF_SEL) &= ~(1 << GPIO_SPI1_MISO);

    GPIO_REG(GPIO_OUTPUT_XOR) &= ~(1 << GPIO_SPI1_MOSI);
    GPIO_REG(GPIO_IOF_EN) &= ~(1 << GPIO_SPI1_MOSI);
    GPIO_REG(GPIO_INPUT_EN) &= ~(1 << GPIO_SPI1_MOSI);
    GPIO_REG(GPIO_OUTPUT_EN) &= ~(1 << GPIO_SPI1_MOSI);
    GPIO_REG(GPIO_PULLUP_EN) &= ~(1 << GPIO_SPI1_MOSI);
    GPIO_REG(GPIO_IOF_SEL) &= ~(1 << GPIO_SPI1_MOSI);

    GPIO_REG(GPIO_OUTPUT_XOR) &= ~(1 << GPIO_SPI1_SCK);
    GPIO_REG(GPIO_IOF_EN) &= ~(1 << GPIO_SPI1_SCK);
    GPIO_REG(GPIO_INPUT_EN) &= ~(1 << GPIO_SPI1_SCK);
    GPIO_REG(GPIO_OUTPUT_EN) &= ~(1 << GPIO_SPI1_SCK);
    GPIO_REG(GPIO_PULLUP_EN) &= ~(1 << GPIO_SPI1_SCK);
    GPIO_REG(GPIO_IOF_SEL) &= ~(1 << GPIO_SPI1_SCK);

    GPIO_REG(GPIO_OUTPUT_XOR) &= ~(1 << GPIO_SPI1_SS0);
    GPIO_REG(GPIO_IOF_EN) &= ~(1 << GPIO_SPI1_SS0);
    GPIO_REG(GPIO_INPUT_EN) &= ~(1 << GPIO_SPI1_SS0);
    GPIO_REG(GPIO_OUTPUT_EN) &= ~(1 << GPIO_SPI1_SS0);
    GPIO_REG(GPIO_PULLUP_EN) &= ~(1 << GPIO_SPI1_SS0);
    GPIO_REG(GPIO_IOF_SEL) &= ~(1 << GPIO_SPI1_SS0);

    GPIO_REG(GPIO_OUTPUT_XOR) &= ~(1 << GPIO_SPI1_SS1);
    GPIO_REG(GPIO_IOF_EN) &= ~(1 << GPIO_SPI1_SS1);
    GPIO_REG(GPIO_INPUT_EN) &= ~(1 << GPIO_SPI1_SS1);
    GPIO_REG(GPIO_OUTPUT_EN) &= ~(1 << GPIO_SPI1_SS1);
    GPIO_REG(GPIO_PULLUP_EN) &= ~(1 << GPIO_SPI1_SS1);
    GPIO_REG(GPIO_IOF_SEL) &= ~(1 << GPIO_SPI1_SS1);

    GPIO_REG(GPIO_OUTPUT_XOR) &= ~(1 << GPIO_SPI1_SS2);
    GPIO_REG(GPIO_IOF_EN) &= ~(1 << GPIO_SPI1_SS2);
    GPIO_REG(GPIO_INPUT_EN) &= ~(1 << GPIO_SPI1_SS2);
    GPIO_REG(GPIO_OUTPUT_EN) &= ~(1 << GPIO_SPI1_SS2);
    GPIO_REG(GPIO_PULLUP_EN) &= ~(1 << GPIO_SPI1_SS2);
    GPIO_REG(GPIO_IOF_SEL) &= ~(1 << GPIO_SPI1_SS2);

    GPIO_REG(GPIO_OUTPUT_XOR) &= ~(1 << GPIO_SPI1_SS3);
    GPIO_REG(GPIO_IOF_EN) &= ~(1 << GPIO_SPI1_SS3);
    GPIO_REG(GPIO_INPUT_EN) &= ~(1 << GPIO_SPI1_SS3);
    GPIO_REG(GPIO_OUTPUT_EN) &= ~(1 << GPIO_SPI1_SS3);
    GPIO_REG(GPIO_PULLUP_EN) &= ~(1 << GPIO_SPI1_SS3);
    GPIO_REG(GPIO_IOF_SEL) &= ~(1 << GPIO_SPI1_SS3);

    SPI1_REG(SPI_REG_SCKDIV) = 0;
    SPI1_REG(SPI_REG_SCKMODE) = 0;
    SPI1_REG(SPI_REG_FMT) = 0;

    s->spi_num = 0;

    return SPI_OK;
}

#endif /* _SIFIVE_SPI_H */
