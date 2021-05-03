#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "platform.h"
#include "devices/plic.h"
#include "devices/clint.h"

#define INT_OK 0
#define INT_ERR_ACTIVE 1

int interrupt_enable();
void interrupt_set_direct_mode(void *isr);
void interrupt_service_routine() __attribute__((interrupt, aligned(64)));

int interrupt_enable()
{
    uint32_t mstatus, mie;

    __asm__ volatile("csrr %0, mstatus" : "=r"(mstatus));

    if ((mstatus & 0b1000) == 0)
    {
    	mie = 0;
    	__asm__ volatile("csrw mie, %0" : : "r"(mie));
    	//mtvec |= MTVEC_ENABLE_DIRECT;
    	//__asm__ volatile("csrw mtvec, %I" : : "r"((unsigned long) interrupt_service_routine));
        write_csr(mtvec, (unsigned long)&interrupt_service_routine);
    	plic_interrupt_enable();
        mstatus = MSTATUS_MIE;
        __asm__ volatile("csrw mstatus, %0" : : "r"(mstatus));
    }
    else
    {
        return -INT_ERR_ACTIVE;
    }

    return INT_OK;
}

void interrupt_set_direct_mode(void *isr)
{
    uint32_t mtvec;
    mtvec = isr;
    //mtvec |= MTVEC_ENABLE_DIRECT;
    __asm__ volatile("csrw mtvec, %0" : : "r"(mtvec));
}

void interrupt_service_routine()
{

    uint32_t mscratch;

    __asm__ volatile("csrrw a0, mscratch, a0\n\t"
                     "sw a1, 0(a0)\n\t"
                     "sw a2, 4(a0)\n\t"
                     "sw a3, 8(a0)\n\t"
                     "sw a4, 12(a0)\n\t");

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

    __asm__ volatile("lw a4, 12(a0)\n\t"
                     "lw a3, 8(a0)\n\t"
                     "lw a2, 4(a0)\n\t"
                     "lw a1, 0(a0)\n\t"
                     "csrrw a0, mscratch, a0\n\t"
                     "mret");
}

#endif
