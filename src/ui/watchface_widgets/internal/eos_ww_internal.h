/**
 * @file eos_ww_internal.h
 * @brief Private helpers shared by complex watchface widgets
 */

#ifndef EOS_WW_INTERNAL_H
#define EOS_WW_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include "lvgl.h"

/* Function prototypes ----------------------------------------*/

/** @brief Remove generic interaction and decoration from a static child. */
void eos_ww_internal_make_static(lv_obj_t *obj);

/** @brief Set a transparent, borderless container style. */
void eos_ww_internal_make_container(lv_obj_t *obj);

/** @brief Keep a widget's requested size inside its parent's content area. */
void eos_ww_internal_fit_size(const lv_obj_t *parent, lv_coord_t *width, lv_coord_t *height);

#ifdef __cplusplus
}
#endif

#endif /* EOS_WW_INTERNAL_H */
