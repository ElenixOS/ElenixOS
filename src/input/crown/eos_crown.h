/**
 * @file eos_crown.h
 * @brief Crown
 */

#ifndef EOS_CROWN_H
#define EOS_CROWN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "eos_core.h"
#include "input/eos_input.h"
#include "eos_slide_widget.h"
/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/
typedef int16_t eos_crown_encoder_diff_t;
/* Public function prototypes ---------------------------------*/
/**
 * @brief Set encoder reverse
 * @param reverse true = reverse, false = forward
 */
void eos_crown_encoder_set_reverse(bool reverse);
/**
 * @brief Set encoder scroll object
 * @param obj Target scroll object
 */
void eos_crown_encoder_set_target_obj(lv_obj_t *obj);
/**
 * @brief Automatically set encoder scroll object based on active View, always gets the first created object
 * @param view Target View
 */
void eos_crown_encoder_set_target_view(lv_obj_t *view);
/**
 * @brief Activate scroll object of currently visible overlay, uniformly selected by overlay priority
 */
void eos_crown_encoder_activate_current_overlay_scrollable(void);
/**
 * @brief Report crown button state
 * @param state State value
 */
void eos_crown_button_report(eos_button_state_t state);
/**
 * @brief Report crown encoder increment
 * @param diff Increment
 */
void eos_crown_encoder_report(eos_crown_encoder_diff_t diff);
/**
 * @brief Report a non-touch pixel-level encoder scroll delta
 * @param delta Scroll delta in display pixels
 *
 * This path keeps Crown scroll feedback, including haptic ticks and the
 * focused scrollbar color, while allowing continuous input sources to avoid
 * restarting the detent animation for every sample.
 */
void eos_crown_encoder_scroll_report(int32_t delta);
/**
 * @brief Initialize
 */
void eos_crown_init(void);
/**
 * @brief Register slide widget to enable automatic encoder targeting when the widget is opened
 * @param sw Target slide widget
 */
void eos_crown_encoder_register_slide_widget(eos_slide_widget_t *sw);
#ifdef __cplusplus
}
#endif

#endif /* EOS_CROWN_H */
