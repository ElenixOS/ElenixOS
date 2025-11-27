/**
 * @file elena_os_radio_list.h
 * @brief 单选列表页面
 * @author Sab1e
 * @date 2025-11-25
 */

#ifndef ELENA_OS_RADIO_LIST_H
#define ELENA_OS_RADIO_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"
/* Public macros ----------------------------------------------*/

#define EOS_INVALID_RADIO_INDEX UINT32_MAX      /**< 无效索引值 */

/* Public typedefs --------------------------------------------*/

typedef struct eos_radio_list_t eos_radio_list_t;

/* Public function prototypes --------------------------------*/

/**
 * @brief 向单选列表内添加新的选项
 *
 * 选项的索引值随着列表内选项从0递增，例如：
 *
 * ```c
 * eos_radio_list_t *rl = eos_radio_list_create("Title")
 * eos_radio_list_add_item(rl,"Option 1");     // index = 0
 * eos_radio_list_add_item(rl,"Option 2");     // index = 1
 * eos_radio_list_add_item(rl,"Option 3");     // index = 2
 * ```
 * @param rl 单选列表指针
 * @param txt 选项文字符串
 * @return uint32_t 创建成功则返回单选项的索引值，否则返回`EOS_INVALID_RADIO_INDEX`
 */
uint32_t eos_radio_list_add_item(eos_radio_list_t *rl, const char *txt);

/**
 * @brief 向单选列表的头部添加子标题
 * @param subtitle 子标题字符串
 */
void eos_radio_list_set_subtitle(eos_radio_list_t *rl, const char *subtitle);

/**
 * @brief 向单选列表的底部添加注释
 * @param comment 注释字符串
 */
void eos_radio_list_set_comment(eos_radio_list_t *rl, const char *comment);

/**
 * @brief 添加单选列表选中时的回调函数
 * @param event_cb 标准的`lv_event_cb_t`，使用`lv_event_get_param()`可以获取选中项目的索引值（0基索引）
 * @param user_data 用户数据
 */
void eos_radio_list_add_event_cb(eos_radio_list_t *rl, lv_event_cb_t event_cb, void *user_data);

/**
 * @brief 创建新的单选列表
 * @param title 标题字符串
 * @return eos_radio_list_t* 创建成功则返回单选列表指针，否则返回 NULL
 */
eos_radio_list_t *eos_radio_list_create(const char *title);

/**
 * @brief 给指定索引的单选项打勾✅
 * @param index 目标单选项的索引值
 */
void eos_radio_list_check(eos_radio_list_t *rl, uint32_t index);

#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_RADIO_LIST_H */
