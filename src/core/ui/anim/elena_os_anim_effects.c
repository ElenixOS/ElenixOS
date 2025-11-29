/**
 * @file elena_os_anim_effects.c
 * @brief 特殊动画
 * @author Sab1e
 * @date 2025-11-28
 */

#include "elena_os_anim_effects.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#define EOS_LOG_TAG "AnimationEffects"
#include "elena_os_log.h"
#include "elena_os_theme.h"
#include "elena_os_port.h"
#include "elena_os_cqueue.h"
#include "elena_os_event.h"
#include "elena_os_nav.h"
#include "elena_os_anim.h"
#include "elena_os_basic_widgets.h"
/* Macros and Definitions -------------------------------------*/
#define _ANIM_LIST_DURATION EOS_SCREEN_SWITCH_DURATION

typedef enum
{
    _SCREEN_LOAD,
    _SCREEN_DELETE,
} eos_screen_state_t;

typedef struct
{
    lv_obj_t *list;
    lv_obj_t *selected_obj;
} eos_anim_list_data_t;

/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

static void _play_anim_list(lv_obj_t *list, lv_obj_t *selected_obj, bool is_anim_entering)
{
    EOS_CHECK_PTR_RETURN(list && selected_obj);
    if (!(lv_obj_is_valid(list) && lv_obj_is_valid(selected_obj)))
        return;
    for (int i = 0; i < lv_obj_get_child_count(list); i++)
    {
        lv_obj_t *obj = lv_obj_get_child(list, i);
        int32_t w, h;
        w = lv_obj_get_width(obj);
        h = lv_obj_get_height(obj);
        if (obj == selected_obj)
        {
            // 选中项动画
            int32_t x, y;
            x = lv_obj_get_x(obj);
            y = lv_obj_get_y(obj);

            if (is_anim_entering)
            {
                eos_anim_move_start(obj, x, y, x - w, y, _ANIM_LIST_DURATION, false);
            }
            else
            {
                eos_anim_move_start(obj, x, y, 0, y, _ANIM_LIST_DURATION, false);
            }
        }
        if (is_anim_entering)
        {
            eos_anim_scale_start(obj, w, (w * 100) / 120, h, (h * 100) / 120, _ANIM_LIST_DURATION, false); // 避免浮点计算
            eos_anim_fade_start(obj, LV_OPA_COVER, LV_OPA_TRANSP, _ANIM_LIST_DURATION, false);
        }
        else
        {
            eos_anim_scale_start(obj, w, (w * 120) / 100, h, (h * 120) / 100, _ANIM_LIST_DURATION, false); // 避免浮点计算
            eos_anim_fade_start(obj, LV_OPA_TRANSP, LV_OPA_COVER, _ANIM_LIST_DURATION, false);
        }
    }
}

static void _anim_list_screen_load_start_cb(lv_event_t *e)
{
    EOS_LOG_D("Screen load start | State: %d", eos_nav_get_state());
    if (eos_nav_get_state() != EOS_NAV_STATE_BACK_PREV_SCREEN)
        return;
    eos_anim_list_data_t *d = lv_event_get_user_data(e);
    EOS_CHECK_PTR_RETURN(d);
    // 触发离开动画
    _play_anim_list(d->list, d->selected_obj, false);
}

static void _anim_list_screen_unloaded_cb(lv_event_t *e)
{
    EOS_LOG_D("Screen unload start | State: %d", eos_nav_get_state());
    if (eos_nav_get_state() != EOS_NAV_STATE_ENTER_NEXT_SCREEN)
        return;
    eos_anim_list_data_t *d = lv_event_get_user_data(e);
    EOS_CHECK_PTR_RETURN(d);
    // 触发进入动画
    if (eos_nav_get_state() == EOS_NAV_STATE_CLEANING_UP)
        return;
    _play_anim_list(d->list, d->selected_obj, true);
}

static void _anim_list_screen_delete_cb(lv_event_t *e)
{
    eos_anim_list_data_t *d = lv_event_get_user_data(e);
    EOS_CHECK_PTR_RETURN(d);
    eos_free(d);
    EOS_LOG_D("Screen freed");
}

void eos_anim_list_bind(lv_obj_t *screen, lv_obj_t *list, lv_obj_t *selected_obj)
{
    EOS_CHECK_PTR_RETURN(list && selected_obj);
    eos_anim_list_data_t *d = eos_malloc(sizeof(eos_anim_list_data_t));
    d->list = list;
    d->selected_obj = selected_obj;

    lv_event_dsc_t *dsc = lv_obj_get_event_dsc(screen, 0);
    if (dsc)
    {
        eos_anim_list_data_t *old_d = lv_event_dsc_get_user_data(dsc);
        if (old_d)
        {
            eos_free(old_d);
            EOS_LOG_D("Old data free");
        }
    }

    lv_obj_remove_event_cb(screen, _anim_list_screen_load_start_cb);
    lv_obj_remove_event_cb(screen, _anim_list_screen_unloaded_cb);
    lv_obj_remove_event_cb(screen, _anim_list_screen_delete_cb);

    lv_obj_add_event_cb(screen, _anim_list_screen_load_start_cb, LV_EVENT_SCREEN_LOAD_START, d);
    lv_obj_add_event_cb(screen, _anim_list_screen_unloaded_cb, LV_EVENT_SCREEN_UNLOAD_START, d);
    lv_obj_add_event_cb(screen, _anim_list_screen_delete_cb, LV_EVENT_DELETE, d);
}
