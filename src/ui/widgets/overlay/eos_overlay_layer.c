/**
 * @file eos_overlay_layer.c
 * @brief Fixed LVGL top-layer slot registry implementation
 */
#include "eos_overlay_layer.h"

/* Includes ---------------------------------------------------*/
#include <stddef.h>
#define EOS_LOG_TAG "OverlayLayer"
#include "eos_log.h"

/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/
static lv_obj_t *_top_root = NULL;
static bool _initialized = false;

static const eos_top_layer_slot_t _slot_order[] = {
    EOS_TOP_LAYER_USER_APP,
    EOS_TOP_LAYER_ACTIVITY_SNAPSHOT,
    EOS_TOP_LAYER_APP_HEADER,
    EOS_TOP_LAYER_INTERACTION,
    EOS_TOP_LAYER_FLASHLIGHT,
    EOS_TOP_LAYER_SYSTEM_TOUCH_STATUS,
    EOS_TOP_LAYER_SYSTEM_TOUCH_MARKER,
    EOS_TOP_LAYER_SYSTEM_TOUCH_DIAGNOSTIC,
    EOS_TOP_LAYER_SYSTEM_TOAST,
    EOS_TOP_LAYER_SYSTEM_CROWN,
    EOS_TOP_LAYER_SYSTEM_OBJS,
    EOS_TOP_LAYER_SYSTEM_FPS,
    EOS_TOP_LAYER_SYSTEM_ESH_CMD,
    EOS_TOP_LAYER_SYSTEM_ERROR,
    EOS_TOP_LAYER_MESSAGE_DETAIL,
    EOS_TOP_LAYER_PERMISSION,
    EOS_TOP_LAYER_LOCK,
    EOS_TOP_LAYER_DEBUG_TEST,
};

static lv_obj_t *_slot_objects[sizeof(_slot_order) / sizeof(_slot_order[0])] = {NULL};

/* Function Implementations -----------------------------------*/

static lv_obj_t *_create_slot(lv_obj_t *parent)
{
    lv_obj_t *slot = lv_obj_create(parent);
    if (!slot)
        return NULL;

    lv_obj_remove_style_all(slot);
    lv_obj_set_size(slot, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_opa(slot, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(slot, 0, 0);
    lv_obj_set_style_pad_all(slot, 0, 0);
    lv_obj_set_scrollbar_mode(slot, LV_SCROLLBAR_MODE_OFF);
    lv_obj_remove_flag(slot, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_flag(slot, LV_OBJ_FLAG_SCROLLABLE);
    return slot;
}

void eos_overlay_layer_init(void)
{
    if (_initialized)
        return;

    /* The only direct LVGL top-layer access in ElenixOS. */
    _top_root = _create_slot(lv_layer_top());
    if (!_top_root)
    {
        EOS_LOG_E("Failed to create top-layer root");
        return;
    }

    /* Create from lowest to highest. LVGL append order establishes z-order. */
    for (size_t i = 0; i < sizeof(_slot_order) / sizeof(_slot_order[0]); i++)
    {
        _slot_objects[i] = _create_slot(_top_root);
        if (!_slot_objects[i])
        {
            EOS_LOG_E("Failed to create top-layer slot[%d]", (int)_slot_order[i]);
            return;
        }
    }

    _initialized = true;
    EOS_LOG_I("Fixed top-layer slot graph initialized (%zu slots)", sizeof(_slot_order) / sizeof(_slot_order[0]));
}

lv_obj_t *eos_overlay_layer_get(eos_top_layer_slot_t slot)
{
    for (size_t i = 0; i < sizeof(_slot_order) / sizeof(_slot_order[0]); i++)
    {
        if (_slot_order[i] == slot)
            return _slot_objects[i];
    }

    return NULL;
}

bool eos_overlay_layer_is_valid_slot(eos_top_layer_slot_t slot)
{
    return eos_overlay_layer_get(slot) != NULL;
}
