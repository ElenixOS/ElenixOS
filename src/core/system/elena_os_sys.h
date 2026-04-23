/**
 * @file elena_os_sys.h
 * @brief System configuration
 * @author Sab1e
 * @date 2025-08-21
 */

#ifndef ELENA_OS_SYS_H
#define ELENA_OS_SYS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "elena_os_core.h"
#include "elena_os_config.h"
/* Public macros ----------------------------------------------*/

/************************** System directory configuration **************************/
#define EOS_SYS_DIR EOS_SYS_ROOT_DIR ".sys/"
#define EOS_SYS_CONFIG_DIR EOS_SYS_DIR "config/"
#define EOS_SYS_CONFIG_FILE_PATH EOS_SYS_CONFIG_DIR "cfg.json"

#define EOS_SYS_RES_DIR EOS_SYS_DIR "res/"
#define EOS_SYS_RES_IMG_DIR EOS_SYS_RES_DIR "img/"

#define EOS_SYS_RES_FONT_DIR EOS_SYS_RES_DIR "font/"
/************************** Keys for system configuration information **************************/
#define EOS_SYS_CFG_KEY_DEVICE_NAME_STR "device_name"
#define EOS_SYS_CFG_KEY_LANGUAGE_STR "language"
#define EOS_SYS_CFG_KEY_WATCHFACE_ID_STR "wf_id"
#define EOS_SYS_CFG_KEY_BLUETOOTH_BOOL "bluetooth"
#define EOS_SYS_CFG_KEY_DISPLAY_BRIGHTNESS_NUMBER "display_brightness"
#define EOS_SYS_CFG_KEY_SPEAKER_VOLUME_NUMBER "speaker_volume"
#define EOS_SYS_CFG_KEY_MUTE_BOOL "mute"
#define EOS_SYS_CFG_KEY_SLEEP_TIMEOUT_SEC_NUMBER "sleep_timeout_sec"
#define EOS_SYS_CFG_KEY_AOD_MODE_BOOL "aod_mode"
#define EOS_SYS_CFG_KEY_WAKE_ON_RAISE_BOOL "wake_on_raise"
#define EOS_SYS_CFG_KEY_VIBRATOR_STRENGTH_NUMBER "vibrator_strength"
/************************** Default values **************************/
#define EOS_SYS_DEFAULT_DEVICE_NAME "Elena Watch"
#define EOS_SYS_DEFAULT_LANG_STR "English"
#define EOS_SYS_DEFAULT_WATCHFACE_ID_STR "cn.sab1e.clock"
/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/

/**
 * @brief Initialize on first system run
 */
void eos_sys_init(void);
/**
 * @brief Set boolean type configuration item
 * @param key Configuration item key
 * @param value Boolean value
 * @return Operation result
 */
eos_result_t eos_sys_cfg_set_bool(const char *key, bool value);
/**
 * @brief Set string type configuration item
 * @param key Configuration item key
 * @param value String value
 * @return Operation result
 */
eos_result_t eos_sys_cfg_set_string(const char *key, const char *value);
/**
 * @brief Set number type configuration item
 * @param key Configuration item key
 * @param value Number value
 * @return Operation result
 */
eos_result_t eos_sys_cfg_set_number(const char *key, double value);
/**
 * @brief Get boolean type configuration item
 * @param key Configuration item key
 * @param default_value Default value (returned when configuration item does not exist or type mismatch)
 * @return Retrieved boolean value or default value
 */
bool eos_sys_cfg_get_bool(const char *key, bool default_value);
/**
 * @brief Get string type configuration item
 * @param key Configuration item key
 * @param default_value Default value (returned when configuration item does not exist or type mismatch)
 * @return Retrieved string value or default value
 * @warning The returned string needs to be freed using `eos_malloc(str)` when no longer needed
 */
char *eos_sys_cfg_get_string(const char *key, const char *default_value);
/**
 * @brief Get number type configuration item
 * @param key Configuration item key
 * @param default_value Default value (returned when configuration item does not exist or type mismatch)
 * @return Retrieved number value or default value
 */
double eos_sys_cfg_get_number(const char *key, double default_value);
/**
 * @brief Add new configuration item to system configuration file
 * @param key Configuration item key to add (string)
 * @param value Configuration item value to add (string)
 * @return Returns result
 */
eos_result_t eos_sys_add_config_item(const char *key, const char *value);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_SYS_H */