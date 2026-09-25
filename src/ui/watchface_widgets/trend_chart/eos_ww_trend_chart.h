/**
 * @file eos_ww_trend_chart.h
 * @brief Rolling Metric trend chart
 */

#ifndef EOS_WW_TREND_CHART_H
#define EOS_WW_TREND_CHART_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include "eos_service_metric.h"
#include "lvgl.h"

/* Public typedefs --------------------------------------------*/
typedef struct
{
    lv_coord_t width;
    lv_coord_t height;
    uint16_t capacity;
    uint32_t sample_interval;
    bool auto_scale;
    bool metric_bound;
    eos_metric_id_t metric_id;
    int32_t min_value;
    int32_t max_value;
} eos_ww_trend_chart_config_t;

/* Public function prototypes ---------------------------------*/
lv_obj_t *eos_ww_trend_chart_create(lv_obj_t *parent, const eos_ww_trend_chart_config_t *config);
void eos_ww_trend_chart_append(lv_obj_t *chart, double value, bool valid);
bool eos_ww_trend_chart_set_data(lv_obj_t *chart, const double *values, uint16_t count);
void eos_ww_trend_chart_clear(lv_obj_t *chart);

#ifdef __cplusplus
}
#endif

#endif /* EOS_WW_TREND_CHART_H */
