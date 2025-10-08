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
#define EOS_FONT_C_NAME          eos_font_resource_han_rounded_30
#elif EOS_FONT_TYPE == EOS_FONT_USE_TTF
#define EOS_FONT_TTF_PATH        EOS_SYS_RES_FONT_DIR "font.ttf"
#endif /* EOS_FONT_TYPE */

/************************** 显示配置 **************************/

#define EOS_DISPLAY_BRIGHTNESS_MIN 5    /**< 亮度为0即关闭屏幕 */
#define EOS_DISPLAY_BRIGHTNESS_MAX 100

/************************** 系统文件目录配置 **************************/

#ifndef EOS_SYS_ROOT_DIR
#define EOS_SYS_ROOT_DIR "/"     /**< 系统根目录 例如：`/user/elenaos/` */
#endif

/************************** 电量检测 **************************/

#define EOS_BATTERY_DISPLAY_UPDATE_MS  1*1000      /**< 电池电量显示更新时间 */

/************************** 配置结束 **************************/

#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_CONFIG_H */
