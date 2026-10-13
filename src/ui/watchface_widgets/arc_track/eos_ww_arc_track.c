/**
 * @file eos_ww_arc_track.c
 * @brief Reusable LVGL arc-segment and marker renderer
 */

#include "eos_ww_arc_track.h"

/* Includes ---------------------------------------------------*/
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include "eos_mem.h"
#include "eos_widget_data.h"
#include "eos_ww_internal.h"
#define EOS_LOG_TAG "ArcTrack"
#include "eos_log.h"

/* Macros and Definitions -------------------------------------*/
#define _ARC_TRACK_MAX_SWEEP 360.0
#define _ARC_TRACK_MAX_GRADIENT_BYTES (128U * 1024U)
#define _ARC_TRACK_GRADIENT_AA_MARGIN 1.0

/* Variables --------------------------------------------------*/
typedef struct
{
    eos_ww_arc_track_config_t config;
    lv_image_dsc_t gradient_image;
    lv_color32_t *gradient_pixels;
    lv_area_t gradient_area;
    double range_min;
    double range_value;
    double range_max;
    bool range_valid;
} _arc_track_t;

/* Function Implementations -----------------------------------*/

static double _arc_track_angle_delta(double angle, double start_angle)
{
    /* LVGL uses 0 degrees at 3 o'clock; positive angles follow screen-clockwise. */
    double delta = fmod(angle - start_angle, 360.0);

    if (delta < 0.0)
        delta += 360.0;
    return delta;
}

static bool _arc_track_area_from_center(int64_t center_x, int64_t center_y, lv_coord_t size, lv_area_t *area)
{
    int64_t half;
    int64_t x1;
    int64_t y1;
    int64_t x2;
    int64_t y2;

    if (!area || size <= 0)
        return false;

    half = (int64_t)size / 2;
    x1 = center_x - half;
    y1 = center_y - half;
    x2 = x1 + (int64_t)size - 1;
    y2 = y1 + (int64_t)size - 1;
    if (x1 < INT32_MIN || y1 < INT32_MIN || x2 > INT32_MAX || y2 > INT32_MAX)
        return false;

    area->x1 = (int32_t)x1;
    area->y1 = (int32_t)y1;
    area->x2 = (int32_t)x2;
    area->y2 = (int32_t)y2;
    return true;
}

static bool _arc_track_translate_area(const lv_area_t *source, int32_t x, int32_t y, lv_area_t *destination)
{
    int64_t x1;
    int64_t y1;
    int64_t x2;
    int64_t y2;

    if (!source || !destination)
        return false;

    x1 = (int64_t)source->x1 + x;
    y1 = (int64_t)source->y1 + y;
    x2 = (int64_t)source->x2 + x;
    y2 = (int64_t)source->y2 + y;
    if (x1 < INT32_MIN || y1 < INT32_MIN || x2 > INT32_MAX || y2 > INT32_MAX)
        return false;

    destination->x1 = (int32_t)x1;
    destination->y1 = (int32_t)y1;
    destination->x2 = (int32_t)x2;
    destination->y2 = (int32_t)y2;
    return true;
}

static double _arc_track_range_progress(double min, double value, double max)
{
    double scale;
    double scaled_min;
    double scaled_value;
    double scaled_max;
    double span;
    double progress;

    if (value <= min)
        return 0.0;
    if (value >= max)
        return 1.0;

    /* Scaling first keeps both differences finite even for [-DBL_MAX, DBL_MAX]. */
    scale = fmax(fabs(min), fabs(max));
    if (scale == 0.0)
        return 0.0;
    scaled_min = min / scale;
    scaled_value = value / scale;
    scaled_max = max / scale;
    span = scaled_max - scaled_min;
    if (!(span > 0.0))
        return 0.5;
    progress = (scaled_value - scaled_min) / span;
    return fmax(0.0, fmin(progress, 1.0));
}

static void _arc_track_add_bound(const eos_ww_arc_track_config_t *config,
                                 double centerline_radius,
                                 double angle,
                                 double *min_x,
                                 double *min_y,
                                 double *max_x,
                                 double *max_y)
{
    double radians = angle * M_PI / 180.0;
    double x = (double)config->center_x + cos(radians) * centerline_radius;
    double y = (double)config->center_y + sin(radians) * centerline_radius;

    if (x < *min_x)
        *min_x = x;
    if (x > *max_x)
        *max_x = x;
    if (y < *min_y)
        *min_y = y;
    if (y > *max_y)
        *max_y = y;
}

static lv_color32_t _arc_track_gradient_color(const eos_ww_arc_track_config_t *config, double position, uint8_t alpha)
{
    double stop = position * (double)(config->segment_count - 1U);
    uint8_t first = (uint8_t)floor(stop);
    uint8_t second;
    double mix;
    uint32_t color_a;
    uint32_t color_b;
    uint8_t red;
    uint8_t green;
    uint8_t blue;

    if (first >= config->segment_count - 1U)
    {
        first = config->segment_count - 1U;
        second = first;
        mix = 0.0;
    }
    else
    {
        second = first + 1U;
        mix = stop - (double)first;
    }
    color_a = config->segment_colors[first];
    color_b = config->segment_colors[second];
    red = (uint8_t)lround((double)((color_a >> 16) & 0xFFU) * (1.0 - mix) + (double)((color_b >> 16) & 0xFFU) * mix);
    green = (uint8_t)lround((double)((color_a >> 8) & 0xFFU) * (1.0 - mix) + (double)((color_b >> 8) & 0xFFU) * mix);
    blue = (uint8_t)lround((double)(color_a & 0xFFU) * (1.0 - mix) + (double)(color_b & 0xFFU) * mix);
    return lv_color32_make(red, green, blue, alpha);
}

static bool _arc_track_build_gradient_cache(_arc_track_t *track)
{
    const eos_ww_arc_track_config_t *config = &track->config;
    double sweep = (double)config->end_angle - (double)config->start_angle;
    double centerline_radius = fmax(0.0, (double)config->radius - (double)config->line_width / 2.0);
    double min_x = HUGE_VAL;
    double min_y = HUGE_VAL;
    double max_x = -HUGE_VAL;
    double max_y = -HUGE_VAL;
    double margin = (double)config->line_width / 2.0 + _ARC_TRACK_GRADIENT_AA_MARGIN;
    int32_t x1;
    int32_t y1;
    int32_t x2;
    int32_t y2;
    uint32_t width;
    uint32_t height;
    size_t data_size;
    double start_radians = (double)config->start_angle * M_PI / 180.0;
    double end_radians = (double)config->end_angle * M_PI / 180.0;
    double start_x = (double)config->center_x + cos(start_radians) * centerline_radius;
    double start_y = (double)config->center_y + sin(start_radians) * centerline_radius;
    double end_x = (double)config->center_x + cos(end_radians) * centerline_radius;
    double end_y = (double)config->center_y + sin(end_radians) * centerline_radius;

    _arc_track_add_bound(config, centerline_radius, (double)config->start_angle, &min_x, &min_y, &max_x, &max_y);
    _arc_track_add_bound(config, centerline_radius, (double)config->end_angle, &min_x, &min_y, &max_x, &max_y);
    for (int32_t cardinal = 0; cardinal < 360; cardinal += 90)
    {
        if (_arc_track_angle_delta((double)cardinal, (double)config->start_angle) <= sweep)
            _arc_track_add_bound(config, centerline_radius, (double)cardinal, &min_x, &min_y, &max_x, &max_y);
    }

    min_x = fmax(0.0, floor(min_x - margin));
    min_y = fmax(0.0, floor(min_y - margin));
    max_x = fmin((double)config->width - 1.0, ceil(max_x + margin));
    max_y = fmin((double)config->height - 1.0, ceil(max_y + margin));
    if (min_x > max_x || min_y > max_y)
        return false;

    x1 = (int32_t)min_x;
    y1 = (int32_t)min_y;
    x2 = (int32_t)max_x;
    y2 = (int32_t)max_y;
    width = (uint32_t)(x2 - x1 + 1);
    height = (uint32_t)(y2 - y1 + 1);
    if (width > UINT16_MAX || height > UINT16_MAX || width > UINT16_MAX / sizeof(lv_color32_t))
        return false;
    if ((size_t)width > SIZE_MAX / (size_t)height / sizeof(lv_color32_t))
        return false;
    data_size = (size_t)width * (size_t)height * sizeof(lv_color32_t);
    if (data_size > _ARC_TRACK_MAX_GRADIENT_BYTES)
    {
        EOS_LOG_E("Gradient cache too large: %u bytes", (unsigned)data_size);
        return false;
    }

    track->gradient_pixels = eos_malloc(data_size);
    EOS_CHECK_PTR_RETURN_VAL(track->gradient_pixels, false);
    track->gradient_area.x1 = x1;
    track->gradient_area.y1 = y1;
    track->gradient_area.x2 = x2;
    track->gradient_area.y2 = y2;
    track->gradient_image.header.magic = LV_IMAGE_HEADER_MAGIC;
    track->gradient_image.header.cf = LV_COLOR_FORMAT_ARGB8888;
    track->gradient_image.header.flags = 0;
    track->gradient_image.header.w = (uint16_t)width;
    track->gradient_image.header.h = (uint16_t)height;
    track->gradient_image.header.stride = (uint16_t)(width * sizeof(lv_color32_t));
    track->gradient_image.data_size = (uint32_t)data_size;
    track->gradient_image.data = (const uint8_t *)track->gradient_pixels;

    for (uint32_t y = 0; y < height; y++)
    {
        for (uint32_t x = 0; x < width; x++)
        {
            double px = (double)(x1 + (int32_t)x);
            double py = (double)(y1 + (int32_t)y);
            double dx = px - (double)config->center_x;
            double dy = py - (double)config->center_y;
            double pixel_radius = hypot(dx, dy);
            double angle = atan2(dy, dx) * 180.0 / M_PI;
            double delta = _arc_track_angle_delta(angle, (double)config->start_angle);
            double coverage;
            double position;

            if (delta <= sweep)
            {
                coverage = (double)config->line_width / 2.0 + 0.5 - fabs(pixel_radius - centerline_radius);
                position = delta / sweep;
            }
            else if (!config->rounded)
            {
                coverage = 0.0;
                position = 0.0;
            }
            else
            {
                double start_distance = hypot(px - start_x, py - start_y);
                double end_distance = hypot(px - end_x, py - end_y);
                if (start_distance <= end_distance)
                {
                    coverage = (double)config->line_width / 2.0 + 0.5 - start_distance;
                    position = 0.0;
                }
                else
                {
                    coverage = (double)config->line_width / 2.0 + 0.5 - end_distance;
                    position = 1.0;
                }
            }

            if (coverage <= 0.0)
                track->gradient_pixels[(size_t)y * width + x] = lv_color32_make(0, 0, 0, 0);
            else
            {
                uint8_t alpha = (uint8_t)lround(fmin(coverage, 1.0) * 255.0);
                track->gradient_pixels[(size_t)y * width + x] = _arc_track_gradient_color(config, position, alpha);
            }
        }
    }

    return true;
}

static void _arc_track_draw_cap(lv_layer_t *layer, const _arc_track_t *track, double angle, uint32_t color)
{
    lv_draw_rect_dsc_t dsc;
    lv_area_t area;
    double radians = angle * M_PI / 180.0;
    int64_t x = (int64_t)track->config.center_x + (int64_t)lround(cos(radians) * track->config.radius);
    int64_t y = (int64_t)track->config.center_y + (int64_t)lround(sin(radians) * track->config.radius);

    if (!_arc_track_area_from_center(x, y, track->config.line_width, &area))
        return;

    lv_draw_rect_dsc_init(&dsc);
    dsc.base.layer = layer;
    dsc.bg_color = lv_color_hex(color);
    dsc.bg_opa = LV_OPA_COVER;
    dsc.radius = LV_RADIUS_CIRCLE;
    lv_draw_rect(layer, &dsc, &area);
}

static void _arc_track_draw_piece(lv_layer_t *layer,
                                  const _arc_track_t *track,
                                  double start_angle,
                                  double end_angle,
                                  uint32_t color,
                                  bool rounded)
{
    lv_draw_arc_dsc_t dsc;

    if (end_angle <= start_angle)
        return;
    lv_draw_arc_dsc_init(&dsc);
    dsc.base.layer = layer;
    dsc.center.x = track->config.center_x;
    dsc.center.y = track->config.center_y;
    dsc.radius = (uint16_t)track->config.radius;
    dsc.width = track->config.line_width;
    dsc.start_angle = (lv_value_precise_t)start_angle;
    dsc.end_angle = (lv_value_precise_t)end_angle;
    dsc.color = lv_color_hex(color);
    dsc.opa = LV_OPA_COVER;
    dsc.rounded = rounded;
    lv_draw_arc(layer, &dsc);
}

static void _arc_track_draw_segment(lv_layer_t *layer,
                                    const _arc_track_t *track,
                                    double start_angle,
                                    double end_angle,
                                    uint32_t color)
{
    double sweep = end_angle - start_angle;
    double normalized_start = fmod(start_angle, 360.0);

    if (normalized_start < 0.0)
        normalized_start += 360.0;
    if (sweep <= 0.0 || sweep > _ARC_TRACK_MAX_SWEEP)
        return;

    if (sweep >= _ARC_TRACK_MAX_SWEEP)
    {
        _arc_track_draw_piece(layer, track, normalized_start, normalized_start + 360.0, color, false);
        return;
    }

    double normalized_end = normalized_start + sweep;
    if (normalized_end <= 360.0)
    {
        _arc_track_draw_piece(layer, track, normalized_start, normalized_end, color, track->config.rounded);
        return;
    }

    _arc_track_draw_piece(layer, track, normalized_start, 360.0, color, false);
    _arc_track_draw_piece(layer, track, 0.0, normalized_end - 360.0, color, false);
    if (track->config.rounded)
    {
        _arc_track_draw_cap(layer, track, normalized_start, color);
        _arc_track_draw_cap(layer, track, normalized_end, color);
    }
}

static void _arc_track_draw_markers(lv_layer_t *layer, const _arc_track_t *track)
{
    const eos_ww_arc_track_config_t *config = &track->config;

    if (config->marker_count == 0U || config->marker_size <= 0 || config->marker_color_count == 0U)
        return;

    for (uint8_t i = 0; i < config->marker_count; i++)
    {
        double ratio = config->marker_count > 1U ? (double)i / (double)(config->marker_count - 1U) : 0.5;
        double angle = (double)config->marker_start_angle
                       + ((double)config->marker_end_angle - (double)config->marker_start_angle) * ratio;
        double radians = angle * M_PI / 180.0;
        int64_t x = (int64_t)config->center_x + (int64_t)lround(cos(radians) * config->radius);
        int64_t y = (int64_t)config->center_y + (int64_t)lround(sin(radians) * config->radius);
        lv_area_t area;
        lv_draw_rect_dsc_t dsc;

        if (!_arc_track_area_from_center(x, y, config->marker_size, &area))
            continue;

        lv_draw_rect_dsc_init(&dsc);
        dsc.base.layer = layer;
        dsc.bg_color = lv_color_hex(config->marker_colors[i % config->marker_color_count]);
        dsc.bg_opa = LV_OPA_COVER;
        dsc.radius = LV_RADIUS_CIRCLE;
        lv_draw_rect(layer, &dsc, &area);
    }
}

static void _arc_track_draw_range_knob(lv_layer_t *layer, const _arc_track_t *track)
{
    const eos_ww_arc_track_config_t *config = &track->config;
    double progress;
    double angle;
    double radians;
    double centerline_radius;
    double center_x;
    double center_y;
    lv_area_t area;
    lv_draw_rect_dsc_t dsc;

    if (!track->range_valid || config->knob_size <= 0 || config->knob_border_width <= 0)
        return;

    progress = _arc_track_range_progress(track->range_min, track->range_value, track->range_max);
    angle = (double)config->start_angle + progress * ((double)config->end_angle - (double)config->start_angle);
    radians = angle * M_PI / 180.0;
    centerline_radius = fmax(0.0, (double)config->radius - (double)config->line_width / 2.0);
    center_x = (double)config->center_x + cos(radians) * centerline_radius;
    center_y = (double)config->center_y + sin(radians) * centerline_radius;

    {
        double rounded_x1 = center_x - (double)config->knob_size / 2.0;
        double rounded_y1 = center_y - (double)config->knob_size / 2.0;
        int64_t x1;
        int64_t y1;
        int64_t x2;
        int64_t y2;

        if (!isfinite(rounded_x1) || !isfinite(rounded_y1) || rounded_x1 < (double)INT32_MIN
            || rounded_x1 > (double)INT32_MAX || rounded_y1 < (double)INT32_MIN || rounded_y1 > (double)INT32_MAX)
            return;
        x1 = (int64_t)lround(rounded_x1);
        y1 = (int64_t)lround(rounded_y1);
        x2 = x1 + (int64_t)config->knob_size - 1;
        y2 = y1 + (int64_t)config->knob_size - 1;
        if (x2 > INT32_MAX || y2 > INT32_MAX)
            return;
        area.x1 = (int32_t)x1;
        area.y1 = (int32_t)y1;
        area.x2 = (int32_t)x2;
        area.y2 = (int32_t)y2;
    }

    lv_draw_rect_dsc_init(&dsc);
    dsc.base.layer = layer;
    dsc.bg_color = lv_color_black();
    dsc.bg_opa = LV_OPA_TRANSP;
    dsc.border_color = lv_color_hex(config->knob_border_color);
    dsc.border_opa = LV_OPA_COVER;
    dsc.border_width = config->knob_border_width;
    dsc.border_side = LV_BORDER_SIDE_FULL;
    dsc.radius = LV_RADIUS_CIRCLE;
    lv_draw_rect(layer, &dsc, &area);
}

static void _arc_track_draw(lv_obj_t *obj, _arc_track_t *track, lv_layer_t *layer)
{
    _arc_track_t positioned_track = *track;
    eos_ww_arc_track_config_t *config = &positioned_track.config;
    lv_area_t object_area;

    lv_obj_get_coords(obj, &object_area);
    {
        int64_t center_x = (int64_t)config->center_x + object_area.x1;
        int64_t center_y = (int64_t)config->center_y + object_area.y1;
        if (center_x < INT32_MIN || center_x > INT32_MAX || center_y < INT32_MIN || center_y > INT32_MAX)
            return;
        config->center_x = (lv_coord_t)center_x;
        config->center_y = (lv_coord_t)center_y;
    }

    if (track->gradient_pixels)
    {
        lv_draw_image_dsc_t image_dsc;
        lv_area_t image_area;
        if (!_arc_track_translate_area(&track->gradient_area, object_area.x1, object_area.y1, &image_area))
        {
            _arc_track_draw_markers(layer, &positioned_track);
            _arc_track_draw_range_knob(layer, &positioned_track);
            return;
        }
        lv_draw_image_dsc_init(&image_dsc);
        image_dsc.src = &track->gradient_image;
        image_dsc.image_area = image_area;
        lv_draw_image(layer, &image_dsc, &image_area);
    }
    else if (config->segment_count == 0U)
    {
        _arc_track_draw_segment(layer, &positioned_track, config->start_angle, config->end_angle, config->color);
    }
    else
    {
        double sweep = (double)config->end_angle - (double)config->start_angle;
        double total_gaps = (double)config->segment_gap_angle * (double)(config->segment_count - 1U);
        double segment_sweep = (sweep - total_gaps) / (double)config->segment_count;
        double cursor = config->start_angle;

        for (uint8_t i = 0; i < config->segment_count; i++)
        {
            _arc_track_draw_segment(layer,
                                    &positioned_track,
                                    cursor,
                                    cursor + segment_sweep,
                                    config->segment_colors[i]);
            cursor += segment_sweep + config->segment_gap_angle;
        }
    }
    _arc_track_draw_markers(layer, &positioned_track);
    _arc_track_draw_range_knob(layer, &positioned_track);
}

static void _arc_track_event_cb(lv_event_t *event)
{
    if (lv_event_get_code(event) != LV_EVENT_DRAW_MAIN)
        return;

    lv_obj_t *obj = lv_event_get_current_target(event);
    _arc_track_t *track = eos_wdata_get(obj, EOS_WDATA_WW_ARC_TRACK);
    if (track)
        _arc_track_draw(obj, track, lv_event_get_layer(event));
}

static void _arc_track_destroy(void *data)
{
    _arc_track_t *track = data;
    if (!track)
        return;
    if (track->gradient_pixels)
        eos_free(track->gradient_pixels);
    eos_free(track);
}

lv_obj_t *eos_ww_arc_track_create(lv_obj_t *parent, const eos_ww_arc_track_config_t *config)
{
    _arc_track_t *track;
    lv_obj_t *root;
    int64_t sweep;
    int64_t total_gaps;

    EOS_CHECK_PTR_RETURN_VAL(parent, NULL);
    EOS_CHECK_PTR_RETURN_VAL(config, NULL);
    sweep = (int64_t)config->end_angle - (int64_t)config->start_angle;
    total_gaps = (int64_t)config->segment_gap_angle * ((int64_t)config->segment_count - 1);
    if (config->width <= 0 || config->width > LV_COORD_MAX || config->height <= 0 || config->height > LV_COORD_MAX
        || config->center_x < LV_COORD_MIN || config->center_x > LV_COORD_MAX || config->center_y < LV_COORD_MIN
        || config->center_y > LV_COORD_MAX || config->radius <= 0 || config->radius > UINT16_MAX
        || config->line_width <= 0 || config->line_width > LV_COORD_MAX
        || config->segment_count > EOS_WW_ARC_TRACK_MAX_SEGMENTS || config->knob_size < 0
        || config->knob_size > LV_COORD_MAX || config->knob_border_width < 0 || config->knob_border_width > LV_COORD_MAX
        || config->marker_size < 0 || config->marker_size > LV_COORD_MAX
        || (config->knob_size > 0
            && (config->knob_border_width <= 0 || config->knob_border_width > config->knob_size / 2))
        || config->marker_count > EOS_WW_ARC_TRACK_MAX_MARKERS
        || config->marker_color_count > EOS_WW_ARC_TRACK_MAX_MARKERS || sweep <= 0 || sweep > 360
        || config->segment_gap_angle < 0 || (config->segment_count > 0U && sweep <= total_gaps))
        return NULL;

    track = eos_malloc_zeroed(sizeof(*track));
    EOS_CHECK_PTR_RETURN_VAL(track, NULL);
    track->config = *config;
    if (config->segment_count > 1U && config->segment_gap_angle == 0 && !_arc_track_build_gradient_cache(track))
    {
        _arc_track_destroy(track);
        return NULL;
    }

    root = lv_obj_create(parent);
    if (!root)
    {
        _arc_track_destroy(track);
        return NULL;
    }
    lv_obj_set_size(root, config->width, config->height);
    eos_ww_internal_make_container(root);
    lv_obj_add_event_cb(root, _arc_track_event_cb, LV_EVENT_DRAW_MAIN, NULL);
    eos_wdata_set(root, EOS_WDATA_WW_ARC_TRACK, track, _arc_track_destroy);
    if (eos_wdata_get(root, EOS_WDATA_WW_ARC_TRACK) != track)
    {
        lv_obj_delete(root);
        _arc_track_destroy(track);
        return NULL;
    }
    return root;
}

bool eos_ww_arc_track_set_range(lv_obj_t *arc_track_obj, double min, double value, double max, bool valid)
{
    _arc_track_t *track;

    if (!arc_track_obj || !lv_obj_is_valid(arc_track_obj))
        return false;
    track = eos_wdata_get(arc_track_obj, EOS_WDATA_WW_ARC_TRACK);
    if (!track || track->config.knob_size <= 0 || track->config.knob_border_width <= 0)
        return false;

    track->range_min = min;
    track->range_max = max;
    track->range_valid = valid && isfinite(min) && isfinite(value) && isfinite(max) && max > min;
    if (track->range_valid)
        track->range_value = fmax(min, fmin(value, max));
    else
        track->range_value = 0.0;

    lv_obj_invalidate(arc_track_obj);
    return true;
}
