/**
 * @file eos_service_display.c
 * @brief Display service
 */

#include "eos_service_display.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "lvgl.h"
#include "eos_config.h"
#include "eos_dev_display.h"

/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/
static uint8_t _saved_brightness = 50;
static uint8_t _temp_brightness = 50;

/* Function Implementations -----------------------------------*/

static void _set_brightness_direct(uint8_t brightness)
{
    eos_dev_display_t *dev = eos_dev_display_get_instance();
    if (dev->ops && dev->ops->set_brightness)
    {
        dev->ops->set_brightness(brightness);
    }
}

static void _brightness_anim_cb(void *var, int32_t v)
{
    LV_UNUSED(var);
    _set_brightness_direct((uint8_t)v);
}

void eos_display_set_brightness(uint8_t brightness, eos_display_duration_t duration_ms)
{
    _temp_brightness = brightness;
    if (duration_ms > 0)
    {
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, NULL);
        lv_anim_set_values(_saved_brightness, brightness);
        lv_anim_set_time(&a, duration_ms);
        lv_anim_set_exec_cb(&a, _brightness_anim_cb);
        lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
        lv_anim_start(&a);
    }
    else
    {
        _set_brightness_direct(brightness);
    }
}

void eos_display_power_on(void)
{
    eos_dev_display_t *dev = eos_dev_display_get_instance();
    if (dev->ops && dev->ops->power_on)
    {
        dev->ops->power_on();
    }
}

void eos_display_power_off(void)
{
    eos_dev_display_t *dev = eos_dev_display_get_instance();
    if (dev->ops && dev->ops->power_off)
    {
        dev->ops->power_off();
    }
}

void eos_display_restore(eos_display_duration_t duration_ms)
{
    _saved_brightness = eos_config_get_number(EOS_CONFIG_KEY_DISPLAY_BRIGHTNESS_NUMBER, 50);
    eos_display_set_brightness(_saved_brightness, duration_ms);
}
