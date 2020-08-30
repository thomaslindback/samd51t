
#include "samd51_gclk.h"
#include "interrupt_sam_nvic.h"

void system_gclk_chan_set_config(const uint8_t channel,
                                 system_gclk_chan_config *const config) {
    /* Disable generic clock channel */
    system_gclk_chan_disable(channel);

    /* Write the new configuration */
    GCLK->PCHCTRL[channel].reg = GCLK_PCHCTRL_GEN(config->source_generator);
}

void system_gclk_chan_disable(const uint8_t channel) {
    system_interrupt_enter_critical_section();

    GCLK->PCHCTRL[channel].bit.CHEN = 0;
    while (GCLK->PCHCTRL[channel].reg & GCLK_GENCTRL_GENEN) {
        /* Wait for clock to become disabled */
    }

    system_interrupt_leave_critical_section();
}

void system_gclk_chan_enable(
    const uint8_t channel) {
    system_interrupt_enter_critical_section();

    /* Enable the generic clock */
    GCLK->PCHCTRL[channel].reg |= GCLK_PCHCTRL_CHEN;

    system_interrupt_leave_critical_section();
}
