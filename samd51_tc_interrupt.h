
#ifndef _SAMD_51_TC_INTERRUPT_H_
#define _SAMD_51_TC_INTERRUPT_H_

#include "samd51_tc.h"

extern void *_tc_instances[TC_INST_NUM];

bool tc_register_callback(
		struct tc_module *const module,
		tc_callback_t callback_func,
		const enum tc_callback callback_type);

bool tc_unregister_callback(
		struct tc_module *const module,
		const enum tc_callback callback_type);

static inline void tc_enable_callback(
		tc_module *const module,
		const enum tc_callback callback_type)
{
	IRQn_Type _irqs[] = {
      TC0_IRQn,
      TC1_IRQn,
      TC2_IRQn,
      TC3_IRQn,
    };

	/* Enable interrupts for this TC module */
	uint8_t instance = _tc_get_inst_index(module->hw);
	NVIC_ClearPendingIRQ(_irqs[instance]);
    NVIC_EnableIRQ(_irqs[instance]);

//	system_interrupt_enable(_tc_interrupt_get_interrupt_vector(_tc_get_inst_index(module->hw)));

	/* Enable callback */
	if (callback_type == TC_CALLBACK_CC_CHANNEL0) {
		module->enable_callback_mask |= TC_INTFLAG_MC0;
		module->hw->COUNT8.INTENSET.reg = TC_INTFLAG_MC0;
	}
	else if (callback_type == TC_CALLBACK_CC_CHANNEL1) {
		module->enable_callback_mask |= TC_INTFLAG_MC1;
		module->hw->COUNT8.INTENSET.reg = TC_INTFLAG_MC1;
	}
	else {
		module->enable_callback_mask |= (1 << callback_type);
		module->hw->COUNT8.INTENSET.reg = (1 << callback_type);
	}
}

/**
 * \brief Disables callback.
 *
 * Disables the callback function registered by the \ref
 * tc_register_callback, and the callback will not be called from the
 * interrupt routine. The function will also disable the appropriate
 * interrupts.
 *
 * \param[in]     module        Pointer to TC software instance struct
 * \param[in]     callback_type Callback type given by an enum
 */
static inline void tc_disable_callback(
		struct tc_module *const module,
		const enum tc_callback callback_type){

	/* Disable callback */
	if (callback_type == TC_CALLBACK_CC_CHANNEL0) {
		module->hw->COUNT8.INTENCLR.reg = TC_INTFLAG_MC(1);
		module->enable_callback_mask &= ~TC_INTFLAG_MC(1);
	}
	else if (callback_type == TC_CALLBACK_CC_CHANNEL1) {
		module->hw->COUNT8.INTENCLR.reg = TC_INTFLAG_MC(2);
		module->enable_callback_mask &= ~TC_INTFLAG_MC(2);
	}
	else {
		module->hw->COUNT8.INTENCLR.reg = (1 << callback_type);
		module->enable_callback_mask &= ~(1 << callback_type);
	}
}


#endif /* _SAMD_51_TC_INTERRUPT_H_ */
