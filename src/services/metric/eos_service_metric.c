/**
 * @file eos_service_metric.c
 * @brief Thin semantic Metric adapter for watchface and script consumers
 */

#include "eos_service_metric.h"

/* Includes ---------------------------------------------------*/
#include <math.h>
#include <string.h>
#include "eos_core.h"
#include "eos_service_battery.h"
#include "eos_service_sensor.h"
#include "eos_service_time.h"
#include "eos_sensor_compass.h"
#define EOS_LOG_TAG "MetricService"
#include "eos_log.h"

/* Macros and Definitions -------------------------------------*/
#define _MOON_SYNODIC_MONTH_DAYS 29.530588
#define _MOON_REF_NEW_MOON_JDN 2451550.5
#define _METRIC_STALE_SENSOR_MS 10000U
#define _METRIC_STALE_HEALTH_MS 30000U
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* Variables --------------------------------------------------*/
static uint32_t _heading_users;
static eos_sensor_request_id_t _heading_request_id = EOS_SENSOR_REQUEST_INVALID;
static eos_sensor_compass_result_t _heading_latest;
static bool _heading_valid;

/* Function Implementations -----------------------------------*/

static void _metric_heading_callback(eos_sensor_request_id_t request_id,
                                     eos_sensor_compass_state_t state,
                                     const eos_sensor_compass_result_t *result,
                                     void *user_data);

static void _metric_heading_start(void)
{
    eos_sensor_request_id_t request_id;
    if (_heading_users == 0U || _heading_request_id != EOS_SENSOR_REQUEST_INVALID)
        return;
    if (eos_sensor_compass_start(_metric_heading_callback, NULL, 0U, &request_id) == EOS_OK)
        _heading_request_id = request_id;
}

static long _metric_julian_day_number(int year, int month, int day)
{
    int a = (14 - month) / 12;
    int y = year + 4800 - a;
    int m = month + 12 * a - 3;
    return (long)day + (153 * m + 2) / 5 + 365L * y + y / 4 - y / 100 + y / 400 - 32045;
}

static double _metric_moon_age(const eos_datetime_t *now)
{
    long jdn = _metric_julian_day_number(now->year, now->month, now->day);
    double age = fmod((double)jdn - _MOON_REF_NEW_MOON_JDN, _MOON_SYNODIC_MONTH_DAYS);
    return age < 0.0 ? age + _MOON_SYNODIC_MONTH_DAYS : age;
}

static void _metric_sample_init(eos_metric_value_t *sample, const char *unit)
{
    memset(sample, 0, sizeof(*sample));
    sample->unit = unit;
    sample->timestamp = eos_tick_get();
}

static bool _metric_sample_is_stale(uint32_t timestamp, uint32_t max_age_ms)
{
    return timestamp == 0U || (uint32_t)(eos_tick_get() - timestamp) > max_age_ms;
}

static void _metric_heading_callback(eos_sensor_request_id_t request_id,
                                     eos_sensor_compass_state_t state,
                                     const eos_sensor_compass_result_t *result,
                                     void *user_data)
{
    (void)user_data;
    if (request_id != _heading_request_id)
        return;

    if (state == EOS_SENSOR_COMPASS_STATE_SUCCESS && result)
    {
        _heading_latest = *result;
        _heading_valid = true;
    }

    if (state == EOS_SENSOR_COMPASS_STATE_SUCCESS || state == EOS_SENSOR_COMPASS_STATE_TIMEOUT
        || state == EOS_SENSOR_COMPASS_STATE_UNAVAILABLE || state == EOS_SENSOR_COMPASS_STATE_ERROR
        || state == EOS_SENSOR_COMPASS_STATE_CANCELLED)
    {
        _heading_request_id = EOS_SENSOR_REQUEST_INVALID;
        _metric_heading_start();
    }
}

bool eos_metric_parse(const char *name, eos_metric_id_t *id)
{
    static const char *const names[EOS_METRIC_COUNT] = {
        [EOS_METRIC_TIME_NOW] = "time.now",
        [EOS_METRIC_BATTERY_PERCENT] = "battery.percent",
        [EOS_METRIC_BATTERY_CHARGING] = "battery.charging",
        [EOS_METRIC_HEART_RATE] = "health.heart_rate",
        [EOS_METRIC_SPO2] = "health.spo2",
        [EOS_METRIC_STEPS] = "activity.steps",
        [EOS_METRIC_TEMPERATURE] = "environment.temperature",
        [EOS_METRIC_PRESSURE] = "environment.pressure",
        [EOS_METRIC_HEADING] = "orientation.heading",
        [EOS_METRIC_MOON_PHASE] = "moon.phase",
        [EOS_METRIC_MOON_ILLUMINATION] = "moon.illumination",
    };

    if (!name || !id)
        return false;
    for (uint32_t i = 0; i < EOS_METRIC_COUNT; i++)
    {
        if (strcmp(name, names[i]) == 0)
        {
            *id = (eos_metric_id_t)i;
            return true;
        }
    }
    return false;
}

const char *eos_metric_name(eos_metric_id_t id)
{
    static const char *const names[EOS_METRIC_COUNT] = {
        [EOS_METRIC_TIME_NOW] = "time.now",
        [EOS_METRIC_BATTERY_PERCENT] = "battery.percent",
        [EOS_METRIC_BATTERY_CHARGING] = "battery.charging",
        [EOS_METRIC_HEART_RATE] = "health.heart_rate",
        [EOS_METRIC_SPO2] = "health.spo2",
        [EOS_METRIC_STEPS] = "activity.steps",
        [EOS_METRIC_TEMPERATURE] = "environment.temperature",
        [EOS_METRIC_PRESSURE] = "environment.pressure",
        [EOS_METRIC_HEADING] = "orientation.heading",
        [EOS_METRIC_MOON_PHASE] = "moon.phase",
        [EOS_METRIC_MOON_ILLUMINATION] = "moon.illumination",
    };

    return id < EOS_METRIC_COUNT ? names[id] : NULL;
}

eos_result_t eos_metric_read(eos_metric_id_t id, eos_metric_value_t *sample)
{
    eos_sensor_raw_data_t raw;
    eos_datetime_t now;

    if (!sample || id >= EOS_METRIC_COUNT)
        return EOS_ERR_INVALID_ARG;

    switch (id)
    {
        case EOS_METRIC_TIME_NOW:
            _metric_sample_init(sample, "s");
            now = eos_time_get();
            sample->value = (double)now.hour * 3600.0 + (double)now.min * 60.0 + (double)now.sec;
            sample->valid = true;
            sample->stale = false;
            return EOS_OK;

        case EOS_METRIC_BATTERY_PERCENT:
            _metric_sample_init(sample, "%");
            sample->value = eos_battery_get_percent();
            sample->valid = sample->value >= 0.0;
            sample->stale = false;
            return EOS_OK;

        case EOS_METRIC_BATTERY_CHARGING:
            _metric_sample_init(sample, "bool");
            sample->value = eos_battery_is_charging() ? 1.0 : 0.0;
            sample->boolean_value = eos_battery_is_charging();
            sample->valid = true;
            sample->stale = false;
            return EOS_OK;

        case EOS_METRIC_HEART_RATE:
            _metric_sample_init(sample, "bpm");
            if (eos_sensor_read_latest(EOS_SENSOR_TYPE_HR, &raw) == EOS_OK)
            {
                sample->value = raw.data.hr.heart_rate;
                sample->timestamp = raw.timestamp;
                sample->valid = raw.timestamp > 0U && raw.data.hr.heart_rate > 0U;
                sample->stale = _metric_sample_is_stale(raw.timestamp, _METRIC_STALE_HEALTH_MS);
            }
            return EOS_OK;

        case EOS_METRIC_SPO2:
            _metric_sample_init(sample, "%");
            if (eos_sensor_read_latest(EOS_SENSOR_TYPE_SPO2, &raw) == EOS_OK)
            {
                sample->value = raw.data.spo2.spo2;
                sample->timestamp = raw.timestamp;
                sample->valid = raw.timestamp > 0U && raw.data.spo2.spo2 > 0U;
                sample->stale = _metric_sample_is_stale(raw.timestamp, _METRIC_STALE_HEALTH_MS);
            }
            return EOS_OK;

        case EOS_METRIC_STEPS:
            _metric_sample_init(sample, "steps");
            if (eos_sensor_read_latest(EOS_SENSOR_TYPE_STEP, &raw) == EOS_OK)
            {
                sample->value = raw.data.step.steps;
                sample->timestamp = raw.timestamp;
                sample->valid = raw.timestamp > 0U;
                sample->stale = _metric_sample_is_stale(raw.timestamp, _METRIC_STALE_SENSOR_MS);
            }
            return EOS_OK;

        case EOS_METRIC_TEMPERATURE:
            _metric_sample_init(sample, "degC");
            if (eos_sensor_read_latest(EOS_SENSOR_TYPE_TEMP, &raw) == EOS_OK)
            {
                sample->value = (double)raw.data.temp.temp / 1000.0;
                sample->timestamp = raw.timestamp;
                sample->valid = raw.timestamp > 0U;
                sample->stale = _metric_sample_is_stale(raw.timestamp, _METRIC_STALE_SENSOR_MS);
            }
            return EOS_OK;

        case EOS_METRIC_PRESSURE:
            _metric_sample_init(sample, "Pa");
            if (eos_sensor_read_latest(EOS_SENSOR_TYPE_BARO, &raw) == EOS_OK)
            {
                sample->value = raw.data.baro.pressure;
                sample->timestamp = raw.timestamp;
                sample->valid = raw.timestamp > 0U;
                sample->stale = _metric_sample_is_stale(raw.timestamp, _METRIC_STALE_SENSOR_MS);
            }
            return EOS_OK;

        case EOS_METRIC_HEADING:
            _metric_sample_init(sample, "deg");
            if (!_heading_valid && eos_sensor_compass_read_latest(&_heading_latest) == EOS_OK)
                _heading_valid = true;
            if (_heading_valid)
            {
                sample->value = (double)_heading_latest.heading_mdeg / 1000.0;
                sample->timestamp = _heading_latest.timestamp;
                sample->valid = true;
                sample->stale = _metric_sample_is_stale(_heading_latest.timestamp, _METRIC_STALE_SENSOR_MS);
            }
            return EOS_OK;

        case EOS_METRIC_MOON_PHASE:
            _metric_sample_init(sample, "phase");
            now = eos_time_get();
            sample->value = _metric_moon_age(&now) / _MOON_SYNODIC_MONTH_DAYS;
            sample->valid = true;
            sample->stale = false;
            return EOS_OK;

        case EOS_METRIC_MOON_ILLUMINATION:
            _metric_sample_init(sample, "%");
            now = eos_time_get();
            sample->value = (1.0 - cos(2.0 * M_PI * _metric_moon_age(&now) / _MOON_SYNODIC_MONTH_DAYS)) * 50.0;
            sample->valid = true;
            sample->stale = false;
            return EOS_OK;

        default:
            return EOS_ERR_INVALID_ARG;
    }
}

eos_result_t eos_metric_acquire(eos_metric_id_t id)
{
    if (id != EOS_METRIC_HEADING)
        return EOS_OK;

    _heading_users++;
    if (_heading_request_id == EOS_SENSOR_REQUEST_INVALID)
    {
        eos_sensor_request_id_t request_id;
        eos_result_t result = eos_sensor_compass_start(_metric_heading_callback, NULL, 0U, &request_id);
        if (result != EOS_OK)
        {
            _heading_users--;
            return result;
        }
        _heading_request_id = request_id;
    }
    return EOS_OK;
}

eos_result_t eos_metric_release(eos_metric_id_t id)
{
    if (id != EOS_METRIC_HEADING)
        return EOS_OK;
    if (_heading_users == 0U)
        return EOS_ERR_INVALID_STATE;
    _heading_users--;
    if (_heading_users == 0U && _heading_request_id != EOS_SENSOR_REQUEST_INVALID)
    {
        (void)eos_sensor_compass_cancel(_heading_request_id);
        _heading_request_id = EOS_SENSOR_REQUEST_INVALID;
    }
    return EOS_OK;
}
