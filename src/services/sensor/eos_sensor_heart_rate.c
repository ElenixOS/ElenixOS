/**
 * @file eos_sensor_heart_rate.c
 * @brief Heart-rate capability of the Sensor API
 */

#include "eos_sensor_heart_rate.h"

/* Includes ---------------------------------------------------*/
#include <string.h>
#include "lvgl.h"
#include "eos_mem.h"
#include "eos_service_sensor.h"
#include "eos_core.h"

/* Macros and Definitions -------------------------------------*/
#define _HEART_RATE_TIMER_PERIOD_MS 20U
#define _HEART_RATE_DEFAULT_TIMEOUT_MS 10000U
#define _HEART_RATE_PPG_PERIOD_MS 20U
#define _HEART_RATE_HR_PERIOD_MS 1000U
#define _HEART_RATE_NO_DATA_AFTER_MS 500U
#define _HEART_RATE_FINGER_THRESHOLD 10000U
#define _HEART_RATE_WARMUP_SAMPLES 25U
#define _HEART_RATE_MIN_AMPLITUDE 20
#define _HEART_RATE_MIN_INTERVAL_MS 270U
#define _HEART_RATE_MAX_INTERVAL_MS 1714U
#define _HEART_RATE_MIN_BPM 35U
#define _HEART_RATE_MAX_BPM 220U

/* Variables --------------------------------------------------*/
typedef struct _heart_rate_client_t
{
    eos_sensor_request_id_t request_id;
    eos_sensor_heart_rate_cb_t callback;
    void *user_data;
    uint32_t deadline;
    bool active;
    bool marked_for_delete;
    bool state_reported;
    eos_sensor_heart_rate_state_t last_state;
    struct _heart_rate_client_t *next;
} _heart_rate_client_t;

typedef struct
{
    lv_timer_t *timer;
    uint32_t started_at;
    eos_sensor_demand_id_t hr_demand_id;
    eos_sensor_demand_id_t ppg_demand_id;
    bool has_hr_source;
    bool has_ppg_source;
    bool pending_failure;
    eos_sensor_heart_rate_state_t failure_state;
    uint32_t dispatch_depth;
    bool finishing;
    _heart_rate_client_t *clients;
    bool hr_seen;
    bool ppg_seen;
    bool hr_timestamp_seen;
    bool ppg_timestamp_seen;
    uint32_t last_hr_timestamp;
    uint32_t last_ppg_timestamp;
    int32_t ir_dc;
    int32_t red_dc;
    bool dc_initialized;
    uint32_t warmup_count;
    int32_t signal_local_max;
    int32_t signal_local_min;
    int32_t recent_amplitude;
    int32_t peak_threshold;
    bool above_threshold;
    uint32_t last_peak_timestamp;
    uint16_t bpm_intervals[5];
    uint8_t bpm_interval_count;
    uint8_t bpm_interval_index;
} _heart_rate_session_t;

static _heart_rate_session_t *_heart_rate_session = NULL;
static eos_sensor_request_id_t _heart_rate_next_request_id = 1U;

/* Function Implementations -----------------------------------*/
static bool _heart_rate_is_terminal(eos_sensor_heart_rate_state_t state)
{
    return state == EOS_SENSOR_HEART_RATE_STATE_SUCCESS || state == EOS_SENSOR_HEART_RATE_STATE_TIMEOUT
           || state == EOS_SENSOR_HEART_RATE_STATE_UNAVAILABLE || state == EOS_SENSOR_HEART_RATE_STATE_ERROR
           || state == EOS_SENSOR_HEART_RATE_STATE_CANCELLED;
}

static eos_sensor_request_id_t _heart_rate_allocate_request_id(void)
{
    eos_sensor_request_id_t id = _heart_rate_next_request_id++;
    if (id == EOS_SENSOR_REQUEST_INVALID)
        id = _heart_rate_next_request_id++;
    return id;
}

static void _heart_rate_release_demands(_heart_rate_session_t *session)
{
    if (!session)
        return;

    if (session->hr_demand_id != EOS_SENSOR_DEMAND_INVALID)
    {
        (void)eos_sensor_demand_release(session->hr_demand_id);
        session->hr_demand_id = EOS_SENSOR_DEMAND_INVALID;
    }
    if (session->ppg_demand_id != EOS_SENSOR_DEMAND_INVALID)
    {
        (void)eos_sensor_demand_release(session->ppg_demand_id);
        session->ppg_demand_id = EOS_SENSOR_DEMAND_INVALID;
    }
}

static void _heart_rate_cleanup_clients(_heart_rate_session_t *session)
{
    _heart_rate_client_t **link;

    if (!session || session->dispatch_depth > 0U)
        return;

    link = &session->clients;
    while (*link)
    {
        if ((*link)->marked_for_delete)
        {
            _heart_rate_client_t *client = *link;
            *link = client->next;
            eos_free(client);
        }
        else
        {
            link = &(*link)->next;
        }
    }
}

static void _heart_rate_release_session(_heart_rate_session_t *session)
{
    if (!session)
        return;
    if (session->timer)
    {
        lv_timer_delete(session->timer);
        session->timer = NULL;
    }
    _heart_rate_cleanup_clients(session);
    eos_free(session);
}

static void _heart_rate_emit_client(_heart_rate_client_t *client,
                                    eos_sensor_heart_rate_state_t state,
                                    const eos_sensor_heart_rate_result_t *result)
{
    eos_sensor_heart_rate_result_t copied = {0};

    if (!client || !client->active || client->marked_for_delete || !client->callback)
        return;
    if (client->state_reported && client->last_state == state && !_heart_rate_is_terminal(state))
        return;

    client->state_reported = true;
    client->last_state = state;
    if (result)
        memcpy(&copied, result, sizeof(copied));
    client->callback(client->request_id, state, result ? &copied : NULL, client->user_data);
}

static void _heart_rate_emit_all(_heart_rate_session_t *session,
                                 eos_sensor_heart_rate_state_t state,
                                 const eos_sensor_heart_rate_result_t *result)
{
    _heart_rate_client_t *client;

    if (!session)
        return;

    session->dispatch_depth++;
    client = session->clients;
    while (client)
    {
        _heart_rate_client_t *next = client->next;
        _heart_rate_emit_client(client, state, result);
        if (_heart_rate_is_terminal(state))
        {
            client->active = false;
            client->marked_for_delete = true;
        }
        client = next;
    }
    session->dispatch_depth--;
    _heart_rate_cleanup_clients(session);
}

static void _heart_rate_finish(_heart_rate_session_t *session,
                               eos_sensor_heart_rate_state_t state,
                               const eos_sensor_heart_rate_result_t *result)
{
    if (!session || _heart_rate_session != session)
        return;

    session->finishing = true;
    _heart_rate_session = NULL;
    _heart_rate_release_demands(session);
    _heart_rate_emit_all(session, state, result);
    _heart_rate_release_session(session);
}

static void _heart_rate_reset_algorithm(_heart_rate_session_t *request)
{
    request->ir_dc = 0;
    request->red_dc = 0;
    request->dc_initialized = false;
    request->warmup_count = 0U;
    request->signal_local_max = 0;
    request->signal_local_min = 0;
    request->recent_amplitude = 0;
    request->peak_threshold = 0;
    request->above_threshold = false;
    request->last_peak_timestamp = 0U;
    request->bpm_interval_count = 0U;
    request->bpm_interval_index = 0U;
}

static void _heart_rate_push_interval(_heart_rate_session_t *request, uint16_t interval_ms)
{
    request->bpm_intervals[request->bpm_interval_index] = interval_ms;
    request->bpm_interval_index = (uint8_t)((request->bpm_interval_index + 1U) % 5U);
    if (request->bpm_interval_count < 5U)
        request->bpm_interval_count++;
}

static uint16_t _heart_rate_median_interval(const _heart_rate_session_t *request)
{
    uint16_t sorted[5] = {0};

    memcpy(sorted, request->bpm_intervals, sizeof(sorted));
    for (uint32_t i = 0U; i < request->bpm_interval_count; i++)
    {
        for (uint32_t j = i + 1U; j < request->bpm_interval_count; j++)
        {
            if (sorted[j] < sorted[i])
            {
                uint16_t temp = sorted[i];
                sorted[i] = sorted[j];
                sorted[j] = temp;
            }
        }
    }
    return request->bpm_interval_count == 0U ? 0U : sorted[request->bpm_interval_count / 2U];
}

static bool _heart_rate_process_ppg(_heart_rate_session_t *request, uint32_t ir, uint32_t red, uint32_t timestamp)
{
    /* red and ir are the EOS_SENSOR_TYPE_PPG ADC counts.  The generic
     * fallback operates on counts directly; it does not apply an IC-specific
     * voltage/current conversion. */
    request->ppg_seen = true;

    if (ir < _HEART_RATE_FINGER_THRESHOLD)
    {
        _heart_rate_reset_algorithm(request);
        return false;
    }

    if (!request->dc_initialized)
    {
        request->ir_dc = (int32_t)ir;
        request->red_dc = (int32_t)red;
        request->dc_initialized = true;
    }

    request->ir_dc += ((int32_t)ir - request->ir_dc) >> 5U;
    request->red_dc += ((int32_t)red - request->red_dc) >> 5U;

    if (request->warmup_count < _HEART_RATE_WARMUP_SAMPLES)
    {
        request->warmup_count++;
        return false;
    }

    int32_t signal = (int32_t)ir - request->ir_dc;
    if (signal > request->signal_local_max)
        request->signal_local_max = signal;
    if (signal < request->signal_local_min)
        request->signal_local_min = signal;

    int32_t amplitude = request->signal_local_max - request->signal_local_min;
    if (amplitude > request->recent_amplitude)
        request->recent_amplitude = amplitude;
    else
        request->recent_amplitude = (request->recent_amplitude * 7 + amplitude) / 8;
    request->peak_threshold = request->recent_amplitude * 3 / 10;

    if (!request->above_threshold)
    {
        if (signal > request->peak_threshold && request->peak_threshold > _HEART_RATE_MIN_AMPLITUDE)
        {
            request->above_threshold = true;
            request->signal_local_max = signal;
        }
    }
    else
    {
        if (signal > request->signal_local_max)
            request->signal_local_max = signal;
        if (signal < 0)
        {
            if (request->last_peak_timestamp != 0U)
            {
                uint32_t interval_ms = timestamp - request->last_peak_timestamp;
                if (interval_ms >= _HEART_RATE_MIN_INTERVAL_MS && interval_ms <= _HEART_RATE_MAX_INTERVAL_MS)
                    _heart_rate_push_interval(request, (uint16_t)interval_ms);
            }
            request->last_peak_timestamp = timestamp;
            request->above_threshold = false;
            request->signal_local_min = signal;
        }
    }

    return request->bpm_interval_count >= 3U;
}

static bool _heart_rate_read_new(eos_sensor_type_t type,
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

static bool _heart_rate_expire_clients(_heart_rate_session_t *session, uint32_t now)
{
    _heart_rate_client_t *client;

    session->dispatch_depth++;
    client = session->clients;
    while (client)
    {
        _heart_rate_client_t *next = client->next;
        if (client->active && (int32_t)(now - client->deadline) >= 0)
        {
            _heart_rate_emit_client(client, EOS_SENSOR_HEART_RATE_STATE_TIMEOUT, NULL);
            client->active = false;
            client->marked_for_delete = true;
        }
        client = next;
    }
    session->dispatch_depth--;
    _heart_rate_cleanup_clients(session);
    return session->clients == NULL;
}

static void _heart_rate_process(_heart_rate_session_t *request)
{
    uint32_t now = eos_tick_get();
    eos_sensor_raw_data_t hr = {0};
    eos_sensor_raw_data_t ppg = {0};
    bool hr_new = false;
    bool ppg_new = false;

    if (request->pending_failure)
    {
        _heart_rate_emit_all(request, EOS_SENSOR_HEART_RATE_STATE_WAITING, NULL);
        if (_heart_rate_session == request)
            _heart_rate_finish(request, request->failure_state, NULL);
        return;
    }

    _heart_rate_emit_all(request, EOS_SENSOR_HEART_RATE_STATE_WAITING, NULL);
    if (_heart_rate_session != request)
        return;

    if (request->has_hr_source)
        hr_new = _heart_rate_read_new(EOS_SENSOR_TYPE_HR,
                                      &hr,
                                      &request->hr_timestamp_seen,
                                      &request->last_hr_timestamp,
                                      request->started_at);
    if (request->has_ppg_source)
        ppg_new = _heart_rate_read_new(EOS_SENSOR_TYPE_PPG,
                                       &ppg,
                                       &request->ppg_timestamp_seen,
                                       &request->last_ppg_timestamp,
                                       request->started_at);

    if (hr_new)
    {
        request->hr_seen = true;
        if (hr.data.hr.heart_rate >= _HEART_RATE_MIN_BPM && hr.data.hr.heart_rate <= _HEART_RATE_MAX_BPM)
        {
            eos_sensor_heart_rate_result_t result = {
                .bpm = hr.data.hr.heart_rate,
                .timestamp = hr.timestamp,
            };
            _heart_rate_finish(request, EOS_SENSOR_HEART_RATE_STATE_SUCCESS, &result);
            return;
        }
        _heart_rate_emit_all(request, EOS_SENSOR_HEART_RATE_STATE_UNSTABLE, NULL);
        if (_heart_rate_session != request)
            return;
    }

    if (ppg_new)
    {
        if (ppg.data.ppg.ir < _HEART_RATE_FINGER_THRESHOLD)
        {
            (void)_heart_rate_process_ppg(request, ppg.data.ppg.ir, ppg.data.ppg.red, ppg.timestamp);
            _heart_rate_emit_all(request, EOS_SENSOR_HEART_RATE_STATE_NO_CONTACT, NULL);
        }
        else if (!request->has_hr_source
                 && _heart_rate_process_ppg(request, ppg.data.ppg.ir, ppg.data.ppg.red, ppg.timestamp))
        {
            uint16_t interval_ms = _heart_rate_median_interval(request);
            uint16_t bpm = interval_ms == 0U ? 0U : (uint16_t)(60000U / interval_ms);
            if (bpm >= _HEART_RATE_MIN_BPM && bpm <= _HEART_RATE_MAX_BPM)
            {
                eos_sensor_heart_rate_result_t result = {
                    .bpm = bpm,
                    .timestamp = ppg.timestamp,
                };
                _heart_rate_finish(request, EOS_SENSOR_HEART_RATE_STATE_SUCCESS, &result);
                return;
            }
            _heart_rate_emit_all(request, EOS_SENSOR_HEART_RATE_STATE_UNSTABLE, NULL);
        }
        else
        {
            _heart_rate_emit_all(request, EOS_SENSOR_HEART_RATE_STATE_UNSTABLE, NULL);
        }
    }

    if (_heart_rate_session != request)
        return;
    if (!request->hr_seen && !request->ppg_seen
        && (uint32_t)(now - request->started_at) >= _HEART_RATE_NO_DATA_AFTER_MS)
    {
        _heart_rate_emit_all(request, EOS_SENSOR_HEART_RATE_STATE_NO_DATA, NULL);
    }
    if (_heart_rate_expire_clients(request, now))
        _heart_rate_finish(request, EOS_SENSOR_HEART_RATE_STATE_TIMEOUT, NULL);
}

static void _heart_rate_timer_cb(lv_timer_t *timer)
{
    _heart_rate_session_t *request = lv_timer_get_user_data(timer);
    if (request && _heart_rate_session == request)
        _heart_rate_process(request);
}

static eos_sensor_heart_rate_state_t _heart_rate_failure_state(eos_result_t result)
{
    return result == EOS_ERR_DEV_NOT_FOUND ? EOS_SENSOR_HEART_RATE_STATE_UNAVAILABLE
                                           : EOS_SENSOR_HEART_RATE_STATE_ERROR;
}

static eos_result_t _heart_rate_start_timer(_heart_rate_session_t *request)
{
    request->timer = lv_timer_create(_heart_rate_timer_cb, _HEART_RATE_TIMER_PERIOD_MS, request);
    if (!request->timer)
        return EOS_ERR_MEM;
    lv_timer_set_repeat_count(request->timer, -1);
    return EOS_OK;
}

eos_result_t eos_sensor_heart_rate_start(eos_sensor_heart_rate_cb_t callback,
                                         void *user_data,
                                         uint32_t timeout_ms,
                                         eos_sensor_request_id_t *request_id)
{
    _heart_rate_session_t *request;
    _heart_rate_client_t *client;
    eos_result_t result = EOS_OK;
    uint32_t now;

    if (!callback || !request_id)
        return EOS_ERR_INVALID_ARG;

    now = eos_tick_get();
    if (_heart_rate_session)
    {
        client = eos_malloc_zeroed(sizeof(*client));
        if (!client)
            return EOS_ERR_MEM;
        client->request_id = _heart_rate_allocate_request_id();
        client->callback = callback;
        client->user_data = user_data;
        client->deadline = now + (timeout_ms == 0U ? _HEART_RATE_DEFAULT_TIMEOUT_MS : timeout_ms);
        client->active = true;
        client->next = _heart_rate_session->clients;
        _heart_rate_session->clients = client;
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
    client->request_id = _heart_rate_allocate_request_id();
    client->callback = callback;
    client->user_data = user_data;
    client->deadline = now + (timeout_ms == 0U ? _HEART_RATE_DEFAULT_TIMEOUT_MS : timeout_ms);
    client->active = true;
    request->clients = client;
    request->started_at = now;
    request->has_hr_source = eos_dev_sensor_get_default(EOS_SENSOR_TYPE_HR) != NULL;
    request->has_ppg_source = eos_dev_sensor_get_default(EOS_SENSOR_TYPE_PPG) != NULL;
    _heart_rate_reset_algorithm(request);

    if (!request->has_hr_source && !request->has_ppg_source)
    {
        request->pending_failure = true;
        request->failure_state = EOS_SENSOR_HEART_RATE_STATE_UNAVAILABLE;
    }
    else
    {
        if (request->has_hr_source)
            result = eos_sensor_demand_acquire(EOS_SENSOR_TYPE_HR, _HEART_RATE_HR_PERIOD_MS, &request->hr_demand_id);
        if (result == EOS_OK && request->has_ppg_source)
            result = eos_sensor_demand_acquire(EOS_SENSOR_TYPE_PPG, _HEART_RATE_PPG_PERIOD_MS, &request->ppg_demand_id);
        if (result != EOS_OK)
        {
            _heart_rate_release_demands(request);
            if (result == EOS_ERR_DEV_NOT_FOUND || result == EOS_ERR_DEV_ERROR)
            {
                request->pending_failure = true;
                request->failure_state = _heart_rate_failure_state(result);
            }
            else
            {
                eos_free(client);
                eos_free(request);
                return result;
            }
        }
    }

    _heart_rate_session = request;
    result = _heart_rate_start_timer(request);
    if (result != EOS_OK)
    {
        _heart_rate_release_demands(request);
        _heart_rate_session = NULL;
        eos_free(client);
        eos_free(request);
        return result;
    }

    *request_id = client->request_id;
    return EOS_OK;
}

eos_result_t eos_sensor_heart_rate_cancel(eos_sensor_request_id_t request_id)
{
    _heart_rate_client_t *client;

    if (!_heart_rate_session)
        return EOS_ERR_NOT_FOUND;

    client = _heart_rate_session->clients;
    while (client && client->request_id != request_id)
        client = client->next;
    if (!client || !client->active || client->marked_for_delete)
        return EOS_ERR_NOT_FOUND;

    _heart_rate_session->dispatch_depth++;
    _heart_rate_emit_client(client, EOS_SENSOR_HEART_RATE_STATE_CANCELLED, NULL);
    client->active = false;
    client->marked_for_delete = true;
    _heart_rate_session->dispatch_depth--;
    _heart_rate_cleanup_clients(_heart_rate_session);
    if (!_heart_rate_session->clients)
        _heart_rate_finish(_heart_rate_session, EOS_SENSOR_HEART_RATE_STATE_CANCELLED, NULL);
    return EOS_OK;
}
