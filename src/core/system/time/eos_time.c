/**
 * @file eos_time.c
 * @brief System time base
 */

#include "eos_time.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "eos_port.h"
/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

eos_datetime_t eos_time_get(void)
{
    static eos_datetime_t last_sec_time;
    static uint32_t sec_base_tick = 0;
    static uint8_t initialized = 0;

    eos_datetime_t now = eos_time_get_core();
    uint32_t tick = lv_tick_get();

    if (!initialized ||
        now.sec  != last_sec_time.sec ||
        now.min  != last_sec_time.min ||
        now.hour != last_sec_time.hour ||
        now.day  != last_sec_time.day ||
        now.month!= last_sec_time.month ||
        now.year != last_sec_time.year)
    {
        sec_base_tick = tick;
        last_sec_time = now;
        initialized = 1;
    }

    uint32_t ms = tick - sec_base_tick;
    if (ms >= 1000)
    {
        ms = 999;
    }

    now.ms = (uint16_t)ms;
    return now;
}