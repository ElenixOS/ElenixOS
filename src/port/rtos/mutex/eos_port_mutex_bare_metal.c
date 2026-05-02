/**
 * @file eos_port_mutex_bare_metal.c
 * @brief Bare metal mutex implementation
 */

#include "eos_config.h"

#if EOS_RTOS_TYPE == EOS_RTOS_BARE_METAL

#include "eos_port_mutex.h"
#include "eos_port.h"

/* Includes ---------------------------------------------------*/
#include <stdlib.h>
#include "eos_log.h"
#include "eos_mem.h"
#if __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_ATOMICS__)
#include <stdatomic.h>
#define MUTEX_HAS_ATOMIC 1
#else
#define MUTEX_HAS_ATOMIC 0
#endif

/*
 * NOTE: On bare metal, mutex is implemented as a simple lock flag.
 *
 * For correct operation in multi-core or interrupt contexts,
 * users MUST disable interrupts before mutex operations:
 *
 *     uint32_t saved = __disable_irq_save();
 *     // mutex operations
 *     __enable_irq_restore(saved);
 *
 * Or use the platform's atomic/test-and-set primitive.
 */

struct eos_mutex_t
{
#if MUTEX_HAS_ATOMIC
    atomic_int locked;
#else
    volatile uint8_t locked;
#endif
};

eos_mutex_t *eos_mutex_create(void)
{
    eos_mutex_t *mutex = eos_malloc_zeroed(sizeof(eos_mutex_t));
    if (!mutex)
        return NULL;
    return mutex;
}

void eos_mutex_destroy(eos_mutex_t *mutex)
{
    if (mutex)
        eos_free(mutex);
}

bool eos_mutex_lock(eos_mutex_t *mutex, uint32_t timeout_ms)
{
    (void)timeout_ms;
    if (!mutex)
        return false;
#if MUTEX_HAS_ATOMIC
    int expected = 0;
    return atomic_compare_exchange_strong(&mutex->locked, &expected, 1);
#else
    if (mutex->locked)
        return false;
    mutex->locked = 1;
    return true;
#endif
}

bool eos_mutex_unlock(eos_mutex_t *mutex)
{
    if (!mutex)
        return false;
#if MUTEX_HAS_ATOMIC
    int expected = 1;
    return atomic_compare_exchange_strong(&mutex->locked, &expected, 0);
#else
    if (!mutex->locked)
        return false;
    mutex->locked = 0;
    return true;
#endif
}

#endif /* EOS_RTOS_TYPE == EOS_RTOS_BARE_METAL */
