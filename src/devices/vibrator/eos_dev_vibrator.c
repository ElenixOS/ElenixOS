/**
 * @file eos_dev_vibrator.c
 * @brief Vibrator device
 */

#include "eos_dev_vibrator.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#define EOS_LOG_TAG "DevVibrator"
#include "eos_log.h"
#include "eos_error.h"

/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/

static eos_dev_vibrator_t _dev_vibrator = {
    .ops = NULL,
    .user_data = NULL,
    ._state = DEV_STATE_NONE,
};

/* Function Implementations -----------------------------------*/

eos_dev_vibrator_t *eos_dev_vibrator_get_instance(void)
{
    return &_dev_vibrator;
}

eos_result_t eos_dev_vibrator_register(const eos_dev_vibrator_ops_t *ops)
{
    if (ops == NULL)
    {
        EOS_LOG_E("OPS is NULL");
        return EOS_ERR_INVALID_ARG;
    }

    if (_dev_vibrator._state != DEV_STATE_NONE)
    {
        EOS_LOG_W("Already registered");
        return EOS_ERR_ALREADY_EXISTS;
    }

    if (ops->on == NULL || ops->off == NULL)
    {
        EOS_LOG_E("OPS incomplete");
        return EOS_ERR_INVALID_ARG;
    }

    _dev_vibrator.ops = ops;
    _dev_vibrator._state = DEV_STATE_READY;
    EOS_LOG_I("Vibrator registered");
    return EOS_OK;
}

eos_dev_state_t eos_dev_vibrator_get_state(void)
{
    return _dev_vibrator._state;
}

void eos_dev_vibrator_report(eos_dev_state_t state)
{
    if (_dev_vibrator._state == state)
    {
        return;
    }
    EOS_LOG_I("State: %d -> %d", _dev_vibrator._state, state);
    _dev_vibrator._state = state;
}
