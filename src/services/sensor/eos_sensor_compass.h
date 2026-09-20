/**
 * @file eos_sensor_compass.h
 * @brief Compass capability of the Sensor API
 */

#ifndef EOS_SENSOR_COMPASS_H
#define EOS_SENSOR_COMPASS_H

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
 * @brief Compass operation state
 */
typedef enum
{
    EOS_SENSOR_COMPASS_STATE_WAITING = 0,
    EOS_SENSOR_COMPASS_STATE_UNSTABLE,
    EOS_SENSOR_COMPASS_STATE_NO_DATA,
    EOS_SENSOR_COMPASS_STATE_SUCCESS,
    EOS_SENSOR_COMPASS_STATE_TIMEOUT,
    EOS_SENSOR_COMPASS_STATE_UNAVAILABLE,
    EOS_SENSOR_COMPASS_STATE_ERROR,
    EOS_SENSOR_COMPASS_STATE_CANCELLED,
} eos_sensor_compass_state_t;

/**
 * @brief Standard compass result in the ElenixOS device frame
 * @details `heading_mdeg` is clockwise heading in milli-degrees; divide by
 *          1000 for degrees in the range [0, 360).  `field_strength_nt` is
 *          calibrated magnetic-field magnitude in nT; divide by 1000 for
 *          microtesla.  `timestamp` is monotonic milliseconds since boot.
 */
typedef struct
{
    int32_t heading_mdeg; /**< Heading in milli-degrees, unscaled in C. */
    int32_t field_strength_nt; /**< Field magnitude in nT. */
    uint32_t timestamp; /**< Monotonic milliseconds since platform boot. */
} eos_sensor_compass_result_t;

/**
 * @brief Compass operation callback
 * @param request_id Asynchronous sensor request identifier
 * @param state Current operation state
 * @param result Result on success, otherwise NULL
 * @param user_data Caller-provided callback data
 */
typedef void (*eos_sensor_compass_cb_t)(eos_sensor_request_id_t request_id,
                                        eos_sensor_compass_state_t state,
                                        const eos_sensor_compass_result_t *result,
                                        void *user_data);

/* Public function prototypes ---------------------------------*/

/**
 * @brief Start one asynchronous compass operation
 * @param callback Progress and completion callback
 * @param user_data Caller-provided callback data
 * @param timeout_ms Timeout in milliseconds, or zero for the default
 * @param request_id Output request identifier
 * @return eos_result_t Operation result
 */
eos_result_t eos_sensor_compass_start(eos_sensor_compass_cb_t callback,
                                      void *user_data,
                                      uint32_t timeout_ms,
                                      eos_sensor_request_id_t *request_id);

/**
 * @brief Cancel one compass operation
 * @param request_id Request identifier returned by eos_sensor_compass_start
 * @return eos_result_t Operation result
 */
eos_result_t eos_sensor_compass_cancel(eos_sensor_request_id_t request_id);

#ifdef __cplusplus
}
#endif

#endif /* EOS_SENSOR_COMPASS_H */
