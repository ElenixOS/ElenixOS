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
 * 选择字体源类型
 *
 * `EOS_FONT_C_*`使用 LVGL Font Converter 生成的 C 文字字库，或 LVGL 内置的字体
 *
 * 可用格式:
 *  - EOS_FONT_C_SCALE:     只存储一种点阵字体，多级字号使用缩放效果实现
 *  - EOS_FONT_C_MULTI:     存储多种点阵字体，多级字号使用不同字体
 *  - EOS_FONT_TTF:         使用文件系统的TTF字体文件
 */
#define EOS_FONT_TYPE EOS_FONT_TTF

/**
 * 配置字体对应字号，下面三个配置分配对应三种字体：
 * - EOS_FONT_SIZE_LARGE
 * - EOS_FONT_SIZE_MEDIUM
 * - EOS_FONT_SIZE_SMALL
 */
#define EOS_FONT_CFG_LARGE_SIZE     30
#define EOS_FONT_CFG_MEDIUM_SIZE    EOS_FONT_CFG_LARGE_SIZE - 4
#define EOS_FONT_CFG_SMALL_SIZE     EOS_FONT_CFG_MEDIUM_SIZE - 4

#if EOS_FONT_TYPE == EOS_FONT_C_SCALE

    #define EOS_FONT_C_NAME             source_han_sans_30  /**< 字体名称 */
    #define EOS_FONT_C_SIZE             30                  /**< C 字体的大小 */

#elif EOS_FONT_TYPE == EOS_FONT_C_MULTI

    #define EOS_FONT_LARGE_NAME         source_han_sans_30
    #define EOS_FONT_MEDIUM_NAME        source_han_sans_26
    #define EOS_FONT_SMALL_NAME         source_han_sans_22

#elif EOS_FONT_TYPE == EOS_FONT_TTF

/**
 * 选择 TTF 数据格式
 * 可用格式:
 *  - EOS_FONT_TTF_DATA:    将 TTF 字体数据编译进固件
 *  - EOS_FONT_TTF_FILE:    从文件系统加载 TTF 字体文件
 */
#define EOS_FONT_TTF_TYPE EOS_FONT_TTF_FILE

#if EOS_FONT_TTF_TYPE == EOS_FONT_TTF_DATA

    #define EOS_FONT_TTF_DATA_NAME  SourceHanSansSC_12M
    #define EOS_FONT_TTF_DATA_SIZE  SourceHanSansSC_12M_size

#elif EOS_FONT_TTF_TYPE == EOS_FONT_TTF_FILE

    #define LV_FS_STDIO_LETTER_STR "A"
    #define EOS_FONT_TTF_FILE_PATH LV_FS_STDIO_LETTER_STR":" EOS_SYS_RES_FONT_DIR "SourceHanSansCN-Bold.ttf"

#endif /* EOS_FONT_TTF_TYPE */

/**
 * 启用额外功能
 */

#define EOS_FONT_TTF_ENABLE_EXTENDED 1

#if EOS_FONT_TTF_ENABLE_EXTENDED

    #define EOS_FONT_TTF_KERNING     0      /**< 字距，单位：px */
    #define EOS_FONT_TTF_CACHE_SIZE  256    /**< TTF 字体缓存大小 */

#endif /* EOS_FONT_TTF_ENABLE_EXTENDED */

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
#endif /* EOS_SYS_ROOT_DIR */

/************************** 电量检测 **************************/

#define EOS_BATTERY_DISPLAY_UPDATE_MS  5*60*1000      /**< 电池电量显示更新时间 */

/************************** 音量配置 **************************/

#define EOS_SPEAKER_VOLUME_MIN 0
#define EOS_SPEAKER_VOLUME_MAX 100

/************************** 日志 **************************/

// 是否启用断言
#define EOS_USE_ASSERT    1

// 是否启用彩色日志
#define EOS_LOG_USE_COLOR 0

/************************** 配置结束 **************************/

#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_CONFIG_H */
