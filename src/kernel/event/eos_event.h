/**
 * @file eos_event.h
 * @brief Event system
 */

#ifndef ELENIX_OS_EVENT_H
#define ELENIX_OS_EVENT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"
#include "eos_event_def.h"
/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/**
 * @brief Event type definitions
 * @note New events can be added here
 */
enum
{
    EOS_EVENT_BASE = LV_EVENT_LAST,
    EOS_EVENT_UNKNOWN = EOS_EVENT_BASE,
    EOS_EVENT_SLIDE_WIDGET_REACHED_THRESHOLD,   /**< Slide exceeded threshold */
    EOS_EVENT_SLIDE_WIDGET_REVERTED,            /**< Slide auto-reverted without exceeding threshold */
    EOS_EVENT_SLIDE_WIDGET_MOVING,              /**< Triggered during sliding (including manual sliding and animation) */
    EOS_EVENT_SLIDE_WIDGET_DONE,                /**< Slide completed */
    EOS_EVENT_SLIDE_WIDGET_OPENED,              /**< Slide widget has been pulled out (global broadcast) */
    EOS_EVENT_SLIDE_WIDGET_CLOSED,              /**< Slide widget has been pulled back (global broadcast) */
    EOS_EVENT_APP_UNINSTALLED,                  /**< Application has been uninstalled */
    EOS_EVENT_APP_INSTALLED,                    /**< Application has been installed */
    EOS_EVENT_SYSTEM_SLEEP,                     /**< System entered sleep mode */
    EOS_EVENT_SYSTEM_DISPLAY_ON,                /**< System has been awakened */
    EOS_EVENT_SYSTEM_DISPLAY_AOD,               /**< Screen always-on mode has been activated */
    EOS_EVENT_SYSTEM_CONFIG_UPDATE,             /**< Configuration file has been updated */
    EOS_EVENT_ROUNDED_CORNER_DELETE,            /**< Remove rounded corners, see `eos_obj_set_corner_radius_bg()` */
    EOS_EVENT_SCRIPT_STARTED,                   /**< Script has started */
    EOS_EVENT_SCRIPT_EXITED,                    /**< Script has exited */
    EOS_EVENT_ACTIVITY_SCREEN_SWITCHED,         /**< Activity page transition completed, param is current activity view */
    /* Add new events here */
    EOS_EVENT_SENSOR_REPORT_START,     /**< Used for sensor event sequence alignment   */
    EOS_EVENT_SENSOR_REPORT_ACCE,      /**< Accelerometer sensor           */
    EOS_EVENT_SENSOR_REPORT_GYRO,      /**< Gyroscope sensor             */
    EOS_EVENT_SENSOR_REPORT_MAG,       /**< Magnetometer sensor              */
    EOS_EVENT_SENSOR_REPORT_TEMP,      /**< Temperature sensor             */
    EOS_EVENT_SENSOR_REPORT_HUMI,      /**< Relative humidity sensor          */
    EOS_EVENT_SENSOR_REPORT_BARO,      /**< Barometric pressure sensor              */
    EOS_EVENT_SENSOR_REPORT_LIGHT,     /**< Ambient light sensor            */
    EOS_EVENT_SENSOR_REPORT_PROXIMITY, /**< Proximity sensor              */
    EOS_EVENT_SENSOR_REPORT_HR,        /**< Heart rate sensor              */
    EOS_EVENT_SENSOR_REPORT_TVOC,      /**< TOVC sensor             */
    EOS_EVENT_SENSOR_REPORT_NOISE,     /**< Noise sensor             */
    EOS_EVENT_SENSOR_REPORT_STEP,      /**< Step counter sensor             */
    EOS_EVENT_SENSOR_REPORT_FORCE,     /**< Force sensor               */
    EOS_EVENT_SENSOR_REPORT_BAT,       /**< Battery level sensor          */
    EOS_EVENT_SENSOR_REPORT_END,       /**< Used for sensor event sequence alignment   */
    EOS_EVENT_LAST
};

#if EOS_EVENT_LAST >= EOS_EVENT_USER_BASE
#error "EOS_EVENT_LAST exceeds EOS_EVENT_USER_BASE, please adjust the event code definitions!"
#endif

/* Public function prototypes --------------------------------*/

/**
 * @brief Add event callback
 * @param obj Object pointer (callback will be automatically removed when object is deleted)
 * @param event Event type
 * @param cb Callback function
 * @param user_data User data
 * @note lv_event_code_t is compatible with lv_event_code_t, so event code can be passed directly.
 * Example:
 *
 * `eos_event_add_cb(obj,cb,LV_EVENT_ALL,NULL);`
 *
 * `eos_event_add_cb(obj,cb,EOS_EVENT_APP_INSTALLED,NULL);`
 *
 */
void eos_event_add_cb(lv_obj_t *obj, lv_event_cb_t cb, lv_event_code_t event, void *user_data);

/**
 * @brief Remove event callback
 * - If not broadcasting: immediately remove from linked list and release, also call lv_obj_remove_event_cb
 * - If currently broadcasting: mark for deletion (deferred cleanup)
 * @param obj Object pointer
 * @param event Event type
 * @param cb Callback function
 */
void eos_event_remove_cb(lv_obj_t *obj, lv_event_code_t event, lv_event_cb_t cb);

/**
 * @brief Broadcast event
 * - Supports nested broadcasting
 * - During broadcast, when object or node is deleted, only mark it, and clean up uniformly after broadcast ends
 * @param event Event type to broadcast
 * @param param Event parameter
 */
void eos_event_broadcast(lv_event_code_t event, void *param);

/**
 * @brief Allow active cleanup to be triggered externally (e.g., called during system idle)
 */
void eos_event_cleanup_now(void);

/**
 * @brief Add global callback
 * @param cb Callback function
 * @param event Event type
 * @param user_data User data
 */
void eos_event_add_global_cb(lv_event_cb_t cb, lv_event_code_t event, void *user_data);
/**
 * @brief Remove specified callback function under specified event
 * @param event Event type
 * @param cb Callback function
 */
void eos_event_remove_global_cb(lv_event_code_t event, lv_event_cb_t cb);
/**
 * @brief Remove specified callback function under specified event (distinguished by user_data)
 * @param event Event type
 * @param cb Callback function
 * @param user_data User data, used to distinguish different registrations of the same callback
 */
void eos_event_remove_global_cb_with_user_data(lv_event_code_t event, lv_event_cb_t cb, void *user_data);
/**
 * @brief Remove all event registrations for specified global callback function
 * @param cb Callback function
 */
void eos_event_remove_all_global_cbs(lv_event_cb_t cb);
/**
 * @brief
 * @return lv_event_code_t
 */
lv_event_code_t eos_event_register_id(void);
#ifdef __cplusplus
}
#endif

#endif /* ELENIX_OS_EVENT_H */
