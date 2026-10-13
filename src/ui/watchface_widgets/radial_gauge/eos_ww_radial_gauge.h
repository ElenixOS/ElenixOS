/**
 * @file eos_ww_radial_gauge.h
 * @brief Configurable radial gauge with scale and needle
 */

#ifndef EOS_WW_RADIAL_GAUGE_H
#define EOS_WW_RADIAL_GAUGE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include "eos_service_metric.h"
#include "lvgl.h"

/* Public typedefs --------------------------------------------*/
#define EOS_WW_RADIAL_GAUGE_MAX_RANGES 8U

typedef struct
{
    double from;
    double to;
    uint32_t color;
} eos_ww_radial_gauge_range_t;

typedef struct
{
    lv_coord_t width;
    lv_coord_t height;
    double min;
    double max;
    int32_t start_angle;
    uint32_t angle_range;
    uint32_t major_step;
    uint32_t minor_step;
    bool needle;
    bool value_label;
    bool metric_bound;
    eos_metric_id_t metric_id;
    uint8_t range_count;
    eos_ww_radial_gauge_range_t ranges[EOS_WW_RADIAL_GAUGE_MAX_RANGES];
} eos_ww_radial_gauge_config_t;

/* Public function prototypes ---------------------------------*/
lv_obj_t *eos_ww_radial_gauge_create(lv_obj_t *parent, const eos_ww_radial_gauge_config_t *config);
void eos_ww_radial_gauge_set_value(lv_obj_t *gauge, double value, bool valid);

#ifdef __cplusplus
}
#endif

#endif /* EOS_WW_RADIAL_GAUGE_H */
