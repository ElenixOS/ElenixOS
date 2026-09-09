/**
 * @file eos_interaction_slot.c
 * @brief Dynamic focus control for peer overlays in the interaction slot
 */

#include "eos_interaction_slot.h"

/* Includes ---------------------------------------------------*/
#include "eos_overlay_layer.h"

/* Variables --------------------------------------------------*/
static lv_obj_t *_focused_obj = NULL;

/* Function Implementations -----------------------------------*/

bool eos_interaction_slot_focus(lv_obj_t *foreground_obj, lv_obj_t *touch_obj)
{
    lv_obj_t *slot = eos_overlay_layer_get(EOS_TOP_LAYER_INTERACTION);
    if (!slot || !foreground_obj || !touch_obj || !lv_obj_is_valid(foreground_obj) || !lv_obj_is_valid(touch_obj))
        return false;

    if (lv_obj_get_parent(foreground_obj) != slot || lv_obj_get_parent(touch_obj) != slot)
        return false;

    lv_obj_move_foreground(foreground_obj);
    lv_obj_move_foreground(touch_obj);
    _focused_obj = foreground_obj;
    return true;
}

lv_obj_t *eos_interaction_slot_get_focused(void)
{
    if (_focused_obj && !lv_obj_is_valid(_focused_obj))
        _focused_obj = NULL;

    return _focused_obj;
}
