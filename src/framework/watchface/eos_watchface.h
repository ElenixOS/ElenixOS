/**
 * @file eos_watchface.h
 * @brief Watchface
 */

#ifndef ELENIX_OS_WATCHFACE_H
#define ELENIX_OS_WATCHFACE_H

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
#include "eos_activity.h"
/* Public macros -----DATA_DIR EOS_WATCHFACE_DIR "wf_data/"
#define EOS_WATCHFACE_BUILTIN_FALLBACK_ID "builtin.fallback"
#define EOS_WATCHFACE_MANIFEST_FILE_NAME "manifest.json"
#define EOS_WATCHFACE_SNAPSHOT_FILE_NAME "snapshot.bin"
#define EOS_WATCHFACE_SCRIPT_ENTRY_FILE_NAME "main.js"
/* Public typedefs --------------------------------------------*/
/* Public function prototypes --------------------------------*/

/**
 * @brief Get the number of currently installed watchfaces, i.e., list size
 * @return size_t List size
 */
size_t eos_watchface_list_size(void);
/**
 * @brief Get watchface id by index
 * @param index Index value (0-based)
 * @return const char* id string
 */
const char* eos_watchface_list_get_id(size_t index);
/**
 * @brief Check if watchface with specified id exists in the list
 * @param watchface_id id string
 * @return true
 * @return false
 */
bool eos_watchface_list_contains(const char* watchface_id);
/**
 * @brief Install watchface
 * @param eapk_path eapk package path
 * @return eos_result_t Installation result
 */
eos_result_t eos_watchface_install(const char *eapk_path);
/**
 * @brief Uninstall watchface
 * @param watchface_id Watchface id
 * @return eos_result_t Uninstallation result
 */
eos_result_t eos_watchface_uninstall(const char *watchface_id);
/**
 * @brief Initialize watchface system
 * @return eos_result_t Initialization result
 */
eos_result_t eos_watchface_init(void);
/**
 * @brief Get watchface Activity created during initialization
 * @return eos_activity_t* Watchface Activity pointer, returns NULL on failure
 */
eos_activity_t *eos_watchface_get_activity(void);
#ifdef __cplusplus
}
#endif

#endif /* ELENIX_OS_WATCHFACE_H */
