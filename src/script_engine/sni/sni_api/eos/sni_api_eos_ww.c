/**
 * @file sni_api_eos_ww.c
 * @brief SNI binding for complex watchface widgets
 */

#include "sni_api_eos_ww.h"

/* Includes ---------------------------------------------------*/
#include <math.h>
#include <stdint.h>
#include <string.h>
#include "eos_mem.h"
#include "eos_service_metric.h"
#include "eos_ww_analog_dial.h"
#include "eos_ww_calendar_grid.h"
#include "eos_ww_compass_dial.h"
#include "eos_ww_metric_rings.h"
#include "eos_ww_radial_gauge.h"
#include "eos_ww_trend_chart.h"
#include "sni_type_bridge.h"
#define EOS_LOG_TAG "SNI_WW"
#include "eos_log.h"

/* Macros and Definitions -------------------------------------*/
#define _DEFAULT_COLOR 0xD4DAE4U

/* Function Implementations -----------------------------------*/
static bool _number(jerry_value_t value, double *out)
{
    if (!jerry_value_is_number(value) || !out)
        return false;
    *out = jerry_value_as_number(value);
    return isfinite(*out);
}

static bool _bool(jerry_value_t value, bool *out)
{
    if (!jerry_value_is_boolean(value) || !out)
        return false;
    *out = jerry_value_is_true(value);
    return true;
}

static bool _string(jerry_value_t value, char *buffer, size_t capacity)
{
    jerry_size_t length;
    if (!jerry_value_is_string(value) || !buffer || capacity == 0U)
        return false;
    length = jerry_string_size(value, JERRY_ENCODING_UTF8);
    if (length >= capacity)
        return false;
    jerry_string_to_buffer(value, JERRY_ENCODING_UTF8, (jerry_char_t *)buffer, length);
    buffer[length] = '\0';
    return true;
}

static bool _read_number(jerry_value_t object, const char *name, double *out)
{
    jerry_value_t value = jerry_object_get_sz(object, name);
    bool ok = _number(value, out);
    jerry_value_free(value);
    return ok;
}

static bool _read_bool(jerry_value_t object, const char *name, bool *out)
{
    jerry_value_t value = jerry_object_get_sz(object, name);
    bool ok = _bool(value, out);
    jerry_value_free(value);
    return ok;
}

static bool _read_string(jerry_value_t object, const char *name, char *buffer, size_t capacity)
{
    jerry_value_t value = jerry_object_get_sz(object, name);
    bool ok = _string(value, buffer, capacity);
    jerry_value_free(value);
    return ok;
}

static jerry_value_t _read_object(jerry_value_t object, const char *name)
{
    jerry_value_t value = jerry_object_get_sz(object, name);
    if (!jerry_value_is_object(value))
    {
        jerry_value_free(value);
        return jerry_undefined();
    }
    return value;
}

static bool _parse_metric(jerry_value_t object, const char *property, eos_metric_id_t *id)
{
    char name[64];
    if (!_read_string(object, property, name, sizeof(name)))
        return false;
    return eos_metric_parse(name, id);
}

static bool _get_parent(const jerry_value_t args_p[], lv_obj_t **parent)
{
    *parent = NULL;
    return sni_tb_js2c_parent(args_p[0], (void **)parent) && *parent;
}

static jerry_value_t _return_widget(lv_obj_t *object)
{
    if (!object)
        return sni_api_throw_error("Failed to create watchface widget");
    return sni_tb_c2js(&object, SNI_H_LV_OBJ);
}

static void _apply_position(jerry_value_t options, lv_obj_t *object)
{
    double x;
    double y;
    if (!object)
        return;
    if (_read_number(options, "x", &x) && _read_number(options, "y", &y))
        lv_obj_set_pos(object, (lv_coord_t)x, (lv_coord_t)y);
}

static void _analog_config_defaults(eos_ww_analog_dial_config_t *config)
{
    memset(config, 0, sizeof(*config));
    config->width = 220;
    config->height = 220;
    config->major_ticks.count = 12;
    config->major_ticks.length = 12;
    config->major_ticks.width = 3;
    config->major_ticks.color = lv_color_hex(0xD4DAE4);
    config->major_ticks.opacity = LV_OPA_COVER;
    config->minor_ticks.count = 60;
    config->minor_ticks.length = 5;
    config->minor_ticks.width = 1;
    config->minor_ticks.color = lv_color_hex(0x394554);
    config->minor_ticks.opacity = LV_OPA_COVER;
    config->center_cap.enabled = true;
    config->center_cap.radius = 5;
    config->center_cap.color = lv_color_hex(0xFF4040);
    config->center_cap.opacity = LV_OPA_COVER;
    config->numerals = true;
    config->smooth_second = true;

    config->hands[EOS_WW_ANALOG_HAND_HOUR] = (eos_ww_analog_hand_config_t){
        .enabled = true,
        .type = EOS_WW_ANALOG_HAND_BAR,
        .length = 62,
        .width = 6,
        .color = lv_color_hex(0xE4E8F0),
        .opacity = LV_OPA_COVER,
        .pivot_x = 0.5f,
        .pivot_y = 0.85f,
    };
    config->hands[EOS_WW_ANALOG_HAND_MINUTE] = (eos_ww_analog_hand_config_t){
        .enabled = true,
        .type = EOS_WW_ANALOG_HAND_BAR,
        .length = 88,
        .width = 4,
        .color = lv_color_hex(0xC0C8D4),
        .opacity = LV_OPA_COVER,
        .pivot_x = 0.5f,
        .pivot_y = 0.85f,
    };
    config->hands[EOS_WW_ANALOG_HAND_SECOND] = (eos_ww_analog_hand_config_t){
        .enabled = true,
        .type = EOS_WW_ANALOG_HAND_LINE,
        .length = 96,
        .width = 2,
        .color = lv_color_hex(0xFF3B3B),
        .opacity = LV_OPA_COVER,
        .pivot_x = 0.5f,
        .pivot_y = 0.85f,
    };
}

static bool _analog_coord(jerry_value_t value, lv_coord_t *out)
{
    double number;
    if (!_number(value, &number) || number < (double)LV_COORD_MIN || number > (double)LV_COORD_MAX)
        return false;
    *out = (lv_coord_t)number;
    return true;
}

static lv_opa_t _analog_opacity(double value)
{
    if (value <= 0.0)
        return LV_OPA_TRANSP;
    if (value >= 255.0)
        return LV_OPA_COVER;
    return (lv_opa_t)value;
}

static bool _analog_color(double value, uint32_t *out)
{
    if (!out || value < 0.0 || value > 4294967295.0)
        return false;
    *out = (uint32_t)value;
    return true;
}

static bool _analog_hand_type(jerry_value_t value, eos_ww_analog_hand_type_t *type)
{
    char name[16];
    if (!_string(value, name, sizeof(name)) || !type)
        return false;
    if (strcmp(name, "line") == 0)
        *type = EOS_WW_ANALOG_HAND_LINE;
    else if (strcmp(name, "bar") == 0)
        *type = EOS_WW_ANALOG_HAND_BAR;
    else if (strcmp(name, "needle") == 0)
        *type = EOS_WW_ANALOG_HAND_NEEDLE;
    else if (strcmp(name, "diamond") == 0)
        *type = EOS_WW_ANALOG_HAND_DIAMOND;
    else if (strcmp(name, "arrow") == 0)
        *type = EOS_WW_ANALOG_HAND_ARROW;
    else if (strcmp(name, "image") == 0)
        *type = EOS_WW_ANALOG_HAND_IMAGE;
    else
        return false;
    return true;
}

static void _parse_analog_hand(jerry_value_t object, eos_ww_analog_hand_config_t *config)
{
    jerry_value_t child;
    double value;
    uint32_t color;
    bool flag;
    eos_ww_analog_hand_type_t type;

    if (_read_bool(object, "enabled", &flag))
        config->enabled = flag;
    child = jerry_object_get_sz(object, "type");
    if (_analog_hand_type(child, &type))
        config->type = type;
    jerry_value_free(child);
    if (_read_number(object, "length", &value))
        config->length = (lv_coord_t)value;
    if (_read_number(object, "width", &value))
        config->width = (lv_coord_t)value;
    if (_read_number(object, "tailLength", &value))
        config->tail_length = (lv_coord_t)value;
    if (_read_number(object, "color", &value) && _analog_color(value, &color))
        config->color = lv_color_hex(color);
    if (_read_number(object, "opacity", &value))
        config->opacity = _analog_opacity(value);
    if (_read_number(object, "angleOffset", &value))
        config->angle_offset = (int32_t)value;
    child = _read_object(object, "offset");
    if (!jerry_value_is_undefined(child))
    {
        if (_read_number(child, "x", &value))
            config->offset_x = (lv_coord_t)value;
        if (_read_number(child, "y", &value))
            config->offset_y = (lv_coord_t)value;
        jerry_value_free(child);
    }
    child = _read_object(object, "pivot");
    if (!jerry_value_is_undefined(child))
    {
        double x;
        double y;
        if (_read_number(child, "x", &x) && _read_number(child, "y", &y) && x >= 0.0 && x <= 1.0 && y >= 0.0
            && y <= 1.0)
        {
            config->pivot_x = (float)x;
            config->pivot_y = (float)y;
            config->pivot_set = true;
        }
        jerry_value_free(child);
    }
    if (_read_string(object, "src", config->image_src, sizeof(config->image_src)))
        config->type = EOS_WW_ANALOG_HAND_IMAGE;
}

static void _parse_analog(jerry_value_t options, eos_ww_analog_dial_config_t *config)
{
    jerry_value_t child;
    double value;
    uint32_t color;
    bool flag;

    child = _read_object(options, "ticks");
    if (!jerry_value_is_undefined(child))
    {
        /* Legacy flat tick fields are read first. The nested schema wins. */
        if (_read_number(child, "majorCount", &value))
            config->major_ticks.count = (uint16_t)value;
        if (_read_number(child, "minorCount", &value))
            config->minor_ticks.count = (uint16_t)value;
        if (_read_number(child, "majorLength", &value))
            config->major_ticks.length = (lv_coord_t)value;
        if (_read_number(child, "minorLength", &value))
            config->minor_ticks.length = (lv_coord_t)value;
        if (_read_number(child, "majorWidth", &value))
            config->major_ticks.width = (lv_coord_t)value;
        if (_read_number(child, "minorWidth", &value))
            config->minor_ticks.width = (lv_coord_t)value;
        if (_read_number(child, "majorColor", &value) && _analog_color(value, &color))
            config->major_ticks.color = lv_color_hex(color);
        if (_read_number(child, "minorColor", &value) && _analog_color(value, &color))
            config->minor_ticks.color = lv_color_hex(color);
        jerry_value_t major = _read_object(child, "major");
        if (!jerry_value_is_undefined(major))
        {
            if (_read_number(major, "count", &value))
                config->major_ticks.count = (uint16_t)value;
            if (_read_number(major, "length", &value))
                config->major_ticks.length = (lv_coord_t)value;
            if (_read_number(major, "width", &value))
                config->major_ticks.width = (lv_coord_t)value;
            if (_read_number(major, "color", &value) && _analog_color(value, &color))
                config->major_ticks.color = lv_color_hex(color);
            if (_read_number(major, "opacity", &value))
                config->major_ticks.opacity = _analog_opacity(value);
            jerry_value_free(major);
        }
        jerry_value_t minor = _read_object(child, "minor");
        if (!jerry_value_is_undefined(minor))
        {
            if (_read_number(minor, "count", &value))
                config->minor_ticks.count = (uint16_t)value;
            if (_read_number(minor, "length", &value))
                config->minor_ticks.length = (lv_coord_t)value;
            if (_read_number(minor, "width", &value))
                config->minor_ticks.width = (lv_coord_t)value;
            if (_read_number(minor, "color", &value) && _analog_color(value, &color))
                config->minor_ticks.color = lv_color_hex(color);
            if (_read_number(minor, "opacity", &value))
                config->minor_ticks.opacity = _analog_opacity(value);
            jerry_value_free(minor);
        }
        jerry_value_free(child);
    }

    child = _read_object(options, "numerals");
    if (!jerry_value_is_undefined(child))
    {
        if (_read_bool(child, "enabled", &flag))
            config->numerals = flag;
        jerry_value_free(child);
    }

    child = _read_object(options, "hourHand");
    if (!jerry_value_is_undefined(child))
    {
        _parse_analog_hand(child, &config->hands[EOS_WW_ANALOG_HAND_HOUR]);
        jerry_value_free(child);
    }
    child = _read_object(options, "minuteHand");
    if (!jerry_value_is_undefined(child))
    {
        _parse_analog_hand(child, &config->hands[EOS_WW_ANALOG_HAND_MINUTE]);
        jerry_value_free(child);
    }
    child = _read_object(options, "secondHand");
    if (!jerry_value_is_undefined(child))
    {
        _parse_analog_hand(child, &config->hands[EOS_WW_ANALOG_HAND_SECOND]);
        jerry_value_free(child);
    }

    child = _read_object(options, "hands");
    if (!jerry_value_is_undefined(child))
    {
        const char *names[EOS_WW_ANALOG_DIAL_HAND_COUNT] = {"hour", "minute", "second"};
        for (uint32_t i = 0; i < EOS_WW_ANALOG_DIAL_HAND_COUNT; i++)
        {
            jerry_value_t hand = _read_object(child, names[i]);
            if (!jerry_value_is_undefined(hand))
            {
                _parse_analog_hand(hand, &config->hands[i]);
                jerry_value_free(hand);
            }
        }
        jerry_value_free(child);
    }

    child = _read_object(options, "centerCap");
    if (!jerry_value_is_undefined(child))
    {
        if (_read_bool(child, "enabled", &flag))
            config->center_cap.enabled = flag;
        if (_read_number(child, "radius", &value))
            config->center_cap.radius = (lv_coord_t)value;
        if (_read_number(child, "color", &value) && _analog_color(value, &color))
            config->center_cap.color = lv_color_hex(color);
        if (_read_number(child, "opacity", &value))
            config->center_cap.opacity = _analog_opacity(value);
        jerry_value_free(child);
    }
    if (_read_bool(options, "smoothSecond", &flag))
        config->smooth_second = flag;
}

static bool _hand_this(const jerry_call_info_t *call_info_p, eos_ww_analog_dial_hand_handle_t **handle)
{
    *handle = NULL;
    return sni_tb_js2c(call_info_p->this_value, SNI_H_EOS_ANALOG_HAND, (void **)handle) && *handle;
}

static jerry_value_t _hand_result(const jerry_call_info_t *call_info_p, bool ok)
{
    if (!ok)
        return sni_api_throw_error("AnalogDial hand is no longer available");
    (void)call_info_p;
    return jerry_undefined();
}

static jerry_value_t _hand_set_visible(const jerry_call_info_t *call_info_p,
                                       const jerry_value_t args_p[],
                                       const jerry_length_t args_count)
{
    eos_ww_analog_dial_hand_handle_t *handle;
    bool enabled;
    if (args_count != 1U || !_bool(args_p[0], &enabled) || !_hand_this(call_info_p, &handle))
        return sni_api_throw_error("Usage: hand.setVisible(boolean)");
    return _hand_result(call_info_p, eos_ww_analog_dial_hand_set_enabled(handle, enabled));
}

static jerry_value_t _hand_set_type(const jerry_call_info_t *call_info_p,
                                    const jerry_value_t args_p[],
                                    const jerry_length_t args_count)
{
    eos_ww_analog_dial_hand_handle_t *handle;
    eos_ww_analog_hand_type_t type;
    if (args_count != 1U || !_analog_hand_type(args_p[0], &type) || !_hand_this(call_info_p, &handle))
        return sni_api_throw_error("Usage: hand.setType(string)");
    return _hand_result(call_info_p, eos_ww_analog_dial_hand_set_type(handle, type));
}

static jerry_value_t _hand_set_color(const jerry_call_info_t *call_info_p,
                                     const jerry_value_t args_p[],
                                     const jerry_length_t args_count)
{
    eos_ww_analog_dial_hand_handle_t *handle;
    double value;
    if (args_count != 1U || !_number(args_p[0], &value) || value < 0.0 || value > 4294967295.0
        || !_hand_this(call_info_p, &handle))
        return sni_api_throw_error("Usage: hand.setColor(number)");
    return _hand_result(call_info_p, eos_ww_analog_dial_hand_set_color(handle, (uint32_t)value));
}

static jerry_value_t _hand_set_opacity(const jerry_call_info_t *call_info_p,
                                       const jerry_value_t args_p[],
                                       const jerry_length_t args_count)
{
    eos_ww_analog_dial_hand_handle_t *handle;
    double value;
    if (args_count != 1U || !_number(args_p[0], &value) || value < 0.0 || value > 255.0
        || !_hand_this(call_info_p, &handle))
        return sni_api_throw_error("Usage: hand.setOpacity(number)");
    return _hand_result(call_info_p, eos_ww_analog_dial_hand_set_opacity(handle, (uint8_t)value));
}

static jerry_value_t _hand_set_coord_property(const jerry_call_info_t *call_info_p,
                                              const jerry_value_t args_p[],
                                              const jerry_length_t args_count,
                                              const char *usage,
                                              bool length_property)
{
    eos_ww_analog_dial_hand_handle_t *handle;
    lv_coord_t coord;
    if (args_count != 1U || !_analog_coord(args_p[0], &coord) || !_hand_this(call_info_p, &handle))
        return sni_api_throw_error(usage);
    if (length_property)
        return _hand_result(call_info_p, eos_ww_analog_dial_hand_set_length(handle, coord));
    return _hand_result(call_info_p, eos_ww_analog_dial_hand_set_width(handle, coord));
}

static jerry_value_t _hand_set_length(const jerry_call_info_t *call_info_p,
                                      const jerry_value_t args_p[],
                                      const jerry_length_t args_count)
{
    return _hand_set_coord_property(call_info_p, args_p, args_count, "Usage: hand.setLength(number)", true);
}

static jerry_value_t _hand_set_width(const jerry_call_info_t *call_info_p,
                                     const jerry_value_t args_p[],
                                     const jerry_length_t args_count)
{
    return _hand_set_coord_property(call_info_p, args_p, args_count, "Usage: hand.setWidth(number)", false);
}

static jerry_value_t _hand_set_tail_length(const jerry_call_info_t *call_info_p,
                                           const jerry_value_t args_p[],
                                           const jerry_length_t args_count)
{
    eos_ww_analog_dial_hand_handle_t *handle;
    lv_coord_t coord;
    if (args_count != 1U || !_analog_coord(args_p[0], &coord) || !_hand_this(call_info_p, &handle))
        return sni_api_throw_error("Usage: hand.setTailLength(number)");
    return _hand_result(call_info_p, eos_ww_analog_dial_hand_set_tail_length(handle, coord));
}

static jerry_value_t _hand_set_angle_offset(const jerry_call_info_t *call_info_p,
                                            const jerry_value_t args_p[],
                                            const jerry_length_t args_count)
{
    eos_ww_analog_dial_hand_handle_t *handle;
    double value;
    if (args_count != 1U || !_number(args_p[0], &value) || value < (double)INT32_MIN || value > (double)INT32_MAX
        || !_hand_this(call_info_p, &handle))
        return sni_api_throw_error("Usage: hand.setAngleOffset(number)");
    return _hand_result(call_info_p, eos_ww_analog_dial_hand_set_angle_offset(handle, (int32_t)value));
}

static jerry_value_t _hand_set_offset(const jerry_call_info_t *call_info_p,
                                      const jerry_value_t args_p[],
                                      const jerry_length_t args_count)
{
    eos_ww_analog_dial_hand_handle_t *handle;
    jerry_value_t object;
    double x;
    double y;
    if (args_count != 1U || !jerry_value_is_object(args_p[0]) || !_hand_this(call_info_p, &handle))
        return sni_api_throw_error("Usage: hand.setOffset({x, y})");
    object = args_p[0];
    if (!_read_number(object, "x", &x) || !_read_number(object, "y", &y))
        return sni_api_throw_error("hand.setOffset requires numeric x and y");
    return _hand_result(call_info_p, eos_ww_analog_dial_hand_set_offset(handle, (lv_coord_t)x, (lv_coord_t)y));
}

static jerry_value_t _hand_set_pivot(const jerry_call_info_t *call_info_p,
                                     const jerry_value_t args_p[],
                                     const jerry_length_t args_count)
{
    eos_ww_analog_dial_hand_handle_t *handle;
    double x;
    double y;
    if (args_count != 1U || !jerry_value_is_object(args_p[0]) || !_hand_this(call_info_p, &handle))
        return sni_api_throw_error("Usage: hand.setPivot({x, y})");
    if (!_read_number(args_p[0], "x", &x) || !_read_number(args_p[0], "y", &y) || x < 0.0 || x > 1.0 || y < 0.0
        || y > 1.0)
        return sni_api_throw_error("hand.setPivot expects normalized x and y");
    return _hand_result(call_info_p, eos_ww_analog_dial_hand_set_pivot(handle, (float)x, (float)y));
}

static jerry_value_t _hand_set_image(const jerry_call_info_t *call_info_p,
                                     const jerry_value_t args_p[],
                                     const jerry_length_t args_count)
{
    eos_ww_analog_dial_hand_handle_t *handle;
    char src[EOS_WW_ANALOG_DIAL_IMAGE_SRC_MAX];
    const char *value = src;
    if (args_count != 1U || !_hand_this(call_info_p, &handle))
        return sni_api_throw_error("Usage: hand.setImage(string|null)");
    if (jerry_value_is_null(args_p[0]))
        value = NULL;
    else if (!_string(args_p[0], src, sizeof(src)))
        return sni_api_throw_error("hand.setImage expects a string or null");
    return _hand_result(call_info_p, eos_ww_analog_dial_hand_set_image(handle, value));
}

static bool _apply_hand_style(eos_ww_analog_dial_hand_handle_t *handle, jerry_value_t object)
{
    jerry_value_t child;
    double value;
    bool flag;
    eos_ww_analog_hand_type_t type;
    bool ok = true;

    if (_read_bool(object, "enabled", &flag))
        ok = eos_ww_analog_dial_hand_set_enabled(handle, flag) && ok;
    child = jerry_object_get_sz(object, "type");
    if (_analog_hand_type(child, &type))
        ok = eos_ww_analog_dial_hand_set_type(handle, type) && ok;
    jerry_value_free(child);
    if (_read_number(object, "color", &value) && value >= 0.0 && value <= 4294967295.0)
        ok = eos_ww_analog_dial_hand_set_color(handle, (uint32_t)value) && ok;
    if (_read_number(object, "opacity", &value) && value >= 0.0 && value <= 255.0)
        ok = eos_ww_analog_dial_hand_set_opacity(handle, (uint8_t)value) && ok;
    if (_read_number(object, "length", &value))
        ok = eos_ww_analog_dial_hand_set_length(handle, (lv_coord_t)value) && ok;
    if (_read_number(object, "width", &value))
        ok = eos_ww_analog_dial_hand_set_width(handle, (lv_coord_t)value) && ok;
    if (_read_number(object, "tailLength", &value))
        ok = eos_ww_analog_dial_hand_set_tail_length(handle, (lv_coord_t)value) && ok;
    if (_read_number(object, "angleOffset", &value))
        ok = eos_ww_analog_dial_hand_set_angle_offset(handle, (int32_t)value) && ok;

    child = _read_object(object, "offset");
    if (!jerry_value_is_undefined(child))
    {
        double x;
        double y;
        if (_read_number(child, "x", &x) && _read_number(child, "y", &y))
            ok = eos_ww_analog_dial_hand_set_offset(handle, (lv_coord_t)x, (lv_coord_t)y) && ok;
        jerry_value_free(child);
    }
    child = _read_object(object, "pivot");
    if (!jerry_value_is_undefined(child))
    {
        double x;
        double y;
        if (_read_number(child, "x", &x) && _read_number(child, "y", &y) && x >= 0.0 && x <= 1.0 && y >= 0.0
            && y <= 1.0)
            ok = eos_ww_analog_dial_hand_set_pivot(handle, (float)x, (float)y) && ok;
        jerry_value_free(child);
    }
    {
        char src[EOS_WW_ANALOG_DIAL_IMAGE_SRC_MAX];
        if (_read_string(object, "src", src, sizeof(src)))
            ok = eos_ww_analog_dial_hand_set_image(handle, src) && ok;
    }
    return ok;
}

static jerry_value_t _hand_set_style(const jerry_call_info_t *call_info_p,
                                     const jerry_value_t args_p[],
                                     const jerry_length_t args_count)
{
    eos_ww_analog_dial_hand_handle_t *handle;
    if (args_count != 1U || !jerry_value_is_object(args_p[0]) || !_hand_this(call_info_p, &handle))
        return sni_api_throw_error("Usage: hand.setStyle(object)");
    return _hand_result(call_info_p, _apply_hand_style(handle, args_p[0]));
}

static bool _add_hand_method(jerry_value_t object, const char *name, jerry_external_handler_t handler)
{
    jerry_value_t function = jerry_function_external(handler);
    jerry_value_t result = jerry_object_set_sz(object, name, function);
    bool ok = !jerry_value_is_exception(result);
    jerry_value_free(result);
    jerry_value_free(function);
    return ok;
}

static void _attach_hand_methods(jerry_value_t hand)
{
    (void)_add_hand_method(hand, "setVisible", _hand_set_visible);
    (void)_add_hand_method(hand, "setEnabled", _hand_set_visible);
    (void)_add_hand_method(hand, "setType", _hand_set_type);
    (void)_add_hand_method(hand, "setColor", _hand_set_color);
    (void)_add_hand_method(hand, "setOpacity", _hand_set_opacity);
    (void)_add_hand_method(hand, "setLength", _hand_set_length);
    (void)_add_hand_method(hand, "setWidth", _hand_set_width);
    (void)_add_hand_method(hand, "setTailLength", _hand_set_tail_length);
    (void)_add_hand_method(hand, "setAngleOffset", _hand_set_angle_offset);
    (void)_add_hand_method(hand, "setOffset", _hand_set_offset);
    (void)_add_hand_method(hand, "setPivot", _hand_set_pivot);
    (void)_add_hand_method(hand, "setImage", _hand_set_image);
    (void)_add_hand_method(hand, "setStyle", _hand_set_style);
}

static void _attach_hand_handles(jerry_value_t dial, lv_obj_t *object)
{
    static const char *const names[EOS_WW_ANALOG_DIAL_HAND_COUNT] = {"hourHand", "minuteHand", "secondHand"};
    for (uint32_t i = 0; i < EOS_WW_ANALOG_DIAL_HAND_COUNT; i++)
    {
        eos_ww_analog_dial_hand_handle_t *handle = eos_ww_analog_dial_get_hand(object, (eos_ww_analog_hand_index_t)i);
        jerry_value_t hand;
        if (!handle)
            continue;
        hand = sni_tb_c2js(&handle, SNI_H_EOS_ANALOG_HAND);
        if (jerry_value_is_undefined(hand) || jerry_value_is_exception(hand))
        {
            jerry_value_free(hand);
            continue;
        }
        sni_tb_link_sub_resource(object, handle, SNI_H_EOS_ANALOG_HAND);
        _attach_hand_methods(hand);
        jerry_value_free(jerry_object_set_sz(dial, names[i], hand));
        jerry_value_free(hand);
    }
}

jerry_value_t sni_api_eos_ww_analog_dial(const jerry_call_info_t *call_info_p,
                                         const jerry_value_t args_p[],
                                         const jerry_length_t args_count)
{
    eos_ww_analog_dial_config_t config;
    lv_obj_t *parent;
    lv_obj_t *object;
    jerry_value_t dial;
    double value;
    (void)call_info_p;
    _analog_config_defaults(&config);
    if (args_count != 2U || !jerry_value_is_object(args_p[1]) || !_get_parent(args_p, &parent))
        return sni_api_throw_error("Usage: ww.analogDial(parent, options)");
    if (_read_number(args_p[1], "width", &value))
        config.width = (lv_coord_t)value;
    if (_read_number(args_p[1], "height", &value))
        config.height = (lv_coord_t)value;
    _parse_analog(args_p[1], &config);
    object = eos_ww_analog_dial_create(parent, &config);
    _apply_position(args_p[1], object);
    dial = _return_widget(object);
    if (jerry_value_is_exception(dial) || jerry_value_is_undefined(dial))
        return dial;
    _attach_hand_handles(dial, object);
    return dial;
}

jerry_value_t sni_api_eos_ww_compass_dial(const jerry_call_info_t *call_info_p,
                                          const jerry_value_t args_p[],
                                          const jerry_length_t args_count)
{
    eos_ww_compass_dial_config_t config = {
        .width = 200,
        .height = 200,
        .tick_step = 5,
        .major_tick_step = 30,
        .cardinal_labels = true,
        .degree_labels = false,
        .show_heading = true,
        .show_calibration_state = true,
        .smoothing = 0.15f,
    };
    lv_obj_t *parent;
    double value;
    bool flag;
    (void)call_info_p;
    if (args_count != 2U || !jerry_value_is_object(args_p[1]) || !_get_parent(args_p, &parent))
        return sni_api_throw_error("Usage: ww.compassDial(parent, options)");
    if (_read_number(args_p[1], "width", &value))
        config.width = (lv_coord_t)value;
    if (_read_number(args_p[1], "height", &value))
        config.height = (lv_coord_t)value;
    if (_read_number(args_p[1], "tickStep", &value))
        config.tick_step = (uint16_t)value;
    if (_read_number(args_p[1], "majorTickStep", &value))
        config.major_tick_step = (uint16_t)value;
    if (_read_number(args_p[1], "smoothing", &value))
        config.smoothing = (float)value;
    if (_read_bool(args_p[1], "cardinalLabels", &flag))
        config.cardinal_labels = flag;
    if (_read_bool(args_p[1], "degreeLabels", &flag))
        config.degree_labels = flag;
    if (_read_bool(args_p[1], "showHeading", &flag))
        config.show_heading = flag;
    if (_read_bool(args_p[1], "showCalibrationState", &flag))
        config.show_calibration_state = flag;
    lv_obj_t *object = eos_ww_compass_dial_create(parent, &config);
    _apply_position(args_p[1], object);
    return _return_widget(object);
}

static void _parse_radial(jerry_value_t options, eos_ww_radial_gauge_config_t *config)
{
    double value;
    bool flag;
    jerry_value_t ranges = jerry_object_get_sz(options, "ranges");
    if (_read_number(options, "width", &value))
        config->width = (lv_coord_t)value;
    if (_read_number(options, "height", &value))
        config->height = (lv_coord_t)value;
    if (_read_number(options, "min", &value))
        config->min = value;
    if (_read_number(options, "max", &value))
        config->max = value;
    if (_read_number(options, "startAngle", &value))
        config->start_angle = (int32_t)value;
    if (_read_number(options, "angleRange", &value))
        config->angle_range = (uint32_t)value;
    if (_read_number(options, "majorStep", &value))
        config->major_step = (uint32_t)value;
    if (_read_number(options, "minorStep", &value))
        config->minor_step = (uint32_t)value;
    if (_read_bool(options, "needle", &flag))
        config->needle = flag;
    if (_read_bool(options, "valueLabel", &flag))
        config->value_label = flag;
    if (_parse_metric(options, "metric", &config->metric_id))
        config->metric_bound = true;
    if (jerry_value_is_array(ranges))
    {
        jerry_length_t length = jerry_array_length(ranges);
        for (jerry_length_t i = 0; i < length && config->range_count < EOS_WW_RADIAL_GAUGE_MAX_RANGES; i++)
        {
            jerry_value_t item = jerry_object_get_index(ranges, i);
            if (jerry_value_is_object(item) && _read_number(item, "from", &value))
            {
                eos_ww_radial_gauge_range_t *range = &config->ranges[config->range_count];
                range->from = value;
                range->to = config->max;
                range->color = _DEFAULT_COLOR;
                if (_read_number(item, "to", &value))
                    range->to = value;
                if (_read_number(item, "color", &value))
                    range->color = (uint32_t)value;
                config->range_count++;
            }
            jerry_value_free(item);
        }
    }
    jerry_value_free(ranges);
}

jerry_value_t sni_api_eos_ww_radial_gauge(const jerry_call_info_t *call_info_p,
                                          const jerry_value_t args_p[],
                                          const jerry_length_t args_count)
{
    eos_ww_radial_gauge_config_t config = {
        .width = 180,
        .height = 180,
        .min = 0,
        .max = 100,
        .start_angle = 135,
        .angle_range = 270,
        .major_step = 10,
        .minor_step = 2,
        .needle = true,
        .value_label = true,
    };
    lv_obj_t *parent;
    (void)call_info_p;
    if (args_count != 2U || !jerry_value_is_object(args_p[1]) || !_get_parent(args_p, &parent))
        return sni_api_throw_error("Usage: ww.radialGauge(parent, options)");
    _parse_radial(args_p[1], &config);
    lv_obj_t *object = eos_ww_radial_gauge_create(parent, &config);
    _apply_position(args_p[1], object);
    return _return_widget(object);
}

jerry_value_t sni_api_eos_ww_radial_gauge_set_value(const jerry_call_info_t *call_info_p,
                                                    const jerry_value_t args_p[],
                                                    const jerry_length_t args_count)
{
    lv_obj_t *gauge;
    double value;
    bool valid = true;
    (void)call_info_p;
    if ((args_count != 2U && args_count != 3U) || !sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &gauge)
        || !_number(args_p[1], &value) || (args_count == 3U && !_bool(args_p[2], &valid)))
        return sni_api_throw_error("Usage: ww.radialGaugeSetValue(gauge, value, valid?)");
    eos_ww_radial_gauge_set_value(gauge, value, valid);
    return jerry_undefined();
}

static void _parse_rings(jerry_value_t options, eos_ww_metric_rings_config_t *config)
{
    double value;
    bool flag;
    jerry_value_t list = jerry_object_get_sz(options, "rings");
    if (_read_number(options, "size", &value))
        config->size = (lv_coord_t)value;
    if (_read_number(options, "ringWidth", &value))
        config->ring_width = (lv_coord_t)value;
    if (_read_number(options, "gap", &value))
        config->gap = (lv_coord_t)value;
    if (_read_number(options, "trackColor", &value))
        config->track_color = (uint32_t)value;
    if (_read_bool(options, "animate", &flag))
        config->animate = flag;
    if (jerry_value_is_array(list))
    {
        jerry_length_t length = jerry_array_length(list);
        for (jerry_length_t i = 0; i < length && config->ring_count < EOS_WW_METRIC_RINGS_MAX; i++)
        {
            jerry_value_t item = jerry_object_get_index(list, i);
            if (jerry_value_is_object(item))
            {
                eos_ww_metric_ring_spec_t *ring = &config->rings[config->ring_count];
                ring->max = 100;
                ring->color = _DEFAULT_COLOR;
                if (_read_number(item, "min", &value))
                    ring->min = value;
                if (_read_number(item, "max", &value))
                    ring->max = value;
                if (_read_number(item, "color", &value))
                    ring->color = (uint32_t)value;
                if (_parse_metric(item, "metric", &ring->metric_id))
                    ring->metric_bound = true;
                config->ring_count++;
            }
            jerry_value_free(item);
        }
    }
    jerry_value_free(list);
}

jerry_value_t sni_api_eos_ww_metric_rings(const jerry_call_info_t *call_info_p,
                                          const jerry_value_t args_p[],
                                          const jerry_length_t args_count)
{
    eos_ww_metric_rings_config_t config = {
        .size = 120,
        .ring_width = 6,
        .gap = 4,
        .track_color = 0x1A2030,
        .animate = true,
    };
    lv_obj_t *parent;
    (void)call_info_p;
    if (args_count != 2U || !jerry_value_is_object(args_p[1]) || !_get_parent(args_p, &parent))
        return sni_api_throw_error("Usage: ww.metricRings(parent, options)");
    _parse_rings(args_p[1], &config);
    if (config.ring_count == 0U)
        return sni_api_throw_error("Metric rings requires a rings array");
    lv_obj_t *object = eos_ww_metric_rings_create(parent, &config);
    _apply_position(args_p[1], object);
    return _return_widget(object);
}

jerry_value_t sni_api_eos_ww_metric_rings_set_value(const jerry_call_info_t *call_info_p,
                                                    const jerry_value_t args_p[],
                                                    const jerry_length_t args_count)
{
    lv_obj_t *rings;
    double index;
    double value;
    bool valid = true;
    (void)call_info_p;
    if ((args_count != 3U && args_count != 4U) || !sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &rings)
        || !_number(args_p[1], &index) || !_number(args_p[2], &value)
        || (args_count == 4U && !_bool(args_p[3], &valid)))
        return sni_api_throw_error("Usage: ww.metricRingsSetValue(rings, index, value, valid?)");
    eos_ww_metric_rings_set_value(rings, (uint8_t)index, value, valid);
    return jerry_undefined();
}

jerry_value_t sni_api_eos_ww_trend_chart(const jerry_call_info_t *call_info_p,
                                         const jerry_value_t args_p[],
                                         const jerry_length_t args_count)
{
    eos_ww_trend_chart_config_t config = {
        .width = 180,
        .height = 60,
        .capacity = 60,
        .sample_interval = 1000,
        .auto_scale = true,
        .min_value = 0,
        .max_value = 100,
    };
    lv_obj_t *parent;
    double value;
    bool flag;
    (void)call_info_p;
    if (args_count != 2U || !jerry_value_is_object(args_p[1]) || !_get_parent(args_p, &parent))
        return sni_api_throw_error("Usage: ww.trendChart(parent, options)");
    if (_read_number(args_p[1], "width", &value))
        config.width = (lv_coord_t)value;
    if (_read_number(args_p[1], "height", &value))
        config.height = (lv_coord_t)value;
    if (_read_number(args_p[1], "capacity", &value))
        config.capacity = (uint16_t)value;
    if (_read_number(args_p[1], "sampleInterval", &value))
        config.sample_interval = (uint32_t)value;
    if (_read_number(args_p[1], "min", &value))
        config.min_value = (int32_t)value;
    if (_read_number(args_p[1], "max", &value))
        config.max_value = (int32_t)value;
    if (_read_bool(args_p[1], "autoScale", &flag))
        config.auto_scale = flag;
    if (_parse_metric(args_p[1], "metric", &config.metric_id))
        config.metric_bound = true;
    lv_obj_t *object = eos_ww_trend_chart_create(parent, &config);
    _apply_position(args_p[1], object);
    return _return_widget(object);
}

jerry_value_t sni_api_eos_ww_trend_chart_append(const jerry_call_info_t *call_info_p,
                                                const jerry_value_t args_p[],
                                                const jerry_length_t args_count)
{
    lv_obj_t *chart;
    double value;
    bool valid = true;
    (void)call_info_p;
    if ((args_count != 2U && args_count != 3U) || !sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &chart)
        || !_number(args_p[1], &value) || (args_count == 3U && !_bool(args_p[2], &valid)))
        return sni_api_throw_error("Usage: ww.trendChartAppend(chart, value, valid?)");
    eos_ww_trend_chart_append(chart, value, valid);
    return jerry_undefined();
}

jerry_value_t sni_api_eos_ww_trend_chart_set_data(const jerry_call_info_t *call_info_p,
                                                  const jerry_value_t args_p[],
                                                  const jerry_length_t args_count)
{
    lv_obj_t *chart;
    jerry_length_t length;
    double *values;
    bool ok = true;
    (void)call_info_p;
    if (args_count != 2U || !sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &chart) || !jerry_value_is_array(args_p[1]))
        return sni_api_throw_error("Usage: ww.trendChartSetData(chart, values)");
    length = jerry_array_length(args_p[1]);
    if (length > UINT16_MAX)
        return sni_api_throw_error("Too many trend samples");
    values = eos_malloc(sizeof(double) * (length == 0U ? 1U : length));
    if (!values)
        return sni_api_throw_error("Out of memory");
    for (jerry_length_t i = 0; i < length; i++)
    {
        jerry_value_t item = jerry_object_get_index(args_p[1], i);
        if (!_number(item, &values[i]))
            ok = false;
        jerry_value_free(item);
    }
    if (ok)
        ok = eos_ww_trend_chart_set_data(chart, values, (uint16_t)length);
    eos_free(values);
    if (!ok)
        return sni_api_throw_error("Invalid trend data");
    return jerry_undefined();
}

jerry_value_t sni_api_eos_ww_trend_chart_clear(const jerry_call_info_t *call_info_p,
                                               const jerry_value_t args_p[],
                                               const jerry_length_t args_count)
{
    lv_obj_t *chart;
    (void)call_info_p;
    if (args_count != 1U || !sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &chart))
        return sni_api_throw_error("Usage: ww.trendChartClear(chart)");
    eos_ww_trend_chart_clear(chart);
    return jerry_undefined();
}

jerry_value_t sni_api_eos_ww_calendar_grid(const jerry_call_info_t *call_info_p,
                                           const jerry_value_t args_p[],
                                           const jerry_length_t args_count)
{
    eos_ww_calendar_grid_config_t config = {
        .width = 0,
        .height = 0,
        .show_header = true,
        .show_weekdays = true,
        .highlight_today = true,
        .week_starts_monday = false,
    };
    lv_obj_t *parent;
    double value;
    bool flag;
    char week_start[16];
    (void)call_info_p;
    if (args_count != 2U || !jerry_value_is_object(args_p[1]) || !_get_parent(args_p, &parent))
        return sni_api_throw_error("Usage: ww.calendarGrid(parent, options)");
    if (_read_number(args_p[1], "year", &value))
        config.year = (uint16_t)value;
    if (_read_number(args_p[1], "month", &value))
        config.month = (uint8_t)value;
    if (_read_number(args_p[1], "width", &value))
        config.width = (lv_coord_t)value;
    if (_read_number(args_p[1], "height", &value))
        config.height = (lv_coord_t)value;
    if (_read_bool(args_p[1], "showHeader", &flag))
        config.show_header = flag;
    if (_read_bool(args_p[1], "showWeekdays", &flag))
        config.show_weekdays = flag;
    if (_read_bool(args_p[1], "highlightToday", &flag))
        config.highlight_today = flag;
    if (_read_string(args_p[1], "weekStartsOn", week_start, sizeof(week_start)))
        config.week_starts_monday = strcmp(week_start, "monday") == 0;
    lv_obj_t *object = eos_ww_calendar_grid_create(parent, &config);
    _apply_position(args_p[1], object);
    return _return_widget(object);
}

jerry_value_t sni_api_eos_ww_calendar_grid_set_month(const jerry_call_info_t *call_info_p,
                                                     const jerry_value_t args_p[],
                                                     const jerry_length_t args_count)
{
    lv_obj_t *calendar;
    double year;
    double month;
    (void)call_info_p;
    if (args_count != 3U || !sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &calendar) || !_number(args_p[1], &year)
        || !_number(args_p[2], &month))
        return sni_api_throw_error("Usage: ww.calendarGridSetMonth(calendar, year, month)");
    eos_ww_calendar_grid_set_month(calendar, (uint16_t)year, (uint8_t)month);
    return jerry_undefined();
}

jerry_value_t sni_api_eos_ww_calendar_grid_set_selected_date(const jerry_call_info_t *call_info_p,
                                                             const jerry_value_t args_p[],
                                                             const jerry_length_t args_count)
{
    lv_obj_t *calendar;
    double year;
    double month;
    double day;
    (void)call_info_p;
    if (args_count != 4U || !sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &calendar) || !_number(args_p[1], &year)
        || !_number(args_p[2], &month) || !_number(args_p[3], &day))
        return sni_api_throw_error("Usage: ww.calendarGridSetSelectedDate(calendar, year, month, day)");
    eos_ww_calendar_grid_set_selected_date(calendar, (uint16_t)year, (uint8_t)month, (uint8_t)day);
    return jerry_undefined();
}

const sni_method_desc_t eos_ww_static_methods[] = {
    {.name = "analogDial", .handler = sni_api_eos_ww_analog_dial},
    {.name = "compassDial", .handler = sni_api_eos_ww_compass_dial},
    {.name = "radialGauge", .handler = sni_api_eos_ww_radial_gauge},
    {.name = "radialGaugeSetValue", .handler = sni_api_eos_ww_radial_gauge_set_value},
    {.name = "metricRings", .handler = sni_api_eos_ww_metric_rings},
    {.name = "metricRingsSetValue", .handler = sni_api_eos_ww_metric_rings_set_value},
    {.name = "trendChart", .handler = sni_api_eos_ww_trend_chart},
    {.name = "trendChartAppend", .handler = sni_api_eos_ww_trend_chart_append},
    {.name = "trendChartSetData", .handler = sni_api_eos_ww_trend_chart_set_data},
    {.name = "trendChartClear", .handler = sni_api_eos_ww_trend_chart_clear},
    {.name = "calendarGrid", .handler = sni_api_eos_ww_calendar_grid},
    {.name = "calendarGridSetMonth", .handler = sni_api_eos_ww_calendar_grid_set_month},
    {.name = "calendarGridSetSelectedDate", .handler = sni_api_eos_ww_calendar_grid_set_selected_date},
    {.name = NULL, .handler = NULL},
};
