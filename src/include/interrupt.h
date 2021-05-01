#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "platform.h"
#include "devices/plic.h"
#include "devices/clint.h"

#define INT_OK 0
#define INT_ERR_ACTIVE 1

int interrupt_enable();
void interrupt_set_direct_mode(void *isr);
void interrupt_service_routine();

#endif