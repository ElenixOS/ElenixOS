/**
 * @file elena_os_side_button.c
 * @brief Side button
 * @author Sab1e
 * @date 2025-11-13
 */

#include "elena_os_side_button.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "elena_os_dispatcher.h"
#include "elena_os_control_center.h"
#include "elena_os_pm.h"
/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

static void _side_button_async_cb(void *user_data)
{
    if (eos_pm_get_state() == EOS_PM_SLEEP)
    {
        eos_pm_wake_up();
        return;
    }
    eos_pm_reset_timer();
    eos_button_state_t state = (eos_button_state_t)(intptr_t)user_data;
    switch (state)
    {
    case EOS_BUTTON_STATE_CLICKED:
        eos_control_panel_slide_change();
    default:
        break;
    }
}

void eos_side_button_report(eos_button_state_t state)
{
    eos_dispatcher_call(_side_button_async_cb, (void *)(intptr_t)state);
}
