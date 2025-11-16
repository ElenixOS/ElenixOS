/**
 * @file elena_os_font.h
 * @brief 字体系统
 * @author Sab1e
 * @date 2025-10-31
 */

#ifndef ELENA_OS_FONT_H
#define ELENA_OS_FONT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"
#include "elena_os_config.h"
#include "elena_os_log.h"

/* Public macros ----------------------------------------------*/
#define EOS_FONT_ICON RemixIcon
#define EOS_FONT_DATA_DECLARE(font_name) LV_ATTRIBUTE_EXTERN_DATA extern const unsigned char font_name[]
#define EOS_FONT_DATA_SIZE_DECLARE(font_size) LV_ATTRIBUTE_EXTERN_DATA extern const int font_size
/* Public typedefs --------------------------------------------*/

typedef enum
{
    EOS_FONT_SIZE_LARGE = EOS_FONT_CFG_LARGE_SIZE,
    EOS_FONT_SIZE_MEDIUM = EOS_FONT_CFG_MEDIUM_SIZE,
    EOS_FONT_SIZE_SMALL = EOS_FONT_CFG_SMALL_SIZE,
} eos_font_size_t;

/* Public function prototypes --------------------------------*/

/**
 * @brief 字体系统初始化
 * @return lv_font_t* 创建成功则返回默认字体指针，否则返回NULL
 */
lv_font_t * eos_font_init(void);

/**
 * @brief 设置标签对象的字体大小
 * @param label 目标标签
 * @param size 字号，支持任意大小，单位：像素
 * @note `EOS_FONT_BASE_SIZE`为字体源的大小，`size`大于此字号时会失真。
 */
void eos_label_set_font_size(lv_obj_t *label, eos_font_size_t size);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_FONT_H */
