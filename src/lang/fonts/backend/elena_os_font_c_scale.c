/**
 * @file elena_os_font_c_scale.c
 * @brief C 缩放
 * @author Sab1e
 * @date 2025-11-05
 */

#include "elena_os_config.h"
#if EOS_FONT_TYPE == EOS_FONT_C_SCALE
#include "elena_os_font.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>

/* Macros and Definitions -------------------------------------*/
#define _SCALE_DEFAULT 255
LV_FONT_DECLARE(EOS_FONT_C_NAME);
/* Variables --------------------------------------------------*/
static const uint16_t scale_large = _SCALE_DEFAULT * EOS_FONT_SIZE_LARGE / EOS_FONT_C_SIZE;
static const uint16_t scale_medium = _SCALE_DEFAULT * EOS_FONT_SIZE_MEDIUM / EOS_FONT_C_SIZE;
static const uint16_t scale_small = _SCALE_DEFAULT * EOS_FONT_SIZE_SMALL / EOS_FONT_C_SIZE;
/* Function Implementations -----------------------------------*/

lv_font_t *eos_font_init(void)
{
    EOS_LOG_I("Font system init");
    return &EOS_FONT_C_NAME;
}

uint16_t _select_font_scale(eos_font_size_t size)
{
    switch (size)
    {
    case EOS_FONT_SIZE_LARGE:
        return scale_large;
    case EOS_FONT_SIZE_MEDIUM:
        return scale_medium;
    case EOS_FONT_SIZE_SMALL:
        return scale_small;
    default:
        return size * _SCALE_DEFAULT / EOS_FONT_C_SIZE;
    }
}

static void _label_size_changed_cb(lv_event_t *e)
{
    lv_obj_t *label = lv_event_get_target(e);
    lv_obj_update_layout(label);
    lv_coord_t w = lv_obj_get_width(label);
    lv_coord_t h = lv_obj_get_height(label);

    lv_align_t align = lv_obj_get_style_align(label, 0);

    switch (align)
    {
    case LV_ALIGN_TOP_LEFT:
        lv_obj_set_style_transform_pivot_x(label, 0, 0);
        lv_obj_set_style_transform_pivot_y(label, 0, 0);
        break;

    case LV_ALIGN_TOP_MID:
        lv_obj_set_style_transform_pivot_x(label, w / 2, 0);
        lv_obj_set_style_transform_pivot_y(label, 0, 0);
        break;

    case LV_ALIGN_TOP_RIGHT:
        lv_obj_set_style_transform_pivot_x(label, w, 0);
        lv_obj_set_style_transform_pivot_y(label, 0, 0);
        break;

    case LV_ALIGN_LEFT_MID:
        lv_obj_set_style_transform_pivot_x(label, 0, 0);
        lv_obj_set_style_transform_pivot_y(label, h / 2, 0);
        break;

    case LV_ALIGN_CENTER:
        lv_obj_set_style_transform_pivot_x(label, w / 2, 0);
        lv_obj_set_style_transform_pivot_y(label, h / 2, 0);
        break;

    case LV_ALIGN_RIGHT_MID:
        lv_obj_set_style_transform_pivot_x(label, w, 0);
        lv_obj_set_style_transform_pivot_y(label, h / 2, 0);
        break;

    case LV_ALIGN_BOTTOM_LEFT:
        lv_obj_set_style_transform_pivot_x(label, 0, 0);
        lv_obj_set_style_transform_pivot_y(label, h, 0);
        break;

    case LV_ALIGN_BOTTOM_MID:
        lv_obj_set_style_transform_pivot_x(label, w / 2, 0);
        lv_obj_set_style_transform_pivot_y(label, h, 0);
        break;

    case LV_ALIGN_BOTTOM_RIGHT:
        lv_obj_set_style_transform_pivot_x(label, w, 0);
        lv_obj_set_style_transform_pivot_y(label, h, 0);
        break;

    default:
        // 默认垂直居中
        lv_obj_set_style_transform_pivot_x(label, 0, 0);
        lv_obj_set_style_transform_pivot_y(label, h / 2, 0);
        break;
    }
}

void eos_label_add_size_changed_cb(lv_obj_t *label)
{
    lv_obj_update_layout(label);
    lv_obj_add_event_cb(label, _label_size_changed_cb, LV_EVENT_SIZE_CHANGED, NULL);
}

void eos_label_set_font_size(lv_obj_t *label, eos_font_size_t size)
{
    EOS_CHECK_PTR_RETURN(label);
    lv_obj_set_style_transform_scale(label, _select_font_scale(size), 0);
}

#endif /* EOS_FONT_TYPE */
