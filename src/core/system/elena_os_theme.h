/**
 * @file elena_os_theme.h
 * @brief 主题色
 * @author Sab1e
 * @date 2025-08-27
 */

#ifndef ELENA_OS_THEME_H
#define ELENA_OS_THEME_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"
/* Public macros ----------------------------------------------*/

#define EOS_COLOR_RED         lv_color_hex(0xFF3B30)
#define EOS_COLOR_ORANGE      lv_color_hex(0xFF9500)
#define EOS_COLOR_YELLOW      lv_color_hex(0xFFE620)
#define EOS_COLOR_GREEN       lv_color_hex(0x04DE71)
#define EOS_COLOR_MINT        lv_color_hex(0x00F5EA)
#define EOS_COLOR_TEAL_BLUE   lv_color_hex(0x5AC8FA)
#define EOS_COLOR_BLUE        lv_color_hex(0x2094FA)
#define EOS_COLOR_PURPLE      lv_color_hex(0x787AFF)
#define EOS_COLOR_PINK        lv_color_hex(0xFA114F)
#define EOS_COLOR_WHITE       lv_color_hex(0xFFFFFF)
#define EOS_COLOR_BLACK       lv_color_hex(0x000000)
#define EOS_COLOR_DARK_GREY_2 lv_color_hex(0x242424)
#define EOS_COLOR_DARK_GREY_1 lv_color_hex(0x48494B)
#define EOS_COLOR_GREY        lv_color_hex(0x9BA0AA)
#define EOS_COLOR_GREY_1      lv_color_hex(0x727272)
#define EOS_COLOR_TEXT_GREY   lv_color_hex(0xAEB4BF)

#define EOS_THEME_PRIMARY_COLOR     EOS_COLOR_BLUE
#define EOS_THEME_SECONDARY_COLOR   EOS_COLOR_DARK_GREY_2
#define EOS_THEME_DANGEROS_COLOR    EOS_COLOR_RED

#define EOS_THEME_BUTTON_HEIGHT 100

/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/
/**
 * @brief 设置系统主题色
 * @param primary_color 主色
 * @param secondary_color 副色
 * @param font 字体
 */
void eos_theme_set(lv_color_t primary_color, lv_color_t secondary_color, lv_font_t *font);
/**
 * @brief 获取当前的 Screen 样式对象
 * @return lv_style_t*
 */
lv_style_t *eos_theme_get_screen_style(void);
/**
 * @brief 获取当前的 Label 样式对象
 * @return lv_style_t*
 */
lv_style_t *eos_theme_get_label_style(void);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_THEME_H */
