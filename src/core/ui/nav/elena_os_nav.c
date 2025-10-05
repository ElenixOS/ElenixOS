/**
 * @file elena_os_nav.c
 * @brief 导航栈
 * @author Sab1e
 * @date 2025-08-24
 */

#include "elena_os_nav.h"

// Includes
#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>

#include "elena_os_log.h"
#include "elena_os_basic_widgets.h"
#include "elena_os_theme.h"
#include "elena_os_app_list.h"
// Macros and Definitions
#define NAV_STACK_SIZE 32
#define NAV_HOME_SCREEN_INDEX 0
/**
 * @brief 导航栈结构体
 */
typedef struct
{
    lv_obj_t *stack[NAV_STACK_SIZE];
    lv_obj_t *launcher_screen;
    int8_t top;
    bool initialized;
} eos_nav_stack_t;

/**
 * @brief 导航正忙，等待响应
 */
#define NAV_BUSY_WAIT()                                                       \
    do                                                                        \
    {                                                                         \
        bool expected = false;                                                \
        while (!atomic_compare_exchange_weak(&eos_nav_busy, &expected, true)) \
        {                                                                     \
            expected = false;                                                 \
            EOS_LOG_D("Waiting for eos_nav stack to be available");           \
            lv_timer_handler();                                               \
        }                                                                     \
    } while (0)

/**
 * @brief 导航正忙，取消操作
 */
#define NAV_BUSY_CHECK()                                                     \
    do                                                                       \
    {                                                                        \
        bool expected = false;                                               \
        if (!atomic_compare_exchange_strong(&eos_nav_busy, &expected, true)) \
        {                                                                    \
            EOS_LOG_D("Nav stack busy, operation rejected");                 \
            return -EOS_ERR_BUSY;                                            \
        }                                                                    \
    } while (0)

// Variables
static eos_nav_stack_t eos_nav = {.top = -1, .initialized = false};
static atomic_bool eos_nav_busy = false; // 正在清除
// Function Implementations
static lv_obj_t *_eos_nav_peek_prev(void);
bool is_eos_nav_stack_initialized(void);
static bool _is_eos_nav_stack_full(void);
static bool _is_eos_nav_stack_empty(void);
eos_result_t eos_nav_back_clean(void);
static eos_result_t _eos_nav_clear_stack(void);
/**
 * @brief 检查导航栈是否已初始化
 */
bool is_eos_nav_stack_initialized(void)
{
    return eos_nav.initialized;
}

/**
 * @brief 检查导航栈是否已满
 */
static bool _is_eos_nav_stack_full(void)
{
    return eos_nav.top >= NAV_STACK_SIZE - 1;
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
    if (eos_nav_busy)
    {
        EOS_LOG_E("Nav stack busy");
        return -EOS_ERR_BUSY;
    }
    atomic_store(&eos_nav_busy, true);

    if (!is_eos_nav_stack_initialized())
    {
        EOS_LOG_E("Nav stack not initialized");
        atomic_store(&eos_nav_busy, false);
        return -EOS_ERR_NOT_INITIALIZED;
    }

    // 加载 launcher_screen
    lv_screen_load(eos_nav.launcher_screen);

    // 从栈顶向下清理所有screen
    for (int i = NAV_STACK_SIZE - 1; i >= 0; i--)
    {
        if (eos_nav.stack[i] != NULL)
        {
            lv_obj_delete(eos_nav.stack[i]); // 彻底删除screen
            EOS_LOG_D("Cleared screen at stack position %d, ptr: %p", i, eos_nav.stack[i]);
        }
        eos_nav.stack[i] = NULL; // 清除指针
    }
    eos_nav.launcher_screen = NULL;
    eos_nav.top = -1;
    eos_nav.initialized = false;
    atomic_store(&eos_nav_busy, false);

    EOS_LOG_D("Nav stack completely cleared.");
    return EOS_OK;
}

lv_obj_t *eos_nav_init(lv_obj_t *launcher_screen)
{
    EOS_CHECK_PTR_RETURN_VAL(launcher_screen, NULL);
    if (script_engine_get_current_script_type() == SCRIPT_TYPE_WATCHFACE)
    {
        EOS_LOG_E("Watchface can't use nav");
        return NULL;
    }
    if (is_eos_nav_stack_initialized())
    {
        eos_nav_clean_up();
    }

    // 创建home_screen（脚本的根页面）
    lv_obj_t *home_screen = lv_obj_create(NULL);
    if (!home_screen)
    {
        EOS_LOG_E("Create root screen failed.");
        return NULL;
    }

    // 初始化栈
    eos_nav.launcher_screen = launcher_screen;
    eos_nav.stack[NAV_HOME_SCREEN_INDEX] = home_screen;
    eos_nav.top = NAV_HOME_SCREEN_INDEX;
    eos_nav.initialized = true;

    lv_obj_add_style(home_screen, eos_theme_get_screen_style(), 0);

    EOS_LOG_D("Launcher screen = %p", eos_nav.launcher_screen);
    EOS_LOG_D("Home screen = %p", home_screen);

    EOS_LOG_D("Nav stack initialized: home_screen=%p, ", home_screen);
    return home_screen;
}

/**
 * @brief 创建新的导航页面
 */
lv_obj_t *eos_nav_scr_create(void)
{
    if (eos_nav_busy)
    {
        EOS_LOG_E("Nav stack busy");
        return NULL;
    }
    atomic_store(&eos_nav_busy, true);

    if (!is_eos_nav_stack_initialized())
    {
        EOS_LOG_E("Nav stack not initialized");
        return NULL;
    }

    if (_is_eos_nav_stack_full())
    {
        EOS_LOG_E("Nav stack full");
        return NULL;
    }

    lv_obj_t *scr = lv_obj_create(NULL);
    if (!scr)
    {
        EOS_LOG_E("Create screen failed.");
        return NULL;
    }
    lv_obj_add_style(scr, eos_theme_get_screen_style(), 0);
    // 确保新屏幕与栈中已有屏幕地址不同
    for (int i = 0; i <= eos_nav.top; i++)
    {
        if (eos_nav.stack[i] == scr)
        {
            EOS_LOG_E("New screen address conflicts with existing screen!");
            lv_obj_delete(scr);
            return NULL;
        }
    }

    EOS_LOG_D("NAV PUSH: new screen at %p", scr);
    eos_nav.stack[++eos_nav.top] = scr;
    EOS_MEM("Create new scr");
    atomic_store(&eos_nav_busy, false);
    return scr;
}

/**
 * @brief 返回并清理当前页面
 */
eos_result_t eos_nav_back_clean(void)
{
    NAV_BUSY_CHECK();

    if (!is_eos_nav_stack_initialized())
    {
        EOS_LOG_E("Nav stack not initialized");
        atomic_store(&eos_nav_busy, false);
        return -EOS_ERR_NOT_INITIALIZED;
    }

    if (_is_eos_nav_stack_empty())
    {
        EOS_LOG_E("Nav stack empty (cannot back from root screen)");
        atomic_store(&eos_nav_busy, false);
        return -EOS_ERR_STACK_EMPTY;
    }

    // 如果当前在home_screen（top==0），则清理整个栈
    if (eos_nav.top == NAV_HOME_SCREEN_INDEX)
    {
        atomic_store(&eos_nav_busy, false);
        // 停止脚本引擎
        if (script_engine_get_state() == SCRIPT_STATE_RUNNING ||
            script_engine_get_state() == SCRIPT_STATE_SUSPEND)
        {
            script_engine_request_stop(); // 内部会自动调用栈清理函数
        }
        else
        {
            eos_nav_clean_up();
        }
        eos_app_list_create();
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
    eos_nav.top--; // 更新栈指针

    // 删除页面
    if (scr_to_del)
    {
        lv_obj_delete(scr_to_del);
        EOS_LOG_D("Deleted screen at %p", scr_to_del);
    }

    // 加载前一个屏幕
    lv_screen_load(prev_scr);

    EOS_MEM("Clear scr");
    atomic_store(&eos_nav_busy, false);
    return EOS_OK;
}

/**
 * @brief 返回但不删除当前页面
 */
eos_result_t eos_nav_back(void)
{
    NAV_BUSY_CHECK();

    if (!is_eos_nav_stack_initialized())
    {
        EOS_LOG_E("Nav stack not initialized");
        atomic_store(&eos_nav_busy, false);
        return -EOS_ERR_NOT_INITIALIZED;
    }

    if (_is_eos_nav_stack_empty())
    {
        EOS_LOG_E("Already at root screen, cannot go back");
        atomic_store(&eos_nav_busy, false);
        return -EOS_ERR_STACK_EMPTY;
    }

    lv_obj_t *prev_scr = _eos_nav_peek_prev();
    if (!prev_scr)
    {
        prev_scr = eos_nav.stack[NAV_HOME_SCREEN_INDEX]; // 回退到 base screen
    }

    eos_nav.top--; // 更新栈指针
    lv_screen_load(prev_scr);

    atomic_store(&eos_nav_busy, false);
    return EOS_OK;
}
