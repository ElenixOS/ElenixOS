/**
 * @file eos_activity.h
 * @brief Activity controller
 */

#ifndef ELENIX_OS_ACTIVITY_H
#define ELENIX_OS_ACTIVITY_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"
#include "eos_core.h"
#include "eos_lang.h"
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
 * @brief Initialize Activity controller
 * @param initial_activity Initial Activity, cannot be NULL
 * @return eos_result_t EOS_OK success, EOS_FAILED failed
 */
eos_result_t eos_activity_controller_init(eos_activity_t *initial_activity);

/**
 * @brief Deinitialize Activity controller and release all Activities
 */
void eos_activity_controller_deinit(void);

/**
 * @brief Create an Activity
 * @return eos_activity_t* Returns Activity pointer on success, NULL on failure
 */
eos_activity_t *eos_activity_create(const eos_activity_lifecycle_t *lifecycle);

/**
 * @brief Get Activity user data
 * @param activity Activity pointer
 * @return void* User data pointer, returns NULL on failure
 */
void *eos_activity_get_user_data(eos_activity_t *activity);

/**
 * @brief Set Activity user data
 * @param activity Activity pointer
 * @param user_data User data pointer
 */
void eos_activity_set_user_data(eos_activity_t *activity, void *user_data);

/**
 * @brief Get Activity title
 * @param activity Activity pointer
 * @return const char* Title string, returns NULL on failure
 */
const char *eos_activity_get_title(eos_activity_t *activity);

/**
 * @brief Set Activity title
 * @param activity Activity pointer
 * @param title Title string
 */
void eos_activity_set_title(eos_activity_t *activity, const char *title);

/**
 * @brief Set Activity title
 * @param activity Activity pointer
 * @param id Title string ID
 */
void eos_activity_set_title_id(eos_activity_t *activity, lang_string_id_t id);

/**
 * @brief Get Activity title color
 * @param activity Activity pointer
 * @return lv_color_t Title color
 */
lv_color_t eos_activity_get_title_color(eos_activity_t *activity);

/**
 * @brief Set Activity title color
 * @param activity Activity pointer
 * @param color Title color
 */
void eos_activity_set_title_color(eos_activity_t *activity, lv_color_t color);

/**
 * @brief Set Activity page type
 * @param activity Activity pointer
 * @param type Page type
 */
void eos_activity_set_type(eos_activity_t *activity, eos_activity_type_t type);

/**
 * @brief Get Activity page type
 * @param activity Activity pointer
 * @return eos_activity_type_t Page type
 */
eos_activity_type_t eos_activity_get_type(eos_activity_t *activity);

/**
 * @brief Register page transition animation route
 * @param from_type Source page type
 * @param to_type Target page type
 * @param cb Animation callback
 * @return eos_result_t EOS_OK success, EOS_FAILED failed
 */
eos_result_t eos_activity_register_anim_route(eos_activity_type_t from_type,
                                              eos_activity_type_t to_type,
                                              eos_activity_anim_cb_t cb);

/**
 * @brief Query page transition animation route
 * @param from_type Source page type
 * @param to_type Target page type
 * @return eos_activity_anim_cb_t Animation callback, returns NULL if not found
 */
eos_activity_anim_cb_t eos_activity_get_anim_route(eos_activity_type_t from_type,
                                                   eos_activity_type_t to_type);

/**
 * @brief Set Activity title visibility
 * @param activity Activity pointer
 * @param visible Whether visible
 */
void eos_activity_set_app_header_visible(eos_activity_t *activity, bool visible);

/**
 * @brief Set Activity title visibility with animation
 * @param activity Activity pointer
 * @param visible Whether visible
 * @param duration_ms Animation duration (milliseconds), switches immediately when 0
 */
void eos_activity_set_app_header_visible_animated(eos_activity_t *activity, bool visible, uint32_t duration_ms);

/**
 * @brief Check if Activity title is visible
 * @param activity Activity pointer
 * @return bool true visible, false not visible
 */
bool eos_activity_is_app_header_visible(eos_activity_t *activity);

/**
 * @brief Set whether Activity AppHeader shows only time label
 * @param activity Activity pointer
 * @param time_only true shows only time, false shows full AppHeader
 */
void eos_activity_set_app_header_time_only(eos_activity_t *activity, bool time_only);

/**
 * @brief Check if Activity AppHeader shows only time label
 * @param activity Activity pointer
 * @return bool true shows only time, false shows full AppHeader
 */
bool eos_activity_is_app_header_time_only(eos_activity_t *activity);

/**
 * @brief Set Activity time font color in AppHeader time-only mode
 * @param activity Activity pointer
 * @param color Time font color
 */
void eos_activity_set_app_header_time_only_text_color(eos_activity_t *activity, lv_color_t color);

/**
 * @brief Get Activity time font color in AppHeader time-only mode
 * @param activity Activity pointer
 * @return lv_color_t Time font color
 */
lv_color_t eos_activity_get_app_header_time_only_text_color(eos_activity_t *activity);

/**
 * @brief Get Activity corresponding View
 * @param activity Activity pointer
 * @return lv_obj_t* View object, returns NULL on failure
 */
lv_obj_t *eos_activity_get_view(eos_activity_t *activity);

/**
 * @brief Set Activity View
 * @param activity Activity pointer
 * @param view View object, returns NULL on failure
 */
void eos_activity_set_view(eos_activity_t *activity, lv_obj_t *view);

/**
 * @brief Get root Screen
 * @return lv_obj_t* Root Screen object, returns NULL on failure
 */
lv_obj_t *eos_activity_get_root_screen(void);

/**
 * @brief Get Activity view snapshot
 * @param activity Activity pointer
 * @param include_header Whether to include Header (only valid when current Activity and Header is visible)
 * @return lv_obj_t* Snapshot image object (lv_image), returns NULL on failure
 * @note Image resources are automatically released when the object is deleted
 */
lv_obj_t *eos_activity_take_snapshot(eos_activity_t *activity, bool include_header);

/**
 * @brief Get Watchface Activity
 * @return eos_activity_t* Watchface Activity pointer, returns NULL on failure
 */
eos_activity_t *eos_activity_get_watchface(void);

/**
 * @brief Get current Activity View
 * @return lv_obj_t* Current Activity View object, returns NULL on failure
 */
lv_obj_t *eos_view_active(void);

/**
 * @brief Enter specified Activity
 * @param activity Activity pointer
 */
void eos_activity_enter(eos_activity_t *activity);

/**
 * @brief Return to previous Activity and destroy current Activity
 * @return eos_result_t EOS_OK success, EOS_FAILED failed
 */
eos_result_t eos_activity_back(void);

/**
 * @brief Return directly to watchface Activity and clear all stacked Activities
 * @return eos_result_t EOS_OK success, EOS_FAILED failed
 */
eos_result_t eos_activity_back_to_watchface(void);

/**
 * @brief Wrapper for returning to previous Activity and destroying current Activity
 * @param e Event object
 */
void eos_activity_back_cb(lv_event_t *e);

/**
 * @brief Get current Activity
 * @return eos_activity_t* Current Activity, returns NULL on failure
 */
eos_activity_t *eos_activity_get_current(void);

/**
 * @brief Get current completed display Activity
 * @return eos_activity_t* Completed display Activity, returns NULL on failure
 */
eos_activity_t *eos_activity_get_visible(void);

/**
 * @brief Get previous Activity (used in event callbacks to get the source page)
 * @return eos_activity_t* Previous Activity, returns NULL on failure
 */
eos_activity_t *eos_activity_get_previous(void);

/**
 * @brief Check if Activity transition animation is in progress
 * @return bool true transitioning, false idle
 */
bool eos_activity_is_transition_in_progress(void);

/**
 * @brief Get bottom Activity in stack (usually watchface Activity)
 * @return eos_activity_t* Bottom Activity in stack, returns NULL on failure
 */
eos_activity_t *eos_activity_get_bottom(void);

#ifdef __cplusplus
}
#endif

#endif /* ELENIX_OS_ACTIVITY_H */
