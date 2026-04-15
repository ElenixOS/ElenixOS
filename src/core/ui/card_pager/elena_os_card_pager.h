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
 * eos_card_pager_t *cp = eos_card_pager_create(eos_screen_active(), EOS_CARD_PAGER_DIR_HOR);
 * lv_obj_t *page = eos_card_pager_create_page(cp);
 * lv_obj_set_style_bg_color(page, EOS_COLOR_GREEN, 0);
 * page = eos_card_pager_create_page(cp);
 * lv_obj_set_style_bg_color(page, EOS_COLOR_YELLOW, 0);
 * page = eos_card_pager_create_page(cp);
 * lv_obj_set_style_bg_color(page, EOS_COLOR_ORANGE, 0);
 * page = eos_card_pager_create_page(cp);
 * lv_obj_set_style_bg_color(page, EOS_COLOR_RED, 0);
 * eos_card_pager_remove_page(cp,3);
 * page = eos_card_pager_create_page(cp);
 * lv_obj_set_style_bg_color(page, EOS_COLOR_MINT, 0);
 * eos_card_pager_set_loop(cp, false); // 开启循环
 * eos_card_pager_move_node(cp,0,3);
 * eos_card_pager_move_page(cp,4);
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

/**
 * @brief 方向定义
 */
typedef enum
{
    EOS_CARD_PAGER_DIR_VER,
    EOS_CARD_PAGER_DIR_HOR
} eos_card_pager_dir_t;

typedef struct eos_card_pager_node_t eos_card_pager_node_t;     // 预定义
typedef struct eos_card_pager_t eos_card_pager_t;

typedef void (*eos_card_pager_page_changed_cb_t)(eos_card_pager_t *cp,
                                                  uint8_t current_page_index,
                                                  void *user_data);

/**
 * @brief 双向链表存储页面
 */
struct eos_card_pager_node_t
{
    lv_obj_t *page;
    lv_obj_t *indicator;
    eos_card_pager_node_t *prev;
    eos_card_pager_node_t *next;
};

/**
 * @brief 卡片式页面结构体定义
 */
struct eos_card_pager_t
{
    lv_obj_t *container;
    lv_obj_t *background;
    lv_obj_t *indicator_container;
    eos_card_pager_node_t *page_list_head;
    uint8_t current_page_index;
    uint8_t page_count;
    eos_slide_widget_t *sw1;
    eos_slide_widget_t *sw2;
    eos_card_pager_dir_t dir;
    bool loop;
    eos_card_pager_page_changed_cb_t page_changed_cb;
    void *page_changed_user_data;
};
/* Public function prototypes --------------------------------*/

/**
 * @brief 创建卡片式页面管理器
 * @param parent 父级LVGL对象，页面将创建在此对象内
 * @param dir 页面滑动方向（水平EOS_CARD_PAGER_DIR_HOR或垂直EOS_CARD_PAGER_DIR_VER）
 * @return eos_card_pager_t* 成功返回页面管理器指针，失败返回NULL
 * @note 创建的页面管理器包含背景层、页面容器和指示器
 */
eos_card_pager_t *eos_card_pager_create(lv_obj_t *parent, eos_card_pager_dir_t dir);

/**
 * @brief 在页面管理器中创建新页面
 * @param cp 页面管理器指针
 * @return lv_obj_t* 成功返回新创建的页面对象，失败返回NULL
 * @note 每个新页面会自动创建对应的指示器，并更新页面导航逻辑
 */
lv_obj_t *eos_card_pager_create_page(eos_card_pager_t *cp);

/**
 * @brief 设置页面循环模式
 * @param cp 页面管理器指针
 * @param loop true启用循环模式，false禁用循环模式
 * @note 循环模式下，从最后一页滑动会自动跳转到第一页，反之亦然
 */
void eos_card_pager_set_loop(eos_card_pager_t *cp, bool loop);

/**
 * @brief 删除指定页面
 * @param cp 页面管理器指针
 * @param page_index 要删除的页面索引（0基）
 * @return bool 成功返回true，失败返回false
 * @note 如果删除的是当前显示页面，会自动切换到首页(索引0)
 */
bool eos_card_pager_remove_page(eos_card_pager_t *cp, uint8_t page_index);

/**
 * @brief 获取指定页面对象
 * @param cp 页面管理器指针
 * @param page_index 页面索引（0基）
 * @return lv_obj_t* 成功返回页面对象，失败返回NULL
 */
lv_obj_t *eos_card_pager_get_page(eos_card_pager_t *cp, uint8_t page_index);

/**
 * @brief 获取指定页面的指示器对象
 * @param cp 页面管理器指针
 * @param page_index 页面索引（0基）
 * @return lv_obj_t* 成功返回指示器对象，失败返回NULL
 * @note 指示器用于显示当前页面位置的小圆点
 */
lv_obj_t *eos_card_pager_get_indicator(eos_card_pager_t *cp, uint8_t page_index);

/**
 * @brief 切换到指定页面
 * @param cp 页面管理器指针
 * @param page_index 目标页面索引（0基）
 * @note 会自动更新指示器状态和页面导航逻辑
 */
void eos_card_pager_move_page(eos_card_pager_t *cp, uint8_t page_index);

/**
 * @brief 移动页面节点位置
 * @param cp 页面管理器指针
 * @param from_index 要移动的页面索引（0基）
 * @param to_index 目标位置索引（0基）
 * @return bool 成功返回true，失败返回false
 * @note 此操作会改变页面的逻辑顺序，但不会立即改变显示状态
 *       如果移动的是当前显示页面，会更新当前页面索引
 */
bool eos_card_pager_move_node(eos_card_pager_t *cp, uint8_t from_index, uint8_t to_index);

void eos_card_pager_set_page_changed_cb(eos_card_pager_t *cp,
                                        eos_card_pager_page_changed_cb_t cb,
                                        void *user_data);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_CARD_PAGER_H */
