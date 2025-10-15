/**
 * @file elena_os_config.h
 * @brief 系统配置
 * @author Sab1e
 * @date 2025-09-13
 */

#ifndef ELENA_OS_CONFIG_H
#define ELENA_OS_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include "elena_os_sys.h"
#include "elena_os_config_internal.h"

/* Public typedefs --------------------------------------------*/

/* Public macros ----------------------------------------------*/

/************************** 配置开始 **************************/

/************************** 字体配置 **************************/

/**
 * 选择字体格式
 * 可用格式:
 *  - EOS_FONT_USE_LVGL:    LVGL内置的 Montserrat 字体
 *  - EOS_FONT_USE_C:       使用 LVGL Font Converter 生成的 C 文字字库
 *  - EOS_FONT_USE_TTF:     使用文件系统的TTF字体文件
 */
#define EOS_FONT_TYPE EOS_FONT_USE_C

#if EOS_FONT_TYPE == EOS_FONT_USE_LVGL
#define EOS_FONT_LVGL   lv_font_montserrat_30
#elif EOS_FONT_TYPE == EOS_FONT_USE_C
#define EOS_FONT_C_NAME          source_han_sans_30
#elif EOS_FONT_TYPE == EOS_FONT_USE_TTF
#define EOS_FONT_TTF_PATH        EOS_SYS_RES_FONT_DIR "font.ttf"
#endif /* EOS_FONT_TYPE */

/************************** 显示配置 **************************/

#define EOS_DISPLAY_WIDTH   lv_display_get_horizontal_resolution(NULL)      /**< 显示器宽度 */
#define EOS_DISPLAY_HEIGHT  lv_display_get_vertical_resolution(NULL)        /**< 显示器高度 */

#define EOS_DISPLAY_RADIUS  80          /**< 显示器圆角半径 */

#define EOS_DISPLAY_BRIGHTNESS_MIN 5    /**< 亮度为0即关闭屏幕 */
#define EOS_DISPLAY_BRIGHTNESS_MAX 100

/************************** 系统文件目录配置 **************************/

#ifndef EOS_SYS_ROOT_DIR
#define EOS_SYS_ROOT_DIR "/"     /**< 系统根目录 例如：`/user/elenaos/` */
#endif

/************************** 电量检测 **************************/

#define EOS_BATTERY_DISPLAY_UPDATE_MS  5*60*1000      /**< 电池电量显示更新时间 */

/************************** 音量配置 **************************/

#define EOS_SPEAKER_VOLUME_MIN 0
#define EOS_SPEAKER_VOLUME_MAX 100

/************************** 日志 **************************/

// 是否启用断言
#ifndef EOS_USE_ASSERT
#define EOS_USE_ASSERT    1
#endif

// 是否启用彩色日志
#ifndef EOS_LOG_USE_COLOR
#define EOS_LOG_USE_COLOR 0
#endif

/************************** 配置结束 **************************/

#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_CONFIG_H */
