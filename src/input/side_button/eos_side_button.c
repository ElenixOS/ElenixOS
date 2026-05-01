/**
 * @file eos_side_button.c
 * @brief Side button
 */

#include "eos_side_button.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "eos_dispatcher.h"
#include "input/eos_input.h"
#include "eos_control_center.h"
#include "eos_service_pm.h"
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
