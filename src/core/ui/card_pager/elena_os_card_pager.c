/**
 * @file elena_os_card_pager.c
 * @brief 卡片式页面
 * @author Sab1e
 * @date 2025-10-17
 */

#include "elena_os_card_pager.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define EOS_LOG_DISABLE
#define EOS_LOG_TAG "CardPager"
#include "elena_os_log.h"
#include "elena_os_config.h"
#include "elena_os_theme.h"
#include "elena_os_event.h"
#include "elena_os_port.h"
#include "elena_os_mem.h"

/* Macros and Definitions -------------------------------------*/
#define _CP_GET_DIR (cp->dir)
#define _IF_DIR_EQUAL_VER (_CP_GET_DIR == EOS_CARD_PAGER_DIR_VER)
#define _INDICATOR_DOT_WIDTH 12
#define _INDICATOR_DOT_HEIGHT _INDICATOR_DOT_WIDTH
#define _INDICATOR_ACTIVE_COLOR EOS_COLOR_WHITE
#define _INDICATOR_INACTIVE_COLOR EOS_COLOR_DARK_GREY_2
#define _PAGE_MARGIN 20
/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/
lv_obj_t *eos_card_pager_get_page(eos_card_pager_t *cp, uint8_t page_index);
static void _page_switch_handler(eos_card_pager_t *cp);
lv_obj_t *eos_card_pager_get_indicator(eos_card_pager_t *cp, uint8_t page_index);
lv_obj_t *eos_card_pager_get_page(eos_card_pager_t *cp, uint8_t page_index);

static inline void _set_indicator_active(lv_obj_t *indicator)
{
    lv_obj_set_style_bg_color(indicator, _INDICATOR_ACTIVE_COLOR, 0);
}

static inline void _set_indicator_inactive(lv_obj_t *indicator)
{
    lv_obj_set_style_bg_color(indicator, _INDICATOR_INACTIVE_COLOR, 0);
}

static inline void _notify_page_changed(eos_card_pager_t *cp)
{
    if (cp->page_changed_cb)
        cp->page_changed_cb(cp, cp->current_page_index, cp->page_changed_user_data);
}

static inline void _bring_pages_to_front(eos_card_pager_t *cp)
{
    EOS_CHECK_PTR_RETURN(cp);

    uint8_t cur = cp->current_page_index;
    uint8_t last = cp->page_count > 0 ? cp->page_count - 1 : 0;
    uint8_t prev_index = (cur == 0) ? (cp->loop ? last : cur) : (cur - 1);
    uint8_t next_index = (cur == last) ? (cp->loop ? 0 : cur) : (cur + 1);

    lv_obj_t *prev_page = eos_card_pager_get_page(cp, prev_index);
    lv_obj_t *next_page = eos_card_pager_get_page(cp, next_index);
    lv_obj_t *cur_page = eos_card_pager_get_page(cp, cur);

    if (prev_page && prev_page != cur_page)
        lv_obj_move_foreground(prev_page);
    if (next_page && next_page != cur_page)
        lv_obj_move_foreground(next_page);
    if (cur_page)
        lv_obj_move_foreground(cur_page);

    // Ensure gesture capture object stays on top, otherwise pages may block touch.
    if (cp->indicator_container)
        lv_obj_move_foreground(cp->indicator_container);
    if (cp->sw1 && cp->sw1->touch_obj)
        lv_obj_move_foreground(cp->sw1->touch_obj);
}

static void _sw1_reachd_threshold_cb(lv_event_t *e)
{
    eos_card_pager_t *cp = (eos_card_pager_t *)lv_event_get_user_data(e);
    if (cp->sw1->state == EOS_SLIDE_WIDGET_STATE_THRESHOLD)
    {
        uint8_t prev_index = cp->current_page_index;
        lv_coord_t displacement = cp->sw1->last_touch_displacement;

        if (displacement > 0)
        {
            // 向右/向下：切上一页
            if (cp->current_page_index == 0)
                cp->current_page_index = cp->loop ? cp->page_count - 1 : 0;
            else
                cp->current_page_index--;
        }
        else if (displacement < 0)
        {
            // 向左/向上：切下一页
            if (cp->current_page_index == cp->page_count - 1)
                cp->current_page_index = cp->loop ? 0 : cp->page_count - 1;
            else
                cp->current_page_index++;
        }
        else
        {
            cp->current_page_index = prev_index;
        }

        // 边界自动回拉：索引未变化则直接复位。
        if (cp->current_page_index == prev_index)
        {
            lv_obj_t *cur_page = eos_card_pager_get_page(cp, cp->current_page_index);
            if (cur_page)
                lv_obj_set_pos(cur_page, 0, 0);
            _page_switch_handler(cp);
            return;
        }

        lv_obj_t *indicator = eos_card_pager_get_indicator(cp, prev_index);
        _set_indicator_inactive(indicator);
        indicator = eos_card_pager_get_indicator(cp, cp->current_page_index);
        _set_indicator_active(indicator);

        _page_switch_handler(cp);
        lv_obj_move_foreground(cp->background);
        lv_obj_move_foreground(eos_card_pager_get_page(cp, cp->current_page_index));
        lv_obj_move_foreground(cp->indicator_container);
        lv_obj_move_foreground(cp->sw1->touch_obj);
        lv_obj_move_foreground(cp->sw2->touch_obj);
        _notify_page_changed(cp);
    }
}

static void _disable_slide_widget(eos_slide_widget_t *sw)
{
    sw->threshold = EOS_THRESHOLD_INFINITE;
    sw->base = 0;
}

static void _page_switch_handler(eos_card_pager_t *cp)
{
    EOS_LOG_I("Curent page index: %d / %d (loop=%d)", cp->current_page_index, cp->page_count, cp->loop);

    if (cp->page_count == 0)
        return;

    if (cp->page_count == 1)
    {
        cp->sw1->target_obj = eos_card_pager_get_page(cp, 0);
        _disable_slide_widget(cp->sw1);
        return;
    }

    bool vertical = (cp->dir == EOS_CARD_PAGER_DIR_VER);
    lv_coord_t base_offset = vertical ? EOS_DISPLAY_HEIGHT : EOS_DISPLAY_WIDTH;

    uint8_t cur = cp->current_page_index;
    uint8_t last = cp->page_count - 1;
    uint8_t prev_index = (cur == 0) ? (cp->loop ? last : cur) : (cur - 1);
    uint8_t next_index = (cur == last) ? (cp->loop ? 0 : cur) : (cur + 1);

    lv_obj_t *cur_page = eos_card_pager_get_page(cp, cur);
    lv_obj_t *prev_page = eos_card_pager_get_page(cp, prev_index);
    lv_obj_t *next_page = eos_card_pager_get_page(cp, next_index);

    cp->sw1->target_obj = cur_page;
    cp->sw1->base = 0;
    cp->sw1->target = base_offset;
    cp->sw1->threshold = EOS_THRESHOLD_30;

    if (cur_page)
        lv_obj_set_pos(cur_page, 0, 0);

    if (prev_page && prev_page != cur_page)
    {
        if (vertical)
            lv_obj_set_pos(prev_page, 0, -base_offset - _PAGE_MARGIN);
        else
            lv_obj_set_pos(prev_page, -base_offset - _PAGE_MARGIN, 0);
    }

    if (next_page && next_page != cur_page)
    {
        if (vertical)
            lv_obj_set_pos(next_page, 0, base_offset + _PAGE_MARGIN);
        else
            lv_obj_set_pos(next_page, base_offset + _PAGE_MARGIN, 0);
    }

    _bring_pages_to_front(cp);

    EOS_LOG_I("Slide config: current=%p, prev=%p, next=%p", cur_page, prev_page, next_page);
}

eos_card_pager_node_t *eos_card_pager_get_node(eos_card_pager_t *cp, uint8_t page_index)
{
    EOS_CHECK_PTR_RETURN_VAL(cp && cp->page_list_head, NULL);

    if (page_index >= cp->page_count)
        return NULL;

    eos_card_pager_node_t *cur = cp->page_list_head;
    for (uint8_t i = 0; i < page_index; i++)
    {
        cur = cur->next;
    }
    return cur ? cur : NULL;
}

lv_obj_t *eos_card_pager_get_indicator(eos_card_pager_t *cp, uint8_t page_index)
{
    eos_card_pager_node_t *cur = eos_card_pager_get_node(cp, page_index);
    return cur ? cur->indicator : NULL;
}

lv_obj_t *eos_card_pager_get_page(eos_card_pager_t *cp, uint8_t page_index)
{
    eos_card_pager_node_t *cur = eos_card_pager_get_node(cp, page_index);
    return cur ? cur->page : NULL;
}

static void _page_init(lv_obj_t *page)
{
    lv_obj_remove_style_all(page);
    lv_obj_set_size(page, EOS_DISPLAY_WIDTH, EOS_DISPLAY_HEIGHT);
    lv_obj_set_style_bg_color(page, EOS_COLOR_WHITE, 0);
    lv_obj_set_style_bg_opa(page, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(page, EOS_DISPLAY_RADIUS, 0);
}

lv_obj_t *eos_card_pager_create_page(eos_card_pager_t *cp)
{
    EOS_CHECK_PTR_RETURN_VAL(cp, NULL);

    eos_card_pager_node_t *node = eos_malloc_zeroed(sizeof(eos_card_pager_node_t));
    EOS_CHECK_PTR_RETURN_VAL(node, NULL);

    lv_obj_t *page = lv_obj_create(cp->container);
    _page_init(page);
    node->page = page;

    lv_obj_t *indicator = lv_obj_create(cp->indicator_container);
    lv_obj_remove_style_all(indicator);
    lv_obj_set_style_radius(indicator, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_size(indicator, _INDICATOR_DOT_WIDTH, _INDICATOR_DOT_HEIGHT);
    lv_obj_set_style_bg_color(indicator, _INDICATOR_INACTIVE_COLOR, 0);
    lv_obj_set_style_bg_opa(indicator, LV_OPA_COVER, 0);
    lv_obj_set_style_margin_all(indicator, 10, 0);
    node->indicator = indicator;

    if (cp->page_list_head == NULL)
    {
        cp->page_list_head = node;
        cp->current_page_index = 0;
        _set_indicator_active(indicator);
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
    lv_obj_move_foreground(cp->indicator_container);
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
        lv_obj_delete_async(cur->page);
    if (cur->indicator)
        lv_obj_delete_async(cur->indicator);
    eos_free(cur);

    cp->page_count--;
    return true;
}

void eos_card_pager_set_loop(eos_card_pager_t *cp, bool loop)
{
    EOS_CHECK_PTR_RETURN(cp);
    cp->loop = loop;
    _page_switch_handler(cp);
}

bool eos_card_pager_move_node(eos_card_pager_t *cp, uint8_t from_index, uint8_t to_index)
{
    EOS_LOG_I("Move Node");
    EOS_CHECK_PTR_RETURN_VAL(cp && cp->page_list_head, false);
    if (from_index >= cp->page_count || to_index >= cp->page_count)
    {
        EOS_LOG_W("Invalid index: from=%d, to=%d (count=%d)", from_index, to_index, cp->page_count);
        return false;
    }

    if (from_index == to_index)
    {
        EOS_LOG_I("Node already in position %d", from_index);
        return true;
    }

    // 找到 from 节点
    eos_card_pager_node_t *from = eos_card_pager_get_node(cp, from_index);
    if (!from)
        return false;

    // 找到 to 节点
    eos_card_pager_node_t *to = eos_card_pager_get_node(cp, to_index);
    EOS_LOG_D("To color: 0x%06X", lv_obj_get_style_bg_color(to->page, 0));
    if (!to)
        return false;

    // 从原链表断开
    if (from->prev)
        from->prev->next = from->next;
    else
        cp->page_list_head = from->next; // 移动的是头节点

    if (from->next)
        from->next->prev = from->prev;

    // 插入到目标节点之后
    from->next = to->next;
    from->prev = to;

    if (to->next)
        to->next->prev = from;

    to->next = from;

    // 更新当前页索引
    if (cp->current_page_index == from_index)
        cp->current_page_index = to_index;
    else if (from_index < cp->current_page_index && to_index >= cp->current_page_index)
        cp->current_page_index--;
    else if (from_index > cp->current_page_index && to_index <= cp->current_page_index)
        cp->current_page_index++;

    EOS_LOG_D("Current page index: %d", cp->current_page_index);

    uint8_t idx = 0;
    for (eos_card_pager_node_t *it = cp->page_list_head; it; it = it->next, idx++)
    {
        if (it->indicator && cp->indicator_container)
        {
            lv_obj_move_to_index(it->indicator, idx);
        }
    }

    // 更新滑动逻辑
    _page_switch_handler(cp);

    lv_obj_t *cur_page_obj = eos_card_pager_get_page(cp, cp->current_page_index);
    lv_obj_move_foreground(cp->background);
    lv_obj_move_foreground(cur_page_obj);
    lv_obj_set_pos(cur_page_obj, 0, 0);
    lv_obj_move_foreground(cp->sw1->touch_obj);
    lv_obj_move_foreground(cp->sw2->touch_obj);
    lv_obj_move_foreground(cp->indicator_container);

    _notify_page_changed(cp);

    EOS_LOG_I("Moved node: from %d -> %d", from_index, to_index);
    return true;
}

void eos_card_pager_move_page(eos_card_pager_t *cp, uint8_t page_index)
{
    EOS_CHECK_PTR_RETURN(cp);
    if (page_index >= cp->page_count)
    {
        EOS_LOG_W("Invalid page index: %d (max=%d)", page_index, cp->page_count - 1);
        return;
    }

    if (cp->current_page_index == page_index)
    {
        EOS_LOG_I("Already at page %d", page_index);
        return;
    }

    uint8_t prev_index = cp->current_page_index;
    cp->current_page_index = page_index;

    // 更新指示器状态
    lv_obj_t *indicator = eos_card_pager_get_indicator(cp, prev_index);
    if (indicator)
        lv_obj_set_style_bg_color(indicator, _INDICATOR_INACTIVE_COLOR, 0);

    indicator = eos_card_pager_get_indicator(cp, page_index);
    if (indicator)
        lv_obj_set_style_bg_color(indicator, _INDICATOR_ACTIVE_COLOR, 0);

    // 切换页面布局逻辑
    _page_switch_handler(cp);

    // 调整显示顺序
    lv_obj_t *cur_page_obj = eos_card_pager_get_page(cp, page_index);
    lv_obj_move_foreground(cp->background);
    lv_obj_move_foreground(cur_page_obj);
    lv_obj_set_pos(cur_page_obj, 0, 0);
    lv_obj_move_foreground(cp->sw1->touch_obj);
    lv_obj_move_foreground(cp->sw2->touch_obj);
    lv_obj_move_foreground(cp->indicator_container);

    _notify_page_changed(cp);

    EOS_LOG_I("Page moved to %d / %d", page_index + 1, cp->page_count);
}

void eos_card_pager_set_page_changed_cb(eos_card_pager_t *cp,
                                        eos_card_pager_page_changed_cb_t cb,
                                        void *user_data)
{
    EOS_CHECK_PTR_RETURN(cp);

    cp->page_changed_cb = cb;
    cp->page_changed_user_data = user_data;
}

static void _sw1_pressed_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    eos_card_pager_t *cp = (eos_card_pager_t *)lv_event_get_user_data(e);
    lv_obj_move_foreground(cp->indicator_container);
    lv_obj_move_foreground(cp->sw1->touch_obj);
    lv_obj_move_foreground(cp->sw2->touch_obj);
}

static void _sw2_pressed_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    eos_card_pager_t *cp = (eos_card_pager_t *)lv_event_get_user_data(e);
    lv_obj_move_foreground(cp->indicator_container);
    lv_obj_move_foreground(cp->sw1->touch_obj);
    lv_obj_move_foreground(cp->sw2->touch_obj);
}

static void _shared_touch_moving_cb(lv_event_t *e)
{
    eos_card_pager_t *cp = (eos_card_pager_t *)lv_event_get_user_data(e);
    EOS_CHECK_PTR_RETURN(cp && cp->sw1);

    eos_card_pager_node_t *cur_node = eos_card_pager_get_node(cp, cp->current_page_index);
    EOS_CHECK_PTR_RETURN(cur_node && cur_node->page);

    lv_obj_t *cur_obj = cur_node->page;
    lv_coord_t cur_pos = (lv_coord_t)(intptr_t)lv_event_get_param(e);

    bool vertical = (cp->dir == EOS_CARD_PAGER_DIR_VER);
    lv_coord_t base_offset = vertical ? EOS_DISPLAY_HEIGHT : EOS_DISPLAY_WIDTH;

    uint8_t cur = cp->current_page_index;
    uint8_t last = cp->page_count - 1;
    bool has_prev = cp->loop || (cur > 0);
    bool has_next = cp->loop || (cur < last);

    uint8_t prev_index = (cur == 0) ? last : (cur - 1);
    uint8_t next_index = (cur == last) ? 0 : (cur + 1);

    lv_obj_t *prev_page = has_prev ? eos_card_pager_get_page(cp, prev_index) : NULL;
    lv_obj_t *next_page = has_next ? eos_card_pager_get_page(cp, next_index) : NULL;

    if (cur_pos == 0)
    {
        cp->sw1->threshold = EOS_THRESHOLD_30;
        if (prev_page && prev_page != cur_obj)
        {
            if (vertical)
                lv_obj_set_pos(prev_page, 0, cur_pos - base_offset - _PAGE_MARGIN);
            else
                lv_obj_set_pos(prev_page, cur_pos - base_offset - _PAGE_MARGIN, 0);
        }

        if (next_page && next_page != cur_obj)
        {
            if (vertical)
                lv_obj_set_pos(next_page, 0, cur_pos + base_offset + _PAGE_MARGIN);
            else
                lv_obj_set_pos(next_page, cur_pos + base_offset + _PAGE_MARGIN, 0);
        }
        return;
    }

    if (cur_pos > 0)
    {
        if (!has_prev)
        {
            // 边界自动回拉（阻尼）
            cp->sw1->threshold = EOS_THRESHOLD_INFINITE;
            lv_coord_t damped = cur_pos / 3;
            if (vertical)
                lv_obj_set_y(cur_obj, damped);
            else
                lv_obj_set_x(cur_obj, damped);
            return;
        }

        cp->sw1->threshold = EOS_THRESHOLD_30;
        if (prev_page && prev_page != cur_obj)
        {
            if (vertical)
                lv_obj_set_pos(prev_page, 0, cur_pos - base_offset - _PAGE_MARGIN);
            else
                lv_obj_set_pos(prev_page, cur_pos - base_offset - _PAGE_MARGIN, 0);
        }
    }
    else
    {
        if (!has_next)
        {
            // 边界自动回拉（阻尼）
            cp->sw1->threshold = EOS_THRESHOLD_INFINITE;
            lv_coord_t damped = cur_pos / 3;
            if (vertical)
                lv_obj_set_y(cur_obj, damped);
            else
                lv_obj_set_x(cur_obj, damped);
            return;
        }

        cp->sw1->threshold = EOS_THRESHOLD_30;
        if (next_page && next_page != cur_obj)
        {
            if (vertical)
                lv_obj_set_pos(next_page, 0, cur_pos + base_offset + _PAGE_MARGIN);
            else
                lv_obj_set_pos(next_page, cur_pos + base_offset + _PAGE_MARGIN, 0);
        }
    }

    if (vertical)
    {
        lv_obj_set_y(cur_obj, cur_pos);
    }
    else
    {
        lv_obj_set_x(cur_obj, cur_pos);
    }

    _bring_pages_to_front(cp);
}

static void _sw1_reverted_cb(lv_event_t *e)
{
    eos_card_pager_t *cp = (eos_card_pager_t *)lv_event_get_user_data(e);
    lv_obj_t *cur_page = eos_card_pager_get_page(cp, cp->current_page_index);
    lv_obj_set_pos(cur_page, 0, 0);
    _page_switch_handler(cp);
}

static void _sw2_reverted_cb(lv_event_t *e)
{
    eos_card_pager_t *cp = (eos_card_pager_t *)lv_event_get_user_data(e);
    lv_obj_t *cur_page = eos_card_pager_get_page(cp, cp->current_page_index);
    lv_obj_set_pos(cur_page, 0, 0);
}

static void _container_delete_cb(lv_event_t *e)
{
    eos_card_pager_t *cp = (eos_card_pager_t *)lv_event_get_user_data(e);
    EOS_CHECK_PTR_RETURN(cp);
    cp->sw1 = NULL;
    cp->sw2 = NULL;

    eos_card_pager_node_t *node = cp->page_list_head;
    while (node)
    {
        eos_card_pager_node_t *next = node->next;
        eos_free(node);
        node = next;
    }
    cp->page_list_head = NULL;

    eos_free(cp);
}

eos_card_pager_t *eos_card_pager_create(lv_obj_t *parent, eos_card_pager_dir_t dir)
{
    eos_card_pager_t *cp = eos_malloc_zeroed(sizeof(eos_card_pager_t));
    EOS_CHECK_PTR_RETURN_VAL(cp && parent, NULL);

    cp->container = lv_obj_create(parent);
    lv_obj_remove_style_all(cp->container);
    lv_obj_set_size(cp->container, EOS_DISPLAY_WIDTH, EOS_DISPLAY_HEIGHT);
    lv_obj_remove_flag(cp->container, LV_OBJ_FLAG_SCROLLABLE);
    cp->dir = dir;
    cp->loop = false;
    cp->background = lv_obj_create(cp->container);
    lv_obj_set_style_bg_opa(cp->background, LV_OPA_TRANSP, 0);
    _page_init(cp->background);
    lv_obj_set_style_bg_color(cp->background, EOS_COLOR_BLACK, 0);

    lv_obj_t *indicator_container = lv_obj_create(cp->container);
    lv_obj_remove_style_all(indicator_container);
    cp->indicator_container = indicator_container;
    // 创建初始页面
    lv_obj_t *page = eos_card_pager_create_page(cp);

    // 初始化滑动控件
    switch (dir)
    {
    case EOS_CARD_PAGER_DIR_VER:
    {
        lv_obj_set_size(indicator_container, LV_SIZE_CONTENT, EOS_DISPLAY_HEIGHT);
        lv_obj_align(indicator_container, LV_ALIGN_RIGHT_MID, 0, 0);
        lv_obj_set_style_bg_opa(indicator_container, LV_OPA_TRANSP, 0);
        lv_obj_set_flex_flow(indicator_container, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(indicator_container,
                              LV_FLEX_ALIGN_CENTER,
                              LV_FLEX_ALIGN_CENTER,
                              LV_FLEX_ALIGN_CENTER);

        cp->sw1 = eos_slide_widget_create(cp->container, page, EOS_SLIDE_DIR_VER, 0, EOS_THRESHOLD_INFINITE);
        cp->sw1->base = 0;
        cp->sw1->target = EOS_DISPLAY_HEIGHT;
        eos_slide_widget_set_bidirectional(cp->sw1, true);
        lv_obj_set_size(cp->sw1->touch_obj, EOS_DISPLAY_WIDTH, EOS_DISPLAY_HEIGHT);
        lv_obj_set_pos(cp->sw1->touch_obj, 0, 0);
        cp->sw2 = cp->sw1;
        break;
    }

    case EOS_CARD_PAGER_DIR_HOR:
    {
        lv_obj_set_size(indicator_container, EOS_DISPLAY_WIDTH, LV_SIZE_CONTENT);
        lv_obj_align(indicator_container, LV_ALIGN_BOTTOM_MID, 0, 0);
        lv_obj_set_style_bg_opa(indicator_container, LV_OPA_TRANSP, 0);
        lv_obj_set_flex_flow(indicator_container, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(indicator_container,
                              LV_FLEX_ALIGN_CENTER,
                              LV_FLEX_ALIGN_CENTER,
                              LV_FLEX_ALIGN_CENTER);

        cp->sw1 = eos_slide_widget_create(cp->container, page, EOS_SLIDE_DIR_HOR, 0, EOS_THRESHOLD_INFINITE);
        cp->sw1->base = 0;
        cp->sw1->target = EOS_DISPLAY_WIDTH;
        eos_slide_widget_set_bidirectional(cp->sw1, true);
        lv_obj_set_size(cp->sw1->touch_obj, EOS_DISPLAY_WIDTH, EOS_DISPLAY_HEIGHT);
        lv_obj_set_pos(cp->sw1->touch_obj, 0, 0);
        cp->sw2 = cp->sw1;
        break;
    }

    default:
        break;
    }

    // 添加回调
    lv_obj_add_event_cb(cp->container, _container_delete_cb, LV_EVENT_DELETE, cp);

    lv_obj_t *shared_touch = cp->sw1->touch_obj;
    lv_obj_add_event_cb(shared_touch, _sw1_pressed_cb, LV_EVENT_PRESSED, cp);

    lv_obj_add_event_cb(shared_touch, _shared_touch_moving_cb, EOS_EVENT_SLIDE_WIDGET_MOVING, cp);

    lv_obj_add_event_cb(shared_touch, _sw1_reachd_threshold_cb, EOS_EVENT_SLIDE_WIDGET_REACHED_THRESHOLD, cp);

    lv_obj_add_event_cb(shared_touch, _sw1_reverted_cb, EOS_EVENT_SLIDE_WIDGET_REVERTED, cp);

    lv_obj_move_foreground(cp->indicator_container);
    return cp;
}
