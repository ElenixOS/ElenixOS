/**
 * @file eos_ww_compass_dial.h
 * @brief Complete compass dial watchface widget
 */

#ifndef EOS_WW_COMPASS_DIAL_H
#define EOS_WW_COMPASS_DIAL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include "lvgl.h"

/* Public typedefs --------------------------------------------*/
typedef struct
{
    lv_coord_t width;
    lv_coord_t height;
    uint16_t tick_step;
    uint16_t major_tick_step;
    bool cardinal_labels;
    bool degree_labels;
    bool show_heading;
    bool show_calibration_state;
    float smoothing;
} eos_ww_compass_dial_config_t;

/* Public function prototypes ---------------------------------*/
lv_obj_t *eos_ww_compass_dial_create(lv_obj_t *parent, const eos_ww_compass_dial_config_t *config);

#ifdef __cplusplus
}
#endif

#endif /* EOS_WW_COMPASS_DIAL_H */
