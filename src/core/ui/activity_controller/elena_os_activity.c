/**
 * @file elena_os_activity.c
 * @brief Activity 控制器
 * @author Sab1e
 * @date 2026-03-21
 */

#include "elena_os_activity.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "elena_os_stack.h"
#include "elena_os_mem.h"
#define EOS_LOG_TAG "Activity"
#include "elena_os_log.h"
#include "elena_os_core.h"
#include "elena_os_config.h"
#include "elena_os_theme.h"
#include "elena_os_lang.h"
#include "elena_os_misc.h"
#include "elena_os_app_header.h"
/* Macros and Definitions -------------------------------------*/
#define _ACTIVITY_STACK_INIT_CAPACITY 8
#define _DEFAULT_TITLE_COLOR EOS_COLOR_BLUE

typedef enum
{
    _TITLE_TYPE_INVALID = 0,
    _TITLE_TYPE_STRING,
    _TITLE_TYPE_ID
} eos_activity_title_type_t;

typedef struct
{
    lv_anim_timeline_t *at;
    lv_anim_t dummy_anim;

    eos_activity_t *from;
    eos_activity_t *to;
    bool destroy_from; // 是否在动画完成后销毁 from activity
} eos_activity_anim_ctx_t;

struct eos_activity_t
{
    lv_obj_t *view;
    bool is_app_header_visible;
    bool destroy_on_exit; // 是否在 exit 时销毁此 activity
    struct
    {
        lv_color_t color;
        union
        {
            const char *string;
            uint32_t id;
        };
        eos_activity_title_type_t type;
    } title;

    eos_activity_lifecycle_t *lifecycle;

    eos_activity_anim_cb_t anim_cb; // 作为页面切换发起方时会调用此回调，用于控制动画行为

    void *user_data;
};

typedef struct
{
    eos_activity_t *watchface_activity;
    eos_activity_t *current_activity;
    eos_stack_t *activity_stack;
    lv_obj_t *root_screen;
} eos_activity_ctx_t;

/* Variables --------------------------------------------------*/
static eos_activity_ctx_t g_activity_ctx = {
    .watchface_activity = NULL,
    .current_activity = NULL,
    .activity_stack = NULL,
    .root_screen = NULL,
};

/* Function Implementations -----------------------------------*/
static void _update_app_header_if_needed(eos_activity_t *activity);
static void _anim_timeline_start(eos_activity_t *from, eos_activity_t *to, eos_activity_anim_ctx_t *anim_ctx);
static void _init_anim_timeline(eos_activity_anim_ctx_t *anim_ctx);

static bool _controller_initialized(void)
{
    return g_activity_ctx.activity_stack != NULL && g_activity_ctx.root_screen != NULL;
}

static void _activity_run_destroy(eos_activity_t *activity)
{
    EOS_CHECK_PTR_RETURN(activity);

    if (activity->view)
    {
        lv_obj_delete(activity->view);
        activity->view = NULL;
    }

    if (activity->title.type == _TITLE_TYPE_STRING)
    {
        if (activity->title.string)
        {
            eos_free(activity->title.string);
            activity->title.string = NULL;
        }
    }

    eos_free(activity);
}

static void _activity_reset_context(void)
{
    g_activity_ctx.watchface_activity = NULL;
    g_activity_ctx.current_activity = NULL;
    g_activity_ctx.activity_stack = NULL;
    g_activity_ctx.root_screen = NULL;
}

static void _activity_show(eos_activity_t *activity)
{
    EOS_CHECK_PTR_RETURN(activity);
    if (!activity->view)
    {
        return;
    }

    lv_obj_move_foreground(activity->view);
}

static void _activity_switch_to(eos_activity_t *next_activity)
{
    EOS_CHECK_PTR_RETURN(next_activity);
    eos_activity_t *cur_activity = g_activity_ctx.current_activity;
    if (cur_activity == next_activity)
    {
        _activity_show(next_activity);
        if (next_activity->is_app_header_visible)
        {
            eos_app_header_show(next_activity);
        }
        else
        {
            eos_app_header_hide();
        }
        return;
    }

    g_activity_ctx.current_activity = next_activity;

    // 执行生命周期回调
    if (cur_activity && cur_activity->lifecycle && cur_activity->lifecycle->on_exit)
    {
        cur_activity->lifecycle->on_exit(cur_activity);
    }

    if (cur_activity && cur_activity->lifecycle && cur_activity->lifecycle->on_pause)
    {
        cur_activity->lifecycle->on_pause(cur_activity);
    }

    if (next_activity->lifecycle && next_activity->lifecycle->on_enter)
    {
        next_activity->lifecycle->on_enter(next_activity);
    }

    if (next_activity->lifecycle && next_activity->lifecycle->on_resume)
    {
        next_activity->lifecycle->on_resume(next_activity);
    }

    // 如果需要动画，创建动画上下文并启动动画
    if (cur_activity && cur_activity->anim_cb)
    {
        eos_activity_anim_ctx_t *anim_ctx = eos_malloc(sizeof(eos_activity_anim_ctx_t));
        if (anim_ctx)
        {
            anim_ctx->at = lv_anim_timeline_create();
            anim_ctx->from = cur_activity;
            anim_ctx->to = next_activity;
            anim_ctx->destroy_from = cur_activity ? cur_activity->destroy_on_exit : false;

            _init_anim_timeline(anim_ctx);
            _anim_timeline_start(cur_activity, next_activity, anim_ctx);
        }
    }

    // 显示 next_activity（在动画启动之后显示，确保动画的初始位置设置不会被覆盖）
    _activity_show(next_activity);

    // 更新 app header
    if (next_activity->is_app_header_visible)
    {
        eos_app_header_show(next_activity);
    }
    else
    {
        eos_app_header_hide();
    }

    // 如果没有动画但需要销毁 from activity，直接销毁
    if (!cur_activity || !cur_activity->anim_cb)
    {
        if (cur_activity && cur_activity->destroy_on_exit)
        {
            _activity_run_destroy(cur_activity);
        }
    }
}

static lv_obj_t *_view_create(lv_obj_t *parent)
{
    if (!parent)
    {
        parent = g_activity_ctx.root_screen;
    }

    // 创建 View（实质上是一个全屏的 lv_obj）
    lv_obj_t *view = lv_obj_create(parent);
    if (!view)
    {
        return NULL;
    }

    // 设置默认样式
    lv_obj_remove_style_all(view);
    lv_obj_add_style(view, eos_theme_get_view_style(), 0);

    return view;
}

static void _anim_clean_up_activity(lv_anim_t *a)
{
    if (!a)
        return;

    eos_activity_anim_ctx_t *anim_ctx = (eos_activity_anim_ctx_t *)lv_anim_get_user_data(a);
    if (!anim_ctx)
        return;

    // 如果需要销毁 from activity
    if (anim_ctx->destroy_from && anim_ctx->from)
    {
        _activity_run_destroy(anim_ctx->from);
        anim_ctx->from = NULL;
    }

    // 清理时间线
    if (anim_ctx->at)
    {
        lv_anim_timeline_delete(anim_ctx->at);
        anim_ctx->at = NULL;
    }

    // 清理 eos_activity_anim_ctx_t
    eos_free(anim_ctx);
}

static void _init_anim_timeline(eos_activity_anim_ctx_t *anim_ctx)
{
    if (!anim_ctx)
    {
        return;
    }

    lv_anim_init(&anim_ctx->dummy_anim);
    lv_anim_set_var(&anim_ctx->dummy_anim, lv_screen_active());
    lv_anim_set_values(&anim_ctx->dummy_anim, 0, 100);
    lv_anim_set_delay(&anim_ctx->dummy_anim, 1);
    lv_anim_set_completed_cb(&anim_ctx->dummy_anim, _anim_clean_up_activity);
    lv_anim_set_user_data(&anim_ctx->dummy_anim, anim_ctx);
    lv_anim_start(&anim_ctx->dummy_anim);
}

static void _anim_timeline_start(eos_activity_t *from, eos_activity_t *to, eos_activity_anim_ctx_t *anim_ctx)
{
    if (!anim_ctx)
    {
        return;
    }

    // 执行当前页面的动画回调
    if (from && from->anim_cb)
    {
        from->anim_cb(anim_ctx->at, from, to);
    }

    // 计算持续时间
    uint32_t playtime = lv_anim_timeline_get_playtime(anim_ctx->at);
    if (playtime == 0)
    {
        // 如果没有动画，直接清理
        _anim_clean_up_activity(&anim_ctx->dummy_anim);
        return;
    }

    // 设置 dummy 动画的持续时间并添加到时间线
    lv_anim_set_duration(&anim_ctx->dummy_anim, playtime);
    lv_anim_timeline_add(anim_ctx->at, 0, &anim_ctx->dummy_anim);

    // 启动动画
    lv_anim_timeline_start(anim_ctx->at);
}

void eos_activity_set_anim_cb(eos_activity_t *activity, eos_activity_anim_cb_t cb)
{
    EOS_CHECK_PTR_RETURN(activity);
    activity->anim_cb = cb;
}

void *eos_activity_get_user_data(eos_activity_t *activity)
{
    EOS_CHECK_PTR_RETURN_VAL(activity, NULL);
    return activity->user_data;
}

void eos_activity_set_user_data(eos_activity_t *activity, void *user_data)
{
    EOS_CHECK_PTR_RETURN(activity);
    activity->user_data = user_data;
}

lv_obj_t *eos_activity_get_view(eos_activity_t *activity)
{
    EOS_CHECK_PTR_RETURN_VAL(activity, NULL);
    return activity->view;
}

void eos_activity_set_view(eos_activity_t *activity, lv_obj_t *view)
{
    EOS_CHECK_PTR_RETURN(activity);
    if (activity->view && activity->view != view && lv_obj_is_valid(activity->view))
    {
        lv_obj_delete(activity->view);
    }
    activity->view = view;
}

lv_obj_t *eos_activity_get_root_screen(void)
{
    return g_activity_ctx.root_screen;
}

eos_activity_t *eos_activity_get_watchface(void)
{
    return g_activity_ctx.watchface_activity;
}

const char *eos_activity_get_title(eos_activity_t *activity)
{
    EOS_CHECK_PTR_RETURN_VAL(activity, NULL);
    if (activity->title.type == _TITLE_TYPE_STRING)
    {
        return activity->title.string;
    }
    else if (activity->title.type == _TITLE_TYPE_ID)
    {
        return eos_lang_get_str(activity->title.id);
    }
    return NULL;
}

void eos_activity_set_title(eos_activity_t *activity, const char *title)
{
    EOS_CHECK_PTR_RETURN(activity);
    if (activity->title.string)
    {
        eos_free(activity->title.string);
        activity->title.string = NULL;
    }
    if (title)
    {
        activity->title.string = eos_strdup(title);
        activity->title.type = _TITLE_TYPE_STRING;
    }
    else
    {
        activity->title.type = _TITLE_TYPE_INVALID;
    }

    // 立即更新app header
    _update_app_header_if_needed(activity);
}

void eos_activity_set_title_id(eos_activity_t *activity, lang_string_id_t id)
{
    EOS_CHECK_PTR_RETURN(activity);
    if (activity->title.string)
    {
        eos_free(activity->title.string);
        activity->title.string = NULL;
    }
    activity->title.id = id;
    activity->title.type = _TITLE_TYPE_ID;

    // 立即更新app header
    _update_app_header_if_needed(activity);
}

void eos_activity_set_app_header_visible(eos_activity_t *activity, bool visible)
{
    EOS_CHECK_PTR_RETURN(activity);
    activity->is_app_header_visible = visible;

    // 立即更新app header
    if (visible)
    {
        _update_app_header_if_needed(activity);
    }
    else
    {
        // 检查当前activity是否是当前显示的activity
        eos_activity_t *current = eos_activity_get_current();
        if (current == activity)
        {
            eos_app_header_hide();
        }
    }
}

static void _update_app_header_if_needed(eos_activity_t *activity)
{
    // 检查当前activity是否是当前显示的activity
    eos_activity_t *current = eos_activity_get_current();
    if (current != activity)
        return;

    // 检查app header是否可见
    if (!activity->is_app_header_visible)
        return;

    // 立即更新app header
    eos_app_header_show(activity);
}

bool eos_activity_is_app_header_visible(eos_activity_t *activity)
{
    EOS_CHECK_PTR_RETURN_VAL(activity, false);
    return activity->is_app_header_visible;
}

lv_color_t eos_activity_get_title_color(eos_activity_t *activity)
{
    EOS_CHECK_PTR_RETURN_VAL(activity, _DEFAULT_TITLE_COLOR);
    return activity->title.color;
}

void eos_activity_set_title_color(eos_activity_t *activity, lv_color_t color)
{
    EOS_CHECK_PTR_RETURN(activity);
    activity->title.color = color;

    // 立即更新app header
    _update_app_header_if_needed(activity);
}

lv_obj_t *eos_view_active(void)
{
    eos_activity_t *current = eos_activity_get_current();
    if (!current || !current->view || !lv_obj_is_valid(current->view))
    {
        return NULL;
    }
    return current->view;
}

eos_result_t eos_activity_controller_init(eos_activity_t *initial_activity)
{
    EOS_CHECK_PTR_RETURN_VAL(initial_activity, EOS_FAILED);

    if (_controller_initialized())
    {
        eos_activity_controller_deinit();
    }

    if (lv_screen_active())
    {
        lv_obj_delete(lv_screen_active());
    }
    g_activity_ctx.root_screen = lv_obj_create(NULL);
    lv_obj_set_scrollbar_mode(g_activity_ctx.root_screen, LV_SCROLLBAR_MODE_OFF);
    lv_obj_remove_flag(g_activity_ctx.root_screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_screen_load(g_activity_ctx.root_screen);

    g_activity_ctx.activity_stack = eos_stack_create_with_mode(_ACTIVITY_STACK_INIT_CAPACITY, EOS_STACK_CAPACITY_FIXED);
    if (!g_activity_ctx.activity_stack)
    {
        if (g_activity_ctx.root_screen)
        {
            lv_obj_delete(g_activity_ctx.root_screen);
            g_activity_ctx.root_screen = NULL;
        }
        return EOS_FAILED;
    }

    if (!initial_activity->view)
    {
        initial_activity->view = _view_create(g_activity_ctx.root_screen);
    }
    else
    {
        lv_obj_set_parent(initial_activity->view, g_activity_ctx.root_screen);
    }

    if (!initial_activity->view)
    {
        eos_stack_destroy(g_activity_ctx.activity_stack);
        _activity_reset_context();
        return EOS_FAILED;
    }

    g_activity_ctx.watchface_activity = initial_activity;

    if (initial_activity->lifecycle && initial_activity->lifecycle->on_enter)
    {
        initial_activity->lifecycle->on_enter(initial_activity);
    }
    _activity_show(initial_activity);
    g_activity_ctx.current_activity = initial_activity;

    if (initial_activity->is_app_header_visible)
    {
        eos_app_header_show(initial_activity);
    }
    else
    {
        eos_app_header_hide();
    }

    return EOS_OK;
}

eos_activity_t *eos_activity_create(const eos_activity_lifecycle_t *lifecycle)
{
    eos_activity_t *activity = eos_malloc(sizeof(eos_activity_t));
    if (!activity)
    {
        return NULL;
    }

    activity->view = _view_create(g_activity_ctx.root_screen);
    if (!activity->view)
    {
        eos_free(activity);
        return NULL;
    }
    activity->lifecycle = lifecycle;
    activity->is_app_header_visible = false;
    activity->destroy_on_exit = false;
    activity->title.color = _DEFAULT_TITLE_COLOR;
    activity->title.type = _TITLE_TYPE_INVALID;
    activity->title.string = NULL;

    return activity;
}

void eos_activity_enter(eos_activity_t *activity)
{
    EOS_CHECK_PTR_RETURN(activity);
    if (!_controller_initialized())
    {
        return;
    }

    if (g_activity_ctx.current_activity == activity)
    {
        _activity_show(activity);
        return;
    }

    if (activity == g_activity_ctx.watchface_activity)
    {
        _activity_switch_to(activity);
        return;
    }

    if (!eos_stack_push(g_activity_ctx.activity_stack, activity))
    {
        return;
    }

    _activity_switch_to(activity);
}

eos_result_t eos_activity_back(void)
{
    if (!_controller_initialized())
    {
        return EOS_FAILED;
    }

    if (eos_stack_get_size(g_activity_ctx.activity_stack) == 0)
    {
        if (g_activity_ctx.watchface_activity && g_activity_ctx.current_activity != g_activity_ctx.watchface_activity)
        {
            _activity_switch_to(g_activity_ctx.watchface_activity);
            return EOS_OK;
        }
        return EOS_FAILED;
    }

    eos_activity_t *current = eos_stack_pop(g_activity_ctx.activity_stack);
    EOS_CHECK_PTR_RETURN_VAL(current, EOS_FAILED);

    // 标记此 activity 在 exit 时需要销毁
    current->destroy_on_exit = true;

    // 执行生命周期回调
    if (current->lifecycle && current->lifecycle->on_exit)
    {
        current->lifecycle->on_exit(current);
    }

    // 确定要切换到的 activity
    eos_activity_t *prev = NULL;
    if (eos_stack_get_size(g_activity_ctx.activity_stack) == 0)
    {
        prev = g_activity_ctx.watchface_activity;
    }
    else
    {
        prev = eos_stack_peek(g_activity_ctx.activity_stack);
    }

    EOS_CHECK_PTR_RETURN_VAL(prev, EOS_FAILED);

    // 切换到上一个 activity，destroy_on_exit 标志会控制是否销毁当前 activity
    _activity_switch_to(prev);

    return EOS_OK;
}

void eos_activity_back_cb(lv_event_t *e)
{
    eos_activity_back();
}

eos_activity_t *eos_activity_get_current(void)
{
    if (!_controller_initialized())
    {
        return NULL;
    }
    if (g_activity_ctx.current_activity)
        return g_activity_ctx.current_activity;
    if (g_activity_ctx.watchface_activity)
        return g_activity_ctx.watchface_activity;
    return NULL;
}

void eos_activity_controller_deinit(void)
{
    if (!g_activity_ctx.activity_stack)
    {
        if (g_activity_ctx.root_screen)
        {
            lv_obj_delete(g_activity_ctx.root_screen);
            g_activity_ctx.root_screen = NULL;
        }
        return;
    }

    while (eos_stack_get_size(g_activity_ctx.activity_stack) > 0)
    {
        eos_activity_t *activity = eos_stack_pop(g_activity_ctx.activity_stack);
        _activity_run_destroy(activity);
    }

    if (g_activity_ctx.watchface_activity)
    {
        _activity_run_destroy(g_activity_ctx.watchface_activity);
        g_activity_ctx.watchface_activity = NULL;
    }

    eos_stack_destroy(g_activity_ctx.activity_stack);

    if (g_activity_ctx.root_screen)
    {
        lv_obj_delete(g_activity_ctx.root_screen);
    }

    _activity_reset_context();
}
