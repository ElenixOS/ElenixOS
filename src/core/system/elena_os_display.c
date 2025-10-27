/**
 * @file elena_os_display.c
 * @brief 显示设置
 * @author Sab1e
 * @date 2025-10-23
 */

#include "elena_os_display.h"

// Includes
#include <stdio.h>
#include <stdlib.h>
#include "elena_os_port.h"
#include "elena_os_config.h"

// Macros and Definitions

// Variables
static uint8_t last_target_brightness = 50;
// Function Implementations

/**
 * @brief 动画回调：在动画过程中设置亮度
 */
static void _brightness_anim_cb(void *var, int32_t v)
{
    EOS_UNUSED(var);
    eos_display_set_brightness((uint8_t)v);
}

void eos_display_set_brightness_smooth(uint8_t start_brightness, uint8_t end_brightness, uint32_t duration_ms)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, NULL);                                // 无需绑定具体对象
    lv_anim_set_values(&a, start_brightness, end_brightness); // 起止值
    lv_anim_set_time(&a, duration_ms);                        // 动画时间
    lv_anim_set_exec_cb(&a, _brightness_anim_cb);             // 执行回调
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);        // 使用平滑曲线
    lv_anim_start(&a);
}

void eos_display_tmp_set_brightness_smooth(uint8_t target_brightness)
{
    uint8_t b = eos_sys_cfg_get_number(EOS_SYS_CFG_KEY_DISPLAY_BRIGHTNESS, 50);
    eos_display_set_brightness_smooth(b, target_brightness, 300);
    last_target_brightness = target_brightness;
}

void eos_display_tmp_restore_brightness_smooth(void)
{
    uint8_t b = eos_sys_cfg_get_number(EOS_SYS_CFG_KEY_DISPLAY_BRIGHTNESS, 50);
    eos_display_set_brightness_smooth(last_target_brightness, b, 300);
    last_target_brightness = 50;
}
