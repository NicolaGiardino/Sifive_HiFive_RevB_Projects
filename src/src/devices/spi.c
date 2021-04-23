#include "../../include/devices/spi.h"

int spi_init(struct spi *s, unsigned int spi_num, struct spi_config config)
{
    if(spi_num != 1)
    {
        return -SPI_ERR_NV;
    }
    if(config.baud > SPI_FREQ_MAX)
    {
        return -SPI_ERR_BAUD;
    }
    if(config.len > 8)
    {
        return -SPI_ERR_LEN;
    }
    uint16_t sckdiv;
    sckdiv = cpu_freq / (2 * config.baud) - 1;

    s->spi_num              = spi_num;
    s->config.baud          = config.baud;
    s->config.direction     = config.direction;
    s->config.endianness    = config.endianness;
    s->config.phase         = config.phase;
    s->config.polarity      = config.polarity;
    s->config.protocol      = config.protocol;

    GPIO_REG(GPIO_OUTPUT_XOR)   |= (1 << GPIO_SPI1_MISO);
    GPIO_REG(GPIO_IOF_EN)       |= (1 << GPIO_SPI1_MISO);
    GPIO_REG(GPIO_INPUT_EN)     &= ~(1 << GPIO_SPI1_MISO);
    GPIO_REG(GPIO_OUTPUT_EN)    |= 1 << GPIO_SPI1_MISO;
    GPIO_REG(GPIO_PULLUP_EN)    &= ~(1 << GPIO_SPI1_MISO);
    GPIO_REG(GPIO_IOF_SEL)      &= ~(1 << GPIO_SPI1_MISO);

    GPIO_REG(GPIO_OUTPUT_XOR)   |= (1 << GPIO_SPI1_MOSI);
    GPIO_REG(GPIO_IOF_EN)       |= (1 << GPIO_SPI1_MOSI);
    GPIO_REG(GPIO_INPUT_EN)     &= ~(1 << GPIO_SPI1_MOSI);
    GPIO_REG(GPIO_OUTPUT_EN)    |= 1 << GPIO_SPI1_MOSI;
    GPIO_REG(GPIO_PULLUP_EN)    &= ~(1 << GPIO_SPI1_MOSI);
    GPIO_REG(GPIO_IOF_SEL)      &= ~(1 << GPIO_SPI1_MOSI);

    GPIO_REG(GPIO_OUTPUT_XOR)   |= (1 << GPIO_SPI1_SCK);
    GPIO_REG(GPIO_IOF_EN)       |= (1 << GPIO_SPI1_SCK);
    GPIO_REG(GPIO_INPUT_EN)     &= ~(1 << GPIO_SPI1_SCK);
    GPIO_REG(GPIO_OUTPUT_EN)    |= 1 << GPIO_SPI1_SCK;
    GPIO_REG(GPIO_PULLUP_EN)    &= ~(1 << GPIO_SPI1_SCK);
    GPIO_REG(GPIO_IOF_SEL)      &= ~(1 << GPIO_SPI1_SCK);

    SPI1_REG(SPI_REG_SCKDIV)    = sckdiv;
    SPI1_REG(SPI_REG_SCKMODE)   = (config.phase ? 1 << SPI_SCK_PHA : 0 << SPI_SCK_PHA) | (config.polarity ? 1 << SPI_SCK_PHA : 0 << SPI_SCK_PHA);
    SPI1_REG(SPI_REG_FMT)       = SPI_FMT_PROTO(config.protocol) | SPI_FMT_ENDIAN(config.endianness) | SPI_FMT_DIR(config.direction) | SPI_FMT_LEN(config.len);

    SPI1_REG(SPI_REG_CSDEF)     = 0xFFFFFFFFUL;

    return SPI_OK;
}

int spi_transmit(struct spi *s, unsigned int cs, uint32_t *buf, spi_dir_t dir, spi_csmode_t mode, spi_csdef_t csdef)
{

    if(dir == SPI_DIR_TX)
    {
        return spi_send(s, cs, buf, mode, csdef);
    }
    else if(dir == SPI_DIR_RX)
    {
        return spi_transmit(s, cs, buf, mode, csdef);
    }
    else
    {
        return -SPI_ERR_NV;
    }

}

int spi_send(struct spi *s, unsigned int cs, const uint8_t *tx, spi_csmode_t mode, spi_csdef_t csdef)
{

    if (s->spi_num != 1)
    {
        return -SPI_ERR_NV;
    }
    if (cs > 3)
    {
        return -SPI_ERR_CS;
    }

    SPI1_REG(SPI_REG_CSID)   = cs;
    SPI1_REG(SPI_REG_CSMODE) = mode;

    if(csdef == SPI_CSDEF_EN)
    {
        SPI1_REG(SPI_REG_CSDEF) |= (1 << cs);
    }
    else if(csdef == SPI_CSDEF_DIS)
    {
        SPI1_REG(SPI_REG_CSDEF) &= ~(1 << cs);
    }
    else
    {
        return -SPI_ERR_CS;
    }

    SPI1_REG(SPI_REG_FMT) = SPI_FMT_PROTO(s->config.protocol) | SPI_FMT_ENDIAN(s->config.endianness) | SPI_FMT_DIR(s->config.direction) | SPI_FMT_LEN(s->config.len) | SPI_FMT_DIR(SPI_DIR_TX);

    while (SPI1_REG(SPI_REG_TXFIFO) > 0xFF)
        ;
    SPI1_REG(SPI_REG_TXFIFO) = *tx;
}

int spi_receive(struct spi *s, unsigned int cs, uint8_t *rx, spi_csmode_t mode, spi_csdef_t csdef)
{

    if (s->spi_num != 1)
    {
        return -SPI_ERR_NV;
    }
    if (cs > 3)
    {
        return -SPI_ERR_CS;
    }
    SPI1_REG(SPI_REG_CSID) = cs;
    SPI1_REG(SPI_REG_CSMODE) = mode;

    if (csdef == SPI_CSDEF_EN)
    {
        SPI1_REG(SPI_REG_CSDEF) |= (1 << cs);
    }
    else if (csdef == SPI_CSDEF_DIS)
    {
        SPI1_REG(SPI_REG_CSDEF) &= ~(1 << cs);
    }
    else
    {
        return -SPI_ERR_CS;
    }

    SPI1_REG(SPI_REG_FMT) = SPI_FMT_PROTO(s->config.protocol) | SPI_FMT_ENDIAN(s->config.endianness) | SPI_FMT_DIR(s->config.direction) | SPI_FMT_LEN(s->config.len) | SPI_FMT_DIR(SPI_DIR_RX);
    while (SPI1_REG(SPI_REG_RXFIFO) > 0xFF)
        ;
    *rx = SPI1_REG(SPI_REG_RXFIFO)
}

int spi_close(struct spi *s)
{
    if (s->spi_num != 1)
    {
        return -SPI_ERR_NV;
    }
    GPIO_REG(GPIO_OUTPUT_XOR)   &= ~(1 << GPIO_SPI1_MISO);
    GPIO_REG(GPIO_IOF_EN)       &= ~(1 << GPIO_SPI1_MISO);
    GPIO_REG(GPIO_INPUT_EN)     &= ~(1 << GPIO_SPI1_MISO);
    GPIO_REG(GPIO_OUTPUT_EN)    &= ~(1 << GPIO_SPI1_MISO);
    GPIO_REG(GPIO_PULLUP_EN)    &= ~(1 << GPIO_SPI1_MISO);
    GPIO_REG(GPIO_IOF_SEL)      &= ~(1 << GPIO_SPI1_MISO);

    GPIO_REG(GPIO_OUTPUT_XOR)   &= ~(1 << GPIO_SPI1_MOSI);
    GPIO_REG(GPIO_IOF_EN)       &= ~(1 << GPIO_SPI1_MOSI);
    GPIO_REG(GPIO_INPUT_EN)     &= ~(1 << GPIO_SPI1_MOSI);
    GPIO_REG(GPIO_OUTPUT_EN)    &= ~(1 << GPIO_SPI1_MOSI);
    GPIO_REG(GPIO_PULLUP_EN)    &= ~(1 << GPIO_SPI1_MOSI);
    GPIO_REG(GPIO_IOF_SEL)      &= ~(1 << GPIO_SPI1_MOSI);

    GPIO_REG(GPIO_OUTPUT_XOR)   &= ~(1 << GPIO_SPI1_MOSI);
    GPIO_REG(GPIO_IOF_EN)       &= ~(1 << GPIO_SPI1_MOSI);
    GPIO_REG(GPIO_INPUT_EN)     &= ~(1 << GPIO_SPI1_SCK);
    GPIO_REG(GPIO_OUTPUT_EN)    &= ~(1 << GPIO_SPI1_MOSI);
    GPIO_REG(GPIO_PULLUP_EN)    &= ~(1 << GPIO_SPI1_SCK);
    GPIO_REG(GPIO_IOF_SEL)      &= ~(1 << GPIO_SPI1_SCK);

    SPI1_REG(SPI_REG_SCKDIV)    = 0;
    SPI1_REG(SPI_REG_SCKMODE)   = 0;
    SPI1_REG(SPI_REG_FMT)       = 0;

    s->spi_num = 0;

    return SPI_OK;
}
