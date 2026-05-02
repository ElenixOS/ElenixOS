/**
 * @file eos_port_sem_rtthread.c
 * @brief RT-Thread semaphore implementation
 */

#include "eos_config.h"

#if EOS_RTOS_TYPE == EOS_RTOS_RTTHREAD

#include "eos_port_sem.h"
#include "eos_port.h"

/* Includes ---------------------------------------------------*/
#include <rtthread.h>
#include "eos_log.h"
#include "eos_mem.h"

struct eos_sem_t
{
    rt_sem_t handle;
    uint32_t max;
};

eos_sem_t *eos_sem_create(uint32_t initial_count, uint32_t max_count)
{
    eos_sem_t *sem = eos_malloc_zeroed(sizeof(eos_sem_t));
    if (!sem)
        return NULL;

    sem->handle = rt_sem_create("eos_sem", initial_count, RT_IPC_FLAG_FIFO);
    if (!sem->handle)
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
    if (sem->handle)
        rt_sem_delete(sem->handle);
    eos_free(sem);
}

bool eos_sem_take(eos_sem_t *sem, uint32_t timeout_ms)
{
    if (!sem || !sem->handle)
        return false;

    rt_int32_t ticks = (timeout_ms == EOS_TIMEOUT_INFINITE) ? RT_WAITING_FOREVER : rt_tick_from_millisecond(timeout_ms);
    return rt_sem_take(sem->handle, ticks) == RT_EOK;
}

void eos_sem_give(eos_sem_t *sem)
{
    if (!sem || !sem->handle)
        return;
    rt_sem_release(sem->handle);
}

#endif /* EOS_RTOS_TYPE == EOS_RTOS_RTTHREAD */
