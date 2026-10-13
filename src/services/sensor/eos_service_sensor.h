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
#include "eos_event.h"
#include "eos_sensor_heart_rate.h"
#include "eos_sensor_compass.h"

/* Public macros ----------------------------------------------*/

#define EOS_SENSOR_FIFO_CAPACITY 64

#define EOS_SENSOR_DEMAND_INVALID (0U)

/* Public typedefs --------------------------------------------*/

/**
 * @brief Identifier for one Sensor Service sampling demand
 */
typedef uint32_t eos_sensor_demand_id_t;

/**
 * @brief Sensor data callback function
 * @param type      Sensor type that produced the data
 * @param data      Sensor data with timestamp (caller-owned; copy if needed)
 * @param user_data User data registered at subscribe time
 * @note  This replaces the previous eos_event_cb_t alias. The callback
 *        now receives typed sensor data directly instead of an opaque
 *        eos_event_t pointer requiring eos_event_get_param().
 */
typedef void (*eos_sensor_data_cb_t)(eos_sensor_type_t type, const eos_sensor_raw_data_t *data, void *user_data);

/**
 * @brief Sensor operating mode — determines sampling policy
 */
typedef enum
{
    EOS_SENSOR_MODE_NORMAL = 0, /**< Normal operation (display on, user active) */
    EOS_SENSOR_MODE_LOW_POWER, /**< Low-power (AOD, battery save) */
    EOS_SENSOR_MODE_ACTIVE, /**< Active (workout / high-motion) */
    EOS_SENSOR_MODE_SLEEP, /**< Sleep (display off, no user interaction) */
    EOS_SENSOR_MODE_COUNT,
} eos_sensor_mode_t;

/**
 * @brief Per-mode sensor sampling policy
 */
typedef struct
{
    uint32_t min_interval_ms; /**< Floor for any subscriber request in this mode */
    bool allow_enable; /**< Whether sensors may be powered on in this mode */
} eos_sensor_policy_t;

/**
 * @brief Subscriber node — links callback, user_data, and interval.
 *        Single source of truth for sensor data push subscribers.
 *        Used for direct callback invocation in eos_sensor_notify().
 */
typedef struct _sensor_subscriber_t
{
    eos_sensor_data_cb_t cb;
    void *user_data;
    uint32_t min_interval_ms;
    eos_sensor_demand_id_t demand_id;
    struct _sensor_subscriber_t *next;
    bool marked_for_delete; /**< Deferred deletion flag for safe broadcast */
} sensor_subscriber_t;

/**
 * @brief Sensor service instance structure
 */
typedef struct
{
    eos_sensor_type_t type;
    eos_dev_sensor_t *device;
    eos_fifo_t *fifo;
    eos_sensor_raw_data_t latest_data;
    uint16_t subscriber_count;
    uint32_t sample_period_ms;
    uint32_t last_sample_time;
    bool is_active;
    bool is_enabled; /**< Whether the hardware device is currently enabled */
    bool has_sample; /**< Whether latest_data contains a real sample */
    sensor_subscriber_t *subscribers; /**< Linked list of active subscribers */
} eos_sensor_service_instance_t;

/* Public function prototypes ---------------------------------*/

/**
 * @brief Initialize sensor service
 * @return eos_result_t Operation result
 */
eos_result_t eos_service_sensor_init(void);

/**
 * @brief Read one queued standardized sample from the FIFO (pull mode)
 * @param type Sensor type; the matching union member in @p data is valid
 * @param data Output sample using the canonical units in eos_dev_sensor.h
 * @return EOS_OK when a sample was dequeued; EOS_ERR_NOT_FOUND when the FIFO
 *         is empty; validation/device errors use the existing eos_result_t
 *         values
 */
eos_result_t eos_sensor_read(eos_sensor_type_t type, eos_sensor_raw_data_t *data);

/**
 * @brief Read the most recently published standardized sample
 * @param type Sensor type; the matching union member in @p data is valid
 * @param data Output sample using the canonical units in eos_dev_sensor.h
 * @return EOS_OK when a real sample exists; EOS_ERR_NOT_FOUND before the
 *         first sample; an all-zero published sample is still EOS_OK
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
eos_result_t eos_sensor_subscribe(eos_sensor_type_t type,
                                  eos_sensor_data_cb_t cb,
                                  void *user_data,
                                  uint32_t min_interval_ms);

/**
 * @brief Unsubscribe from sensor data
 * @param type Sensor type
 * @param cb Callback function
 * @param user_data User data
 * @return eos_result_t Operation result
 */
eos_result_t eos_sensor_unsubscribe(eos_sensor_type_t type, eos_sensor_data_cb_t cb, void *user_data);

/**
 * @brief Request a sensor output period
 * @param type Sensor type
 * @param period_ms Requested output period in milliseconds; zero disables the
 *        service-owned sampling request.  The value is not a promise of an
 *        exact hardware cadence and does not change the sample's units.
 * @return eos_result_t Operation result
 */
eos_result_t eos_sensor_set_sample_period(eos_sensor_type_t type, uint32_t period_ms);

/**
 * @brief Acquire an independent sampling demand
 * @param type Sensor type
 * @param period_ms Requested minimum period in milliseconds
 * @param demand_id Output demand identifier
 * @return eos_result_t Operation result
 * @note The effective hardware period is arbitrated with all other active
 *       demands and the current power policy.
 */
eos_result_t eos_sensor_demand_acquire(eos_sensor_type_t type, uint32_t period_ms, eos_sensor_demand_id_t *demand_id);

/**
 * @brief Release one sampling demand
 * @param demand_id Demand identifier returned by eos_sensor_demand_acquire
 * @return eos_result_t Operation result
 */
eos_result_t eos_sensor_demand_release(eos_sensor_demand_id_t demand_id);

/**
 * @brief Get the effective requested output period
 * @param type Sensor type
 * @return uint32_t Effective period in milliseconds, not a timestamp or a
 *         guaranteed hardware ODR
 */
uint32_t eos_sensor_get_sample_period(eos_sensor_type_t type);

/**
 * @brief Cancel a pending type-specific sensor operation
 * @param request_id Asynchronous sensor request identifier
 * @return eos_result_t Operation result
 */
eos_result_t eos_sensor_cancel(eos_sensor_request_id_t request_id);

/**
 * @brief Publish one standardized sample from a platform/driver
 * @param type Sensor type
 * @param data Sensor data in the canonical units documented by
 *        eos_dev_sensor.h; this function copies the value
 * @param timestamp Monotonic milliseconds since platform boot, captured when
 *        the platform obtained the sample
 */
void eos_sensor_notify(eos_sensor_type_t type, const eos_sensor_data_t *data, uint32_t timestamp);

/**
 * @brief Set sensor mode policy
 * @param mode Sensor operating mode
 * @param policy Policy configuration (NULL to reset to default)
 * @return eos_result_t Operation result
 */
eos_result_t eos_sensor_set_mode_policy(eos_sensor_mode_t mode, const eos_sensor_policy_t *policy);

/**
 * @brief Get current sensor mode policy
 * @param mode Sensor operating mode
 * @param policy Output for policy configuration
 * @return eos_result_t Operation result
 */
eos_result_t eos_sensor_get_mode_policy(eos_sensor_mode_t mode, eos_sensor_policy_t *policy);

/**
 * @brief Get current sensor operating mode
 * @return eos_sensor_mode_t Current mode
 */
eos_sensor_mode_t eos_sensor_get_current_mode(void);

#ifdef __cplusplus
}
#endif

#endif /* EOS_SERVICE_SENSOR_H */
