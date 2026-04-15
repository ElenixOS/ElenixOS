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
#include "elena_os_lang.h"
/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/
typedef struct eos_activity_t eos_activity_t;

typedef enum
{
    EOS_ACTIVITY_TYPE_NULL = 0,
    EOS_ACTIVITY_TYPE_APP,
    EOS_ACTIVITY_TYPE_APP_LIST,
    EOS_ACTIVITY_TYPE_WATCHFACE,
    EOS_ACTIVITY_TYPE_WATCHFACE_LIST,
    EOS_ACTIVITY_TYPE_COUNT
} eos_activity_type_t;

typedef void (*eos_activity_on_enter_t)(eos_activity_t *activity);
typedef void (*eos_activity_on_destroy_t)(eos_activity_t *activity);
typedef void (*eos_activity_on_pause_t)(eos_activity_t *activity);
typedef void (*eos_activity_on_resume_t)(eos_activity_t *activity);
typedef void (*eos_activity_anim_cb_t)(lv_anim_timeline_t *at, eos_activity_t *this, eos_activity_t *next);

typedef struct
{
    eos_activity_on_enter_t on_enter;
    eos_activity_on_destroy_t on_destroy;
    eos_activity_on_pause_t on_pause;
    eos_activity_on_resume_t on_resume;
} eos_activity_lifecycle_t;

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
eos_activity_t *eos_activity_create(const eos_activity_lifecycle_t *lifecycle);

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
 * @brief 获取 Activity 的标题
 * @param activity Activity 指针
 * @return const char* 标题字符串，失败返回 NULL
 */
const char *eos_activity_get_title(eos_activity_t *activity);

/**
 * @brief 设置 Activity 的标题
 * @param activity Activity 指针
 * @param title 标题字符串
 */
void eos_activity_set_title(eos_activity_t *activity, const char *title);

/**
 * @brief 设置 Activity 的标题
 * @param activity Activity 指针
 * @param id 标题字符串ID
 */
void eos_activity_set_title_id(eos_activity_t *activity, lang_string_id_t id);

/**
 * @brief 获取 Activity 的标题颜色
 * @param activity Activity 指针
 * @return lv_color_t 标题颜色
 */
lv_color_t eos_activity_get_title_color(eos_activity_t *activity);

/**
 * @brief 设置 Activity 的标题颜色
 * @param activity Activity 指针
 * @param color 标题颜色
 */
void eos_activity_set_title_color(eos_activity_t *activity, lv_color_t color);

/**
 * @brief 设置 Activity 的页面类型
 * @param activity Activity 指针
 * @param type 页面类型
 */
void eos_activity_set_type(eos_activity_t *activity, eos_activity_type_t type);

/**
 * @brief 获取 Activity 的页面类型
 * @param activity Activity 指针
 * @return eos_activity_type_t 页面类型
 */
eos_activity_type_t eos_activity_get_type(eos_activity_t *activity);

/**
 * @brief 注册页面切换动画路由
 * @param from_type 来源页面类型
 * @param to_type 目标页面类型
 * @param cb 动画回调
 * @return eos_result_t EOS_OK 成功，EOS_FAILED 失败
 */
eos_result_t eos_activity_register_anim_route(eos_activity_type_t from_type,
                                              eos_activity_type_t to_type,
                                              eos_activity_anim_cb_t cb);

/**
 * @brief 查询页面切换动画路由
 * @param from_type 来源页面类型
 * @param to_type 目标页面类型
 * @return eos_activity_anim_cb_t 动画回调，未找到返回 NULL
 */
eos_activity_anim_cb_t eos_activity_get_anim_route(eos_activity_type_t from_type,
                                                   eos_activity_type_t to_type);

/**
 * @brief 设置 Activity 的标题可见性
 * @param activity Activity 指针
 * @param visible 是否可见
 */
void eos_activity_set_app_header_visible(eos_activity_t *activity, bool visible);

/**
 * @brief 以动画方式设置 Activity 的标题可见性
 * @param activity Activity 指针
 * @param visible 是否可见
 * @param duration_ms 动画时长（毫秒），为0时立即切换
 */
void eos_activity_set_app_header_visible_animated(eos_activity_t *activity, bool visible, uint32_t duration_ms);

/**
 * @brief 判断 Activity 的标题是否可见
 * @param activity Activity 指针
 * @return bool true 可见，false 不可见
 */
bool eos_activity_is_app_header_visible(eos_activity_t *activity);

/**
 * @brief 设置 Activity 的 AppHeader 是否仅显示时间标签
 * @param activity Activity 指针
 * @param time_only true 仅显示时间，false 显示完整 AppHeader
 */
void eos_activity_set_app_header_time_only(eos_activity_t *activity, bool time_only);

/**
 * @brief 判断 Activity 的 AppHeader 是否仅显示时间标签
 * @param activity Activity 指针
 * @return bool true 仅显示时间，false 显示完整 AppHeader
 */
bool eos_activity_is_app_header_time_only(eos_activity_t *activity);

/**
 * @brief 设置 Activity 在 AppHeader 仅时间模式下的时间字体颜色
 * @param activity Activity 指针
 * @param color 时间字体颜色
 */
void eos_activity_set_app_header_time_only_text_color(eos_activity_t *activity, lv_color_t color);

/**
 * @brief 获取 Activity 在 AppHeader 仅时间模式下的时间字体颜色
 * @param activity Activity 指针
 * @return lv_color_t 时间字体颜色
 */
lv_color_t eos_activity_get_app_header_time_only_text_color(eos_activity_t *activity);

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
 * @brief 获取 Activity 视图快照
 * @param activity Activity 指针
 * @param include_header 是否包含 Header（仅当前 Activity 且 Header 可见时有效）
 * @return lv_obj_t* 快照图像对象（lv_image），失败返回 NULL
 * @note 图像资源会在该对象删除时自动释放
 */
lv_obj_t *eos_activity_take_snapshot(eos_activity_t *activity, bool include_header);

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
 * @brief 获取当前已完成显示的 Activity
 * @return eos_activity_t* 已完成显示的 Activity，失败返回 NULL
 */
eos_activity_t *eos_activity_get_visible(void);

/**
 * @brief 获取前一个 Activity（用于事件回调中获取起源页面）
 * @return eos_activity_t* 前一个 Activity，失败返回 NULL
 */
eos_activity_t *eos_activity_get_previous(void);

/**
 * @brief 当前是否有 Activity 切换动画正在进行
 * @return bool true 正在切换，false 空闲
 */
bool eos_activity_is_transition_in_progress(void);

/**
 * @brief 获取栈底 Activity（通常是表盘 Activity）
 * @return eos_activity_t* 栈底 Activity，失败返回 NULL
 */
eos_activity_t *eos_activity_get_bottom(void);

#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_ACTIVITY_H */
