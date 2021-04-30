#include "../../include/devices/plic.h"

int plic_interrupt_enable()
{
    uint32_t mstatus;

    __asm__ volatile("csrr %[r] mstatus", [r] "=r"(mstatus));

    if(mstatus & MSTATUS_MIE == 0)
        __asm__ volatile("csrw mstatus, %[r]", [r] = "r" (MSTATUS_MIE));

    return 0;
}

int plic_interrupt_handler()
{



}
