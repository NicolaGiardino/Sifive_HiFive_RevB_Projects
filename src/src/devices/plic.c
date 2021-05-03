#include "../../include/devices/plic.h"
#include "../../include/platform.h"
#include <math.h>

int plic_interrupt_enable()
{
    uint32_t mie;

    __asm__ volatile("csrr %[r], mie", [r] = "=r"(mie));

    if(mie & MIE_MEIE == 0)
    {
        mie |= MIE_MEIE;
        __asm__ volatile("csrw mie, %[r]", [r] = "r"(mie));
        return 0;
    }

    return -1;
}

void plic_interrupt_handler()
{

    uint32_t pending;
    unsigned int int_number;

    if(pending = PLIC_REG(PLIC_PENDING_OFFSET))
    {
        int_number = sqrt(pending) - 1;
    }
    else if((pending = PLIC_REG(PLIC_PENDING_OFFSET_2)) & PLIC_PENDING2_MASK)
    {
        int_number = sqrt(pending) + PLIC_ENABLE_OFFSET_MAX;
    }
    
    if(irq_functions[int_number].active)
    {
        irq_functions[int_number].irq_handler();
    }

}