
/**
 * @file script_engine_core.h
 * @brief Application system external interface definition
 */
#ifndef SCRIPT_ENGINE_CORE_H
#define SCRIPT_ENGINE_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"
#include "jerryscript.h"

/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/
/**
 * @brief Script running state
 */
typedef enum {
    SCRIPT_STATE_STOPPED,      /**< Stopped: script has stopped and released resources */
    SCRIPT_STATE_RUNNING,      /**< Running: script is running */
    SCRIPT_STATE_SUSPEND,      /**< Suspended: script has finished running, waiting for callback */
    SCRIPT_STATE_STOPPING,     /**< Stopping: script is being stopped */
    SCRIPT_STATE_ERROR,        /**< Error: script execution error */
} script_state_t;
/**
 * @brief Script package type
 */
typedef enum{
    SCRIPT_TYPE_UNKNOWN=0,
    SCRIPT_TYPE_APPLICATION=1,
    SCRIPT_TYPE_WATCHFACE=2
}script_pkg_type_t;
/**
 * @brief Function entry link structure
 */
typedef struct {
    const char* class_name;
    const char* method_name;
    jerry_external_handler_t handler;
} script_engine_func_entry_t;
/**
 * @brief Script package description structure
 */
typedef struct {
    const char* id;               /**< Application unique ID, e.g., "com.mydev.clock" */
    const char* name;             /**< Application display name, e.g., "Clock" */
    script_pkg_type_t type;       /**< Script type, e.g., SCRIPT_TYPE_APPLICATION */
    const char* version;          /**< Application version, e.g., "1.0.2" */
    const char* author;           /**< Developer name */
    const char* description;      /**< Brief description */
    const char* script_str;       /**< Main JS script string (UTF-8) */
    const char* base_path;       /**< Script base path, used to resolve relative path module imports */
} script_pkg_t;

/**
 * @brief Script engine running result
 */
typedef enum {
    SE_OK = 0,                   /**< Startup successful */
    SE_FAILED,
    SE_ERR_NULL_PACKAGE,         /**< Incoming package pointer is null */
    SE_ERR_INVALID_JS,           /**< JS script is invalid (syntax error, empty string, etc.) */
    SE_ERR_JERRY_EXCEPTION,      /**< JS exception thrown during operation */
    SE_ERR_ALREADY_RUNNING,      /**< There is already an APP running */
    SE_ERR_JERRY_INIT_FAIL,      /**< JerryScript initialization failed */
    SE_ERR_NOT_INITIALIZED,      /**< Not initialized */
    SE_ERR_SCRIPT_NOT_RUNNING,   /**< No application running */
    SE_ERR_BUSY,                 /**< Busy */
    SE_ERR_VAR_NULL,             /**< Value is null */
    SE_ERR_ALREADY_INITIALIZED,  /**< Already initialized */
    SE_ERR_STACK_EMPTY,          /**< Stack empty */
    SE_ERR_MALLOC,               /**< Memory allocation error */
    SE_ERR_INVALID_STATE,        /**< Invalid state */
    SE_ERR_UNKNOWN               /**< Unknown error */
} script_engine_result_t;

/* Public function prototypes --------------------------------*/

/**
 * @brief Throw error
 * @param message Error content
 * @return jerry_value_t Error object
 */
jerry_value_t script_engine_throw_error(const char *message);
/**
 * @brief Get last run error information
 * @return const char* Error message string
 */
const char *script_engine_get_error_info(void);
/**
 * @brief Add parameter to specified JerryScript object (numeric type)
 * @param obj Target object
 * @param prop_name Parameter name (key)
 * @param value Parameter: numeric value
 */
extern inline void script_engine_set_prop_number(jerry_value_t obj,
                                    const char* prop_name,
                                    double value);
/**
 * @brief Add parameter to specified JerryScript object (boolean type)
 * @param obj Target object
 * @param prop_name Parameter name (key)
 * @param value Parameter: boolean value
 */
extern inline void script_engine_set_prop_bool(jerry_value_t obj,
                                    const char* prop_name,
                                    bool value);
/**
 * @brief Add parameter to specified JerryScript object (string type)
 * @param obj Target object
 * @param prop_name Parameter name (key)
 * @param value Parameter: string
 */
extern inline void script_engine_set_prop_string(jerry_value_t obj,
                                    const char* prop_name,
                                    const char* value);
/**
 * @brief Close currently running JS application
 * @return script_engine_result_t Return operation result
 */
script_engine_result_t script_engine_request_stop(void);

/**
 * @brief Get manifest.json and fill script_pkg_t structure
 * @param manifest_path manifest.json file path
 * @param pkg Target structure pointer
 * @return script_engine_result_t
 */
script_engine_result_t script_engine_get_manifest(const char *manifest_path, script_pkg_t *pkg);

/**
 * @brief Initialize script engine
 * @return script_engine_result_t
 */
script_engine_result_t script_engine_init(void);
/**
 * @brief Run specified application, automatically clear if there is already an application running
 * @param script_package Script package (read-only borrow, function internally deep copies and manages its lifecycle)
 * @return script_engine_result_t Return operation result
 */
script_engine_result_t script_engine_run(const script_pkg_t* script_package);

/**
 * @brief Get script engine current state
 * @return script_state_t State
 */
script_state_t script_engine_get_state(void);
/**
 * @brief Register C functions to JS
 * @param parent Parent object
 * @param entry Function entry array; if class_name == NULL, then directly register handler to parent
 * @param funcs_count Array length
 */
void script_engine_register_functions(jerry_value_t parent, const script_engine_func_entry_t *entry, const size_t funcs_count);
/**
 * @brief Set script running state
 * @param state script_state_t
 */
void script_engine_set_script_state(script_state_t state);
/**
 * @brief Get current running script ID
 * @return char* ID string
 */
char *script_engine_get_current_script_id(void);
/**
 * @brief Get current running script name
 * @return char* Name string
 */
char *script_engine_get_current_script_name(void);
/**
 * @brief Get current running script type
 * @return script_pkg_type_t
 */
script_pkg_type_t script_engine_get_current_script_type(void);
#ifdef __cplusplus
}
#endif

#endif /* SCRIPT_ENGINE_CORE_H */
