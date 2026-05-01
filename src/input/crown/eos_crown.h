/**
 * @file eos_crown.h
 * @brief Crown
 */

#ifndef ELENIX_OS_CROWN_H
#define ELENIX_OS_CROWN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "eos_core.h"
#include "input/eos_input.h"
/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/
typedef int16_t eos_crown_encoder_diff_t;
/* Public function prototypes --------------------------------*/
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
 * @brief Initialize
 */
void eos_crown_init(void);
#ifdef __cplusplus
}
#endif

#endif /* ELENIX_OS_CROWN_H */
