#ifndef _SAMD51_GCLK_H_
#define _SAMD51_GCLK_H_

#include <Arduino.h>

enum gclk_generator {
    GCLK_GENERATOR_0,
    GCLK_GENERATOR_1,
    GCLK_GENERATOR_2,
    GCLK_GENERATOR_3,
    GCLK_GENERATOR_4,
    GCLK_GENERATOR_5,
    GCLK_GENERATOR_6,
    GCLK_GENERATOR_7,
    GCLK_GENERATOR_8,
    GCLK_GENERATOR_9,
    GCLK_GENERATOR_10,
    GCLK_GENERATOR_11
};

struct system_gclk_chan_config {
    /** Generic Clock Generator source channel */
    enum gclk_generator source_generator;
};

inline void system_gclk_chan_get_config_defaults(
    system_gclk_chan_config *const config) {
    /* Default configuration values */
    config->source_generator = GCLK_GENERATOR_0;
}

void system_gclk_chan_disable(const uint8_t channel);
void system_gclk_chan_enable(const uint8_t channel);
void system_gclk_chan_set_config(const uint8_t channel, system_gclk_chan_config *const config);

#endif // _SAMD51_GCLK_H_