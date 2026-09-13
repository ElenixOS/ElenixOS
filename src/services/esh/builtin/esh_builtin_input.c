/**
 * @file esh_builtin_input.c
 * @brief ESH commands for logical hardware button input
 */

#include "esh_builtin_commands.h"

/* Includes ---------------------------------------------------*/
#include <string.h>
#include "input/eos_input.h"
#include "input/crown/eos_crown.h"
#include "input/side_button/eos_side_button.h"

/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/

/* Function Prototypes ----------------------------------------*/

/* Function Implementations -----------------------------------*/

static int _esh_button_help(esh_cmd_ctx_t *ctx)
{
    if (esh_printf(ctx, "usage: button <crown|sideButton> <click|longPress>\r\n") != EOS_OK
        || esh_printf(ctx, "  button crown click\r\n") != EOS_OK
        || esh_printf(ctx, "  button sideButton click\r\n") != EOS_OK
        || esh_printf(ctx, "button events are queued for asynchronous dispatch\r\n") != EOS_OK)
    {
        return EOS_ERR_IO;
    }

    return EOS_OK;
}

int esh_builtin_cmd_button(esh_cmd_ctx_t *ctx, int argc, char *argv[])
{
    eos_button_state_t state;
    const char *device;
    const char *action;

    if (!ctx || !argv)
    {
        return EOS_ERR_INVALID_ARG;
    }

    if (argc == 1 || (argc == 2 && strcmp(argv[1], "help") == 0))
    {
        return _esh_button_help(ctx);
    }

    if (argc != 3)
    {
        return (int)esh_printf(ctx, "button: usage: button <crown|sideButton> <click|longPress>\r\n");
    }

    device = argv[1];
    action = argv[2];
    if (strcmp(action, "click") == 0)
    {
        state = EOS_BUTTON_STATE_CLICKED;
    }
    else if (strcmp(action, "longPress") == 0)
    {
        state = EOS_BUTTON_STATE_LONG_PRESSED;
    }
    else
    {
        return (int)esh_printf(ctx, "button: unknown action '%s'; use 'button help'\r\n", action);
    }

    if (strcmp(device, "crown") == 0)
    {
        eos_crown_button_report(state);
    }
    else if (strcmp(device, "sideButton") == 0)
    {
        eos_side_button_report(state);
    }
    else
    {
        return (int)esh_printf(ctx, "button: unknown device '%s'; use 'button help'\r\n", device);
    }

    return (int)esh_printf(ctx, "button: %s %s queued\r\n", device, action);
}
