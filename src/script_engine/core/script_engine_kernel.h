/**
 * @file script_engine_kernel.h
 * @brief Script engine kernel layer API (low-level JerryScript operations)
 */

#ifndef SCRIPT_ENGINE_KERNEL_H
#define SCRIPT_ENGINE_KERNEL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include "script_engine_types.h"
#include <stddef.h>

/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/* Public function prototypes ---------------------------------*/

/**
 * @brief Run a script package (load and execute JavaScript)
 * @param script_package Script package containing source code and metadata
 * @return Kernel run result with status and error information
 * @note This is a low-level function. Use script_engine_run() from manager layer for high-level control.
 */
se_kernel_run_result_t se_kernel_run(const script_pkg_t *script_package);

/**
 * @brief Stop the currently running script
 * @return Kernel run result with status and error information
 * @note For running scripts, this will asynchronously stop after current execution completes.
 */
se_kernel_run_result_t se_kernel_stop(void);

/**
 * @brief Call a JavaScript function with arguments
 * @param func Function value to call
 * @param this_val This value for the call
 * @param args_p Array of argument values
 * @param args_count Number of arguments
 * @return Kernel call result including return value
 */
se_kernel_call_result_t se_kernel_call(jerry_value_t func,
                                       jerry_value_t this_val,
                                       const jerry_value_t args_p[],
                                       const jerry_length_t args_count);

/**
 * @brief Save current script context (for watchface suspend)
 * @return Saved context pointer, or NULL on failure
 * @note The caller must later call se_kernel_restore_context() or se_kernel_destroy_context()
 */
script_engine_saved_context_t *se_kernel_save_context(void);

/**
 * @brief Restore previously saved script context
 * @param ctx Context to restore (obtained from se_kernel_save_context)
 * @return Kernel run result with status
 */
se_kernel_run_result_t se_kernel_restore_context(script_engine_saved_context_t *ctx);

/**
 * @brief Destroy a saved context and free resources
 * @param ctx Context to destroy
 */
void se_kernel_destroy_context(script_engine_saved_context_t *ctx);

/**
 * @brief Throw a JavaScript error with message
 * @param message Error message string
 * @return JerryScript error value
 */
jerry_value_t se_kernel_throw_error(const char *message);

/**
 * @brief Get last error information string
 * @return Error message string, or NULL if no error
 */
const char *se_kernel_get_error_info(void);

/**
 * @brief Get primary error location
 * @return Pointer to error location structure, or NULL if no error
 */
const script_error_location_t *se_kernel_get_error_location(void);

/**
 * @brief Get error backtrace array
 * @param count Output parameter for number of backtrace entries
 * @return Pointer to backtrace array, or NULL if no backtrace available
 */
const script_error_location_t *se_kernel_get_error_backtrace(uint32_t *count);

/**
 * @brief Get backtrace entry count
 * @return Number of backtrace entries
 */
uint32_t se_kernel_get_backtrace_count(void);

/**
 * @brief Get current engine state
 * @return Current script state enumeration value
 */
script_state_t se_kernel_get_state(void);

/**
 * @brief Get currently loaded script type
 * @return Script package type (application or watchface)
 */
script_pkg_type_t se_kernel_get_current_type(void);

/**
 * @brief Get current script ID
 * @return Newly allocated string with script ID (caller must free), or NULL
 */
char *se_kernel_get_current_id(void);

/**
 * @brief Get current script name
 * @return Newly allocated string with script name (caller must free), or NULL
 */
char *se_kernel_get_current_name(void);

/**
 * @brief Parse manifest file and populate script package structure
 * @param manifest_path Path to manifest.json file
 * @param pkg Output package structure to fill
 * @return Result code (SE_OK on success)
 */
script_engine_result_t se_kernel_get_manifest(const char *manifest_path, script_pkg_t *pkg);

/**
 * @brief Register C functions as JavaScript methods on an object
 * @param parent Parent object to register functions on
 * @param entries Array of function entries
 * @param funcs_count Number of entries in array
 */
void se_kernel_register_functions(jerry_value_t parent,
                                  const script_engine_func_entry_t *entries,
                                  size_t funcs_count);

/**
 * @brief Set a numeric property on a JavaScript object
 * @param obj Target object
 * @param prop_name Property name
 * @param value Numeric value to set
 */
void se_kernel_set_prop_number(jerry_value_t obj, const char *prop_name, double value);

/**
 * @brief Set a boolean property on a JavaScript object
 * @param obj Target object
 * @param prop_name Property name
 * @param value Boolean value to set
 */
void se_kernel_set_prop_bool(jerry_value_t obj, const char *prop_name, bool value);

/**
 * @brief Set a string property on a JavaScript object
 * @param obj Target object
 * @param prop_name Property name
 * @param value String value to set
 */
void se_kernel_set_prop_string(jerry_value_t obj, const char *prop_name, const char *value);

/**
 * @brief Set script execution timeout in milliseconds
 * @param timeout_ms Timeout value (0 = no timeout)
 */
void se_kernel_set_timeout(uint32_t timeout_ms);

/**
 * @brief Get current script execution timeout setting
 * @return Timeout value in milliseconds
 */
uint32_t se_kernel_get_timeout(void);

/**
 * @brief Initialize the script engine kernel
 * @return Kernel run result with status
 */
se_kernel_run_result_t se_kernel_init(void);

/**
 * @brief Clean up and shutdown the script engine kernel
 * @return Kernel run result with status
 */
se_kernel_run_result_t se_kernel_clean_up(void);

#ifdef __cplusplus
}
#endif

#endif /* SCRIPT_ENGINE_KERNEL_H */
