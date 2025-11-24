/**
 * @file elena_os_crown.c
 * @brief 表冠
 * @author Sab1e
 * @date 2025-11-13
 */

#include "elena_os_crown.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "elena_os_scene.h"
#include "elena_os_dispatcher.h"
#define EOS_LOG_DISABLE
#define EOS_LOG_TAG "Crown"
#include "elena_os_log.h"
#include "elena_os_pm.h"
/* Macros and Definitions -------------------------------------*/
#define _CROWN_ENCODER_SCROLL_COEFFICIENT 50
/* Variables --------------------------------------------------*/
static lv_obj_t *scrollable_obj = NULL;
static int8_t encoder_reverse = -1;
/* Function Implementations -----------------------------------*/
static void _crown_button_async_cb(void *user_data)
{
    if (eos_pm_get_state() == EOS_PM_SLEEP)
    {
        eos_pm_wake_up();
        return;
    }
    eos_pm_reset_timer();
    eos_button_state_t state = (eos_button_state_t)(intptr_t)user_data;
    switch (state)
    {
    case EOS_BUTTON_STATE_CLICKED:
        eos_scene_auto_switch();
    default:
        break;
    }
}

static void _crown_encoder_async_cb(void *user_data)
{
    eos_crown_encoder_diff_t diff = (eos_crown_encoder_diff_t)(intptr_t)user_data;
    if (scrollable_obj && lv_obj_is_valid(scrollable_obj) && lv_obj_has_class(scrollable_obj, &lv_obj_class))
    {
        int32_t dy = diff * encoder_reverse * _CROWN_ENCODER_SCROLL_COEFFICIENT;
        lv_obj_scroll_by_bounded(scrollable_obj, 0, dy, LV_ANIM_ON);
    }
}

void eos_crown_encoder_set_target_obj(lv_obj_t *obj)
{
    scrollable_obj = obj;
}

void eos_crown_encoder_set_target_screen(lv_obj_t *screen)
{
    lv_obj_t *list = lv_obj_get_child_by_type(screen, 0, &lv_list_class);
    EOS_LOG_D("List=%p", list);
    if (list)
        scrollable_obj = list;
}

void eos_crown_encoder_set_reverse(bool reverse)
{
    if (reverse)
    {
        encoder_reverse = 1;
    }
    else
    {
        encoder_reverse = -1;
    }
}

void eos_crown_encoder_report(eos_crown_encoder_diff_t diff)
{
    eos_dispatcher_call(_crown_encoder_async_cb, (void *)(intptr_t)diff);
}

void eos_crown_button_report(eos_button_state_t state)
{
    eos_dispatcher_call(_crown_button_async_cb, (void *)(intptr_t)state);
}
