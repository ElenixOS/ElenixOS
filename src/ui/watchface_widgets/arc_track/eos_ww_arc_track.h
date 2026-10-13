/**
 * @file eos_ww_arc_track.h
 * @brief Reusable LVGL arc-segment and marker renderer
 */

#ifndef EOS_WW_ARC_TRACK_H
#define EOS_WW_ARC_TRACK_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include "lvgl.h"

/* Public macros ----------------------------------------------*/
#define EOS_WW_ARC_TRACK_MAX_SEGMENTS 8U
#define EOS_WW_ARC_TRACK_MAX_MARKERS 16U

/* Public typedefs --------------------------------------------*/
typedef struct
{
    lv_coord_t width;
    lv_coord_t height;
    lv_coord_t center_x;
    lv_coord_t center_y;
    lv_coord_t radius;
    lv_coord_t line_width;
    lv_coord_t marker_size;
    lv_coord_t knob_size;
    lv_coord_t knob_border_width;
    int32_t start_angle;
    int32_t end_angle;
    int32_t marker_start_angle;
    int32_t marker_end_angle;
    int32_t segment_gap_angle;
    uint32_t color;
    uint32_t knob_border_color;
    uint32_t segment_colors[EOS_WW_ARC_TRACK_MAX_SEGMENTS];
    uint32_t marker_colors[EOS_WW_ARC_TRACK_MAX_MARKERS];
    uint8_t segment_count;
    uint8_t marker_count;
    uint8_t marker_color_count;
    bool rounded;
} eos_ww_arc_track_config_t;

/* Public function prototypes ---------------------------------*/
lv_obj_t *eos_ww_arc_track_create(lv_obj_t *parent, const eos_ww_arc_track_config_t *config);

/**
 * @brief Set the value range and move the ArcTrack's hollow knob.
 * @param arc_track ArcTrack object returned by eos_ww_arc_track_create.
 * @param min Minimum value represented by the start angle.
 * @param value Current value represented by the knob position.
 * @param max Maximum value represented by the end angle.
 * @param valid Whether the supplied data is current and valid.
 * @return true if the object is an ArcTrack with its range knob enabled.
 */
bool eos_ww_arc_track_set_range(lv_obj_t *arc_track, double min, double value, double max, bool valid);

#ifdef __cplusplus
}
#endif

#endif /* EOS_WW_ARC_TRACK_H */
