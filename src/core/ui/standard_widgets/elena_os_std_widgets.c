/**
 * @file elena_os_std_widgets.c
 * @brief 系统标准组件
 * @author Sab1e
 * @date 2025-12-07
 */

#include "elena_os_std_widgets.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "elena_os_lang.h"
#include "elena_os_theme.h"
#include "elena_os_basic_widgets.h"
#include "elena_os_icon.h"
#include "elena_os_font.h"
#include "elena_os_nav.h"
/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

lv_obj_t *eos_std_info_create(lv_obj_t *scr,
                              lv_color_t icon_bg_color,
                              const char *icon,
                              const char *title_txt,
                              const char *txt,
                              bool clear_nav)
{
    lv_obj_t *list = eos_list_create(scr);
    lv_obj_set_style_pad_row(list, 20, 0);

    eos_round_icon_create(list, icon_bg_color, icon);

    lv_obj_t *title_label = lv_label_create(list);
    lv_label_set_text(title_label, title_txt);
    eos_label_set_font_size(title_label, EOS_FONT_SIZE_LARGE);

    lv_obj_t *label = lv_label_create(list);
    if (eos_lang_get == LANG_EN)
        lv_obj_set_width(label, lv_pct(100));
    else if (eos_lang_get == LANG_ZH)
        lv_obj_set_width(label, lv_pct(60));
    else
        lv_obj_set_width(label, lv_pct(80));
    lv_label_set_text(label, txt);
    lv_obj_t *btn;
    if (clear_nav)
        btn = eos_button_create(list, current_lang[STR_ID_BASE_ITEM_BACK], eos_nav_clean_up_cb, NULL);
    else
        btn = eos_button_create(list, current_lang[STR_ID_BASE_ITEM_BACK], eos_nav_back_prev_cb, NULL);
    lv_obj_set_width(btn, lv_pct(80));
    return list;
}
