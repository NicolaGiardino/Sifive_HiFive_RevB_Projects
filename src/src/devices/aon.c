#include "../../include/devices/aon.h"

void aon_rtc_enable()
{
    AON_REG(AON_RTCCFG) |= AON_RTCCFG_ENALWAYS;
}

void aon_rtc_next_wake_time(uint64_t next)
{
	uint64_t time;
	uint32_t mie, mip;

	__asm__ volatile("csrr %0, mie" : "=r"(mie));
	__asm__ volatile("csrr %0, mie" : "=r"(mip));

	mie &= ~(MIE_MTIE);
	mip &= ~(MIE_MTIE);

	__asm__ volatile("csrw mie, %0" : : "r" (mie));

	time = *mtime;
	time += next;
	*mtimecmp = time;

	mie |= MIE_MTIE;

	__asm__ volatile("csrw mie, %0" : : "r" (mip));
	__asm__ volatile("csrw mie, %0" : : "r" (mie));
}

unsigned int aon_wdt_init(unsigned int ms, wdt_en_t en, wdt_rst_t rst, uint16_t timer)
{
    uint8_t scale = 0;
    uint32_t cmp = ms * RTC_FREQ / 1000;

    while (cmp > 65535)
    {
        scale++;
        cmp /= 2;
    }

    AON_REG(AON_WDOGKEY) = AON_WDOGKEY_VALUE;
    AON_REG(AON_WDOGFEED) = AON_WDOGFEED_VALUE;
    aon_wdt_set_timer(timer);
    
    return aon_wdt_cfg(scale, en, rst);

}

unsigned int aon_wdt_cfg(wdt_scale_t scale, wdt_en_t en, wdt_rst_t rst)
{

    if (scale > AON_WDOGCFG_SCALE || en > 2 || rst > 2)
    {
        return -WDT_ERR_NV;
    }
    AON_REG(AON_WDOGKEY) = AON_WDOGKEY_VALUE;
    AON_REG(AON_WDOGCFG) = scale | (rst << 8) | (en << 12);

    return WDT_OK;
}

uint8_t aon_wdt_get_scale()
{
    uint8_t scale;

    scale = AON_REG(AON_WDOGCFG);

    return scale;
}

unsigned int aon_wdt_get_wdogs()
{
    unsigned int wdogs;

    wdogs = AON_REG(AON_WDOGS);

    return wdogs;
}

void aon_wdt_set_timer(uint16_t time)
{

    AON_REG(AON_WDOGKEY) = AON_WDOGKEY_VALUE;
    AON_REG(AON_WDOGCMP) = time;

}

uint16_t aon_wdt_get_timer()
{
    uint16_t time;

    time = AON_REG(AON_WDOGCMP);

    return time;
}