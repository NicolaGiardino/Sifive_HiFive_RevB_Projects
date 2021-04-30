#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "platform.h"
#include "devices/plic.h"
#include "devices/clint.h"

#define INT_OK 0
#define INT_ERR_ACTIVE 1

int interrupt_enable();
int interrupt_service_routine();

int interrupt_enable()
{
    uint32_t mstatus;

    __asm__ volatile("csrr %[r], mstatus", [r] = "=r"(mstatus));

    if (mstatus & MSTATUS_MIE == 0)
    {
        __asm__ volatile ("csrw mtvec, %r", [r] = "=r" (interrupt_service_routine));
        __asm__ volatile ("csrrci %0, mtvec, 1");
        mstatus |= MSTATUS_MIE;
        __asm__ volatile ("csrw mstatus, %[r]", [r] = "r" (mstatus));
        plic_interrupt_enable();
    }
    else
    {
        return -INT_ERR_ACTIVE;
    }
}

#endif