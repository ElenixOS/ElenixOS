/**
 * @file elena_os_core.h
 * @brief Elena OS core header file
 * @author Sab1e
 * @date 2025-08-10
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
 * @brief ElenaOS return value status
 */
typedef enum
{
    EOS_OK = 0,                  /**< Operation successful */
    EOS_FAILED,                  /**< Operation failed */
    EOS_ERR_MEM,                 /**< Operation failed: memory related error */
    EOS_ERR_STACK_EMPTY,         /**< Operation failed: stack empty */
    EOS_ERR_STACK_FULL,          /**< Operation failed: stack full */
    EOS_ERR_VAR_NOT_NULL,        /**< Operation failed: variable not null */
    EOS_ERR_VAR_NULL,            /**< Operation failed: variable null */
    EOS_ERR_NOT_INITIALIZED,     /**< Operation failed: not initialized */
    EOS_ERR_ALREADY_INITIALIZED, /**< Operation failed: already initialized */
    EOS_ERR_BUSY,                /**< Operation failed: busy         */
    EOS_ERR_FILE_ERROR,          /**< Operation failed: file related error*/
    EOS_ERR_JSON_ERROR,          /**< Operation failed: JSON related error*/
    EOS_ERR_VALUE_MISMATCH,      /**< Operation failed: value mismatch*/
    EOS_ERR_TIMEOUT,             /**< Operation failed: timeout */
    EOS_ERR_INVALID_STATE,       /**< Operation failed: state machine invalid state */
    EOS_ERR_UNKNOWN,
} eos_result_t;

/**
 * @brief Button press state
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
 * @brief ElenaOS entry function
 */
void eos_run(void);
/**
 * @brief Start ElenaOS Logo page
 * @param anim Whether to display Logo fade animation
 * @note Will only be displayed once and will remain until the system is fully started.
 */
void eos_logo_play(bool anim);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_CORE_H */