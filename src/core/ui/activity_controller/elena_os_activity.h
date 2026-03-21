/**
 * @file elena_os_activity.h
 * @brief Activity 控制器
 * @author Sab1e
 * @date 2026-03-21
 */

#ifndef ELENA_OS_ACTIVITY_H
#define ELENA_OS_ACTIVITY_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"
#include "elena_os_core.h"
/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/
typedef struct eos_activity_t eos_activity_t;

typedef void (*eos_activity_on_enter_t)(eos_activity_t *activity);
typedef void (*eos_activity_on_exit_t)(eos_activity_t *activity);

/* Public function prototypes --------------------------------*/

/**
 * @brief 初始化 Activity 控制器
 * @param initial_activity 初始 Activity，不能为空
 * @return eos_result_t EOS_OK 成功，EOS_FAILED 失败
 */
eos_result_t eos_activity_controller_init(eos_activity_t *initial_activity);

/**
 * @brief 反初始化 Activity 控制器并释放所有 Activity
 */
void eos_activity_controller_deinit(void);

/**
 * @brief 创建一个 Activity
 * @return eos_activity_t* 创建成功返回 Activity 指针，失败返回 NULL
 */
eos_activity_t *eos_activity_create(eos_activity_on_enter_t on_enter,
									eos_activity_on_exit_t on_exit);

/**
 * @brief 获取 Activity 的用户数据
 * @param activity Activity 指针
 * @return void* 用户数据指针，失败返回 NULL
 */
void *eos_activity_get_user_data(eos_activity_t *activity);

/**
 * @brief 设置 Activity 的用户数据
 * @param activity Activity 指针
 * @param user_data 用户数据指针
 */
void eos_activity_set_user_data(eos_activity_t *activity, void *user_data);

/**
 * @brief 获取 Activity 对应的 View
 * @param activity Activity 指针
 * @return lv_obj_t* View 对象，失败返回 NULL
 */
lv_obj_t *eos_activity_get_view(eos_activity_t *activity);

/**
 * @brief 设置 Activity 的 View
 * @param activity Activity 指针
 * @param view View 对象，失败返回 NULL
 */
void eos_activity_set_view(eos_activity_t *activity, lv_obj_t *view);

/**
 * @brief 获取根 Screen
 * @return lv_obj_t* 根 Screen 对象，失败返回 NULL
 */
lv_obj_t *eos_activity_get_root_screen(void);

/**
 * @brief 获取 Watchface Activity
 * @return eos_activity_t* Watchface Activity 指针，失败返回 NULL
 */
eos_activity_t *eos_activity_get_watchface(void);

/**
 * @brief 获取当前 Activity 的 View
 * @return lv_obj_t* 当前 Activity 的 View 对象，失败返回 NULL
 */
lv_obj_t *eos_view_active(void);

/**
 * @brief 进入指定 Activity
 * @param activity Activity 指针
 */
void eos_activity_enter(eos_activity_t *activity);

/**
 * @brief 返回上一个 Activity 并销毁当前 Activity
 * @return eos_result_t EOS_OK 成功，EOS_FAILED 失败
 */
eos_result_t eos_activity_back(void);

/**
 * @brief 返回上一个 Activity 并销毁当前 Activity 的 Wrapper
 * @param e 事件对象
 */
void eos_activity_back_cb(lv_event_t *e);

/**
 * @brief 获取当前 Activity
 * @return eos_activity_t* 当前 Activity，失败返回 NULL
 */
eos_activity_t *eos_activity_get_current(void);

/**
 * @brief 获取栈底 Activity（通常是表盘 Activity）
 * @return eos_activity_t* 栈底 Activity，失败返回 NULL
 */
eos_activity_t *eos_activity_get_bottom(void);

#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_ACTIVITY_H */
