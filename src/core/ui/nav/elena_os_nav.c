/**
 * @file elena_os_nav.c
 * @brief 导航栈
 * @author Sab1e
 * @date 2025-08-24
 */

#include "elena_os_nav.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#define EOS_LOG_DISABLE
#define EOS_LOG_TAG "Navigation"
#include "elena_os_log.h"
#include "elena_os_basic_widgets.h"
#include "elena_os_theme.h"
#include "elena_os_app_list.h"
#include "elena_os_watchface.h"
#include "elena_os_port.h"
#include "elena_os_scene.h"
#include "elena_os_event.h"
#include "elena_os_app_header.h"

/* Macros and Definitions -------------------------------------*/
#define NAV_INITIAL_CAPACITY 16  // 初始容量
#define NAV_CAPACITY_GROWTH 2    // 容量增长因子
#define NAV_HOME_SCREEN_INDEX 0
#define NAV_SEMAPHORE_TIMEOUT 0 // 信号量获取超时时间(ms)

/**
 * @brief 导航栈结构体
 */
typedef struct
{
    lv_obj_t **stack;           // 动态分配的页面指针数组
    lv_obj_t *launcher_screen;
    int32_t top;                // 栈顶索引
    int32_t capacity;           // 当前栈容量
    bool initialized;
    eos_sem_t *semaphore;
} eos_nav_stack_t;

/* Variables --------------------------------------------------*/
static eos_nav_stack_t eos_nav = {.stack = NULL, .top = -1, .capacity = 0, .initialized = false, .semaphore = NULL};

/* Function Implementations -----------------------------------*/
static lv_obj_t *_eos_nav_peek_prev(void);
bool eos_nav_get_initialized(void);
static bool _is_eos_nav_stack_full(void);
static bool _is_eos_nav_stack_empty(void);
eos_result_t eos_nav_back_clean(void);
static eos_result_t _eos_nav_clear_stack(void);
static eos_result_t _eos_nav_sem_take(void);
static void _eos_nav_sem_give(void);
static eos_result_t _eos_nav_stack_expand(void);

/**
 * @brief 扩展栈容量
 */
static eos_result_t _eos_nav_stack_expand(void)
{
    int32_t new_capacity = eos_nav.capacity * NAV_CAPACITY_GROWTH;
    if (new_capacity < NAV_INITIAL_CAPACITY) {
        new_capacity = NAV_INITIAL_CAPACITY;
    }

    lv_obj_t **new_stack = eos_realloc(eos_nav.stack, new_capacity * sizeof(lv_obj_t *));
    if (new_stack == NULL) {
        EOS_LOG_E("Failed to expand nav stack to capacity %d", new_capacity);
        return -EOS_ERR_MEM;
    }

    // 初始化新分配的内存
    for (int32_t i = eos_nav.capacity; i < new_capacity; i++) {
        new_stack[i] = NULL;
    }

    eos_nav.stack = new_stack;
    eos_nav.capacity = new_capacity;
    EOS_LOG_D("Nav stack expanded to capacity %d", new_capacity);
    return EOS_OK;
}

/**
 * @brief 获取导航栈信号量
 * @return 成功返回EOS_OK，超时返回-EOS_ERR_TIMEOUT
 */
static eos_result_t _eos_nav_sem_take(void)
{
    if (eos_nav.semaphore == NULL)
    {
        EOS_LOG_E("Nav semaphore not created");
        return -EOS_ERR_NOT_INITIALIZED;
    }

    if (!eos_sem_take(eos_nav.semaphore, NAV_SEMAPHORE_TIMEOUT))
    {
        EOS_LOG_E("Take nav semaphore timeout");
        return -EOS_ERR_TIMEOUT;
    }

    return EOS_OK;
}

/**
 * @brief 释放导航栈信号量
 */
static void _eos_nav_sem_give(void)
{
    if (eos_nav.semaphore != NULL)
    {
        eos_sem_give(eos_nav.semaphore);
    }
}

/**
 * @brief 检查导航栈是否已初始化
 */
bool eos_nav_get_initialized(void)
{
    return eos_nav.initialized;
}

/**
 * @brief 检查导航栈是否已满
 */
static bool _is_eos_nav_stack_full(void)
{
    return eos_nav.top >= eos_nav.capacity - 1;
}

/**
 * @brief 检查导航栈是否为空
 */
static bool _is_eos_nav_stack_empty(void)
{
    return eos_nav.top < 0;
}

/**
 * @brief 获取前一个页面对象
 */
static lv_obj_t *_eos_nav_peek_prev(void)
{
    EOS_LOG_D("NAV PREV");
    return (eos_nav.top > 0) ? eos_nav.stack[eos_nav.top - 1] : NULL;
}

lv_obj_t *eos_nav_get_home_screen(void)
{
    return eos_nav.top >= NAV_HOME_SCREEN_INDEX ? eos_nav.stack[NAV_HOME_SCREEN_INDEX] : NULL;
}

/**
 * @brief 清除整个导航栈
 */
eos_result_t eos_nav_clean_up(void)
{
    eos_result_t ret = _eos_nav_sem_take();
    if (ret != EOS_OK)
    {
        return ret;
    }

    if (!eos_nav_get_initialized())
    {
        EOS_LOG_E("Nav stack not initialized");
        _eos_nav_sem_give();
        return -EOS_ERR_NOT_INITIALIZED;
    }

    // 加载 launcher_screen
    eos_screen_load(eos_nav.launcher_screen);

    // 从栈顶向下清理所有screen
    for (int32_t i = eos_nav.top; i >= 0; i--)
    {
        if (eos_nav.stack[i] != NULL)
        {
            if (lv_obj_is_valid(eos_nav.stack[i]))
            {
                EOS_LOG_D("Screen[%p]-[%d] Clearing", eos_nav.stack[i], i);
                lv_obj_delete(eos_nav.stack[i]); // 彻底删除screen
                EOS_LOG_D("Screen[%p] Cleared", eos_nav.stack[i]);
            }
            else
            {
                EOS_LOG_W("Screen[%p] Is not a valid obj, ignored.", eos_nav.stack[i]);
            }
            eos_nav.stack[i] = NULL; // 清除指针
        }
    }

    // 释放栈内存
    if (eos_nav.stack != NULL) {
        eos_free(eos_nav.stack);
        eos_nav.stack = NULL;
    }

    eos_nav.launcher_screen = NULL;
    eos_nav.top = -1;
    eos_nav.capacity = 0;
    eos_nav.initialized = false;

    // 销毁信号量
    if (eos_nav.semaphore != NULL)
    {
        eos_sem_destroy(eos_nav.semaphore);
        eos_nav.semaphore = NULL;
    }
    eos_scene_back();
    EOS_LOG_D("Nav stack completely cleared.");
    eos_event_broadcast(EOS_EVENT_NAVIGATION_CLEAN_UP, NULL);
    return EOS_OK;
}

lv_obj_t *eos_nav_init(lv_obj_t *launcher_screen)
{
    EOS_CHECK_PTR_RETURN_VAL(launcher_screen, NULL);

    if (eos_nav_get_initialized())
    {
        eos_nav_clean_up();
    }

    // 创建信号量
    eos_nav.semaphore = eos_sem_create(1, 1);
    if (eos_nav.semaphore == NULL)
    {
        EOS_LOG_E("Create nav semaphore failed");
        return NULL;
    }

    // 创建home_screen（脚本的根页面）
    lv_obj_t *home_screen = eos_screen_create();
    if (!home_screen)
    {
        EOS_LOG_E("Create root screen failed.");
        eos_sem_destroy(eos_nav.semaphore);
        eos_nav.semaphore = NULL;
        return NULL;
    }

    // 初始化动态栈
    eos_nav.stack = eos_malloc(NAV_INITIAL_CAPACITY * sizeof(lv_obj_t *));
    if (eos_nav.stack == NULL) {
        EOS_LOG_E("Failed to allocate nav stack");
        lv_obj_delete(home_screen);
        eos_sem_destroy(eos_nav.semaphore);
        eos_nav.semaphore = NULL;
        return NULL;
    }

    // 初始化栈内存
    for (int32_t i = 0; i < NAV_INITIAL_CAPACITY; i++) {
        eos_nav.stack[i] = NULL;
    }

    eos_nav.launcher_screen = launcher_screen;
    eos_nav.stack[NAV_HOME_SCREEN_INDEX] = home_screen;
    eos_nav.top = NAV_HOME_SCREEN_INDEX;
    eos_nav.capacity = NAV_INITIAL_CAPACITY;
    eos_nav.initialized = true;
    // eos_app_header_set_title_anim(launcher_screen, home_screen, true);

    eos_scene_switch(EOS_SCENE_NAVIGATION);
    eos_event_broadcast(EOS_EVENT_NAVIGATION_INIT, NULL);

    EOS_LOG_D("Launcher screen = %p", eos_nav.launcher_screen);
    EOS_LOG_D("Home screen = %p", home_screen);
    EOS_LOG_D("Nav stack initialized with capacity %d", eos_nav.capacity);

    return home_screen;
}

/**
 * @brief 创建新的导航页面
 */
lv_obj_t *eos_nav_scr_create(void)
{
    eos_result_t ret = _eos_nav_sem_take();
    if (ret != EOS_OK)
    {
        return NULL;
    }

    if (!eos_nav_get_initialized())
    {
        EOS_LOG_E("Nav stack not initialized");
        _eos_nav_sem_give();
        return NULL;
    }

    // 检查是否需要扩展栈容量
    if (_is_eos_nav_stack_full())
    {
        ret = _eos_nav_stack_expand();
        if (ret != EOS_OK) {
            EOS_LOG_E("Failed to expand nav stack");
            _eos_nav_sem_give();
            return NULL;
        }
    }

    lv_obj_t *scr = eos_screen_create();
    if (!scr)
    {
        EOS_LOG_E("Create screen failed.");
        _eos_nav_sem_give();
        return NULL;
    }

    // 确保新屏幕与栈中已有屏幕地址不同
    for (int32_t i = 0; i <= eos_nav.top; i++)
    {
        if (eos_nav.stack[i] == scr)
        {
            EOS_LOG_E("New screen address conflicts with existing screen!");
            lv_obj_delete(scr);
            _eos_nav_sem_give();
            return NULL;
        }
    }

    eos_app_header_set_title_anim(eos_nav.stack[eos_nav.top], scr, true);
    EOS_LOG_D("NAV PUSH: new screen at %p", scr);
    eos_nav.stack[++eos_nav.top] = scr;
    EOS_MEM("Create new scr");
    _eos_nav_sem_give();
    eos_event_broadcast(EOS_EVENT_NAVIGATION_ENTER_NEW, NULL);
    return scr;
}

/**
 * @brief 返回并清理当前页面
 */
eos_result_t eos_nav_back_clean(void)
{
    eos_result_t ret = _eos_nav_sem_take();
    if (ret != EOS_OK)
    {
        return ret;
    }

    if (!eos_nav_get_initialized())
    {
        EOS_LOG_E("Nav stack not initialized");
        _eos_nav_sem_give();
        return -EOS_ERR_NOT_INITIALIZED;
    }

    if (_is_eos_nav_stack_empty())
    {
        EOS_LOG_E("Nav stack empty (cannot back from root screen)");
        _eos_nav_sem_give();
        return -EOS_ERR_STACK_EMPTY;
    }

    eos_app_header_set_title_anim(eos_nav.stack[eos_nav.top], eos_nav.stack[eos_nav.top - 1], false);

    // 如果当前在home_screen（top==0），则清理整个栈
    if (eos_nav.top == NAV_HOME_SCREEN_INDEX)
    {
        _eos_nav_sem_give(); // 先释放信号量，因为eos_nav_clean_up内部会获取

        // 停止脚本引擎
        if (script_engine_get_state() == SCRIPT_STATE_RUNNING ||
            script_engine_get_state() == SCRIPT_STATE_SUSPEND)
        {
            script_engine_request_stop();
        }

        eos_nav_clean_up();

        EOS_LOG_D("Nav Exit");
        EOS_MEM("Mem check: eos_nav_back_clean");
        return EOS_OK;
    }

    // 其他页面正常返回逻辑
    lv_obj_t *prev_scr = _eos_nav_peek_prev();
    if (!prev_scr)
    {
        prev_scr = eos_nav.stack[NAV_HOME_SCREEN_INDEX]; // 回退到 home_screen
    }

    // 保存要删除的页面
    lv_obj_t *scr_to_del = eos_nav.stack[eos_nav.top];
    eos_nav.stack[eos_nav.top] = NULL;
    eos_nav.top--; // 更新栈指针

    // 删除页面
    if (scr_to_del)
    {
        lv_obj_delete(scr_to_del);
        EOS_LOG_D("Deleted screen at %p", scr_to_del);
    }

    // 加载前一个屏幕
    eos_screen_load(prev_scr);

    EOS_MEM("Clear scr");
    _eos_nav_sem_give();
    eos_event_broadcast(EOS_EVENT_NAVIGATION_BACK_PREV, NULL);
    return EOS_OK;
}

/**
 * @brief 返回但不删除当前页面
 */
eos_result_t eos_nav_back(void)
{
    eos_result_t ret = _eos_nav_sem_take();
    if (ret != EOS_OK)
    {
        return ret;
    }

    if (!eos_nav_get_initialized())
    {
        EOS_LOG_E("Nav stack not initialized");
        _eos_nav_sem_give();
        return -EOS_ERR_NOT_INITIALIZED;
    }

    if (_is_eos_nav_stack_empty())
    {
        EOS_LOG_E("Already at root screen, cannot go back");
        _eos_nav_sem_give();
        return -EOS_ERR_STACK_EMPTY;
    }

    eos_app_header_set_title_anim(eos_nav.stack[eos_nav.top], eos_nav.stack[eos_nav.top - 1], false);

    lv_obj_t *prev_scr = _eos_nav_peek_prev();
    if (!prev_scr)
    {
        prev_scr = eos_nav.stack[NAV_HOME_SCREEN_INDEX]; // 回退到 base screen
    }

    eos_nav.top--; // 更新栈指针
    eos_screen_load(prev_scr);

    _eos_nav_sem_give();
    eos_event_broadcast(EOS_EVENT_NAVIGATION_BACK_PREV, NULL);
    return EOS_OK;
}
