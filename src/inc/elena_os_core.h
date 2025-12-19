/**
 * @file elena_os_core.h
 * @brief Elena OS 核心头文件
 * @author Sab1e
 * @date 2025-08-10
 * @details
 *
 * # Elena OS
 *
 * ## 系统启动
 *
 * 启动 Elena OS 只需要调用`eos_run()`即可。
 *
 * ## 侧键
 *
 * 侧键需要在按键的回调函数中调用`eos_side_btn_handler()`并设置按键状态。
 *
 */

#ifndef ELENA_OS_CORE_H
#define ELENA_OS_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"

/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/**
 * @brief ElenaOS 返回值状态
 */
typedef enum
{
    EOS_OK = 0,                  /**< 操作成功 */
    EOS_FAILED,                  /**< 操作失败 */
    EOS_ERR_MEM,                 /**< 操作失败：内存相关错误 */
    EOS_ERR_STACK_EMPTY,         /**< 操作失败：栈空 */
    EOS_ERR_STACK_FULL,          /**< 操作失败：栈满 */
    EOS_ERR_VAR_NOT_NULL,        /**< 操作失败：变量不为空 */
    EOS_ERR_VAR_NULL,            /**< 操作失败：变量为空 */
    EOS_ERR_NOT_INITIALIZED,     /**< 操作失败：未初始化 */
    EOS_ERR_ALREADY_INITIALIZED, /**< 操作失败：已初始化 */
    EOS_ERR_BUSY,                /**< 操作失败：正忙         */
    EOS_ERR_FILE_ERROR,          /**< 操作失败：文件相关错误*/
    EOS_ERR_JSON_ERROR,          /**< 操作失败：JSON相关错误*/
    EOS_ERR_VALUE_MISMATCH,      /**< 操作失败：值不匹配*/
    EOS_ERR_TIMEOUT,             /**< 操作失败：超时 */
    EOS_ERR_INVALID_STATE,       /**< 操作识别：状态机无效状态 */
    EOS_ERR_UNKNOWN,
} eos_result_t;

/**
 * @brief 按钮按下状态
 */
typedef enum
{
    EOS_BUTTON_STATE_CLICKED,
    EOS_BUTTON_STATE_PRESSED,
    EOS_BUTTON_STATE_LONG_PRESSED,
    EOS_BUTTON_STATE_RELEASED,
    EOS_BUTTON_STATE_DOUBLE_CLICKED
} eos_button_state_t;

/* Public function prototypes --------------------------------*/

/**
 * @brief ElenaOS 入口函数
 */
void eos_run(void);
/**
 * @brief 启动 ElenaOS Logo 页面
 * @param anim 是否显示 Logo 渐变动画
 * @note 只会被显示一次，在系统正式启动完成前会一直显示。
 */
void eos_logo_play(bool anim);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_CORE_H */
