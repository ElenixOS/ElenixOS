/**
 * @file elena_os_services.c
 * @brief 服务
 * @author Sab1e
 * @date 2025-11-06
 */

#include "elena_os_services.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "elena_os_core.h"
#include "elena_os_sensor.h"
#include "elena_os_config.h"
#include "elena_os_event.h"
#include "elena_os_port.h"
#include "elena_os_log.h"

/* Macros and Definitions -------------------------------------*/
#define DEFINE_SENSOR_SERVICE(sensor_name, SENSOR_TYPE, EVENT_TYPE, update_ms)       \
    static eos_sensor_t sensor_name##_data = {.type = SENSOR_TYPE};                  \
    static void _##sensor_name##_cb(lv_event_t *e)                                   \
    {                                                                                \
        eos_sensor_t *sensor = lv_event_get_param(e);                                \
        if (!sensor || sensor->type != SENSOR_TYPE)                                  \
            return;                                                                  \
        memcpy(&sensor_name##_data, sensor, sizeof(eos_sensor_t));                   \
    }                                                                                \
    static void _##sensor_name##_timer_cb(lv_timer_t *t)                             \
    {                                                                                \
        eos_sensor_read_async(SENSOR_TYPE);                                                \
    }                                                                                \
    void eos_##sensor_name##_service_start(void)                                     \
    {                                                                                \
        lv_timer_t *t = lv_timer_create(_##sensor_name##_timer_cb, update_ms, NULL); \
        eos_event_add_cb(lv_screen_active(), _##sensor_name##_cb, EVENT_TYPE, NULL); \
        eos_sensor_read_async(SENSOR_TYPE);                                                \
    }
/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/
DEFINE_SENSOR_SERVICE(battery, EOS_SENSOR_TYPE_BAT, EOS_EVENT_SENSOR_REPORT_BAT, 5000)

uint8_t eos_battery_service_get_level(void)
{
    return battery_data.data.bat.level;
}

bool eos_battery_service_get_charging(void)
{
    return battery_data.data.bat.charging;
}
