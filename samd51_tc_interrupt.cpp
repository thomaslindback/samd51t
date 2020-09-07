/**
 * \file
 *
 * \brief SAM TC - Timer Counter Callback Driver
 *
 * Copyright (c) 2013-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */

/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#include "samd51_tc_interrupt.h"
#include "utils.h"

std::array<tc_module*, TC_INST_NUM> _tc_instances; 
//void *_tc_instances[TC_INST_NUM];

void _tc_interrupt_handler(uint8_t instance);

/**
 * \brief Registers a callback.
 *
 * Registers a callback function which is implemented by the user.
 *
 * \note The callback must be enabled by \ref tc_enable_callback,
 * in order for the interrupt handler to call it when the conditions for the
 * callback type is met.
 *
 * \param[in]     module        Pointer to TC software instance struct
 * \param[in]     callback_func Pointer to callback function
 * \param[in]     callback_type Callback type given by an enum
 */
bool tc_register_callback(
    tc_module *const module,
    tc_callback_t callback_func,
    tc_callback callback_type) {
    /* Register callback function */
    module->callback[callback_type] = callback_func;

    /* Set the bit corresponding to the callback_type */
    if (callback_type == TC_CALLBACK_CC_CHANNEL0) {
        module->register_callback_mask |= TC_INTFLAG_MC(1);
    } else if (callback_type == TC_CALLBACK_CC_CHANNEL1) {
        module->register_callback_mask |= TC_INTFLAG_MC(2);
    } else {
        module->register_callback_mask |= (1 << callback_type);
    }
    return true;
}

/**
 * \brief Unregisters a callback.
 *
 * Unregisters a callback function implemented by the user. The callback should be
 * disabled before it is unregistered.
 *
 * \param[in]     module        Pointer to TC software instance struct
 * \param[in]     callback_type Callback type given by an enum
 */
bool tc_unregister_callback(
    struct tc_module *const module,
    const enum tc_callback callback_type) {
    /* Unregister callback function */
    module->callback[callback_type] = NULL;

    /* Clear the bit corresponding to the callback_type */
    if (callback_type == TC_CALLBACK_CC_CHANNEL0) {
        module->register_callback_mask &= ~TC_INTFLAG_MC(1);
    } else if (callback_type == TC_CALLBACK_CC_CHANNEL1) {
        module->register_callback_mask &= ~TC_INTFLAG_MC(2);
    } else {
        module->register_callback_mask &= ~(1 << callback_type);
    }
    return true;
}

/**
 * \internal ISR handler for TC
 *
 * Auto-generate a set of interrupt handlers for each TC in the device.
 */
#define _TC_INTERRUPT_HANDLER(n, m) \
    void TC##n##_Handler(void) {    \
        _tc_interrupt_handler(m);   \
    }

#ifdef _SAMD51G19A_
_TC_INTERRUPT_HANDLER(0, 0)
_TC_INTERRUPT_HANDLER(1, 1)
_TC_INTERRUPT_HANDLER(2, 2)
_TC_INTERRUPT_HANDLER(3, 3)
#endif

/**
 * \internal Interrupt Handler for TC module
 *
 * Handles interrupts as they occur, it will run the callback functions
 * that are registered and enabled.
 *
 * \param[in]  instance  ID of the TC instance calling the interrupt
 *                       handler
 */
void _tc_interrupt_handler(
    uint8_t instance) {

    // Serial.print("_tc_interrupt_handler. instance = ");
    // Serial.println(instance);

    /* Temporary variable */
    uint8_t interrupt_and_callback_status_mask;

    /* Get device instance from the look-up table */
    tc_module *module = _tc_instances[instance];

    /* Read and mask interrupt flag register */
    interrupt_and_callback_status_mask = module->hw->COUNT8.INTFLAG.reg &
                                         module->register_callback_mask &
                                         module->enable_callback_mask;
    // Serial.print("INTFLAG.reg: ");                                     
    // printReg<8>(module->hw->COUNT8.INTFLAG.reg);
    // Serial.println("");
    // Serial.print("interrupt_and_callback_status_mask: ");
    // printReg<8>(interrupt_and_callback_status_mask);
    // Serial.println("");

    /* Check if an Overflow interrupt has occurred */
    if (interrupt_and_callback_status_mask & TC_INTFLAG_OVF) {
        //Serial.println("interrupt_and_callback_status_mask & TC_INTFLAG_OVF");
        /* Invoke registered and enabled callback function */
        (module->callback[TC_CALLBACK_OVERFLOW])(module);
        /* Clear interrupt flag */
        module->hw->COUNT8.INTFLAG.reg = TC_INTFLAG_OVF;
    }

    /* Check if an Error interrupt has occurred */
    if (interrupt_and_callback_status_mask & TC_INTFLAG_ERR) {
        //Serial.println("interrupt_and_callback_status_mask & TC_INTFLAG_ERR");
        /* Invoke registered and enabled callback function */
        (module->callback[TC_CALLBACK_ERROR])(module);
        /* Clear interrupt flag */
        module->hw->COUNT8.INTFLAG.reg = TC_INTFLAG_ERR;
    }

    /* Check if an Match/Capture Channel 0 interrupt has occurred */
    if (interrupt_and_callback_status_mask & TC_INTFLAG_MC0) {
        //Serial.println("interrupt_and_callback_status_mask & TC_INTFLAG_MC0");
        /* Invoke registered and enabled callback function */
        (module->callback[TC_CALLBACK_CC_CHANNEL0])(module);
        /* Clear interrupt flag */
        module->hw->COUNT8.INTFLAG.reg = TC_INTFLAG_MC0;
    }

    /* Check if an Match/Capture Channel 1 interrupt has occurred */
    if (interrupt_and_callback_status_mask & TC_INTFLAG_MC1) {
        //Serial.println("interrupt_and_callback_status_mask & TC_INTFLAG_MC1");
        /* Invoke registered and enabled callback function */
        (module->callback[TC_CALLBACK_CC_CHANNEL1])(module);
        /* Clear interrupt flag */
        module->hw->COUNT8.INTFLAG.reg = TC_INTFLAG_MC1;
    }
}
