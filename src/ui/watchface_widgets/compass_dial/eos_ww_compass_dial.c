/**
 * @file eos_ww_compass_dial.c
 * @brief Complete compass dial watchface widget
 */

#include "eos_ww_compass_dial.h"

/* Includes ---------------------------------------------------*/
#include <math.h>
#include <stdio.h>
#include "eos_font.h"
#include "eos_mem.h"
#include "eos_sensor_compass.h"
#include "eos_service_sensor.h"
#include "eos_widget_data.h"
#include "eos_ww_internal.h"
#define EOS_LOG_TAG "CompassDial"
#include "eos_log.h"

/* Macros and Definitions -------------------------------------*/
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* Variables --------------------------------------------------*/
typedef struct
{
    lv_obj_t *root;
    lv_obj_t *needle;
    lv_obj_t *heading_label;
    lv_obj_t *state_label;
    lv_point_precise_t needle_points[2];
    lv_coord_t dial_center_x;
    lv_coord_t dial_center_y;
    lv_coord_t dial_radius;
    lv_timer_t *timer;
    eos_sensor_request_id_t request_id;
    float smoothing;
    float displayed_heading;
    float target_heading;
    bool has_heading;
} _compass_dial_t;

/* Function Implementations -----------------------------------*/

static float _compass_wrap_delta(float target, float current)
{
    float delta = target - current;
    while (delta > 180.0f)
        delta -= 360.0f;
    while (delta < -180.0f)
        delta += 360.0f;
    return delta;
}

static void _compass_apply_heading(_compass_dial_t *compass)
{
    char text[24];
    float delta;

    if (!compass || !compass->needle || !compass->has_heading)
        return;
    delta = _compass_wrap_delta(compass->target_heading, compass->displayed_heading);
    if (compass->smoothing > 0.0f && compass->smoothing < 1.0f)
        compass->displayed_heading += delta * compass->smoothing;
    else
        compass->displayed_heading = compass->target_heading;
    if (compass->displayed_heading < 0.0f)
        compass->displayed_heading += 360.0f;
    if (compass->displayed_heading >= 360.0f)
        compass->displayed_heading -= 360.0f;
    {
        double radians = (double)compass->displayed_heading * M_PI / 180.0;
        compass->needle_points[0].x = compass->dial_center_x;
        compass->needle_points[0].y = compass->dial_center_y;
        compass->needle_points[1].x = compass->dial_center_x + sin(radians) * compass->dial_radius;
        compass->needle_points[1].y = compass->dial_center_y - cos(radians) * compass->dial_radius;
        lv_line_set_points(compass->needle, compass->needle_points, 2);
    }
    if (compass->heading_label)
    {
        int32_t heading;

        heading = (int32_t)lroundf(compass->displayed_heading);
        if (heading >= 360)
            heading = 0;
        snprintf(text, sizeof(text), "%03d°", heading);
        lv_label_set_text(compass->heading_label, text);
    }
}

static void _compass_timer_cb(lv_timer_t *timer)
{
    _compass_apply_heading(lv_timer_get_user_data(timer));
}

static void _compass_start(_compass_dial_t *compass);

static void _compass_callback(eos_sensor_request_id_t request_id,
                              eos_sensor_compass_state_t state,
                              const eos_sensor_compass_result_t *result,
                              void *user_data)
{
    _compass_dial_t *compass = user_data;

    if (!compass || compass->request_id != request_id)
        return;
    if (state == EOS_SENSOR_COMPASS_STATE_SUCCESS && result)
    {
        compass->target_heading = (float)result->heading_mdeg / 1000.0f;
        if (!compass->has_heading)
            compass->displayed_heading = compass->target_heading;
        compass->has_heading = true;
        if (compass->state_label)
            lv_label_set_text(compass->state_label, "");
    }
    else if (state == EOS_SENSOR_COMPASS_STATE_UNSTABLE || state == EOS_SENSOR_COMPASS_STATE_NO_DATA)
    {
        if (compass->state_label && !compass->has_heading)
            lv_label_set_text(compass->state_label, "CAL");
    }
    else if (state == EOS_SENSOR_COMPASS_STATE_UNAVAILABLE || state == EOS_SENSOR_COMPASS_STATE_ERROR)
    {
        if (compass->state_label && !compass->has_heading)
            lv_label_set_text(compass->state_label, "--");
    }

    if (state == EOS_SENSOR_COMPASS_STATE_SUCCESS || state == EOS_SENSOR_COMPASS_STATE_TIMEOUT
        || state == EOS_SENSOR_COMPASS_STATE_UNAVAILABLE || state == EOS_SENSOR_COMPASS_STATE_ERROR)
    {
        compass->request_id = EOS_SENSOR_REQUEST_INVALID;
        _compass_start(compass);
    }
}

static void _compass_start(_compass_dial_t *compass)
{
    eos_sensor_request_id_t request_id;
    if (!compass || !compass->root || !lv_obj_is_valid(compass->root))
        return;
    if (eos_sensor_compass_start(_compass_callback, compass, 5000U, &request_id) == EOS_OK)
        compass->request_id = request_id;
}

static void _compass_destroy(void *data)
{
    _compass_dial_t *compass = data;
    if (!compass)
        return;
    compass->root = NULL;
    if (compass->request_id != EOS_SENSOR_REQUEST_INVALID)
    {
        eos_sensor_request_id_t request_id = compass->request_id;
        compass->request_id = EOS_SENSOR_REQUEST_INVALID;
        (void)eos_sensor_compass_cancel(request_id);
    }
    if (compass->timer)
        lv_timer_delete(compass->timer);
    eos_free(compass);
}

static void _compass_create_tick(lv_obj_t *parent,
                                 lv_coord_t cx,
                                 lv_coord_t cy,
                                 lv_coord_t radius,
                                 uint32_t index,
                                 uint32_t count,
                                 bool major)
{
    lv_coord_t width = major ? 3 : 1;
    lv_coord_t length = major ? 10 : 5;
    int32_t angle = (int32_t)((index * 360U) / count);
    lv_obj_t *tick = lv_obj_create(parent);
    if (!tick)
        return;
    lv_obj_set_size(tick, width, length);
    lv_obj_set_pos(tick, cx - width / 2, cy - radius + 8);
    lv_obj_set_style_transform_pivot_x(tick, width / 2, 0);
    lv_obj_set_style_transform_pivot_y(tick, radius - 8, 0);
    lv_obj_set_style_transform_rotation(tick, angle * 10, 0);
    lv_obj_set_style_bg_color(tick, lv_color_hex(major ? 0xD4DAE4 : 0x536174), 0);
    lv_obj_set_style_bg_opa(tick, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(tick, width / 2, 0);
    eos_ww_internal_make_static(tick);
}

static void _compass_create_label(lv_obj_t *parent,
                                  lv_coord_t cx,
                                  lv_coord_t cy,
                                  lv_coord_t radius,
                                  const char *text,
                                  double angle,
                                  uint32_t color)
{
    lv_point_t text_size;
    lv_coord_t label_radius;
    lv_coord_t x;
    lv_coord_t y;
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
    eos_label_set_font_size(label, EOS_FONT_SIZE_SMALL);
    eos_ww_internal_make_static(label);
    lv_text_get_size(&text_size,
                     text,
                     lv_obj_get_style_text_font(label, LV_PART_MAIN),
                     lv_obj_get_style_text_letter_space(label, LV_PART_MAIN),
                     lv_obj_get_style_text_line_space(label, LV_PART_MAIN),
                     LV_COORD_MAX,
                     LV_TEXT_FLAG_NONE);
    label_radius = radius - 8 - 10 - text_size.y / 2 - LV_MAX(3, radius / 24);
    label_radius = LV_MIN(label_radius, radius - LV_MAX(text_size.x, text_size.y) / 2 - 2);
    label_radius = LV_MAX(0, label_radius);
    x = (lv_coord_t)lround(cos(angle) * label_radius);
    y = (lv_coord_t)lround(sin(angle) * label_radius);
    lv_obj_set_size(label, text_size.x, text_size.y);
    lv_obj_set_pos(label, cx + x - text_size.x / 2, cy + y - text_size.y / 2);
}

lv_obj_t *eos_ww_compass_dial_create(lv_obj_t *parent, const eos_ww_compass_dial_config_t *config)
{
    eos_ww_compass_dial_config_t defaults = {
        .width = 200,
        .height = 200,
        .tick_step = 5,
        .major_tick_step = 30,
        .cardinal_labels = true,
        .degree_labels = false,
        .show_heading = true,
        .show_calibration_state = true,
        .smoothing = 0.15f,
    };
    _compass_dial_t *compass;
    lv_obj_t *root;
    lv_coord_t radius;
    lv_coord_t cx;
    lv_coord_t cy;
    lv_coord_t content_width;
    lv_coord_t content_height;
    lv_coord_t heading_height;
    lv_coord_t dial_height;
    uint32_t count;

    EOS_CHECK_PTR_RETURN_VAL(parent, NULL);
    if (config)
        defaults = *config;
    if (defaults.tick_step == 0U)
        defaults.tick_step = 5U;
    if (defaults.major_tick_step == 0U)
        defaults.major_tick_step = 30U;
    if (defaults.width <= 0 || defaults.height <= 0 || defaults.tick_step == 0U)
        return NULL;
    eos_ww_internal_fit_size(parent, &defaults.width, &defaults.height);
    if (defaults.width <= 0 || defaults.height <= 0)
        return NULL;
    compass = eos_malloc_zeroed(sizeof(*compass));
    EOS_CHECK_PTR_RETURN_VAL(compass, NULL);
    root = lv_obj_create(parent);
    if (!root)
    {
        eos_free(compass);
        return NULL;
    }
    lv_obj_set_size(root, defaults.width, defaults.height);
    lv_obj_set_style_radius(root, LV_MIN(defaults.width, defaults.height) / 2, 0);
    lv_obj_set_style_border_width(root, 2, 0);
    lv_obj_set_style_border_color(root, lv_color_hex(0x2E3640), 0);
    eos_ww_internal_make_container(root);
    lv_obj_update_layout(root);
    content_width = lv_obj_get_content_width(root);
    content_height = lv_obj_get_content_height(root);
    heading_height = 0;
    if (defaults.show_heading)
    {
        compass->heading_label = lv_label_create(root);
        lv_label_set_text(compass->heading_label, "---°");
        eos_label_set_font_size(compass->heading_label, EOS_FONT_SIZE_SMALL);
        heading_height = lv_font_get_line_height(lv_obj_get_style_text_font(compass->heading_label, LV_PART_MAIN)) + 6;
        heading_height = LV_MIN(heading_height, content_height / 3);
    }
    dial_height = content_height - heading_height;
    cx = content_width / 2;
    cy = dial_height / 2;
    radius = LV_MIN(content_width, dial_height) / 2;
    radius = LV_MAX(1, radius - 2);
    compass->dial_center_x = cx;
    compass->dial_center_y = cy;
    compass->dial_radius = LV_MAX(1, radius - 10);
    count = 360U / defaults.tick_step;
    for (uint32_t i = 0; i < count; i++)
        _compass_create_tick(root, cx, cy, radius, i, count, (i * defaults.tick_step) % defaults.major_tick_step == 0U);
    if (defaults.cardinal_labels)
    {
        _compass_create_label(root, cx, cy, radius, "N", -M_PI / 2.0, 0xFF3B30);
        _compass_create_label(root, cx, cy, radius, "E", 0.0, 0xD4DAE4);
        _compass_create_label(root, cx, cy, radius, "S", M_PI / 2.0, 0xD4DAE4);
        _compass_create_label(root, cx, cy, radius, "W", M_PI, 0xD4DAE4);
    }

    compass->needle = lv_line_create(root);
    lv_obj_set_size(compass->needle, content_width, content_height);
    lv_obj_set_pos(compass->needle, 0, 0);
    lv_obj_set_style_bg_opa(compass->needle, LV_OPA_TRANSP, 0);
    lv_obj_set_style_line_color(compass->needle, lv_color_hex(0xFF3B30), 0);
    lv_obj_set_style_line_width(compass->needle, 3, 0);
    lv_obj_set_style_line_rounded(compass->needle, true, 0);
    eos_ww_internal_make_static(compass->needle);
    compass->needle_points[0].x = cx;
    compass->needle_points[0].y = cy;
    compass->needle_points[1].x = cx;
    compass->needle_points[1].y = cy - compass->dial_radius;
    lv_line_set_points(compass->needle, compass->needle_points, 2);

    if (defaults.show_heading)
    {
        lv_obj_set_width(compass->heading_label, content_width);
        lv_obj_set_height(compass->heading_label, heading_height);
        lv_obj_set_style_pad_all(compass->heading_label, 0, 0);
        lv_obj_set_style_text_align(compass->heading_label, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_align(compass->heading_label, LV_ALIGN_BOTTOM_MID, 0, 0);
        eos_ww_internal_make_static(compass->heading_label);
    }
    if (defaults.show_calibration_state)
    {
        compass->state_label = lv_label_create(root);
        lv_coord_t state_height;

        lv_label_set_text(compass->state_label, "");
        eos_label_set_font_size(compass->state_label, EOS_FONT_SIZE_SMALL);
        state_height = lv_font_get_line_height(lv_obj_get_style_text_font(compass->state_label, LV_PART_MAIN));
        lv_obj_set_size(compass->state_label, content_width, state_height);
        lv_obj_set_pos(compass->state_label, 0, cy - state_height / 2);
        lv_obj_set_style_text_align(compass->state_label, LV_TEXT_ALIGN_CENTER, 0);
        eos_ww_internal_make_static(compass->state_label);
    }

    compass->root = root;
    compass->smoothing = defaults.smoothing;
    compass->request_id = EOS_SENSOR_REQUEST_INVALID;
    compass->timer = lv_timer_create(_compass_timer_cb, 33U, compass);
    if (!compass->timer)
    {
        lv_obj_delete(root);
        eos_free(compass);
        return NULL;
    }
    eos_wdata_set(root, EOS_WDATA_WW_COMPASS_DIAL, compass, _compass_destroy);
    _compass_start(compass);
    return root;
}
