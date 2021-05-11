// See LICENSE for license details.

#ifndef _SIFIVE_PRCI_H
#define _SIFIVE_PRCI_H

#include "../platform.h"


/* Register offsets */

#define PRCI_HFROSCCFG   (0x00)
#define PRCI_HFXOSCCFG   (0x04)
#define PRCI_PLLCFG      (0x08)
#define PRCI_PLLDIV      (0x0C)
#define PRCI_PROCMONCFG  (0xF0)

/* Fields */
#define ROSC_DIV(x)    (((x) & 0x2F) << 0 ) 
#define ROSC_TRIM(x)   (((x) & 0x1F) << 16)
#define ROSC_EN(x)     (((x) & 0x1 ) << 30) 
#define ROSC_RDY(x)    (((x) & 0x1 ) << 31)

#define XOSC_EN(x)     (((x) & 0x1) << 30)
#define XOSC_RDY(x)    (((x) & 0x1) << 31)

#define PLL_R(x)       (((x) & 0x7)  << 0)
// single reserved bit for F LSB.
#define PLL_F(x)       (((x) & 0x3F) << 4)
#define PLL_Q(x)       (((x) & 0x3)  << 10)
#define PLL_SEL(x)     (((x) & 0x1)  << 16)
#define PLL_REFSEL(x)  (((x) & 0x1)  << 17)
#define PLL_BYPASS(x)  (((x) & 0x1)  << 18)
#define PLL_LOCK(x)    (((x) & 0x1)  << 31)

#define PLL_R_default 0x1
#define PLL_F_default 0x1F
#define PLL_Q_default 0x3

#define PLL_REFSEL_HFROSC 0x0
#define PLL_REFSEL_HFXOSC 0x1

#define PLL_SEL_HFROSC 0x0
#define PLL_SEL_PLL    0x1

#define PLL_FINAL_DIV(x)      (((x) & 0x3F) << 0)
#define PLL_FINAL_DIV_BY_1(x) (((x) & 0x1 ) << 8)

#define PROCMON_DIV(x)   (((x) & 0x1F) << 0)
#define PROCMON_TRIM(x)  (((x) & 0x1F) << 8)
#define PROCMON_EN(x)    (((x) & 0x1)  << 16)
#define PROCMON_SEL(x)   (((x) & 0x3)  << 24)
#define PROCMON_NT_EN(x) (((x) & 0x1)  << 28)

#define PROCMON_SEL_HFCLK     0
#define PROCMON_SEL_HFXOSCIN  1
#define PROCMON_SEL_PLLOUTDIV 2
#define PROCMON_SEL_PROCMON   3

void prci_pll_set_freq_hyperspace();

void prci_pll_set_freq_320MHz();

void prci_pll_set_freq_32MHz();

void prci_pll_set_freq_16MHz();

void prci_pll_set_freq_sloth();

void prci_pll_set_freq_hyperspace()
{
    /*Divide standard frequency by 2 PLL_R = 0b01 = 2,
     * multiply by 80 PLL_F = 96/2-1 = M/2-1 = 39,
	 * divide by 2 PLL_Q = 1 = 0b01 = 2
	 */
    PRCI_REG(PRCI_PLLCFG) = PLL_R(1) | PLL_F(96 / 2 - 1) | PLL_Q(1) | PLL_REFSEL(1);

    while (PRCI_REG(PRCI_PLLCFG) & PLL_LOCK(1) == 0)
        ;

    PRCI_REG(PRCI_PLLCFG) |= PLL_SEL(1);

    cpu_freq = 384000000;
}

void prci_pll_set_freq_320MHz()
{
    /* Divide standard frequency by 2 PLL_R = 0b01 = 2,
	 * multiply by 80 PLL_F = 80/2-1 = M/2-1 = 39,
	 * divide by 2 PLL_Q = 1 = 0b01 = 2
	 */
    PRCI_REG(PRCI_PLLCFG) = PLL_R(1) | PLL_F(39) | PLL_Q(0b01) | PLL_REFSEL(1);

    while (PRCI_REG(PRCI_PLLCFG) & PLL_LOCK(1) == 0)
        ;

    PRCI_REG(PRCI_PLLCFG) |= PLL_SEL(1);

    cpu_freq = 320000000;
}

void prci_pll_set_freq_32MHz()
{
    /*Divide standard frequency by 2 PLL_R = 0b01 = 2,
	 * multiply by 80 PLL_F = 48/2-1 = M/2-1,
	 * divide by 2 PLL_Q = 1 = 0b01 = 2
	 * final divide by 4 to get 32
	 */
    PRCI_REG(PRCI_PLLCFG) = PLL_R(1) | PLL_F(48 / 2 - 1) | PLL_Q(0b10) | PLL_SEL(1);

    while (PRCI_REG(PRCI_PLLCFG) & PLL_LOCK(1) == 0)
        ;

    PRCI_REG(PRCI_PLLCFG) |= PLL_SEL(1);

    PRCI_REG(PRCI_PLLDIV) = PLL_FINAL_DIV(4 / 2 - 1);

    cpu_freq = 32000000;
}

void prci_pll_set_freq_16MHz()
{
    PRCI_REG(PRCI_PLLCFG) = 0;

    cpu_freq = 16000000;
}

void prci_pll_set_freq_sloth()
{
    /*Divide standard frequency by 2 PLL_R = 0b01 = 2,
	 * multiply by 80 PLL_F = 48/2-1 = M/2-1,
	 * divide by 2 PLL_Q = 1 = 0b01 = 2
	 * final divide by 128 to get 0.375MHz
	 */
    PRCI_REG(PRCI_PLLCFG) = PLL_R(0b10) | PLL_F(48 / 2 - 1) | PLL_Q(0b11) | PLL_REFSEL(1);

    while (PRCI_REG(PRCI_PLLCFG) & PLL_LOCK(1) == 0)
        ;

    PRCI_REG(PRCI_PLLCFG) |= PLL_SEL(1);

    PRCI_REG(PRCI_PLLDIV) = PLL_FINAL_DIV(0x3F);

    cpu_freq = 375000;
}

#endif // _SIFIVE_PRCI_H
