/**
 * @file elena_os_sensor.c
 * @brief 传感器测试应用
 * @author Sab1e
 * @date 2025-09-14
 */

#include "elena_os_sensor.h"

/**
 * TODO:
 * LVGL线程安全问题，非LVGL线程如何更新Label
 */

// Includes
#include <stdio.h>
#include <stdlib.h>
#include "lvgl.h"
#include "elena_os_log.h"
#include "elena_os_port.h"
#include "elena_os_nav.h"
// Macros and Definitions

// Variables

// Function Implementations

static void _heart_rate_cb(eos_sensor_t s, void *user_data)
{
    lv_obj_t *label = (lv_obj_t *)user_data;
    EOS_CHECK_PTR_RETURN(label);
    char str[256];
    snprintf(str,sizeof(str),
    "Heart Rate: %d  valid:%d\nSpO2: %d  valid:%d\n",
    s.data.hr.heart_rate,s.data.hr.heart_rate_valid,s.data.hr.spo2,s.data.hr.spo2_valid);
    EOS_LOG_D("%s",str);
    lv_label_set_text(label,str);
}

static void _heart_rate_rm_cb(lv_event_t *e)
{
    eos_sensor_stop(EOS_SENSOR_HR);
}

static void _heart_rate(lv_event_t *e)
{
    eos_sensor_type_t t = EOS_SENSOR_HR;
    lv_obj_t *scr = eos_nav_scr_create();
    lv_screen_load(scr);

    lv_obj_t *label = lv_label_create(scr);
    lv_obj_center(label);
    lv_label_set_text(label, "Measuring in progress...");

    eos_sensor_add_callback(t, _heart_rate_cb, (void *)label);

    eos_sensor_read(t);
}

EOS_ASYNC_SCREEN_CREATE(eos_sensor_tester_create){
    lv_obj_t *scr = lv_screen_active();
    lv_obj_t *test_list = lv_list_create(scr);
    lv_obj_set_size(test_list, lv_pct(100), lv_pct(100));

    lv_obj_t *btn;
    lv_list_add_text(test_list, "[ElenaOS Sensor Tester]");

    btn = lv_list_add_button(test_list, NULL, "Heart rate");
    lv_obj_add_event_cb(btn, _heart_rate, LV_EVENT_CLICKED, NULL);
}