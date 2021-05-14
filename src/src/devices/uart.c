#include "../../include/devices/uart.h"

static int uart_active[2] = {0, 0};
static int uart_int_active[2][2] = {{0, 0}, {0, 0}};

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

int uart_interrupt_enable(unsigned int uart, uart_int_t type, void *isr, unsigned int prio)
{
    if (uart > 1)
    {
        return -UART_ERR_NV;
    }
    if (uart_active[uart] == UART_UNACTIVE || uart_int_active[type - 1][uart] == UART_UNACTIVE)
    {
        return -UART_ERR_ACT;
    }

    uart_int_active[uart][type - 1] = UART_ACTIVE;

    irq_functions[INT_UART0_BASE + uart].irq_handler = isr;
    irq_functions[INT_UART0_BASE + uart].priority     = prio;
    irq_functions[INT_UART0_BASE + uart].active       = 1;

    PLIC_REG(PLIC_PRIORITY_OFFSET + 4 * (INT_UART0_BASE + uart)) = prio;
    PLIC_REG(PLIC_ENABLE_OFFSET) |= (1 << (INT_UART0_BASE + uart));

    switch (uart)
    {
    case 0:
        UART0_REG(UART_REG_IP) |= type;
        break;
    
    case 1:
        UART1_REG(UART_REG_IP) |= type;
        break;

    default:
        return -UART_ERR_NV;
    }

    return UART_OK;
}

int uart_interrupt_disable(unsigned int uart, uart_int_t type)
{
    if (uart > 1)
    {
        return -UART_ERR_NV;
    }
    if (uart_active[uart] == UART_UNACTIVE || uart_int_active[type - 1][uart] == UART_ACTIVE)
    {
        return -UART_ERR_ACT;
    }

    irq_functions[INT_UART0_BASE + uart].active       = 0;
    irq_functions[INT_UART0_BASE + uart].irq_handler  = NULL;
    irq_functions[INT_UART0_BASE + uart].priority     = 0;

    uart_int_active[uart][type - 1] = UART_UNACTIVE;

    PLIC_REG(PLIC_ENABLE_OFFSET) &= ~(1 << (INT_UART0_BASE + uart));

    switch (uart)
    {
    case 0:
        UART0_REG(UART_REG_IP) &= ~type;
        break;

    case 1:
        UART1_REG(UART_REG_IP) &= ~type;
        break;

    default:
        return -UART_ERR_NV;
    }

    return UART_OK;
}