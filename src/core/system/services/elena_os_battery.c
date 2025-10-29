/**
 * @file elena_os_battery.c
 * @brief 电池服务
 * @author Sab1e
 * @date 2025-10-07
 */

// Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "elena_os_core.h"
#include "elena_os_sensor.h"
#include "elena_os_config.h"
#include "elena_os_event.h"
#include "elena_os_port.h"
#include "elena_os_log.h"

// Macros and Definitions

// Variables
static eos_sensor_t battery_sensor = {0};
// Function Implementations

static void _battery_measurement_cb(lv_event_t *e)
{
    eos_sensor_t *sensor = lv_event_get_param(e);
    EOS_CHECK_PTR_RETURN(sensor);
    memcpy(&battery_sensor, sensor, sizeof(eos_sensor_t));
}

static void _battery_measurement_timer_cb(lv_timer_t *timer)
{
    eos_sensor_read(EOS_SENSOR_BAT);
}

uint8_t eos_battery_service_get_level(void)
{
    return battery_sensor.data.bat.level;
}

eos_result_t eos_battery_service_start(void)
{
    lv_timer_t *timer = lv_timer_create(
        _battery_measurement_timer_cb,
        EOS_BATTERY_DISPLAY_UPDATE_MS,
        NULL);
    EOS_CHECK_PTR_RETURN_VAL(timer, EOS_FAILED);
    eos_event_add_cb(
        lv_screen_active(),
        _battery_measurement_cb,
        EOS_EVENT_SENSOR_REPORT_BAT,
        NULL);
    // 先读取一次电量
    eos_sensor_read(EOS_SENSOR_BAT);
}
