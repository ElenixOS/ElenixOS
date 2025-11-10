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
    EOS_ERR_UNKNOWN,
} eos_result_t;

/**
 * @brief 侧面按钮按下状态
 */
typedef enum
{
    EOS_SIDE_BTN_CLICKED,
    EOS_SIDE_BTN_PRESSED,
    EOS_SIDE_BTN_LONG_PRESSED,
    EOS_SIDE_BTN_RELEASED,
    EOS_SIDE_BTN_DOUBLE_CLICKED
} eos_side_btn_state_t;

/**
 * @brief 时间结构体定义
 */
typedef struct
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint8_t day_of_week;    /**< 取值范围[0,6] */
} eos_datetime_t;
/* Public function prototypes --------------------------------*/

/**
 * @brief ElenaOS 入口函数
 * @return eos_result_t 返回运行结果
 */
eos_result_t eos_run(void);
/**
 * @brief 设置侧面按钮的状态
 * @param state 状态值
 */
void eos_side_btn_handler(eos_side_btn_state_t state);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_CORE_H */
