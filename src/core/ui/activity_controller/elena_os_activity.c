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
/* Macros and Definitions -------------------------------------*/
#define _ACTIVITY_STACK_INIT_CAPACITY 8

struct eos_activity_t
{
    lv_obj_t *view;

    eos_activity_on_enter_t on_enter;
    eos_activity_on_exit_t on_exit;

    void* user_data;
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
        return;
    }

    if (cur_activity && cur_activity->on_exit)
    {
        cur_activity->on_exit(cur_activity);
    }

    if (next_activity->on_enter)
    {
        next_activity->on_enter(next_activity);
    }

    _activity_show(next_activity);
    g_activity_ctx.current_activity = next_activity;
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

    if (initial_activity->on_enter)
    {
        initial_activity->on_enter(initial_activity);
    }
    _activity_show(initial_activity);
    g_activity_ctx.current_activity = initial_activity;

    return EOS_OK;
}

eos_activity_t *eos_activity_create(eos_activity_on_enter_t on_enter,
                                    eos_activity_on_exit_t on_exit)
{
    eos_activity_t *activity = eos_malloc(sizeof(eos_activity_t));
    if (!activity)
    {
        return NULL;
    }

    activity->view = _view_create(g_activity_ctx.root_screen);
    if(!activity->view)
    {
        eos_free(activity);
        return NULL;
    }
    activity->on_enter = on_enter;
    activity->on_exit = on_exit;

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

    if (current->on_exit)
    {
        current->on_exit(current);
    }

    g_activity_ctx.current_activity = NULL;
    _activity_run_destroy(current);

    if (eos_stack_get_size(g_activity_ctx.activity_stack) == 0)
    {
        if (g_activity_ctx.watchface_activity)
        {
            _activity_switch_to(g_activity_ctx.watchface_activity);
            return EOS_OK;
        }
        return EOS_FAILED;
    }

    eos_activity_t *prev = eos_stack_peek(g_activity_ctx.activity_stack);
    EOS_CHECK_PTR_RETURN_VAL(prev, EOS_FAILED);

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
