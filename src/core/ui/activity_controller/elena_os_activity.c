/**
 * @file elena_os_activity.c
 * @brief Activity 控制器
 * @author Sab1e
 * @date 2026-03-21
 */

#include "elena_os_activity.h"

/* Includes ---------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "elena_os_stack.h"
#include "elena_os_mem.h"
#define EOS_LOG_TAG "Activity"
#include "elena_os_log.h"
#include "elena_os_core.h"
#include "elena_os_config.h"
#include "elena_os_dispatcher.h"
#include "elena_os_theme.h"
#include "elena_os_lang.h"
#include "elena_os_misc.h"
#include "elena_os_basic_widgets.h"
#include "elena_os_app_header.h"
/* Macros and Definitions -------------------------------------*/
#define _ACTIVITY_STACK_INIT_CAPACITY 8
#define _DEFAULT_TITLE_COLOR EOS_COLOR_BLUE
#define _SNAPSHOT_COLOR_FORMAT LV_COLOR_FORMAT_NATIVE_WITH_ALPHA
#define _DEBUG_SNAPSHOT 0

typedef enum
{
    _TITLE_TYPE_INVALID = 0,
    _TITLE_TYPE_STRING,
    _TITLE_TYPE_ID
} eos_activity_title_type_t;

typedef struct eos_activity_snapshot_node_t
{
    lv_obj_t *snapshot_obj;
    lv_draw_buf_t *draw_buf;
    eos_activity_t *owner;
    struct eos_activity_snapshot_node_t *next;
} eos_activity_snapshot_node_t;

typedef struct
{
    lv_anim_timeline_t *at;
    lv_anim_t dummy_anim;

    eos_activity_t *from;
    eos_activity_t *to;
    bool destroy_from; // 是否在动画完成后销毁 from activity
    bool cleanup_scheduled;
    eos_activity_snapshot_node_t *snapshots;
} eos_activity_anim_ctx_t;

struct eos_activity_t
{
    lv_obj_t *view;
    eos_activity_type_t type;
    uint8_t snapshot_ref_count;
    bool is_app_header_visible;
    bool destroy_on_exit; // 是否在 exit 时销毁此 activity
    struct
    {
        lv_color_t color;
        union
        {
            char *string;
            uint32_t id;
        };
        eos_activity_title_type_t type;
    } title;

    const eos_activity_lifecycle_t *lifecycle;

    void *user_data;
};

typedef struct
{
    eos_activity_t *watchface_activity;
    eos_activity_t *current_activity;
    eos_activity_t *visible_activity;
    eos_stack_t *activity_stack;
    lv_obj_t *root_screen;
    bool transition_in_progress;
    bool snapshot_capture_window;
    eos_activity_anim_ctx_t *active_anim_ctx;
} eos_activity_ctx_t;

/* Variables --------------------------------------------------*/
static eos_activity_ctx_t g_activity_ctx = {
    .watchface_activity = NULL,
    .current_activity = NULL,
    .visible_activity = NULL,
    .activity_stack = NULL,
    .root_screen = NULL,
    .transition_in_progress = false,
    .snapshot_capture_window = false,
    .active_anim_ctx = NULL,
};

static eos_activity_anim_cb_t g_anim_callback_routes[EOS_ACTIVITY_TYPE_COUNT][EOS_ACTIVITY_TYPE_COUNT] = {0};

/* Function Implementations -----------------------------------*/
static void _update_app_header_if_needed(eos_activity_t *activity);
static void _anim_timeline_start(eos_activity_t *from, eos_activity_t *to, eos_activity_anim_ctx_t *anim_ctx);
static void _init_anim_timeline(eos_activity_anim_ctx_t *anim_ctx);
static void _anim_dummy_exec_cb(void *var, int32_t value);
static void _anim_clean_up_activity_deferred(void *user_data);
static void _activity_mark_visible(eos_activity_t *activity);
static void _snapshot_img_delete_cb(lv_event_t *e);
static void _activity_snapshot_hold(eos_activity_t *activity);
static void _activity_snapshot_release(eos_activity_t *activity);

static bool _controller_initialized(void)
{
    return g_activity_ctx.activity_stack != NULL && g_activity_ctx.root_screen != NULL;
}

static void _activity_run_destroy(eos_activity_t *activity)
{
    EOS_CHECK_PTR_RETURN(activity);

    activity->user_data = NULL;

    if (activity->view && lv_obj_is_valid(activity->view))
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
    g_activity_ctx.visible_activity = NULL;
    g_activity_ctx.activity_stack = NULL;
    g_activity_ctx.root_screen = NULL;
    g_activity_ctx.transition_in_progress = false;
    g_activity_ctx.snapshot_capture_window = false;
    g_activity_ctx.active_anim_ctx = NULL;
    memset(g_anim_callback_routes, 0, sizeof(g_anim_callback_routes));
}

static void _activity_show(eos_activity_t *activity)
{
    EOS_CHECK_PTR_RETURN(activity);
    if (!activity->view)
    {
        return;
    }

    lv_obj_remove_flag(activity->view, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(activity->view);
}

static void _anim_dummy_exec_cb(void *var, int32_t value)
{
    LV_UNUSED(var);
    LV_UNUSED(value);
}

static void _activity_mark_visible(eos_activity_t *activity)
{
    g_activity_ctx.visible_activity = activity;
    g_activity_ctx.transition_in_progress = false;
}

static void _snapshot_img_delete_cb(lv_event_t *e)
{
    eos_activity_snapshot_node_t *snapshot_node = lv_event_get_user_data(e);
    if (!snapshot_node)
    {
        return;
    }

    if (snapshot_node->draw_buf)
    {
        lv_draw_buf_destroy(snapshot_node->draw_buf);
        snapshot_node->draw_buf = NULL;
    }

    if (snapshot_node->owner)
    {
        _activity_snapshot_release(snapshot_node->owner);
        snapshot_node->owner = NULL;
    }
}

static void _activity_snapshot_hold(eos_activity_t *activity)
{
    EOS_CHECK_PTR_RETURN(activity);
    if (activity->snapshot_ref_count < UINT8_MAX)
    {
        activity->snapshot_ref_count++;
    }

    if (activity->view)
    {
        lv_obj_add_flag(activity->view, LV_OBJ_FLAG_HIDDEN);
    }
}

static void _activity_snapshot_release(eos_activity_t *activity)
{
    EOS_CHECK_PTR_RETURN(activity);
    if (activity->snapshot_ref_count > 0)
    {
        activity->snapshot_ref_count--;
    }
}

static void _anim_clean_up_activity_deferred(void *user_data)
{
    eos_activity_anim_ctx_t *anim_ctx = (eos_activity_anim_ctx_t *)user_data;
    if (!anim_ctx)
    {
        return;
    }

    eos_activity_snapshot_node_t *node = anim_ctx->snapshots;
    while (node)
    {
        eos_activity_snapshot_node_t *next = node->next;
        if (node->snapshot_obj && lv_obj_is_valid(node->snapshot_obj))
        {
            lv_obj_delete(node->snapshot_obj);
        }
        eos_free(node);
        node = next;
    }
    anim_ctx->snapshots = NULL;

    // 如果需要销毁 from activity
    if (anim_ctx->destroy_from && anim_ctx->from)
    {
        // 在删除View之前，恢复header的父级对象
        if (!eos_activity_is_app_header_visible(anim_ctx->to))
        {
            // 从有header进入无header，动画完成后隐藏header
            eos_app_header_hide();
        }
        _activity_run_destroy(anim_ctx->from);
        anim_ctx->from = NULL;
    }
    else if (!eos_activity_is_app_header_visible(anim_ctx->to) && eos_activity_is_app_header_visible(anim_ctx->from))
    {
        // 从有header进入无header，且不需要销毁from activity，动画完成后隐藏header
        eos_app_header_hide();
    }

    if (anim_ctx->to && anim_ctx->to->snapshot_ref_count == 0)
    {
        _activity_show(anim_ctx->to);
    }
    if (anim_ctx->to && eos_activity_is_app_header_visible(anim_ctx->to))
    {
        eos_app_header_show(anim_ctx->to);
    }

    if (anim_ctx->at)
    {
        lv_anim_timeline_delete(anim_ctx->at);
        anim_ctx->at = NULL;
    }

    _activity_mark_visible(anim_ctx->to);
    eos_free(anim_ctx);
}

static void _activity_switch_to(eos_activity_t *next_activity)
{
    EOS_CHECK_PTR_RETURN(next_activity);
    eos_activity_t *cur_activity = g_activity_ctx.current_activity;
    if (cur_activity == next_activity)
    {
        _activity_mark_visible(next_activity);
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
    if (cur_activity && cur_activity->lifecycle && cur_activity->lifecycle->on_destroy)
    {
        cur_activity->lifecycle->on_destroy(cur_activity);
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

    // 处理AppHeader逻辑
    if (eos_activity_is_app_header_visible(next_activity))
    {
        // NextView有app_header
        if (cur_activity)
        {
            bool need_anim = false;
            bool reverse_anim = false;

            // 从有header进入有header才有动画
            if (eos_activity_is_app_header_visible(cur_activity))
            {
                need_anim = true;

                // 检查是否是back操作
                if (cur_activity->destroy_on_exit)
                {
                    reverse_anim = true;
                }
            }
            // 从无header进入有header无需动画

            _play_title_changed_anim(cur_activity, next_activity, need_anim, reverse_anim);
        }
    }
    else
    {
        // NextView没有app_header，将header附加到当前View
        if (cur_activity && cur_activity->view && lv_obj_is_valid(cur_activity->view) && eos_activity_is_app_header_visible(cur_activity))
        {
            // 只有从有header进入无header时，才将header附加到当前View
            eos_app_header_attach_to_view(cur_activity->view);
            // 不立即隐藏header，让它随View执行动画
            // eos_app_header_hide()会在动画完成后调用
        }
        else
        {
            // 从无header进入无header，直接隐藏header
            eos_app_header_hide();
        }
    }

    eos_activity_anim_cb_t anim_cb = NULL;
    if (cur_activity)
    {
        anim_cb = eos_activity_get_anim_route(cur_activity->type, next_activity->type);
    }

    // 如果需要动画，创建动画上下文并启动动画
    bool transition_started = false;
    if (cur_activity && anim_cb)
    {
        eos_activity_anim_ctx_t *anim_ctx = eos_malloc_zeroed(sizeof(eos_activity_anim_ctx_t));
        if (anim_ctx)
        {
            anim_ctx->at = lv_anim_timeline_create();
            if (!anim_ctx->at)
            {
                eos_free(anim_ctx);
                anim_ctx = NULL;
            }
        }

        if (anim_ctx)
        {
            anim_ctx->from = cur_activity;
            anim_ctx->to = next_activity;
            anim_ctx->destroy_from = cur_activity ? cur_activity->destroy_on_exit : false;

            g_activity_ctx.transition_in_progress = true;
            transition_started = true;
            _init_anim_timeline(anim_ctx);
            g_activity_ctx.active_anim_ctx = anim_ctx;
            g_activity_ctx.snapshot_capture_window = true;
            anim_cb(anim_ctx->at, cur_activity, next_activity);
            g_activity_ctx.snapshot_capture_window = false;
            g_activity_ctx.active_anim_ctx = NULL;
            _anim_timeline_start(cur_activity, next_activity, anim_ctx);
        }
    }

    // 显示 next_activity（在动画启动之后显示，确保动画的初始位置设置不会被覆盖）
    // _activity_show(next_activity); // Commented out to delay showing next_activity

    // 如果没有动画但需要销毁 from activity，直接销毁
    if (!transition_started)
    {
        if (cur_activity && cur_activity->destroy_on_exit)
        {
            // 在删除View之前，恢复header的父级对象
            if (!eos_activity_is_app_header_visible(next_activity))
            {
                // 从有header进入无header，直接隐藏header
                eos_app_header_hide();
            }
            _activity_run_destroy(cur_activity);
        }
        else if (!eos_activity_is_app_header_visible(next_activity) && cur_activity && eos_activity_is_app_header_visible(cur_activity))
        {
            // 从有header进入无header，且不需要销毁from activity，直接隐藏header
            eos_app_header_hide();
        }

        _activity_show(next_activity);
        if (eos_activity_is_app_header_visible(next_activity))
        {
            eos_app_header_show(next_activity);
        }
        _activity_mark_visible(next_activity);
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
    lv_obj_update_layout(view);

    return view;
}

static void _anim_clean_up_activity(lv_anim_t *a)
{
    if (!a)
        return;

    eos_activity_anim_ctx_t *anim_ctx = (eos_activity_anim_ctx_t *)lv_anim_get_user_data(a);
    if (!anim_ctx)
        return;

    if (anim_ctx->cleanup_scheduled)
    {
        return;
    }

    anim_ctx->cleanup_scheduled = true;
    eos_dispatcher_call(_anim_clean_up_activity_deferred, anim_ctx);
}

static void _init_anim_timeline(eos_activity_anim_ctx_t *anim_ctx)
{
    if (!anim_ctx)
    {
        return;
    }

    lv_anim_init(&anim_ctx->dummy_anim);
    lv_anim_set_var(&anim_ctx->dummy_anim, lv_screen_active());
    lv_anim_set_exec_cb(&anim_ctx->dummy_anim, _anim_dummy_exec_cb);
    lv_anim_set_values(&anim_ctx->dummy_anim, 0, 100);
    lv_anim_set_delay(&anim_ctx->dummy_anim, 1);
    lv_anim_set_completed_cb(&anim_ctx->dummy_anim, _anim_clean_up_activity);
    lv_anim_set_user_data(&anim_ctx->dummy_anim, anim_ctx);
}

static void _anim_timeline_start(eos_activity_t *from, eos_activity_t *to, eos_activity_anim_ctx_t *anim_ctx)
{
    LV_UNUSED(from);
    LV_UNUSED(to);

    if (!(anim_ctx && anim_ctx->at))
    {
        return;
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

    // 隐藏 header
    eos_app_header_hide();

    // 启动动画
    lv_anim_timeline_start(anim_ctx->at);
}

void eos_activity_set_type(eos_activity_t *activity, eos_activity_type_t type)
{
    EOS_CHECK_PTR_RETURN(activity);
    if (type <= EOS_ACTIVITY_TYPE_NULL || type >= EOS_ACTIVITY_TYPE_COUNT)
    {
        EOS_LOG_W("Invalid activity type: %d", type);
        activity->type = EOS_ACTIVITY_TYPE_NULL;
        return;
    }

    activity->type = type;
}

eos_activity_type_t eos_activity_get_type(eos_activity_t *activity)
{
    EOS_CHECK_PTR_RETURN_VAL(activity, EOS_ACTIVITY_TYPE_NULL);
    return activity->type;
}

eos_result_t eos_activity_register_anim_route(eos_activity_type_t from_type,
                                              eos_activity_type_t to_type,
                                              eos_activity_anim_cb_t cb)
{
    if (from_type <= EOS_ACTIVITY_TYPE_NULL || from_type >= EOS_ACTIVITY_TYPE_COUNT ||
        to_type <= EOS_ACTIVITY_TYPE_NULL || to_type >= EOS_ACTIVITY_TYPE_COUNT ||
        cb == NULL)
    {
        EOS_LOG_W("Invalid route register request: from=%d to=%d cb=%p", from_type, to_type, cb);
        return EOS_FAILED;
    }

    if (g_anim_callback_routes[from_type][to_type] != NULL)
    {
        EOS_LOG_W("Animation route duplicated: from=%d to=%d, overwrite", from_type, to_type);
    }

    g_anim_callback_routes[from_type][to_type] = cb;
    return EOS_OK;
}

eos_activity_anim_cb_t eos_activity_get_anim_route(eos_activity_type_t from_type,
                                                   eos_activity_type_t to_type)
{
    if (from_type <= EOS_ACTIVITY_TYPE_NULL || from_type >= EOS_ACTIVITY_TYPE_COUNT ||
        to_type <= EOS_ACTIVITY_TYPE_NULL || to_type >= EOS_ACTIVITY_TYPE_COUNT)
    {
        return NULL;
    }

    return g_anim_callback_routes[from_type][to_type];
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

lv_obj_t *eos_activity_take_snapshot(eos_activity_t *activity, bool include_header)
{
#if LV_USE_SNAPSHOT
    EOS_CHECK_PTR_RETURN_VAL(activity, NULL);

    if (!(g_activity_ctx.snapshot_capture_window && g_activity_ctx.active_anim_ctx))
    {
        EOS_LOG_W("eos_activity_take_snapshot is only allowed in animation callback");
        return NULL;
    }

    lv_obj_t *view = activity->view;
    if (!(view && lv_obj_is_valid(view)))
    {
        return NULL;
    }

    eos_activity_snapshot_node_t *snapshot_node = eos_malloc_zeroed(sizeof(eos_activity_snapshot_node_t));
    if (!snapshot_node)
    {
        return NULL;
    }

    lv_obj_t *snapshot_obj = lv_image_create(lv_layer_top());
    if (!snapshot_obj)
    {
        eos_free(snapshot_node);
        return NULL;
    }

#if _DEBUG_SNAPSHOT
    lv_obj_set_style_image_recolor(snapshot_obj,lv_color_hex(0xFF0000),0);
    lv_obj_set_style_image_recolor_opa(snapshot_obj, LV_OPA_20, 0);
#endif /* _DEBUG_SNAPSHOT */

    lv_draw_buf_t *snapshot = eos_draw_buf_create(
        (uint32_t)lv_obj_get_width(view),
        (uint32_t)lv_obj_get_height(view),
        _SNAPSHOT_COLOR_FORMAT,
        0);
    if (!snapshot)
    {
        lv_obj_delete(snapshot_obj);
        eos_free(snapshot_node);
        return NULL;
    }

#if EOS_APP_HEADER_ENABLE
    bool prev_header_visible = eos_app_header_is_visible();
    eos_activity_t *prev_visible_activity = eos_activity_get_visible();
    bool need_attach_header = include_header && activity->is_app_header_visible;
    if (need_attach_header)
    {
        eos_app_header_show(activity);
        eos_app_header_attach_to_view(view);
    }

    lv_result_t snapshot_result = lv_snapshot_take_to_draw_buf(view, _SNAPSHOT_COLOR_FORMAT, snapshot);

    if (need_attach_header)
    {
        eos_app_header_detach_from_view();

        if (prev_header_visible)
        {
            if (prev_visible_activity)
            {
                eos_app_header_show(prev_visible_activity);
            }
            else
            {
                eos_app_header_show(eos_activity_get_current());
            }
        }
        else
        {
            eos_app_header_hide();
        }
    }
#else
    LV_UNUSED(include_header);
    lv_result_t snapshot_result = lv_snapshot_take_to_draw_buf(view, _SNAPSHOT_COLOR_FORMAT, snapshot);
#endif

    if (snapshot_result != LV_RESULT_OK)
    {
        lv_draw_buf_destroy(snapshot);
        lv_obj_delete(snapshot_obj);
        eos_free(snapshot_node);
        return NULL;
    }

    snapshot_node->snapshot_obj = snapshot_obj;
    snapshot_node->draw_buf = snapshot;
    snapshot_node->owner = activity;
    snapshot_node->next = g_activity_ctx.active_anim_ctx->snapshots;
    g_activity_ctx.active_anim_ctx->snapshots = snapshot_node;

    lv_image_set_src(snapshot_obj, snapshot);
    lv_obj_set_pos(snapshot_obj, lv_obj_get_x(view), lv_obj_get_y(view));
    lv_obj_add_event_cb(snapshot_obj, _snapshot_img_delete_cb, LV_EVENT_DELETE, snapshot_node);
    _activity_snapshot_hold(activity);
    return snapshot_obj;
#else
    LV_UNUSED(activity);
    LV_UNUSED(include_header);
    EOS_LOG_W("LV_USE_SNAPSHOT disabled");
    return NULL;
#endif
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
        activity->title.string = (char *)eos_strdup(title);
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

    // 检查是否是Watchface Activity，如果是则不能设置为可见
    if (visible && activity == eos_activity_get_watchface())
    {
        EOS_LOG_D("Cannot set app header visible for watchface activity");
        return;
    }

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
    g_activity_ctx.visible_activity = initial_activity;
    g_activity_ctx.transition_in_progress = false;

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
    eos_activity_t *activity = eos_malloc_zeroed(sizeof(eos_activity_t));
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
    activity->type = EOS_ACTIVITY_TYPE_APP;
    activity->snapshot_ref_count = 0;
    activity->is_app_header_visible = false;
    activity->destroy_on_exit = false;
    activity->title.color = _DEFAULT_TITLE_COLOR;
    activity->title.type = _TITLE_TYPE_INVALID;
    activity->title.string = NULL;
    activity->user_data = NULL;

    return activity;
}

void eos_activity_enter(eos_activity_t *activity)
{
    EOS_CHECK_PTR_RETURN(activity);
    if (!_controller_initialized())
    {
        return;
    }

    if (g_activity_ctx.transition_in_progress)
    {
        EOS_LOG_W("Activity transition in progress");
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

    if (g_activity_ctx.transition_in_progress)
    {
        EOS_LOG_W("Activity transition in progress");
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

eos_activity_t *eos_activity_get_visible(void)
{
    if (!_controller_initialized())
    {
        return NULL;
    }
    if (g_activity_ctx.visible_activity)
        return g_activity_ctx.visible_activity;
    return eos_activity_get_current();
}

bool eos_activity_is_transition_in_progress(void)
{
    return g_activity_ctx.transition_in_progress;
}

eos_activity_t *eos_activity_get_bottom(void)
{
    if (!_controller_initialized())
    {
        return NULL;
    }
    return g_activity_ctx.watchface_activity;
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
