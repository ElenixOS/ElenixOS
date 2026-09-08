/**
 * @file eos_overlay_layer.h
 * @brief Fixed LVGL top-layer slot registry
 */
#ifndef EOS_OVERLAY_LAYER_H
#define EOS_OVERLAY_LAYER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include "lvgl.h"

/* Public typedefs --------------------------------------------*/

/**
 * @brief Fixed semantic slots in the ElenixOS top-layer graph
 *
 * Values are ordering keys. They are not LVGL child indices and must not be
 * persisted. Each real slot is backed by one permanent LVGL container.
 */
typedef enum
{
    EOS_TOP_LAYER_USER_BEGIN = 0,
    EOS_TOP_LAYER_USER_APP,
    EOS_TOP_LAYER_USER_END = 999,

    EOS_TOP_LAYER_SNAPSHOT_BEGIN = 1000,
    EOS_TOP_LAYER_ACTIVITY_SNAPSHOT,
    EOS_TOP_LAYER_SNAPSHOT_END = 1999,

    EOS_TOP_LAYER_HEADER_BEGIN = 2000,
    EOS_TOP_LAYER_APP_HEADER,
    EOS_TOP_LAYER_HEADER_END = 2999,

    /** Global interaction UI sits above application content and below system UI. */
    EOS_TOP_LAYER_GLOBAL_BEGIN = 3000,
    EOS_TOP_LAYER_MSG_LIST,
    EOS_TOP_LAYER_CONTROL_CENTER,
    EOS_TOP_LAYER_FLASHLIGHT,
    EOS_TOP_LAYER_GLOBAL_END = 3999,

    /** System UI is above global interaction UI. */
    EOS_TOP_LAYER_SYSTEM_BEGIN = 4000,
    EOS_TOP_LAYER_SYSTEM_TOUCH_STATUS,
    EOS_TOP_LAYER_SYSTEM_TOUCH_MARKER,
    EOS_TOP_LAYER_SYSTEM_TOUCH_DIAGNOSTIC,
    EOS_TOP_LAYER_SYSTEM_TOAST,
    EOS_TOP_LAYER_SYSTEM_CROWN,
    EOS_TOP_LAYER_SYSTEM_OBJS,
    EOS_TOP_LAYER_SYSTEM_FPS,
    EOS_TOP_LAYER_SYSTEM_ESH_CMD,
    EOS_TOP_LAYER_SYSTEM_ERROR,
    EOS_TOP_LAYER_SYSTEM_END = 4999,

    EOS_TOP_LAYER_MODAL_BEGIN = 5000,
    EOS_TOP_LAYER_MESSAGE_DETAIL,
    EOS_TOP_LAYER_PERMISSION,
    EOS_TOP_LAYER_MODAL_END = 5999,

    EOS_TOP_LAYER_SECURITY_BEGIN = 6000,
    EOS_TOP_LAYER_LOCK,
    EOS_TOP_LAYER_SECURITY_END = 6999,

    EOS_TOP_LAYER_DEBUG_BEGIN = 7000,
    EOS_TOP_LAYER_DEBUG_TEST,
    EOS_TOP_LAYER_DEBUG_END = 7999,
} eos_top_layer_slot_t;

/* Public function prototypes ---------------------------------*/

/**
 * @brief Initialize the fixed top-layer graph
 */
void eos_overlay_layer_init(void);

/**
 * @brief Get the permanent container for a semantic top-layer slot
 * @param slot Semantic layer slot
 * @return Slot container, or NULL when the slot is invalid or uninitialized
 */
lv_obj_t *eos_overlay_layer_get(eos_top_layer_slot_t slot);

/**
 * @brief Check whether a value identifies a real top-layer slot
 * @param slot Candidate slot value
 * @return true when the value identifies a real slot
 */
bool eos_overlay_layer_is_valid_slot(eos_top_layer_slot_t slot);

#ifdef __cplusplus
}
#endif

#endif /* EOS_OVERLAY_LAYER_H */
