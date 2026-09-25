/**
 * @file eos_ww_calendar_grid.h
 * @brief Monthly calendar grid watchface widget
 */

#ifndef EOS_WW_CALENDAR_GRID_H
#define EOS_WW_CALENDAR_GRID_H

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
    uint16_t year;
    uint8_t month;
    bool show_header;
    bool show_weekdays;
    bool highlight_today;
    bool week_starts_monday;
} eos_ww_calendar_grid_config_t;

/* Public function prototypes ---------------------------------*/
lv_obj_t *eos_ww_calendar_grid_create(lv_obj_t *parent, const eos_ww_calendar_grid_config_t *config);
void eos_ww_calendar_grid_set_month(lv_obj_t *calendar, uint16_t year, uint8_t month);
void eos_ww_calendar_grid_set_selected_date(lv_obj_t *calendar, uint16_t year, uint8_t month, uint8_t day);

#ifdef __cplusplus
}
#endif

#endif /* EOS_WW_CALENDAR_GRID_H */
