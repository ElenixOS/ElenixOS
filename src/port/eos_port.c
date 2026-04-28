/**
 * @file eos_port.c
 * @brief ElenixOS porting
 */

#include "eos_port.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eos_log.h"
#include "eos_mem.h"
#if __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_ATOMICS__)
#include <stdatomic.h>
#define HAS_ATOMIC 1
#else
#define HAS_ATOMIC 0
#endif
/* Macros and Definitions -------------------------------------*/
#if HAS_ATOMIC
struct eos_sem_t
{
    atomic_uint count;
    unsigned int max;
};
#else
struct eos_sem_t
{
    uint32_t count;
    uint32_t max;
};
#endif
/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/
EOS_WEAK eos_sem_t *eos_sem_create(uint32_t initial_count, uint32_t max_count)
{
#if HAS_ATOMIC
    eos_sem_t *sem = eos_malloc(sizeof(eos_sem_t));
    if (!sem)
        return NULL;
    atomic_init(&sem->count, initial_count);
    sem->max = max_count;
    return sem;
#else
    eos_sem_t *sem = eos_malloc(sizeof(eos_sem_t));
    if (!sem)
        return NULL;
    sem->count = initial_count;
    sem->max = max_count;
    return sem;
#endif
}

EOS_WEAK void eos_sem_destroy(eos_sem_t *sem)
{
    if (sem)
        eos_free(sem);
}

EOS_WEAK bool eos_sem_take(eos_sem_t *sem, uint32_t timeout_ms)
{
    (void)timeout_ms;
#if HAS_ATOMIC
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

EOS_WEAK void eos_sem_give(eos_sem_t *sem)
{
#if HAS_ATOMIC
    unsigned int c = atomic_load_explicit(&sem->count, memory_order_relaxed);
    if (c < sem->max)
    {
        atomic_fetch_add_explicit(&sem->count, 1, memory_order_release);
    }
#else
    if (sem->count < sem->max)
        sem->count++;
#endif
}

EOS_WEAK void eos_delay(uint32_t ms)
{
    LV_UNUSED(ms);
    return;
}

EOS_WEAK void eos_cpu_reset(void)
{
    return;
}

EOS_WEAK void eos_bluetooth_enable(void)
{
    return;
}

EOS_WEAK void eos_bluetooth_disable(void)
{
    return;
}

EOS_WEAK eos_datetime_t eos_time_get_core(void)
{
    /* EXAMPLE */
    eos_datetime_t dt = {0};
    dt.year = 2025;
    dt.month = 9;
    dt.day = 7;
    dt.hour = 18;
    dt.min = 26;
    dt.sec = 13;
    dt.day_of_week = 7;
    return dt;
}

EOS_WEAK void eos_display_set_brightness(uint8_t brightness)
{
    LV_UNUSED(brightness);
    return;
}

EOS_WEAK void eos_locate_phone(void)
{
    return;
}

EOS_WEAK void eos_speaker_set_volume(uint8_t volume)
{
    LV_UNUSED(volume);
    return;
}

EOS_WEAK bool eos_speaker_detect(void)
{
    return false;
}

EOS_WEAK bool eos_microphone_detect(void)
{
    return false;
}

EOS_WEAK eos_audio_state_t eos_audio_get_state(void)
{
    return EOS_AUDIO_STATE_UNAVAILABLE;
}

EOS_WEAK int eos_audio_play_file(const char *file_path)
{
    LV_UNUSED(file_path);
    return -1;
}

EOS_WEAK int eos_audio_stop(void)
{
    return -1;
}

EOS_WEAK void eos_sys_sleep(void)
{
    eos_display_set_brightness(0);
    return;
}

EOS_WEAK void eos_sys_aod(void)
{
    eos_display_set_brightness(20);
    return;
}

EOS_WEAK void eos_sys_wake(void)
{
    eos_display_set_brightness(100);
    return;
}
