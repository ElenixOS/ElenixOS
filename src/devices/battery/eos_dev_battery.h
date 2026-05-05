/**
 * @file eos_dev_battery.h
 * @brief Battery device abstraction layer
 *
 * This module provides the device layer interface for battery hardware.
 * The battery service (eos_service_battery) depends on this interface
 * to communicate with battery hardware without direct hardware access.
 *
 * @note Application layer should NOT access this module directly.
 *       Use eos_service_battery API instead.
 */

#ifndef EOS_DEV_BATTERY_H
#define EOS_DEV_BATTERY_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "eos_error.h"

/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/**
 * @brief Battery device operation callbacks
 *
 * This structure defines the interface that battery hardware
 * implementation must provide. All pointers are optional except
 * request_update.
 */
typedef struct {
    void (*request_update)(void);     /**< Request battery data update (mandatory) */
    int (*get_voltage_mv)(void);      /**< Get battery voltage in millivolts (optional) */
    int (*get_current_ma)(void);      /**< Get battery current in milliamps (optional) */
    int (*get_percent)(void);         /**< Get battery percentage (optional) */
    bool (*is_charging)(void);        /**< Check if battery is charging (optional) */
} eos_battery_dev_ops_t;

typedef struct {
    eos_battery_dev_ops_t ops;          /**< Battery device operation callbacks */
    uint16_t design_capacity_mah;      /**< Battery design capacity in mAh */
} eos_dev_battery_t;

/* Public function prototypes --------------------------------*/

/**
 * @brief Register battery device operations
 *
 * Registers the battery hardware implementation with the device layer.
 * This function should be called during system initialization before
 * the battery service is initialized.
 *
 * @param ops Pointer to battery device operations structure
 * @param design_capacity_mah Battery design capacity in mAh
 * @return None
 *
 * @note If get_percent is not provided, voltage-based fallback
 *       calculation will be used in battery service
 */
void eos_dev_battery_register(const eos_battery_dev_ops_t *ops, uint16_t design_capacity_mah);

/**
 * @brief Get battery device operations
 *
 * Retrieves the currently registered battery device operations.
 *
 * @return Pointer to battery device operations, or NULL if not registered
 */
eos_dev_battery_t *eos_dev_battery_get_instance(void);

#ifdef __cplusplus
}
#endif

#endif /* EOS_DEV_BATTERY_H */
