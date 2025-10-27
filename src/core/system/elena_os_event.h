/**
 * @file elena_os_event.h
 * @brief 事件系统
 * @author Sab1e
 * @date 2025-08-17
 */

#ifndef ELENA_OS_EVENT_H
#define ELENA_OS_EVENT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"
/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/**
 * @brief 全局广播事件类型定义
 * @note 此处可添加新的事件
 */
typedef enum
{
    EOS_EVENT_BASE = LV_EVENT_LAST,
    EOS_EVENT_SLIDE_WIDGET_REACHED_THRESHOLD,  /**< 滑动超过阈值 */
    EOS_EVENT_SLIDE_WIDGET_REVERTED,           /**< 滑动未超过阈值自动回弹 */
    EOS_EVENT_SLIDE_WIDGET_MOVING,
    EOS_EVENT_SLIDE_WIDGET_DONE,
    EOS_EVENT_THEME_UPDATED,
    EOS_EVENT_APP_DELETED,
    EOS_EVENT_APP_INSTALLED,
    /* 此处添加新的事件 */
    EOS_EVENT_SENSOR_REPORT_BASE,      /**< 用于传感器事件序号对齐   */
    EOS_EVENT_SENSOR_REPORT_ACCE,      /**< 加速度传感器           */
    EOS_EVENT_SENSOR_REPORT_GYRO,      /**< 重力传感器             */
    EOS_EVENT_SENSOR_REPORT_MAG,       /**< 磁传感器              */
    EOS_EVENT_SENSOR_REPORT_TEMP,      /**< 温度传感器             */
    EOS_EVENT_SENSOR_REPORT_HUMI,      /**< 相对湿度传感器          */
    EOS_EVENT_SENSOR_REPORT_BARO,      /**< 气压传感器              */
    EOS_EVENT_SENSOR_REPORT_LIGHT,     /**< 环境光传感器            */
    EOS_EVENT_SENSOR_REPORT_PROXIMITY, /**< 距离传感器              */
    EOS_EVENT_SENSOR_REPORT_HR,        /**< 心率传感器              */
    EOS_EVENT_SENSOR_REPORT_TVOC,      /**< TOVC传感器             */
    EOS_EVENT_SENSOR_REPORT_NOISE,     /**< 噪声传感器             */
    EOS_EVENT_SENSOR_REPORT_STEP,      /**< 计步传感器             */
    EOS_EVENT_SENSOR_REPORT_FORCE,     /**< 力传感器               */
    EOS_EVENT_SENSOR_REPORT_BAT,       /**< 电池电量传感器          */
} eos_event_code_t;

/* Public function prototypes --------------------------------*/

/**
 * @brief 添加事件回调
 * @param obj 对象指针（对象被删除时回自动删除此回调）
 * @param event 事件类型
 * @param cb 回调函数
 * @param user_data 用户数据
 * @note eos_event_code_t 兼容 lv_event_code_t，因此可以直接传入事件号。
 * 示例：
 *
 * `eos_event_add_cb(obj,cb,LV_EVENT_ALL,NULL);`
 *
 * `eos_event_add_cb(obj,cb,EOS_EVENT_THEME_UPDATED,NULL);`
 *
 */
void eos_event_add_cb(lv_obj_t *obj, lv_event_cb_t cb, lv_event_code_t event, void *user_data);

/**
 * @brief 移除事件回调
 * - 如果未在广播：立即从链表中移除并释放，同时调用 lv_obj_remove_event_cb
 * - 如果当前正在广播：标记删除（延迟清理）
 * @param obj 对象指针
 * @param event 事件类型
 * @param cb 回调函数
 */
void eos_event_remove_cb(lv_obj_t *obj, lv_event_code_t event, lv_event_cb_t cb);

/**
 * @brief 广播事件
 * - 支持嵌套广播
 * - 在广播期间，对象或节点被删除时仅标记，广播结束后统一清理
 * @param event 要广播的事件类型
 * @param param 事件参数
 */
void eos_event_broadcast(lv_event_code_t event, void *param);
/**
 * @brief 允许在外部主动清理（例如系统空闲时调用）
 */
void eos_event_cleanup_now(void);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_EVENT_H */
