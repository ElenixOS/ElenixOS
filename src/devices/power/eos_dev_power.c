/**
 * @file eos_dev_power.c
 * @brief Power device
 */

#include "eos_dev_power.h"

#define EOS_LOG_TAG "DevPower"
#include "eos_log.h"

static eos_dev_power_t _dev_power = {
    .ops = NULL,
    ._state = DEV_STATE_NONE,
};

eos_dev_power_t *eos_dev_power_get_instance(void)
{
    return &_dev_power;
}

eos_result_t eos_dev_power_register(const eos_dev_power_ops_t *ops)
{
    if (ops == NULL)
    {
        EOS_LOG_E("OPS is NULL");
        return EOS_ERR_INVALID_ARG;
    }

    if (_dev_power._state != DEV_STATE_NONE)
    {
        EOS_LOG_W("Already registered");
        return EOS_ERR_ALREADY_EXISTS;
    }

    if (ops->set_power == NULL)
    {
        EOS_LOG_E("OPS incomplete");
        return EOS_ERR_INVALID_ARG;
    }

    _dev_power.ops = ops;
    _dev_power._state = DEV_STATE_READY;
    EOS_LOG_I("Registered");
    return EOS_OK;
}

eos_dev_state_t eos_dev_power_get_state(void)
{
    return _dev_power._state;
}

void eos_dev_power_report(eos_dev_state_t state)
{
    if (_dev_power._state == state)
    {
        return;
    }
    EOS_LOG_I("State: %d -> %d", _dev_power._state, state);
    _dev_power._state = state;
}
