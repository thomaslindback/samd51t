
#include "samd51_mclk.h"
#include "samd51_gclk.h"
#include "samd51_tc.h"
#include "samd51_tc_interrupt.h"
#include "wiring_private.h"

uint8_t _tc_get_inst_index(Tc *const hw)
{
	/* List of available TC modules. */
	Tc *const tc_modules[TC_INST_NUM] = TC_INSTS;

	/* Find index for TC instance. */
	for (uint32_t i = 0; i < TC_INST_NUM; i++) {
		if (hw == tc_modules[i]) {
			return i;
		}
	}

	return 0;
}

uint32_t tc_get_capture_value(
    const struct tc_module *const module_inst,
    const enum tc_compare_capture_channel channel_index) {
}

bool tc_set_compare_value(
    const tc_module *const module_inst,
    const enum tc_compare_capture_channel channel_index,
    const uint32_t compare_value) {
}

bool tc_init(tc_module *const module_inst,
             Tc *const hw,
             const tc_config *const config) {
    
    /* Temporary variable to hold TC instance number */
    uint8_t instance = _tc_get_inst_index(hw);

    /* Array of GLCK ID for different TC instances */
    uint8_t inst_gclk_id[] = { TC0_GCLK_ID, TC1_GCLK_ID, TC2_GCLK_ID, TC3_GCLK_ID };

    /* Initialize parameters */
    for (uint8_t i = 0; i < TC_CALLBACK_N; i++) {
        module_inst->callback[i] = NULL;
    }
    module_inst->register_callback_mask = 0x00;
    module_inst->enable_callback_mask = 0x00;

    /* Register this instance for callbacks*/
    _tc_instances[instance] = module_inst;

    /* Associate the given device instance with the hardware module */
    module_inst->hw = hw;

    /* Make the counter size variable in the module_inst struct reflect
	 * the counter size in the module
	 */
    module_inst->counter_size = config->counter_size;

    if (hw->COUNT8.CTRLA.reg & TC_CTRLA_SWRST) {
        /* We are in the middle of a reset. Abort. */
        return false;
    }

    if (hw->COUNT8.STATUS.reg & TC_STATUS_SLAVE) {
        /* Module is used as a slave */
        return false;
    }

    if (hw->COUNT8.CTRLA.reg & TC_CTRLA_ENABLE) {
        /* Module must be disabled before initialization. Abort. */
        return false;
    }

    /* Set up the TC PWM out pin for channel 0 */
    if (config->pwm_channel[0].enabled) {
        pinPeripheral(config->pwm_channel[0].pin_out, (EPioType)config->pwm_channel[0].pin_mux);
    }

    /* Set up the TC PWM out pin for channel 1 */
    if (config->pwm_channel[1].enabled) {
        pinPeripheral(config->pwm_channel[0].pin_out, (EPioType)config->pwm_channel[0].pin_mux);
    }

    switch (instance)
    {
    case 0:
        mclk_apb_set_mask(MCLK_APBAMASK, MCLK_APBAMASK_TC0);
        if (config->counter_size == TC_COUNTER_SIZE_32BIT) {
            mclk_apb_set_mask(MCLK_APBAMASK, MCLK_APBAMASK_TC1);
        }
        break;
    case 1:
        mclk_apb_set_mask(MCLK_APBAMASK, MCLK_APBAMASK_TC1);
        break;
    case 2:
        mclk_apb_set_mask(MCLK_APBBMASK, MCLK_APBBMASK_TC2);
        if (config->counter_size == TC_COUNTER_SIZE_32BIT) {
            mclk_apb_set_mask(MCLK_APBBMASK, MCLK_APBBMASK_TC3);
        }
        break;
    case 3:
        mclk_apb_set_mask(MCLK_APBBMASK, MCLK_APBBMASK_TC3);
        break;
    }

    /* Setup clock for module */
    // GCLK->PCHCTRL[inst_gclk_id[instance]].reg =
    //   GCLK_PCHCTRL_GEN_GCLK1_Val |
    //   (1 << GCLK_PCHCTRL_CHEN_Pos); // use clock generator 0

    system_gclk_chan_config gclk_chan_config;
    system_gclk_chan_get_config_defaults(&gclk_chan_config);
    gclk_chan_config.source_generator = config->clock_source;
    system_gclk_chan_set_config(inst_gclk_id[instance], &gclk_chan_config);
    system_gclk_chan_enable(inst_gclk_id[instance]);

    /* Set ctrla register */
    uint32_t ctrla_tmp = 
        (uint32_t)config->counter_size |
        (uint32_t)config->clock_prescaler;

    if (config->run_in_standby) {
        ctrla_tmp |= TC_CTRLA_RUNSTDBY;
    }
    if( config->on_demand) {
        ctrla_tmp |= TC_CTRLA_ONDEMAND;
    }
    for (uint8_t i = 0; i < NUMBER_OF_COMPARE_CAPTURE_CHANNELS; i++) {
        if (config->enable_capture_on_channel[i] == true) {
            ctrla_tmp |= (TC_CTRLA_CAPTEN(1) << i);
        }
    }

    /* Write configuration to register */
    while (tc_is_syncing(module_inst)) {
        /* Wait for sync */
    }
    hw->COUNT8.CTRLA.reg = ctrla_tmp;
    
    /* Temporary variable to hold all updates to the CTRLBSET
	 * register before they are written to it */
    uint8_t ctrlbset_tmp = 0;

    /* Set ctrlb register */
    if (config->oneshot) {
        ctrlbset_tmp = TC_CTRLBSET_ONESHOT;
    }

    if (config->count_direction) {
        ctrlbset_tmp |= TC_CTRLBSET_DIR;
    }

    /* Clear old ctrlb configuration */
    while (tc_is_syncing(module_inst)) {
        /* Wait for sync */
    }
    hw->COUNT8.CTRLBCLR.reg = 0xFF;

    /* Check if we actually need to go into a wait state. */
    if (ctrlbset_tmp) {
        while (tc_is_syncing(module_inst)) {
            /* Wait for sync */
        }
        /* Write configuration to register */
        hw->COUNT8.CTRLBSET.reg = ctrlbset_tmp;
    }

    /* Switch for TC counter size  */
    switch (module_inst->counter_size) {
        case TC_COUNTER_SIZE_8BIT:
            while (tc_is_syncing(module_inst)) {
                /* Wait for sync */
            }

            hw->COUNT8.COUNT.reg =
                config->counter_8_bit.value;

            while (tc_is_syncing(module_inst)) {
                /* Wait for sync */
            }

            hw->COUNT8.PER.reg =
                config->counter_8_bit.period;

            while (tc_is_syncing(module_inst)) {
                /* Wait for sync */
            }

            hw->COUNT8.CC[0].reg =
                config->counter_8_bit.compare_capture_channel[0];

            while (tc_is_syncing(module_inst)) {
                /* Wait for sync */
            }

            hw->COUNT8.CC[1].reg =
                config->counter_8_bit.compare_capture_channel[1];

            hw->COUNT8.WAVE.reg = config->wave_generation;
            return true;

        case TC_COUNTER_SIZE_16BIT:
            while (tc_is_syncing(module_inst)) {
                /* Wait for sync */
            }

            hw->COUNT16.COUNT.reg = config->counter_16_bit.value;

            while (tc_is_syncing(module_inst)) {
                /* Wait for sync */
            }

            hw->COUNT16.CC[0].reg =
                config->counter_16_bit.compare_capture_channel[0];

            while (tc_is_syncing(module_inst)) {
                /* Wait for sync */
            }

            hw->COUNT16.CC[1].reg =
                config->counter_16_bit.compare_capture_channel[1];

            hw->COUNT16.WAVE.reg = config->wave_generation;
            return true;

        case TC_COUNTER_SIZE_32BIT:
            Serial.println("Setup 32-bit");

            while (tc_is_syncing(module_inst)) {
                /* Wait for sync */
            }

            hw->COUNT32.COUNT.reg = config->counter_32_bit.value;
            Serial.print("config->counter_32_bit.value: ");
            Serial.println(config->counter_32_bit.value);
            Serial.print("config->counter_32_bit.compare_capture_channel[0]: ");
            Serial.println(config->counter_32_bit.compare_capture_channel[0]);

            while (tc_is_syncing(module_inst)) {
                /* Wait for sync */
            }

            if(config->counter_32_bit.compare_capture_channel[0] > 0) {
                hw->COUNT32.CC[0].reg = config->counter_32_bit.compare_capture_channel[0];
                while (tc_is_syncing(module_inst)) { }
            }
            if(config->counter_32_bit.compare_capture_channel[1] > 0) {
                hw->COUNT32.CC[1].reg = config->counter_32_bit.compare_capture_channel[1];
                while (tc_is_syncing(module_inst)) { }
            }
            
            hw->COUNT32.WAVE.reg = config->wave_generation;
            return true;
    }
    return false;
}

uint32_t tc_get_count_value(
		const struct tc_module *const module_inst)
{
	/* Get a pointer to the module's hardware instance */
	Tc *const tc_module = module_inst->hw;

	while (tc_is_syncing(module_inst)) {
		/* Wait for sync */
	}

	/* Read from based on the TC counter size */
	switch (module_inst->counter_size) {
		case TC_COUNTER_SIZE_8BIT:
			return (uint32_t)tc_module->COUNT8.COUNT.reg;

		case TC_COUNTER_SIZE_16BIT:
			return (uint32_t)tc_module->COUNT16.COUNT.reg;

		case TC_COUNTER_SIZE_32BIT:
			return tc_module->COUNT32.COUNT.reg;
	}

	return 0;
}