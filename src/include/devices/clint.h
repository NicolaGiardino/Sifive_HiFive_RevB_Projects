// See LICENSE for license details

#ifndef _SIFIVE_CLINT_H
#define _SIFIVE_CLINT_H


#define CLINT_MSIP 0x0000
#define CLINT_MSIP_size   0x4
#define CLINT_MTIMECMP 0x4000
#define CLINT_MTIMECMP_size 0x8
#define CLINT_MTIME 0xBFF8
#define CLINT_MTIME_size 0x8

void clint_interrupt_enable();

void clint_interrupt_timer_enable();

void clint_interrupt_software_enable();

void clint_interrupt_handler();

extern void clint_timer_interrupt_handler();

void clint_interrupt_enable()
{
    clint_interrupt_timer_enable();
    clint_interrupt_software_enable();
}

void clint_interrupt_timer_enable()
{
    uint32_t mstatus, mie;

    mstatus = 0;

    __asm__ volatile("csrw mstatus, %0" : : "r"(mstatus));

    __asm__ volatile("csrr %0, mie" : "=r"(mie));

    mie |= MIE_MTIE;
    __asm__ volatile("csrw mie, %0" : : "r"(mie));

    mstatus |= MSTATUS_MIE;
    __asm__ volatile("csrw mstatus, %0" : : "r"(mstatus));

}

void clint_interrupt_software_enable()
{
    uint32_t mstatus, mie;

    mstatus = 0;

    __asm__ volatile("csrw mstatus, %0" : : "r"(mstatus));

    __asm__ volatile("csrr %0, mie" : "=r"(mie));

    mie |= MIE_MSIE;
    __asm__ volatile("csrw mie, %0" : : "r"(mie));

    mstatus |= MSTATUS_MIE;
    __asm__ volatile("csrw mstatus, %0" : : "r"(mstatus));
}

#endif /* _SIFIVE_CLINT_H */ 
