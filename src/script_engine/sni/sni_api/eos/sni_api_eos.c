/**
 * @file sni_api_eos.c
 * @brief ElenaOS API
 * @author Sab1e
 * @date 2026-03-14
 */

#include "sni_api_eos.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "lvgl.h"
#include "sni_type_bridge.h"
#include "sni_types.h"
#include "sni_api_export.h"
#include "elena_os_log.h"
#include "elena_os_screen_mgr.h"
#include "script_engine_core.h"
/* Macros and Definitions -------------------------------------*/
#define EOS_API_NAME "eos"
#define CONSOLE_LOG_TAG script_engine_get_current_script_id()
/* Variables --------------------------------------------------*/
static jerry_value_t eos_api_obj;

typedef enum
{
    EOS_CONSOLE_LEVEL_LOG,
    EOS_CONSOLE_LEVEL_ERROR,
    EOS_CONSOLE_LEVEL_WARN,
    EOS_CONSOLE_LEVEL_DEBUG,
} eos_console_level_t;

static jerry_value_t sni_api_eos_console_write(const jerry_value_t args_p[],
                                               const jerry_length_t args_count,
                                               eos_console_level_t level)
{
    if (args_count < 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (jerry_value_is_string(args_p[0]))
    {
        const char *str;
        if (!sni_tb_js2c(args_p[0], SNI_T_STRING, &str))
        {
            return sni_api_throw_error("Failed to convert argument");
        }

        switch (level)
        {
            case EOS_CONSOLE_LEVEL_LOG:
                EOS_LOG_I("[%s] %s", CONSOLE_LOG_TAG, str);
                break;
            case EOS_CONSOLE_LEVEL_ERROR:
                EOS_LOG_E("[%s] %s", CONSOLE_LOG_TAG, str);
                break;
            case EOS_CONSOLE_LEVEL_WARN:
                EOS_LOG_W("[%s] %s", CONSOLE_LOG_TAG, str);
                break;
            case EOS_CONSOLE_LEVEL_DEBUG:
                EOS_LOG_D("[%s] %s", CONSOLE_LOG_TAG, str);
                break;
            default:
                return sni_api_throw_error("Invalid console log level");
        }
    }

    return jerry_undefined();
}
/* Function Implementations -----------------------------------*/

jerry_value_t sni_api_eos_screen_active(const jerry_call_info_t *call_info_p,
                                        const jerry_value_t args_p[],
                                        const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    lv_obj_t *result = eos_screen_active();
    return sni_tb_c2js(&result, SNI_H_LV_OBJ);
}

jerry_value_t sni_api_eos_console_log(const jerry_call_info_t *call_info_p,
                                      const jerry_value_t args_p[],
                                      const jerry_length_t args_count)
{
    (void)call_info_p;

    return sni_api_eos_console_write(args_p, args_count, EOS_CONSOLE_LEVEL_LOG);
}

jerry_value_t sni_api_eos_console_error(const jerry_call_info_t *call_info_p,
                                       const jerry_value_t args_p[],
                                       const jerry_length_t args_count)
{
    (void)call_info_p;

    return sni_api_eos_console_write(args_p, args_count, EOS_CONSOLE_LEVEL_ERROR);
}

jerry_value_t sni_api_eos_console_warn(const jerry_call_info_t *call_info_p,
                                      const jerry_value_t args_p[],
                                      const jerry_length_t args_count)
{
    (void)call_info_p;

    return sni_api_eos_console_write(args_p, args_count, EOS_CONSOLE_LEVEL_WARN);
}

jerry_value_t sni_api_eos_console_debug(const jerry_call_info_t *call_info_p,
                                       const jerry_value_t args_p[],
                                       const jerry_length_t args_count)
{
    (void)call_info_p;

    return sni_api_eos_console_write(args_p, args_count, EOS_CONSOLE_LEVEL_DEBUG);
}

const sni_method_desc_t eos_class_static_methods_screen[] = {
    {.name = "active", .handler = sni_api_eos_screen_active},
    {.name = NULL, .handler = NULL},
};

const sni_method_desc_t eos_class_static_methods_console[] = {
    {.name = "log", .handler = sni_api_eos_console_log},
    {.name = "error", .handler = sni_api_eos_console_error},
    {.name = "warn", .handler = sni_api_eos_console_warn},
    {.name = "info", .handler = sni_api_eos_console_log},
    {.name = "debug", .handler = sni_api_eos_console_debug},
    {.name = NULL, .handler = NULL},
};

const sni_class_desc_t eos_class_desc_screen = {
    .name = "screen",
    .constructor = NULL,
    .base_class = NULL,
    .methods = NULL,
    .properties = NULL,
    .static_methods = eos_class_static_methods_screen,
    .constants = NULL,
};

const sni_class_desc_t eos_class_desc_console = {
    .name = "console",
    .constructor = NULL,
    .base_class = NULL,
    .methods = NULL,
    .properties = NULL,
    .static_methods = eos_class_static_methods_console,
    .constants = NULL,
};

const sni_class_desc_t *const eos_api_classes[] = {
    &eos_class_desc_screen,
    &eos_class_desc_console,
    NULL,
};

void sni_api_eos_init(void)
{
    eos_api_obj = sni_api_build(eos_api_classes);
}

void sni_api_eos_mount(jerry_value_t realm)
{
    bool result = sni_api_mount(realm, eos_api_obj, EOS_API_NAME);
    if (!result)
    {
        EOS_LOG_E("Failed to mount ElenaOS API");
    }
}
