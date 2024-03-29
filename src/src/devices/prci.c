#include "../../include/devices/prci.h"

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