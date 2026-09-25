/**
 * @file eos_ww_internal.c
 * @brief Private helpers shared by complex watchface widgets
 */

#include "eos_ww_internal.h"

/* Function Implementations -----------------------------------*/

void eos_ww_internal_make_static(lv_obj_t *obj)
{
    if (!obj)
        return;
    lv_obj_set_scrollable(obj, false);
    lv_obj_set_clickable(obj, false);
    lv_obj_set_click_focusable(obj, false);
    lv_obj_set_style_pad_all(obj, 0, 0);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_shadow_width(obj, 0, 0);
}

void eos_ww_internal_make_container(lv_obj_t *obj)
{
    if (!obj)
        return;
    lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, 0);
    eos_ww_internal_make_static(obj);
}

void eos_ww_internal_fit_size(const lv_obj_t *parent, lv_coord_t *width, lv_coord_t *height)
{
    lv_coord_t parent_width;
    lv_coord_t parent_height;

    if (!parent || !width || !height)
        return;
    parent_width = lv_obj_get_content_width(parent);
    parent_height = lv_obj_get_content_height(parent);
    if (parent_width > 0 && *width > parent_width)
        *width = parent_width;
    if (parent_height > 0 && *height > parent_height)
        *height = parent_height;
}
