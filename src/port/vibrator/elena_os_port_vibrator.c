/**
 * @file elena_os_port_vibrator.c
 * @brief Vibrator
 * @author Sab1e
 * @date 2025-12-10
 */

#include "elena_os_port_vibrator.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "elena_os_port.h"
#include "elena_os_log.h"

/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

EOS_WEAK void eos_vibrator_on(uint8_t strength)
{
    EOS_LOG_I("Vibrator on: %d", strength);
}

EOS_WEAK void eos_vibrator_off(void)
{
    EOS_LOG_I("Vibrator off");
}
