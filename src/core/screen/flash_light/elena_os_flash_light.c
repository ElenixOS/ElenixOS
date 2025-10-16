/**
 * @file elena_os_flash_light.c
 * @brief 手电筒
 * @author Sab1e
 * @date 2025-10-14
 */

#include "elena_os_flash_light.h"

// Includes
#include <stdio.h>
#include <stdlib.h>
#include "elena_os_theme.h"
#include "elena_os_config.h"
#include "elena_os_swipe_panel.h"
#define EOS_LOG_TAG "FlashLight"
#include "elena_os_log.h"
#include "elena_os_event.h"
#include "elena_os_icon.h"
#include "elena_os_anim.h"

// Macros and Definitions
#define MASK_OPA LV_OPA_80
typedef struct
{
    swipe_panel_t *sp;
    lv_obj_t *mask;
} _pressing_user_data_t;
// Variables

// Function Implementations

static void _swipe_panel_pull_back_cb(lv_event_t *e)
{
    EOS_LOG_I("PB");
    _pressing_user_data_t *ud = lv_event_get_user_data(e);
    swipe_panel_t *sp = lv_event_get_param(e);
    EOS_CHECK_PTR_RETURN(ud && sp);

    if (ud->sp == sp)
    {

        int32_t swipe_obj_coord_y = lv_obj_get_y(ud->sp->swipe_obj);
        if (swipe_obj_coord_y >= EOS_DISPLAY_HEIGHT)
        {
            EOS_LOG_I("Delete flash light");
            if (lv_obj_has_class(ud->mask, &lv_obj_class))
                lv_obj_delete(ud->mask);
            eos_swipe_panel_delete(ud->sp);
            free(ud);
        }
    }
}

static void _swipe_panel_moving_cb(lv_event_t *e)
{
    EOS_LOG_I("CB");
    _pressing_user_data_t *ud = lv_event_get_user_data(e);
    int32_t swipe_obj_coord_y = lv_obj_get_y(ud->sp->swipe_obj);
    float opa = MASK_OPA - ((float)swipe_obj_coord_y / (float)EOS_DISPLAY_HEIGHT * 3.0 * MASK_OPA);
    lv_obj_set_style_bg_opa(ud->mask, (lv_opa_t)opa, 0);
}

void eos_flash_light_show(void)
{
    _pressing_user_data_t *ud = malloc(sizeof(_pressing_user_data_t));
    EOS_CHECK_PTR_RETURN(ud);

    lv_obj_t *mask = lv_obj_create(lv_screen_active());
    lv_obj_remove_style_all(mask);
    lv_obj_set_size(mask, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(mask, EOS_COLOR_BLACK, 0);

    eos_anim_fade_start(mask, LV_OPA_TRANSP, MASK_OPA, 300);

    ud->mask = mask;

    swipe_panel_t *sp = eos_swipe_panel_create(lv_screen_active());
    eos_swipe_panel_set_dir(sp, EOS_SWIPE_DIR_UP);
    eos_swipe_panel_move(sp, 0, true);
    eos_swipe_panel_hide_handle_bar(sp);
    lv_obj_set_style_bg_opa(sp->swipe_obj, LV_OPA_TRANSP, 0);

    ud->sp = sp;

    eos_event_add_cb(
        sp->swipe_obj,
        _swipe_panel_pull_back_cb,
        eos_event_get_code(EOS_EVENT_SWIPE_PANEL_PULL_BACK),
        (void *)ud);
    eos_event_add_cb(
        sp->swipe_obj,
        _swipe_panel_moving_cb,
        eos_event_get_code(EOS_EVENT_SWIPE_PANEL_MOVING),
        (void *)ud);
    int32_t touch_area_height = EOS_DISPLAY_HEIGHT * 0.2;
    lv_obj_set_height(sp->touch_area, touch_area_height);

    lv_obj_t *container = sp->swipe_obj;
    lv_obj_set_style_bg_opa(container, LV_OPA_TRANSP, 0);

    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(container,
                          LV_FLEX_ALIGN_START,
                          LV_FLEX_ALIGN_START,
                          LV_FLEX_ALIGN_CENTER);
    lv_obj_remove_flag(container, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *row1 = lv_obj_create(container);
    lv_obj_remove_style_all(row1);
    lv_obj_set_size(row1, lv_pct(100), touch_area_height);

    lv_obj_t *label = lv_label_create(row1);
    lv_obj_set_height(label, LV_SIZE_CONTENT);
    lv_label_set_text(label, "Ignore\n" RI_ARROW_DOWN_WIDE_FILL);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, 0);

    lv_obj_t *flash_light = lv_obj_create(container);
    lv_obj_set_style_bg_color(flash_light, EOS_COLOR_WHITE, 0);
    lv_obj_set_size(flash_light, lv_pct(100), EOS_DISPLAY_HEIGHT);
    lv_obj_set_style_border_width(flash_light, 0, 0);
    lv_obj_set_style_radius(flash_light, EOS_DISPLAY_RADIUS, 0);
}

void eos_flash_light_create()
{
}
