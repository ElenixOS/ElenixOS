/**
 * @file eos_port_sem_freertos.c
 * @brief FreeRTOS semaphore implementation
 */

#include "eos_config.h"

#if EOS_RTOS_TYPE == EOS_RTOS_FREERTOS

#include "eos_port_sem.h"
#include "eos_port.h"

/* Includes ---------------------------------------------------*/
#include "FreeRTOS.h"
#include "semphr.h"
#include "eos_log.h"
#include "eos_mem.h"

struct eos_sem_t
{
    SemaphoreHandle_t handle;
    uint32_t max;
};

eos_sem_t *eos_sem_create(uint32_t initial_count, uint32_t max_count)
{
    eos_sem_t *sem = eos_malloc_zeroed(sizeof(eos_sem_t));
    if (!sem)
        return NULL;

    if (max_count == 1)
        sem->handle = xSemaphoreCreateBinary();
    else
        sem->handle = xSemaphoreCreateCounting(max_count, initial_count);

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
        vSemaphoreDelete(sem->handle);
    eos_free(sem);
}

bool eos_sem_take(eos_sem_t *sem, uint32_t timeout_ms)
{
    if (!sem || !sem->handle)
        return false;

    TickType_t ticks = (timeout_ms == EOS_TIMEOUT_INFINITE) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    return xSemaphoreTake(sem->handle, ticks) == pdTRUE;
}

void eos_sem_give(eos_sem_t *sem)
{
    if (!sem || !sem->handle)
        return;
    xSemaphoreGive(sem->handle);
}

#endif /* EOS_RTOS_TYPE == EOS_RTOS_FREERTOS */
