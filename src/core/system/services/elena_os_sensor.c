/**
 * @file elena_os_sensor.c
 * @brief 传感器测试应用
 * @author Sab1e
 * @date 2025-09-14
 */

#include "elena_os_sensor.h"

// Includes
#include <stdio.h>
#include <stdlib.h>
#include "lvgl.h"
#include "elena_os_log.h"
#include "elena_os_port.h"
#include "elena_os_nav.h"
#include "elena_os_basic_widgets.h"
#include "elena_os_event.h"

// Macros and Definitions

// Variables

// Function Implementations

static void _heart_rate_cb(lv_event_t *e)
{
    lv_obj_t *label = (lv_obj_t *)lv_event_get_target(e);
    eos_sensor_t *s = (eos_sensor_t *)lv_event_get_param(e);
    EOS_CHECK_PTR_RETURN(label && s);
    char str[256];
    snprintf(str, sizeof(str),
             "Heart Rate: %d\nSpO2: %d\n",
             s->data.hr.heart_rate, s->data.hr.spo2);
    EOS_LOG_D("%s", str);
    lv_label_set_text(label, str);
}

static void _heart_rate_rm_cb(lv_event_t *e)
{
}

static void _heart_rate(lv_event_t *e)
{
    eos_sensor_type_t t = EOS_SENSOR_HR;
    lv_obj_t *scr = eos_nav_scr_create();
    eos_screen_bind_header(scr, "Heart Rate");
    lv_screen_load(scr);

    lv_obj_t *label = lv_label_create(scr);
    lv_obj_center(label);
    lv_label_set_text(label, "Measuring in progress...");

    eos_event_add_cb(
        label,
        _heart_rate_cb,
        EOS_EVENT_SENSOR_REPORT_HR,
        NULL);

    eos_sensor_read(t);
}

void eos_sensor_tester_create(void)
{
    lv_obj_t *scr = eos_nav_scr_create();
    eos_screen_bind_header(scr, "Heart Rate");
    lv_screen_load(scr);
    lv_obj_t *test_list = lv_list_create(scr);
    lv_obj_set_size(test_list, lv_pct(100), lv_pct(100));

    lv_obj_t *btn;
    lv_list_add_text(test_list, "[ElenaOS Sensor Tester]");

    btn = lv_list_add_button(test_list, NULL, "Heart rate");
    lv_obj_add_event_cb(btn, _heart_rate, LV_EVENT_CLICKED, NULL);
}

void eos_sensor_report(eos_sensor_t *sensor)
{
    EOS_CHECK_PTR_RETURN(sensor);
    uint32_t event_code = sensor->type + EOS_EVENT_SENSOR_REPORT_BASE;
    eos_event_broadcast(event_code, (void *)sensor);
}
