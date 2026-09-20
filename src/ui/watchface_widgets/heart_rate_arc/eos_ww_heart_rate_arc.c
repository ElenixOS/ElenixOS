/**
 * @file eos_ww_heart_rate_arc.c
 * @brief Watchface heart rate arc (ring + BPM, colour-zoned)
 */

#include "eos_ww_heart_rate_arc.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include "eos_ww_common.h"
#include "eos_mem.h"
#include "eos_font.h"
#include "eos_service_sensor.h"
#include "eos_sensor_heart_rate.h"
#define EOS_LOG_TAG "HeartRateArc"
#include "eos_log.h"
/* Macros and Definitions -------------------------------------*/
#define _HR_MIN 40
#define _HR_MAX 180
/* Variables --------------------------------------------------*/
typedef struct
{
    lv_obj_t *arc;
    lv_obj_t *label;
    eos_sensor_request_id_t request_id;
} _heart_rate_arc_t;

/* Function Implementations -----------------------------------*/

static lv_color_t _hr_color(uint16_t hr)
{
    if (hr > 140)
    {
        return lv_color_hex(0xFF3B30); /* red */
    }
    if (hr > 100)
    {
        return lv_color_hex(0xFFCC00); /* yellow */
    }
    if (hr < 60)
    {
        return lv_color_hex(0x5AC8FA); /* blue */
    }
    return lv_color_hex(0x4CD964); /* green */
}

static void _heart_rate_arc_start(_heart_rate_arc_t *heart_rate);

static void _heart_rate_arc_callback(eos_sensor_request_id_t request_id,
                                     eos_sensor_heart_rate_state_t state,
                                     const eos_sensor_heart_rate_result_t *result,
                                     void *user_data)
{
    _heart_rate_arc_t *heart_rate = user_data;
    if (!heart_rate || heart_rate->request_id != request_id)
    {
        return;
    }

    if (state == EOS_SENSOR_HEART_RATE_STATE_SUCCESS && result && result->bpm > 0U)
    {
        char buf[8];
        snprintf(buf, sizeof(buf), "%u", result->bpm);
        lv_label_set_text(heart_rate->label, buf);
        lv_arc_set_value(heart_rate->arc, result->bpm);
        lv_obj_set_style_arc_color(heart_rate->arc, _hr_color(result->bpm), LV_PART_INDICATOR);
    }
    else
    {
        lv_label_set_text(heart_rate->label, "--");
        lv_arc_set_value(heart_rate->arc, _HR_MIN);
    }

    if (state == EOS_SENSOR_HEART_RATE_STATE_SUCCESS || state == EOS_SENSOR_HEART_RATE_STATE_TIMEOUT
        || state == EOS_SENSOR_HEART_RATE_STATE_ERROR)
    {
        heart_rate->request_id = EOS_SENSOR_REQUEST_INVALID;
        _heart_rate_arc_start(heart_rate);
    }
}

static void _heart_rate_arc_delete_cb(lv_event_t *e)
{
    _heart_rate_arc_t *heart_rate = lv_event_get_user_data(e);
    if (!heart_rate)
    {
        return;
    }
    heart_rate->arc = NULL;
    heart_rate->label = NULL;
    if (heart_rate->request_id != EOS_SENSOR_REQUEST_INVALID)
    {
        eos_sensor_request_id_t request_id = heart_rate->request_id;
        heart_rate->request_id = EOS_SENSOR_REQUEST_INVALID;
        (void)eos_sensor_cancel(request_id);
    }
    eos_free(heart_rate);
}

static void _heart_rate_arc_start(_heart_rate_arc_t *heart_rate)
{
    eos_sensor_request_id_t request_id;

    if (!heart_rate || !heart_rate->arc || !heart_rate->label)
        return;
    if (eos_sensor_heart_rate_start(_heart_rate_arc_callback, heart_rate, 10000U, &request_id) == EOS_OK)
        heart_rate->request_id = request_id;
}

lv_obj_t *eos_ww_heart_rate_arc_create(lv_obj_t *parent, lv_coord_t size, uint32_t track_color)
{
    EOS_CHECK_PTR_RETURN_VAL(parent, NULL);
    if (size <= 0)
    {
        return NULL;
    }

    _heart_rate_arc_t *h = eos_malloc_zeroed(sizeof(_heart_rate_arc_t));
    EOS_CHECK_PTR_RETURN_VAL(h, NULL);

    lv_obj_t *container = lv_obj_create(parent);
    lv_obj_set_size(container, size, size);
    lv_obj_set_style_bg_opa(container, LV_OPA_TRANSP, 0);
    eos_ww_make_static(container);

    lv_obj_t *arc = lv_arc_create(container);
    lv_obj_set_size(arc, size, size);
    lv_arc_set_range(arc, _HR_MIN, _HR_MAX);
    lv_arc_set_mode(arc, LV_ARC_MODE_NORMAL);
    lv_arc_set_rotation(arc, 0);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
    lv_obj_set_style_arc_width(arc, 6, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(arc, 6, LV_PART_MAIN);
    lv_obj_set_style_arc_color(arc, lv_color_hex(track_color), LV_PART_MAIN);
    lv_obj_set_style_arc_color(arc, lv_color_hex(0x4CD964), LV_PART_INDICATOR);

    lv_obj_t *label = lv_label_create(container);
    lv_label_set_text(label, "--");
    lv_obj_center(label);
    eos_label_set_font_size(label, EOS_FONT_SIZE_SMALL);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    eos_ww_make_static(label);

    h->arc = arc;
    h->label = label;
    h->request_id = EOS_SENSOR_REQUEST_INVALID;

    lv_obj_add_event_cb(container, _heart_rate_arc_delete_cb, LV_EVENT_DELETE, h);
    _heart_rate_arc_start(h);

    return container;
}
