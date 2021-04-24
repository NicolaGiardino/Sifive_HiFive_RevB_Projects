#ifndef SERVO_H
#define SERVO_H

#include "hifive1.h"
#include "platform.h"

#define PERIOD 20000 //us
#define MINPWM 544
#define MAXPWM 2400
#define DEFPWM 1472
#define DEFPOS 90
#define ERR 1
#define OK 0

struct Servo
{
    unsigned int pin;
    unsigned int position;
};

int InitServo(struct Servo *servo, unsigned int pin);
int MoveServo(struct Servo *servo, unsigned int pos);
int EndServo(struct Servo *servo);

#endif
