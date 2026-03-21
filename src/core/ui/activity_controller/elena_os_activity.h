/**
 * @file elena_os_activity.h
 * @brief 头文件说明
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

typedef void (*eos_activity_on_create_t)(void);
typedef void (*eos_activity_on_entry_t)(void);
typedef void (*eos_activity_on_exit_t)(void);
typedef void (*eos_activity_on_destroy_t)(void);

/* Public typedefs --------------------------------------------*/

typedef struct eos_activity_t eos_activity_t;

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
eos_activity_t *eos_activity_create(eos_activity_on_create_t on_create,
									eos_activity_on_entry_t on_entry,
									eos_activity_on_exit_t on_exit,
									eos_activity_on_destroy_t on_destroy);

/**
 * @brief 进入指定 Activity
 * @param activity Activity 指针
 */
void eos_activity_enter(eos_activity_t *activity);

/**
 * @brief 返回上一个 Activity
 * @return eos_result_t EOS_OK 成功，EOS_FAILED 失败
 */
eos_result_t eos_activity_back(void);

/**
 * @brief 获取当前 Activity
 * @return eos_activity_t* 当前 Activity，失败返回 NULL
 */
eos_activity_t *eos_activity_get_current(void);

/**
 * @brief 获取 Activity 对应的 View
 * @param activity Activity 指针
 * @return lv_obj_t* View 对象，失败返回 NULL
 */
lv_obj_t *eos_activity_get_view(eos_activity_t *activity);

/**
 * @brief 销毁 Activity
 * @param activity Activity 指针
 */
void eos_activity_destroy(eos_activity_t *activity);

#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_ACTIVITY_H */
