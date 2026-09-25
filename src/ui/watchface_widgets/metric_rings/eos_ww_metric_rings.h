/**
 * @file eos_ww_metric_rings.h
 * @brief Configurable multi-Metric concentric rings
 */

#ifndef EOS_WW_METRIC_RINGS_H
#define EOS_WW_METRIC_RINGS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include "eos_service_metric.h"
#include "lvgl.h"

/* Public typedefs --------------------------------------------*/
#define EOS_WW_METRIC_RINGS_MAX 8U

typedef struct
{
    double min;
    double max;
    uint32_t color;
    bool metric_bound;
    eos_metric_id_t metric_id;
} eos_ww_metric_ring_spec_t;

typedef struct
{
    lv_coord_t size;
    lv_coord_t ring_width;
    lv_coord_t gap;
    uint32_t track_color;
    bool animate;
    uint8_t ring_count;
    eos_ww_metric_ring_spec_t rings[EOS_WW_METRIC_RINGS_MAX];
} eos_ww_metric_rings_config_t;

/* Public function prototypes ---------------------------------*/
lv_obj_t *eos_ww_metric_rings_create(lv_obj_t *parent, const eos_ww_metric_rings_config_t *config);
void eos_ww_metric_rings_set_value(lv_obj_t *rings, uint8_t index, double value, bool valid);

#ifdef __cplusplus
}
#endif

#endif /* EOS_WW_METRIC_RINGS_H */
