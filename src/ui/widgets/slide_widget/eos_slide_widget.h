/**
 * @file eos_slide_widget.h
 * @brief Slide widget
 */

#ifndef EOS_SLIDE_WIDGET_H
#define EOS_SLIDE_WIDGET_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"
#include "eos_utils.h"

/* Public macros ----------------------------------------------*/
#define EOS_THRESHOLD_SCALE 256
/* Public typedefs --------------------------------------------*/

typedef uint32_t eos_threshold_t;

enum{
    EOS_THRESHOLD_0      = 0,
    EOS_THRESHOLD_10     = 25,
    EOS_THRESHOLD_20     = 51,
    EOS_THRESHOLD_30     = 76,
    EOS_THRESHOLD_40     = 102,
    EOS_THRESHOLD_50     = 127,
    EOS_THRESHOLD_60     = 153,
    EOS_THRESHOLD_70     = 178,
    EOS_THRESHOLD_80     = 204,
    EOS_THRESHOLD_100    = 255,
    EOS_THRESHOLD_INFINITE = INT_MAX,
};

typedef enum
{
    EOS_SLIDE_DIR_VER,
    EOS_SLIDE_DIR_HOR,
} eos_slide_widget_dir_t;

typedef enum
{
    EOS_SLIDE_WIDGET_STATE_IDLE = 0,      /**< Idle, not sliding */
    EOS_SLIDE_WIDGET_STATE_DRAGGING,  /**< Currently sliding (gesture drag) */
    EOS_SLIDE_WIDGET_STATE_THRESHOLD, /**< Exceeded threshold, slide confirmed (execute expand/trigger operation) */
    EOS_SLIDE_WIDGET_STATE_REVERTING, /**< Reverting (auto return when threshold not exceeded) */
    EOS_SLIDE_WIDGET_STATE_ANIMATING, /**< Manually triggered animation */
    EOS_SLIDE_WIDGET_STATE_OPEN,      /**< Panel fully open */
} eos_slide_widget_state_t;

typedef struct
{
    lv_obj_t *touch_obj;
    lv_obj_t *target_obj;
    eos_slide_widget_dir_t dir;
    lv_coord_t base;   /**< Base point: default position of target object */
    lv_coord_t target; /**< Target point: final position for target object to move to */
    lv_coord_t touch_obj_base;
    lv_coord_t touch_obj_target;
    eos_threshold_t threshold; /**< Threshold: trigger when (current - base)/(target - base) > threshold */
    eos_slide_widget_state_t state;
    eos_slide_widget_state_t settle_state; /**< Settling state after current animation completes */
    lv_coord_t _indev_start;            /**< Touch start point */
    lv_coord_t last_touch_displacement; /**< Previous movement displacement */
    bool bidirectional;                 /**< Whether to support bidirectional sliding */
    bool move_foreground_on_pressed;    /**< Whether to move to foreground when pressed, default on, auto off when parent is `lv_list_class` */
    bool reversed;      /**< Whether to reverse direction */
    bool owns_touch_obj; /**< Whether owns and is responsible for releasing touch_obj */
} eos_slide_widget_t;

/* Public function prototypes --------------------------------*/

/**
 * @brief Move from start position to end position
 * @param sw Target slide widget
 * @param start Start coordinate
 * @param end End coordinate
 * @param duration Duration, set to 0 for no animation
 */
void eos_slide_widget_move(eos_slide_widget_t *sw, lv_coord_t start, lv_coord_t end, uint32_t duration);
/**
 * @brief Reverse the `target` and `base` positions of the target object
 * @param sw Target slide widget
 */
void eos_slide_widget_reverse(eos_slide_widget_t *sw);
/**
 * @brief Create slide widget
 * @param parent        Parent object of the touch object
 * @param target_obj    Target object (will slide following the touch area)
 * @param dir           Slide direction (only supports vertical `EOS_SLIDE_DIR_VER` and horizontal `EOS_SLIDE_DIR_HOR`)
 * @param target        Target coordinate point (`target` refers to y-axis when `dir` is vertical; x-axis when horizontal)
 *
 * When the threshold `abs(current_coord - touch_start_coord)/abs(target-base)` is exceeded,
 * the full pull-out animation is triggered, automatically pulling from the target object's current position to target position.
 * @param threshold Threshold (0~255) see `EOS_THRESHOLD_XXX`
 * @return eos_slide_widget_t* Returns slide widget object on success, NULL on failure
 * @warning Do not use `lv_obj_center` on the target object, otherwise coordinate movement will be chaotic.
 */
eos_slide_widget_t *eos_slide_widget_create(
    lv_obj_t *parent,
    lv_obj_t *target_obj,
    eos_slide_widget_dir_t dir,
    lv_coord_t target,
    eos_threshold_t threshold);
/**
 * @brief Create slide widget (without creating touch object internally)
 * @param touch_obj Touch object
 */
eos_slide_widget_t *eos_slide_widget_create_with_touch(
    lv_obj_t *touch_obj,
    lv_obj_t *target_obj,
    eos_slide_widget_dir_t dir,
    lv_coord_t target,
    eos_threshold_t threshold);
/**
 * @brief Set whether to support bidirectional sliding
 */
void eos_slide_widget_set_bidirectional(eos_slide_widget_t *sw, bool enable);
void eos_slide_widget_set_move_foreground_on_pressed(eos_slide_widget_t *sw, bool enable);
void eos_slide_widget_set_anim_transition(eos_slide_widget_t *sw,
                                          eos_slide_widget_state_t transit_state,
                                          eos_slide_widget_state_t settle_state);
void eos_slide_widget_delete(eos_slide_widget_t *sw);
#ifdef __cplusplus
}
#endif

#endif /* EOS_SLIDE_WIDGET_H */
