/**
 * @file eos_port_mutex_freertos.c
 * @brief FreeRTOS mutex implementation
 */

#include "eos_config.h"

#if EOS_RTOS_TYPE == EOS_RTOS_FREERTOS

#include "eos_port_mutex.h"
#include "eos_port.h"

/* Includes ---------------------------------------------------*/
#include "FreeRTOS.h"
#include "semphr.h"
#include "eos_log.h"
#include "eos_mem.h"

struct eos_mutex_t
{
    SemaphoreHandle_t handle;
};

eos_mutex_t *eos_mutex_create(void)
{
    eos_mutex_t *mutex = eos_malloc_zeroed(sizeof(eos_mutex_t));
    if (!mutex)
        return NULL;

    mutex->handle = xSemaphoreCreateMutex();
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
        vSemaphoreDelete(mutex->handle);
    eos_free(mutex);
}

bool eos_mutex_lock(eos_mutex_t *mutex, uint32_t timeout_ms)
{
    if (!mutex || !mutex->handle)
        return false;

    TickType_t ticks = (timeout_ms == EOS_TIMEOUT_INFINITE) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    return xSemaphoreTake(mutex->handle, ticks) == pdTRUE;
}

bool eos_mutex_unlock(eos_mutex_t *mutex)
{
    if (!mutex || !mutex->handle)
        return false;
    return xSemaphoreGive(mutex->handle) == pdTRUE;
}

#endif /* EOS_RTOS_TYPE == EOS_RTOS_FREERTOS */
