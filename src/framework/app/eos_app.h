/**
 * @file eos_app.h
 * @brief Application system
 */

#ifndef EOS_APP_H
#define EOS_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "eos_core.h"
#include "eos_service_config.h"
#include "eos_storage_paths.h"
#include "lvgl.h"
#include "script_engine_core.h"

/* Public macros ----------------------------------------------*/
#define EOS_APP_ICON_FILE_NAME "icon.bin"
#define EOS_APP_MANIFEST_FILE_NAME "manifest.json"
#define EOS_APP_SCRIPT_ENTRY_FILE_NAME "main.js"
/* Public typedefs --------------------------------------------*/

struct eos_pkg;

/**
 * @brief Script error handler configuration
 */
typedef struct
{
    const char *title_text; /**< Error title text (NULL for default) */
    lang_string_id_t title_id; /**< Error title string ID (0 for default) */
    /* Confirm button (e.g., "Restart App", "Restart") */
    const char *confirm_btn_text; /**< Confirm button text (NULL for default) */
    lang_string_id_t confirm_btn_id; /**< Confirm button string ID (0 for hidden) */
    lv_event_cb_t confirm_cb; /**< Confirm button click callback */
    /* Cancel button (e.g., "Exit App", "Switch Watch Face") */
    const char *cancel_btn_text; /**< Cancel button text (NULL for default) */
    lang_string_id_t cancel_btn_id; /**< Cancel button string ID (0 for default) */
    lv_event_cb_t cancel_cb; /**< Cancel button click callback (NULL for default back) */
} eos_script_error_handler_cfg_t;

/* Public function prototypes ---------------------------------*/

/**
 * @brief Handle script execution error
 * @param error_type Type of script error
 * @param error_code Error code from script engine
 * @param app_id Application ID that caused the error
 * @param cfg Optional configuration for customizing error page
 */
void eos_app_handle_script_error(eos_script_error_type_t error_type,
                                 eos_result_t error_code,
                                 const char *app_id,
                                 const eos_script_error_handler_cfg_t *cfg);

/**
 * @brief Move app with target ID to specified position for app_list sorting
 * @param app_id Target ID
 * @param new_index New index value
 * @return eos_result_t
 */
eos_result_t eos_app_order_move(const char *app_id, size_t new_index);
/**
 * @brief Get the number of currently installed apps
 */
uint32_t eos_app_get_installed(void);
/**
 * @brief Get app id by index
 * @param index Index value (0-based)
 * @return const char* id string
 */
const char *eos_app_list_get_id(size_t index);
/**
 * @brief Check if app with specified id exists in the list
 * @param app_id id string
 * @return true
 * @return false
 */
bool eos_app_list_contains(const char *app_id);
/**
 * @brief Get existing ID from app list that matches input string (avoid duplicate memory allocation)
 * @param id Original ID to find (the ID declared by manifest.json)
 * @return Existing string pointer in the list (lifecycle managed by the list), returns NULL if not found
 */
const char *eos_app_list_get_existing_id(const char *id);
/**
 * @brief Install app
 * @param pkg_path EPK package path
 * @return eos_result_t Installation result
 */
eos_result_t eos_app_install(const char *pkg_path);

/**
 * @brief Install an already opened Application package
 * @param package Open EPK package context
 * @return eos_result_t Installation result
 */
eos_result_t eos_app_install_package(struct eos_pkg *package);
/**
 * @brief Uninstall app
 * @param app_id App id
 * @return eos_result_t Uninstallation result
 */
eos_result_t eos_app_uninstall(const char *app_id);
/**
 * @brief Automatically delete specified object when app is deleted
 * @param obj Target object
 * @param app_id Target app ID
 */
void eos_app_obj_auto_delete(lv_obj_t *obj, const char *app_id);
/**
 * @brief Initialize app system
 * @return eos_result_t Initialization result
 */
eos_result_t eos_app_init(void);

/**
 * @brief Initialize crash recovery event handler
 * Must be called during system init to subscribe to EOS_EVENT_SCRIPT_FATAL
 */
void eos_app_crash_handler_init(void);
#ifdef __cplusplus
}
#endif

#endif /* EOS_APP_H */
