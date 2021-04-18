#ifndef HCSR04_H
#define HCSR04_H

#include "hifive1.h"
#include "platform.h"
#include "gpio_to_pin.h"

#define ERR 1
#define OK  0

int cpu_freq = 16000000;

int InitHCSR04(unsigned int echo, unsigned int trigger);
int ReadHCSR04(unsigned int echo, unsigned int trigger);
int EndHCSR04(unsigned int echo, unsigned int trigger);

#endif