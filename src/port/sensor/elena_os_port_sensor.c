/**
 * @file elena_os_port_sensor.c
 * @brief 传感器移植接口
 * @author Sab1e
 * @date 2025-12-10
 */

#include "elena_os_port_sensor.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "elena_os_port.h"
#include "elena_os_sensor.h"

/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

EOS_WEAK void eos_sensor_read_async(eos_sensor_type_t type)
{
    float value = 0;
    switch (type)
    {
    case EOS_SENSOR_TYPE_ACCE: /**< 加速度传感器 */
        // TODO: 处理加速度数据，例如三轴加速度值滤波或姿态估计
        printf("加速度传感器: %.2f\n", value);
        break;

    case EOS_SENSOR_TYPE_GYRO: /**< 重力传感器 */
        // TODO: 处理陀螺仪数据，用于角速度或方向变化检测
        printf("陀螺仪传感器: %.2f\n", value);
        break;

    case EOS_SENSOR_TYPE_MAG: /**< 磁传感器 */
        // TODO: 处理磁场数据，可用于电子罗盘或方向校准
        printf("磁传感器: %.2f\n", value);
        break;

    case EOS_SENSOR_TYPE_TEMP: /**< 温度传感器 */
        // TODO: 处理环境或芯片温度数据
        printf("温度传感器: %.2f°C\n", value);
        break;

    case EOS_SENSOR_TYPE_HUMI: /**< 相对湿度传感器 */
        // TODO: 处理湿度百分比
        printf("湿度传感器: %.2f%%RH\n", value);
        break;

    case EOS_SENSOR_TYPE_BARO: /**< 气压传感器 */
        // TODO: 处理气压数据，可用于海拔计算
        printf("气压传感器: %.2f hPa\n", value);
        break;

    case EOS_SENSOR_TYPE_LIGHT: /**< 环境光传感器 */
        // TODO: 处理光照强度数据，可用于自动亮度调节
        printf("光照传感器: %.2f lux\n", value);
        break;

    case EOS_SENSOR_TYPE_PROXIMITY: /**< 距离传感器 */
        // TODO: 处理接近检测，例如抬腕息屏逻辑
        printf("距离传感器: %.2f cm\n", value);
        break;

    case EOS_SENSOR_TYPE_HR: /**< 心率传感器 */
        // TODO: 处理心率信号，例如实时心率显示
        printf("心率传感器: %.2f bpm\n", value);
        break;

    case EOS_SENSOR_TYPE_TVOC: /**< TVOC传感器 */
        // TODO: 处理空气质量数据（总挥发性有机化合物浓度）
        printf("TVOC传感器: %.2f ppm\n", value);
        break;

    case EOS_SENSOR_TYPE_NOISE: /**< 噪声传感器 */
        // TODO: 处理噪声分贝值，可用于环境噪音监测
        printf("噪声传感器: %.2f dB\n", value);
        break;

    case EOS_SENSOR_TYPE_STEP: /**< 计步传感器 */
        // TODO: 处理步数累计或运动距离估算
        printf("计步传感器: %.0f 步\n", value);
        break;

    case EOS_SENSOR_TYPE_FORCE: /**< 力传感器 */
        // TODO: 处理压力/触控力度信息
        printf("力传感器: %.2f N\n", value);
        break;

    case EOS_SENSOR_TYPE_BAT: /**< 电池电量传感器 */
        // TODO: 处理电池电量百分比，用于电量显示
        printf("电池传感器: %.2f%%\n", value);
        break;

    case EOS_SENSOR_TYPE_UNKNOWN:
    default:
        printf("未知传感器类型 (%d)\n", type);
        break;
    }
}

EOS_WEAK void eos_sensor_read_sync(eos_sensor_type_t type, eos_sensor_t *out)
{
    eos_sensor_t sensor = {.type = type};
    float value = 0;
    switch (type)
    {
    case EOS_SENSOR_TYPE_ACCE: /**< 加速度传感器 */
        // TODO: 处理加速度数据，例如三轴加速度值滤波或姿态估计
        printf("加速度传感器: %.2f\n", value);
        break;

    case EOS_SENSOR_TYPE_GYRO: /**< 重力传感器 */
        // TODO: 处理陀螺仪数据，用于角速度或方向变化检测
        printf("陀螺仪传感器: %.2f\n", value);
        break;

    case EOS_SENSOR_TYPE_MAG: /**< 磁传感器 */
        // TODO: 处理磁场数据，可用于电子罗盘或方向校准
        printf("磁传感器: %.2f\n", value);
        break;

    case EOS_SENSOR_TYPE_TEMP: /**< 温度传感器 */
        // TODO: 处理环境或芯片温度数据
        printf("温度传感器: %.2f°C\n", value);
        break;

    case EOS_SENSOR_TYPE_HUMI: /**< 相对湿度传感器 */
        // TODO: 处理湿度百分比
        printf("湿度传感器: %.2f%%RH\n", value);
        break;

    case EOS_SENSOR_TYPE_BARO: /**< 气压传感器 */
        // TODO: 处理气压数据，可用于海拔计算
        printf("气压传感器: %.2f hPa\n", value);
        break;

    case EOS_SENSOR_TYPE_LIGHT: /**< 环境光传感器 */
        // TODO: 处理光照强度数据，可用于自动亮度调节
        printf("光照传感器: %.2f lux\n", value);
        break;

    case EOS_SENSOR_TYPE_PROXIMITY: /**< 距离传感器 */
        // TODO: 处理接近检测，例如抬腕息屏逻辑
        printf("距离传感器: %.2f cm\n", value);
        break;

    case EOS_SENSOR_TYPE_HR: /**< 心率传感器 */
        // TODO: 处理心率信号，例如实时心率显示
        printf("心率传感器: %.2f bpm\n", value);
        break;

    case EOS_SENSOR_TYPE_TVOC: /**< TVOC传感器 */
        // TODO: 处理空气质量数据（总挥发性有机化合物浓度）
        printf("TVOC传感器: %.2f ppm\n", value);
        break;

    case EOS_SENSOR_TYPE_NOISE: /**< 噪声传感器 */
        // TODO: 处理噪声分贝值，可用于环境噪音监测
        printf("噪声传感器: %.2f dB\n", value);
        break;

    case EOS_SENSOR_TYPE_STEP: /**< 计步传感器 */
        // TODO: 处理步数累计或运动距离估算
        printf("计步传感器: %.0f 步\n", value);
        break;

    case EOS_SENSOR_TYPE_FORCE: /**< 力传感器 */
        // TODO: 处理压力/触控力度信息
        printf("力传感器: %.2f N\n", value);
        break;

    case EOS_SENSOR_TYPE_BAT: /**< 电池电量传感器 */
        // TODO: 处理电池电量百分比，用于电量显示
        printf("电池传感器: %.2f%%\n", value);
        break;

    case EOS_SENSOR_TYPE_UNKNOWN:
    default:
        printf("未知传感器类型 (%d)\n", type);
        break;
    }

    if (out)
        memcpy(out, &sensor, sizeof(eos_sensor_t));
}
