/**
 * @file eos_radio_list.h
 * @brief Radio list page
 */

#ifndef ELENIX_OS_RADIO_LIST_H
#define ELENIX_OS_RADIO_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"
/* Public macros ----------------------------------------------*/

#define EOS_INVALID_RADIO_INDEX UINT32_MAX      /**< Invalid index value */

/* Public typedefs --------------------------------------------*/

typedef struct eos_radio_list_t eos_radio_list_t;

/* Public function prototypes --------------------------------*/

/**
 * @brief Add new option to radio list
 *
 * Option index values increment from 0, for example:
 *
 * ```c
 * eos_radio_list_t *rl = eos_radio_list_enter("Title")
 * eos_radio_list_add_item(rl,"Option 1");     // index = 0
 * eos_radio_list_add_item(rl,"Option 2");     // index = 1
 * eos_radio_list_add_item(rl,"Option 3");     // index = 2
 * ```
 * @param rl Radio list pointer
 * @param txt Option text string
 * @return uint32_t Returns index value of the radio option on success, otherwise returns `EOS_INVALID_RADIO_INDEX`
 */
uint32_t eos_radio_list_add_item(eos_radio_list_t *rl, const char *txt);

/**
 * @brief Add subtitle to the top of radio list
 * @param subtitle Subtitle string
 */
void eos_radio_list_set_subtitle(eos_radio_list_t *rl, const char *subtitle);

/**
 * @brief Add comment to the bottom of radio list
 * @param comment Comment string
 */
void eos_radio_list_set_comment(eos_radio_list_t *rl, const char *comment);

/**
 * @brief Add callback function for radio list selection
 * @param event_cb Standard `lv_event_cb_t`, use `lv_event_get_param()` to get the selected item's index (0-based)
 * @param user_data User data
 */
void eos_radio_list_add_event_cb(eos_radio_list_t *rl, lv_event_cb_t event_cb, void *user_data);

/**
 * @brief Create new radio list
 * @param title Title string
 * @return eos_radio_list_t* Returns radio list pointer on success, NULL on failure
 */
eos_radio_list_t *eos_radio_list_enter(const char *title);

/**
 * @brief Check the radio option at specified index
 * @param index Index of the target radio option
 */
void eos_radio_list_check(eos_radio_list_t *rl, uint32_t index);

#ifdef __cplusplus
}
#endif

#endif /* ELENIX_OS_RADIO_LIST_H */
