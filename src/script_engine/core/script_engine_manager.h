/**
 * @file script_engine_manager.h
 * @brief Script engine manager layer API (high-level policy and control)
 */

#ifndef SCRIPT_ENGINE_MANAGER_H
#define SCRIPT_ENGINE_MANAGER_H

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
 * @brief Run a script package with full lifecycle management
 * @param script_package Script package containing source code and metadata
 * @return Result code indicating success or failure type
 * @note This is the main entry point for running scripts. Handles state transitions,
 *       context switching, and error reporting automatically.
 */
script_engine_result_t script_engine_run(const script_pkg_t *script_package);

/**
 * @brief Stop the currently running script
 * @return Result code indicating success or failure
 * @note For watchfaces, this will save context before stopping if possible.
 *       For applications, this will stop immediately.
 */
script_engine_result_t script_engine_stop(void);

/**
 * @brief Initialize the script engine system
 * @return Result code indicating success or failure
 * @note Must be called before any other script engine functions
 */
script_engine_result_t script_engine_init(void);

/**
 * @brief Clean up and shutdown the script engine system
 * @return Result code indicating success or failure
 * @note Will stop any running script and release all resources
 */
script_engine_result_t script_engine_clean_up(void);

/**
 * @brief Reload the currently running script from disk
 * @return Result code indicating success or failure
 * @note Useful for hot-reloading during development
 */
script_engine_result_t script_engine_reload_current_script(void);

/**
 * @brief Reload the current application script
 * @return Result code indicating success or failure
 * @note Similar to reload_current_script but specifically for applications
 */
script_engine_result_t script_engine_reload_current_app(void);

/**
 * @brief Throw a JavaScript error with message in current context
 * @param message Error message string to throw
 * @return JerryScript error value
 */
jerry_value_t script_engine_throw_error(const char *message);

/**
 * @brief Get last error information string
 * @return Error message string, or NULL if no error
 */
const char *script_engine_get_error_info(void);

/**
 * @brief Get primary error location for debugging
 * @return Pointer to error location structure, or NULL if no error
 */
const script_error_location_t *script_engine_get_error_location(void);

/**
 * @brief Get error backtrace array for debugging
 * @param count Output parameter for number of backtrace entries
 * @return Pointer to backtrace array, or NULL if no backtrace available
 */
const script_error_location_t *script_engine_get_error_backtrace(uint32_t *count);

/**
 * @brief Get backtrace entry count
 * @return Number of backtrace entries available
 */
uint32_t script_engine_get_backtrace_count(void);

/**
 * @brief Get current engine state
 * @return Current script state enumeration value
 */
script_state_t script_engine_get_state(void);

/**
 * @brief Get currently loaded script package type
 * @return Script package type (application or watchface)
 */
script_pkg_type_t script_engine_get_current_script_type(void);

/**
 * @brief Get current script ID
 * @return Newly allocated string with script ID (caller must free), or NULL
 */
char *script_engine_get_current_script_id(void);

/**
 * @brief Get current script name
 * @return Newly allocated string with script name (caller must free), or NULL
 */
char *script_engine_get_current_script_name(void);

/**
 * @brief Parse manifest file and populate script package structure
 * @param manifest_path Path to manifest.json file
 * @param pkg Output package structure to fill
 * @return Result code (SE_OK on success)
 */
script_engine_result_t script_engine_get_manifest(const char *manifest_path, script_pkg_t *pkg);

/**
 * @brief Register C functions as JavaScript methods on an object
 * @param parent Parent object to register functions on
 * @param entries Array of function entries containing class/method names and handlers
 * @param funcs_count Number of entries in array
 */
void script_engine_register_functions(jerry_value_t parent,
                                      const script_engine_func_entry_t *entries,
                                      size_t funcs_count);

/**
 * @brief Set a numeric property on a JavaScript object (inline)
 * @param obj Target object
 * @param prop_name Property name
 * @param value Numeric value to set
 */
extern inline void script_engine_set_prop_number(jerry_value_t obj,
                                                  const char *prop_name,
                                                  double value);

/**
 * @brief Set a boolean property on a JavaScript object (inline)
 * @param obj Target object
 * @param prop_name Property name
 * @param value Boolean value to set
 */
extern inline void script_engine_set_prop_bool(jerry_value_t obj,
                                                const char *prop_name,
                                                bool value);

/**
 * @brief Set a string property on a JavaScript object (inline)
 * @param obj Target object
 * @param prop_name Property name
 * @param value String value to set
 */
extern inline void script_engine_set_prop_string(jerry_value_t obj,
                                                  const char *prop_name,
                                                  const char *value);

/**
 * @brief Set internal script state (for kernel use only)
 * @param state New state to set
 */
void script_engine_set_script_state(script_state_t state);

/**
 * @brief Set script execution timeout in milliseconds
 * @param timeout_ms Timeout value (0 = no timeout)
 */
void script_engine_set_timeout(uint32_t timeout_ms);

/**
 * @brief Get current script execution timeout setting
 * @return Timeout value in milliseconds
 */
uint32_t script_engine_get_timeout(void);

/**
 * @brief Lightweight watchface resume – intended for use by the watchface
 *        activity's on_resume callback. Internally decides whether to:
 *          - Restore a previously saved watchface context (return from app)
 *          - Return immediately (same watchface still running)
 *
 * If the watchface cannot be resumed (e.g. first boot, different watchface
 * selected), the function returns SE_FAILED and the caller should perform
 * a full script_engine_run() with a complete script_pkg_t.
 *
 * @param watchface_id The ID of the watchface to resume (e.g. "cn.sab1e.clock")
 * @return SE_OK if resumed successfully, SE_FAILED if caller needs full run
 */
script_engine_result_t script_engine_watchface_resume(const char *watchface_id);

/**
 * @brief Call a JavaScript function with arguments
 * @param func Function value to call
 * @param this_val This value for the call
 * @param args_p Array of argument values
 * @param args_count Number of arguments
 * @return JerryScript return value from function call
 */
jerry_value_t script_engine_call(jerry_value_t func,
                                  jerry_value_t this_val,
                                  const jerry_value_t args_p[],
                                  const jerry_length_t args_count);

/**
 * @brief Request asynchronous stop of current script
 * @return Result code indicating success or failure
 * @note This queues a stop request that will be processed after current JS execution completes.
 *       Use this instead of script_engine_stop() when calling from within JS execution context.
 */
script_engine_result_t script_engine_request_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* SCRIPT_ENGINE_MANAGER_H */
