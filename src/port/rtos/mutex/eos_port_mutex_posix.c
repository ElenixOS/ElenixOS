/**
 * @file eos_port_mutex_posix.c
 * @brief POSIX mutex implementation
 */

#include "eos_config.h"

#if EOS_RTOS_TYPE == EOS_RTOS_POSIX

#include "eos_port_mutex.h"
#include "eos_port.h"

/* Includes ---------------------------------------------------*/
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include "eos_log.h"
#include "eos_mem.h"

struct eos_mutex_t
{
    pthread_mutex_t handle;
};

eos_mutex_t *eos_mutex_create(void)
{
    eos_mutex_t *mutex = eos_malloc_zeroed(sizeof(eos_mutex_t));
    if (!mutex)
        return NULL;

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    int ret = pthread_mutex_init(&mutex->handle, &attr);
    pthread_mutexattr_destroy(&attr);

    if (ret != 0)
    {
        eos_free(mutex);
        return NULL;
    }
    return mutex;
}

void eos_mutex_destroy(eos_mutex_t *mutex)
{
    if (!mutex)
        return;
    pthread_mutex_destroy(&mutex->handle);
    eos_free(mutex);
}

bool eos_mutex_lock(eos_mutex_t *mutex, uint32_t timeout_ms)
{
    if (!mutex)
        return false;

    if (timeout_ms == EOS_TIMEOUT_INFINITE)
        return pthread_mutex_lock(&mutex->handle) == 0;

    if (timeout_ms == 0)
        return pthread_mutex_trylock(&mutex->handle) == 0;

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += timeout_ms / 1000;
    ts.tv_nsec += (timeout_ms % 1000) * 1000000L;
    if (ts.tv_nsec >= 1000000000L)
    {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000L;
    }

    return pthread_mutex_timedlock(&mutex->handle, &ts) == 0;
}

bool eos_mutex_unlock(eos_mutex_t *mutex)
{
    if (!mutex)
        return false;
    return pthread_mutex_unlock(&mutex->handle) == 0;
}

#endif /* EOS_RTOS_TYPE == EOS_RTOS_POSIX */
