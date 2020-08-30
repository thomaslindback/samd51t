#ifndef _INTERRUPT_SAM_NVIC_H_
#define _INTERRUPT_SAM_NVIC_H_

#include <Arduino.h>

static volatile uint32_t cpu_irq_critical_section_counter;
static volatile bool     cpu_irq_prev_interrupt_state;
extern volatile bool g_interrupt_enabled;

#define cpu_irq_is_enabled()    (__get_PRIMASK() == 0)
#  define cpu_irq_enable()                     \
	do {                                       \
		g_interrupt_enabled = true;            \
		__DMB();                               \
		__enable_irq();                        \
	} while (0)
#  define cpu_irq_disable()                    \
	do {                                       \
		__disable_irq();                       \
		__DMB();                               \
		g_interrupt_enabled = false;           \
	} while (0)

void cpu_irq_enter_critical(void);
void cpu_irq_leave_critical(void);

static inline void system_interrupt_enter_critical_section(void)
{
	cpu_irq_enter_critical();
}

static inline void system_interrupt_leave_critical_section(void)
{
	cpu_irq_leave_critical();
}

#endif