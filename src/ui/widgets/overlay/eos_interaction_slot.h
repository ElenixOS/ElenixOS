/**
 * @file eos_interaction_slot.h
 * @brief Dynamic focus control for peer overlays in the interaction slot
 */

#ifndef EOS_INTERACTION_SLOT_H
#define EOS_INTERACTION_SLOT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdbool.h>
#include "lvgl.h"

/* Public function prototypes ---------------------------------*/

/**
 * @brief Bring peer overlay objects to the front of the interaction slot
 * @param foreground_obj Main panel or content object to focus
 * @param touch_obj Touch object associated with the focused overlay
 * @return true when both objects belong to the interaction slot and were focused
 *
 * Objects must be direct children of EOS_TOP_LAYER_INTERACTION. The slot
 * itself remains fixed; only its peer children are reordered.
 */
bool eos_interaction_slot_focus(lv_obj_t *foreground_obj, lv_obj_t *touch_obj);

/**
 * @brief Get the object most recently focused in the interaction slot
 * @return Focused object, or NULL when no object is focused
 */
lv_obj_t *eos_interaction_slot_get_focused(void);

#ifdef __cplusplus
}
#endif

#endif /* EOS_INTERACTION_SLOT_H */
