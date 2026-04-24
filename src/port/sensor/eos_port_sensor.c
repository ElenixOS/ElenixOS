/**
 * @file eos_port_sensor.c
 * @brief Sensor porting interface
 */

#include "eos_port_sensor.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eos_port.h"
#include "eos_sensor.h"

/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

EOS_WEAK void eos_sensor_read_async(eos_sensor_type_t type)
{
    float value = 0;
    switch (type)
    {
    case EOS_SENSOR_TYPE_ACCE: /**< Accelerometer sensor */
        // TODO: Process acceleration data, such as three-axis acceleration value filtering or attitude estimation
        printf("Accelerometer sensor: %.2f\n", value);
        break;

    case EOS_SENSOR_TYPE_GYRO: /**< Gyroscope sensor */
        // TODO: Process gyroscope data for angular velocity or direction change detection
        printf("Gyroscope sensor: %.2f\n", value);
        break;

    case EOS_SENSOR_TYPE_MAG: /**< Magnetic sensor */
        // TODO: Process magnetic field data, can be used for electronic compass or direction calibration
        printf("Magnetic sensor: %.2f\n", value);
        break;

    case EOS_SENSOR_TYPE_TEMP: /**< Temperature sensor */
        // TODO: Process environment or chip temperature data
        printf("Temperature sensor: %.2f°C\n", value);
        break;

    case EOS_SENSOR_TYPE_HUMI: /**< Relative humidity sensor */
        // TODO: Process humidity percentage
        printf("Humidity sensor: %.2f%%RH\n", value);
        break;

    case EOS_SENSOR_TYPE_BARO: /**< Barometric pressure sensor */
        // TODO: Process air pressure data, can be used for altitude calculation
        printf("Barometric pressure sensor: %.2f hPa\n", value);
        break;

    case EOS_SENSOR_TYPE_LIGHT: /**< Ambient light sensor */
        // TODO: Process light intensity data, can be used for automatic brightness adjustment
        printf("Light sensor: %.2f lux\n", value);
        break;

    case EOS_SENSOR_TYPE_PROXIMITY: /**< Proximity sensor */
        // TODO: Process proximity detection, such as wrist lift to wake logic
        printf("Proximity sensor: %.2f cm\n", value);
        break;

    case EOS_SENSOR_TYPE_HR: /**< Heart rate sensor */
        // TODO: Process heart rate signals, such as real-time heart rate display
        printf("Heart rate sensor: %.2f bpm\n", value);
        break;

    case EOS_SENSOR_TYPE_TVOC: /**< TVOC sensor */
        // TODO: Process air quality data (total volatile organic compound concentration)
        printf("TVOC sensor: %.2f ppm\n", value);
        break;

    case EOS_SENSOR_TYPE_NOISE: /**< Noise sensor */
        // TODO: Process noise decibel value, can be used for environmental noise monitoring
        printf("Noise sensor: %.2f dB\n", value);
        break;

    case EOS_SENSOR_TYPE_STEP: /**< Step counter sensor */
        // TODO: Process step count or exercise distance estimation
        printf("Step sensor: %.0f steps\n", value);
        break;

    case EOS_SENSOR_TYPE_FORCE: /**< Force sensor */
        // TODO: Process pressure/touch force information
        printf("Force sensor: %.2f N\n", value);
        break;

    case EOS_SENSOR_TYPE_BAT: /**< Battery sensor */
        // TODO: Process battery percentage for battery level display
        printf("Battery sensor: %.2f%%\n", value);
        break;

    case EOS_SENSOR_TYPE_UNKNOWN:
    default:
        printf("Unknown sensor type (%d)\n", type);
        break;
    }
}

EOS_WEAK void eos_sensor_read_sync(eos_sensor_type_t type, eos_sensor_t *out)
{
    eos_sensor_t sensor = {.type = type};
    float value = 0;
    switch (type)
    {
    case EOS_SENSOR_TYPE_ACCE: /**< Accelerometer sensor */
        // TODO: Process acceleration data, such as three-axis acceleration value filtering or attitude estimation
        printf("Accelerometer sensor: %.2f\n", value);
        break;

    case EOS_SENSOR_TYPE_GYRO: /**< Gyroscope sensor */
        // TODO: Process gyroscope data for angular velocity or direction change detection
        printf("Gyroscope sensor: %.2f\n", value);
        break;

    case EOS_SENSOR_TYPE_MAG: /**< Magnetic sensor */
        // TODO: Process magnetic field data, can be used for electronic compass or direction calibration
        printf("Magnetic sensor: %.2f\n", value);
        break;

    case EOS_SENSOR_TYPE_TEMP: /**< Temperature sensor */
        // TODO: Process environment or chip temperature data
        printf("Temperature sensor: %.2f°C\n", value);
        break;

    case EOS_SENSOR_TYPE_HUMI: /**< Relative humidity sensor */
        // TODO: Process humidity percentage
        printf("Humidity sensor: %.2f%%RH\n", value);
        break;

    case EOS_SENSOR_TYPE_BARO: /**< Barometric pressure sensor */
        // TODO: Process air pressure data, can be used for altitude calculation
        printf("Barometric pressure sensor: %.2f hPa\n", value);
        break;

    case EOS_SENSOR_TYPE_LIGHT: /**< Ambient light sensor */
        // TODO: Process light intensity data, can be used for automatic brightness adjustment
        printf("Light sensor: %.2f lux\n", value);
        break;

    case EOS_SENSOR_TYPE_PROXIMITY: /**< Proximity sensor */
        // TODO: Process proximity detection, such as wrist lift to wake logic
        printf("Proximity sensor: %.2f cm\n", value);
        break;

    case EOS_SENSOR_TYPE_HR: /**< Heart rate sensor */
        // TODO: Process heart rate signals, such as real-time heart rate display
        printf("Heart rate sensor: %.2f bpm\n", value);
        break;

    case EOS_SENSOR_TYPE_TVOC: /**< TVOC sensor */
        // TODO: Process air quality data (total volatile organic compound concentration)
        printf("TVOC sensor: %.2f ppm\n", value);
        break;

    case EOS_SENSOR_TYPE_NOISE: /**< Noise sensor */
        // TODO: Process noise decibel value, can be used for environmental noise monitoring
        printf("Noise sensor: %.2f dB\n", value);
        break;

    case EOS_SENSOR_TYPE_STEP: /**< Step counter sensor */
        // TODO: Process step count or exercise distance estimation
        printf("Step sensor: %.0f steps\n", value);
        break;

    case EOS_SENSOR_TYPE_FORCE: /**< Force sensor */
        // TODO: Process pressure/touch force information
        printf("Force sensor: %.2f N\n", value);
        break;

    case EOS_SENSOR_TYPE_BAT: /**< Battery sensor */
        // TODO: Process battery percentage for battery level display
        printf("Battery sensor: %.2f%%\n", value);
        break;

    case EOS_SENSOR_TYPE_UNKNOWN:
    default:
        printf("Unknown sensor type (%d)\n", type);
        break;
    }

    if (out)
        memcpy(out, &sensor, sizeof(eos_sensor_t));
}
