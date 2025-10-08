/**
 * @file elena_os_sensor.h
 * @brief 传感器定义
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

/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/**
 * @brief 传感器类型
 */
typedef enum
{
    EOS_SENSOR_UNKONWN = 0,
    EOS_SENSOR_ACCE,      /**< 加速度传感器     */
    EOS_SENSOR_GYRO,      /**< 重力传感器       */
    EOS_SENSOR_MAG,       /**< 磁传感器         */
    EOS_SENSOR_TEMP,      /**< 温度传感器       */
    EOS_SENSOR_HUMI,      /**< 相对湿度传感器   */
    EOS_SENSOR_BARO,      /**< 气压传感器       */
    EOS_SENSOR_LIGHT,     /**< 环境光传感器     */
    EOS_SENSOR_PROXIMITY, /**< 距离传感器       */
    EOS_SENSOR_HR,        /**< 心率传感器       */
    EOS_SENSOR_TVOC,      /**< TOVC传感器       */
    EOS_SENSOR_NOISE,     /**< 噪声传感器       */
    EOS_SENSOR_STEP,      /**< 计步传感器       */
    EOS_SENSOR_FORCE,     /**< 力传感器         */
    EOS_SENSOR_BAT        /**< 电池电量传感器    */
} eos_sensor_type_t;


/**
 * @brief 加速度传感器数据
 */
typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} eos_acce_data_t;

/**
 * @brief 陀螺仪数据
 */
typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} eos_gyro_data_t;

/**
 * @brief 磁力计数据
 */
typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} eos_mag_data_t;

/**
 * @brief 温度传感器数据
 */
typedef struct {
    int32_t temp;       /**< 单位: 0.01 °C  */
} eos_temp_data_t;

/**
 * @brief 湿度传感器数据
 */
typedef struct {
    uint32_t humidity;   /**< 单位: 0.01 %RH */
} eos_humi_data_t;

/**
 * @brief 气压传感器数据
 */
typedef struct {
    int32_t pressure;   /**< 单位: 0.01 hPa */
} eos_baro_data_t;

/**
 * @brief 光照传感器数据
 */
typedef struct {
    uint32_t lux;       /**< 单位: lx */
} eos_light_data_t;

/**
 * @brief 距离传感器数据
 */
typedef struct {
    uint16_t distance_mm;
} eos_proximity_data_t;

/**
 * @brief 心率传感器数据
 */
typedef struct {
    uint16_t heart_rate;    /**< 单位: bpm */
    uint16_t spo2;          /**< 单位: 0.01% */
} eos_heart_rate_data_t;

/**
 * @brief TVOC传感器数据
 */
typedef struct {
    uint16_t tvoc;     /**< 单位: ppb */
} eos_tvoc_data_t;

/**
 * @brief 噪声传感器数据
 */
typedef struct {
    uint16_t noise_db;  /**< 单位: 0.01 dB */
} eos_noise_data_t;

/**
 * @brief 计步传感器数据
 */
typedef struct {
    uint32_t steps;
} eos_step_data_t;

/**
 * @brief 力传感器数据
 */
typedef struct {
    uint32_t force;       /**< 单位: 0.01 牛顿 */
} eos_force_data_t;

/**
 * @brief 电池电量传感器数据
 */
typedef struct {
    uint8_t level;     /**< 单位: % */
    bool charging;  /**< 充电状态: false-未充电,true-充电中 */
} eos_bat_data_t;

/**
 * @brief 传感器数据共用体
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

/**
 * @brief 传感器结构体
 */
typedef struct
{
    eos_sensor_type_t type;
    eos_sensor_data_t data;
}eos_sensor_t;

typedef void (*eos_sensor_cb_t)(eos_sensor_t, void *);

/* Public function prototypes --------------------------------*/
void eos_sensor_tester_create(void);
/**
 * @brief 上报传感器数据
 * @param sensor 传感器指针
 */
void eos_sensor_report(eos_sensor_t *sensor);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_SENSOR_H */
