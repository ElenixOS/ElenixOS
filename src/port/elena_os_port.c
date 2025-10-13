/**
 * @file elena_os_port.c
 * @brief ElenaOS 移植
 * @author Sab1e
 * @date 2025-08-21
 */

#include "elena_os_port.h"

// Includes
#include <stdio.h>
#include <stdlib.h>

// Macros and Definitions

// Variables

// Function Implementations

EOS_WEAK void *eos_malloc_large(size_t size)
{
    return malloc(size);
}

EOS_WEAK void eos_free_large(void *ptr)
{
    free(ptr);
}

EOS_WEAK void eos_delay(uint32_t ms)
{
    EOS_UNUSED(ms);
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

EOS_WEAK eos_datetime_t eos_time_get(void)
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
    EOS_UNUSED(brightness);
    return;
}

EOS_WEAK void eos_sensor_read(eos_sensor_type_t type)
{
    switch (type)
    {
    case EOS_SENSOR_HR:
        // 触发心率测量函数
        break;

    default:
        break;
    }
}

EOS_WEAK void eos_locate_phone(void)
{
    return;
}

EOS_WEAK void eos_speaker_set_volume(uint8_t volume)
{
    EOS_UNUSED(volume);
    return;
}
