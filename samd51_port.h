#ifndef _SAMD_51_PORT_H_
#define _SAMD_51_PORT_H_

#include <Arduino.h>

/**
 *  \brief Port input event action.
 *
 *  List of port input events action on pin.
 */
enum e_port_input_event_action {
    /** Event out to pin */
    PORT_INPUT_EVENT_ACTION_OUT,
    /** Set output register of pin on event */
    PORT_INPUT_EVENT_ACTION_SET,
    /** Clear output register pin on event */
    PORT_INPUT_EVENT_ACTION_CLR,
    /** Toggle output register pin on event */
    PORT_INPUT_EVENT_ACTION_TGL,
};

/**
 *  \brief Port input event.
 *
 *  List of port input events.
 */
enum e_port_input_event {
    /** Port input event 0 */
    PORT_INPUT_EVENT_0,
    /** Port input event 1 */
    PORT_INPUT_EVENT_1,
    /** Port input event 2 */
    PORT_INPUT_EVENT_2,
    /** Port input event 3 */
    PORT_INPUT_EVENT_3,
};

/**
 *  \brief Port input event configuration structure.
 *
 *  Configuration structure for a port input event.
 */
struct s_port_input_event_config{
	/** Port input event action */
	e_port_input_event_action  action;
	/** GPIO pin */
	uint8_t gpio_pin;
};

PortGroup* port_get_port_group(uint32_t pin);

static inline void port_toggle_pin(uint32_t ulPin) {
    // Handle the case the pin isn't usable as PIO
    if (g_APinDescription[ulPin].ulPinType == PIO_NOT_A_PIN) {
        return;
    }

    EPortType port = g_APinDescription[ulPin].ulPort;
    uint32_t pin = g_APinDescription[ulPin].ulPin;
    uint32_t pinMask = (1ul << pin);

    if ((PORT->Group[port].DIRSET.reg & pinMask) == 0) {
        return;
    }

    PORT->Group[port].OUTTGL.reg = pinMask;
}

/**
 *  \brief Enable the port event input.
 *
 *  Enable the port event input with the given pin and event.
 *
 *  \param[in] gpio_pin  Index of the GPIO pin
 *  \param[in] n  Port input event
 *
 * \retval STATUS_ERR_INVALID_ARG  Invalid parameter
 * \retval STATUS_OK               Successfully
 */
static inline bool port_enable_input_event(
    const uint8_t gpio_pin,
    const e_port_input_event n) {

    PortGroup* const port_base = port_get_port_group(gpio_pin);
    switch (n) {
        case PORT_INPUT_EVENT_0:
            port_base->EVCTRL.reg |= PORT_EVCTRL_PORTEI0;
            break;
        case PORT_INPUT_EVENT_1:
            port_base->EVCTRL.reg |= PORT_EVCTRL_PORTEI1;
            break;
        case PORT_INPUT_EVENT_2:
            port_base->EVCTRL.reg |= PORT_EVCTRL_PORTEI2;
            break;
        case PORT_INPUT_EVENT_3:
            port_base->EVCTRL.reg |= PORT_EVCTRL_PORTEI3;
            break;
        default:
            return false;
    }
    return true;
}

/**
 *  \brief Disable the port event input.
 *
 *  Disable the port event input with the given pin and event.
 *
 *  \param[in] gpio_pin  Index of the GPIO pin
 *  \param[in] gpio_pin  Port input event
 *
 * \retval STATUS_ERR_INVALID_ARG  Invalid parameter
 * \retval STATUS_OK               Successfully
 */
static inline bool port_disable_input_event(
    const uint8_t gpio_pin,
    const e_port_input_event n) {

    PortGroup* const port_base = port_get_port_group(gpio_pin);
    switch (n) {
        case PORT_INPUT_EVENT_0:
            port_base->EVCTRL.reg &= ~PORT_EVCTRL_PORTEI0;
            break;
        case PORT_INPUT_EVENT_1:
            port_base->EVCTRL.reg &= ~PORT_EVCTRL_PORTEI1;
            break;
        case PORT_INPUT_EVENT_2:
            port_base->EVCTRL.reg &= ~PORT_EVCTRL_PORTEI2;
            break;
        case PORT_INPUT_EVENT_3:
            port_base->EVCTRL.reg &= ~PORT_EVCTRL_PORTEI3;
            break;
        default:
            return false;
    }
    return true;
}

/**
 * \brief Retrieve the default configuration for port input event.
 *
 * Fills a configuration structure with the default configuration for port input event:
 *   - Event output to pin
 *   - Event action to be executed on PIN 0
 *
 * \param[out] config  Configuration structure to fill with default values
 */
static inline void port_input_event_get_config_defaults(
    s_port_input_event_config *const config) {

    config->action = PORT_INPUT_EVENT_ACTION_OUT;
    config->gpio_pin = 0;
}

/**
 * \brief Configure port input event.
 *
 * Configures port input event with the given configuration settings.
 *
 * \param[in] config  Port input even configuration structure containing the new config
 *
 * \retval STATUS_ERR_INVALID_ARG  Invalid parameter
 * \retval STATUS_OK               Successfully
 */

static inline bool port_input_event_set_config(
    const e_port_input_event n,
    s_port_input_event_config *const config) {

    PortGroup* const port_base = port_get_port_group(config->gpio_pin);
    
    uint8_t pin_nr = g_APinDescription[config->gpio_pin].ulPin;
  
    pinMode(config->gpio_pin, OUTPUT);

    switch (n) {
        case PORT_INPUT_EVENT_0:
            port_base->EVCTRL.reg |= PORT_EVCTRL_EVACT0(config->action) | PORT_EVCTRL_PID0(pin_nr);
            break;
        case PORT_INPUT_EVENT_1:
            port_base->EVCTRL.reg |= PORT_EVCTRL_EVACT1(config->action) | PORT_EVCTRL_PID1(pin_nr);
            break;
        case PORT_INPUT_EVENT_2:
            port_base->EVCTRL.reg |= PORT_EVCTRL_EVACT2(config->action) | PORT_EVCTRL_PID2(pin_nr);
            break;
        case PORT_INPUT_EVENT_3:
            port_base->EVCTRL.reg |= PORT_EVCTRL_EVACT3(config->action) | PORT_EVCTRL_PID3(pin_nr);
            break;
        default:
            return false;
    }
    return true;
}

#endif