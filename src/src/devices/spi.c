#include "../../include/devices/spi.h"

int spi_init(struct spi *s, unsigned int spi_num, struct spi_config config)
{
    if(spi_num != 1)
    {
        return -SPI_ERR_NV;
    }
    if(config.baud > cpu_freq)
    {
        return -SPI_ERR_BAUD;
    }
    if(config.len > 8)
    {
        return -SPI_ERR_LEN;
    }
    uint16_t sckdiv;
    sckdiv = cpu_freq / (2 * config.baud) - 1;

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
    SPI1_REG(SPI_REG_SCKMODE)   = (config.phase ? 1 << SPI_SCK_PHA : 0 << SPI_SCK_PHA) | (config.polarity  ? 1 << SPI_SCK_PHA : 0 << SPI_SCK_PHA);
    SPI1_REG(SPI_REG_FMT)       = SPI_FMT_PROTO(config.protocol) | SPI_FMT_ENDIAN(config.endianness) | SPI_FMT_DIR(config.direction) | SPI_FMT_LEN(config.len);

}

int spi_transmit(struct spi *s, unsigned int cs, uint32_t *buf, spi_dir_t dir, spi_csmode_t mode, spi_csdef_t csdef);

int spi_send(struct spi *s, unsigned int cs, uint32_t *tx, spi_csmode_t mode, spi_csdef_t csdef);

int spi_receive(struct spi *s, unsigned int cs, uint32_t *tx, spi_csmode_t mode, spi_csdef_t csdef);

int spi_close(struct spi *s);
