#ifndef SERVO_H
#define SERVO_H

#include "hifive1.h"
#include "platform.h"
#include "gpio_to_pin.h"

#define PERIOD 20000 //us
#define SERVO PIN6
#define MINPWM 544
#define MAXPWM 2400
#define DEFPWM 1472
#define ERR 1
#define OK 0

int InitServo(unsigned int pin);
int MoveServo(unsigned int pin, unsigned int pos);
int EndServo(unsigned int pin);

#endif
