/**
 * @file elena_os_msg_list.c
 * @brief 下拉消息列表
 * @author Sab1e
 * @date 2025-08-13
 */

#include "elena_os_msg_list.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "elena_os_lang.h"
#include "elena_os_anim.h"
#define EOS_LOG_DISABLE
#define EOS_LOG_TAG "MessageList"
#include "elena_os_log.h"
#include "elena_os_img.h"
#include "elena_os_config.h"
#include "elena_os_theme.h"
#include "elena_os_slide_widget.h"
#include "elena_os_event.h"
#include "elena_os_port.h"
/* Macros and Definitions -------------------------------------*/
#define _DEBUG_LAYOUT 0

#define _MSG_LIST_ITEM_MARGIN_BOTTOM 25

#define _ICON_WIDTH 64
#define _ICON_HEIGHT _ICON_WIDTH

#define _ICON_LARGE_WIDTH 80
#define _ICON_LARGE_HEIGHT _ICON_LARGE_WIDTH

#define _TIME_LABEL_FONT lv_font_montserrat_24

#define _ITEM_CLICK_THRESHOLD 3
// 滑动删除相关的宏定义
#define _SWIPE_THRESHOLD (EOS_DISPLAY_WIDTH / 2) // 触发删除的滑动距离阈值
#define _SWIPE_ANIM_DURATION 200                 // 滑动动画持续时间
#define _COMMON_ANIM_DURATION 150
/**
 * @brief 临时存储用户数据
 */
typedef struct
{
    eos_msg_list_item_t *item;
    lv_obj_t *detail_container;
} btn_data_t;

/* Variables --------------------------------------------------*/
static bool detail_flag = false;
static eos_msg_list_t *message_list_instance = NULL;
/* Function Implementations -----------------------------------*/

static void _msg_list_item_clicked_cb(lv_event_t *e);

/************************** 对象移动动画回调 **************************/

/**
 * @brief 水平偏移量的动画回调
 */
static void _set_translate_x_cb(void *var, int32_t value)
{
    EOS_CHECK_PTR_RETURN(var);
    lv_obj_set_style_translate_x((lv_obj_t *)var, value, 0);
}

/**
 * @brief 垂直偏移量的动画回调
 */
static void _set_translate_y_cb(void *var, int32_t value)
{
    EOS_CHECK_PTR_RETURN(var);
    lv_obj_set_style_translate_y((lv_obj_t *)var, value, 0);
}

/************************** 删除 Item 回调 **************************/

static void _del_item_cb(eos_msg_list_t *list)
{
    EOS_CHECK_PTR_RETURN(list);
    uint8_t child_count = lv_obj_get_child_count(list->list);
    if (child_count <= 1)
    {
        // 显示无消息标签
        if (list->no_msg_label)
        {
            lv_obj_remove_flag(list->no_msg_label, LV_OBJ_FLAG_HIDDEN);
        }
        // 隐藏清除按钮
        if (list->clear_all_btn)
        {
            lv_obj_add_flag(list->clear_all_btn, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

/************************** 滑动删除相关回调 **************************/

static void _slide_widget_reached_threashold_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    eos_msg_list_t *list = (eos_msg_list_t *)lv_event_get_user_data(e);
    eos_slide_widget_t *sw = (eos_slide_widget_t *)lv_event_get_param(e);
    eos_slide_widget_delete(sw);
    _del_item_cb(list);
}

/************************** 详情页相关回调 **************************/

/**
 * @brief 详情页缩小动画播放完成时的回调
 */
static void _mark_as_read_anim_end_cb(eos_anim_t *a)
{
    btn_data_t *data = (btn_data_t *)eos_anim_get_user_data(a);
    EOS_CHECK_PTR_RETURN(data);

    // 删除容器（自动删除所有子对象）
    if (data->detail_container)
    {
        lv_obj_delete(data->detail_container);
    }

    // 释放数据内存
    eos_free(data);
}

/**
 * @brief Mark as read 按钮回调
 */
static void _mark_as_read_btn_click_cb(lv_event_t *e)
{
    // 获取事件的目标对象（mark_as_read_btn）
    lv_obj_t *btn = lv_event_get_target(e);

    // 获取按钮的用户数据（包含item和list）
    btn_data_t *data = (btn_data_t *)lv_event_get_user_data(e);
    EOS_CHECK_PTR_RETURN(data && !data->item->is_deleted && btn);
    // 获取按钮的父容器（即详情页面container）
    lv_obj_t *container = lv_obj_get_parent(btn);

    eos_anim_t *anim = eos_anim_scale_create(container, EOS_DISPLAY_WIDTH, 0, EOS_DISPLAY_HEIGHT, 0, _COMMON_ANIM_DURATION, false);
    eos_anim_add_cb(anim, _mark_as_read_anim_end_cb, data);
    eos_anim_start(anim);

    // 删除消息项并更新UI状态
    if (data->item)
    {
        eos_msg_list_t *msg_list = data->item->msg_list;
        eos_msg_list_item_delete(data->item);
        if (msg_list)
        {
            _del_item_cb(msg_list);
        }
    }
    detail_flag = false;
}

/**
 * @brief msg_list_item 被按下时的回调 用于打开详情页
 */
static void _msg_list_item_clicked_cb(lv_event_t *e)
{
    eos_slide_widget_t *sw = (eos_slide_widget_t *)lv_event_get_param(e);
    if (abs(sw->last_touch_displacement) > _ITEM_CLICK_THRESHOLD)
        return;

    // 获取被点击的消息项容器
    lv_obj_t *item_container = lv_event_get_current_target(e);
    eos_msg_list_item_t *item = (eos_msg_list_item_t *)lv_obj_get_user_data(item_container);
    EOS_CHECK_PTR_RETURN(item && !item->is_deleted && item->container && !detail_flag);
    // 如果已经打开一个详情页，就不能再打开了，即便按钮继续按下。
    detail_flag = true;

    // 获取当前屏幕
    lv_obj_t *scr = lv_screen_active();

    // 创建详情页面容器
    lv_obj_t *detail_container = lv_obj_create(scr);
    lv_obj_center(detail_container);
    lv_obj_set_style_bg_color(detail_container, EOS_COLOR_BLACK, 0);
    lv_obj_set_flex_flow(detail_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_size(detail_container, lv_pct(100), lv_pct(100));
    lv_obj_set_style_pad_all(detail_container, 20, 0);
    lv_obj_set_style_border_width(detail_container, 0, 0);
    lv_obj_set_flex_align(
        detail_container,
        LV_FLEX_ALIGN_START,  // 主轴(竖直方向)居中
        LV_FLEX_ALIGN_CENTER, // 交叉轴(水平方向)
        LV_FLEX_ALIGN_CENTER  // 每行内子项的对齐方式
    );

    // 添加图标区域（复制原消息项的图标）
    lv_obj_t *original_icon = item->icon;
    if (original_icon)
    {
        lv_obj_t *icon = lv_image_create(detail_container);
        lv_obj_set_size(icon, _ICON_LARGE_WIDTH, _ICON_LARGE_HEIGHT);
        lv_obj_set_style_border_width(icon, 0, 0);
        lv_image_set_src(icon, lv_image_get_src(original_icon));
        eos_img_set_size(icon, _ICON_LARGE_WIDTH, _ICON_LARGE_HEIGHT);
        lv_obj_remove_flag(icon, LV_OBJ_FLAG_SCROLLABLE);
    }

    // 添加标题（复制原消息项的标题）
    lv_obj_t *title_label = lv_label_create(detail_container);
    lv_label_set_text(title_label, lv_label_get_text(item->title_label));

    lv_obj_set_style_margin_top(title_label, 5, 0);

    // 添加消息内容（复制原消息项的内容）
    lv_obj_t *msg_label = lv_label_create(detail_container);
    lv_label_set_text(msg_label, item->msg_str);

    lv_obj_set_width(msg_label, lv_pct(90));
    lv_label_set_long_mode(msg_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_margin_top(msg_label, 10, 0);

    // 添加"标记为已读"按钮
    lv_obj_t *mark_as_read_btn = lv_button_create(detail_container);
    lv_obj_set_size(mark_as_read_btn, lv_pct(80), 80);
    lv_obj_remove_flag(mark_as_read_btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(mark_as_read_btn, EOS_COLOR_DARK_GREY_2, 0);
    lv_obj_set_style_border_width(mark_as_read_btn, 0, 0);
    lv_obj_set_style_margin_bottom(mark_as_read_btn, _MSG_LIST_ITEM_MARGIN_BOTTOM, 0);
    lv_obj_set_style_pad_all(mark_as_read_btn, 0, 0);
    lv_obj_set_style_align(mark_as_read_btn, LV_ALIGN_CENTER, 0);
    lv_obj_set_style_radius(mark_as_read_btn, 50, 0);
    lv_obj_set_style_shadow_width(mark_as_read_btn, 0, 0);
    lv_obj_set_style_margin_top(msg_label, 10, 0);

    // 添加按钮标签
    lv_obj_t *btn_label = lv_label_create(mark_as_read_btn);
    eos_label_set_text_id(btn_label, STR_ID_MSG_LIST_ITEM_MARK_AS_READ);
    lv_obj_center(btn_label);

    btn_data_t *data = eos_malloc(sizeof(btn_data_t));
    data->item = item;
    data->detail_container = detail_container;

    // 为按钮添加点击事件，并将 item 作为用户数据传递
    lv_obj_add_event_cb(mark_as_read_btn, _mark_as_read_btn_click_cb, LV_EVENT_CLICKED, data);

    // 添加动画
    eos_anim_scale_start(detail_container,
                         0, EOS_DISPLAY_WIDTH,
                         0, EOS_DISPLAY_HEIGHT,
                         _COMMON_ANIM_DURATION, false);
}

/************************** 公共函数 **************************/

eos_msg_list_item_t *eos_msg_list_item_create(eos_msg_list_t *list)
{
    eos_msg_list_item_t *item = eos_malloc_zeroed(sizeof(eos_msg_list_item_t));
    EOS_CHECK_PTR_RETURN_VAL(list, NULL);
    EOS_CHECK_PTR_RETURN_VAL_FREE(item, NULL, item);

    item->msg_list = list;
    item->is_deleted = false;
    // 创建容器
    item->container = lv_button_create(list->list);
    EOS_CHECK_PTR_RETURN_VAL_FREE(item->container, NULL, item);
    lv_obj_set_size(item->container, lv_pct(100), LV_SIZE_CONTENT);
    // 垂直排布
    lv_obj_set_flex_flow(item->container, LV_FLEX_FLOW_COLUMN);
    lv_obj_remove_flag(item->container, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(item->container, EOS_COLOR_DARK_GREY_2, 0);
    lv_obj_set_style_border_width(item->container, 0, 0);
    lv_obj_set_style_margin_bottom(item->container, _MSG_LIST_ITEM_MARGIN_BOTTOM, 0);
    lv_obj_set_style_pad_all(item->container, 20, 0);
    lv_obj_set_style_align(item->container, LV_ALIGN_CENTER, 0);
    lv_obj_set_style_radius(item->container, 30, 0);
    lv_obj_set_user_data(item->container, item);
    lv_obj_set_style_translate_x(item->container, 0, 0);
    lv_obj_set_style_shadow_width(item->container, 0, 0);
    lv_obj_remove_flag(item->container, LV_OBJ_FLAG_SCROLL_ON_FOCUS); // 移除标志避免回抽问题
    lv_obj_update_layout(item->container);
    eos_slide_widget_t *sw = eos_slide_widget_create_with_touch(item->container, item->container, EOS_SLIDE_DIR_HOR, EOS_DISPLAY_WIDTH, EOS_THRESHOLD_40);
    eos_slide_widget_set_bidirectional(sw, true);
    lv_obj_add_event_cb(sw->touch_obj, _slide_widget_reached_threashold_cb, EOS_EVENT_SLIDE_WIDGET_REACHED_THRESHOLD, list);
    lv_obj_add_event_cb(sw->touch_obj, _msg_list_item_clicked_cb, EOS_EVENT_SLIDE_WIDGET_REVERTED, NULL);
    // 第一行
    item->row1 = lv_obj_create(item->container);
    lv_obj_set_size(item->row1, lv_pct(100), 64);
    lv_obj_set_style_bg_opa(item->row1, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(item->row1, 0, 0);
    lv_obj_set_style_pad_all(item->row1, 0, 0);
    lv_obj_remove_flag(item->row1, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(item->row1, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(item->row1, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_SPACE_BETWEEN);
#if _DEBUG_LAYOUT
    lv_obj_set_style_bg_opa(item->row1, LV_OPA_60, 0);
    lv_obj_set_style_bg_color(item->row1, EOS_COLOR_RED, 0);
#endif
    // 图标区域
    item->icon = lv_image_create(item->row1);
    lv_obj_set_size(item->icon, _ICON_WIDTH, _ICON_HEIGHT);
    lv_obj_set_style_bg_opa(item->icon, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(item->icon, 0, 0);
    lv_obj_set_style_margin_all(item->icon, 0, 0);
    lv_obj_remove_flag(item->icon, LV_OBJ_FLAG_SCROLLABLE);
    eos_img_set_src(item->icon, EOS_IMG_APP);
    eos_img_set_size(item->icon, _ICON_WIDTH, _ICON_HEIGHT);

    // 标题
    item->title_label = lv_label_create(item->row1);
    lv_label_set_long_mode(item->title_label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_flex_grow(item->title_label, 1);

    // 时间
    item->time_label = lv_label_create(item->row1);
    lv_obj_set_style_text_font(item->time_label, &_TIME_LABEL_FONT, 0);
    lv_obj_set_style_text_align(item->time_label, LV_TEXT_ALIGN_RIGHT, 0);

    // 消息内容
    item->msg_label = lv_label_create(item->container);
    lv_obj_set_width(item->msg_label, lv_pct(100));
    lv_label_set_long_mode(item->msg_label, LV_LABEL_LONG_DOT);
    lv_obj_set_style_margin_ver(item->msg_label, 16, 0);
    lv_obj_align_to(item->msg_label, item->row1, LV_ALIGN_BOTTOM_MID, 0, 0);

    // 设置忽略事件，避免影响 container 的 Clicked 事件
    lv_obj_add_flag(item->row1, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_flag(item->icon, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_flag(item->title_label, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_flag(item->time_label, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_flag(item->msg_label, LV_OBJ_FLAG_EVENT_BUBBLE);

    // 当添加新消息时处理UI状态
    if (list->no_msg_label)
    {
        lv_obj_add_flag(list->no_msg_label, LV_OBJ_FLAG_HIDDEN);
    }
    if (list->clear_all_btn)
    {
        lv_obj_remove_flag(list->clear_all_btn, LV_OBJ_FLAG_HIDDEN);
    }

    return item;
}

void eos_msg_list_item_delete(eos_msg_list_item_t *item)
{
    EOS_CHECK_PTR_RETURN(item);

    // 清除容器用户数据
    if (item->container)
    {
        lv_obj_set_user_data(item->container, NULL);
    }

    // 释放消息字符串
    if (item->msg_str)
    {
        eos_free(item->msg_str);
        item->msg_str = NULL;
    }

    // 直接删除容器（LVGL会自动删除子对象）
    if (item->container)
    {
        lv_obj_delete(item->container);
        item->container = NULL;
    }

    // 释放结构体
    eos_free(item);
}

void eos_msg_list_item_set_msg(eos_msg_list_item_t *item, const char *msg)
{
    EOS_CHECK_PTR_RETURN(item);

    // 释放旧消息
    if (item->msg_str)
    {
        eos_free(item->msg_str);
        item->msg_str = NULL;
    }

    if (!msg || strlen(msg) == 0)
    {
        lv_label_set_text(item->msg_label, "");
    }
    else
    {
        // 分配新内存并复制
        item->msg_str = eos_malloc(strlen(msg) + 1);
        if (item->msg_str)
        {
            strcpy(item->msg_str, msg);
            lv_label_set_text(item->msg_label, item->msg_str);
        }
        else
        {
            lv_label_set_text(item->msg_label, "");
        }
    }
}

void eos_msg_list_item_set_title(eos_msg_list_item_t *item, const char *title)
{
    EOS_CHECK_PTR_RETURN(item);
    lv_label_set_text(item->title_label, title ? title : "");
}

void eos_msg_list_item_set_time(eos_msg_list_item_t *item, const char *time)
{
    EOS_CHECK_PTR_RETURN(item);
    lv_label_set_text(item->time_label, time ? time : "");
}

void eos_msg_list_item_icon_set_src(eos_msg_list_item_t *item, const char *src)
{
    EOS_CHECK_PTR_RETURN(item && src);
    eos_img_set_src(item->icon, src);
    eos_img_set_size(item->icon, _ICON_WIDTH, _ICON_HEIGHT);
}

void eos_msg_list_clear_all(eos_msg_list_t *msg_list)
{
    EOS_CHECK_PTR_RETURN(msg_list && msg_list->list);

    // 获取所有子对象
    uint32_t child_count = lv_obj_get_child_count(msg_list->list);
    lv_obj_t **children = eos_malloc(sizeof(lv_obj_t *) * child_count);

    // 保存子对象指针（避免动态删除影响遍历）
    for (uint32_t i = 0; i < child_count; i++)
    {
        children[i] = lv_obj_get_child(msg_list->list, i);
    }

    // 删除子对象
    for (uint32_t i = 0; i < child_count; i++)
    {
        lv_obj_t *child = children[i];
        if (child == msg_list->clear_all_btn)
        {
            continue;
        }
        eos_msg_list_item_t *item = (eos_msg_list_item_t *)lv_obj_get_user_data(child);
        if (item)
        {
            eos_msg_list_item_delete(item);
        }
        else
        {
            lv_obj_delete(child);
        }
    }

    eos_free(children);
}

/************************** 清除按钮相关回调 **************************/

/**
 * @brief 逐个删除消息动画播放完成时的回调
 */
static void _msg_list_item_anim_end_cb(lv_anim_t *a)
{
    eos_msg_list_t *list = (eos_msg_list_t *)lv_anim_get_user_data(a);
    EOS_CHECK_PTR_RETURN(list);
    // 减少动画计数
    if (list->animating_count > 0)
    {
        list->animating_count--;
    }

    // 检查是否所有动画都完成了
    if (list->animating_count == 0)
    {
        // 直接清除所有内容
        eos_msg_list_clear_all(list);
        eos_swipe_panel_pull_back(list->swipe_panel);
        _del_item_cb(list);
    }
}

/**
 * @brief 触发消息删除动画
 * @param list 目标消息列表
 */
static void _trigger_msg_anims(eos_msg_list_t *list)
{
    EOS_CHECK_PTR_RETURN(list);
    static uint8_t anim_index = 0; // 静态变量记录当前动画序号
    lv_obj_t *parent = list->list;
    uint8_t child_count = lv_obj_get_child_count(parent);

    // 每次只处理一个消息项的动画
    for (uint8_t i = anim_index; i < child_count && list->animating_count < 2; i++)
    {
        lv_obj_t *child = lv_obj_get_child(parent, i);
        eos_msg_list_item_t *item = (eos_msg_list_item_t *)lv_obj_get_user_data(child);

        // 跳过清除按钮本身
        if (child == list->clear_all_btn)
        {
            anim_index++;
            continue;
        }

        if (item)
        {
            // 创建动画
            lv_anim_t anim;
            lv_anim_init(&anim);
            lv_anim_set_var(&anim, item->container);
            lv_anim_set_values(&anim, lv_obj_get_x(item->container), EOS_DISPLAY_WIDTH);
            lv_anim_set_time(&anim, 120);
            lv_anim_set_exec_cb(&anim, _set_translate_x_cb);
            lv_anim_set_user_data(&anim, list);
            lv_anim_set_completed_cb(&anim, _msg_list_item_anim_end_cb);
            lv_anim_start(&anim);

            list->animating_count++;
            anim_index++;
        }
    }

    anim_index = 0;
}

/**
 * @brief 清除所有消息按钮回调
 */
static void _msg_list_clear_all_btn_cb(lv_event_t *e)
{
    eos_msg_list_t *list = (eos_msg_list_t *)lv_event_get_user_data(e);
    EOS_CHECK_PTR_RETURN(list);
    // 隐藏无消息标签
    if (list->no_msg_label)
    {
        lv_obj_add_flag(list->no_msg_label, LV_OBJ_FLAG_HIDDEN);
    }

    // 重置动画计数
    list->animating_count = 0;

    // 触发动画
    _trigger_msg_anims(list);
}

/************************** List相关函数 **************************/

static void _msg_list_deleted_cb(lv_event_t *e)
{
    eos_msg_list_t *list = (eos_msg_list_t *)lv_event_get_user_data(e);
    EOS_CHECK_PTR_RETURN(list);

    // 清除所有消息项
    eos_msg_list_clear_all(list);

    // 删除无消息标签
    if (list->no_msg_label)
    {
        lv_obj_delete(list->no_msg_label);
        list->no_msg_label = NULL;
    }

    // 删除滑动面板（会自动删除内部所有对象）
    if (list->swipe_panel)
    {
        eos_swipe_panel_delete(list->swipe_panel);
        list->swipe_panel = NULL;
    }

    // 释放列表结构体
    eos_free(list);
}

eos_msg_list_t *eos_msg_list_create(lv_obj_t *parent)
{
    EOS_CHECK_PTR_RETURN_VAL(parent, NULL);
    eos_msg_list_t *list = eos_malloc_zeroed(sizeof(eos_msg_list_t));
    EOS_CHECK_PTR_RETURN_VAL_FREE(list, NULL, list);
    detail_flag = false;

    list->swipe_panel = eos_swipe_panel_create(parent);
    EOS_CHECK_PTR_RETURN_VAL_FREE(list->swipe_panel, NULL, list);
    eos_swipe_panel_set_dir(list->swipe_panel, EOS_SWIPE_DIR_DOWN);

    list->list = lv_list_create(list->swipe_panel->swipe_obj);
    EOS_CHECK_PTR_RETURN_VAL_FREE(list->list, NULL, list);
    lv_obj_set_size(list->list, LV_PCT(100), LV_PCT(88));
    lv_obj_center(list->list);
    lv_obj_set_style_bg_opa(list->list, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(list->list, 0, 0);
    lv_obj_set_style_pad_all(list->list, 30, 0);
    lv_obj_align(list->list, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_user_data(list->list, list);
    lv_obj_set_scroll_dir(list->list, LV_DIR_VER);
    lv_obj_add_event_cb(list->list, _msg_list_deleted_cb, LV_EVENT_DELETE, list);
    // 创建清除所有按钮
    list->clear_all_btn = lv_button_create(list->list);
    lv_obj_set_size(list->clear_all_btn, lv_pct(100), 80);
    lv_obj_remove_flag(list->clear_all_btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(list->clear_all_btn, EOS_COLOR_DARK_GREY_2, 0);
    lv_obj_set_style_border_width(list->clear_all_btn, 0, 0);
    lv_obj_set_style_margin_bottom(list->clear_all_btn, _MSG_LIST_ITEM_MARGIN_BOTTOM, 0);
    lv_obj_set_style_pad_all(list->clear_all_btn, 0, 0);
    lv_obj_set_style_align(list->clear_all_btn, LV_ALIGN_CENTER, 0);
    lv_obj_set_style_radius(list->clear_all_btn, 50, 0);
    lv_obj_set_style_shadow_width(list->clear_all_btn, 0, 0);

    // 初始时隐藏清除按钮
    lv_obj_add_flag(list->clear_all_btn, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t *clear_all_label = lv_label_create(list->clear_all_btn);
    eos_label_set_text_id(clear_all_label, STR_ID_MSG_LIST_CLEAR_ALL);

    lv_obj_center(clear_all_label);

    lv_obj_add_event_cb(list->clear_all_btn, _msg_list_clear_all_btn_cb, LV_EVENT_CLICKED, list);

    // 创建无消息标签
    list->no_msg_label = lv_label_create(list->swipe_panel->swipe_obj);
    eos_label_set_text_id(list->no_msg_label, STR_ID_MSG_LIST_NO_MSG);

    lv_obj_center(list->no_msg_label);

    return list;
}

void eos_msg_list_hide(void)
{
    EOS_CHECK_PTR_RETURN(message_list_instance);
    eos_swipe_panel_hide(message_list_instance->swipe_panel);
}

void eos_msg_list_show(void)
{
    EOS_CHECK_PTR_RETURN(message_list_instance);
    eos_swipe_panel_show(message_list_instance->swipe_panel);
}

eos_msg_list_t *eos_msg_list_get_instance(void)
{
    return message_list_instance;
}

void eos_msg_list_init(void)
{
    message_list_instance = eos_msg_list_create(lv_layer_top());
    eos_msg_list_hide();
}
