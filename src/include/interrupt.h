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

int interrupt_enable()
{
    uint32_t mstatus;

    __asm__ volatile("csrr %0, mstatus" : "=r"(mstatus));

    if (mstatus & MSTATUS_MIE == 0)
    {
        interrupt_set_direct_mode(interrupt_service_routine);
        plic_interrupt_enable();
        mstatus |= MSTATUS_MIE;
        __asm__ volatile("csrw mstatus, %0" : : "r"(mstatus));
    }
    else
    {
        return -INT_ERR_ACTIVE;
    }
}

void interrupt_set_direct_mode(void *isr)
{
    uintptr_t mtvec;
    mtvec = isr;
    mtvec |= MTVEC_ENABLE_DIRECT;
    __asm__ volatile("csrw mtvec, %0" : : "r"(mtvec));
}

void interrupt_service_routine()
{

    uint32_t mscratch;

    /*__asm__ volatile("cssrw a0, mscratch, a0\n\t"
                     "sw a1, 0(a0)\n\t"
                     "sw a2, 4(a0)\n\t"
                     "sw a3, 8(a0)\n\t"
                     "sw a4, 12(a0)\n\t");*/

    __asm__ volatile("csrr %0, mcause" : "=r"(mscratch));
    if (mscratch & MCAUSE_INT)
    {
        if (mscratch & MCAUSE_MTI)
            ; /* Clic interrupt handling not yet coded */
        else if (mscratch & MCAUSE_MSI)
            ; /* Clic interrupt handling not yet coded */
        else if (mscratch & MCAUSE_MEI)
        {
            plic_interrupt_handler();
        }
    }

    /*__asm__ volatile("lw a4, 12(a0)\n\t"
                     "lw a3, 8(a0)\n\t"
                     "lw a2, 4(a0)\n\t"
                     "lw a1, 0(a0)\n\t"
                     "csrrw a0, mscratch, a0\n\t"
                     "mret");*/
}

#endif
