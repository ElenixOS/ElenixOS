/**
 * @file elena_os_port.c
 * @brief ElenaOS 移植
 * @author Sab1e
 * @date 2025-08-21
 */

#include "elena_os_port.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_ATOMICS__)
#include <stdatomic.h>
#define HAS_ATOMIC 1
#else
#define HAS_ATOMIC 0
#endif
/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/
#if HAS_ATOMIC
typedef struct eos_sem
{
    atomic_uint count;
    unsigned int max;
} eos_sem_t;
#else
typedef struct eos_sem
{
    uint32_t count;
    uint32_t max;
} eos_sem_t;
#endif

EOS_WEAK eos_sem_t *eos_sem_create(uint32_t initial_count, uint32_t max_count)
{
#if HAS_ATOMIC
    eos_sem_t *sem = (eos_sem_t *)malloc(sizeof(eos_sem_t));
    if (!sem)
        return NULL;
    atomic_init(&sem->count, initial_count);
    sem->max = max_count;
    return sem;
#else
    eos_sem_t *sem = (eos_sem_t *)malloc(sizeof(eos_sem_t));
    if (!sem)
        return NULL;
    sem->count = initial_count;
    sem->max = max_count;
    return sem;
#endif
}

EOS_WEAK void eos_sem_destroy(eos_sem_t *sem)
{
    if (sem)
        free(sem);
}

EOS_WEAK bool eos_sem_take(eos_sem_t *sem, uint32_t timeout_ms)
{
    (void)timeout_ms;
#if HAS_ATOMIC
    unsigned int c = atomic_load_explicit(&sem->count, memory_order_acquire);
    if (c == 0)
        return false;
    return atomic_compare_exchange_strong(&sem->count, &c, c - 1);
#else
    if (sem->count == 0)
        return false;
    sem->count--;
    return true;
#endif
}

EOS_WEAK void eos_sem_give(eos_sem_t *sem)
{
#if HAS_ATOMIC
    unsigned int c = atomic_load_explicit(&sem->count, memory_order_relaxed);
    if (c < sem->max)
    {
        atomic_fetch_add_explicit(&sem->count, 1, memory_order_release);
    }
#else
    if (sem->count < sem->max)
        sem->count++;
#endif
}

EOS_WEAK void *eos_malloc(size_t size)
{
    return malloc(size);
}

EOS_WEAK void *eos_malloc_zeroed(size_t size)
{
    return calloc(1, size);
}

EOS_WEAK void eos_free(void *ptr)
{
    free(ptr);
}

EOS_WEAK void *eos_malloc_large(size_t size)
{
    return malloc(size);
}

EOS_WEAK void eos_free_large(void *ptr)
{
    free(ptr);
}

EOS_WEAK void eos_delay(uint32_t ms)
{
    LV_UNUSED(ms);
    return;
}

EOS_WEAK void eos_cpu_reset(void)
{
    return;
}

EOS_WEAK void eos_bluetooth_enable(void)
{
    return;
}

EOS_WEAK void eos_bluetooth_disable(void)
{
    return;
}

EOS_WEAK eos_datetime_t eos_time_get(void)
{
    /* EXAMPLE */
    eos_datetime_t dt = {0};
    dt.year = 2025;
    dt.month = 9;
    dt.day = 7;
    dt.hour = 18;
    dt.min = 26;
    dt.sec = 13;
    dt.day_of_week = 7;
    return dt;
}

EOS_WEAK void eos_display_set_brightness(uint8_t brightness)
{
    LV_UNUSED(brightness);
    return;
}

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

EOS_WEAK void eos_locate_phone(void)
{
    return;
}

EOS_WEAK void eos_speaker_set_volume(uint8_t volume)
{
    LV_UNUSED(volume);
    return;
}
