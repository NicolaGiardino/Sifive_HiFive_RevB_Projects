/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <stdio.h>
#include <string.h>
#include "./include/devices/spi.h"
#include "./include/cpu_freq.h"
#include "./include/devices/uart.h"

#define CS_ESP01	PIN_SPI1_SS2
#define HS_GPIO		10


int main (void)
{
    int rc, push;

    struct spi s;
    struct spi_config config = {
    		0b1000,
			80000,
			0,
			0,
			0,
			0,
			0,
    };

    GPIO_REG(GPIO_INPUT_EN) |= 1 << HS_GPIO;

    set_freq_320MHz();

    uart_init(0, 115200);


    rc = spi_init(&s, 1, config);
    if(rc)
    {
    	printf("SPI Init error\n");
    	return 7;
    }

    rc = spi_set_cs(&s, CS_ESP01, SPI_CSMODE_AUTO, SPI_CSDEF_EN);

    uint8_t snd_msg[4] = {0x02, 0x00, 0x00, 0x00};
    uint8_t len_msg[4] = {0x00, 0x00, 0x00, 'A'};

    uint8_t rcv[10];

    uint32_t len = strlen("AT+RST\r\n");


    delay(20000000);

    rc = spi_send_multiple(&s, CS_ESP01, snd_msg, 4u);

    while((GPIO_REG(GPIO_INPUT_VAL) & (1 << HS_GPIO)) == 0)
    	;

    len_msg[0] = len & 127;
    len_msg[1] = len >> 7;

    rc = spi_send_multiple(&s, CS_ESP01, len_msg, 4u);

    while((GPIO_REG(GPIO_INPUT_VAL) & (1 << HS_GPIO)) == 0)
        	;

    rc = spi_send_multiple(&s, CS_ESP01, "AT+RST\r\n", len);

    while((GPIO_REG(GPIO_INPUT_VAL) & (1 << HS_GPIO)) == 0)
        	;

    snd_msg[0] = 0x01;

    rc = spi_send_multiple(&s, CS_ESP01, snd_msg, 4u);

    while((GPIO_REG(GPIO_INPUT_VAL) & (1 << HS_GPIO)) == 0)
            	;

    rc = spi_receive_multiple(&s, CS_ESP01, len_msg, 4u);

    len = (len_msg[1] << 7) + len_msg[0];

    rc = spi_receive_multiple(&s, CS_ESP01, rcv, len);

    printf("\n\n\n");
    for (size_t i = 0; i < len; i++)
    {
        printf("%c", rcv[i]);
    }
    while (1)
    {



	}

    return 0;
}


//asm(".global _printf_float");
