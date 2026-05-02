/**
 * @file eos_port_critical_custom.c
 * @brief Custom RTOS critical section implementation (default fallback)
 *
 * Users must implement eos_critical_enter() and eos_critical_leave()
 * according to their RTOS or hardware:
 *
 *   - On bare metal:     disable/enable interrupts
 *   - On an RTOS:        use the RTOS's scheduler lock / interrupt control
 *   - On other systems:  use the appropriate synchronization primitive
 *
 * The default stubs below do nothing and provide NO protection.
 */

#include "eos_config.h"

#if EOS_RTOS_TYPE == EOS_RTOS_CUSTOM

#include "eos_port_critical.h"

/* Includes ---------------------------------------------------*/
#include "eos_port.h"

EOS_WEAK eos_critical_ctx_t eos_critical_enter(void)
{
    return 0;
}

EOS_WEAK void eos_critical_leave(eos_critical_ctx_t ctx)
{
    (void)ctx;
}

#endif /* EOS_RTOS_TYPE == EOS_RTOS_CUSTOM */
