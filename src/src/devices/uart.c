#include "../../include/devices/uart.h"

static int uart_active[2] = {0, 0};

int uart_init(unsigned int uart, unsigned int baud)
{
    if (uart > 1)
    {
        return -UART_ERR_NV;
    }
    if (uart_active[uart] == UART_ACTIVE)
    {
        return -UART_ERR_ACT;
    }
    if (baud > cpu_freq)
    {
        return -UART_ERR_BAUD;
    }

    unsigned int div;
    div = cpu_freq / baud - 1;

    switch (uart)
    {
    case 0:
        //GPIO_REG(GPIO_OUTPUT_XOR)   &= ~(1 << GPIO_UART0_RX);
        GPIO_REG(GPIO_IOF_EN) |= (1 << GPIO_UART0_RX);
        //GPIO_REG(GPIO_INPUT_EN)     |= (1 << GPIO_UART0_RX);
        //GPIO_REG(GPIO_OUTPUT_EN)    &= ~(1 << GPIO_UART0_RX);
        //GPIO_REG(GPIO_PULLUP_EN)    &= ~(1 << GPIO_UART0_RX);
        GPIO_REG(GPIO_IOF_SEL) &= ~(1 << GPIO_UART0_RX);

        //GPIO_REG(GPIO_OUTPUT_XOR)   |= (1 << GPIO_UART0_TX);
        GPIO_REG(GPIO_IOF_EN) |= (1 << GPIO_UART0_TX);
        //GPIO_REG(GPIO_INPUT_EN)     &= ~(1 << GPIO_UART0_TX);
        //GPIO_REG(GPIO_OUTPUT_EN)    |= 1 << GPIO_UART0_TX;
        //GPIO_REG(GPIO_PULLUP_EN)    &= ~(1 << GPIO_UART0_TX);
        GPIO_REG(GPIO_IOF_SEL) &= ~(1 << GPIO_UART0_TX);

        UART0_REG(UART_REG_TXCTRL) = 0;
        UART0_REG(UART_REG_RXCTRL) = 0;
        UART0_REG(UART_REG_DIV) = div;
        UART0_REG(UART_REG_RXCTRL) |= 1 << UART_RXEN;
        UART0_REG(UART_REG_TXCTRL) |= 1 << UART_TXEN;
        break;

    case 1:
        GPIO_REG(GPIO_OUTPUT_XOR) &= ~(1 << GPIO_UART1_RX);
        GPIO_REG(GPIO_IOF_EN) |= (1 << GPIO_UART1_RX);
        GPIO_REG(GPIO_INPUT_EN) |= (1 << GPIO_UART1_RX);
        GPIO_REG(GPIO_OUTPUT_EN) &= ~(1 << GPIO_UART1_RX);
        GPIO_REG(GPIO_PULLUP_EN) &= ~(1 << GPIO_UART1_RX);
        GPIO_REG(GPIO_IOF_SEL) &= ~(1 << GPIO_UART1_RX);

        GPIO_REG(GPIO_OUTPUT_XOR) |= (1 << GPIO_UART1_TX);
        GPIO_REG(GPIO_IOF_EN) |= (1 << GPIO_UART1_TX);
        GPIO_REG(GPIO_INPUT_EN) &= ~(1 << GPIO_UART1_TX);
        GPIO_REG(GPIO_OUTPUT_EN) |= 1 << GPIO_UART1_TX;
        GPIO_REG(GPIO_PULLUP_EN) &= ~(1 << GPIO_UART1_TX);
        GPIO_REG(GPIO_IOF_SEL) &= ~(1 << GPIO_UART1_TX);

        UART1_REG(UART_REG_DIV) = div;
        break;

    default:
        return -UART_ERR_NV;
    }

    uart_active[uart] = UART_ACTIVE;

    return UART_OK;
}

int uart_send(unsigned int uart, const uint8_t *tx)
{
    if (uart > 1)
    {
        return -UART_ERR_NV;
    }
    if (uart_active[uart] == UART_UNACTIVE)
    {
        return -UART_ERR_ACT;
    }

    switch (uart)
    {
    case 0:
        while (UART0_REG(UART_REG_TXFIFO) & (1 << UART_TX_FULL))
            ;
        UART0_REG(UART_REG_TXFIFO) |= *tx;
        break;
    
    case 1:
        UART1_REG(UART_REG_TXCTRL) |= 1 << UART_TXEN;
        while (UART1_REG(UART_REG_TXFIFO) & (1 << UART_TX_FULL))
            ;
        UART1_REG(UART_REG_TXFIFO) |= *tx;
        break;

    default:
        return -UART_ERR_NV;
    }

    return UART_OK;
}

int uart_receive(unsigned int uart, uint8_t *rx)
{
    if (uart > 1)
    {
        return -UART_ERR_NV;
    }
    if (uart_active[uart] == UART_UNACTIVE)
    {
        return -UART_ERR_ACT;
    }

    switch (uart)
    {
    case 0:
        while (UART0_REG(UART_REG_RXFIFO) & (1 << UART_RX_EMPTY))
            ;
        *rx = UART0_REG(UART_REG_RXFIFO);
        break;

    case 1:
        UART1_REG(UART_REG_RXCTRL) |= 1 << UART_RXEN;
        while (UART1_REG(UART_REG_RXFIFO) & (1 << UART_RX_EMPTY))
            ;
        *rx = UART1_REG(UART_REG_RXFIFO);
        break;

    default:
        return -UART_ERR_NV;
    }

    return UART_OK;
}

int uart_close(unsigned int uart)
{
    if(uart > 1)
    {
        return -UART_ERR_NV;
    }
    if(uart_active[uart] == UART_UNACTIVE)
    {
        return -UART_ERR_ACT;
    }

    switch (uart)
    {
    case 0:
        GPIO_REG(GPIO_OUTPUT_XOR)   &= ~(1 << GPIO_UART0_RX);
        GPIO_REG(GPIO_IOF_EN)       &= ~(1 << GPIO_UART0_RX);
        GPIO_REG(GPIO_INPUT_EN)     &= ~(1 << GPIO_UART0_RX);
        GPIO_REG(GPIO_OUTPUT_EN)    &= ~(1 << GPIO_UART0_RX);
        GPIO_REG(GPIO_PULLUP_EN)    &= ~(1 << GPIO_UART0_RX);
        GPIO_REG(GPIO_IOF_SEL)      &= ~(1 << GPIO_UART0_RX);

        GPIO_REG(GPIO_OUTPUT_XOR)   &= ~(1 << GPIO_UART0_TX);
        GPIO_REG(GPIO_IOF_EN)       &= ~(1 << GPIO_UART0_TX);
        GPIO_REG(GPIO_INPUT_EN)     &= ~(1 << GPIO_UART0_TX);
        GPIO_REG(GPIO_OUTPUT_EN)    &= ~(1 << GPIO_UART0_TX);
        GPIO_REG(GPIO_PULLUP_EN)    &= ~(1 << GPIO_UART0_TX);
        GPIO_REG(GPIO_IOF_SEL)      &= ~(1 << GPIO_UART0_TX);
        break;

    case 1:
        GPIO_REG(GPIO_OUTPUT_XOR)   &= ~(1 << GPIO_UART1_RX);
        GPIO_REG(GPIO_IOF_EN)       &= ~(1 << GPIO_UART1_RX);
        GPIO_REG(GPIO_INPUT_EN)     &= ~(1 << GPIO_UART1_RX);
        GPIO_REG(GPIO_OUTPUT_EN)    &= ~(1 << GPIO_UART1_RX);
        GPIO_REG(GPIO_PULLUP_EN)    &= ~(1 << GPIO_UART1_RX);
        GPIO_REG(GPIO_IOF_SEL)      &= ~(1 << GPIO_UART1_RX);

        GPIO_REG(GPIO_OUTPUT_XOR)   &= ~(1 << GPIO_UART1_TX);
        GPIO_REG(GPIO_IOF_EN)       &= ~(1 << GPIO_UART1_TX);
        GPIO_REG(GPIO_INPUT_EN)     &= ~(1 << GPIO_UART1_TX);
        GPIO_REG(GPIO_OUTPUT_EN)    &= ~(1 << GPIO_UART1_TX);
        GPIO_REG(GPIO_PULLUP_EN)    &= ~(1 << GPIO_UART1_TX);
        GPIO_REG(GPIO_IOF_SEL)      &= ~(1 << GPIO_UART1_TX);
        break;

    default:
        return -UART_ERR_NV;
    }
    

    uart_active[uart] = UART_ACTIVE;

    return UART_OK;
}
