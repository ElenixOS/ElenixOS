/**
 * @file eos_service_sensor.c
 * @brief Sensor service implementation
 */

/* Includes ---------------------------------------------------*/
#include <string.h>
#include "eos_service_sensor.h"
#include "eos_fifo.h"
#include "eos_port_critical.h"
#include "eos_event.h"
#include "eos_core.h"
#define EOS_LOG_TAG "SensorService"
#define EOS_LOG_DISABLE
#include "eos_log.h"

/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/
static eos_sensor_service_instance_t _instances[EOS_SENSOR_TYPE_MAX] = {0};
static bool _service_initialized = false;

/* Function Implementations -----------------------------------*/

eos_result_t eos_service_sensor_init(void)
{
    if (_service_initialized)
        return EOS_ERR_ALREADY_INITIALIZED;

    for (uint32_t i = EOS_SENSOR_TYPE_UNKNOWN + 1; i < EOS_SENSOR_TYPE_MAX; i++) {
        _instances[i].type = (eos_sensor_type_t)i;
        _instances[i].device = eos_dev_sensor_get_default((eos_sensor_type_t)i);
        _instances[i].fifo = eos_fifo_create(EOS_SENSOR_FIFO_CAPACITY * sizeof(eos_sensor_raw_data_t));
        _instances[i].sample_period_ms = 0;
        _instances[i].last_sample_time = 0;
        _instances[i].is_active = false;
        _instances[i].subscriber_count = 0;
        memset(&_instances[i].latest_data, 0, sizeof(eos_sensor_raw_data_t));
    }

    _service_initialized = true;
    return EOS_OK;
}

eos_result_t eos_sensor_read(eos_sensor_type_t type, eos_sensor_raw_data_t *data)
{
    if (!_service_initialized || !data)
        return EOS_ERR_INVALID_ARG;

    if (type <= EOS_SENSOR_TYPE_UNKNOWN || type >= EOS_SENSOR_TYPE_MAX)
        return EOS_ERR_INVALID_ARG;

    eos_sensor_service_instance_t *inst = &_instances[type];
    if (!inst->fifo || eos_fifo_is_empty(inst->fifo))
        return EOS_ERR_NOT_FOUND;

    uint16_t read = eos_fifo_read(inst->fifo, data, sizeof(eos_sensor_raw_data_t));
    return (read == sizeof(eos_sensor_raw_data_t)) ? EOS_OK : EOS_ERR_NOT_FOUND;
}

eos_result_t eos_sensor_read_latest(eos_sensor_type_t type, eos_sensor_raw_data_t *data)
{
    if (!_service_initialized || !data)
        return EOS_ERR_INVALID_ARG;

    if (type <= EOS_SENSOR_TYPE_UNKNOWN || type >= EOS_SENSOR_TYPE_MAX)
        return EOS_ERR_INVALID_ARG;

    eos_sensor_service_instance_t *inst = &_instances[type];
    memcpy(data, &inst->latest_data, sizeof(eos_sensor_raw_data_t));
    return EOS_OK;
}

eos_result_t eos_sensor_subscribe(eos_sensor_type_t type, eos_sensor_data_cb_t cb, void *user_data, uint32_t min_interval_ms)
{
    if (!_service_initialized || !cb)
        return EOS_ERR_INVALID_ARG;

    if (type <= EOS_SENSOR_TYPE_UNKNOWN || type >= EOS_SENSOR_TYPE_MAX)
        return EOS_ERR_INVALID_ARG;

    eos_sensor_service_instance_t *inst = &_instances[type];
    inst->subscriber_count++;

    if (inst->device) {
        eos_event_add_global_cb((lv_event_cb_t)cb, eos_dev_sensor_get_event_id(inst->device), user_data);
    }

    if (inst->sample_period_ms == 0 || min_interval_ms < inst->sample_period_ms) {
        eos_sensor_set_sample_period(type, min_interval_ms);
    }

    return EOS_OK;
}

eos_result_t eos_sensor_unsubscribe(eos_sensor_type_t type, eos_sensor_data_cb_t cb, void *user_data)
{
    if (!_service_initialized)
        return EOS_ERR_INVALID_ARG;

    if (type <= EOS_SENSOR_TYPE_UNKNOWN || type >= EOS_SENSOR_TYPE_MAX)
        return EOS_ERR_INVALID_ARG;

    eos_sensor_service_instance_t *inst = &_instances[type];

    if (inst->device) {
        eos_event_remove_global_cb_with_user_data(eos_dev_sensor_get_event_id(inst->device), (lv_event_cb_t)cb, user_data);
    }

    if (inst->subscriber_count > 0) {
        inst->subscriber_count--;
    }

    if (inst->subscriber_count == 0) {
        eos_sensor_set_sample_period(type, 0);
    }

    return EOS_OK;
}

eos_result_t eos_sensor_set_sample_period(eos_sensor_type_t type, uint32_t period_ms)
{
    if (!_service_initialized)
        return EOS_ERR_NOT_INITIALIZED;

    if (type <= EOS_SENSOR_TYPE_UNKNOWN || type >= EOS_SENSOR_TYPE_MAX)
        return EOS_ERR_INVALID_ARG;

    eos_sensor_service_instance_t *inst = &_instances[type];
    inst->sample_period_ms = period_ms;
    inst->is_active = (period_ms > 0);

    if (inst->device && inst->device->ops) {
        if (period_ms > 0) {
            if (inst->device->ops->enable) {
                inst->device->ops->enable(inst->device);
            }
            if (inst->device->ops->set_sample_rate) {
                uint32_t hz = 1000 / period_ms;
                inst->device->ops->set_sample_rate(inst->device, hz);
            }
        } else {
            if (inst->device->ops->disable) {
                inst->device->ops->disable(inst->device);
            }
        }
    }

    return EOS_OK;
}

uint32_t eos_sensor_get_sample_period(eos_sensor_type_t type)
{
    if (!_service_initialized)
        return 0;

    if (type <= EOS_SENSOR_TYPE_UNKNOWN || type >= EOS_SENSOR_TYPE_MAX)
        return 0;

    return _instances[type].sample_period_ms;
}

void eos_sensor_notify(eos_sensor_type_t type, const eos_sensor_data_t *data, uint32_t timestamp)
{
    if (!_service_initialized || !data)
        return;

    if (type <= EOS_SENSOR_TYPE_UNKNOWN || type >= EOS_SENSOR_TYPE_MAX)
        return;

    eos_sensor_service_instance_t *inst = &_instances[type];
    if (!inst->fifo)
        return;

    eos_sensor_raw_data_t raw_data = {
        .type = type,
        .timestamp = timestamp
    };
    memcpy(&raw_data.data, data, sizeof(eos_sensor_data_t));

    eos_critical_ctx_t ctx = eos_critical_enter();
    eos_fifo_write(inst->fifo, &raw_data, sizeof(eos_sensor_raw_data_t));
    memcpy(&inst->latest_data, &raw_data, sizeof(eos_sensor_raw_data_t));
    eos_critical_leave(ctx);

    EOS_LOG_I("eos_sensor_notify: type=%d, timestamp=%d", type, timestamp);

    eos_event_broadcast(eos_dev_sensor_get_event_id(inst->device), &inst->latest_data);
}
