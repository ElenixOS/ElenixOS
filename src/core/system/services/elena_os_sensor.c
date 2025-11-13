/**
 * @file elena_os_sensor.c
 * @brief 传感器测试应用
 * @author Sab1e
 * @date 2025-09-14
 */

#include "elena_os_sensor.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lvgl.h"
#include "elena_os_log.h"
#include "elena_os_port.h"
#include "elena_os_nav.h"
#include "elena_os_basic_widgets.h"
#include "elena_os_event.h"
#include "elena_os_config.h"
#include "elena_os_async.h"

/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/
static eos_sensor_t *_sensor_list[EOS_SENSOR_NUMBER][EOS_SENSOR_CFG_INST_MAX] = {0};
static eos_sensor_id_t _sensor_count[EOS_SENSOR_NUMBER] = {0};
/* Function Implementations -----------------------------------*/

eos_event_code_t eos_sensor_get_event_code(eos_sensor_type_t type)
{
    if (type >= _EOS_SENSOR_MAX || type <= EOS_SENSOR_TYPE_UNKNOWN)
        return EOS_EVENT_UNKNOWN;
    return EOS_EVENT_SENSOR_REPORT_START + type;
}

eos_sensor_type_t eos_sensor_get_type_by_event_code(eos_event_code_t code)
{
    if (code >= EOS_EVENT_SENSOR_REPORT_END || code <= EOS_EVENT_SENSOR_REPORT_START)
        return EOS_SENSOR_TYPE_UNKNOWN;
    return code - EOS_EVENT_SENSOR_REPORT_START;
}

static void _async_report_sensor_data(void *user_data)
{
    eos_sensor_t *sensor = (eos_sensor_t *)user_data;
    EOS_CHECK_PTR_RETURN(sensor);
    eos_event_broadcast(eos_sensor_get_event_code(sensor->type), (void *)sensor);
}

void eos_sensor_report(eos_sensor_t *sensor)
{
    eos_async_call(_async_report_sensor_data, sensor);
}

eos_sensor_t *eos_sensor_get(eos_sensor_type_t type)
{
    if (type <= EOS_SENSOR_TYPE_UNKNOWN || type >= _EOS_SENSOR_MAX)
        return NULL;

    if (_sensor_count[type] == 0)
        return NULL;

    return _sensor_list[type][0];
}

eos_sensor_t *eos_sensor_find(eos_sensor_type_t type, uint8_t index)
{
    if (type <= EOS_SENSOR_TYPE_UNKNOWN || type >= _EOS_SENSOR_MAX)
        return NULL;

    if (index >= _sensor_count[type])
        return NULL;

    return _sensor_list[type][index];
}

eos_sensor_t *eos_sensor_register(eos_sensor_type_t type)
{
    if (type <= EOS_SENSOR_TYPE_UNKNOWN || type >= _EOS_SENSOR_MAX)
        return NULL;

    if (_sensor_count[type] >= EOS_SENSOR_CFG_INST_MAX)
        return NULL;

    eos_sensor_t *s = eos_malloc(sizeof(eos_sensor_t));
    EOS_CHECK_PTR_RETURN_VAL(s, NULL);
    memset(s, 0, sizeof(eos_sensor_t));

    s->type = type;
    s->id = _sensor_count[type];

    _sensor_list[type][_sensor_count[type]] = s;
    _sensor_count[type]++;

    return s;
}

uint8_t eos_sensor_get_count(eos_sensor_type_t type)
{
    if (type <= EOS_SENSOR_TYPE_UNKNOWN || type >= _EOS_SENSOR_MAX)
        return 0;

    return _sensor_count[type];
}
