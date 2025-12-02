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
#define EOS_LOG_TAG "Services"
#include "elena_os_log.h"
#include "elena_os_basic_widgets.h"
#include "elena_os_screen_mgr.h"

/* Macros and Definitions -------------------------------------*/
#define _DEFINE_SENSOR_SERVICE(sensor_name, SENSOR_TYPE, update_ms)                                               \
    static eos_sensor_t sensor_name##_data = {.type = SENSOR_TYPE};                                               \
    static void _##sensor_name##_cb(lv_event_t *e)                                                                \
    {                                                                                                             \
        eos_sensor_t *sensor = lv_event_get_param(e);                                                             \
        if (!sensor || sensor->type != SENSOR_TYPE)                                                               \
            return;                                                                                               \
        memcpy(&sensor_name##_data, sensor, sizeof(eos_sensor_t));                                                \
    }                                                                                                             \
    static void _##sensor_name##_timer_cb(lv_timer_t *t)                                                          \
    {                                                                                                             \
        eos_sensor_read_async(SENSOR_TYPE);                                                                       \
    }                                                                                                             \
    void eos_##sensor_name##_service_start(void)                                                                  \
    {                                                                                                             \
        lv_timer_t *t = lv_timer_create(_##sensor_name##_timer_cb, update_ms, NULL);                              \
        eos_event_add_cb(eos_screen_active(), _##sensor_name##_cb, eos_sensor_get_event_code(SENSOR_TYPE), NULL); \
        eos_sensor_read_async(SENSOR_TYPE);                                                                       \
    }

#define _START_SERVICE(sensor_name, SENSOR_TYPE)        \
    if (eos_sensor_get(SENSOR_TYPE))                    \
    {                                                   \
        eos_##sensor_name##_service_start();            \
    }                                                   \
    else                                                \
    {                                                   \
        EOS_LOG_W("Sensor not found: %d", SENSOR_TYPE); \
    }

/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/
_DEFINE_SENSOR_SERVICE(battery, EOS_SENSOR_TYPE_BAT, 5000)
_DEFINE_SENSOR_SERVICE(acce, EOS_SENSOR_TYPE_ACCE, 5000)

uint8_t eos_battery_service_get_level(void)
{
    return battery_data.data.bat.level;
}

bool eos_battery_service_get_charging(void)
{
    return battery_data.data.bat.charging;
}

void eos_services_start(void)
{
    _START_SERVICE(battery, EOS_SENSOR_TYPE_BAT);
    _START_SERVICE(acce, EOS_SENSOR_TYPE_ACCE);
}
