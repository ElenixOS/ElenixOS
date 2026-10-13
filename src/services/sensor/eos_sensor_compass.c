/**
 * @file eos_sensor_compass.c
 * @brief Compass capability of the Sensor API
 */

#include "eos_sensor_compass.h"

/* Includes ---------------------------------------------------*/
#include <math.h>
#include <string.h>
#include "lvgl.h"
#include "eos_mem.h"
#include "eos_service_sensor.h"
#include "eos_core.h"

/* Macros and Definitions -------------------------------------*/
#define _COMPASS_TIMER_PERIOD_MS 20U
#define _COMPASS_DEFAULT_TIMEOUT_MS 5000U
#define _COMPASS_ACCE_PERIOD_MS 20U
#define _COMPASS_MAG_PERIOD_MS 50U
#define _COMPASS_NO_DATA_AFTER_MS 500U
#define _COMPASS_MAX_SENSOR_AGE_MS 250U
#define _COMPASS_CALIBRATION_SAMPLES 24U
#define _COMPASS_MIN_GOOD_SAMPLES 3U
#define _COMPASS_MIN_GRAVITY 7.0f
#define _COMPASS_MAX_GRAVITY 12.0f
#define _COMPASS_MIN_FIELD_NT 10000.0f
#define _COMPASS_MAX_FIELD_NT 100000.0f
#define _COMPASS_PI 3.14159265358979323846f

/* Variables --------------------------------------------------*/
typedef struct
{
    bool valid;
    float hard_iron[3];
    float scale[3];
} _compass_calibration_t;

typedef struct _compass_client_t
{
    eos_sensor_request_id_t request_id;
    eos_sensor_compass_cb_t callback;
    void *user_data;
    uint32_t deadline;
    bool active;
    bool marked_for_delete;
    bool state_reported;
    eos_sensor_compass_state_t last_state;
    struct _compass_client_t *next;
} _compass_client_t;

typedef struct
{
    lv_timer_t *timer;
    uint32_t started_at;
    eos_sensor_demand_id_t acce_demand_id;
    eos_sensor_demand_id_t mag_demand_id;
    bool has_acce_source;
    bool has_mag_source;
    bool pending_failure;
    eos_sensor_compass_state_t failure_state;
    uint32_t dispatch_depth;
    bool finishing;
    _compass_client_t *clients;
    bool acce_timestamp_seen;
    bool mag_timestamp_seen;
    uint32_t last_acce_timestamp;
    uint32_t last_mag_timestamp;
    bool acce_seen;
    bool mag_seen;
    eos_sensor_raw_data_t acce;
    eos_sensor_raw_data_t mag;
    float mag_min[3];
    float mag_max[3];
    uint32_t calibration_sample_count;
    uint32_t good_sample_count;
} _compass_session_t;

static _compass_session_t *_compass_session = NULL;
static eos_sensor_request_id_t _compass_next_request_id = 0x40000001U;
static _compass_calibration_t _compass_calibration = {0};
static eos_sensor_compass_result_t _compass_latest;
static bool _compass_has_latest;

/* Function Implementations -----------------------------------*/
static bool _compass_is_terminal(eos_sensor_compass_state_t state)
{
    return state == EOS_SENSOR_COMPASS_STATE_SUCCESS || state == EOS_SENSOR_COMPASS_STATE_TIMEOUT
           || state == EOS_SENSOR_COMPASS_STATE_UNAVAILABLE || state == EOS_SENSOR_COMPASS_STATE_ERROR
           || state == EOS_SENSOR_COMPASS_STATE_CANCELLED;
}

static eos_sensor_request_id_t _compass_allocate_request_id(void)
{
    eos_sensor_request_id_t id = _compass_next_request_id++;
    if (id == EOS_SENSOR_REQUEST_INVALID)
        id = _compass_next_request_id++;
    return id;
}

static void _compass_release_demands(_compass_session_t *session)
{
    if (!session)
        return;
    if (session->acce_demand_id != EOS_SENSOR_DEMAND_INVALID)
    {
        (void)eos_sensor_demand_release(session->acce_demand_id);
        session->acce_demand_id = EOS_SENSOR_DEMAND_INVALID;
    }
    if (session->mag_demand_id != EOS_SENSOR_DEMAND_INVALID)
    {
        (void)eos_sensor_demand_release(session->mag_demand_id);
        session->mag_demand_id = EOS_SENSOR_DEMAND_INVALID;
    }
}

static void _compass_cleanup_clients(_compass_session_t *session)
{
    _compass_client_t **link;

    if (!session || session->dispatch_depth > 0U)
        return;

    link = &session->clients;
    while (*link)
    {
        if ((*link)->marked_for_delete)
        {
            _compass_client_t *client = *link;
            *link = client->next;
            eos_free(client);
        }
        else
        {
            link = &(*link)->next;
        }
    }
}

static void _compass_release_session(_compass_session_t *session)
{
    if (!session)
        return;
    if (session->timer)
    {
        lv_timer_delete(session->timer);
        session->timer = NULL;
    }
    _compass_cleanup_clients(session);
    eos_free(session);
}

static void _compass_emit_client(_compass_client_t *client,
                                 eos_sensor_compass_state_t state,
                                 const eos_sensor_compass_result_t *result)
{
    eos_sensor_compass_result_t copied = {0};

    if (!client || !client->active || client->marked_for_delete || !client->callback)
        return;
    if (client->state_reported && client->last_state == state && !_compass_is_terminal(state))
        return;

    client->state_reported = true;
    client->last_state = state;
    if (result)
        memcpy(&copied, result, sizeof(copied));
    client->callback(client->request_id, state, result ? &copied : NULL, client->user_data);
}

static void _compass_emit_all(_compass_session_t *session,
                              eos_sensor_compass_state_t state,
                              const eos_sensor_compass_result_t *result)
{
    _compass_client_t *client;

    if (!session)
        return;

    session->dispatch_depth++;
    client = session->clients;
    while (client)
    {
        _compass_client_t *next = client->next;
        _compass_emit_client(client, state, result);
        if (_compass_is_terminal(state))
        {
            client->active = false;
            client->marked_for_delete = true;
        }
        client = next;
    }
    session->dispatch_depth--;
    _compass_cleanup_clients(session);
}

static void _compass_finish(_compass_session_t *session,
                            eos_sensor_compass_state_t state,
                            const eos_sensor_compass_result_t *result)
{
    if (!session || _compass_session != session)
        return;

    session->finishing = true;
    _compass_session = NULL;
    _compass_release_demands(session);
    _compass_emit_all(session, state, result);
    _compass_release_session(session);
}

static bool _compass_read_new(eos_sensor_type_t type,
                              eos_sensor_raw_data_t *data,
                              bool *seen,
                              uint32_t *last_timestamp,
                              uint32_t started_at)
{
    if (eos_sensor_read_latest(type, data) != EOS_OK)
        return false;
    if (!*seen && (int32_t)(data->timestamp - started_at) < 0)
        return false;
    if (*seen && data->timestamp == *last_timestamp)
        return false;
    *seen = true;
    *last_timestamp = data->timestamp;
    return true;
}

static void _compass_update_calibration(_compass_session_t *request, float x, float y, float z)
{
    float values[3] = {x, y, z};

    if (_compass_calibration.valid)
        return;
    if (request->calibration_sample_count == 0U)
    {
        for (uint32_t i = 0U; i < 3U; i++)
        {
            request->mag_min[i] = values[i];
            request->mag_max[i] = values[i];
        }
    }
    else
    {
        for (uint32_t i = 0U; i < 3U; i++)
        {
            if (values[i] < request->mag_min[i])
                request->mag_min[i] = values[i];
            if (values[i] > request->mag_max[i])
                request->mag_max[i] = values[i];
        }
    }
    request->calibration_sample_count++;

    if (request->calibration_sample_count >= _COMPASS_CALIBRATION_SAMPLES)
    {
        float average_span = 0.0f;
        bool enough_motion = true;
        for (uint32_t i = 0U; i < 3U; i++)
        {
            float span = request->mag_max[i] - request->mag_min[i];
            if (span < 1.0f)
                enough_motion = false;
            average_span += span;
        }
        if (enough_motion)
        {
            average_span /= 3.0f;
            for (uint32_t i = 0U; i < 3U; i++)
            {
                float span = request->mag_max[i] - request->mag_min[i];
                _compass_calibration.hard_iron[i] = (request->mag_max[i] + request->mag_min[i]) / 2.0f;
                _compass_calibration.scale[i] = average_span / span;
            }
            _compass_calibration.valid = true;
        }
    }
}

static bool _compass_expire_clients(_compass_session_t *session, uint32_t now)
{
    _compass_client_t *client;

    session->dispatch_depth++;
    client = session->clients;
    while (client)
    {
        _compass_client_t *next = client->next;
        if (client->active && (int32_t)(now - client->deadline) >= 0)
        {
            _compass_emit_client(client, EOS_SENSOR_COMPASS_STATE_TIMEOUT, NULL);
            client->active = false;
            client->marked_for_delete = true;
        }
        client = next;
    }
    session->dispatch_depth--;
    _compass_cleanup_clients(session);
    return session->clients == NULL;
}

static void _compass_process(_compass_session_t *request)
{
    uint32_t now = eos_tick_get();
    bool acce_new;
    bool mag_new;

    if (request->pending_failure)
    {
        _compass_emit_all(request, EOS_SENSOR_COMPASS_STATE_WAITING, NULL);
        if (_compass_session == request)
            _compass_finish(request, request->failure_state, NULL);
        return;
    }

    _compass_emit_all(request, EOS_SENSOR_COMPASS_STATE_WAITING, NULL);
    if (_compass_session != request)
        return;

    acce_new = _compass_read_new(EOS_SENSOR_TYPE_ACCE,
                                 &request->acce,
                                 &request->acce_timestamp_seen,
                                 &request->last_acce_timestamp,
                                 request->started_at);
    mag_new = _compass_read_new(EOS_SENSOR_TYPE_MAG,
                                &request->mag,
                                &request->mag_timestamp_seen,
                                &request->last_mag_timestamp,
                                request->started_at);
    request->acce_seen = request->acce_seen || acce_new;
    request->mag_seen = request->mag_seen || mag_new;

    if (!request->acce_seen || !request->mag_seen)
    {
        if ((uint32_t)(now - request->started_at) >= _COMPASS_NO_DATA_AFTER_MS)
            _compass_emit_all(request, EOS_SENSOR_COMPASS_STATE_NO_DATA, NULL);
    }
    else if ((uint32_t)(now - request->acce.timestamp) > _COMPASS_MAX_SENSOR_AGE_MS
             || (uint32_t)(now - request->mag.timestamp) > _COMPASS_MAX_SENSOR_AGE_MS
             || (request->acce.timestamp > request->mag.timestamp ? request->acce.timestamp - request->mag.timestamp
                                                                  : request->mag.timestamp - request->acce.timestamp)
                    > _COMPASS_MAX_SENSOR_AGE_MS)
    {
        _compass_emit_all(request, EOS_SENSOR_COMPASS_STATE_UNSTABLE, NULL);
    }
    else if (acce_new || mag_new)
    {
        /* Convert only the canonical Sensor Contract scales for the internal
         * math: acceleration milli-m/s^2 -> m/s^2, while magnetic values
         * remain in nT throughout calibration and heading calculation. */
        float ax = (float)request->acce.data.acce.x / 1000.0f;
        float ay = (float)request->acce.data.acce.y / 1000.0f;
        float az = (float)request->acce.data.acce.z / 1000.0f;
        float mx = (float)request->mag.data.mag.x;
        float my = (float)request->mag.data.mag.y;
        float mz = (float)request->mag.data.mag.z;
        float gravity = sqrtf(ax * ax + ay * ay + az * az);
        float field;

        _compass_update_calibration(request, mx, my, mz);
        if (_compass_calibration.valid)
        {
            mx = (mx - _compass_calibration.hard_iron[0]) * _compass_calibration.scale[0];
            my = (my - _compass_calibration.hard_iron[1]) * _compass_calibration.scale[1];
            mz = (mz - _compass_calibration.hard_iron[2]) * _compass_calibration.scale[2];
        }
        field = sqrtf(mx * mx + my * my + mz * mz);

        if (!_compass_calibration.valid || gravity < _COMPASS_MIN_GRAVITY || gravity > _COMPASS_MAX_GRAVITY
            || field < _COMPASS_MIN_FIELD_NT || field > _COMPASS_MAX_FIELD_NT)
        {
            _compass_emit_all(request, EOS_SENSOR_COMPASS_STATE_UNSTABLE, NULL);
        }
        else
        {
            float roll = atan2f(ay, az);
            float pitch = atan2f(-ax, sqrtf(ay * ay + az * az));
            float mx_level = mx * cosf(pitch) + mz * sinf(pitch);
            float my_level = mx * sinf(roll) * sinf(pitch) + my * cosf(roll) - mz * sinf(roll) * cosf(pitch);
            float heading = atan2f(-my_level, mx_level) * 180.0f / _COMPASS_PI;
            if (heading < 0.0f)
                heading += 360.0f;

            request->good_sample_count++;
            if (request->good_sample_count >= _COMPASS_MIN_GOOD_SAMPLES)
            {
                eos_sensor_compass_result_t result = {
                    .heading_mdeg = (int32_t)(heading * 1000.0f),
                    .field_strength_nt = (int32_t)field,
                    .timestamp = request->mag.timestamp,
                };
                _compass_latest = result;
                _compass_has_latest = true;
                _compass_finish(request, EOS_SENSOR_COMPASS_STATE_SUCCESS, &result);
                return;
            }
            _compass_emit_all(request, EOS_SENSOR_COMPASS_STATE_UNSTABLE, NULL);
        }
    }

    if (_compass_session != request)
        return;
    if (_compass_expire_clients(request, now))
        _compass_finish(request, EOS_SENSOR_COMPASS_STATE_TIMEOUT, NULL);
}

static void _compass_timer_cb(lv_timer_t *timer)
{
    _compass_session_t *request = lv_timer_get_user_data(timer);
    if (request && _compass_session == request)
        _compass_process(request);
}

static eos_sensor_compass_state_t _compass_failure_state(eos_result_t result)
{
    return result == EOS_ERR_DEV_NOT_FOUND ? EOS_SENSOR_COMPASS_STATE_UNAVAILABLE : EOS_SENSOR_COMPASS_STATE_ERROR;
}

eos_result_t eos_sensor_compass_start(eos_sensor_compass_cb_t callback,
                                      void *user_data,
                                      uint32_t timeout_ms,
                                      eos_sensor_request_id_t *request_id)
{
    _compass_session_t *request;
    _compass_client_t *client;
    eos_result_t result = EOS_OK;
    uint32_t now;

    if (!callback || !request_id)
        return EOS_ERR_INVALID_ARG;

    now = eos_tick_get();
    if (_compass_session)
    {
        client = eos_malloc_zeroed(sizeof(*client));
        if (!client)
            return EOS_ERR_MEM;
        client->request_id = _compass_allocate_request_id();
        client->callback = callback;
        client->user_data = user_data;
        client->deadline = now + (timeout_ms == 0U ? _COMPASS_DEFAULT_TIMEOUT_MS : timeout_ms);
        client->active = true;
        client->next = _compass_session->clients;
        _compass_session->clients = client;
        *request_id = client->request_id;
        return EOS_OK;
    }

    request = eos_malloc_zeroed(sizeof(*request));
    if (!request)
        return EOS_ERR_MEM;

    client = eos_malloc_zeroed(sizeof(*client));
    if (!client)
    {
        eos_free(request);
        return EOS_ERR_MEM;
    }
    client->request_id = _compass_allocate_request_id();
    client->callback = callback;
    client->user_data = user_data;
    client->deadline = now + (timeout_ms == 0U ? _COMPASS_DEFAULT_TIMEOUT_MS : timeout_ms);
    client->active = true;
    request->clients = client;
    request->started_at = now;
    request->has_acce_source = eos_dev_sensor_get_default(EOS_SENSOR_TYPE_ACCE) != NULL;
    request->has_mag_source = eos_dev_sensor_get_default(EOS_SENSOR_TYPE_MAG) != NULL;

    if (!request->has_acce_source || !request->has_mag_source)
    {
        request->pending_failure = true;
        request->failure_state = EOS_SENSOR_COMPASS_STATE_UNAVAILABLE;
    }
    else
    {
        result = eos_sensor_demand_acquire(EOS_SENSOR_TYPE_ACCE, _COMPASS_ACCE_PERIOD_MS, &request->acce_demand_id);
        if (result == EOS_OK)
            result = eos_sensor_demand_acquire(EOS_SENSOR_TYPE_MAG, _COMPASS_MAG_PERIOD_MS, &request->mag_demand_id);
        if (result != EOS_OK)
        {
            _compass_release_demands(request);
            if (result == EOS_ERR_DEV_NOT_FOUND || result == EOS_ERR_DEV_ERROR)
            {
                request->pending_failure = true;
                request->failure_state = _compass_failure_state(result);
            }
            else
            {
                eos_free(client);
                eos_free(request);
                return result;
            }
        }
    }

    request->timer = lv_timer_create(_compass_timer_cb, _COMPASS_TIMER_PERIOD_MS, request);
    if (!request->timer)
    {
        _compass_release_demands(request);
        eos_free(request);
        return EOS_ERR_MEM;
    }
    lv_timer_set_repeat_count(request->timer, -1);
    _compass_session = request;
    *request_id = client->request_id;
    return EOS_OK;
}

eos_result_t eos_sensor_compass_cancel(eos_sensor_request_id_t request_id)
{
    _compass_client_t *client;

    if (!_compass_session)
        return EOS_ERR_NOT_FOUND;

    client = _compass_session->clients;
    while (client && client->request_id != request_id)
        client = client->next;
    if (!client || !client->active || client->marked_for_delete)
        return EOS_ERR_NOT_FOUND;

    _compass_session->dispatch_depth++;
    _compass_emit_client(client, EOS_SENSOR_COMPASS_STATE_CANCELLED, NULL);
    client->active = false;
    client->marked_for_delete = true;
    _compass_session->dispatch_depth--;
    _compass_cleanup_clients(_compass_session);
    if (!_compass_session->clients)
        _compass_finish(_compass_session, EOS_SENSOR_COMPASS_STATE_CANCELLED, NULL);
    return EOS_OK;
}

eos_result_t eos_sensor_compass_read_latest(eos_sensor_compass_result_t *result)
{
    if (!result)
        return EOS_ERR_INVALID_ARG;
    if (!_compass_has_latest)
        return EOS_ERR_NOT_FOUND;
    *result = _compass_latest;
    return EOS_OK;
}
