/**
 * @file eos_port_sem_bare_metal.c
 * @brief Bare metal semaphore implementation
 */

#include "eos_config.h"

#if EOS_RTOS_TYPE == EOS_RTOS_BARE_METAL

#include "eos_port_sem.h"
#include "eos_port.h"

/* Includes ---------------------------------------------------*/
#include <stdlib.h>
#include "eos_log.h"
#include "eos_mem.h"
#if __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_ATOMICS__)
#include <stdatomic.h>
#define SEM_HAS_ATOMIC 1
#else
#define SEM_HAS_ATOMIC 0
#endif

/*
 * NOTE: On bare metal, semaphore operations use atomic operations or
 * simple count manipulation. For thread-safety in multi-core or
 * interrupt contexts, users should disable interrupts around
 * semaphore operations:
 *
 *     __disable_irq();
 *     // semaphore operations
 *     __enable_irq();
 *
 * Or use the platform's atomic primitive equivalent.
 */

struct eos_sem_t
{
#if SEM_HAS_ATOMIC
    atomic_uint count;
    unsigned int max;
#else
    uint32_t count;
    uint32_t max;
#endif
};

eos_sem_t *eos_sem_create(uint32_t initial_count, uint32_t max_count)
{
    eos_sem_t *sem = eos_malloc(sizeof(eos_sem_t));
    if (!sem)
        return NULL;
#if SEM_HAS_ATOMIC
    atomic_init(&sem->count, initial_count);
    sem->max = max_count;
#else
    sem->count = initial_count;
    sem->max = max_count;
#endif
    return sem;
}

void eos_sem_destroy(eos_sem_t *sem)
{
    if (sem)
        eos_free(sem);
}

bool eos_sem_take(eos_sem_t *sem, uint32_t timeout_ms)
{
    (void)timeout_ms;
    if (!sem)
        return false;
#if SEM_HAS_ATOMIC
    unsigned int c = atomic_load_explicit(&sem->count, memory_order_acquire);
    if (c == 0)
        return false;
    return atomic_compare_exchange_strong(&sem->count, &c, c - 1);
#else
    if (sem->count == 0)
        return false;
    sem->count--;
    return true;
#endif
}

void eos_sem_give(eos_sem_t *sem)
{
    if (!sem)
        return;
#if SEM_HAS_ATOMIC
    unsigned int c = atomic_load_explicit(&sem->count, memory_order_relaxed);
    if (c < sem->max)
        atomic_fetch_add_explicit(&sem->count, 1, memory_order_release);
#else
    if (sem->count < sem->max)
        sem->count++;
#endif
}

#endif /* EOS_RTOS_TYPE == EOS_RTOS_BARE_METAL */
