/**
 * @file eos_ww_trend_chart.c
 * @brief Rolling Metric trend chart
 */

#include "eos_ww_trend_chart.h"

/* Includes ---------------------------------------------------*/
#include <limits.h>
#include "eos_mem.h"
#include "eos_widget_data.h"
#include "eos_ww_internal.h"
#define EOS_LOG_TAG "TrendChart"
#include "eos_log.h"

/* Variables --------------------------------------------------*/
typedef struct
{
    lv_obj_t *root;
    lv_obj_t *chart;
    lv_chart_series_t *series;
    lv_timer_t *timer;
    int32_t *values;
    uint16_t capacity;
    bool auto_scale;
    bool metric_bound;
    eos_metric_id_t metric_id;
    int32_t min_value;
    int32_t max_value;
} _trend_chart_t;

/* Function Implementations -----------------------------------*/

static void _trend_chart_update_range(_trend_chart_t *chart)
{
    int32_t min_value = INT32_MAX;
    int32_t max_value = INT32_MIN;
    bool has_value = false;

    if (!chart || !chart->auto_scale)
        return;
    for (uint16_t i = 0; i < chart->capacity; i++)
    {
        if (chart->values[i] == LV_CHART_POINT_NONE)
            continue;
        if (chart->values[i] < min_value)
            min_value = chart->values[i];
        if (chart->values[i] > max_value)
            max_value = chart->values[i];
        has_value = true;
    }
    if (!has_value)
        return;
    if (min_value == max_value)
    {
        min_value--;
        max_value++;
    }
    chart->min_value = min_value;
    chart->max_value = max_value;
    lv_chart_set_axis_range(chart->chart, LV_CHART_AXIS_PRIMARY_Y, min_value, max_value);
}

static void _trend_chart_render(_trend_chart_t *chart)
{
    if (!chart || !chart->chart || !chart->series)
        return;
    _trend_chart_update_range(chart);
    lv_chart_set_series_values(chart->chart, chart->series, chart->values, chart->capacity);
    lv_chart_refresh(chart->chart);
}

static void _trend_chart_timer_cb(lv_timer_t *timer)
{
    _trend_chart_t *chart = lv_timer_get_user_data(timer);
    eos_metric_value_t sample;

    if (!chart || !chart->metric_bound)
        return;
    if (eos_metric_read(chart->metric_id, &sample) == EOS_OK)
        eos_ww_trend_chart_append(chart->root, sample.value, sample.valid && !sample.stale);
}

static void _trend_chart_destroy(void *data)
{
    _trend_chart_t *chart = data;
    if (!chart)
        return;
    if (chart->timer)
        lv_timer_delete(chart->timer);
    eos_free(chart->values);
    eos_free(chart);
}

lv_obj_t *eos_ww_trend_chart_create(lv_obj_t *parent, const eos_ww_trend_chart_config_t *config)
{
    eos_ww_trend_chart_config_t defaults = {
        .width = 180,
        .height = 60,
        .capacity = 60,
        .sample_interval = 1000,
        .auto_scale = true,
        .metric_bound = false,
        .metric_id = EOS_METRIC_HEART_RATE,
        .min_value = 0,
        .max_value = 100,
    };
    _trend_chart_t *chart;
    lv_obj_t *root;

    EOS_CHECK_PTR_RETURN_VAL(parent, NULL);
    if (config)
        defaults = *config;
    if (defaults.width <= 0 || defaults.height <= 0 || defaults.capacity == 0U || defaults.capacity > 512U
        || defaults.sample_interval < 100U)
        return NULL;
    chart = eos_malloc_zeroed(sizeof(*chart));
    EOS_CHECK_PTR_RETURN_VAL(chart, NULL);
    chart->values = eos_malloc(sizeof(int32_t) * defaults.capacity);
    if (!chart->values)
    {
        eos_free(chart);
        return NULL;
    }
    root = lv_obj_create(parent);
    if (!root)
    {
        eos_free(chart->values);
        eos_free(chart);
        return NULL;
    }
    lv_obj_set_size(root, defaults.width, defaults.height);
    eos_ww_internal_make_container(root);
    chart->root = root;
    chart->capacity = defaults.capacity;
    chart->auto_scale = defaults.auto_scale;
    chart->metric_bound = defaults.metric_bound;
    chart->metric_id = defaults.metric_id;
    chart->min_value = defaults.min_value;
    chart->max_value = defaults.max_value;
    for (uint16_t i = 0; i < chart->capacity; i++)
        chart->values[i] = LV_CHART_POINT_NONE;

    chart->chart = lv_chart_create(root);
    if (!chart->chart)
    {
        lv_obj_delete(root);
        eos_free(chart->values);
        eos_free(chart);
        return NULL;
    }
    lv_obj_set_size(chart->chart, defaults.width, defaults.height);
    lv_obj_set_style_bg_color(chart->chart, lv_color_hex(0x0F1824), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(chart->chart, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(chart->chart, 1, LV_PART_MAIN);
    lv_obj_set_style_border_color(chart->chart, lv_color_hex(0x273142), LV_PART_MAIN);
    lv_obj_set_style_line_color(chart->chart, lv_color_hex(0x273142), LV_PART_MAIN);
    lv_obj_set_style_line_width(chart->chart, 1, LV_PART_MAIN);
    lv_obj_set_style_line_color(chart->chart, lv_color_hex(0x4CD964), LV_PART_ITEMS);
    lv_obj_set_style_line_width(chart->chart, 3, LV_PART_ITEMS);
    lv_obj_set_style_width(chart->chart, 4, LV_PART_INDICATOR);
    lv_obj_set_style_height(chart->chart, 4, LV_PART_INDICATOR);
    lv_chart_set_type(chart->chart, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(chart->chart, chart->capacity);
    lv_chart_set_update_mode(chart->chart, LV_CHART_UPDATE_MODE_SHIFT);
    lv_chart_set_div_line_count(chart->chart, 3, 6);
    lv_chart_set_axis_range(chart->chart, LV_CHART_AXIS_PRIMARY_Y, chart->min_value, chart->max_value);
    chart->series = lv_chart_add_series(chart->chart, lv_color_hex(0x4CD964), LV_CHART_AXIS_PRIMARY_Y);
    eos_ww_internal_make_static(chart->chart);
    if (!chart->series)
    {
        lv_obj_delete(root);
        eos_free(chart->values);
        eos_free(chart);
        return NULL;
    }
    _trend_chart_render(chart);
    eos_wdata_set(root, EOS_WDATA_WW_TREND_CHART, chart, _trend_chart_destroy);
    if (defaults.metric_bound)
    {
        chart->timer = lv_timer_create(_trend_chart_timer_cb, defaults.sample_interval, chart);
        if (!chart->timer)
        {
            lv_obj_delete(root);
            return NULL;
        }
        lv_timer_ready(chart->timer);
    }
    return root;
}

void eos_ww_trend_chart_append(lv_obj_t *chart_obj, double value, bool valid)
{
    _trend_chart_t *chart;

    if (!chart_obj || !lv_obj_is_valid(chart_obj))
        return;
    chart = eos_wdata_get(chart_obj, EOS_WDATA_WW_TREND_CHART);
    if (!chart)
        return;
    lv_chart_set_next_value(chart->chart, chart->series, valid ? (int32_t)value : LV_CHART_POINT_NONE);
    for (uint16_t i = 0; i + 1U < chart->capacity; i++)
        chart->values[i] = chart->values[i + 1U];
    chart->values[chart->capacity - 1U] = valid ? (int32_t)value : LV_CHART_POINT_NONE;
    _trend_chart_update_range(chart);
    lv_chart_refresh(chart->chart);
}

bool eos_ww_trend_chart_set_data(lv_obj_t *chart_obj, const double *values, uint16_t count)
{
    _trend_chart_t *chart;

    if (!chart_obj || !lv_obj_is_valid(chart_obj) || !values)
        return false;
    chart = eos_wdata_get(chart_obj, EOS_WDATA_WW_TREND_CHART);
    if (!chart || count > chart->capacity)
        return false;
    for (uint16_t i = 0; i < chart->capacity; i++)
        chart->values[i] = i < count ? (int32_t)values[i] : LV_CHART_POINT_NONE;
    _trend_chart_render(chart);
    return true;
}

void eos_ww_trend_chart_clear(lv_obj_t *chart_obj)
{
    _trend_chart_t *chart;

    if (!chart_obj || !lv_obj_is_valid(chart_obj))
        return;
    chart = eos_wdata_get(chart_obj, EOS_WDATA_WW_TREND_CHART);
    if (!chart)
        return;
    for (uint16_t i = 0; i < chart->capacity; i++)
        chart->values[i] = LV_CHART_POINT_NONE;
    _trend_chart_render(chart);
}
