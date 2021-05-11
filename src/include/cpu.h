#ifndef CPU_H
#define CPU_H

#include "devices/plic.h"
#include "devices/clint.h"

#define INT_OK 0
#define INT_ERR_ACTIVE 1

unsigned int cpu_get_mhartid();

unsigned int cpu_get_marchid();

unsigned int cpu_get_machimplid();

int cpu_interrupt_enable();

void cpu_interrupt_service_routine() __attribute__((interrupt, aligned(64)));

unsigned int cpu_get_mhartid()
{
    uint32_t mhartid;

    __asm__ volatile("csrr %0, mhartid" : "=r"(mhartid));

    return mhartid;
}

unsigned int cpu_get_marchid()
{
    uint32_t mharchid;

    __asm__ volatile("csrr %0, mharchid" : "=r"(mharchid));

    return mharchid;
}

unsigned int prci_pll_get_machimplid()
{
    uint32_t mimpid;

    __asm__ volatile("csrr %0, mimpid" : "=r"(mimpid));

    return mimpid;
}

int cpu_interrupt_enable()
{
    uint32_t mstatus, mie;

    __asm__ volatile("csrr %0, mstatus" : "=r"(mstatus));

    if ((mstatus & 0b1000) == 0)
    {
    	mie = 0;
    	__asm__ volatile("csrw mie, %0" : : "r"(mie));
    	//mtvec |= MTVEC_ENABLE_DIRECT;
    	//__asm__ volatile("csrw mtvec, %I" : : "r"((unsigned long) interrupt_service_routine));
        write_csr(mtvec, ((unsigned long)&interrupt_service_routine | MTVEC_ENABLE_DIRECT));
        mstatus = MSTATUS_MIE;
        __asm__ volatile("csrw mstatus, %0" : : "r"(mstatus));
    }
    else
    {
        return -INT_ERR_ACTIVE;
    }

    return INT_OK;
}

void cpu_interrupt_service_routine()
{

    uint32_t mcause;

   /* __asm__ volatile("csrrw a0, mscratch, a0\n\t"
                     "sw a1, 0(a0)\n\t"
                     "sw a2, 4(a0)\n\t"
                     "sw a3, 8(a0)\n\t"
                     "sw a4, 12(a0)\n\t");*/

    __asm__ volatile("csrr %0, mcause" : "=r"(mcause));
    if (mcause & MCAUSE_INT)
    {
        if (mcause == ((1 << 31) | MCAUSE_MTI))
        {
            clint_timer_interrupt_handler();
        }
        else if (mcause == ((1 << 31) | MCAUSE_MSI))
            ; /* Clint interrupt handling not yet coded */
        else if (mcause == ((1 << 31) | MCAUSE_MEI))
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

#endif /* CPU_H */
