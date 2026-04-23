/**
 * @file elena_os_sensor.h
 * @brief Sensor definitions
 * @author Sab1e
 * @date 2025-09-14
 */

#ifndef ELENA_OS_SENSOR_H
#define ELENA_OS_SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "elena_os_core.h"
#include "elena_os_event.h"

/* Public macros ----------------------------------------------*/
#define EOS_SENSOR_NUMBER       _EOS_SENSOR_MAX - 1   /**< Total number of sensors */

/* Public typedefs --------------------------------------------*/

typedef enum
{
    EOS_SENSOR_STATE_IDLE,
    EOS_SENSOR_STATE_READING,
    EOS_SENSOR_STATE_ERROR,
} eos_sensor_state_t;

/**
 * @brief Sensor types
 */
typedef enum
{
    EOS_SENSOR_TYPE_UNKNOWN = 0,
    EOS_SENSOR_TYPE_ACCE,      /**< Accelerometer sensor     */
    EOS_SENSOR_TYPE_GYRO,      /**< Gyroscope sensor       */
    EOS_SENSOR_TYPE_MAG,       /**< Magnetometer sensor         */
    EOS_SENSOR_TYPE_TEMP,      /**< Temperature sensor       */
    EOS_SENSOR_TYPE_HUMI,      /**< Relative humidity sensor   */
    EOS_SENSOR_TYPE_BARO,      /**< Barometric pressure sensor       */
    EOS_SENSOR_TYPE_LIGHT,     /**< Ambient light sensor     */
    EOS_SENSOR_TYPE_PROXIMITY, /**< Proximity sensor       */
    EOS_SENSOR_TYPE_HR,        /**< Heart rate sensor       */
    EOS_SENSOR_TYPE_TVOC,      /**< TVOC sensor       */
    EOS_SENSOR_TYPE_NOISE,     /**< Noise sensor       */
    EOS_SENSOR_TYPE_STEP,      /**< Step counter sensor       */
    EOS_SENSOR_TYPE_FORCE,     /**< Force sensor         */
    EOS_SENSOR_TYPE_BAT,       /**< Battery level sensor    */
    _EOS_SENSOR_MAX            /**< Maximum enum value for sensors */
} eos_sensor_type_t;

/**
 * @brief Accelerometer sensor data
 */
typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} eos_acce_data_t;

/**
 * @brief Gyroscope sensor data
 */
typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} eos_gyro_data_t;

/**
 * @brief Magnetometer sensor data
 */
typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} eos_mag_data_t;

/**
 * @brief Temperature sensor data
 */
typedef struct {
    int32_t temp;       /**< Unit: 0.01 °C  */
} eos_temp_data_t;

/**
 * @brief Humidity sensor data
 */
typedef struct {
    uint32_t humidity;   /**< Unit: 0.01 %RH */
} eos_humi_data_t;

/**
 * @brief Barometric pressure sensor data
 */
typedef struct {
    int32_t pressure;   /**< Unit: 0.01 hPa */
} eos_baro_data_t;

/**
 * @brief Light sensor data
 */
typedef struct {
    uint32_t lux;       /**< Unit: lx */
} eos_light_data_t;

/**
 * @brief Proximity sensor data
 */
typedef struct {
    uint16_t distance_mm;
} eos_proximity_data_t;

/**
 * @brief Heart rate sensor data
 */
typedef struct {
    uint16_t heart_rate;    /**< Unit: bpm */
    uint16_t spo2;          /**< Unit: 0.01% */
} eos_heart_rate_data_t;

/**
 * @brief TVOC sensor data
 */
typedef struct {
    uint16_t tvoc;     /**< Unit: ppb */
} eos_tvoc_data_t;

/**
 * @brief Noise sensor data
 */
typedef struct {
    uint16_t noise_db;  /**< Unit: 0.01 dB */
} eos_noise_data_t;

/**
 * @brief Step counter sensor data
 */
typedef struct {
    uint32_t steps;
} eos_step_data_t;

/**
 * @brief Force sensor data
 */
typedef struct {
    uint32_t force;       /**< Unit: 0.01 Newtons */
} eos_force_data_t;

/**
 * @brief Battery level sensor data
 */
typedef struct {
    uint8_t level;     /**< Unit: % */
    bool charging;  /**< Charging status: false-not charging, true-charging */
} eos_bat_data_t;

/**
 * @brief Sensor data union
 */
typedef union
{
    eos_acce_data_t acce;
    eos_gyro_data_t gyro;
    eos_mag_data_t mag;
    eos_temp_data_t temp;
    eos_humi_data_t humi;
    eos_baro_data_t baro;
    eos_light_data_t light;
    eos_proximity_data_t proximity;
    eos_heart_rate_data_t hr;
    eos_tvoc_data_t tvoc;
    eos_noise_data_t noise;
    eos_step_data_t step;
    eos_force_data_t force;
    eos_bat_data_t bat;
}eos_sensor_data_t;

typedef uint8_t eos_sensor_id_t;

/**
 * @brief Sensor structure
 */
typedef struct
{
    eos_sensor_id_t id;
    eos_sensor_type_t type;
    eos_sensor_data_t data;
}eos_sensor_t;

typedef void (*eos_sensor_cb_t)(eos_sensor_t, void *);

/* Public function prototypes --------------------------------*/
/**
 * @brief Report sensor data
 * @param sensor Sensor pointer
 */
void eos_sensor_report(eos_sensor_t *sensor);
/**
 * @brief Get first sensor instance by sensor type
 * @param type Sensor type
 * @return eos_sensor_t* Returns sensor pointer on success, otherwise returns NULL
 */
eos_sensor_t *eos_sensor_get(eos_sensor_type_t type);
/**
 * @brief Find sensor instance by type and index
 * @param type Sensor type
 * @param index Sensor index
 * @return eos_sensor_t* Returns sensor pointer on success, otherwise returns NULL
 */
eos_sensor_t *eos_sensor_find(eos_sensor_type_t type, uint8_t index);
/**
 * @brief Register sensor instance (ID is automatically assigned)
 * @param type Sensor type
 * @return eos_sensor_t* Returns sensor pointer on success, otherwise returns NULL
 */
eos_sensor_t *eos_sensor_register(eos_sensor_type_t type);
/**
 * @brief Get count of sensors of a certain type
 * @param type Sensor type
 * @return uint8_t Count
 */
uint8_t eos_sensor_get_count(eos_sensor_type_t type);
/**
 * @brief Get sensor event code
 * @param type Sensor type
 * @return eos_event_code_t Event code
 */
eos_event_code_t eos_sensor_get_event_code(eos_sensor_type_t type);
/**
 * @brief Get sensor type by event code
 * @param code Event code
 * @return eos_sensor_type_t Sensor type
 */
eos_sensor_type_t eos_sensor_get_type_by_event_code(eos_event_code_t code);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_SENSOR_H */