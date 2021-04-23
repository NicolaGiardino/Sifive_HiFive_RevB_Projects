// See LICENSE for license details.

#ifndef _SIFIVE_HIFIVE1_H
#define _SIFIVE_HIFIVE1_H

#include <stdint.h>

/****************************************************************************
 * GPIO Connections
 *****************************************************************************/

// These are the GPIO bit offsets for the RGB LED on HiFive1 Board.
// These are also mapped to RGB LEDs on the Freedom E300 Arty
// FPGA
// Dev Kit.

#define RED_LED_OFFSET   22
#define GREEN_LED_OFFSET 19
#define BLUE_LED_OFFSET  21

// These are the GPIO bit offsets for the differen digital pins
// on the headers for both the HiFive1 Board and the Freedom E300 Arty FPGA Dev Kit.
#define PIN_0_OFFSET 16
#define PIN_1_OFFSET 17
#define PIN_2_OFFSET 18
#define PIN_3_OFFSET 19
#define PIN_4_OFFSET 20
#define PIN_5_OFFSET 21
#define PIN_6_OFFSET 22
#define PIN_7_OFFSET 23
#define PIN_8_OFFSET 0
#define PIN_9_OFFSET 1
#define PIN_10_OFFSET 2
#define PIN_11_OFFSET 3
#define PIN_12_OFFSET 4
#define PIN_13_OFFSET 5
//#define PIN_14_OFFSET 8 //This pin is not connected on either board.
#define PIN_15_OFFSET 9
#define PIN_16_OFFSET 10
#define PIN_17_OFFSET 11
#define PIN_18_OFFSET 12
#define PIN_19_OFFSET 13

#define MAXPIN 20

#define VARIANT_DIGITAL_PIN_MAP  {{.io_port = 0, .bit_pos = 16, .pwm_num = 0xF, .pwm_cmp_num = 0}, \
	{.io_port = 0, .bit_pos = 17, .pwm_num = 0xF, .pwm_cmp_num = 0}, \
	{.io_port = 0, .bit_pos = 18, .pwm_num = 0xF, .pwm_cmp_num = 0}, \
	{.io_port = 0, .bit_pos = 19, .pwm_num = 1,  .pwm_cmp_num = 1}, \
	{.io_port = 0, .bit_pos = 20, .pwm_num = 1,  .pwm_cmp_num = 0}, \
	{.io_port = 0, .bit_pos = 21, .pwm_num = 1,  .pwm_cmp_num = 2}, \
	{.io_port = 0, .bit_pos = 22, .pwm_num = 1,  .pwm_cmp_num = 3}, \
	{.io_port = 0, .bit_pos = 23, .pwm_num = 0xF, .pwm_cmp_num = 0}, \
	{.io_port = 0, .bit_pos = 0,  .pwm_num = 0,  .pwm_cmp_num = 0}, \
	{.io_port = 0, .bit_pos = 1,  .pwm_num = 0,  .pwm_cmp_num = 1}, \
	{.io_port = 0, .bit_pos = 2,  .pwm_num = 0,  .pwm_cmp_num = 2}, \
	{.io_port = 0, .bit_pos = 3,  .pwm_num = 0, .pwm_cmp_num  = 3}, \
	{.io_port = 0, .bit_pos = 4,  .pwm_num = 0xF, .pwm_cmp_num = 0}, \
	{.io_port = 0, .bit_pos = 5,  .pwm_num = 0xF, .pwm_cmp_num = 0}, \
	{.io_port = 0, .bit_pos = 8,  .pwm_num = 0xF, .pwm_cmp_num = 0}, \
	{.io_port = 0, .bit_pos = 9,  .pwm_num = 0xF, .pwm_cmp_num = 0}, \
	{.io_port = 0, .bit_pos = 10, .pwm_num = 2,   .pwm_cmp_num = 0}, \
	{.io_port = 0, .bit_pos = 11, .pwm_num = 2,   .pwm_cmp_num = 1}, \
	{.io_port = 0, .bit_pos = 12, .pwm_num = 2,   .pwm_cmp_num = 2}, \
	{.io_port = 0, .bit_pos = 13, .pwm_num = 2,   .pwm_cmp_num = 3}}

struct variant_pin_map_s {
  uint8_t io_port;
  uint8_t bit_pos;
  uint8_t pwm_num;
  uint8_t pwm_cmp_num;
};

const struct variant_pin_map_s variant_pin_map[] = VARIANT_DIGITAL_PIN_MAP;

// These are *PIN* numbers, not
// GPIO Offset Numbers.
#define PIN_SPI1_SCK    (13u)
#define PIN_SPI1_MISO   (12u)
#define PIN_SPI1_MOSI   (11u)
#define PIN_SPI1_SS0    (10u)
#define PIN_SPI1_SS1    (14u) 
#define PIN_SPI1_SS2    (15u)
#define PIN_SPI1_SS3    (16u)

/* These are *GPIO* numbers */
#define GPIO_SPI1_SCK	(5u)
#define GPIO_SPI1_MISO	(4u)
#define GPIO_SPI1_MOSI	(3u)
#define GPIO_SPI1_SS0	(2u)
#define GPIO_SPI1_SS1	(8u)
#define GPIO_SPI1_SS2	(9u)
#define GPIO_SPI1_SS3	(10u)

#define GPIO_SPI_ESP32_SS GPIO_SPI1_SS2

#define SS_PIN_TO_CS_ID(x) \
  ((x==PIN_SPI1_SS0 ? 0 :		 \
    (x==PIN_SPI1_SS1 ? 1 :		 \
     (x==PIN_SPI1_SS2 ? 2 :		 \
      (x==PIN_SPI1_SS3 ? 3 :		 \
       -1))))) 

#define GPIO_UART0_TX	(17u)
#define GPIO_UART0_RX	(16u)

#define GPIO_UART1_TX 	(18u)
#define GPIO_UART1_RX	(19u)

// These buttons are present only on the Freedom E300 Arty Dev Kit.
#ifdef HAS_BOARD_BUTTONS
#define BUTTON_0_OFFSET 15
#define BUTTON_1_OFFSET 30
#define BUTTON_2_OFFSET 31

#define INT_DEVICE_BUTTON_0 (INT_GPIO_BASE + BUTTON_0_OFFSET)
#define INT_DEVICE_BUTTON_1 (INT_GPIO_BASE + BUTTON_1_OFFSET)
#define INT_DEVICE_BUTTON_2 (INT_GPIO_BASE + BUTTON_2_OFFSET)

#endif

#define HAS_HFXOSC 1
#define HAS_LFROSC_BYPASS 1

#define RTC_FREQ 32768

unsigned int cpu_freq = 16000000;

void write_hex(int fd, uint32_t hex);

#endif /* _SIFIVE_HIFIVE1_H */
