// See LICENSE for license details.

#ifndef _SIFIVE_SPI_H
#define _SIFIVE_SPI_H

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

struct spi_config
{
    unsigned int baud;
    spi_sckpha_t phase;
    spi_sckpol_t polarity;
    spi_proto_t  protocol;
    spi_endian_t endianness;
    spi_dir_t    direction;
    spi_len_t    len;
};

struct spi
{
    unsigned int spi_num;
    struct spi_config config;
};

int spi_init(struct spi *s, unsigned int spi_num, struct spi_config config);

int spi_transmit(struct spi *s, unsigned int cs, uint8_t *buf, spi_dir_t dir, spi_csmode_t mode, spi_csdef_t csdef);

int spi_send(struct spi *s, unsigned int cs, uint8_t *tx, spi_csmode_t csmode, spi_csdef_t csdef);

int spi_receive(struct spi *s, unsigned int cs, uint8_t *rx, spi_csmode_t csmode, spi_csdef_t csdef);

int spi_close(struct spi *s);

#endif /* _SIFIVE_SPI_H */
