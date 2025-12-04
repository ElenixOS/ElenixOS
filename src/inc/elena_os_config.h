/**
 * @file elena_os_config.h
 * @brief 系统配置
 * @author Sab1e
 * @date 2025-09-13
 */

#if 1 /* 启用配置 */

#ifndef ELENA_OS_CONFIG_H
#define ELENA_OS_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------*/
#include "elena_os_sys.h"
#include "elena_os_sensor.h"
#include "elena_os_config_internal.h"

/* Public typedefs --------------------------------------------*/

/* Public macros ----------------------------------------------*/

/* 配置开始 ----------------------------------------------------*/

/**
 * 选择编译模式
 * 可用模式:
 *  - DEBUG:        调试模式，启用所有初始化检查。
 *  - RELEASE:      发布模式，关闭所有初始化检查。
 */
#ifndef EOS_COMPILE_MODE
#define EOS_COMPILE_MODE DEBUG
#endif

/**
 * 选择系统启动模式
 * 可用模式:
 *  - NORMAL_MODE:      正常模式，直接进入表盘
 *  - TEST_MODE:        测试模式，进入测试页面
 */
#define EOS_SYSTEM_MODE NORMAL_MODE

/**
 * 选择是否启用动画
 */
#define EOS_ANIMATION_ENABLE 0

/**
 * 选择是否将图标的字符串构建入 JerryScript
 */
#define EOS_ICON_STR_BUILD_IN 0

/**
 * 选择系统监控类型
 * 可用模式:
 *  - EOS_SYSMON_DISABLE
 *  - EOS_SYSMON_USE_INTERNAL
 *  - EOS_SYSMON_USE_CUSTOM
 */
#define EOS_SYSMON_TYPE EOS_SYSMON_DISABLE

#if EOS_SYSMON_TYPE == EOS_SYSMON_USE_CUSTOM
    #define EOS_SYSMON_HANDLER my_sysmon_handler();
#endif /* EOS_SYSMON_TYPE */

/************************** 模块 **************************/

/**
 * 启用虚拟显示器模块
 */
#define EOS_USE_VIRTUAL_DISPLAY 1

/************************** 内存配置 **************************/

/**
 * 选择内存分配模式
 * 可用选项:
 *  - EOS_MEM_PROVIDER_STDLIB_CLIB:    使用标准库的内存分配函数
 *  - EOS_MEM_PROVIDER_AUTO:           使用自动分配方法
 *  - EOS_MEM_PROVIDER_CUSTOM:         使用自定义的分配函数（覆盖弱定义的`eos_malloc_core()`等函数即可）
 */
#ifndef EOS_MEM_ALLOC_PROVIDER
    #define EOS_MEM_ALLOC_PROVIDER EOS_MEM_PROVIDER_STDLIB_CLIB
#endif /* EOS_MEM_ALLOC_PROVIDER */

#if EOS_MEM_ALLOC_PROVIDER == EOS_MEM_PROVIDER_AUTO
    #define EOS_MEM_POOL_ALLOC_THRESHOLD 32/*KB*/ * 1024/*B*/    /**< 分配阈值，单位：字节 */

    #define EOS_MEM_PROVIDER_AUTO_INCLUDE    "mem_mgr.h"

    #define EOS_MEM_ALLOC_FAST malloc
    #define EOS_MEM_ALLOC_LARGE mem_mgr_alloc

    #define EOS_MEM_FREE_FAST free
    #define EOS_MEM_FREE_LARGE mem_mgr_free

    #define EOS_MEM_REALLOC_FAST realloc
    #define EOS_MEM_REALLOC_LARGE mem_mgr_realloc

    #define EOS_MEM_CALLOC_FAST calloc
    #define EOS_MEM_CALLOC_LARGE mem_mgr_calloc
#endif /* EOS_MEM_ALLOC_PROVIDER */

/**
 * 是否让 ElenaOS 覆盖 LVGL 的标准库 malloc
 *
 * - 当此项关闭时：
 *   LVGL 在需要分配显示相关的大块内存（如图片、截图缓冲等）时，
 *   最终都会调用 ElenaOS 的 `eos_malloc()` 来完成分配，
 *   因此这些显示用内存实际上仍由 ElenaOS 分配和管理。
 *
 * - 当此项开启时：
 *   LVGL 的所有内存申请（包括显示相关和其它内部数据）都将完全交由
 *   ElenaOS 的分配函数处理，由 ElenaOS 统一管理全部内存。
 */
#define EOS_OVERRIDE_LVGL_STDLIB_MALLOC_ENABLE \
    (LV_USE_STDLIB_MALLOC == LV_STDLIB_CUSTOM)

#if EOS_OVERRIDE_LVGL_STDLIB_MALLOC_ENABLE
    #define EOS_MEM_TRACK_ENABLE LV_USE_MEM_MONITOR /**< 是否启用内存检测 */
#endif /* EOS_OVERRIDE_LVGL_STDLIB_MALLOC_ENABLE */

/************************** 传感器配置 **************************/

#define EOS_SENSOR_CFG_INST_MAX 1 /**< 传感器实例最大值，同一类型的传感器允许存在的最大值，一般设置为`1`即可 */

/************************** 字体配置 **************************/

/**
 * 选择字体源类型
 *
 * `EOS_FONT_C_*`使用 LVGL Font Converter 生成的 C 文字字库，或 LVGL 内置的字体
 *
 * 可用格式:
 *  - EOS_FONT_C_MULTI:     存储多种点阵字体，多级字号使用不同字体
 *  - EOS_FONT_TTF:         使用文件系统的TTF字体文件
 */
#define EOS_FONT_TYPE EOS_FONT_C_MULTI

/**
 * 配置字体对应字号，下面三个配置分配对应三种字体：
 * - EOS_FONT_SIZE_LARGE
 * - EOS_FONT_SIZE_MEDIUM
 * - EOS_FONT_SIZE_SMALL
 */
#define EOS_FONT_CFG_LARGE_SIZE 30
#define EOS_FONT_CFG_MEDIUM_SIZE EOS_FONT_CFG_LARGE_SIZE - 4
#define EOS_FONT_CFG_SMALL_SIZE EOS_FONT_CFG_MEDIUM_SIZE - 4

#if EOS_FONT_TYPE == EOS_FONT_C_MULTI

#define EOS_ENABLE_CHINESE_FONT 1

#if EOS_ENABLE_CHINESE_FONT

#define EOS_FONT_LARGE_NAME source_han_sans_30
#define EOS_FONT_MEDIUM_NAME source_han_sans_26
#define EOS_FONT_SMALL_NAME source_han_sans_22

#else

#define EOS_FONT_LARGE_NAME lv_font_montserrat_30
#define EOS_FONT_MEDIUM_NAME lv_font_montserrat_30
#define EOS_FONT_SMALL_NAME lv_font_montserrat_30

#endif

#elif EOS_FONT_TYPE == EOS_FONT_TTF

/**
 * 选择 TTF 数据格式
 * 可用格式:
 *  - EOS_FONT_TTF_DATA:    将 TTF 字体数据编译进固件
 *  - EOS_FONT_TTF_FILE:    从文件系统加载 TTF 字体文件
 */
#define EOS_FONT_TTF_TYPE EOS_FONT_TTF_FILE

#if EOS_FONT_TTF_TYPE == EOS_FONT_TTF_DATA
    #define EOS_FONT_TTF_DATA_NAME SourceHanSansSC_12M
    #define EOS_FONT_TTF_DATA_SIZE SourceHanSansSC_12M_size
#elif EOS_FONT_TTF_TYPE == EOS_FONT_TTF_FILE
    #define LV_FS_STDIO_LETTER_STR "A"
    #define EOS_FONT_TTF_FILE_PATH LV_FS_STDIO_LETTER_STR ":" EOS_SYS_RES_FONT_DIR "MiSans-Normal.ttf"
#endif /* EOS_FONT_TTF_TYPE */

/**
 * 启用额外功能
 */
#define EOS_FONT_TTF_ENABLE_EXTENDED 0

#if EOS_FONT_TTF_ENABLE_EXTENDED
    #define EOS_FONT_TTF_KERNING 0      /**< 字距，单位：px */
    #define EOS_FONT_TTF_CACHE_SIZE 256 /**< TTF 字体缓存大小 */
#endif /* EOS_FONT_TTF_ENABLE_EXTENDED */

#endif /* EOS_FONT_TYPE */

/************************** 显示配置 **************************/

#define EOS_DISPLAY_WIDTH 390    /**< 绘制宽度 */
#define EOS_DISPLAY_HEIGHT 450     /**< 绘制高度 */

#define EOS_DISPLAY_RADIUS 80 /**< 显示器圆角半径 */

#define EOS_DISPLAY_BRIGHTNESS_MIN 5 /**< 亮度为0即关闭屏幕 */
#define EOS_DISPLAY_BRIGHTNESS_MAX 100

/************************** 系统文件目录配置 **************************/

#ifndef EOS_SYS_ROOT_DIR
    #define EOS_SYS_ROOT_DIR "/" /**< 系统根目录 例如：`/user/elenaos/` */
#endif /* EOS_SYS_ROOT_DIR */

/**
 * 选择文件系统接口类型
 * 可选选项:
 *  - EOS_FS_POSIX
 *  - EOS_FS_FATFS
 *  - EOS_FS_LITTLEFS
 *  - EOS_FS_RTTHREAD
 *  - EOS_FS_CUSTOM
 */
#ifndef EOS_FS_TYPE
    #define EOS_FS_TYPE EOS_FS_POSIX
#endif /* EOS_FS_TYPE */

#if EOS_FS_TYPE == EOS_FS_CUSTOM
    #define EOS_FS_FILE_TYPE void*          /**< 文件的数据类型 */
    #define EOS_FS_DIR_TYPE void*           /**< 目录的数据类型 */
    #define EOS_FILE_INVALID NULL           /**< 无效的文件 */
    #define EOS_DIR_INVALID NULL            /**< 无效的目录 */
#endif /* EOS_FS_TYPE */

#define EOS_DFW_ENABLE 0        /**< 是否启用延后文件写入器 */

/************************** 电量检测 **************************/

#define EOS_BATTERY_DISPLAY_UPDATE_MS 5 * 60 * 1000 /**< 电池电量显示更新时间 */

/************************** 音量配置 **************************/

#define EOS_SPEAKER_VOLUME_MIN 0
#define EOS_SPEAKER_VOLUME_MAX 100

/************************** MISC **************************/

#define EOS_ASYNC_SEM_TIMEOUT 1000

/************************** 日志 **************************/

// 是否启用断言
#define EOS_USE_ASSERT 1
#define EOS_ASSERT_HANDLER while(1);

// 是否启用彩色日志
#define EOS_LOG_USE_COLOR 0

/* 配置结束 ----------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_CONFIG_H */

#endif /* 启用配置 */
