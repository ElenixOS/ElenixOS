/**
 * @file script_engine_types.h
 * @brief Script engine type definitions and constants
 */

#ifndef SCRIPT_ENGINE_TYPES_H
#define SCRIPT_ENGINE_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "jerryscript.h"

/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/**
 * @brief Script engine state enumeration
 */
typedef enum {
    SCRIPT_STATE_STOPPED,     /**< Engine is stopped, no script loaded or context saved externally */
    SCRIPT_STATE_RUNNING,     /**< Script is currently executing JavaScript code */
    SCRIPT_STATE_SUSPEND,     /**< Dual state: (A) Script idle with active callbacks, OR (B) Ready for context save. See detailed note above. */
    SCRIPT_STATE_STOPPING,    /**< Script is in the process of stopping (async cleanup pending) */
    SCRIPT_STATE_ERROR,       /**< Script encountered an error during execution */
} script_state_t;

/**
 * @brief Script package type enumeration
 */
typedef enum {
    SCRIPT_TYPE_UNKNOWN = 0,
    SCRIPT_TYPE_APPLICATION = 1,  /**< Application script (calculator, settings, etc.) */
    SCRIPT_TYPE_WATCHFACE = 2,    /**< Watchface script (clock UI) */
} script_pkg_type_t;

/**
 * @brief Script package structure containing metadata and source code
 */
typedef struct {
    const char *id;           /**< Unique identifier (e.g., "cn.sab1e.clock") */
    const char *name;         /**< Human-readable name */
    script_pkg_type_t type;   /**< Package type (application or watchface) */
    const char *version;      /**< Version string */
    const char *author;       /**< Author information */
    const char *description;  /**< Description text */
    const char *script_str;   /**< JavaScript source code string */
    const char *base_path;    /**< Base path for resolving relative paths */
} script_pkg_t;

/**
 * @brief Function entry structure for registering JavaScript functions
 */
typedef struct {
    const char *class_name;              /**< JavaScript class name */
    const char *method_name;             /**< JavaScript method name */
    jerry_external_handler_t handler;    /**< C handler function */
} script_engine_func_entry_t;

/**
 * @brief Error location structure for debugging
 */
typedef struct {
    char source_name[128];  /**< Source file name */
    uint32_t line;          /**< Line number */
    uint32_t column;        /**< Column number */
} script_error_location_t;

/**
 * @brief Result codes for script engine operations
 */
typedef enum {
    SE_OK = 0,                  /**< Operation successful */
    SE_FAILED,                  /**< General failure */
    SE_ERR_NULL_PACKAGE,        /**< NULL package pointer */
    SE_ERR_INVALID_JS,          /**< Invalid JavaScript syntax */
    SE_ERR_JERRY_EXCEPTION,     /**< JerryScript runtime exception */
    SE_ERR_ALREADY_RUNNING,     /**< Script already running */
    SE_ERR_JERRY_INIT_FAIL,     /**< JerryScript initialization failed */
    SE_ERR_NOT_INITIALIZED,     /**< Engine not initialized */
    SE_ERR_SCRIPT_NOT_RUNNING,  /**< No script currently running */
    SE_ERR_BUSY,                /**< Engine busy with another operation */
    SE_ERR_VAR_NULL,            /**< NULL variable encountered */
    SE_ERR_ALREADY_INITIALIZED, /**< Already initialized */
    SE_ERR_STACK_EMPTY,         /**< Operation stack is empty */
    SE_ERR_MALLOC,              /**< Memory allocation failed */
    SE_ERR_INVALID_STATE,       /**< Invalid engine state for operation */
    SE_ERR_TIMEOUT,             /**< Operation timed out */
    SE_ERR_UNKNOWN,             /**< Unknown error */
} script_engine_result_t;

/**
 * @brief Error type classification for error handling
 */
typedef enum {
    EOS_SCRIPT_FAULT_ERROR_UNKNOWN = 0,  /**< Unknown error type */
    EOS_SCRIPT_FAULT_ERROR_EXCEPTION,    /**< JavaScript exception */
    EOS_SCRIPT_FAULT_UNRESPONSIVE,       /**< Script not responding (timeout) */
    EOS_SCRIPT_FAULT_ERROR_PARSE,        /**< Parse error */
    EOS_SCRIPT_FAULT_ERROR_MODULE_LINK,  /**< Module linking error */
} eos_script_error_type_t;

/* Forward declaration */
typedef struct script_engine_saved_context_t script_engine_saved_context_t;

/**
 * @brief Kernel run result structure
 */
typedef struct {
    script_engine_result_t result;              /**< Result code */
    eos_script_error_type_t error_type;         /**< Error type classification */
    char error_info[256];                       /**< Error message string */
    script_error_location_t location;            /**< Primary error location */
    script_error_location_t backtrace[16];      /**< Call stack backtrace */
    uint32_t backtrace_count;                   /**< Number of backtrace entries */
} se_kernel_run_result_t;

/**
 * @brief Kernel call result structure (extends run result)
 */
typedef struct {
    se_kernel_run_result_t base;        /**< Base result fields */
    jerry_value_t return_value;         /**< Return value from function call */
} se_kernel_call_result_t;

#ifdef __cplusplus
}
#endif

#endif /* SCRIPT_ENGINE_TYPES_H */
