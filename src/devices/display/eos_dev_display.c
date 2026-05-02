/**
 * @file eos_dev_display.c
 * @brief Display device
 */

#include "eos_dev_display.h"

#define EOS_LOG_TAG "DevDisplay"
#include "eos_log.h"

static eos_dev_display_t _dev_display = {
    .ops = NULL,
    ._state = DEV_STATE_NONE,
};

eos_dev_display_t *eos_dev_display_get_instance(void)
{
    return &_dev_display;
}

eos_result_t eos_dev_display_register(const eos_dev_display_ops_t *ops)
{
    if (ops == NULL)
    {
        EOS_LOG_E("OPS is NULL");
        return EOS_ERR_INVALID_ARG;
    }

    if (_dev_display._state != DEV_STATE_NONE)
    {
        EOS_LOG_W("Already registered");
        return EOS_ERR_ALREADY_EXISTS;
    }

    if (ops->set_brightness == NULL)
    {
        EOS_LOG_E("OPS incomplete");
        return EOS_ERR_INVALID_ARG;
    }

    _dev_display.ops = ops;
    _dev_display._state = DEV_STATE_READY;
    EOS_LOG_I("Registered");
    return EOS_OK;
}

eos_dev_state_t eos_dev_display_get_state(void)
{
    return _dev_display._state;
}

void eos_dev_display_report_state(eos_dev_state_t state)
{
    if (_dev_display._state == state)
    {
        return;
    }
    EOS_LOG_I("State: %d -> %d", _dev_display._state, state);
    _dev_display._state = state;
}
