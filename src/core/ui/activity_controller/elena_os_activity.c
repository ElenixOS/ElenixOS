/**
 * @file elena_os_activity.c
 * @brief Activity 管理
 * @author Sab1e
 * @date 2026-03-21
 */

#include "elena_os_activity.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "elena_os_stack.h"
#include "elena_os_mem.h"
#include "elena_os_core.h"
#include "elena_os_config.h"
#include "elena_os_theme.h"
/* Macros and Definitions -------------------------------------*/
#define _ACTIVITY_STACK_INIT_CAPACITY 8

struct eos_activity_t
{
    lv_obj_t *view;

    eos_activity_on_create_t on_create;
    eos_activity_on_entry_t on_entry;
    eos_activity_on_exit_t on_exit;
    eos_activity_on_destroy_t on_destroy;
};

typedef struct
{
    eos_stack_t *activity_stack;
    lv_obj_t *root_screen;
} eos_activity_ctx_t;

/* Variables --------------------------------------------------*/
static eos_activity_ctx_t g_activity_ctx = {
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
    if (!activity)
    {
        return;
    }

    if (activity->on_destroy)
    {
        activity->on_destroy();
    }

    if (activity->view)
    {
        lv_obj_delete(activity->view);
        activity->view = NULL;
    }

    eos_free(activity);
}

static void _activity_reset_context(void)
{
    g_activity_ctx.activity_stack = NULL;
    g_activity_ctx.root_screen = NULL;
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
    lv_obj_set_size(view, EOS_DISPLAY_WIDTH, EOS_DISPLAY_HEIGHT);
    lv_obj_set_pos(view, 0, 0);
    lv_obj_set_style_bg_color(view, EOS_COLOR_BLACK, 0);
    lv_obj_set_style_border_width(view, 0, 0);

    return view;
}

eos_result_t eos_activity_controller_init(eos_activity_t *initial_activity)
{
    if (!initial_activity)
    {
        return EOS_FAILED;
    }

    if (_controller_initialized())
    {
        eos_activity_controller_deinit();
    }

    // 获取根 Screen
    if (lv_screen_active())
    {
        // 删除活动 Screen，创建空 Screen
        lv_obj_delete(lv_screen_active());
    }
    g_activity_ctx.root_screen = lv_obj_create(NULL);
    lv_screen_load(g_activity_ctx.root_screen);
    // 创建 Activity 栈
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

    // 确保初始 Activity 的 view 挂载在 root_screen 下
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

    // 将初始 Activity 压入栈顶
    if (!eos_stack_push(g_activity_ctx.activity_stack, initial_activity))
    {
        eos_stack_destroy(g_activity_ctx.activity_stack);
        lv_obj_delete(g_activity_ctx.root_screen);
        _activity_reset_context();
        return EOS_FAILED;
    }

    if (initial_activity->on_entry)
    {
        initial_activity->on_entry();
    }
    lv_obj_move_foreground(initial_activity->view);

    return EOS_OK;
}

eos_activity_t *eos_activity_create(eos_activity_on_create_t on_create,
                                    eos_activity_on_entry_t on_entry,
                                    eos_activity_on_exit_t on_exit,
                                    eos_activity_on_destroy_t on_destroy)
{
    if (!_controller_initialized())
    {
        return NULL;
    }

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
    activity->on_create = on_create;
    activity->on_entry = on_entry;
    activity->on_exit = on_exit;
    activity->on_destroy = on_destroy;

    if (activity->on_create)
    {
        activity->on_create();
    }

    return activity;
}

void eos_activity_enter(eos_activity_t *activity)
{
    if (!activity || !_controller_initialized())
    {
        return;
    }

    eos_activity_t *cur_activity = eos_stack_peek(g_activity_ctx.activity_stack);
    if (cur_activity == activity)
    {
        lv_obj_move_foreground(activity->view);
        return;
    }

    // 进入新 Activity
    if (!eos_stack_push(g_activity_ctx.activity_stack, activity))
    {
        return;
    }

    if (cur_activity && cur_activity->on_exit)
    {
        cur_activity->on_exit();
    }

    if (activity->on_entry)
    {
        activity->on_entry();
    }
    lv_obj_move_foreground(activity->view);
}

eos_result_t eos_activity_back(void)
{
    if (!_controller_initialized())
    {
        return EOS_FAILED;
    }

    if (eos_stack_get_size(g_activity_ctx.activity_stack) <= 1)
    {
        return EOS_FAILED;
    }

    eos_activity_t *current = eos_stack_pop(g_activity_ctx.activity_stack);
    eos_activity_t *prev = eos_stack_peek(g_activity_ctx.activity_stack);

    if (!current || !prev)
    {
        return EOS_FAILED;
    }

    if (current->on_exit)
    {
        current->on_exit();
    }
    _activity_run_destroy(current);

    if (prev->on_entry)
    {
        prev->on_entry();
    }
    if (prev->view)
    {
        lv_obj_move_foreground(prev->view);
    }

    return EOS_OK;
}

eos_activity_t *eos_activity_get_current(void)
{
    if (!_controller_initialized())
    {
        return NULL;
    }

    return eos_stack_peek(g_activity_ctx.activity_stack);
}

lv_obj_t *eos_activity_get_view(eos_activity_t *activity)
{
    if (!activity)
    {
        return NULL;
    }

    return activity->view;
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

    eos_stack_destroy(g_activity_ctx.activity_stack);

    if (g_activity_ctx.root_screen)
    {
        lv_obj_delete(g_activity_ctx.root_screen);
    }

    _activity_reset_context();
}



void eos_activity_destroy(eos_activity_t *activity)
{
    _activity_run_destroy(activity);
}
