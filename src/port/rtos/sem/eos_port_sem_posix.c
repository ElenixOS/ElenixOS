/**
 * @file eos_port_sem_posix.c
 * @brief POSIX semaphore implementation
 */

#include "eos_config.h"

#if EOS_RTOS_TYPE == EOS_RTOS_POSIX

#include "eos_port_sem.h"
#include "eos_port.h"

/* Includes ---------------------------------------------------*/
#include <stdlib.h>
#include <semaphore.h>
#include <time.h>
#include <errno.h>
#include "eos_log.h"
#include "eos_mem.h"

struct eos_sem_t
{
    sem_t handle;
    uint32_t max;
};

eos_sem_t *eos_sem_create(uint32_t initial_count, uint32_t max_count)
{
    eos_sem_t *sem = eos_malloc_zeroed(sizeof(eos_sem_t));
    if (!sem)
        return NULL;

    if (sem_init(&sem->handle, 0, initial_count) != 0)
    {
        eos_free(sem);
        return NULL;
    }
    sem->max = max_count;
    return sem;
}

void eos_sem_destroy(eos_sem_t *sem)
{
    if (!sem)
        return;
    sem_destroy(&sem->handle);
    eos_free(sem);
}

bool eos_sem_take(eos_sem_t *sem, uint32_t timeout_ms)
{
    if (!sem)
        return false;

    if (timeout_ms == EOS_TIMEOUT_INFINITE)
        return sem_wait(&sem->handle) == 0;

    if (timeout_ms == 0)
        return sem_trywait(&sem->handle) == 0;

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += timeout_ms / 1000;
    ts.tv_nsec += (timeout_ms % 1000) * 1000000L;
    if (ts.tv_nsec >= 1000000000L)
    {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000L;
    }

    return sem_timedwait(&sem->handle, &ts) == 0;
}

void eos_sem_give(eos_sem_t *sem)
{
    if (!sem)
        return;
    sem_post(&sem->handle);
}

#endif /* EOS_RTOS_TYPE == EOS_RTOS_POSIX */
