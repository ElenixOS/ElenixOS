/**
 * @file eos_ww_radial_gauge.c
 * @brief Configurable radial gauge with scale and needle
 */

#include "eos_ww_radial_gauge.h"

/* Includes ---------------------------------------------------*/
#include <inttypes.h>
#include <stdio.h>
#include "eos_font.h"
#include "eos_mem.h"
#include "eos_widget_data.h"
#include "eos_ww_internal.h"
#define EOS_LOG_TAG "RadialGauge"
#include "eos_log.h"

/* Variables --------------------------------------------------*/
typedef struct
{
    lv_obj_t *root;
    lv_obj_t *scale;
    lv_obj_t *needle;
    lv_obj_t *label;
    lv_timer_t *timer;
    double min;
    double max;
    double value;
    bool valid;
    bool metric_bound;
    eos_metric_id_t metric_id;
    lv_style_t styles[EOS_WW_RADIAL_GAUGE_MAX_RANGES];
    bool style_initialized[EOS_WW_RADIAL_GAUGE_MAX_RANGES];
} _radial_gauge_t;

/* Function Implementations -----------------------------------*/

static void _radial_gauge_render(_radial_gauge_t *gauge)
{
    int32_t value;
    char text[32];

    if (!gauge || !gauge->scale)
        return;
    if (!gauge->valid)
    {
        if (gauge->label)
            lv_label_set_text(gauge->label, "--");
        return;
    }
    value = (int32_t)gauge->value;
    if (gauge->needle)
        lv_scale_set_line_needle_value(gauge->scale, gauge->needle, -1, value);
    if (gauge->label)
    {
        snprintf(text, sizeof(text), "%.0f", gauge->value);
        lv_label_set_text(gauge->label, text);
    }
}

static void _radial_gauge_timer_cb(lv_timer_t *timer)
{
    _radial_gauge_t *gauge = lv_timer_get_user_data(timer);
    eos_metric_value_t sample;

    if (!gauge || !gauge->metric_bound)
        return;
    if (eos_metric_read(gauge->metric_id, &sample) == EOS_OK)
        eos_ww_radial_gauge_set_value(gauge->root, sample.value, sample.valid && !sample.stale);
}

static void _radial_gauge_destroy(void *data)
{
    _radial_gauge_t *gauge = data;
    if (!gauge)
        return;
    if (gauge->timer)
        lv_timer_delete(gauge->timer);
    for (uint8_t i = 0; i < EOS_WW_RADIAL_GAUGE_MAX_RANGES; i++)
    {
        if (gauge->style_initialized[i])
            lv_style_reset(&gauge->styles[i]);
    }
    eos_free(gauge);
}

static lv_coord_t _radial_gauge_max_label_extent(const eos_ww_radial_gauge_config_t *config, const lv_font_t *font)
{
    lv_coord_t max_extent = 0;
    lv_point_t size;
    char text[32];
    int32_t min_value;
    int32_t max_value;
    int32_t step;

    if (!config || !font)
        return 0;
    min_value = (int32_t)config->min;
    max_value = (int32_t)config->max;
    step = (int32_t)config->major_step;
    for (int32_t value = min_value; value <= max_value; value += step)
    {
        snprintf(text, sizeof(text), "%" PRId32, value);
        lv_text_get_size(&size, text, font, 0, 0, LV_COORD_MAX, LV_TEXT_FLAG_NONE);
        max_extent = LV_MAX(max_extent, LV_MAX(size.x, size.y));
        if (value > max_value - step)
            break;
    }
    return max_extent;
}

lv_obj_t *eos_ww_radial_gauge_create(lv_obj_t *parent, const eos_ww_radial_gauge_config_t *config)
{
    _radial_gauge_t *gauge;
    lv_obj_t *root;
    lv_obj_t *scale;
    lv_coord_t scale_size;
    lv_coord_t scale_radius;
    lv_coord_t content_radius;
    lv_coord_t major_length;
    lv_coord_t minor_length;
    lv_coord_t label_gap;
    lv_coord_t label_extent;
    eos_ww_radial_gauge_config_t defaults = {
        .width = 180,
        .height = 180,
        .min = 0.0,
        .max = 100.0,
        .start_angle = 135,
        .angle_range = 270,
        .major_step = 10,
        .minor_step = 2,
        .needle = true,
        .value_label = true,
        .metric_bound = false,
        .metric_id = EOS_METRIC_BATTERY_PERCENT,
    };

    EOS_CHECK_PTR_RETURN_VAL(parent, NULL);
    if (config)
        defaults = *config;
    if (defaults.width <= 0 || defaults.height <= 0 || defaults.max <= defaults.min || defaults.major_step == 0U
        || defaults.minor_step == 0U)
        return NULL;
    eos_ww_internal_fit_size(parent, &defaults.width, &defaults.height);
    if (defaults.width <= 0 || defaults.height <= 0)
        return NULL;

    gauge = eos_malloc_zeroed(sizeof(*gauge));
    EOS_CHECK_PTR_RETURN_VAL(gauge, NULL);
    root = lv_obj_create(parent);
    if (!root)
    {
        eos_free(gauge);
        return NULL;
    }
    lv_obj_set_size(root, defaults.width, defaults.height);
    eos_ww_internal_make_container(root);
    lv_obj_update_layout(root);
    scale = lv_scale_create(root);
    if (!scale)
    {
        lv_obj_delete(root);
        eos_free(gauge);
        return NULL;
    }
    content_radius = LV_MIN(lv_obj_get_content_width(root), lv_obj_get_content_height(root)) / 2;
    major_length = LV_MAX(4, content_radius / 8);
    minor_length = LV_MAX(2, major_length / 2);
    label_gap = LV_MAX(4, content_radius / 16);
    lv_obj_set_style_text_font(scale, &lv_font_montserrat_14, LV_PART_INDICATOR);
    label_extent = _radial_gauge_max_label_extent(&defaults, lv_obj_get_style_text_font(scale, LV_PART_INDICATOR));
    scale_radius = content_radius - 2 - major_length - label_gap - label_extent / 2;
    if (scale_radius <= 0)
    {
        lv_obj_delete(root);
        eos_free(gauge);
        return NULL;
    }
    scale_size = scale_radius * 2;
    lv_obj_set_size(scale, scale_size, scale_size);
    lv_obj_center(scale);
    lv_scale_set_mode(scale, LV_SCALE_MODE_ROUND_OUTER);
    lv_scale_set_range(scale, (int32_t)defaults.min, (int32_t)defaults.max);
    lv_scale_set_total_tick_count(scale, (uint32_t)((defaults.max - defaults.min) / defaults.minor_step) + 1U);
    lv_scale_set_major_tick_every(scale, defaults.major_step / defaults.minor_step);
    lv_scale_set_label_show(scale, true);
    lv_scale_set_rotation(scale, defaults.start_angle);
    lv_scale_set_angle_range(scale, defaults.angle_range);
    lv_obj_set_style_length(scale, major_length, LV_PART_INDICATOR);
    lv_obj_set_style_length(scale, minor_length, LV_PART_ITEMS);
    lv_obj_set_style_pad_radial(scale, label_gap - 15, LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(scale, LV_OPA_TRANSP, 0);
    lv_obj_set_style_line_color(scale, lv_color_hex(0xD4DAE4), LV_PART_MAIN);
    lv_obj_set_style_line_width(scale, 2, LV_PART_MAIN);
    lv_obj_set_style_line_color(scale, lv_color_hex(0x7C899B), LV_PART_INDICATOR);
    lv_obj_set_style_line_width(scale, 3, LV_PART_INDICATOR);
    eos_ww_internal_make_static(scale);

    if (defaults.needle)
    {
        gauge->needle = lv_line_create(scale);
    }
    if (gauge->needle)
    {
        lv_obj_set_style_line_color(gauge->needle, lv_color_hex(0xFF3B30), 0);
        lv_obj_set_style_line_width(gauge->needle, 3, 0);
        lv_obj_set_style_line_rounded(gauge->needle, true, 0);
        eos_ww_internal_make_static(gauge->needle);
    }
    if (defaults.value_label)
    {
        gauge->label = lv_label_create(root);
        lv_obj_center(gauge->label);
        eos_label_set_font_size(gauge->label, EOS_FONT_SIZE_SMALL);
        eos_ww_internal_make_static(gauge->label);
    }

    gauge->root = root;
    gauge->scale = scale;
    gauge->min = defaults.min;
    gauge->max = defaults.max;
    gauge->metric_bound = defaults.metric_bound;
    gauge->metric_id = defaults.metric_id;
    for (uint8_t i = 0; i < defaults.range_count && i < EOS_WW_RADIAL_GAUGE_MAX_RANGES; i++)
    {
        lv_scale_section_t *section = lv_scale_add_section(scale);
        if (!section)
            continue;
        lv_scale_set_section_range(scale, section, (int32_t)defaults.ranges[i].from, (int32_t)defaults.ranges[i].to);
        lv_style_init(&gauge->styles[i]);
        gauge->style_initialized[i] = true;
        lv_style_set_line_color(&gauge->styles[i], lv_color_hex(defaults.ranges[i].color));
        lv_style_set_line_width(&gauge->styles[i], 3);
        lv_scale_set_section_style_indicator(scale, section, &gauge->styles[i]);
    }
    eos_wdata_set(root, EOS_WDATA_WW_RADIAL_GAUGE, gauge, _radial_gauge_destroy);
    if (defaults.metric_bound)
    {
        gauge->timer = lv_timer_create(_radial_gauge_timer_cb, 500U, gauge);
        if (!gauge->timer)
        {
            lv_obj_delete(root);
            return NULL;
        }
        lv_timer_ready(gauge->timer);
    }
    eos_ww_radial_gauge_set_value(root, defaults.min, false);
    return root;
}

void eos_ww_radial_gauge_set_value(lv_obj_t *gauge_obj, double value, bool valid)
{
    _radial_gauge_t *gauge;

    if (!gauge_obj || !lv_obj_is_valid(gauge_obj))
        return;
    gauge = eos_wdata_get(gauge_obj, EOS_WDATA_WW_RADIAL_GAUGE);
    if (!gauge)
        return;
    if (value < gauge->min)
        value = gauge->min;
    if (value > gauge->max)
        value = gauge->max;
    gauge->value = value;
    gauge->valid = valid;
    _radial_gauge_render(gauge);
}
