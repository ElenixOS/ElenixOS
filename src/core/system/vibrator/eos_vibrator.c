/**
 * @file eos_vibrator.c
 * @brief Vibrator driver
 */

#include "eos_vibrator.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "eos_sys.h"
#include "eos_utils.h"
/* Macros and Definitions -------------------------------------*/

// Duration in ms
#define _TICK_PERIOD 20
#define _BUZZ_PERIOD 50
#define _VIBRATE_LONG_PERIOD 150

// Vibration strength [0,255]
#define _TICK_STRENGTH 127
#define _BUZZ_STRENGTH 200
#define _VIBRATE_LONG_STRENGTH 255

/* Variables --------------------------------------------------*/
static lv_timer_t *t;
static eos_vibrator_strength_t strength_option;
/* Function Implementations -----------------------------------*/

static void _vibrator_timer_stop_cb(lv_timer_t *t)
{
    eos_vibrator_off();
    lv_timer_pause(t);
}

static void _vibrator_common(uint32_t period, uint8_t strength)
{
    switch (strength_option)
    {
    case EOS_VIBRATOR_STRENGTH_OFF:
        return;
    case EOS_VIBRATOR_STRENGTH_INTENSE:
        strength += 25;
        break;
    default:
        break;
    }
    EOS_CLAMP(strength, 0, 255);
    eos_vibrator_on(strength);

    lv_timer_set_period(t, period);
    lv_timer_reset(t);
    lv_timer_resume(t);
}

void eos_vibrator_tick(void)
{
    _vibrator_common(_TICK_PERIOD, _TICK_STRENGTH);
}

void eos_vibrator_buzz(void)
{
    _vibrator_common(_BUZZ_PERIOD, _BUZZ_STRENGTH);
}

void eos_vibrator_vibrate_long(void)
{
    _vibrator_common(_VIBRATE_LONG_PERIOD, _VIBRATE_LONG_STRENGTH);
}

void eos_vibrator_set_strength(eos_vibrator_strength_t s)
{
    strength_option = s;
    EOS_CLAMP(strength_option, 0, 255);
    eos_sys_cfg_set_number(EOS_SYS_CFG_KEY_VIBRATOR_STRENGTH_NUMBER, strength_option);
}

void eos_vibrator_init(void)
{
    t = lv_timer_create_basic();
    lv_timer_set_period(t, 0);
    lv_timer_set_repeat_count(t, -1);
    lv_timer_set_cb(t, _vibrator_timer_stop_cb);
    lv_timer_pause(t);
}
