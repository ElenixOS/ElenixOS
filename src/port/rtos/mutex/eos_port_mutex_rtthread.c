/**
 * @file eos_port_mutex_rtthread.c
 * @brief RT-Thread mutex implementation
 */

#include "eos_config.h"

#if EOS_RTOS_TYPE == EOS_RTOS_RTTHREAD

#include "eos_port_mutex.h"
#include "eos_port.h"

/* Includes ---------------------------------------------------*/
#include <rtthread.h>
#include "eos_log.h"
#include "eos_mem.h"

struct eos_mutex_t
{
    rt_mutex_t handle;
};

eos_mutex_t *eos_mutex_create(void)
{
    eos_mutex_t *mutex = eos_malloc_zeroed(sizeof(eos_mutex_t));
    if (!mutex)
        return NULL;

    mutex->handle = rt_mutex_create("eos_mutex", RT_IPC_FLAG_FIFO);
    if (!mutex->handle)
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
    if (mutex->handle)
        rt_mutex_delete(mutex->handle);
    eos_free(mutex);
}

bool eos_mutex_lock(eos_mutex_t *mutex, uint32_t timeout_ms)
{
    if (!mutex || !mutex->handle)
        return false;

    rt_int32_t ticks = (timeout_ms == EOS_TIMEOUT_INFINITE) ? RT_WAITING_FOREVER : rt_tick_from_millisecond(timeout_ms);
    return rt_mutex_take(mutex->handle, ticks) == RT_EOK;
}

bool eos_mutex_unlock(eos_mutex_t *mutex)
{
    if (!mutex || !mutex->handle)
        return false;
    return rt_mutex_release(mutex->handle) == RT_EOK;
}

#endif /* EOS_RTOS_TYPE == EOS_RTOS_RTTHREAD */
