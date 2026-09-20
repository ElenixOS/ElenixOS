/**
 * @file eos_ww_compass.c
 * @brief Watchface compass (magnetometer-driven needle)
 */

#include "eos_ww_compass.h"

/* Includes ---------------------------------------------------*/
#include "eos_ww_common.h"
#include "eos_mem.h"
#include "eos_font.h"
#include "eos_service_sensor.h"
#include "eos_sensor_compass.h"
#define EOS_LOG_TAG "Compass"
#include "eos_log.h"
/* Macros and Definitions -------------------------------------*/
/* Variables --------------------------------------------------*/
typedef struct
{
    lv_obj_t *needle;
    eos_sensor_request_id_t request_id;
} _compass_t;

/* Function Implementations -----------------------------------*/

static void _compass_start(_compass_t *compass);

static void _compass_callback(eos_sensor_request_id_t request_id,
                              eos_sensor_compass_state_t state,
                              const eos_sensor_compass_result_t *result,
                              void *user_data)
{
    _compass_t *compass = user_data;
    if (!compass || compass->request_id != request_id)
    {
        return;
    }

    if (state == EOS_SENSOR_COMPASS_STATE_SUCCESS && result && compass->needle)
        lv_obj_set_style_transform_rotation(compass->needle, -(int32_t)(result->heading_mdeg / 100), 0);

    if (state == EOS_SENSOR_COMPASS_STATE_SUCCESS || state == EOS_SENSOR_COMPASS_STATE_TIMEOUT
        || state == EOS_SENSOR_COMPASS_STATE_ERROR)
    {
        compass->request_id = EOS_SENSOR_REQUEST_INVALID;
        _compass_start(compass);
    }
}

static void _compass_delete_cb(lv_event_t *e)
{
    _compass_t *compass = lv_event_get_user_data(e);
    if (!compass)
    {
        return;
    }
    compass->needle = NULL;
    if (compass->request_id != EOS_SENSOR_REQUEST_INVALID)
    {
        eos_sensor_request_id_t request_id = compass->request_id;
        compass->request_id = EOS_SENSOR_REQUEST_INVALID;
        (void)eos_sensor_cancel(request_id);
    }
    eos_free(compass);
}

static void _compass_start(_compass_t *compass)
{
    eos_sensor_request_id_t request_id;

    if (!compass || !compass->needle)
        return;
    if (eos_sensor_compass_start(_compass_callback, compass, 5000U, &request_id) == EOS_OK)
        compass->request_id = request_id;
}

lv_obj_t *eos_ww_compass_create(lv_obj_t *parent, lv_coord_t size, uint32_t needle_color)
{
    EOS_CHECK_PTR_RETURN_VAL(parent, NULL);
    if (size <= 0)
    {
        return NULL;
    }

    _compass_t *c = eos_malloc_zeroed(sizeof(_compass_t));
    EOS_CHECK_PTR_RETURN_VAL(c, NULL);

    lv_obj_t *container = lv_obj_create(parent);
    lv_obj_set_size(container, size, size);
    lv_obj_set_style_bg_opa(container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_radius(container, size / 2, 0);
    lv_obj_set_style_border_width(container, 2, 0);
    lv_obj_set_style_border_color(container, lv_color_hex(0x2E3640), 0);
    eos_ww_make_static(container);

    /* North marker */
    lv_obj_t *north = lv_label_create(container);
    lv_label_set_text(north, "N");
    lv_obj_align(north, LV_ALIGN_TOP_MID, 0, 0);
    eos_label_set_font_size(north, EOS_FONT_SIZE_SMALL);
    lv_obj_set_style_text_color(north, lv_color_hex(0xFF3B30), 0);
    eos_ww_make_static(north);

    /* Needle — thin bar rotating around the container centre */
    lv_coord_t needle_h = size * 4 / 10;
    lv_obj_t *needle = lv_obj_create(container);
    lv_obj_set_size(needle, 3, needle_h);
    lv_obj_center(needle);
    lv_obj_set_style_transform_pivot_x(needle, 1, 0);
    lv_obj_set_style_transform_pivot_y(needle, needle_h / 2, 0);
    lv_obj_set_style_bg_color(needle, lv_color_hex(needle_color), 0);
    lv_obj_set_style_bg_opa(needle, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(needle, 1, 0);
    eos_ww_make_static(needle);

    c->needle = needle;
    c->request_id = EOS_SENSOR_REQUEST_INVALID;

    lv_obj_add_event_cb(container, _compass_delete_cb, LV_EVENT_DELETE, c);
    _compass_start(c);

    return container;
}
