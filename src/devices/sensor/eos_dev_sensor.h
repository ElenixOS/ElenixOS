/**
 * @file eos_dev_sensor.h
 * @brief Sensor device header file
 */

#ifndef EOS_DEV_SENSOR_H
#define EOS_DEV_SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "eos_device.h"
#include "eos_error.h"
#include "eos_event.h"
#include "lvgl.h"

/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/**
 * @brief Sensor data and request contract
 *
 * The C Sensor API uses fixed-point integer values.  The scale is part of
 * the public contract and is the same on every platform:
 *
 *   ACCE      int32, milli-m/s^2       value / 1000 = m/s^2
 *   GYRO      int32, milli-deg/s        value / 1000 = deg/s
 *   MAG       int32, nT                 value / 1000 = microtesla
 *   TEMP      int32, milli-degC         value / 1000 = degC
 *   BARO      int32, Pa; temperature    pressure is Pa, temperature / 1000
 *             int32, milli-degC         is degC
 *   HUMIDITY  int32, milli-%RH          value / 1000 = %RH
 *   LIGHT     uint32, lux               no scaling
 *   PROXIMITY uint16, mm                no scaling
 *   HR        uint16, BPM               no scaling
 *   SPO2      uint16, percent           0..100 percent, no scaling
 *   ECG       uint16, source counts     no physical conversion is defined
 *   CAP       uint16, source counts     no physical conversion is defined
 *   STEP      uint32, count             monotonic count, no scaling
 *   PPG       uint32 red/IR ADC counts  source counts, no physical conversion
 *
 * ECG and CAP deliberately document source counts rather than inventing a
 * physical unit: the current project has no verified ADC reference or
 * calibration contract for those two sources.  PPG is also source data by
 * design, because its ADC scale depends on the optical front-end; it is not
 * a heart-rate result.
 *
 * Sensor samples use monotonic milliseconds since platform boot for their
 * timestamp.  The timestamp is captured by the platform when the sample is
 * acquired and is preserved by the Sensor Service.  It is not wall-clock
 * time.  A requested sample period is an output-rate request, not an exact
 * hardware cadence guarantee; the latest-sample and FIFO/subscribe APIs keep
 * their existing semantics.
 *
 * Three-axis samples use one right-handed ElenixOS device coordinate frame.
 * Platform adapters must convert the native IC axes into that frame before
 * calling eos_sensor_notify().  The exact physical +X/+Y/+Z orientation is a
 * board/platform contract; application code must never identify a board or
 * IC and apply another axis swap or sign inversion.
 *
 * A field value of zero is a valid physical/source value.  No sample is
 * represented by the service validity state, not by zero-initialized data:
 * eos_sensor_read_latest() returns EOS_ERR_NOT_FOUND until a real sample has
 * been published, while a published all-zero sample returns EOS_OK.
 *
 * Platform adapters apply fixed sensor-mount transforms before calling
 * eos_sensor_notify(); applications and type-specific sensor operations do
 * not apply board- or IC-specific axis corrections.
 */

/**
 * @brief Identifier for an asynchronous type-specific sensor operation
 */
typedef uint32_t eos_sensor_request_id_t;

/**
 * @brief Invalid asynchronous sensor request identifier
 */
#define EOS_SENSOR_REQUEST_INVALID (0U)

/**
 * @brief Sensor type definitions
 */
typedef enum
{
    EOS_SENSOR_TYPE_UNKNOWN = 0,
    EOS_SENSOR_TYPE_ACCE, /**< SI acceleration vector, milli-m/s^2 */
    EOS_SENSOR_TYPE_GYRO, /**< Angular velocity vector, milli-deg/s */
    EOS_SENSOR_TYPE_HR, /**< Derived heart-rate result, BPM */
    EOS_SENSOR_TYPE_SPO2, /**< Derived blood oxygen result, percent */
    EOS_SENSOR_TYPE_LIGHT, /**< Ambient illuminance, lux */
    EOS_SENSOR_TYPE_PROXIMITY, /**< Distance, millimeters */
    EOS_SENSOR_TYPE_ECG, /**< ECG source sample, project-defined counts */
    EOS_SENSOR_TYPE_TEMP, /**< Temperature, milli-degC */
    EOS_SENSOR_TYPE_MAG, /**< Magnetic field vector, nano-tesla */
    EOS_SENSOR_TYPE_BARO, /**< Absolute pressure in Pa plus milli-degC */
    EOS_SENSOR_TYPE_CAP, /**< Capacitance source sample, project-defined counts */
    EOS_SENSOR_TYPE_STEP, /**< Monotonic step count */
    EOS_SENSOR_TYPE_HUMIDITY, /**< Relative humidity, milli-%RH */
    EOS_SENSOR_TYPE_PPG, /**< Red/IR optical source samples, ADC counts */
    EOS_SENSOR_TYPE_MAX
} eos_sensor_type_t;

/**
 * @brief Accelerometer data in the ElenixOS device frame
 * @details Each component is acceleration in milli-m/s^2.  Divide by 1000
 *          to obtain m/s^2; approximately 9807 represents 1 g.
 */
typedef struct
{
    int32_t x; /**< X acceleration in milli-m/s^2. */
    int32_t y; /**< Y acceleration in milli-m/s^2. */
    int32_t z; /**< Z acceleration in milli-m/s^2. */
} eos_sensor_data_acce_t;

/**
 * @brief Gyroscope data in the ElenixOS device frame
 * @details Each component is angular velocity in milli-deg/s.  Divide by
 *          1000 to obtain degrees per second.
 */
typedef struct
{
    int32_t x; /**< X angular velocity in milli-deg/s. */
    int32_t y; /**< Y angular velocity in milli-deg/s. */
    int32_t z; /**< Z angular velocity in milli-deg/s. */
} eos_sensor_data_gyro_t;

/**
 * @brief Magnetometer data in the ElenixOS device frame
 * @details Each component is magnetic flux density in nT.  Divide by 1000
 *          to obtain microtesla.
 */
typedef struct
{
    int32_t x; /**< X magnetic field in nT. */
    int32_t y; /**< Y magnetic field in nT. */
    int32_t z; /**< Z magnetic field in nT. */
} eos_sensor_data_mag_t;

/**
 * @brief Temperature data
 * @details The value is temperature in milli-degC.  Divide by 1000 to
 *          obtain degrees Celsius.
 */
typedef struct
{
    int32_t temp; /**< Temperature in milli-degC. */
} eos_sensor_data_temp_t;

/**
 * @brief Barometer data
 * @details Pressure is absolute pressure in Pa.  The associated temperature
 *          is in milli-degC and must be divided by 1000 for degC.
 */
typedef struct
{
    int32_t pressure; /**< Absolute pressure in Pa. */
    int32_t temperature; /**< Barometer temperature in milli-degC. */
} eos_sensor_data_baro_t;

/**
 * @brief Relative humidity data
 * @details Relative humidity is stored in milli-%RH.  For example, 50000
 *          represents 50.000 %RH.
 */
typedef struct
{
    int32_t humidity; /**< Relative humidity in milli-%RH. */
} eos_sensor_data_humidity_t;

/**
 * @brief Light sensor data
 * @details Illuminance in lux.  The value is not scaled.
 */
typedef struct
{
    uint32_t lux; /**< Illuminance in lux. */
} eos_sensor_data_light_t;

/**
 * @brief Proximity sensor data
 * @details Distance in millimeters.  The value is not scaled.
 */
typedef struct
{
    uint16_t distance_mm; /**< Distance in millimeters. */
} eos_sensor_data_proximity_t;

/**
 * @brief Heart rate sensor data
 * @details Derived heart rate in beats per minute.  This is a result, not
 *          an optical source sample; raw optical data uses EOS_SENSOR_TYPE_PPG.
 */
typedef struct
{
    uint16_t heart_rate; /**< Heart rate in beats per minute (BPM). */
} eos_sensor_data_hr_t;

/**
 * @brief SpO2 sensor data
 * @details Blood oxygen saturation in percent, normally in the range 0..100.
 */
typedef struct
{
    uint16_t spo2; /**< Oxygen saturation in percent. */
} eos_sensor_data_spo2_t;

/**
 * @brief ECG sensor data
 * @details Source sample count.  No voltage unit or ADC reference is defined
 *          by the current Sensor Contract.
 */
typedef struct
{
    uint16_t ecg; /**< ECG source sample in project-defined counts. */
} eos_sensor_data_ecg_t;

/**
 * @brief Capacitance sensor data
 * @details Source count.  No capacitance unit or calibration is defined by
 *          the current Sensor Contract.
 */
typedef struct
{
    uint16_t cap; /**< Capacitance source sample in project-defined counts. */
} eos_sensor_data_cap_t;

/**
 * @brief Step counter data
 * @details Monotonic accumulated step count.  It is not a rate or a delta.
 */
typedef struct
{
    uint32_t steps; /**< Accumulated number of steps. */
} eos_sensor_data_step_t;

/**
 * @brief Photoplethysmography sample
 *
 * The values are ADC counts from the standard red and infrared optical
 * channels.  They have no additional scale conversion in the Sensor API and
 * are sensor-source data, not a heart-rate result.
 */
typedef struct
{
    uint32_t red; /**< Red-channel ADC count. */
    uint32_t ir; /**< Infrared-channel ADC count. */
} eos_sensor_data_ppg_t;

/**
 * @brief Battery sensor data
 * @details Battery level is a percentage in the range 0..100.  The charging
 *          flag is a boolean state.  This type is retained for platform
 *          integrations even though it is not currently an enum Sensor Type.
 */
typedef struct
{
    uint8_t level; /**< Battery level in percent. */
    bool charging; /**< True while the battery is charging. */
} eos_sensor_data_battery_t;

/**
 * @brief Sensor data union
 */
typedef union
{
    eos_sensor_data_acce_t acce;
    eos_sensor_data_gyro_t gyro;
    eos_sensor_data_mag_t mag;
    eos_sensor_data_temp_t temp;
    eos_sensor_data_baro_t baro;
    eos_sensor_data_humidity_t humidity;
    eos_sensor_data_light_t light;
    eos_sensor_data_proximity_t proximity;
    eos_sensor_data_hr_t hr;
    eos_sensor_data_spo2_t spo2;
    eos_sensor_data_ecg_t ecg;
    eos_sensor_data_cap_t cap;
    eos_sensor_data_step_t step;
    eos_sensor_data_battery_t battery;
    eos_sensor_data_ppg_t ppg;
} eos_sensor_data_t;

/**
 * @brief Standard Sensor API sample with timestamp
 * @details `data` uses the canonical units documented by the selected union
 *          member above.  `timestamp` is monotonic milliseconds since boot,
 *          captured by the producer/platform and preserved by the service.
 */
typedef struct
{
    eos_sensor_type_t type; /**< Identifies which union member is valid. */
    eos_sensor_data_t data; /**< Standardized sample value. */
    uint32_t timestamp; /**< Monotonic milliseconds since platform boot. */
} eos_sensor_raw_data_t;

typedef struct eos_dev_sensor_t eos_dev_sensor_t;

/**
 * @brief Sensor data-ready callback — called by the device driver when the hardware
 *        FIFO watermark is reached or new data is available via interrupt.
 * @param type  Sensor type that has data ready
 * @param count Number of samples available in the hardware FIFO
 */
typedef void (*eos_sensor_data_ready_cb_t)(eos_sensor_type_t type, uint32_t count);

/**
 * @brief Sensor device operations
 * @note Device layer does NOT provide read operation. Data is pushed to service layer
 *       via eos_sensor_notify(). Service layer manages FIFO and broadcasts to subscribers.
 *
 *       The last three ops are OPTIONAL (NULL = not supported). When not implemented,
 *       the service layer falls back to software-timer-based polling via the port layer.
 */
typedef struct
{
    /* Required ops -----------------------------------------------*/
    void (*init)(eos_dev_sensor_t *dev);
    void (*deinit)(eos_dev_sensor_t *dev);
    void (*enable)(eos_dev_sensor_t *dev);
    void (*disable)(eos_dev_sensor_t *dev);
    void (*set_sample_rate)(eos_dev_sensor_t *dev, uint32_t hz);
    void (*get_sample_rate)(eos_dev_sensor_t *dev, uint32_t *hz);

    /* Optional hardware-FIFO ops (NULL = unsupported) ------------*/
    /**
     * @brief Set the hardware FIFO watermark threshold.
     *        When the sensor's internal FIFO reaches this many samples,
     *        the driver fires the data_ready callback.
     * @param dev     Sensor device
     * @param samples Watermark in number of samples (0 = disable watermark IRQ)
     */
    void (*set_fifo_watermark)(eos_dev_sensor_t *dev, uint16_t samples);

    /**
     * @brief Flush (clear) the hardware FIFO.
     * @param dev Sensor device
     */
    void (*flush_fifo)(eos_dev_sensor_t *dev);

    /**
     * @brief Register a data-ready callback for interrupt-driven batch reads.
     *        When the hardware FIFO watermark is reached, the driver calls `cb`
     *        (from ISR or a deferred task). The service layer then reads batched
     *        data and pushes it through eos_sensor_notify().
     * @param dev Sensor device
     * @param cb  Callback (NULL = unregister)
     */
    void (*set_data_ready_cb)(eos_dev_sensor_t *dev, eos_sensor_data_ready_cb_t cb);
} eos_dev_sensor_ops_t;

/**
 * @brief Sensor device structure
 */
struct eos_dev_sensor_t
{
    const eos_dev_sensor_ops_t *ops;
    const char *name;
    eos_sensor_type_t type;
    eos_dev_state_t _state;
    eos_event_code_t _event_id;
    struct eos_dev_sensor_t *_next;
};

/* Public function prototypes ---------------------------------*/

/**
 * @brief Register a sensor device
 * @param name Device name
 * @param type Sensor type
 * @param ops Device operations pointer
 * @return eos_result_t Operation result
 */
eos_result_t eos_dev_sensor_register(const char *name, eos_sensor_type_t type, const eos_dev_sensor_ops_t *ops);

/**
 * @brief Find sensor device by name
 * @param name Device name
 * @return eos_dev_sensor_t* Device pointer or NULL
 */
eos_dev_sensor_t *eos_dev_sensor_find(const char *name);

/**
 * @brief Find sensor device by type
 * @param type Sensor type
 * @return eos_dev_sensor_t* Device pointer or NULL
 */
eos_dev_sensor_t *eos_dev_sensor_find_by_type(eos_sensor_type_t type);

/**
 * @brief Get default sensor device by type
 * @param type Sensor type
 * @return eos_dev_sensor_t* Device pointer or NULL
 */
eos_dev_sensor_t *eos_dev_sensor_get_default(eos_sensor_type_t type);

/**
 * @brief Get sensor device state
 * @param dev Device pointer
 * @return eos_dev_state_t Device state
 */
eos_dev_state_t eos_dev_sensor_get_state(eos_dev_sensor_t *dev);

/**
 * @brief Report sensor device state
 * @param dev Device pointer
 * @param state New state
 */
void eos_dev_sensor_report_state(eos_dev_sensor_t *dev, eos_dev_state_t state);

/**
 * @brief Get sensor device event ID
 * @param dev Device pointer
 * @return eos_event_code_t Event ID
 */
eos_event_code_t eos_dev_sensor_get_event_id(eos_dev_sensor_t *dev);

/**
 * @brief Get sensor device type
 * @param dev Device pointer
 * @return eos_sensor_type_t Sensor type
 */
eos_sensor_type_t eos_dev_sensor_get_type(eos_dev_sensor_t *dev);

/**
 * @brief Get the head of sensor list for iteration
 * @return eos_dev_sensor_t* Pointer to the first sensor in the list
 */
eos_dev_sensor_t *eos_dev_sensor_get_list_head(void);

#ifdef __cplusplus
}
#endif

#endif /* EOS_DEV_SENSOR_H */
