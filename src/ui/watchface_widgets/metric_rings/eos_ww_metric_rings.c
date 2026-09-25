/**
 * @file eos_ww_metric_rings.c
 * @brief Configurable multi-Metric concentric rings
 */

#include "eos_ww_metric_rings.h"

/* Includes ---------------------------------------------------*/
#include "eos_mem.h"
#include "eos_widget_data.h"
#include "eos_ww_internal.h"
#define EOS_LOG_TAG "MetricRings"
#include "eos_log.h"

/* Variables --------------------------------------------------*/
typedef struct
{
    lv_obj_t *root;
    lv_obj_t *arc[EOS_WW_METRIC_RINGS_MAX];
    eos_ww_metric_ring_spec_t spec[EOS_WW_METRIC_RINGS_MAX];
    lv_timer_t *timer;
    uint8_t count;
    bool animate;
} _metric_rings_t;

/* Function Implementations -----------------------------------*/

static void _metric_rings_exec(void *var, int32_t value)
{
    if (var)
        lv_arc_set_value(var, value);
}

static void _metric_rings_set_arc(_metric_rings_t *rings, uint8_t index, double value, bool valid)
{
    double range;
    int32_t target;
    lv_anim_t animation;

    if (!rings || index >= rings->count || !rings->arc[index])
        return;
    if (!valid)
        value = rings->spec[index].min;
    if (value < rings->spec[index].min)
        value = rings->spec[index].min;
    if (value > rings->spec[index].max)
        value = rings->spec[index].max;
    range = rings->spec[index].max - rings->spec[index].min;
    target = range > 0.0 ? (int32_t)((value - rings->spec[index].min) * 100.0 / range) : 0;
    if (!rings->animate)
    {
        lv_arc_set_value(rings->arc[index], target);
        return;
    }
    lv_anim_delete(rings->arc[index], _metric_rings_exec);
    lv_anim_init(&animation);
    lv_anim_set_var(&animation, rings->arc[index]);
    lv_anim_set_values(&animation, lv_arc_get_value(rings->arc[index]), target);
    lv_anim_set_duration(&animation, 250U);
    lv_anim_set_exec_cb(&animation, _metric_rings_exec);
    lv_anim_start(&animation);
}

static void _metric_rings_timer_cb(lv_timer_t *timer)
{
    _metric_rings_t *rings = lv_timer_get_user_data(timer);
    for (uint8_t i = 0; rings && i < rings->count; i++)
    {
        eos_metric_value_t sample;
        if (rings->spec[i].metric_bound && eos_metric_read(rings->spec[i].metric_id, &sample) == EOS_OK)
            _metric_rings_set_arc(rings, i, sample.value, sample.valid && !sample.stale);
    }
}

static void _metric_rings_destroy(void *data)
{
    _metric_rings_t *rings = data;
    if (!rings)
        return;
    if (rings->timer)
        lv_timer_delete(rings->timer);
    for (uint8_t i = 0; i < rings->count; i++)
    {
        if (rings->arc[i])
            lv_anim_delete(rings->arc[i], _metric_rings_exec);
    }
    eos_free(rings);
}

lv_obj_t *eos_ww_metric_rings_create(lv_obj_t *parent, const eos_ww_metric_rings_config_t *config)
{
    eos_ww_metric_rings_config_t defaults = {
        .size = 120,
        .ring_width = 6,
        .gap = 4,
        .track_color = 0x1A2030,
        .animate = true,
        .ring_count = 0,
    };
    _metric_rings_t *rings;
    lv_obj_t *root;

    EOS_CHECK_PTR_RETURN_VAL(parent, NULL);
    if (config)
        defaults = *config;
    if (defaults.size <= 0 || defaults.ring_count == 0U || defaults.ring_count > EOS_WW_METRIC_RINGS_MAX)
        return NULL;
    rings = eos_malloc_zeroed(sizeof(*rings));
    EOS_CHECK_PTR_RETURN_VAL(rings, NULL);
    root = lv_obj_create(parent);
    if (!root)
    {
        eos_free(rings);
        return NULL;
    }
    lv_obj_set_size(root, defaults.size, defaults.size);
    eos_ww_internal_make_container(root);
    rings->root = root;
    rings->count = defaults.ring_count;
    rings->animate = defaults.animate;
    for (uint8_t i = 0; i < rings->count; i++)
    {
        lv_coord_t ring_size = defaults.size - (lv_coord_t)i * 2 * (defaults.ring_width + defaults.gap);
        lv_obj_t *arc = lv_arc_create(root);
        if (!arc || ring_size <= 0)
            continue;
        rings->arc[i] = arc;
        rings->spec[i] = defaults.rings[i];
        lv_obj_set_size(arc, ring_size, ring_size);
        lv_obj_center(arc);
        lv_arc_set_range(arc, 0, 100);
        lv_arc_set_value(arc, 0);
        lv_arc_set_rotation(arc, 0);
        lv_arc_set_bg_angles(arc, 0, 360);
        lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
        lv_obj_set_style_arc_width(arc, defaults.ring_width, LV_PART_MAIN);
        lv_obj_set_style_arc_width(arc, defaults.ring_width, LV_PART_INDICATOR);
        lv_obj_set_style_arc_color(arc, lv_color_hex(defaults.track_color), LV_PART_MAIN);
        lv_obj_set_style_arc_color(arc, lv_color_hex(defaults.rings[i].color), LV_PART_INDICATOR);
        eos_ww_internal_make_static(arc);
    }
    eos_wdata_set(root, EOS_WDATA_WW_METRIC_RINGS, rings, _metric_rings_destroy);
    for (uint8_t i = 0; i < rings->count; i++)
    {
        if (rings->spec[i].metric_bound)
        {
            eos_metric_value_t sample;
            if (eos_metric_read(rings->spec[i].metric_id, &sample) == EOS_OK)
                _metric_rings_set_arc(rings, i, sample.value, sample.valid && !sample.stale);
        }
    }
    rings->timer = lv_timer_create(_metric_rings_timer_cb, 500U, rings);
    if (!rings->timer)
    {
        lv_obj_delete(root);
        return NULL;
    }
    lv_timer_ready(rings->timer);
    return root;
}

void eos_ww_metric_rings_set_value(lv_obj_t *rings_obj, uint8_t index, double value, bool valid)
{
    _metric_rings_t *rings;

    if (!rings_obj || !lv_obj_is_valid(rings_obj))
        return;
    rings = eos_wdata_get(rings_obj, EOS_WDATA_WW_METRIC_RINGS);
    if (rings)
        _metric_rings_set_arc(rings, index, value, valid);
}
