#ifndef _SAMD51_MCLK_H_
#define _SAMD51_MCLK_H_

#include <Arduino.h>

enum mclk_apb_bus {
	MCLK_APBAMASK,
	MCLK_APBBMASK,
	MCLK_APBCMASK,
	MCLK_APBDMASK
};


static inline bool mclk_apb_set_mask(
		const enum mclk_apb_bus bus,
		const uint32_t mask)
{
    switch (bus) {
        case MCLK_APBAMASK:
			MCLK->APBAMASK.reg |= mask;
			break;
		case MCLK_APBBMASK:
			MCLK->APBBMASK.reg |= mask;
			break;
		case MCLK_APBCMASK:
			MCLK->APBCMASK.reg |= mask;
			break;
		case MCLK_APBDMASK:
			MCLK->APBDMASK.reg |= mask;
			break;
		default:
            return false;
    }
    return true;
}

#endif // _SAMD51_MCLK_H_