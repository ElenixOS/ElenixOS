/**
 * @file eos_ww_calendar_grid.c
 * @brief Monthly calendar grid watchface widget
 */

#include "eos_ww_calendar_grid.h"

/* Includes ---------------------------------------------------*/
#include "eos_mem.h"
#include "eos_service_time.h"
#include "eos_widget_data.h"
#include "eos_ww_internal.h"
#define EOS_LOG_TAG "CalendarGrid"
#include "eos_log.h"

/* Variables --------------------------------------------------*/
typedef struct
{
    lv_obj_t *calendar;
    lv_obj_t *header;
    lv_calendar_date_t selected;
    bool has_selected;
} _calendar_grid_t;

/* Function Implementations -----------------------------------*/

static void _calendar_grid_destroy(void *data)
{
    eos_free(data);
}

static void _calendar_grid_set_day_names(lv_obj_t *calendar, bool monday_first)
{
    static const char *const sunday_first[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    static const char *const monday_first_names[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
    lv_calendar_set_day_names(calendar, (const char **)(monday_first ? monday_first_names : sunday_first));
}

lv_obj_t *eos_ww_calendar_grid_create(lv_obj_t *parent, const eos_ww_calendar_grid_config_t *config)
{
    eos_ww_calendar_grid_config_t defaults = {
        .width = 0,
        .height = 0,
        .year = 0,
        .month = 0,
        .show_header = true,
        .show_weekdays = true,
        .highlight_today = true,
        .week_starts_monday = false,
    };
    _calendar_grid_t *grid;
    eos_datetime_t now;
    lv_obj_t *calendar;

    EOS_CHECK_PTR_RETURN_VAL(parent, NULL);
    if (config)
        defaults = *config;
    now = eos_time_get();
    if (defaults.year == 0U)
        defaults.year = (uint16_t)now.year;
    if (defaults.month == 0U || defaults.month > 12U)
        defaults.month = now.month;

    grid = eos_malloc_zeroed(sizeof(*grid));
    EOS_CHECK_PTR_RETURN_VAL(grid, NULL);
    calendar = lv_calendar_create(parent);
    if (!calendar)
    {
        eos_free(grid);
        return NULL;
    }
    if (defaults.width > 0 && defaults.height > 0)
        lv_obj_set_size(calendar, defaults.width, defaults.height);
    else
        lv_obj_set_size(calendar, LV_PCT(100), LV_PCT(100));
    lv_obj_t *button_matrix = lv_calendar_get_btnmatrix(calendar);
    if (button_matrix)
    {
        lv_obj_set_style_text_font(button_matrix, &lv_font_montserrat_14, LV_PART_MAIN);
        lv_obj_set_style_text_line_space(button_matrix, 0, LV_PART_MAIN);
    }
    lv_calendar_set_month_shown(calendar, defaults.year, defaults.month);
    lv_calendar_set_today_date(calendar, now.year, now.month, now.day);
    if (defaults.show_weekdays)
        _calendar_grid_set_day_names(calendar, defaults.week_starts_monday);
    eos_ww_internal_make_static(calendar);
    if (defaults.show_header)
        grid->header = lv_calendar_add_header_arrow(calendar);
    grid->calendar = calendar;
    eos_wdata_set(calendar, EOS_WDATA_WW_CALENDAR_GRID, grid, _calendar_grid_destroy);
    return calendar;
}

void eos_ww_calendar_grid_set_month(lv_obj_t *calendar, uint16_t year, uint8_t month)
{
    if (!calendar || !lv_obj_is_valid(calendar) || month == 0U || month > 12U)
        return;
    lv_calendar_set_month_shown(calendar, year, month);
}

void eos_ww_calendar_grid_set_selected_date(lv_obj_t *calendar, uint16_t year, uint8_t month, uint8_t day)
{
    _calendar_grid_t *grid;

    if (!calendar || !lv_obj_is_valid(calendar) || month == 0U || month > 12U || day == 0U || day > 31U)
        return;
    grid = eos_wdata_get(calendar, EOS_WDATA_WW_CALENDAR_GRID);
    if (!grid)
        return;
    grid->selected.year = year;
    grid->selected.month = month;
    grid->selected.day = day;
    grid->has_selected = true;
    lv_calendar_set_highlighted_dates(calendar, &grid->selected, 1U);
}
