#include "../../include/devices/plic.h"
#include "../../include/platform.h"
#include <math.h>

int plic_interrupt_enable()
{
    uint32_t mstatus, mie;

    mstatus = 0;

    __asm__ volatile("csrw mstatus, %0" : :  "r"(mstatus));

    __asm__ volatile("csrr %0, mie" : "=r"(mie));

    __asm__ volatile("csrw mie, %0" : : "r"(mstatus));

    mie |= MIE_MEIE;
    __asm__ volatile("csrw mie, %0" : : "r"(mie));

    PLIC_REG(PLIC_ENABLE_OFFSET) = 0;
    PLIC_REG(PLIC_ENABLE_OFFSET_2) = 0;

    mstatus |= MSTATUS_MIE;
    __asm__ volatile("csrw mstatus, %0" : : "r"(mstatus));

    return 0;

}

void plic_interrupt_handler()
{

    uint32_t pending, claim;
    unsigned int int_number;

    claim = PLIC_REG(PLIC_CLAIM_OFFSET);
    //pending = PLIC_REG(PLIC_PENDING_OFFSET);

    int_number = claim - 1;

    if (irq_functions[int_number].active)
    {
        irq_functions[int_number].irq_handler();
    }

    PLIC_REG(PLIC_CLAIM_OFFSET) = claim;
    
}