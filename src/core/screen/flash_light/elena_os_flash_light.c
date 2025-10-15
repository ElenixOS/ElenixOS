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
#include "elena_os_log.h"
#include "elena_os_event.h"

// Macros and Definitions

// Variables

// Function Implementations

static void _swipe_panel_pull_back_cb(lv_event_t *e)
{
    EOS_LOG_D("SWIPE_PULL_BACK_CB");
    swipe_panel_t *sp = lv_event_get_user_data(e);
    EOS_CHECK_PTR_RETURN(sp);
    int32_t swipe_obj_coord_y = lv_obj_get_y(sp->swipe_obj);
    if (swipe_obj_coord_y >= EOS_DISPLAY_HEIGHT)
    {
        EOS_LOG_D("DEL_SWIPE");
        eos_swipe_panel_delete(sp);
    }
}

void eos_flash_light_show(void)
{
    swipe_panel_t *sp = eos_swipe_panel_create(lv_screen_active());
    eos_swipe_panel_set_dir(sp, EOS_SWIPE_DIR_UP);
    eos_swipe_panel_move(sp, 0, true);
    eos_swipe_panel_hide_handle_bar(sp);
    lv_obj_set_style_bg_opa(sp->swipe_obj, LV_OPA_TRANSP, 0);
    lv_obj_t *row1 = lv_obj_create(sp->swipe_obj);
    eos_event_add_cb(
        sp->swipe_obj,
        _swipe_panel_pull_back_cb,
        eos_event_get_code(EOS_EVENT_SWIPE_PANEL_PULL_BACK),
        (void *)sp);
}

void eos_flash_light_create()
{
}
