/**
 * @file eos_ww_analog_dial.c
 * @brief Complete analog watchface dial
 */

#include "eos_ww_analog_dial.h"

/* Includes ---------------------------------------------------*/
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "eos_mem.h"
#include "eos_service_storage.h"
#include "eos_service_time.h"
#include "eos_widget_data.h"
#include "eos_ww_internal.h"
#define EOS_LOG_TAG "AnalogDial"
#include "eos_log.h"

/* Macros and Definitions -------------------------------------*/
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define _ANALOG_FULL_CIRCLE_TENTHS 3600
#define _ANALOG_DEFAULT_PIVOT_X 0.5f
#define _ANALOG_DEFAULT_PIVOT_Y 0.85f

/* Variables --------------------------------------------------*/
typedef struct
{
    eos_ww_analog_hand_config_t style;
    eos_ww_analog_dial_hand_handle_t handle;
    lv_obj_t *object;
    lv_image_dsc_t image_dsc;
    uint8_t *image_data;
    uint32_t image_data_size;
    int32_t angle_tenths;
} _analog_hand_t;

typedef struct
{
    lv_area_t content_area;
    lv_coord_t center_x;
    lv_coord_t center_y;
    lv_coord_t dial_radius;
    lv_coord_t tick_margin;
    lv_coord_t major_outer_radius;
    lv_coord_t major_inner_radius;
    lv_coord_t number_radius;
    const lv_font_t *number_font;
    lv_point_t number_sizes[12];
    char number_texts[12][4];
} _analog_geometry_t;

typedef struct eos_ww_analog_dial_t _analog_dial_t;

struct eos_ww_analog_dial_t
{
    lv_obj_t *root;
    lv_timer_t *timer;
    _analog_hand_t hands[EOS_WW_ANALOG_DIAL_HAND_COUNT];
    eos_ww_analog_tick_config_t major_ticks;
    eos_ww_analog_tick_config_t minor_ticks;
    eos_ww_analog_center_cap_config_t center_cap;
    bool numerals;
    bool smooth_second;
    bool alive;
    bool geometry_valid;
    _analog_geometry_t geometry;
};

/* Function Prototypes ----------------------------------------*/
static void _analog_event_cb(lv_event_t *event);
static void _analog_update(_analog_dial_t *dial);
static void _analog_layout_hands(_analog_dial_t *dial);
static void _analog_release_image_source(_analog_hand_t *hand);
static bool _analog_load_image_source(_analog_hand_t *hand);
static bool _analog_set_image_source(_analog_hand_t *hand);
static bool _analog_ensure_image(_analog_hand_t *hand);
static void _analog_layout_primitive(_analog_hand_t *hand);
static void _analog_update_geometry(_analog_dial_t *dial);
static void _analog_refresh_ext_draw_size(_analog_dial_t *dial);
static void _analog_draw_triangle(lv_layer_t *layer,
                                  const lv_point_precise_t *p0,
                                  const lv_point_precise_t *p1,
                                  const lv_point_precise_t *p2,
                                  lv_color_t color,
                                  lv_opa_t opacity);
static void _analog_draw_quad(lv_layer_t *layer,
                              const lv_point_precise_t points[4],
                              lv_color_t color,
                              lv_opa_t opacity);

/* Function Implementations -----------------------------------*/
static eos_ww_analog_hand_config_t _analog_default_hand(eos_ww_analog_hand_index_t index)
{
    eos_ww_analog_hand_config_t config = {
        .enabled = true,
        .type = EOS_WW_ANALOG_HAND_BAR,
        .length = 62,
        .width = 6,
        .tail_length = 0,
        .color = lv_color_hex(0xE4E8F0),
        .opacity = LV_OPA_COVER,
        .angle_offset = 0,
        .offset_x = 0,
        .offset_y = 0,
        .pivot_x = _ANALOG_DEFAULT_PIVOT_X,
        .pivot_y = _ANALOG_DEFAULT_PIVOT_Y,
        .pivot_set = false,
        .image_src = {0},
    };

    if (index == EOS_WW_ANALOG_HAND_MINUTE)
    {
        config.length = 88;
        config.width = 4;
        config.color = lv_color_hex(0xC0C8D4);
    }
    else if (index == EOS_WW_ANALOG_HAND_SECOND)
    {
        config.type = EOS_WW_ANALOG_HAND_LINE;
        config.length = 96;
        config.width = 2;
        config.color = lv_color_hex(0xFF3B3B);
    }

    return config;
}

static eos_ww_analog_dial_config_t _analog_default_config(void)
{
    eos_ww_analog_dial_config_t config = {
        .width = 220,
        .height = 220,
        .major_ticks =
            {
                .count = 12,
                .length = 12,
                .width = 3,
                .color = lv_color_hex(0xD4DAE4),
                .opacity = LV_OPA_COVER,
            },
        .minor_ticks =
            {
                .count = 60,
                .length = 5,
                .width = 1,
                .color = lv_color_hex(0x394554),
                .opacity = LV_OPA_COVER,
            },
        .center_cap =
            {
                .enabled = true,
                .radius = 5,
                .color = lv_color_hex(0xFF4040),
                .opacity = LV_OPA_COVER,
            },
        .numerals = true,
        .smooth_second = true,
    };

    for (uint32_t i = 0; i < EOS_WW_ANALOG_DIAL_HAND_COUNT; i++)
        config.hands[i] = _analog_default_hand((eos_ww_analog_hand_index_t)i);

    return config;
}

static int32_t _analog_normalize_angle(int32_t angle_tenths)
{
    angle_tenths %= _ANALOG_FULL_CIRCLE_TENTHS;
    if (angle_tenths < 0)
        angle_tenths += _ANALOG_FULL_CIRCLE_TENTHS;
    return angle_tenths;
}

static _analog_hand_t *_analog_hand_from_handle(eos_ww_analog_dial_hand_handle_t *handle)
{
    if (!handle || !handle->owner || !handle->owner->alive || handle->index >= EOS_WW_ANALOG_DIAL_HAND_COUNT)
        return NULL;
    if (!handle->owner->root || !lv_obj_is_valid(handle->owner->root))
        return NULL;
    return &handle->owner->hands[handle->index];
}

static _analog_hand_t *_analog_hand_from_index(_analog_dial_t *dial, eos_ww_analog_hand_index_t index)
{
    if (!dial || index >= EOS_WW_ANALOG_DIAL_HAND_COUNT)
        return NULL;
    return &dial->hands[index];
}

static void _analog_set_hand_hidden(_analog_hand_t *hand)
{
    if (!hand || !hand->object || !lv_obj_is_valid(hand->object))
        return;

    bool hidden = !hand->style.enabled;
    if (hand->style.type == EOS_WW_ANALOG_HAND_IMAGE)
    {
        /*
         * Do not use the decoded source dimensions as an existence test here.
         * File-backed image metadata can become available after the object is
         * laid out.  Hiding the object at that point leaves it hidden because
         * no later layout pass is guaranteed to run when the decoder finishes.
         */
        hidden = hidden || hand->style.image_src[0] == '\0' || hand->image_data == NULL;
    }
    lv_obj_set_hidden(hand->object, hidden);
}

static void _analog_release_image_source(_analog_hand_t *hand)
{
    if (!hand)
        return;
    if (hand->image_data)
        eos_free(hand->image_data);
    hand->image_data = NULL;
    hand->image_data_size = 0;
    memset(&hand->image_dsc, 0, sizeof(hand->image_dsc));
}

static bool _analog_load_image_source(_analog_hand_t *hand)
{
    eos_file_t file;
    uint32_t file_size;
    uint32_t offset = 0;
    uint8_t *data;

    if (!hand || hand->style.image_src[0] == '\0')
        return false;

    file = eos_storage_file_open_read(hand->style.image_src);
    if (file == EOS_FILE_INVALID)
        return false;
    if (eos_storage_file_size(file, &file_size) != EOS_OK || file_size == 0U)
    {
        eos_storage_file_close(file);
        return false;
    }

    data = eos_malloc(file_size);
    if (!data)
    {
        eos_storage_file_close(file);
        return false;
    }

    while (offset < file_size)
    {
        ssize_t read_size = eos_storage_file_read(file, data + offset, file_size - offset);
        if (read_size <= 0 || (uint32_t)read_size > file_size - offset)
        {
            eos_free(data);
            eos_storage_file_close(file);
            return false;
        }
        offset += (uint32_t)read_size;
    }
    eos_storage_file_close(file);

    _analog_release_image_source(hand);
    hand->image_data = data;
    hand->image_data_size = file_size;
    hand->image_dsc = (lv_image_dsc_t){
        .header =
            {
                .magic = LV_IMAGE_HEADER_MAGIC,
                .cf = LV_COLOR_FORMAT_UNKNOWN,
            },
        .data_size = file_size,
        .data = data,
    };
    return true;
}

static bool _analog_set_image_source(_analog_hand_t *hand)
{
    if (!hand || !hand->object || !lv_obj_is_valid(hand->object))
        return false;

    lv_image_set_src(hand->object, NULL);
    _analog_release_image_source(hand);
    if (hand->style.image_src[0] == '\0')
        return true;
    if (!_analog_load_image_source(hand))
        return false;

    lv_image_set_src(hand->object, &hand->image_dsc);
    return lv_image_get_src_width(hand->object) > 0 && lv_image_get_src_height(hand->object) > 0;
}

static void _analog_layout_image(_analog_hand_t *hand)
{
    lv_area_t content_area;
    lv_point_t pivot;
    float pivot_x;
    float pivot_y;
    lv_coord_t center_x;
    lv_coord_t center_y;

    if (!hand || !hand->object || !hand->handle.owner || !lv_obj_is_valid(hand->object))
        return;

    pivot_x = hand->style.pivot_set ? hand->style.pivot_x : _ANALOG_DEFAULT_PIVOT_X;
    pivot_y = hand->style.pivot_set ? hand->style.pivot_y : _ANALOG_DEFAULT_PIVOT_Y;
    pivot_x = LV_CLAMP(0.0f, pivot_x, 1.0f);
    pivot_y = LV_CLAMP(0.0f, pivot_y, 1.0f);

    /* LVGL v9 stores image percentage pivots in the 0..100 range. */
    lv_image_set_pivot(hand->object,
                       lv_pct((int32_t)llround((double)pivot_x * 100.0)),
                       lv_pct((int32_t)llround((double)pivot_y * 100.0)));
    lv_obj_update_layout(hand->object);
    lv_image_get_pivot(hand->object, &pivot);

    _analog_update_geometry(hand->handle.owner);
    lv_obj_get_content_coords(hand->handle.owner->root, &content_area);
    center_x = hand->handle.owner->geometry.center_x - content_area.x1;
    center_y = hand->handle.owner->geometry.center_y - content_area.y1;
    lv_obj_set_pos(hand->object, center_x + hand->style.offset_x - pivot.x, center_y + hand->style.offset_y - pivot.y);
    lv_image_set_rotation(hand->object, hand->angle_tenths);
    _analog_set_hand_hidden(hand);
}

static bool _analog_ensure_image(_analog_hand_t *hand)
{
    if (!hand || !hand->handle.owner || !hand->handle.owner->root)
        return false;
    if (hand->object && lv_obj_is_valid(hand->object))
        return true;

    hand->object = lv_image_create(hand->handle.owner->root);
    if (!hand->object)
        return false;

    eos_ww_internal_make_static(hand->object);
    lv_obj_set_style_image_opa(hand->object, hand->style.opacity, LV_PART_MAIN);
    if (!_analog_set_image_source(hand) && hand->style.image_src[0] != '\0')
        EOS_LOG_W("Image hand source unavailable: %s", hand->style.image_src);
    _analog_layout_image(hand);
    return true;
}

typedef struct
{
    double length;
    double tail;
    double half_width;
    double head_half_width;
    lv_coord_t width;
    lv_coord_t height;
    double pivot_x;
    double pivot_y;
} _analog_local_geometry_t;

static void _analog_get_local_geometry(const _analog_hand_t *hand, _analog_local_geometry_t *geometry)
{
    _analog_dial_t *dial;
    const eos_ww_analog_hand_config_t *style;
    double offset_radius;
    double safe_radius;
    double max_half_width;

    if (!geometry)
        return;
    memset(geometry, 0, sizeof(*geometry));
    if (!hand || !hand->handle.owner)
        return;

    dial = hand->handle.owner;
    style = &hand->style;
    _analog_update_geometry(dial);

    geometry->half_width = LV_MAX(1, style->width) / 2.0;
    geometry->length = (double)LV_MAX(0, style->length);
    geometry->tail = (double)LV_MAX(0, style->tail_length);
    offset_radius = sqrt((double)style->offset_x * style->offset_x + (double)style->offset_y * style->offset_y);
    safe_radius = LV_MAX(0.0, (double)dial->geometry.major_outer_radius - offset_radius - geometry->half_width - 1.0);
    geometry->length = LV_MIN(geometry->length, safe_radius);
    geometry->tail = LV_MIN(geometry->tail, safe_radius);

    max_half_width = geometry->half_width;
    if (style->type == EOS_WW_ANALOG_HAND_ARROW)
        max_half_width = LV_MAX(geometry->half_width * 2.0, geometry->half_width + 2.0);
    geometry->head_half_width = max_half_width;
    /*
     * LVGL transform pivots are integer coordinates.  A primitive width of
     * 1, 3, 5, ... otherwise produces a fractional local centre which is
     * rounded independently from the object position.  The hand can then
     * draw around one axis while rotating around the neighbouring pixel.  An
     * An integer-centred local canvas gives the drawing and the transform one
     * shared pivot without changing the configured visual width.  The extra
     * pixel on each side also keeps the custom draw primitive inside its own
     * object when its configured width is odd.
     */
    geometry->width = LV_MAX(1, 2 * (lv_coord_t)ceil(max_half_width) + 1);
    geometry->height = LV_MAX(1, (lv_coord_t)ceil(geometry->length + geometry->tail));
    geometry->pivot_x = (geometry->width - 1) / 2.0;
    geometry->pivot_y = geometry->length;
}

static void _analog_layout_primitive(_analog_hand_t *hand)
{
    _analog_local_geometry_t geometry;
    lv_area_t content_area;
    lv_coord_t center_x;
    lv_coord_t center_y;

    if (!hand || !hand->object || !hand->handle.owner || !lv_obj_is_valid(hand->object))
        return;

    _analog_get_local_geometry(hand, &geometry);
    lv_obj_set_size(hand->object, geometry.width, geometry.height);
    lv_obj_set_style_transform_pivot_x(hand->object, (lv_coord_t)llround(geometry.pivot_x), LV_PART_MAIN);
    lv_obj_set_style_transform_pivot_y(hand->object, (lv_coord_t)llround(geometry.pivot_y), LV_PART_MAIN);
    lv_obj_set_style_transform_rotation(hand->object, hand->angle_tenths, LV_PART_MAIN);

    lv_obj_get_content_coords(hand->handle.owner->root, &content_area);
    center_x = hand->handle.owner->geometry.center_x - content_area.x1;
    center_y = hand->handle.owner->geometry.center_y - content_area.y1;
    lv_obj_set_pos(hand->object,
                   (lv_coord_t)llround((double)center_x + hand->style.offset_x - geometry.pivot_x),
                   (lv_coord_t)llround((double)center_y + hand->style.offset_y - geometry.pivot_y));
    _analog_set_hand_hidden(hand);
}

static void _analog_local_point(const lv_area_t *area,
                                const _analog_local_geometry_t *geometry,
                                double along,
                                double across,
                                lv_point_precise_t *point)
{
    point->x = (lv_value_precise_t)(area->x1 + geometry->pivot_x + across);
    point->y = (lv_value_precise_t)(area->y1 + geometry->pivot_y - along);
}

static void _analog_draw_local_hand(const _analog_hand_t *hand, lv_layer_t *layer)
{
    _analog_local_geometry_t geometry;
    lv_area_t area;
    lv_point_precise_t p0;
    lv_point_precise_t p1;
    lv_point_precise_t p2;
    lv_point_precise_t p3;
    lv_draw_line_dsc_t line_dsc;
    const eos_ww_analog_hand_config_t *style;

    if (!hand || !hand->object || !lv_obj_is_valid(hand->object) || hand->style.type == EOS_WW_ANALOG_HAND_IMAGE
        || !hand->style.enabled)
        return;

    style = &hand->style;
    _analog_get_local_geometry(hand, &geometry);
    if (geometry.length <= 0.0 || style->width <= 0 || style->opacity <= LV_OPA_MIN)
        return;
    lv_obj_get_content_coords(hand->object, &area);

    switch (style->type)
    {
        case EOS_WW_ANALOG_HAND_LINE:
            _analog_local_point(&area, &geometry, -geometry.tail, 0.0, &p0);
            _analog_local_point(&area, &geometry, geometry.length, 0.0, &p1);
            lv_draw_line_dsc_init(&line_dsc);
            line_dsc.base.layer = layer;
            line_dsc.p1 = p0;
            line_dsc.p2 = p1;
            line_dsc.color = style->color;
            line_dsc.width = LV_MAX(1, style->width);
            line_dsc.opa = style->opacity;
            line_dsc.round_start = 1;
            line_dsc.round_end = 1;
            lv_draw_line(layer, &line_dsc);
            break;

        case EOS_WW_ANALOG_HAND_BAR:
            _analog_local_point(&area, &geometry, -geometry.tail, -geometry.half_width, &p0);
            _analog_local_point(&area, &geometry, geometry.length, -geometry.half_width, &p1);
            _analog_local_point(&area, &geometry, geometry.length, geometry.half_width, &p2);
            _analog_local_point(&area, &geometry, -geometry.tail, geometry.half_width, &p3);
            _analog_draw_quad(layer, (const lv_point_precise_t[4]){p0, p1, p2, p3}, style->color, style->opacity);
            break;

        case EOS_WW_ANALOG_HAND_NEEDLE:
            _analog_local_point(&area, &geometry, geometry.length, 0.0, &p0);
            _analog_local_point(&area, &geometry, -geometry.tail, -geometry.half_width, &p1);
            _analog_local_point(&area, &geometry, -geometry.tail, geometry.half_width, &p2);
            _analog_draw_triangle(layer, &p0, &p1, &p2, style->color, style->opacity);
            break;

        case EOS_WW_ANALOG_HAND_DIAMOND:
            _analog_local_point(&area, &geometry, geometry.length, 0.0, &p0);
            _analog_local_point(&area, &geometry, (geometry.length - geometry.tail) / 2.0, geometry.half_width, &p1);
            _analog_local_point(&area, &geometry, -geometry.tail, 0.0, &p2);
            _analog_local_point(&area, &geometry, (geometry.length - geometry.tail) / 2.0, -geometry.half_width, &p3);
            _analog_draw_quad(layer, (const lv_point_precise_t[4]){p0, p1, p2, p3}, style->color, style->opacity);
            break;

        case EOS_WW_ANALOG_HAND_ARROW:
        {
            const double head_length = LV_MAX(geometry.half_width * 2.0, 8.0);
            const double body_end = LV_MAX(0.0, geometry.length - head_length);
            lv_point_precise_t body0;
            lv_point_precise_t body1;
            lv_point_precise_t body2;
            lv_point_precise_t body3;
            lv_point_precise_t head;
            lv_point_precise_t head_left;
            lv_point_precise_t head_right;

            _analog_local_point(&area, &geometry, -geometry.tail, -geometry.half_width, &body0);
            _analog_local_point(&area, &geometry, body_end, -geometry.half_width, &body1);
            _analog_local_point(&area, &geometry, body_end, geometry.half_width, &body2);
            _analog_local_point(&area, &geometry, -geometry.tail, geometry.half_width, &body3);
            _analog_local_point(&area, &geometry, body_end, -geometry.head_half_width, &head_left);
            _analog_local_point(&area, &geometry, body_end, geometry.head_half_width, &head_right);
            _analog_local_point(&area, &geometry, geometry.length, 0.0, &head);
            _analog_draw_quad(layer,
                              (const lv_point_precise_t[4]){body0, body1, body2, body3},
                              style->color,
                              style->opacity);
            _analog_draw_triangle(layer, &head, &head_right, &head_left, style->color, style->opacity);
            break;
        }

        case EOS_WW_ANALOG_HAND_IMAGE:
        default:
            break;
    }
}

static void _analog_hand_event_cb(lv_event_t *event)
{
    _analog_hand_t *hand = lv_event_get_user_data(event);
    if (lv_event_get_code(event) == LV_EVENT_DRAW_MAIN)
        _analog_draw_local_hand(hand, lv_event_get_layer(event));
}

static bool _analog_ensure_primitive(_analog_hand_t *hand)
{
    if (!hand || !hand->handle.owner || !hand->handle.owner->root)
        return false;
    if (hand->object && lv_obj_is_valid(hand->object))
        return true;

    hand->object = lv_obj_create(hand->handle.owner->root);
    if (!hand->object)
        return false;
    eos_ww_internal_make_static(hand->object);
    lv_obj_set_style_bg_opa(hand->object, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_add_event_cb(hand->object, _analog_hand_event_cb, LV_EVENT_DRAW_MAIN, hand);
    _analog_layout_primitive(hand);
    return true;
}

static void _analog_refresh_ext_draw_size(_analog_dial_t *dial)
{
    if (!dial || !dial->root || !lv_obj_is_valid(dial->root))
        return;
    lv_obj_refresh_ext_draw_size(dial->root);
}

static void _analog_point(lv_point_precise_t *point,
                          lv_coord_t center_x,
                          lv_coord_t center_y,
                          double angle_degrees,
                          double radius)
{
    double radians = angle_degrees * M_PI / 180.0;
    point->x = (lv_value_precise_t)((double)center_x + sin(radians) * radius);
    point->y = (lv_value_precise_t)((double)center_y - cos(radians) * radius);
}

static void _analog_draw_triangle(lv_layer_t *layer,
                                  const lv_point_precise_t *p0,
                                  const lv_point_precise_t *p1,
                                  const lv_point_precise_t *p2,
                                  lv_color_t color,
                                  lv_opa_t opacity)
{
    lv_draw_triangle_dsc_t dsc;

    lv_draw_triangle_dsc_init(&dsc);
    dsc.base.layer = layer;
    dsc.p[0] = *p0;
    dsc.p[1] = *p1;
    dsc.p[2] = *p2;
    dsc.color = color;
    dsc.opa = opacity;
    lv_draw_triangle(layer, &dsc);
}

static void _analog_draw_quad(lv_layer_t *layer, const lv_point_precise_t points[4], lv_color_t color, lv_opa_t opacity)
{
    _analog_draw_triangle(layer, &points[0], &points[1], &points[2], color, opacity);
    _analog_draw_triangle(layer, &points[0], &points[2], &points[3], color, opacity);
}

static void _analog_measure_numbers(const lv_font_t *font, char texts[12][4], lv_point_t sizes[12])
{
    for (uint8_t i = 0; i < 12; i++)
    {
        (void)snprintf(texts[i], sizeof(texts[i]), "%u", (unsigned)(i + 1));
        lv_text_get_size(&sizes[i], texts[i], font, 0, 0, LV_COORD_MAX, LV_TEXT_FLAG_NONE);
    }
}

static void _analog_number_area(const _analog_geometry_t *geometry,
                                const lv_point_t sizes[12],
                                uint8_t index,
                                lv_coord_t number_radius,
                                lv_area_t *area)
{
    double angle = (double)((index + 1U) % 12U) * 30.0;
    double radians = angle * M_PI / 180.0;
    lv_coord_t x = (lv_coord_t)llround(sin(radians) * number_radius);
    lv_coord_t y = (lv_coord_t)llround(-cos(radians) * number_radius);

    area->x1 = geometry->center_x + x - sizes[index].x / 2;
    area->y1 = geometry->center_y + y - sizes[index].y / 2;
    area->x2 = area->x1 + sizes[index].x - 1;
    area->y2 = area->y1 + sizes[index].y - 1;
}

static bool _analog_number_layout_valid(const _analog_geometry_t *geometry,
                                        const lv_point_t sizes[12],
                                        lv_coord_t number_radius,
                                        lv_coord_t outer_limit,
                                        lv_coord_t center_limit)
{
    lv_area_t areas[12];
    int64_t outer_squared = (int64_t)outer_limit * outer_limit;
    lv_coord_t max_half_diagonal = 0;

    if (number_radius <= 0 || outer_limit <= 0 || number_radius <= center_limit)
        return false;

    for (uint8_t i = 0; i < 12; i++)
    {
        int32_t half_width = sizes[i].x / 2;
        int32_t half_height = sizes[i].y / 2;
        lv_area_t area;
        const lv_coord_t half_diagonal =
            (lv_coord_t)ceil(sqrt((double)half_width * half_width + (double)half_height * half_height));

        max_half_diagonal = LV_MAX(max_half_diagonal, half_diagonal);
        _analog_number_area(geometry, sizes, i, number_radius, &area);
        areas[i] = area;

        const int32_t dx[4] = {area.x1 - geometry->center_x,
                               area.x1 - geometry->center_x,
                               area.x2 - geometry->center_x,
                               area.x2 - geometry->center_x};
        const int32_t dy[4] = {area.y1 - geometry->center_y,
                               area.y2 - geometry->center_y,
                               area.y1 - geometry->center_y,
                               area.y2 - geometry->center_y};
        for (uint8_t corner = 0; corner < 4; corner++)
        {
            int64_t distance_squared = (int64_t)dx[corner] * dx[corner] + (int64_t)dy[corner] * dy[corner];
            if (distance_squared > outer_squared)
                return false;
        }
    }

    if (number_radius - max_half_diagonal < center_limit)
        return false;

    for (uint8_t i = 0; i < 12; i++)
    {
        for (uint8_t j = (uint8_t)(i + 1); j < 12; j++)
        {
            bool separated = areas[i].x2 < areas[j].x1 || areas[j].x2 < areas[i].x1 || areas[i].y2 < areas[j].y1
                             || areas[j].y2 < areas[i].y1;
            if (!separated)
                return false;
        }
    }
    return true;
}

static bool _analog_try_number_font(_analog_geometry_t *geometry,
                                    const lv_font_t *font,
                                    lv_coord_t outer_limit,
                                    lv_coord_t center_limit)
{
    lv_point_t sizes[12];
    char texts[12][4];

    _analog_measure_numbers(font, texts, sizes);
    for (lv_coord_t radius = outer_limit; radius > 0; radius--)
    {
        if (_analog_number_layout_valid(geometry, sizes, radius, outer_limit, center_limit))
        {
            geometry->number_font = font;
            geometry->number_radius = radius;
            memcpy(geometry->number_sizes, sizes, sizeof(sizes));
            memcpy(geometry->number_texts, texts, sizeof(texts));
            return true;
        }
    }
    return false;
}

static void _analog_update_geometry(_analog_dial_t *dial)
{
    _analog_geometry_t *geometry;
    const lv_font_t *base_font;
    const lv_font_t *font_candidates[4];
    uint8_t font_count = 0;
    lv_coord_t content_width;
    lv_coord_t content_height;
    lv_coord_t label_gap;
    lv_coord_t outer_limit;
    lv_coord_t center_limit;
    lv_area_t old_content_area;

    if (!dial || !dial->root || !lv_obj_is_valid(dial->root))
        return;

    old_content_area = dial->geometry.content_area;
    lv_obj_get_content_coords(dial->root, &dial->geometry.content_area);
    content_width = lv_area_get_width(&dial->geometry.content_area);
    content_height = lv_area_get_height(&dial->geometry.content_area);
    if (dial->geometry_valid && old_content_area.x1 == dial->geometry.content_area.x1
        && old_content_area.y1 == dial->geometry.content_area.y1
        && old_content_area.x2 == dial->geometry.content_area.x2
        && old_content_area.y2 == dial->geometry.content_area.y2)
    {
        return;
    }

    geometry = &dial->geometry;
    geometry->center_x = (geometry->content_area.x1 + geometry->content_area.x2) / 2;
    geometry->center_y = (geometry->content_area.y1 + geometry->content_area.y2) / 2;
    geometry->dial_radius = LV_MIN(content_width, content_height) / 2;
    geometry->tick_margin = LV_MAX(4, geometry->dial_radius / 10);
    geometry->major_outer_radius = LV_MAX(0, geometry->dial_radius - geometry->tick_margin);
    geometry->major_inner_radius = LV_MAX(0,
                                          geometry->major_outer_radius - LV_MAX(0, dial->major_ticks.length)
                                              - LV_MAX(0, dial->major_ticks.width + 1) / 2);
    geometry->number_radius = 0;
    geometry->number_font = NULL;
    dial->geometry_valid = false;

    if (!dial->numerals || dial->major_ticks.count == 0U || geometry->dial_radius <= 0)
    {
        dial->geometry_valid = true;
        return;
    }

    label_gap = LV_MAX(4, geometry->dial_radius / 24);
    outer_limit = LV_MAX(0, geometry->major_inner_radius - label_gap);
    center_limit =
        LV_MAX(dial->center_cap.enabled ? dial->center_cap.radius + label_gap : geometry->dial_radius / 10, label_gap);
    base_font = lv_obj_get_style_text_font(dial->root, LV_PART_MAIN);
    if (!base_font)
        base_font = lv_font_get_default();
    font_candidates[font_count++] = base_font;
#if LV_FONT_MONTSERRAT_16
    font_candidates[font_count++] = &lv_font_montserrat_16;
#endif
#if LV_FONT_MONTSERRAT_14
    font_candidates[font_count++] = &lv_font_montserrat_14;
#endif

    for (uint8_t i = 0; i < font_count; i++)
    {
        if (_analog_try_number_font(geometry, font_candidates[i], outer_limit, center_limit))
            break;
    }

    if (!geometry->number_font)
    {
        /* No font can satisfy the geometry; do not draw a clipped or overlapping ring. */
        geometry->number_radius = 0;
    }
    dial->geometry_valid = true;
}

static void _analog_draw_ticks_and_numbers(lv_obj_t *root, _analog_dial_t *dial, lv_layer_t *layer)
{
    _analog_geometry_t *geometry = &dial->geometry;
    uint16_t count;

    (void)root;
    _analog_update_geometry(dial);
    count = dial->minor_ticks.count;
    if (geometry->dial_radius <= 0)
        return;

    for (uint32_t i = 0; i < count; i++)
    {
        bool major = dial->major_ticks.count > 0U && (i * dial->major_ticks.count) % count == 0U;
        const eos_ww_analog_tick_config_t *style = major ? &dial->major_ticks : &dial->minor_ticks;
        lv_draw_line_dsc_t dsc;
        lv_point_precise_t outer;
        lv_point_precise_t inner;
        double angle = (double)i * 360.0 / (double)count;
        double outer_radius = geometry->major_outer_radius;
        double inner_radius = LV_MAX(0, outer_radius - LV_MAX(0, style->length));

        if (count == 0U)
            break;
        if (style->width <= 0 || style->opacity <= LV_OPA_MIN)
            continue;
        _analog_point(&outer, geometry->center_x, geometry->center_y, angle, outer_radius);
        _analog_point(&inner, geometry->center_x, geometry->center_y, angle, inner_radius);
        lv_draw_line_dsc_init(&dsc);
        dsc.base.layer = layer;
        dsc.p1 = inner;
        dsc.p2 = outer;
        dsc.color = style->color;
        dsc.width = style->width;
        dsc.opa = style->opacity;
        dsc.round_start = 1;
        dsc.round_end = 1;
        lv_draw_line(layer, &dsc);
    }

    if (dial->numerals && dial->major_ticks.count > 0U && geometry->number_font)
    {
        for (uint8_t i = 0; i < 12; i++)
        {
            lv_draw_label_dsc_t dsc;
            lv_area_t label_area;

            lv_draw_label_dsc_init(&dsc);
            dsc.base.layer = layer;
            dsc.text = geometry->number_texts[i];
            dsc.text_size = geometry->number_sizes[i];
            dsc.text_static = 1;
            dsc.font = geometry->number_font;
            dsc.color = lv_color_hex(0xD4DAE4);
            dsc.opa = LV_OPA_COVER;
            dsc.align = LV_TEXT_ALIGN_CENTER;
            _analog_number_area(geometry, geometry->number_sizes, i, geometry->number_radius, &label_area);
            lv_draw_label(layer, &dsc, &label_area);
        }
    }
}

static void _analog_draw_center_cap(_analog_dial_t *dial, lv_layer_t *layer)
{
    lv_area_t cap_area;
    lv_draw_rect_dsc_t dsc;
    lv_coord_t center_x;
    lv_coord_t center_y;
    lv_coord_t radius;

    if (!dial->center_cap.enabled || dial->center_cap.radius <= 0 || dial->center_cap.opacity <= LV_OPA_MIN)
        return;
    _analog_update_geometry(dial);
    center_x = dial->geometry.center_x;
    center_y = dial->geometry.center_y;
    radius = dial->center_cap.radius;
    cap_area.x1 = center_x - radius;
    cap_area.y1 = center_y - radius;
    cap_area.x2 = center_x + radius;
    cap_area.y2 = center_y + radius;

    lv_draw_rect_dsc_init(&dsc);
    dsc.base.layer = layer;
    dsc.radius = LV_RADIUS_CIRCLE;
    dsc.bg_color = dial->center_cap.color;
    dsc.bg_opa = dial->center_cap.opacity;
    lv_draw_rect(layer, &dsc, &cap_area);
}

static void _analog_event_cb(lv_event_t *event)
{
    lv_obj_t *root = lv_event_get_current_target(event);
    _analog_dial_t *dial = (root != NULL) ? eos_wdata_get(root, EOS_WDATA_WW_ANALOG_DIAL) : NULL;
    lv_event_code_t code = lv_event_get_code(event);

    if (!dial || !dial->alive)
        return;

    if (code == LV_EVENT_DRAW_MAIN)
    {
        _analog_draw_ticks_and_numbers(root, dial, lv_event_get_layer(event));
    }
    else if (code == LV_EVENT_DRAW_POST)
    {
        _analog_draw_center_cap(dial, lv_event_get_layer(event));
    }
    else if (code == LV_EVENT_SIZE_CHANGED)
    {
        dial->geometry_valid = false;
        _analog_layout_hands(dial);
    }
    else if (code == LV_EVENT_REFR_EXT_DRAW_SIZE)
    {
        int32_t extent = 0;
        for (uint32_t i = 0; i < EOS_WW_ANALOG_DIAL_HAND_COUNT; i++)
        {
            const eos_ww_analog_hand_config_t *style = &dial->hands[i].style;
            int32_t hand_extent = LV_MAX(style->length, style->tail_length) + style->width
                                  + LV_MAX(LV_ABS(style->offset_x), LV_ABS(style->offset_y));
            extent = LV_MAX(extent, hand_extent);
            if (dial->hands[i].style.type == EOS_WW_ANALOG_HAND_IMAGE && dial->hands[i].object
                && lv_obj_is_valid(dial->hands[i].object))
            {
                extent = LV_MAX(extent,
                                LV_MAX(lv_image_get_transformed_width(dial->hands[i].object),
                                       lv_image_get_transformed_height(dial->hands[i].object)));
            }
        }
        lv_event_set_ext_draw_size(event, LV_MAX(0, extent));
    }
}

static double _analog_hour_angle(const eos_datetime_t *now)
{
    double seconds = (double)now->sec + (double)now->ms / 1000.0;
    return (double)(now->hour % 12) * 30.0 + (double)now->min * 0.5 + seconds / 120.0;
}

static double _analog_minute_angle(const eos_datetime_t *now)
{
    double seconds = (double)now->sec + (double)now->ms / 1000.0;
    return (double)now->min * 6.0 + seconds * 0.1;
}

static double _analog_second_angle(const eos_datetime_t *now)
{
    return ((double)now->sec + (double)now->ms / 1000.0) * 6.0;
}

static void _analog_update(_analog_dial_t *dial)
{
    eos_datetime_t now;
    double calculated[EOS_WW_ANALOG_DIAL_HAND_COUNT];

    if (!dial || !dial->alive || !dial->root || !lv_obj_is_valid(dial->root))
        return;

    now = eos_time_get();
    calculated[EOS_WW_ANALOG_HAND_HOUR] = _analog_hour_angle(&now);
    calculated[EOS_WW_ANALOG_HAND_MINUTE] = _analog_minute_angle(&now);
    calculated[EOS_WW_ANALOG_HAND_SECOND] = _analog_second_angle(&now);
    for (uint32_t i = 0; i < EOS_WW_ANALOG_DIAL_HAND_COUNT; i++)
    {
        int32_t angle_tenths = (int32_t)llround(calculated[i] * 10.0) + dial->hands[i].style.angle_offset * 10;
        dial->hands[i].angle_tenths = _analog_normalize_angle(angle_tenths);
        if (dial->hands[i].object && lv_obj_is_valid(dial->hands[i].object))
        {
            if (dial->hands[i].style.type == EOS_WW_ANALOG_HAND_IMAGE)
                lv_image_set_rotation(dial->hands[i].object, dial->hands[i].angle_tenths);
            else
                lv_obj_set_style_transform_rotation(dial->hands[i].object, dial->hands[i].angle_tenths, LV_PART_MAIN);
        }
    }
    lv_obj_invalidate(dial->root);
}

static void _analog_timer_cb(lv_timer_t *timer)
{
    _analog_update((_analog_dial_t *)lv_timer_get_user_data(timer));
}

static void _analog_layout_hands(_analog_dial_t *dial)
{
    if (!dial)
        return;
    for (uint32_t i = 0; i < EOS_WW_ANALOG_DIAL_HAND_COUNT; i++)
    {
        if (dial->hands[i].style.type == EOS_WW_ANALOG_HAND_IMAGE)
            _analog_layout_image(&dial->hands[i]);
        else
            _analog_layout_primitive(&dial->hands[i]);
    }
}

static void _analog_destroy(void *data)
{
    _analog_dial_t *dial = data;
    if (!dial)
        return;

    dial->alive = false;
    for (uint32_t i = 0; i < EOS_WW_ANALOG_DIAL_HAND_COUNT; i++)
    {
        if (dial->hands[i].style.type == EOS_WW_ANALOG_HAND_IMAGE && dial->hands[i].object
            && lv_obj_is_valid(dial->hands[i].object))
        {
            lv_image_set_src(dial->hands[i].object, NULL);
        }
        _analog_release_image_source(&dial->hands[i]);
        dial->hands[i].handle.owner = NULL;
    }
    if (dial->timer)
    {
        lv_timer_set_user_data(dial->timer, NULL);
        lv_timer_set_cb(dial->timer, NULL);
        lv_timer_delete(dial->timer);
        dial->timer = NULL;
    }
    /* Hand children belong to root and are deleted by LVGL with root. */
    eos_free(dial);
}

lv_obj_t *eos_ww_analog_dial_create(lv_obj_t *parent, const eos_ww_analog_dial_config_t *config)
{
    eos_ww_analog_dial_config_t defaults = _analog_default_config();
    _analog_dial_t *dial;
    lv_obj_t *root;

    EOS_CHECK_PTR_RETURN_VAL(parent, NULL);
    if (config)
        defaults = *config;
    if (defaults.width <= 0 || defaults.height <= 0 || defaults.minor_ticks.count == 0U)
        return NULL;
    if (defaults.major_ticks.count > defaults.minor_ticks.count)
        defaults.major_ticks.count = defaults.minor_ticks.count;
    eos_ww_internal_fit_size(parent, &defaults.width, &defaults.height);
    if (defaults.width <= 0 || defaults.height <= 0)
        return NULL;

    dial = eos_malloc_zeroed(sizeof(*dial));
    EOS_CHECK_PTR_RETURN_VAL(dial, NULL);
    root = lv_obj_create(parent);
    if (!root)
    {
        eos_free(dial);
        return NULL;
    }

    lv_obj_set_size(root, defaults.width, defaults.height);
    eos_ww_internal_make_container(root);
    lv_obj_set_overflow_visible(root, true);
    lv_obj_update_layout(root);
    dial->root = root;
    dial->major_ticks = defaults.major_ticks;
    dial->minor_ticks = defaults.minor_ticks;
    dial->center_cap = defaults.center_cap;
    dial->numerals = defaults.numerals;
    dial->smooth_second = defaults.smooth_second;
    dial->alive = true;

    for (uint32_t i = 0; i < EOS_WW_ANALOG_DIAL_HAND_COUNT; i++)
    {
        dial->hands[i].style = defaults.hands[i];
        dial->hands[i].style.image_src[EOS_WW_ANALOG_DIAL_IMAGE_SRC_MAX - 1U] = '\0';
        dial->hands[i].handle.owner = dial;
        dial->hands[i].handle.index = (uint8_t)i;
        bool created = dial->hands[i].style.type == EOS_WW_ANALOG_HAND_IMAGE
                           ? _analog_ensure_image(&dial->hands[i])
                           : _analog_ensure_primitive(&dial->hands[i]);
        if (!created)
        {
            lv_obj_delete(root);
            eos_free(dial);
            return NULL;
        }
    }

    lv_obj_add_event_cb(root, _analog_event_cb, LV_EVENT_ALL, NULL);
    dial->timer = lv_timer_create(_analog_timer_cb, defaults.smooth_second ? 16U : 500U, dial);
    if (!dial->timer)
    {
        lv_obj_delete(root);
        eos_free(dial);
        return NULL;
    }

    eos_wdata_set(root, EOS_WDATA_WW_ANALOG_DIAL, dial, _analog_destroy);
    _analog_refresh_ext_draw_size(dial);
    _analog_layout_hands(dial);
    lv_timer_ready(dial->timer);
    _analog_update(dial);
    return root;
}

eos_ww_analog_dial_hand_handle_t *eos_ww_analog_dial_get_hand(lv_obj_t *root, eos_ww_analog_hand_index_t index)
{
    _analog_dial_t *dial;
    _analog_hand_t *hand;

    if (!root || !lv_obj_is_valid(root))
        return NULL;
    dial = eos_wdata_get(root, EOS_WDATA_WW_ANALOG_DIAL);
    hand = _analog_hand_from_index(dial, index);
    return hand ? &hand->handle : NULL;
}

bool eos_ww_analog_dial_hand_set_enabled(eos_ww_analog_dial_hand_handle_t *handle, bool enabled)
{
    _analog_hand_t *hand = _analog_hand_from_handle(handle);
    if (!hand)
        return false;
    hand->style.enabled = enabled;
    _analog_set_hand_hidden(hand);
    lv_obj_invalidate(hand->handle.owner->root);
    return true;
}

bool eos_ww_analog_dial_hand_set_type(eos_ww_analog_dial_hand_handle_t *handle, eos_ww_analog_hand_type_t type)
{
    _analog_hand_t *hand = _analog_hand_from_handle(handle);
    if (!hand || type > EOS_WW_ANALOG_HAND_IMAGE)
        return false;
    if (hand->object && lv_obj_is_valid(hand->object))
    {
        if (hand->style.type == EOS_WW_ANALOG_HAND_IMAGE)
            lv_image_set_src(hand->object, NULL);
        lv_obj_delete(hand->object);
        hand->object = NULL;
    }
    if (hand->style.type == EOS_WW_ANALOG_HAND_IMAGE)
        _analog_release_image_source(hand);
    hand->style.type = type;
    if ((type == EOS_WW_ANALOG_HAND_IMAGE ? !_analog_ensure_image(hand) : !_analog_ensure_primitive(hand)))
        return false;
    if (type == EOS_WW_ANALOG_HAND_IMAGE)
        _analog_layout_image(hand);
    else
        _analog_layout_primitive(hand);
    _analog_refresh_ext_draw_size(hand->handle.owner);
    lv_obj_invalidate(hand->handle.owner->root);
    return true;
}

bool eos_ww_analog_dial_hand_set_color(eos_ww_analog_dial_hand_handle_t *handle, uint32_t color)
{
    _analog_hand_t *hand = _analog_hand_from_handle(handle);
    if (!hand)
        return false;
    hand->style.color = lv_color_hex(color);
    lv_obj_invalidate(hand->handle.owner->root);
    return true;
}

bool eos_ww_analog_dial_hand_set_opacity(eos_ww_analog_dial_hand_handle_t *handle, uint8_t opacity)
{
    _analog_hand_t *hand = _analog_hand_from_handle(handle);
    if (!hand)
        return false;
    hand->style.opacity = opacity;
    if (hand->style.type == EOS_WW_ANALOG_HAND_IMAGE && hand->object && lv_obj_is_valid(hand->object))
        lv_obj_set_style_image_opa(hand->object, opacity, LV_PART_MAIN);
    lv_obj_invalidate(hand->handle.owner->root);
    return true;
}

bool eos_ww_analog_dial_hand_set_length(eos_ww_analog_dial_hand_handle_t *handle, lv_coord_t length)
{
    _analog_hand_t *hand = _analog_hand_from_handle(handle);
    if (!hand)
        return false;
    hand->style.length = LV_MAX(0, length);
    if (hand->style.type != EOS_WW_ANALOG_HAND_IMAGE)
        _analog_layout_primitive(hand);
    _analog_refresh_ext_draw_size(hand->handle.owner);
    lv_obj_invalidate(hand->handle.owner->root);
    return true;
}

bool eos_ww_analog_dial_hand_set_width(eos_ww_analog_dial_hand_handle_t *handle, lv_coord_t width)
{
    _analog_hand_t *hand = _analog_hand_from_handle(handle);
    if (!hand)
        return false;
    hand->style.width = LV_MAX(0, width);
    if (hand->style.type != EOS_WW_ANALOG_HAND_IMAGE)
        _analog_layout_primitive(hand);
    _analog_refresh_ext_draw_size(hand->handle.owner);
    lv_obj_invalidate(hand->handle.owner->root);
    return true;
}

bool eos_ww_analog_dial_hand_set_tail_length(eos_ww_analog_dial_hand_handle_t *handle, lv_coord_t tail_length)
{
    _analog_hand_t *hand = _analog_hand_from_handle(handle);
    if (!hand)
        return false;
    hand->style.tail_length = LV_MAX(0, tail_length);
    if (hand->style.type != EOS_WW_ANALOG_HAND_IMAGE)
        _analog_layout_primitive(hand);
    _analog_refresh_ext_draw_size(hand->handle.owner);
    lv_obj_invalidate(hand->handle.owner->root);
    return true;
}

bool eos_ww_analog_dial_hand_set_angle_offset(eos_ww_analog_dial_hand_handle_t *handle, int32_t angle_offset)
{
    _analog_hand_t *hand = _analog_hand_from_handle(handle);
    if (!hand)
        return false;
    hand->style.angle_offset = angle_offset;
    _analog_update(hand->handle.owner);
    return true;
}

bool eos_ww_analog_dial_hand_set_offset(eos_ww_analog_dial_hand_handle_t *handle,
                                        lv_coord_t offset_x,
                                        lv_coord_t offset_y)
{
    _analog_hand_t *hand = _analog_hand_from_handle(handle);
    if (!hand)
        return false;
    hand->style.offset_x = offset_x;
    hand->style.offset_y = offset_y;
    if (hand->style.type == EOS_WW_ANALOG_HAND_IMAGE)
        _analog_layout_image(hand);
    else
        _analog_layout_primitive(hand);
    _analog_refresh_ext_draw_size(hand->handle.owner);
    lv_obj_invalidate(hand->handle.owner->root);
    return true;
}

bool eos_ww_analog_dial_hand_set_pivot(eos_ww_analog_dial_hand_handle_t *handle, float x, float y)
{
    _analog_hand_t *hand = _analog_hand_from_handle(handle);
    if (!hand || !isfinite(x) || !isfinite(y) || x < 0.0f || x > 1.0f || y < 0.0f || y > 1.0f)
        return false;
    hand->style.pivot_x = x;
    hand->style.pivot_y = y;
    hand->style.pivot_set = true;
    if (hand->style.type == EOS_WW_ANALOG_HAND_IMAGE)
        _analog_layout_image(hand);
    _analog_refresh_ext_draw_size(hand->handle.owner);
    lv_obj_invalidate(hand->handle.owner->root);
    return true;
}

bool eos_ww_analog_dial_hand_set_image(eos_ww_analog_dial_hand_handle_t *handle, const char *src)
{
    _analog_hand_t *hand = _analog_hand_from_handle(handle);
    size_t length = src ? strlen(src) : 0U;
    if (!hand || length >= EOS_WW_ANALOG_DIAL_IMAGE_SRC_MAX)
        return false;
    if (hand->style.type != EOS_WW_ANALOG_HAND_IMAGE
        && !eos_ww_analog_dial_hand_set_type(handle, EOS_WW_ANALOG_HAND_IMAGE))
        return false;
    if (src)
        memcpy(hand->style.image_src, src, length + 1U);
    else
        hand->style.image_src[0] = '\0';
    if (!_analog_set_image_source(hand))
    {
        _analog_layout_image(hand);
        return false;
    }
    _analog_layout_image(hand);
    _analog_refresh_ext_draw_size(hand->handle.owner);
    lv_obj_invalidate(hand->handle.owner->root);
    return true;
}
