/**
 * @file elena_os_event.c
 * @brief 事件系统
 * @author Sab1e
 * @date 2025-08-16
 */

#include "elena_os_event.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "lvgl.h"
#define EOS_LOG_DISABLE
#define EOS_LOG_TAG "EventSystem"
#include "elena_os_log.h"
#include "elena_os_port.h"
#include "elena_os_config.h"
#include "lvgl_private.h"
/* Macros and Definitions -------------------------------------*/
/**
 * @brief 事件回调节点结构
 */
typedef struct _event_node_t
{
    lv_obj_t *obj;
    lv_event_code_t event;
    lv_event_cb_t cb;
    void *user_data;
    struct _event_node_t *next;
    bool marked_for_delete;
    bool is_global;
} event_node_t;
/* Variables --------------------------------------------------*/
static event_node_t *event_list_head = NULL; // 事件链表头
static int g_broadcast_depth = 0;             // 嵌套广播计数（>0 表示正在广播）
static bool g_event_list_modified = false;    // 在广播期间，如果链表被修改（节点被标记删除），置为 true

/* Function Implementations -----------------------------------*/

#if EOS_COMPILE_MODE == DEBUG
void _event_list_show(void)
{
    event_node_t *curr = event_list_head;
    EOS_LOG_I("Event list:");
    while (curr)
    {
        EOS_LOG_I(" current: [%p] obj[%p] event[%d] cb[%p] marked[%d] global[%d]",
                  curr, curr->obj, (int)curr->event, (void *)curr->cb,
                  curr->marked_for_delete ? 1 : 0, curr->is_global ? 1 : 0);
        curr = curr->next;
    }
}
#endif /* EOS_COMPILE_MODE */

/**
 * @brief 将匹配的节点标记为删除，延时清理
 */
static void _mark_node_deleted_by_predicate(bool (*pred)(event_node_t *, void *), void *ctx)
{
    for (event_node_t *n = event_list_head; n; n = n->next)
    {
        if (pred(n, ctx))
        {
            if (!n->marked_for_delete)
            {
                n->marked_for_delete = true;
                g_event_list_modified = true;
                EOS_LOG_D("Marked node [%p] obj[%p] event[%d] cb[%p] global[%d] for deletion",
                         n, n->obj, (int)n->event, (void *)n->cb, n->is_global ? 1 : 0);
            }
        }
    }
}

/**
 * @brief 立即清理被标记的节点；在非广播时也可被调用以立即释放
 */
static void _cleanup_deleted_nodes(void)
{
    event_node_t **curr = &event_list_head;
    while (*curr)
    {
        if ((*curr)->marked_for_delete)
        {
            event_node_t *tmp = *curr;
            *curr = (*curr)->next;

            EOS_LOG_D("Freeing node [%p] obj[%p] event[%d] cb[%p] global[%d]",
                     tmp, tmp->obj, (int)tmp->event, (void *)tmp->cb, tmp->is_global ? 1 : 0);

            eos_free(tmp);
            g_event_list_modified = true;
        }
        else
        {
            curr = &(*curr)->next;
        }
    }
}

static bool _pred_match_obj(event_node_t *n, void *ctx)
{
    lv_obj_t *obj = ctx;
    return (n->obj == obj && !n->is_global); // 只匹配非全局回调
}

static bool _pred_match_global_cb(event_node_t *n, void *ctx)
{
    lv_event_cb_t cb = ctx;
    return (n->is_global && n->cb == cb);
}

static bool _pred_match_global_event_cb(event_node_t *n, void *ctx)
{
    struct {
        lv_event_code_t event;
        lv_event_cb_t cb;
    } *params = ctx;

    return (n->is_global && n->event == params->event && n->cb == params->cb);
}

static void _obj_delete_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    EOS_LOG_D("_obj_delete_cb: obj [%p]", obj);

    if (g_broadcast_depth > 0)
    {
        // 深度大于0，说明广播嵌套，标记删除
        _mark_node_deleted_by_predicate(_pred_match_obj, obj);
    }
    else
    {
        // 深度小于0，直接删除节点即可
        _mark_node_deleted_by_predicate(_pred_match_obj, obj);
        _cleanup_deleted_nodes();
    }
}

void eos_event_add_cb(lv_obj_t *obj, lv_event_cb_t cb, lv_event_code_t event, void *user_data)
{
    EOS_CHECK_PTR_RETURN(obj && cb);
    EOS_LOG_I("Event %d add callback: [%p], obj: [%p]", (int)event, (void *)cb, (void *)obj);

    event_node_t *new_node = eos_malloc(sizeof(event_node_t));
    if (!new_node)
    {
        EOS_LOG_E("Failed to allocate event node");
        return;
    }

    new_node->obj = obj;
    new_node->event = event;
    new_node->cb = cb;
    new_node->user_data = user_data;
    new_node->next = NULL;
    new_node->marked_for_delete = false;
    new_node->is_global = false;

    // 添加到链表头部
    new_node->next = event_list_head;
    event_list_head = new_node;

    // 向LVGL注册事件回调
    lv_obj_add_event_cb(obj, cb, event, user_data);

    // 当对象被删除时自动清理删除此回调
    lv_obj_add_event_cb(obj, _obj_delete_cb, LV_EVENT_DELETE, NULL);
}

void eos_event_add_global_cb(lv_event_cb_t cb, lv_event_code_t event, void *user_data)
{
    EOS_CHECK_PTR_RETURN(cb);
    EOS_LOG_I("Add global event callback: event[%d] cb[%p]", (int)event, (void *)cb);

    event_node_t *new_node = eos_malloc(sizeof(event_node_t));
    if (!new_node)
    {
        EOS_LOG_E("Failed to allocate global event node");
        return;
    }

    new_node->obj = NULL;
    new_node->event = event;
    new_node->cb = cb;
    new_node->user_data = user_data;
    new_node->next = NULL;
    new_node->marked_for_delete = false;
    new_node->is_global = true;

    // 添加到链表头部
    new_node->next = event_list_head;
    event_list_head = new_node;

    EOS_LOG_D("Global event callback added successfully");
}

void eos_event_remove_cb(lv_obj_t *obj, lv_event_code_t event, lv_event_cb_t cb)
{
    EOS_LOG_I("Remove callback manual: obj[%p] event[%d] cb[%p] broadcasting=%d",
              (void *)obj, (int)event, (void *)cb, g_broadcast_depth);

    event_node_t **curr = &event_list_head;

    while (*curr)
    {
        event_node_t *n = *curr;
        bool match = false;

        if (obj == NULL)
        {
            // 移除全局回调：匹配事件类型和回调函数
            match = (n->is_global && n->event == event && n->cb == cb);
        }
        else
        {
            // 移除对象回调：匹配对象、事件类型和回调函数
            match = (!n->is_global && n->obj == obj && n->event == event && n->cb == cb);
        }

        if (match)
        {
            if (g_broadcast_depth > 0)
            {
                // 正在广播：只标记删除
                n->marked_for_delete = true;
                g_event_list_modified = true;
                EOS_LOG_D("Marked node [%p] for deletion (during broadcast)", n);

                // 如果是对象回调，还需要从LVGL中移除
                if (!n->is_global && n->obj)
                {
                    lv_obj_remove_event_cb(n->obj, cb);
                }
            }
            else
            {
                // 非广播：立即从链表移除并释放
                *curr = n->next;

                // 如果是对象回调，还需要从LVGL中移除
                if (!n->is_global && n->obj)
                {
                    lv_obj_remove_event_cb(n->obj, cb);
                }

                EOS_LOG_D("Immediately freeing node [%p] (manual remove)", n);
                eos_free(n);
            }
            return;
        }
        curr = &(*curr)->next;
    }

    EOS_LOG_W("Callback not found for removal: obj[%p] event[%d] cb[%p]",
              (void *)obj, (int)event, (void *)cb);
}

inline void eos_event_remove_global_cb(lv_event_code_t event, lv_event_cb_t cb)
{
    eos_event_remove_cb(NULL, event, cb);
}

void eos_event_broadcast(lv_event_code_t event, void *param)
{
    EOS_LOG_I("Broadcast event: [%d] (begin) depth=%d", (int)event, g_broadcast_depth + 1);
    // 进入新的广播层级
    g_broadcast_depth++;
    bool local_list_was_modified = false;
    event_node_t *curr = event_list_head;
    while (curr)
    {
        event_node_t *next = curr->next;

        EOS_LOG_D("Broadcast visiting node [%p] obj[%p] event[%d] marked[%d] global[%d]",
                 curr, curr->obj, (int)curr->event, curr->marked_for_delete ? 1 : 0, curr->is_global ? 1 : 0);

        if (!curr->marked_for_delete && curr->event == event)
        {
            if (curr->is_global)
            {
                // 处理全局回调：直接调用回调函数
                EOS_LOG_D("Calling global callback [%p] for event [%d]", (void *)curr->cb, (int)event);

                // 创建模拟的事件对象供回调函数使用
                lv_event_t e;
                e.code = event;
                e.param = param;
                e.user_data = curr->user_data;
                e.current_target = NULL;
                e.original_target = NULL;

                curr->cb(&e);

                if (g_event_list_modified)
                    local_list_was_modified = true;
            }
            else if (curr->obj && lv_obj_is_valid(curr->obj) && lv_obj_has_class(curr->obj, &lv_obj_class))
            {
                // 处理对象绑定回调：通过LVGL发送事件
                lv_obj_t *target = curr->obj;
                lv_result_t res = lv_obj_send_event(target, event, param);
                if (res != LV_RESULT_OK)
                {
                    EOS_LOG_W("Failed to send event %d to obj [%p]", (int)event, (void *)target);
                }
                if (g_event_list_modified)
                    local_list_was_modified = true;
            }
        }

        curr = next;
    }

    // 离开广播层级
    g_broadcast_depth--;

    // 只有当最外层广播完成（depth == 0）时，才真正清理标记节点
    if (g_broadcast_depth == 0)
    {
        if (g_event_list_modified || local_list_was_modified)
        {
            EOS_LOG_D("Cleaning up marked nodes after broadcast");
            _cleanup_deleted_nodes();
            g_event_list_modified = false;
        }
    }

    EOS_LOG_I("Broadcast event: [%d] (end) depth=%d", (int)event, g_broadcast_depth);
}

void eos_event_cleanup_now(void)
{
    if (g_broadcast_depth > 0)
    {
        EOS_LOG_W("Cleanup requested during broadcast; marking only.");
        g_event_list_modified = true;
        return;
    }
    _cleanup_deleted_nodes();
    g_event_list_modified = false;
}

void eos_event_remove_all_global_cbs(lv_event_cb_t cb)
{
    EOS_CHECK_PTR_RETURN(cb);
    EOS_LOG_I("Remove all global callbacks for cb[%p]", (void *)cb);

    if (g_broadcast_depth > 0)
    {
        _mark_node_deleted_by_predicate(_pred_match_global_cb, cb);
    }
    else
    {
        _mark_node_deleted_by_predicate(_pred_match_global_cb, cb);
        _cleanup_deleted_nodes();
    }
}
