/**
 * @file elena_os_app_header.c
 * @brief 应用的顶部导航头
 * @author Sab1e
 * @date 2025-11-25
 */

#include "elena_os_app_header.h"
#include "elena_os_config.h"

#if EOS_APP_HEADER_ENABLE

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#define EOS_LOG_DISABLE
#define EOS_LOG_TAG "AppHeader"
#include "elena_os_log.h"
#include "elena_os_core.h"
#include "elena_os_port.h"
#include "elena_os_lang.h"
#include "elena_os_img.h"
#include "elena_os_theme.h"
#include "elena_os_font.h"
#include "elena_os_basic_widgets.h"
#include "elena_os_misc.h"
#include "elena_os_anim.h"
#include "elena_os_mem.h"
#include "elena_os_activity.h"

/* Macros and Definitions -------------------------------------*/
#define _HEADER_HEIGHT 120
#define _HEADER_CLOCK_UPDATE_PERIOD_MINUTES 1 /**< 时钟标签文本更新间隔，单位：分钟 */

#define _HEADER_MARGIN_RIGHT 30

#define _HEADER_TITLE_WIDTH 240

#define _TITLE_LABEL_Y_OFFSET 20
#define _TITLE_LABEL_X_OFFSET -_HEADER_MARGIN_RIGHT
#define _ANIM_DURATION 350

#define _BACK_BTN_MARGIN_LEFT 20

#define _ANIM_TITLE_MOVE_DISTANCE 50
#define _ANIM_BACK_BTN_MOVE_DISTANCE _ANIM_TITLE_MOVE_DISTANCE

/**
 * @brief 应用内上方的头部结构体定义
 */
typedef struct
{
    lv_obj_t *container;
    lv_obj_t *clock_label;
    lv_obj_t *title_label;
    lv_obj_t *back_btn;
    lv_timer_t *clock_timer;
    lv_obj_t *original_parent; // 原始父对象，用于恢复
    bool is_anim_entering; // 动画方向
    bool attached_to_view; // 是否附加到View
} eos_app_header_t;

/* Variables --------------------------------------------------*/
static eos_app_header_t *app_header = NULL;
/* Function Implementations -----------------------------------*/
static void _clock_update_cb(lv_timer_t *timer);

static void _app_header_set_opa_anim_cb(void *var, int32_t value)
{
    lv_obj_t *obj = (lv_obj_t *)var;
    if (!obj || !lv_obj_is_valid(obj))
        return;

    lv_obj_set_style_opa(obj, (lv_opa_t)value, 0);
}

static void _app_header_fade_out_ready_cb(lv_anim_t *a)
{
    lv_obj_t *obj = (lv_obj_t *)a->var;
    if (!obj || !lv_obj_is_valid(obj))
        return;

    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_opa(obj, LV_OPA_COVER, 0);
}

static void _app_header_apply_activity_mode(eos_activity_t *activity)
{
    EOS_CHECK_PTR_RETURN(app_header);

    bool time_only = false;
    lv_color_t clock_color = EOS_COLOR_WHITE;
    if (activity)
    {
        time_only = eos_activity_is_app_header_time_only(activity);
        if (time_only)
        {
            clock_color = eos_activity_get_app_header_time_only_text_color(activity);
        }
    }

    if (app_header->container && lv_obj_is_valid(app_header->container))
    {
        lv_opa_t bg_opa = time_only ? LV_OPA_TRANSP : LV_OPA_COVER;
        lv_obj_set_style_bg_opa(app_header->container, bg_opa, 0);
    }

    if (app_header->clock_label && lv_obj_is_valid(app_header->clock_label))
    {
        lv_obj_set_style_text_color(app_header->clock_label, clock_color, 0);
        lv_obj_remove_flag(app_header->clock_label, LV_OBJ_FLAG_HIDDEN);
    }

    if (app_header->title_label && lv_obj_is_valid(app_header->title_label))
    {
        if (time_only)
            lv_obj_add_flag(app_header->title_label, LV_OBJ_FLAG_HIDDEN);
        else
            lv_obj_remove_flag(app_header->title_label, LV_OBJ_FLAG_HIDDEN);
    }

    if (app_header->back_btn && lv_obj_is_valid(app_header->back_btn))
    {
        if (time_only)
            lv_obj_add_flag(app_header->back_btn, LV_OBJ_FLAG_HIDDEN);
        else
            lv_obj_remove_flag(app_header->back_btn, LV_OBJ_FLAG_HIDDEN);
    }
}

static bool _app_header_can_reparent(lv_obj_t *new_parent)
{
    if (!app_header)
    {
        return false;
    }

    if (!app_header->container || !lv_obj_is_valid(app_header->container))
    {
        app_header->attached_to_view = false;
        EOS_LOG_E("AppHeader container is invalid");
        return false;
    }

    if (!new_parent || !lv_obj_is_valid(new_parent))
    {
        EOS_LOG_E("AppHeader target parent is invalid");
        return false;
    }

    return true;
}

static void _set_title_style(lv_obj_t *label)
{
    lv_obj_add_style(label, eos_theme_get_label_style(), 0);
    lv_obj_set_width(label, _HEADER_TITLE_WIDTH);
    lv_label_set_text(label, "");
    eos_label_set_font_size(label, EOS_FONT_SIZE_LARGE);

    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_RIGHT, 0);
    lv_obj_set_style_text_color(label, EOS_THEME_PRIMARY_COLOR, 0);
    lv_obj_align(label, LV_ALIGN_RIGHT_MID, _TITLE_LABEL_X_OFFSET, _TITLE_LABEL_Y_OFFSET);
}

static void _set_back_btn_style(lv_obj_t *btn)
{
    lv_obj_set_size(btn, 64, 64);
    lv_obj_set_style_radius(btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(btn, EOS_THEME_SECONDARY_COLOR, 0);
    lv_obj_align(btn, LV_ALIGN_LEFT_MID, _BACK_BTN_MARGIN_LEFT, 0);
}
// TODO 似乎Settings页面存在0.1KB内存泄漏
void _play_title_changed_anim(eos_activity_t *from, eos_activity_t *to, bool need_anim, bool reverse_anim)
{
    EOS_CHECK_PTR_RETURN(app_header);
    if (!(lv_obj_is_valid(app_header->title_label) &&
          lv_obj_has_class(app_header->title_label, &lv_label_class)))
        return;

    bool from_time_only = from ? eos_activity_is_app_header_time_only(from) : false;
    bool to_time_only = to ? eos_activity_is_app_header_time_only(to) : false;

    // 任一侧为仅时间模式时，直接切换显示模式，不执行标题/返回键转场动画。
    if (from_time_only || to_time_only)
    {
        need_anim = false;
    }

    // 如果不需要动画，直接更新标题和颜色
    if (!need_anim) {
        // 从to activity获取标题
        const char *new_title = eos_activity_get_title(to);
        EOS_LOG_D("New title: %s", new_title);
        lv_label_set_text(app_header->title_label, new_title ? new_title : "");

        // 从to activity获取标题颜色
        lv_color_t color = eos_activity_get_title_color(to);
        lv_obj_set_style_text_color(app_header->title_label, color, 0);
        _app_header_apply_activity_mode(to);
        return;
    }

    // 确定动画方向
    if (reverse_anim) {
        app_header->is_anim_entering = false;
    } else {
        app_header->is_anim_entering = true;
    }

    lv_obj_t *l = app_header->title_label;
    lv_obj_t *back_btn = app_header->back_btn;
    lv_obj_t *parent = lv_obj_get_parent(l);

    int32_t title_start_x = 0;
    int32_t title_end_x;
    int32_t back_btn_start_x = 0;
    int32_t back_btn_end_x;

    // 确定移动方向
    if (app_header->is_anim_entering)
    {
        // 进入：从右往左滑入
        title_end_x = title_start_x - _ANIM_TITLE_MOVE_DISTANCE; // 向左移动
        back_btn_end_x = back_btn_start_x - _ANIM_BACK_BTN_MOVE_DISTANCE;
    }
    else
    {
        // 退出：从左往右滑入
        title_end_x = title_start_x + _ANIM_TITLE_MOVE_DISTANCE; // 向右移动
        back_btn_end_x = back_btn_start_x + _ANIM_BACK_BTN_MOVE_DISTANCE;
    }

    // 原始按钮从默认位置移动到目标位置
    eos_anim_move_start(l, title_start_x, 0, title_end_x, 0, _ANIM_DURATION, false);
    eos_anim_fade_start(l, LV_OPA_COVER, LV_OPA_TRANSP, _ANIM_DURATION + 1, true);

    // 原始 back_btn 从默认位置移动到目标位置
    eos_anim_move_start(back_btn, back_btn_start_x, 0, back_btn_end_x, 0, _ANIM_DURATION, false);
    eos_anim_fade_start(back_btn, LV_OPA_COVER, LV_OPA_TRANSP, _ANIM_DURATION + 1, true); // 淡出后自动删除

    // 创建新的 title_label 和 back_btn
    lv_obj_t *new_l = lv_label_create(parent);
    _set_title_style(new_l);

    // 从to activity获取标题
    const char *new_title = eos_activity_get_title(to);
    EOS_LOG_D("New title: %s", new_title);
    lv_label_set_text(new_l, new_title ? new_title : "");

    // 从to activity获取标题颜色
    lv_color_t color = eos_activity_get_title_color(to);
    lv_obj_set_style_text_color(new_l, color, 0);

    // 创建新的返回按钮
    lv_obj_t *new_back_btn = eos_back_btn_create(parent, false);
    _set_back_btn_style(new_back_btn);

    // 新按钮的起始和结束位置
    int32_t new_title_start_x, new_title_end_x = 0;
    int32_t new_back_btn_start_x, new_back_btn_end_x = 0;

    if (app_header->is_anim_entering)
    {
        // 新按钮从右侧进入
        new_title_start_x = new_title_end_x + _ANIM_TITLE_MOVE_DISTANCE;
        new_back_btn_start_x = new_back_btn_end_x + _ANIM_BACK_BTN_MOVE_DISTANCE;
    }
    else
    {
        // 新按钮从左侧进入
        new_title_start_x = new_title_end_x - _ANIM_TITLE_MOVE_DISTANCE;
        new_back_btn_start_x = new_back_btn_end_x - _ANIM_BACK_BTN_MOVE_DISTANCE;
    }

    // 从起始位置移动到默认位置
    eos_anim_move_start(new_l, new_title_start_x, 0, new_title_end_x, 0, _ANIM_DURATION, false);
    eos_anim_fade_start(new_l, LV_OPA_TRANSP, LV_OPA_COVER, _ANIM_DURATION, false);

    // 从起始位置移动到默认位置
    eos_anim_move_start(new_back_btn, new_back_btn_start_x, 0, new_back_btn_end_x, 0, _ANIM_DURATION, false);
    eos_anim_fade_start(new_back_btn, LV_OPA_TRANSP, LV_OPA_COVER, _ANIM_DURATION, false);

    // 更新指针
    app_header->title_label = new_l;
    app_header->back_btn = new_back_btn;
}

/**
 * @brief 更新LVGL字符串，显示当前时间
 */
static inline void _app_header_update_clock_label(lv_obj_t *label)
{
    eos_datetime_t dt = eos_time_get_core();
    uint32_t next_ms = (uint32_t)((_HEADER_CLOCK_UPDATE_PERIOD_MINUTES * 60) - dt.sec) * 1000;
    lv_timer_set_period(app_header->clock_timer, next_ms);
    lv_label_set_text_fmt(label, "%02d:%02d", dt.hour, dt.min);
}

/**
 * @brief 时间刷新的回调，由 LVGL 的定时器触发
 */
static void _clock_update_cb(lv_timer_t *timer)
{
    lv_obj_t *label = lv_timer_get_user_data(timer);
    EOS_CHECK_PTR_RETURN(app_header && label);
    if (lv_obj_has_flag(app_header->container, LV_OBJ_FLAG_HIDDEN))
    {
        return;
    }
    // 更新显示文字
    _app_header_update_clock_label(label);
}

void eos_app_header_hide(void)
{
    EOS_CHECK_PTR_RETURN(app_header);
    EOS_LOG_D("Hide app header");
    // 如果附加到View，先恢复父对象
    if (app_header->attached_to_view)
    {
        lv_obj_t *restore_parent = app_header->original_parent;
        if (!restore_parent || !lv_obj_is_valid(restore_parent))
        {
            restore_parent = lv_layer_sys();
            app_header->original_parent = restore_parent;
        }

        if (_app_header_can_reparent(restore_parent))
        {
            lv_obj_set_parent(app_header->container, restore_parent);
        }
        app_header->attached_to_view = false;
    }

    if (app_header->container && lv_obj_is_valid(app_header->container))
    {
        lv_obj_add_flag(app_header->container, LV_OBJ_FLAG_HIDDEN);
    }
}

void eos_app_header_show(eos_activity_t *a)
{
    EOS_CHECK_PTR_RETURN(app_header);

    // 检查是否是Watchface Activity，如果是则不显示AppHeader
    eos_activity_t *target_activity = a;
    if (!target_activity)
        target_activity = eos_activity_get_current();

    if (target_activity && target_activity == eos_activity_get_watchface())
    {
        EOS_LOG_D("Skip showing app header for watchface activity");
        return;
    }

    EOS_LOG_D("Show app header");
    if (!(app_header->container && lv_obj_is_valid(app_header->container)))
    {
        app_header->attached_to_view = false;
        EOS_LOG_E("AppHeader container is invalid");
        return;
    }

    if (app_header->attached_to_view)
    {
        lv_obj_t *restore_parent = app_header->original_parent;
        if (!restore_parent || !lv_obj_is_valid(restore_parent))
        {
            restore_parent = lv_layer_sys();
            app_header->original_parent = restore_parent;
        }

        if (_app_header_can_reparent(restore_parent))
        {
            lv_obj_set_parent(app_header->container, restore_parent);
        }
        app_header->attached_to_view = false;
    }
    // 从当前 Activity 获取标题文字
    const char *title = NULL;
    if (a)
        title = eos_activity_get_title(a);
    else
        title = eos_activity_get_title(eos_activity_get_current());
    if (title)
        lv_label_set_text(app_header->title_label, title);
    else
        lv_label_set_text(app_header->title_label, "");
    // 从当前 Activity 获取标题颜色
    lv_color_t color = EOS_COLOR_WHITE;
    if (a)
        color = eos_activity_get_title_color(a);
    else
        color = eos_activity_get_title_color(eos_activity_get_current());
    if (lv_obj_is_valid(app_header->title_label))
        lv_obj_set_style_text_color(app_header->title_label, color, 0);

    _app_header_apply_activity_mode(target_activity);
    _app_header_update_clock_label(app_header->clock_label);
    lv_obj_remove_flag(app_header->container, LV_OBJ_FLAG_HIDDEN);
}

void eos_app_header_set_visible_animated(eos_activity_t *a, bool visible, uint32_t duration_ms)
{
    EOS_CHECK_PTR_RETURN(app_header);

    if (!(app_header->container && lv_obj_is_valid(app_header->container)))
        return;

    if (duration_ms == 0)
    {
        if (visible)
            eos_app_header_show(a);
        else
            eos_app_header_hide();
        return;
    }

    lv_obj_t *container = app_header->container;
    lv_anim_del(container, _app_header_set_opa_anim_cb);

    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, container);
    lv_anim_set_exec_cb(&anim, _app_header_set_opa_anim_cb);
    lv_anim_set_duration(&anim, duration_ms);

    if (visible)
    {
        eos_app_header_show(a);
        lv_obj_set_style_opa(container, LV_OPA_TRANSP, 0);
        lv_anim_set_values(&anim, LV_OPA_TRANSP, LV_OPA_COVER);
        lv_anim_start(&anim);
    }
    else
    {
        if (lv_obj_has_flag(container, LV_OBJ_FLAG_HIDDEN))
            return;

        lv_obj_set_style_opa(container, LV_OPA_COVER, 0);
        lv_anim_set_values(&anim, LV_OPA_COVER, LV_OPA_TRANSP);
        lv_anim_set_ready_cb(&anim, _app_header_fade_out_ready_cb);
        lv_anim_start(&anim);
    }
}

/**
 * @brief 附加app header到指定View
 * @param view 要附加的View
 */
void eos_app_header_attach_to_view(lv_obj_t *view)
{
    EOS_CHECK_PTR_RETURN(app_header && view);

    if (!_app_header_can_reparent(view))
    {
        app_header->attached_to_view = false;
        return;
    }

    if (app_header->attached_to_view)
    {
        lv_obj_t *cur_parent = lv_obj_get_parent(app_header->container);
        if (cur_parent == view)
        {
            return;
        }
    }

    if (!app_header->original_parent || !lv_obj_is_valid(app_header->original_parent))
    {
        app_header->original_parent = lv_layer_sys();
    }

    lv_obj_set_parent(app_header->container, view);
    app_header->attached_to_view = true;
}

/**
 * @brief 从View中分离app header，恢复到原始父对象
 */
void eos_app_header_detach_from_view(void)
{
    EOS_CHECK_PTR_RETURN(app_header);
    if (!app_header->attached_to_view)
    {
        return;
    }

    lv_obj_t *restore_parent = app_header->original_parent;
    if (!restore_parent || !lv_obj_is_valid(restore_parent))
    {
        restore_parent = lv_layer_sys();
        app_header->original_parent = restore_parent;
    }

    if (_app_header_can_reparent(restore_parent))
    {
        lv_obj_set_parent(app_header->container, restore_parent);
    }
    app_header->attached_to_view = false;
}

bool eos_app_header_is_visible(void)
{
    EOS_CHECK_PTR_RETURN_VAL(app_header, false);
    return !lv_obj_has_flag(app_header->container, LV_OBJ_FLAG_HIDDEN);
}

static void _update_title_label(lv_event_t *e)
{
    const char *title = eos_activity_get_title(eos_activity_get_current());
    if (title)
        lv_label_set_text(app_header->title_label, title);
    else
        lv_label_set_text(app_header->title_label, "");
}

void eos_app_header_init(void)
{
    EOS_LOG_D("Init eos_app_header");
    app_header = eos_malloc_zeroed(sizeof(eos_app_header_t));
    EOS_CHECK_PTR_RETURN_FREE(app_header, app_header);

    static lv_grad_dsc_t grad;
    grad.dir = LV_GRAD_DIR_VER;
    grad.stops_count = 2;
    grad.stops[0].color = lv_color_black();
    grad.stops[0].opa = LV_OPA_90;
    grad.stops[0].frac = 125;

    grad.stops[1].color = lv_color_black();
    grad.stops[1].opa = LV_OPA_TRANSP;
    grad.stops[1].frac = 255;

    // 半透明容器
    app_header->container = lv_obj_create(lv_layer_sys());
    app_header->original_parent = lv_obj_get_parent(app_header->container); // 保存原始父对象
    lv_obj_remove_style_all(app_header->container);
    lv_obj_set_size(app_header->container, EOS_DISPLAY_WIDTH, _HEADER_HEIGHT);
    lv_obj_align(app_header->container, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_move_background(app_header->container);
    lv_obj_set_style_bg_grad(app_header->container, &grad, 0);
    lv_obj_set_style_bg_opa(app_header->container, LV_OPA_COVER, 0);
    lv_obj_remove_flag(app_header->container, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_remove_flag(app_header->container, LV_OBJ_FLAG_CLICKABLE);

    lv_coord_t header_h = _HEADER_HEIGHT;
    lv_coord_t header_w = lv_obj_get_width(app_header->container);

    // 返回按钮
    app_header->back_btn = eos_back_btn_create(app_header->container, false);
    _set_back_btn_style(app_header->back_btn);

    // 时间文字
    app_header->clock_label = lv_label_create(app_header->container);
    lv_obj_add_style(app_header->clock_label, eos_theme_get_label_style(), 0);
    app_header->clock_timer = lv_timer_create(_clock_update_cb, _HEADER_CLOCK_UPDATE_PERIOD_MINUTES * 60 * 1000, app_header->clock_label);
    lv_timer_set_repeat_count(app_header->clock_timer, -1);
    _app_header_update_clock_label(app_header->clock_label);
    lv_obj_align(app_header->clock_label, LV_ALIGN_RIGHT_MID, -_HEADER_MARGIN_RIGHT, -20);

    // 标题文字
    app_header->title_label = lv_label_create(app_header->container);
    _set_title_style(app_header->title_label);
    eos_event_add_cb(app_header->title_label, _update_title_label, LV_EVENT_REFRESH, NULL);

    // 默认隐藏 app_header
    lv_obj_add_flag(app_header->container, LV_OBJ_FLAG_HIDDEN);

    app_header->is_anim_entering = false;
    app_header->attached_to_view = false;
}

#endif /* EOS_APP_HEADER_ENABLE */
