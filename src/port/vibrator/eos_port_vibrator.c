/**
 * @file eos_port_vibrator.c
 * @brief Vibrator
 */

#include "eos_port_vibrator.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "eos_port.h"
#include "eos_log.h"

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
