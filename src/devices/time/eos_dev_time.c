/**
 * @file eos_dev_time.c
 * @brief Time device
 */

#include "eos_dev_time.h"

#define EOS_LOG_TAG "DevTime"
#include "eos_log.h"

static eos_dev_time_t _dev_time = {
    .ops = NULL,
    ._state = DEV_STATE_NONE,
};

eos_dev_time_t *eos_dev_time_get_instance(void)
{
    return &_dev_time;
}

eos_result_t eos_dev_time_register(const eos_dev_time_ops_t *ops)
{
    if (ops == NULL)
    {
        EOS_LOG_E("OPS is NULL");
        return EOS_ERR_INVALID_ARG;
    }

    if (_dev_time._state != DEV_STATE_NONE)
    {
        EOS_LOG_W("Already registered");
        return EOS_ERR_ALREADY_EXISTS;
    }

    if (ops->get_datetime == NULL)
    {
        EOS_LOG_E("OPS incomplete");
        return EOS_ERR_INVALID_ARG;
    }

    _dev_time.ops = ops;
    _dev_time._state = DEV_STATE_READY;
    EOS_LOG_I("Registered");
    return EOS_OK;
}

eos_dev_state_t eos_dev_time_get_state(void)
{
    return _dev_time._state;
}

void eos_dev_time_report_state(eos_dev_state_t state)
{
    if (_dev_time._state == state)
    {
        return;
    }
    EOS_LOG_I("State: %d -> %d", _dev_time._state, state);
    _dev_time._state = state;
}
