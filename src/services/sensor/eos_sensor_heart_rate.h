/**
 * @file eos_sensor_heart_rate.h
 * @brief Heart-rate capability of the Sensor API
 */

#ifndef EOS_SENSOR_HEART_RATE_H
#define EOS_SENSOR_HEART_RATE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include "eos_error.h"
#include "eos_dev_sensor.h"

/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/**
 * @brief Heart-rate operation state
 */
typedef enum
{
    EOS_SENSOR_HEART_RATE_STATE_WAITING = 0,
    EOS_SENSOR_HEART_RATE_STATE_UNSTABLE,
    EOS_SENSOR_HEART_RATE_STATE_NO_CONTACT,
    EOS_SENSOR_HEART_RATE_STATE_NO_DATA,
    EOS_SENSOR_HEART_RATE_STATE_SUCCESS,
    EOS_SENSOR_HEART_RATE_STATE_TIMEOUT,
    EOS_SENSOR_HEART_RATE_STATE_UNAVAILABLE,
    EOS_SENSOR_HEART_RATE_STATE_ERROR,
    EOS_SENSOR_HEART_RATE_STATE_CANCELLED,
} eos_sensor_heart_rate_state_t;

/**
 * @brief Standard heart-rate result
 * @details `bpm` is beats per minute with no fixed-point scale.  The
 *          timestamp is monotonic milliseconds since platform boot and
 *          identifies the sample used for the result.
 */
typedef struct
{
    uint16_t bpm; /**< Heart rate in beats per minute (BPM), unscaled. */
    uint32_t timestamp; /**< Monotonic milliseconds since platform boot. */
} eos_sensor_heart_rate_result_t;

/**
 * @brief Heart-rate operation callback
 * @param request_id Asynchronous sensor request identifier
 * @param state Current operation state
 * @param result Result on success, otherwise NULL
 * @param user_data Caller-provided callback data
 */
typedef void (*eos_sensor_heart_rate_cb_t)(eos_sensor_request_id_t request_id,
                                           eos_sensor_heart_rate_state_t state,
                                           const eos_sensor_heart_rate_result_t *result,
                                           void *user_data);

/* Public function prototypes ---------------------------------*/

/**
 * @brief Start one asynchronous heart-rate operation
 * @param callback Progress and completion callback
 * @param user_data Caller-provided callback data
 * @param timeout_ms Timeout in milliseconds, or zero for the default
 * @param request_id Output request identifier
 * @return eos_result_t Operation result
 */
eos_result_t eos_sensor_heart_rate_start(eos_sensor_heart_rate_cb_t callback,
                                         void *user_data,
                                         uint32_t timeout_ms,
                                         eos_sensor_request_id_t *request_id);

/**
 * @brief Cancel one heart-rate operation
 * @param request_id Request identifier returned by eos_sensor_heart_rate_start
 * @return eos_result_t Operation result
 */
eos_result_t eos_sensor_heart_rate_cancel(eos_sensor_request_id_t request_id);

#ifdef __cplusplus
}
#endif

#endif /* EOS_SENSOR_HEART_RATE_H */
