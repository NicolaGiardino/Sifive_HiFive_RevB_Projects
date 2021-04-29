// See LICENSE for license details.

#ifndef _SIFIVE_UART_H
#define _SIFIVE_UART_H

#include "../platform.h"
#include "gpio.h"

/* Register offsets */
#define UART_REG_TXFIFO         0x00
#define UART_REG_RXFIFO         0x04
#define UART_REG_TXCTRL         0x08
#define UART_REG_RXCTRL         0x0c
#define UART_REG_IE             0x10
#define UART_REG_IP             0x14
#define UART_REG_DIV            0x18

/* TXFIFO register */
#define UART_TX_FULL            31

/* RXFIFO register */
#define UART_RX_EMPTY           31

/* TXCTRL register */
#define UART_TXEN               0x1
#define UART_TXWM(x)            (((x) & 0xffff) << 16)

/* RXCTRL register */
#define UART_RXEN               0x1
#define UART_RXWM(x)            (((x) & 0xffff) << 16)

/* IP register */
#define UART_IP_TXWM            0x1
#define UART_IP_RXWM            0x2

#define UART_UNACTIVE           0
#define UART_ACTIVE             1

#define UART_OK                 0
#define UART_ERR_NV             1
#define UART_ERR_ACT            2
#define UART_ERR_BAUD           3

int uart_init(unsigned int uart, unsigned int baud);

int uart_send(unsigned int uart, const uint8_t *tx);

int uart_receive(unsigned int uart, uint8_t *rx);

int uart_close(unsigned int uart);



#endif /* _SIFIVE_UART_H */
