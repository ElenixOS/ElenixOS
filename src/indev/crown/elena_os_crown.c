/**
 * @file elena_os_crown.c
 * @brief 表冠
 * @author Sab1e
 * @date 2025-11-13
 */

#include "elena_os_crown.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "elena_os_scene.h"
#include "elena_os_async.h"
/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/
static void _crown_button_async_cb(void *user_data)
{
    eos_button_state_t state = (eos_button_state_t)(intptr_t)user_data;
    switch (state)
    {
    case EOS_BUTTON_STATE_CLICKED:
        eos_scene_auto_switch();
    default:
        break;
    }
}

void eos_crown_button_report(eos_button_state_t state)
{
    eos_async_call(_crown_button_async_cb, (void *)(intptr_t)state);
}
