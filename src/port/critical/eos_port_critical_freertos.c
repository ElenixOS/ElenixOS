/**
 * @file eos_port_critical_freertos.c
 * @brief FreeRTOS critical section implementation
 *
 * Uses taskENTER_CRITICAL() / taskEXIT_CRITICAL() to protect
 * critical sections from being interrupted by other tasks
 * and ISRs.
 *
 * NOTE: taskENTER_CRITICAL() can only be called from task context,
 * not from ISRs. For ISR protection, use:
 *   UBaseType_t saved = taskENTER_CRITICAL_FROM_ISR();
 *   taskEXIT_CRITICAL_FROM_ISR(saved);
 */

#include "eos_config.h"

#if EOS_RTOS_TYPE == EOS_RTOS_FREERTOS

#include "eos_port_critical.h"

/* Includes ---------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "eos_port.h"

eos_critical_ctx_t eos_critical_enter(void)
{
    taskENTER_CRITICAL();
    return 1;
}

void eos_critical_leave(eos_critical_ctx_t ctx)
{
    (void)ctx;
    taskEXIT_CRITICAL();
}

#endif /* EOS_RTOS_TYPE == EOS_RTOS_FREERTOS */
