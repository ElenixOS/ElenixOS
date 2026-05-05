/**
 * @file eos_service_sensor.h
 * @brief Sensor service header file
 */

#ifndef EOS_SERVICE_SENSOR_H
#define EOS_SERVICE_SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "eos_error.h"
#include "eos_dev_sensor.h"
#include "eos_fifo.h"
#include "lvgl.h"

/* Public macros ----------------------------------------------*/

#define EOS_SENSOR_FIFO_CAPACITY    64

/* Public typedefs --------------------------------------------*/

/**
 * @brief Sensor data callback function
 */
typedef lv_event_cb_t eos_sensor_data_cb_t;

/**
 * @brief Sensor service instance structure
 */
typedef struct {
    eos_sensor_type_t type;
    eos_dev_sensor_t *device;
    eos_fifo_t *fifo;
    eos_sensor_raw_data_t latest_data;
    uint8_t subscriber_count;
    uint32_t sample_period_ms;
    uint32_t last_sample_time;
    bool is_active;
} eos_sensor_service_instance_t;

/* Public function prototypes --------------------------------*/

/**
 * @brief Initialize sensor service
 * @return eos_result_t Operation result
 */
eos_result_t eos_service_sensor_init(void);

/**
 * @brief Read sensor data from FIFO (Pull mode)
 * @param type Sensor type
 * @param data Pointer to store sensor data
 * @return eos_result_t Operation result
 */
eos_result_t eos_sensor_read(eos_sensor_type_t type, eos_sensor_raw_data_t *data);

/**
 * @brief Read latest sensor data
 * @param type Sensor type
 * @param data Pointer to store sensor data
 * @return eos_result_t Operation result
 */
eos_result_t eos_sensor_read_latest(eos_sensor_type_t type, eos_sensor_raw_data_t *data);

/**
 * @brief Subscribe to sensor data (Push mode)
 * @param type Sensor type
 * @param cb Callback function
 * @param user_data User data passed to callback
 * @param min_interval_ms Minimum callback interval in milliseconds
 * @return eos_result_t Operation result
 */
eos_result_t eos_sensor_subscribe(eos_sensor_type_t type, eos_sensor_data_cb_t cb, void *user_data, uint32_t min_interval_ms);

/**
 * @brief Unsubscribe from sensor data
 * @param type Sensor type
 * @param cb Callback function
 * @param user_data User data
 * @return eos_result_t Operation result
 */
eos_result_t eos_sensor_unsubscribe(eos_sensor_type_t type, eos_sensor_data_cb_t cb, void *user_data);

/**
 * @brief Set sensor sample period
 * @param type Sensor type
 * @param period_ms Sample period in milliseconds
 * @return eos_result_t Operation result
 */
eos_result_t eos_sensor_set_sample_period(eos_sensor_type_t type, uint32_t period_ms);

/**
 * @brief Get sensor sample period
 * @param type Sensor type
 * @return uint32_t Sample period in milliseconds
 */
uint32_t eos_sensor_get_sample_period(eos_sensor_type_t type);

/**
 * @brief Notify sensor data (called by driver)
 * @param type Sensor type
 * @param data Sensor data
 * @param timestamp Data timestamp
 */
void eos_sensor_notify(eos_sensor_type_t type, const eos_sensor_data_t *data, uint32_t timestamp);

#ifdef __cplusplus
}
#endif

#endif /* EOS_SERVICE_SENSOR_H */
