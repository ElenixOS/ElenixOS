/**
 * @file elena_os_card_pager.h
 * @brief 卡片式页面
 * @author Sab1e
 * @date 2025-10-17
 * @details
 *
 * # CardPager
 *
 * ## 简述
 *
 * 卡片式换页组件，页面列表使用链表控制，支持循环翻页。
 *
 * ## 示例
 *
 * ```c
 * eos_card_pager_t *cp = eos_card_pager_create(lv_screen_active(), EOS_CARD_PAGER_DIR_HOR);
 * lv_obj_t *page = eos_card_pager_create_page(cp);
 * lv_obj_set_style_bg_color(page, EOS_COLOR_GREEN, 0);
 * page = eos_card_pager_create_page(cp);
 * lv_obj_set_style_bg_color(page, EOS_COLOR_YELLOW, 0);
 * eos_card_pager_set_loop(cp, true);   // 开启循环
 * ```
 */

#ifndef ELENA_OS_CARD_PAGER_H
#define ELENA_OS_CARD_PAGER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"
#include "elena_os_slide_widget.h"
/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

typedef enum
{
    EOS_CARD_PAGER_DIR_VER,
    EOS_CARD_PAGER_DIR_HOR
} eos_card_pager_dir_t;

typedef struct eos_card_pager_node_t eos_card_pager_node_t;

/**
 * @brief 双向链表存储页面
 */
struct eos_card_pager_node_t
{
    lv_obj_t *page;
    eos_card_pager_node_t *prev;
    eos_card_pager_node_t *next;
};

typedef struct
{
    lv_obj_t *parent;
    lv_obj_t *background;
    lv_obj_t *indicator;
    eos_card_pager_node_t *page_list_head;
    uint8_t current_page_index;
    uint8_t page_count;
    eos_slide_widget_t *sw1;
    eos_slide_widget_t *sw2;
    eos_card_pager_dir_t dir;
    bool loop;
} eos_card_pager_t;
/* Public function prototypes --------------------------------*/
eos_card_pager_t *eos_card_pager_create(lv_obj_t *parent, eos_card_pager_dir_t dir);
lv_obj_t *eos_card_pager_create_page(eos_card_pager_t *cp);
void eos_card_pager_set_loop(eos_card_pager_t *cp, bool loop);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_CARD_PAGER_H */
