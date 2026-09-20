/**
 * @file eos_ww_heart_rate.c
 * @brief Watchface heart rate indicator (icon + BPM)
 */

#include "eos_ww_heart_rate.h"

/* Includes ---------------------------------------------------*/
#include "eos_ww_common.h"
#include "eos_icon.h"
#include "eos_mem.h"
#include "eos_service_sensor.h"
#include "eos_sensor_heart_rate.h"
#define EOS_LOG_TAG "HeartRate"
#include "eos_log.h"
/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/
typedef struct
{
    eos_ww_status_t *status;
    eos_sensor_request_id_t request_id;
} _heart_rate_t;

/* Function Implementations -----------------------------------*/

static void _heart_rate_start(_heart_rate_t *heart_rate);

static void _heart_rate_callback(eos_sensor_request_id_t request_id,
                                 eos_sensor_heart_rate_state_t state,
                                 const eos_sensor_heart_rate_result_t *result,
                                 void *user_data)
{
    _heart_rate_t *heart_rate = user_data;
    if (!heart_rate || heart_rate->request_id != request_id)
        return;

    if (state == EOS_SENSOR_HEART_RATE_STATE_SUCCESS && result && result->bpm > 0U)
        eos_ww_status_set_value(heart_rate->status, "%u", result->bpm);
    else
        eos_ww_status_set_value(heart_rate->status, "--");

    if (state == EOS_SENSOR_HEART_RATE_STATE_SUCCESS || state == EOS_SENSOR_HEART_RATE_STATE_TIMEOUT
        || state == EOS_SENSOR_HEART_RATE_STATE_ERROR)
    {
        heart_rate->request_id = EOS_SENSOR_REQUEST_INVALID;
        _heart_rate_start(heart_rate);
    }
}

static void _heart_rate_delete_cb(lv_event_t *event)
{
    _heart_rate_t *heart_rate = lv_event_get_user_data(event);
    if (!heart_rate)
        return;

    heart_rate->status = NULL;
    if (heart_rate->request_id != EOS_SENSOR_REQUEST_INVALID)
    {
        eos_sensor_request_id_t request_id = heart_rate->request_id;
        heart_rate->request_id = EOS_SENSOR_REQUEST_INVALID;
        (void)eos_sensor_cancel(request_id);
    }
    eos_free(heart_rate);
}

static void _heart_rate_start(_heart_rate_t *heart_rate)
{
    eos_sensor_request_id_t request_id;

    if (!heart_rate || !heart_rate->status)
        return;
    if (eos_sensor_heart_rate_start(_heart_rate_callback, heart_rate, 10000U, &request_id) == EOS_OK)
        heart_rate->request_id = request_id;
    else
        eos_ww_status_set_value(heart_rate->status, "--");
}

lv_obj_t *eos_ww_heart_rate_create(lv_obj_t *parent)
{
    EOS_CHECK_PTR_RETURN_VAL(parent, NULL);

    eos_ww_status_t *s = eos_ww_status_create(parent, RI_HEART_PULSE_FILL);
    EOS_CHECK_PTR_RETURN_VAL(s, NULL);

    _heart_rate_t *heart_rate = eos_malloc_zeroed(sizeof(*heart_rate));
    if (!heart_rate)
        return eos_ww_status_get_container(s);
    heart_rate->status = s;
    heart_rate->request_id = EOS_SENSOR_REQUEST_INVALID;
    lv_obj_add_event_cb(eos_ww_status_get_container(s), _heart_rate_delete_cb, LV_EVENT_DELETE, heart_rate);
    _heart_rate_start(heart_rate);
    return eos_ww_status_get_container(s);
}
