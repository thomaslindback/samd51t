#ifndef _SAMD_51_TC_H_
#define _SAMD_51_TC_H_

#include <Arduino.h>
#include <array>
#include "samd51_gclk.h"
#include "utils.h"

#define NUMBER_OF_COMPARE_CAPTURE_CHANNELS TC0_CC_NUM

struct tc_module;

/** Type of the callback functions. */
using tc_callback_t = void (*)(tc_module* const module);

enum tc_compare_capture_channel {
    /** Index of compare capture channel 0 */
    TC_COMPARE_CAPTURE_CHANNEL_0,
    /** Index of compare capture channel 1 */
    TC_COMPARE_CAPTURE_CHANNEL_1,
};

/** Enum for the possible callback types for the TC module. */
enum tc_callback {
    /** Callback for TC overflow */
    TC_CALLBACK_OVERFLOW,
    /** Callback for capture overflow error */
    TC_CALLBACK_ERROR,
    /** Callback for capture compare channel 0 */
    TC_CALLBACK_CC_CHANNEL0,
    /** Callback for capture compare channel 1 */
    TC_CALLBACK_CC_CHANNEL1,
    /** Number of available callbacks */
    TC_CALLBACK_N,
};

enum tc_clock_prescaler {
    /** Divide clock by 1 */
    TC_CLOCK_PRESCALER_DIV1 = TC_CTRLA_PRESCALER(0),
    /** Divide clock by 2 */
    TC_CLOCK_PRESCALER_DIV2 = TC_CTRLA_PRESCALER(1),
    /** Divide clock by 4 */
    TC_CLOCK_PRESCALER_DIV4 = TC_CTRLA_PRESCALER(2),
    /** Divide clock by 8 */
    TC_CLOCK_PRESCALER_DIV8 = TC_CTRLA_PRESCALER(3),
    /** Divide clock by 16 */
    TC_CLOCK_PRESCALER_DIV16 = TC_CTRLA_PRESCALER(4),
    /** Divide clock by 64 */
    TC_CLOCK_PRESCALER_DIV64 = TC_CTRLA_PRESCALER(5),
    /** Divide clock by 256 */
    TC_CLOCK_PRESCALER_DIV256 = TC_CTRLA_PRESCALER(6),
    /** Divide clock by 1024 */
    TC_CLOCK_PRESCALER_DIV1024 = TC_CTRLA_PRESCALER(7),
};

enum tc_counter_size {
    /** The counter's maximum value is 0xFF, the period register is
	 * available to be used as top value
	 */
    TC_COUNTER_SIZE_8BIT = TC_CTRLA_MODE_COUNT8,

    /** The counter's maximum value is 0xFFFF. There is no separate
	 * period register, to modify top one of the capture compare
	 * registers has to be used. This limits the amount of
	 * available channels.
	 */
    TC_COUNTER_SIZE_16BIT = TC_CTRLA_MODE_COUNT16,

    /** The counter's maximum value is 0xFFFFFFFF. There is no separate
	 * period register, to modify top one of the capture compare
	 * registers has to be used. This limits the amount of
	 * available channels.
	 */
    TC_COUNTER_SIZE_32BIT = TC_CTRLA_MODE_COUNT32,
};

/**
 * \brief Configuration struct for TC module in 8-bit size counter mode.
 */
struct tc_8bit_config {
    /** Initial timer count value */
    uint8_t value;
    /** Where to count to or from depending on the direction on the counter */
    uint8_t period;
    /** Value to be used for compare match on each channel */
    uint8_t compare_capture_channel[NUMBER_OF_COMPARE_CAPTURE_CHANNELS];
};

/**
 * \brief Configuration struct for TC module in 16-bit size counter mode.
 */
struct tc_16bit_config {
    /** Initial timer count value */
    uint16_t value;
    /** Value to be used for compare match on each channel */
    uint16_t compare_capture_channel[NUMBER_OF_COMPARE_CAPTURE_CHANNELS];
};

/**
 * \brief Configuration struct for TC module in 32-bit size counter mode.
 */
struct tc_32bit_config {
    /** Initial timer count value */
    uint32_t value;
    /** Value to be used for compare match on each channel */
    uint32_t compare_capture_channel[NUMBER_OF_COMPARE_CAPTURE_CHANNELS];
};

/** TC wave generation mode: normal frequency. */
#define TC_WAVE_GENERATION_NORMAL_FREQ_MODE TC_WAVE_WAVEGEN_NFRQ
/** TC wave generation mode: match frequency. */
#define TC_WAVE_GENERATION_MATCH_FREQ_MODE TC_WAVE_WAVEGEN_MFRQ
/** TC wave generation mode: normal PWM. */
#define TC_WAVE_GENERATION_NORMAL_PWM_MODE TC_WAVE_WAVEGEN_NPWM
/** TC wave generation mode: match PWM. */
#define TC_WAVE_GENERATION_MATCH_PWM_MODE TC_WAVE_WAVEGEN_MPWM

enum tc_wave_generation {
    /** Top is maximum, except in 8-bit counter size where it is the PER
	 * register
	 */
    TC_WAVE_GENERATION_NORMAL_FREQ = TC_WAVE_GENERATION_NORMAL_FREQ_MODE,

    /** Top is CC0, except in 8-bit counter size where it is the PER
	 * register
	 */
    TC_WAVE_GENERATION_MATCH_FREQ = TC_WAVE_GENERATION_MATCH_FREQ_MODE,

    /** Top is maximum, except in 8-bit counter size where it is the PER
	 * register
	 */
    TC_WAVE_GENERATION_NORMAL_PWM = TC_WAVE_GENERATION_NORMAL_PWM_MODE,

    /** Top is CC0, except in 8-bit counter size where it is the PER
	 * register
	 */
    TC_WAVE_GENERATION_MATCH_PWM = TC_WAVE_GENERATION_MATCH_PWM_MODE,
};

/**
 * This enum specify how the counter and prescaler should reload.
 */
enum tc_reload_action {
    /** The counter is reloaded/reset on the next GCLK and starts
	 * counting on the prescaler clock
	 */
    TC_RELOAD_ACTION_GCLK = TC_CTRLA_PRESCSYNC_GCLK,

    /** The counter is reloaded/reset on the next prescaler clock
	 */
    TC_RELOAD_ACTION_PRESC = TC_CTRLA_PRESCSYNC_PRESC,

    /** The counter is reloaded/reset on the next GCLK, and the
	 * prescaler is restarted as well
	 */
    TC_RELOAD_ACTION_RESYNC = TC_CTRLA_PRESCSYNC_RESYNC
};

enum tc_count_direction {
    /** Timer should count upward from zero to MAX */
    TC_COUNT_DIRECTION_UP,

    /** Timer should count downward to zero from MAX */
    TC_COUNT_DIRECTION_DOWN
};

/**
 * \name Waveform Inversion Mode
 */
/** Waveform inversion CC0 mode. */
#define TC_WAVEFORM_INVERT_CC0_MODE  TC_DRVCTRL_INVEN0
/** Waveform inversion CC1 mode. */
#define TC_WAVEFORM_INVERT_CC1_MODE  TC_DRVCTRL_INVEN0

/**
 * \brief Waveform inversion mode.
 *
 * Output waveform inversion mode.
 */
enum tc_waveform_invert_output {
	/** No inversion of the waveform output */
	TC_WAVEFORM_INVERT_OUTPUT_NONE      = 0,
	/** Invert output from compare channel 0 */
	TC_WAVEFORM_INVERT_OUTPUT_CHANNEL_0 = TC_WAVEFORM_INVERT_CC0_MODE,
	/** Invert output from compare channel 1 */
	TC_WAVEFORM_INVERT_OUTPUT_CHANNEL_1 = TC_WAVEFORM_INVERT_CC1_MODE
};
/**
 * \brief Configuration struct for TC module in 32-bit size counter mode.
 */
struct tc_pwm_channel {
    /** When \c true, PWM output for the given channel is enabled */
    bool enabled;
    /** Specifies pin output for each channel */
    uint32_t pin_out;
    /** Specifies Multiplexer (MUX) setting for each output channel pin */
    uint32_t pin_mux;
};

struct tc_config {
    /** GCLK generator used to clock the peripheral */
    enum gclk_generator clock_source;

    bool run_in_standby;
    bool on_demand;
    /** Specifies either 8-, 16-, or 32-bit counter size */
    enum tc_counter_size counter_size;
    /** Specifies the prescaler value for GCLK_TC */
    enum tc_clock_prescaler clock_prescaler;
    /** Specifies which waveform generation mode to use */
    enum tc_wave_generation wave_generation;

    /** Specifies the reload or reset time of the counter and prescaler
	 *  resynchronization on a re-trigger event for the TC
	 */
    enum tc_reload_action reload_action;

    /** Specifies which channel(s) to invert the waveform on. */
    uint8_t waveform_invert_output;

    /** Specifies which channel(s) to enable channel capture
	 *  operation on
	 */
    bool enable_capture_on_channel[NUMBER_OF_COMPARE_CAPTURE_CHANNELS];
    /** Specifies which channel(s) to enable I/O capture
	 *  operation on
	 */
    bool enable_capture_on_IO[NUMBER_OF_COMPARE_CAPTURE_CHANNELS];

    /** When \c true, one-shot will stop the TC on next hardware or software
	 *  re-trigger event or overflow/underflow
	 */
    bool oneshot;

    /** Specifies the direction for the TC to count */
    enum tc_count_direction count_direction;

    /** Specifies the PWM channel for TC */
    struct tc_pwm_channel pwm_channel[NUMBER_OF_COMPARE_CAPTURE_CHANNELS];

    /** Access the different counter size settings through this configuration member. */
    union {
        /** Struct for 8-bit specific timer configuration */
        tc_8bit_config counter_8_bit;
        /** Struct for 16-bit specific timer configuration */
        tc_16bit_config counter_16_bit;
        /** Struct for 32-bit specific timer configuration */
        tc_32bit_config counter_32_bit;
    };

    bool double_buffering_enabled;
};

struct tc_module {
    /** Hardware module pointer of the associated Timer/Counter peripheral */
    Tc *hw;

    /** Size of the initialized Timer/Counter module configuration */
    enum tc_counter_size counter_size;
    /** Array of callbacks */
    tc_callback_t callback[TC_CALLBACK_N];
    /** Bit mask for callbacks registered */
    uint8_t register_callback_mask;
    /** Bit mask for callbacks enabled */
    uint8_t enable_callback_mask;
    /** Set to \c true to enable double buffering write */
    bool double_buffering_enabled;
};

/**
 * \brief Action to perform when the TC module is triggered by an event.
 *
 * Event action to perform when the module is triggered by an event.
 */
enum tc_event_action {
	/** No event action */
	TC_EVENT_ACTION_OFF                 = TC_EVCTRL_EVACT_OFF,
	/** Re-trigger on event */
	TC_EVENT_ACTION_RETRIGGER           = TC_EVCTRL_EVACT_RETRIGGER,
	/** Increment counter on event */
	TC_EVENT_ACTION_INCREMENT_COUNTER   = TC_EVCTRL_EVACT_COUNT,
	/** Start counter on event */
	TC_EVENT_ACTION_START               = TC_EVCTRL_EVACT_START,

	/** Store period in capture register 0, pulse width in capture
	 *  register 1
	 */
	TC_EVENT_ACTION_PPW                 = TC_EVCTRL_EVACT_PPW,

	/** Store pulse width in capture register 0, period in capture
	 *  register 1
	 */
	TC_EVENT_ACTION_PWP                 = TC_EVCTRL_EVACT_PWP,
	/** Time stamp capture */
	TC_EVENT_ACTION_STAMP               = TC_EVCTRL_EVACT_STAMP,
	/** Pulse width capture */
	TC_EVENT_ACTION_PW                  = TC_EVCTRL_EVACT_PW,
};

struct tc_events {
	/** Generate an output event on a compare channel match */
	bool generate_event_on_compare_channel[NUMBER_OF_COMPARE_CAPTURE_CHANNELS] = {false, false};
	/** Generate an output event on counter overflow */
	bool generate_event_on_overflow = false;
	/** Perform the configured event action when an incoming event is signalled */
	bool on_event_perform_action = false;
	/** Specifies if the input event source is inverted, when used in PWP or
	 *  PPW event action modes
	 */
	bool invert_event_input = false;
	/** Specifies which event to trigger if an event is triggered */
	enum tc_event_action event_action = TC_EVENT_ACTION_OFF;
};

uint8_t _tc_get_inst_index(Tc *const hw);

/**
 * Initializes config with predefined default values.
 *
 * This function will initialize a given TC configuration structure to
 * a set of known default values. This function should be called on
 * any new instance of the configuration structures before being
 * modified by the user application.
 *
 * The default configuration is as follows:
 *  \li GCLK generator 0 (GCLK main) clock source
 *  \li 16-bit counter size on the counter
 *  \li No prescaler
 *  \li Normal frequency wave generation
 *  \li GCLK reload action
 *  \li Don't run in standby
 *  \li Don't run on demand for SAM L21/L22/C20/C21
 *  \li No inversion of waveform output
 *  \li No capture enabled
 *  \li No I/O capture enabled for SAM L21/L22/C20/C21
 *  \li No event input enabled
 *  \li Count upward
 *  \li Don't perform one-shot operations
 *  \li No event action
 *  \li No channel 0 PWM output
 *  \li No channel 1 PWM output
 *  \li Counter starts on 0
 *  \li Capture compare channel 0 set to 0
 *  \li Capture compare channel 1 set to 0
 *  \li No PWM pin output enabled
 *  \li Pin and MUX configuration not set
 *  \li Double buffer disabled (if have this feature)
 *
 * \param[out]  config  Pointer to a TC module configuration structure to set
 */
static inline void tc_get_config_defaults(
		struct tc_config *const config)
{

	/* Write default config to config struct */
	config->clock_source               = GCLK_GENERATOR_0;
	config->counter_size               = TC_COUNTER_SIZE_16BIT;
	config->clock_prescaler            = TC_CLOCK_PRESCALER_DIV1;
	config->wave_generation            = TC_WAVE_GENERATION_NORMAL_FREQ;
	config->reload_action              = TC_RELOAD_ACTION_GCLK;
	config->run_in_standby             = false;
	config->on_demand                  = false;
	config->waveform_invert_output     = TC_WAVEFORM_INVERT_OUTPUT_NONE;
	config->enable_capture_on_channel[TC_COMPARE_CAPTURE_CHANNEL_0] = false;
	config->enable_capture_on_channel[TC_COMPARE_CAPTURE_CHANNEL_1] = false;
	config->enable_capture_on_IO[TC_COMPARE_CAPTURE_CHANNEL_0] = false;
	config->enable_capture_on_IO[TC_COMPARE_CAPTURE_CHANNEL_1] = false;
	config->count_direction            = TC_COUNT_DIRECTION_UP;
	config->oneshot                    = false;
	config->pwm_channel[TC_COMPARE_CAPTURE_CHANNEL_0].enabled = false;
	config->pwm_channel[TC_COMPARE_CAPTURE_CHANNEL_0].pin_out = 0;
	config->pwm_channel[TC_COMPARE_CAPTURE_CHANNEL_0].pin_mux = 0;
	config->pwm_channel[TC_COMPARE_CAPTURE_CHANNEL_1].enabled = false;
	config->pwm_channel[TC_COMPARE_CAPTURE_CHANNEL_1].pin_out = 0;
	config->pwm_channel[TC_COMPARE_CAPTURE_CHANNEL_1].pin_mux = 0;
	config->counter_16_bit.value                   = 0x0000;
	config->counter_16_bit.compare_capture_channel[TC_COMPARE_CAPTURE_CHANNEL_0] = 0x0000;
	config->counter_16_bit.compare_capture_channel[TC_COMPARE_CAPTURE_CHANNEL_1] = 0x0000;
	config->double_buffering_enabled = false;
}

static inline bool tc_is_syncing(
    const struct tc_module *const module_inst) {
    /* Get a pointer to the module's hardware instance */
    TcCount8 *const tc_module = &(module_inst->hw->COUNT8);

    return (tc_module->SYNCBUSY.reg & TC_SYNCBUSY_ENABLE);
}

bool tc_init(
    struct tc_module *const module_inst,
    Tc *const hw,
    const struct tc_config *const config);

static inline void tc_enable(
    const struct tc_module *const module_inst) {
    /* Get a pointer to the module's hardware instance */
    TcCount8 *const tc_module = &(module_inst->hw->COUNT8);

    while (tc_is_syncing(module_inst)) {
        /* Wait for sync */
    }

    /* Enable TC module */
    tc_module->CTRLA.reg |= TC_CTRLA_ENABLE;
}

/**
 * \brief Disables the TC module.
 *
 * Disables a TC module and stops the counter.
 *
 * \param[in]  module_inst   Pointer to the software module instance struct
 */
static inline void tc_disable(
		const tc_module *const module_inst)
{
	/* Get a pointer to the module's hardware instance */
	TcCount8 *const tc_module = &(module_inst->hw->COUNT8);

	while (tc_is_syncing(module_inst)) {
		/* Wait for sync */
	}

	/* Disbale interrupt */
	tc_module->INTENCLR.reg = TC_INTENCLR_MASK;
	/* Clear interrupt flag */
	tc_module->INTFLAG.reg = TC_INTFLAG_MASK;

	/* Disable TC module */
	tc_module->CTRLA.reg  &= ~TC_CTRLA_ENABLE;
}

uint32_t tc_get_capture_value(
    const struct tc_module *const module_inst,
    const enum tc_compare_capture_channel channel_index);

bool tc_set_compare_value(
    const struct tc_module *const module_inst,
    const enum tc_compare_capture_channel channel_index,
    const uint32_t compare_value);

uint32_t tc_get_count_value(
		const struct tc_module *const module_inst);

/**
 * \brief Enables a TC module event input or output.
 *
 * Enables one or more input or output events to or from the TC module.
 * See \ref tc_events for a list of events this module supports.
 *
 * \note Events cannot be altered while the module is enabled.
 *
 * \param[in]  module_inst  Pointer to the software module instance struct
 * \param[in]  events       Struct containing flags of events to enable
 */
static inline void tc_enable_events(
		struct tc_module *const module_inst,
		struct tc_events *const events)
{

	Tc *const tc_module = module_inst->hw;

	uint16_t event_mask = 0;

	if (events->invert_event_input == true) {
        Serial.println("events->invert_event_input == true");
		event_mask |= TC_EVCTRL_TCINV;
	}

	if (events->on_event_perform_action == true) {
        Serial.println("vents->on_event_perform_action == true");
		event_mask |= TC_EVCTRL_TCEI;
	}

	if (events->generate_event_on_overflow == true) {
        Serial.println("events->generate_event_on_overflow == true");
		event_mask |= TC_EVCTRL_OVFEO;
	}

	for (uint8_t i = 0; i < NUMBER_OF_COMPARE_CAPTURE_CHANNELS; i++) {
		if (events->generate_event_on_compare_channel[i] == true) {
            Serial.println("events->generate_event_on_compare_channel[i] == true");
			event_mask |= (TC_EVCTRL_MCEO(1) << i);
		}
	}
    Serial.print("event_mask: ");
    printReg<16>(event_mask, "\n");
    Serial.print("event_mask | events->event_action: ");
    printReg<16>(event_mask | events->event_action, "\n");

	tc_module->COUNT8.EVCTRL.reg |= (event_mask | events->event_action);

    Serial.print("tc_module->COUNT8.EVCTRL.reg: ");
    printReg<16>(tc_module->COUNT8.EVCTRL.reg, "\n");

}

/**
 * \brief Disables a TC module event input or output.
 *
 * Disables one or more input or output events to or from the TC module.
 * See \ref tc_events for a list of events this module supports.
 *
 * \note Events cannot be altered while the module is enabled.
 *
 * \param[in]  module_inst  Pointer to the software module instance struct
 * \param[in]  events       Struct containing flags of events to disable
 */
static inline void tc_disable_events(
		struct tc_module *const module_inst,
		struct tc_events *const events)
{
	Tc *const tc_module = module_inst->hw;

	uint32_t event_mask = 0;

	if (events->invert_event_input == true) {
		event_mask |= TC_EVCTRL_TCINV;
	}

	if (events->on_event_perform_action == true) {
		event_mask |= TC_EVCTRL_TCEI;
	}

	if (events->generate_event_on_overflow == true) {
		event_mask |= TC_EVCTRL_OVFEO;
	}

	for (uint8_t i = 0; i < NUMBER_OF_COMPARE_CAPTURE_CHANNELS; i++) {
		if (events->generate_event_on_compare_channel[i] == true) {
			event_mask |= (TC_EVCTRL_MCEO(1) << i);
		}
	}

	tc_module->COUNT8.EVCTRL.reg &= ~event_mask;
}

struct samd51_tc {
    samd51_tc(Tc *const hw, tc_counter_size counter_size) : _hw(hw), _counter_size(counter_size) {
    }

    Tc *_hw;
    tc_counter_size _counter_size;
};

#endif
