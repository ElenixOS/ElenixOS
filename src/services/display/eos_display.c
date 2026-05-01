/**
 * @file eos_display.c
 * @brief Display settings
 */

#include "eos_display.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "eos_port.h"
#include "eos_config.h"

/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/
static uint8_t last_target_brightness = 50;
/* Function Implementations -----------------------------------*/

/**
 * @brief Animation callback: set brightness during animation
 */
static void _brightness_anim_cb(void *var, int32_t v)
{
    LV_UNUSED(var);
    eos_display_set_brightness((uint8_t)v);
}

void eos_display_set_brightness_smooth(uint8_t start_brightness, uint8_t end_brightness, uint32_t duration_ms)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, NULL);
    lv_anim_set_values(&a, start_brightness, end_brightness);
    lv_anim_set_time(&a, duration_ms);
    lv_anim_set_exec_cb(&a, _brightness_anim_cb);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
    lv_anim_start(&a);
}

void eos_display_tmp_set_brightness_smooth(uint8_t target_brightness)
{
    uint8_t b = eos_config_get_number(EOS_CONFIG_KEY_DISPLAY_BRIGHTNESS_NUMBER, 50);
    eos_display_set_brightness_smooth(b, target_brightness, 300);
    last_target_brightness = target_brightness;
}

void eos_display_tmp_restore_brightness_smooth(void)
{
    uint8_t b = eos_config_get_number(EOS_CONFIG_KEY_DISPLAY_BRIGHTNESS_NUMBER, 50);
    eos_display_set_brightness_smooth(last_target_brightness, b, 300);
    last_target_brightness = 50;
}