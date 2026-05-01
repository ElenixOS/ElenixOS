/**
 * @file eos_port_sensor.h
 * @brief Sensor porting interface
 */

#ifndef EOS_PORT_SENSOR_H
#define EOS_PORT_SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "eos_sensor.h"
/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/

/**
 * @brief Asynchronously read sensor value once
 *
 * Example usage:
 * Create a reading thread in advance, start the thread in this function, start reading the sensor, and report the data after reading is completed.
 * @param type Sensor type
 * @note After reading is completed, you must use `eos_sensor_report()` to report the reading result, otherwise the upper layer cannot get sensor data
 * @warning Do not perform time-consuming tasks in the function
 */
void eos_sensor_read_async(eos_sensor_type_t type);
/**
 * @brief Synchronously read sensor value once
 * @param type Sensor type
 * @param out Sensor reading result
 */
void eos_sensor_read_sync(eos_sensor_type_t type, eos_sensor_t *out);
#ifdef __cplusplus
}
#endif

#endif /* EOS_PORT_SENSOR_H */
