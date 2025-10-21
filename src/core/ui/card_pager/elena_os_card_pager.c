/**
 * @file elena_os_card_pager.c
 * @brief 卡片式页面
 * @author Sab1e
 * @date 2025-10-17
 */

#include "elena_os_card_pager.h"

// Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define EOS_LOG_TAG "CardPager"
#include "elena_os_log.h"
#include "elena_os_config.h"
#include "elena_os_theme.h"
#include "elena_os_event.h"

// Macros and Definitions
#define CARD_PAGER_TOUCH_OBJ_HEIGHT 50
#define CP_GET_DIR (cp->dir)
#define IF_DIR_EQUAL_VER (CP_GET_DIR == EOS_CARD_PAGER_DIR_VER)
// Variables

// Function Implementations
lv_obj_t *eos_card_pager_get_page(eos_card_pager_t *cp, uint8_t page_index);
static void _page_switch_handler(eos_card_pager_t *cp);

static void _sw1_reachd_threshold_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    eos_card_pager_t *cp = (eos_card_pager_t *)lv_event_get_user_data(e);
    if (cp->sw1->state == EOS_SLIDE_WIDGET_STATE_THRESHOLD)
    {
        // 返回上一页面
        if (cp->current_page_index == 0)
            cp->current_page_index = cp->loop ? cp->page_count - 1 : 0;
        else
            cp->current_page_index--;

        _page_switch_handler(cp);
        lv_obj_move_foreground(cp->background);
        lv_obj_move_foreground(eos_card_pager_get_page(cp, cp->current_page_index));
    }
}

static void _sw2_reachd_threshold_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    eos_card_pager_t *cp = (eos_card_pager_t *)lv_event_get_user_data(e);
    if (cp->sw2->state == EOS_SLIDE_WIDGET_STATE_THRESHOLD)
    {
        // 下一页面
        if (cp->current_page_index == cp->page_count - 1)
            cp->current_page_index = cp->loop ? 0 : cp->page_count - 1;
        else
            cp->current_page_index++;

        _page_switch_handler(cp);
        lv_obj_move_foreground(cp->background);
        lv_obj_move_foreground(eos_card_pager_get_page(cp, cp->current_page_index));
    }
}

static void _disable_slide_widget(eos_slide_widget_t *sw)
{
    sw->threshold = EOS_THRESHOLD_INFINITE;
    sw->base = 0;
}

static void _page_switch_handler(eos_card_pager_t *cp)
{
    EOS_LOG_I("Curent page index: %d / %d (loop=%d)", cp->current_page_index+1, cp->page_count, cp->loop);

    if (cp->page_count == 0)
        return;

    if (cp->page_count == 1)
    {
        cp->sw1->target_obj = eos_card_pager_get_page(cp, 0);
        cp->sw2->target_obj = eos_card_pager_get_page(cp, 0);
        _disable_slide_widget(cp->sw1);
        _disable_slide_widget(cp->sw2);
        return;
    }

    bool vertical = (cp->dir == EOS_CARD_PAGER_DIR_VER);
    lv_coord_t base_offset = vertical ? EOS_DISPLAY_HEIGHT : EOS_DISPLAY_WIDTH;

    // 默认启用滑动
    cp->sw1->threshold = EOS_THRESHOLD_30;
    cp->sw2->threshold = EOS_THRESHOLD_30;

    uint8_t cur = cp->current_page_index;
    uint8_t last = cp->page_count - 1;

    // 上一个方向
    if (cur == 0)
    {
        if (cp->loop)
        {
            cp->sw1->target_obj = eos_card_pager_get_page(cp, last);
            cp->sw1->base = vertical ? -base_offset : -base_offset;
        }
        else
        {
            _disable_slide_widget(cp->sw1);
            cp->sw1->target_obj = eos_card_pager_get_page(cp, 0);
        }

        cp->sw2->target_obj = eos_card_pager_get_page(cp, 1);
        cp->sw2->base = vertical ? base_offset : base_offset;
    }
    else if (cur == last)
    {
        cp->sw1->target_obj = eos_card_pager_get_page(cp, cur - 1);
        cp->sw1->base = vertical ? -base_offset : -base_offset;

        if (cp->loop)
        {
            cp->sw2->target_obj = eos_card_pager_get_page(cp, 0);
            cp->sw2->base = vertical ? base_offset : base_offset;
        }
        else
        {
            _disable_slide_widget(cp->sw2);
            cp->sw2->target_obj = eos_card_pager_get_page(cp, cur);
        }
    }
    else
    {
        cp->sw1->target_obj = eos_card_pager_get_page(cp, cur - 1);
        cp->sw1->base = vertical ? -base_offset : -base_offset;

        cp->sw2->target_obj = eos_card_pager_get_page(cp, cur + 1);
        cp->sw2->base = vertical ? base_offset : base_offset;
    }

    EOS_LOG_I("Slide config: sw1->target=%p, sw2->target=%p", cp->sw1->target_obj, cp->sw2->target_obj);
}

lv_obj_t *eos_card_pager_get_page(eos_card_pager_t *cp, uint8_t page_index)
{
    EOS_CHECK_PTR_RETURN_VAL(cp && cp->page_list_head, NULL);

    if (page_index >= cp->page_count)
        return NULL;

    eos_card_pager_node_t *cur = cp->page_list_head;
    for (uint8_t i = 0; i < page_index; i++)
    {
        cur = cur->next;
    }
    return cur ? cur->page : NULL;
}

static void _page_init(lv_obj_t *page)
{
    lv_obj_remove_style_all(page);
    lv_obj_set_size(page, EOS_DISPLAY_WIDTH, EOS_DISPLAY_HEIGHT);
    lv_obj_set_style_bg_color(page, EOS_COLOR_WHITE, 0);
    lv_obj_set_style_bg_opa(page, LV_OPA_COVER, 0);
}

lv_obj_t *eos_card_pager_create_page(eos_card_pager_t *cp)
{
    EOS_CHECK_PTR_RETURN_VAL(cp && cp->parent, NULL);

    eos_card_pager_node_t *node = lv_malloc(sizeof(eos_card_pager_node_t));
    EOS_CHECK_PTR_RETURN_VAL(node, NULL);
    memset(node, 0, sizeof(eos_card_pager_node_t));

    lv_obj_t *page = lv_obj_create(cp->parent);
    _page_init(page);
    lv_obj_add_event_cb(page, _sw1_reachd_threshold_cb, EOS_EVENT_SLIDE_WIDGET_REACHED_THRESHOLD, cp);
    lv_obj_add_event_cb(page, _sw2_reachd_threshold_cb, EOS_EVENT_SLIDE_WIDGET_REACHED_THRESHOLD, cp);
    node->page = page;

    if (cp->page_list_head == NULL)
    {
        // 第一个页面
        cp->page_list_head = node;
        cp->current_page_index = 0;
        cp->page_count++;
    }
    else
    {
        // 链表尾部
        eos_card_pager_node_t *cur = cp->page_list_head;
        while (cur->next)
            cur = cur->next;
        cur->next = node;
        node->prev = cur;
        if (cp->dir == EOS_CARD_PAGER_DIR_VER)
        {
            lv_obj_set_pos(page, 0, EOS_DISPLAY_HEIGHT);
        }
        else
        {
            lv_obj_set_pos(page, EOS_DISPLAY_WIDTH, 0);
        }
        cp->page_count++;
        _page_switch_handler(cp);
    }
    EOS_LOG_I("Page created: [%p]\nPage count: %d", page, cp->page_count);
    return page;
}

bool eos_card_pager_remove_page(eos_card_pager_t *cp, uint8_t page_index)
{
    EOS_CHECK_PTR_RETURN_VAL(cp && cp->page_list_head, false);
    if (page_index >= cp->page_count)
        return false;

    eos_card_pager_node_t *cur = cp->page_list_head;
    for (uint8_t i = 0; i < page_index; i++)
        cur = cur->next;

    // 重新链接链表
    if (cur->prev)
        cur->prev->next = cur->next;
    else
        cp->page_list_head = cur->next; // 删除的是头节点

    if (cur->next)
        cur->next->prev = cur->prev;

    // 如果删除的是当前页面节点，则重设当前页
    if (cp->current_page_index == page_index)
        cp->current_page_index = 0;

    // 释放 LVGL 对象与节点
    if (cur->page)
        lv_obj_del(cur->page);
    lv_free(cur);

    cp->page_count--;
    return true;
}

void eos_card_pager_set_loop(eos_card_pager_t *cp, bool loop)
{
    EOS_CHECK_PTR_RETURN(cp);
    cp->loop = loop;
    _page_switch_handler(cp);
}

eos_card_pager_t *eos_card_pager_create(lv_obj_t *parent, eos_card_pager_dir_t dir)
{
    EOS_CHECK_PTR_RETURN_VAL(parent, NULL);

    eos_card_pager_t *cp = lv_malloc(sizeof(eos_card_pager_t));
    EOS_CHECK_PTR_RETURN_VAL(cp, NULL);
    memset(cp, 0, sizeof(eos_card_pager_t));

    cp->parent = parent;
    cp->dir = dir;
    cp->loop = false;
    cp->background = lv_obj_create(parent);
    _page_init(cp->background);
    lv_obj_set_style_bg_color(cp->background, EOS_COLOR_BLUE, 0);
    // 创建初始页面
    lv_obj_t *page = eos_card_pager_create_page(cp);

    // 初始化滑动控件（示例）
    switch (dir)
    {
    case EOS_CARD_PAGER_DIR_VER:
    {
        cp->sw1 = eos_slide_widget_create(parent, page, EOS_SLIDE_DIR_VER, 0, EOS_THRESHOLD_INFINITE);
        cp->sw1->base = -EOS_DISPLAY_HEIGHT;
        lv_obj_set_size(cp->sw1->touch_obj, EOS_DISPLAY_WIDTH, CARD_PAGER_TOUCH_OBJ_HEIGHT);
        lv_obj_set_pos(cp->sw1->touch_obj, 0, 0);

        cp->sw2 = eos_slide_widget_create(parent, page, EOS_SLIDE_DIR_VER, 0, EOS_THRESHOLD_INFINITE);
        cp->sw2->base = EOS_DISPLAY_HEIGHT;
        lv_obj_set_size(cp->sw2->touch_obj, EOS_DISPLAY_WIDTH, CARD_PAGER_TOUCH_OBJ_HEIGHT);
        lv_obj_set_pos(cp->sw2->touch_obj, 0, EOS_DISPLAY_HEIGHT - CARD_PAGER_TOUCH_OBJ_HEIGHT);
        break;
    }

    case EOS_CARD_PAGER_DIR_HOR:
    {
        cp->sw1 = eos_slide_widget_create(parent, page, EOS_SLIDE_DIR_HOR, 0, EOS_THRESHOLD_INFINITE);
        cp->sw1->base = -EOS_DISPLAY_WIDTH;
        lv_obj_set_size(cp->sw1->touch_obj, CARD_PAGER_TOUCH_OBJ_HEIGHT, EOS_DISPLAY_HEIGHT);
        lv_obj_set_pos(cp->sw1->touch_obj, 0, 0);

        cp->sw2 = eos_slide_widget_create(parent, page, EOS_SLIDE_DIR_HOR, 0, EOS_THRESHOLD_INFINITE);
        cp->sw2->base = EOS_DISPLAY_WIDTH;
        lv_obj_set_size(cp->sw2->touch_obj, CARD_PAGER_TOUCH_OBJ_HEIGHT, EOS_DISPLAY_HEIGHT);
        lv_obj_set_pos(cp->sw2->touch_obj, EOS_DISPLAY_WIDTH - CARD_PAGER_TOUCH_OBJ_HEIGHT, 0);
        break;
    }

    default:
        break;
    }

    return cp;
}
