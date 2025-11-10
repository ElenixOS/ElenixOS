/**
 * @file elena_os_font_ttf.c
 * @brief TTF 文件
 * @author Sab1e
 * @date 2025-11-05
 */

#include "elena_os_config.h"
#if EOS_FONT_TYPE == EOS_FONT_TTF
#include "elena_os_font.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>

/* Macros and Definitions -------------------------------------*/
LV_FONT_DECLARE(EOS_FONT_ICON);
#if EOS_FONT_TTF_TYPE == EOS_FONT_TTF_DATA
EOS_FONT_DATA_DECLARE(EOS_FONT_TTF_DATA_NAME);
EOS_FONT_DATA_SIZE_DECLARE(EOS_FONT_TTF_DATA_SIZE);
#endif /* EOS_FONT_TTF_TYPE */
/* Variables --------------------------------------------------*/
static lv_font_t *font_large;
static lv_font_t *font_medium;
static lv_font_t *font_small;
/* Function Implementations -----------------------------------*/

lv_font_t *eos_font_init(void)
{
    EOS_LOG_I("Font system init");

#if EOS_FONT_TTF_TYPE == EOS_FONT_TTF_DATA

#if EOS_FONT_TTF_ENABLE_EXTENDED
    font_large = lv_tiny_ttf_create_data_ex(EOS_FONT_TTF_DATA_NAME, EOS_FONT_TTF_DATA_SIZE, EOS_FONT_SIZE_LARGE, EOS_FONT_TTF_KERNING, EOS_FONT_TTF_CACHE_SIZE);
    font_medium = lv_tiny_ttf_create_data_ex(EOS_FONT_TTF_DATA_NAME, EOS_FONT_TTF_DATA_SIZE, EOS_FONT_SIZE_MEDIUM, EOS_FONT_TTF_KERNING, EOS_FONT_TTF_CACHE_SIZE);
    font_small = lv_tiny_ttf_create_data_ex(EOS_FONT_TTF_DATA_NAME, EOS_FONT_TTF_DATA_SIZE, EOS_FONT_SIZE_SMALL, EOS_FONT_TTF_KERNING, EOS_FONT_TTF_CACHE_SIZE);
#else
    font_large = lv_tiny_ttf_create_data(EOS_FONT_TTF_DATA_NAME, EOS_FONT_TTF_DATA_SIZE, EOS_FONT_SIZE_LARGE);
    font_medium = lv_tiny_ttf_create_data(EOS_FONT_TTF_DATA_NAME, EOS_FONT_TTF_DATA_SIZE, EOS_FONT_SIZE_MEDIUM);
    font_small = lv_tiny_ttf_create_data(EOS_FONT_TTF_DATA_NAME, EOS_FONT_TTF_DATA_SIZE, EOS_FONT_SIZE_SMALL);
#endif /* EOS_FONT_TTF_ENABLE_EXTENDED */

#elif EOS_FONT_TTF_TYPE == EOS_FONT_TTF_FILE

#if EOS_FONT_TTF_ENABLE_EXTENDED
    font_large = lv_tiny_ttf_create_file_ex(EOS_FONT_TTF_FILE_PATH, EOS_FONT_SIZE_LARGE, EOS_FONT_TTF_KERNING, EOS_FONT_TTF_CACHE_SIZE);
    font_medium = lv_tiny_ttf_create_file_ex(EOS_FONT_TTF_FILE_PATH, EOS_FONT_SIZE_MEDIUM, EOS_FONT_TTF_KERNING, EOS_FONT_TTF_CACHE_SIZE);
    font_small = lv_tiny_ttf_create_file_ex(EOS_FONT_TTF_FILE_PATH, EOS_FONT_SIZE_SMALL, EOS_FONT_TTF_KERNING, EOS_FONT_TTF_CACHE_SIZE);
#else
    font_large = lv_tiny_ttf_create_file(EOS_FONT_TTF_FILE_PATH, EOS_FONT_SIZE_LARGE);
    font_medium = lv_tiny_ttf_create_file(EOS_FONT_TTF_FILE_PATH, EOS_FONT_SIZE_MEDIUM);
    font_small = lv_tiny_ttf_create_file(EOS_FONT_TTF_FILE_PATH, EOS_FONT_SIZE_SMALL);
#endif /* EOS_FONT_TTF_ENABLE_EXTENDED */

#endif /* EOS_FONT_TTF_TYPE */

    if (!font_large || !font_medium || !font_small)
    {
        EOS_LOG_E("Some fonts failed to load!");
        return NULL;
    }
    else
    {
        font_large->fallback = font_medium;
        font_medium->fallback = font_small;
        font_small->fallback = &EOS_FONT_ICON;
        EOS_LOG_D("All TTF fonts loaded successfully");
    }
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
#if EOS_FONT_TTF_TYPE == EOS_FONT_TTF_DATA
        return lv_tiny_ttf_create_data(EOS_FONT_TTF_DATA_NAME, EOS_FONT_TTF_DATA_SIZE, size);
#elif EOS_FONT_TTF_TYPE == EOS_FONT_TTF_FILE
        return lv_tiny_ttf_create_file(EOS_FONT_TTF_FILE_PATH, size);
#endif /* EOS_FONT_TTF_TYPE */
    }
}

void eos_label_set_font_size(lv_obj_t *label, eos_font_size_t size)
{
    EOS_CHECK_PTR_RETURN(label);
    lv_obj_set_style_text_font(label, _select_font(size), 0);
}
#endif /* EOS_FONT_TYPE */
