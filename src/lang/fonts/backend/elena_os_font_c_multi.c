/**
 * @file elena_os_font_c_multi.c
 * @brief 多字体资源
 * @author Sab1e
 * @date 2025-11-05
 */

#include "elena_os_config.h"
#if EOS_FONT_TYPE == EOS_FONT_C_MULTI
#include "elena_os_font.h"

// Includes
#include <stdio.h>
#include <stdlib.h>

// Macros and Definitions
LV_FONT_DECLARE(EOS_FONT_LARGE_NAME);
LV_FONT_DECLARE(EOS_FONT_MEDIUM_NAME);
LV_FONT_DECLARE(EOS_FONT_SMALL_NAME);
LV_FONT_DECLARE(EOS_FONT_ICON);
// Variables
static lv_font_t *font_large;
static lv_font_t *font_medium;
static lv_font_t *font_small;
// Function Implementations

lv_font_t *eos_font_init(void)
{
    EOS_LOG_I("Font system init");

    font_large = &EOS_FONT_LARGE_NAME;
    font_medium = &EOS_FONT_MEDIUM_NAME;
    font_small = &EOS_FONT_SMALL_NAME;

    return font_medium;
}

lv_font_t *_select_font(eos_font_size_t size)
{
    switch (size)
    {
    case EOS_FONT_SIZE_LARGE:
        return font_large;
    case EOS_FONT_SIZE_MEDIUM:
        return font_medium;
    case EOS_FONT_SIZE_SMALL:
        return font_small;
    default:
        EOS_LOG_W("Multiple font sizes are not supported");
        return font_medium;
    }
}

void eos_label_set_font_size(lv_obj_t *label, eos_font_size_t size)
{
    EOS_CHECK_PTR_RETURN(label);
    lv_obj_set_style_text_font(label, _select_font(size), 0);
}

#endif /* EOS_FONT_TYPE */
