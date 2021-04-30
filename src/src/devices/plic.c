#include "../../include/devices/plic.h"

int plic_interrupt_enable()
{
    uint32_t mie;

    __asm__ volatile("csrr %[r], mie", [r] = "=r"(mie));

    if(mie & MIE_MEIE == 0)
    {
        mie |= MIE_MEIE;
        __asm__ volatile ("csrw mie, %[r]", [r] = "r" (mie));
    }
    return 0;
}

int plic_interrupt_handler()
{



}
