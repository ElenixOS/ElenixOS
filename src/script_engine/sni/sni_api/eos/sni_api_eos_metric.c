/**
 * @file sni_api_eos_metric.c
 * @brief SNI binding for semantic Metric data
 */

#include "sni_api_eos_metric.h"

/* Includes ---------------------------------------------------*/
#include <math.h>
#include "eos_mem.h"
#include "eos_service_metric.h"
#include "script_engine_core.h"
#include "sni_callback_runtime.h"
#define EOS_LOG_TAG "SNI_Metric"
#include "eos_log.h"

/* Macros and Definitions -------------------------------------*/

/* Function Implementations -----------------------------------*/

static bool _metric_string(jerry_value_t value, char **out)
{
    jerry_size_t length;
    char *buffer;

    if (!out || !jerry_value_is_string(value))
        return false;
    length = jerry_string_size(value, JERRY_ENCODING_UTF8);
    buffer = eos_malloc(length + 1U);
    if (!buffer)
        return false;
    jerry_string_to_buffer(value, JERRY_ENCODING_UTF8, (jerry_char_t *)buffer, length);
    buffer[length] = '\0';
    *out = buffer;
    return true;
}

static bool _metric_interval(jerry_value_t value, uint32_t *interval_ms)
{
    double number;

    if (!interval_ms || !jerry_value_is_number(value))
        return false;
    number = jerry_value_as_number(value);
    if (number < 100.0 || number > 60000.0 || floor(number) != number)
        return false;
    *interval_ms = (uint32_t)number;
    return true;
}

static jerry_value_t _metric_to_js(eos_metric_id_t id, const eos_metric_value_t *sample)
{
    jerry_value_t object = jerry_object();
    const char *name = eos_metric_name(id);

    script_engine_set_prop_string(object, "metric", name ? name : "unknown");
    if (sample->boolean_value)
        jerry_value_free(jerry_object_set_sz(object, "value", jerry_boolean(sample->value != 0.0)));
    else
        script_engine_set_prop_number(object, "value", sample->value);
    script_engine_set_prop_string(object, "unit", sample->unit ? sample->unit : "");
    script_engine_set_prop_number(object, "timestamp", (double)sample->timestamp);
    jerry_value_free(jerry_object_set_sz(object, "valid", jerry_boolean(sample->valid)));
    jerry_value_free(jerry_object_set_sz(object, "stale", jerry_boolean(sample->stale)));
    return object;
}

jerry_value_t sni_api_eos_metric_get(const jerry_call_info_t *call_info_p,
                                     const jerry_value_t args_p[],
                                     const jerry_length_t args_count)
{
    char *name = NULL;
    eos_metric_id_t id;
    eos_metric_value_t sample;

    (void)call_info_p;
    if (args_count != 1U || !_metric_string(args_p[0], &name))
        return sni_api_throw_error("Usage: metric.get(name)");
    if (!eos_metric_parse(name, &id))
    {
        eos_free(name);
        return sni_api_throw_error("Unknown Metric");
    }
    eos_free(name);
    if (eos_metric_read(id, &sample) != EOS_OK)
        return sni_api_throw_error("Failed to read Metric");
    return _metric_to_js(id, &sample);
}

jerry_value_t sni_api_eos_metric_subscribe(const jerry_call_info_t *call_info_p,
                                           const jerry_value_t args_p[],
                                           const jerry_length_t args_count)
{
    char *name = NULL;
    eos_metric_id_t id;
    uint32_t interval_ms = 1000U;
    uint32_t handle;
    sni_context_t *ctx;

    (void)call_info_p;
    if ((args_count != 2U && args_count != 3U) || !_metric_string(args_p[0], &name)
        || !jerry_value_is_function(args_p[1]))
    {
        return sni_api_throw_error("Usage: metric.subscribe(name, callback, interval_ms?)");
    }
    if (args_count == 3U && !_metric_interval(args_p[2], &interval_ms))
    {
        eos_free(name);
        return sni_api_throw_error("Invalid Metric interval");
    }
    if (!eos_metric_parse(name, &id))
    {
        eos_free(name);
        return sni_api_throw_error("Unknown Metric");
    }
    eos_free(name);
    ctx = sni_cb_get_context();
    if (!ctx || !sni_cb_metric_subscribe(ctx, (uint32_t)id, args_p[1], interval_ms, &handle))
        return sni_api_throw_error("Failed to subscribe Metric");
    return jerry_number((double)handle);
}

jerry_value_t sni_api_eos_metric_unsubscribe(const jerry_call_info_t *call_info_p,
                                             const jerry_value_t args_p[],
                                             const jerry_length_t args_count)
{
    double value;
    sni_context_t *ctx;

    (void)call_info_p;
    if (args_count != 1U || !jerry_value_is_number(args_p[0]))
        return sni_api_throw_error("Usage: metric.unsubscribe(handle)");
    value = jerry_value_as_number(args_p[0]);
    if (value <= 0.0 || value > 4294967295.0 || floor(value) != value)
        return sni_api_throw_error("Invalid Metric handle");
    ctx = sni_cb_get_context();
    return jerry_boolean(ctx && sni_cb_metric_unsubscribe(ctx, (uint32_t)value));
}

const sni_method_desc_t eos_metric_static_methods[] = {
    {.name = "get", .handler = sni_api_eos_metric_get},
    {.name = "subscribe", .handler = sni_api_eos_metric_subscribe},
    {.name = "unsubscribe", .handler = sni_api_eos_metric_unsubscribe},
    {.name = NULL, .handler = NULL},
};
