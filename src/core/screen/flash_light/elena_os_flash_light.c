/**
 * @file elena_os_flash_light.c
 * @brief 手电筒
 * @author Sab1e
 * @date 2025-10-14
 */

#include "elena_os_flash_light.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "elena_os_theme.h"
#include "elena_os_config.h"
#include "elena_os_swipe_panel.h"
#define EOS_LOG_DISABLE
#define EOS_LOG_TAG "FlashLight"
#include "elena_os_log.h"
#include "elena_os_event.h"
#include "elena_os_icon.h"
#include "elena_os_anim.h"
#include "elena_os_utils.h"
#include "elena_os_card_pager.h"
#include "elena_os_watchface.h"
#include "elena_os_port.h"
#include "elena_os_sys.h"
#include "elena_os_display.h"
#include "elena_os_app_list.h"
#include "elena_os_lang.h"
#include "elena_os_basic_widgets.h"
#include "elena_os_app_header.h"
#include "elena_os_mem.h"

/* Macros and Definitions -------------------------------------*/
#define _MASK_OPA LV_OPA_80
#define _OPA_MAX_DIST_DIV 1 /**< 达到最大不透明度的距离 */
#define _OPA_SCALE 1000     /**< 比例计算的放大倍数 */
#define _BRIGHTNESS_SMOOTH_DURATION 300
typedef struct
{
    eos_swipe_panel_t *sp;
    lv_obj_t *mask;
} _pressing_user_data_t;
/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/
static void _flash_light_on_destroy(eos_activity_t *a);
static inline void _flash_light_delete(_pressing_user_data_t *ud);
static const eos_activity_lifecycle_t _flash_light_lifecycle = {
    .on_enter = NULL,
    .on_destroy = _flash_light_on_destroy,
};

static void _flash_light_on_destroy(eos_activity_t *a)
{
    _flash_light_delete(NULL);
    eos_display_tmp_restore_brightness_smooth();
}

static inline void _flash_light_delete(_pressing_user_data_t *ud)
{
    EOS_CHECK_PTR_RETURN(ud);

    if (ud->sp)
        eos_swipe_panel_delete(ud->sp);

    if (ud->mask && lv_obj_is_valid(ud->mask))
        lv_obj_delete_async(ud->mask);

    ud->mask = NULL;
    ud->sp = NULL;

    eos_free(ud);
    EOS_LOG_I("Flash light deleted");
}

static void _swipe_panel_pull_back_cb(lv_event_t *e)
{
    _pressing_user_data_t *ud = lv_event_get_user_data(e);
    EOS_CHECK_PTR_RETURN(ud);

    /* Use final position after slide DONE as cleanup fallback,
     * because threshold event may be skipped in some edge drags. */
    int32_t swipe_obj_coord_y = lv_obj_get_y(ud->sp->swipe_obj);
    if (swipe_obj_coord_y >= EOS_DISPLAY_HEIGHT)
    {
        _flash_light_delete(ud);
        eos_display_tmp_restore_brightness_smooth();
    }
}

/**
 * @brief 用于设置背景透明度，随y轴增加而降低透明度
 */
static void _swipe_panel_moving_cb(lv_event_t *e)
{
    _pressing_user_data_t *ud = lv_event_get_user_data(e);
    eos_swipe_panel_t *sp = ud->sp;

    lv_obj_update_layout(sp->swipe_obj);

    int32_t y = lv_obj_get_y(sp->swipe_obj);

    int32_t max_dist = EOS_DISPLAY_HEIGHT / _OPA_MAX_DIST_DIV;

    int32_t ratio = ((max_dist - y) * _OPA_SCALE) / max_dist;

    ratio = EOS_CLAMP(ratio, 0, _OPA_SCALE);

    lv_opa_t opa = (lv_opa_t)((ratio * _MASK_OPA) / _OPA_SCALE);

    EOS_LOG_I("y=%d, ratio=%d‰, opa=%d", y, ratio, opa);

    lv_obj_set_style_bg_opa(ud->mask, opa, 0);
}

static void _flash_light_clicked_cb(lv_event_t *e)
{
    _pressing_user_data_t *ud = lv_event_get_user_data(e);
    _flash_light_delete(ud);
    eos_flash_light_enter();
}

void eos_flash_light_show(void)
{
    _pressing_user_data_t *ud = eos_malloc(sizeof(_pressing_user_data_t));
    EOS_CHECK_PTR_RETURN(ud);

    lv_obj_t *layer_top = lv_layer_top();

    lv_obj_t *mask = lv_obj_create(layer_top);
    lv_obj_remove_style_all(mask);
    lv_obj_set_size(mask, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(mask, EOS_COLOR_BLACK, 0);

    ud->mask = mask;

    eos_swipe_panel_t *sp = eos_swipe_panel_create(layer_top);
    eos_swipe_panel_set_dir(sp, EOS_SWIPE_DIR_UP);
    eos_swipe_panel_slide_down(sp);
    eos_slide_widget_reverse(sp->sw);
    eos_swipe_panel_hide_handle_bar(sp);
    lv_obj_set_style_bg_opa(sp->swipe_obj, LV_OPA_TRANSP, 0);

    ud->sp = sp;

    lv_obj_add_event_cb(
        sp->sw->touch_obj,
        _swipe_panel_pull_back_cb,
        EOS_EVENT_SLIDE_WIDGET_DONE,
        ud);
    lv_obj_add_event_cb(
        sp->sw->touch_obj,
        _swipe_panel_moving_cb,
        EOS_EVENT_SLIDE_WIDGET_MOVING,
        ud);
    int32_t touch_area_height = EOS_DISPLAY_HEIGHT * 0.2;
    lv_obj_set_height(sp->sw->touch_obj, touch_area_height);

    lv_obj_t *container = sp->swipe_obj;
    lv_obj_set_height(container, 2 * EOS_DISPLAY_HEIGHT);
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

    lv_label_set_text_fmt(label, "%s\n" RI_ARROW_DOWN_WIDE_FILL,
                          current_lang[STR_ID_APP_FLASH_LIGHT_DISMISS]);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, 0);

    lv_obj_t *flash_light = lv_obj_create(container);
    lv_obj_set_style_bg_color(flash_light, EOS_COLOR_WHITE, 0);
    lv_obj_set_size(flash_light, lv_pct(100), EOS_DISPLAY_HEIGHT);
    lv_obj_set_style_border_width(flash_light, 0, 0);
    lv_obj_set_style_radius(flash_light, EOS_DISPLAY_RADIUS, 0);
    lv_obj_add_event_cb(flash_light,
                        _flash_light_clicked_cb,
                        LV_EVENT_CLICKED,
                        ud);

    eos_display_tmp_set_brightness_smooth(EOS_DISPLAY_BRIGHTNESS_MAX);
}

void eos_flash_light_enter(void)
{
    eos_display_tmp_set_brightness_smooth(EOS_DISPLAY_BRIGHTNESS_MAX);
    eos_activity_t *a = eos_activity_create(&_flash_light_lifecycle);
    if(!a) return;

    eos_activity_set_type(a, EOS_ACTIVITY_TYPE_APP);
    eos_activity_set_app_header_visible(a, true);
    eos_activity_set_app_header_time_only(a, true);

    lv_obj_t *view = eos_activity_get_view(a);
    if(!view) {
        eos_activity_back();
        return;
    }

    lv_obj_remove_style_all(view);
    lv_obj_set_size(view, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(view, EOS_COLOR_WHITE, 0);

    eos_card_pager_t *cp = eos_card_pager_create(view, EOS_CARD_PAGER_DIR_HOR);
    if(cp) {
        lv_obj_t *page = eos_card_pager_create_page(cp);
        lv_obj_set_style_bg_color(page, EOS_COLOR_YELLOW, 0);
        page = eos_card_pager_create_page(cp);
        lv_obj_set_style_bg_color(page, lv_palette_main(LV_PALETTE_RED), 0);
        eos_card_pager_move_node(cp, 0, 1);
        eos_card_pager_move_page(cp, 1);
    }

    eos_activity_enter(a);
}
