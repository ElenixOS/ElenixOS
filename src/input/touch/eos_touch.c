/**
 * @file eos_touch.c
 * @brief Touch input device configuration
 */

#include "eos_touch.h"

/* Includes ---------------------------------------------------*/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define EOS_LOG_TAG "Touch"
#include "eos_config.h"
#include "eos_log.h"
#include "eos_port_critical.h"
#include "eos_overlay_layer.h"

/* Macros and Definitions -------------------------------------*/

/**
 * @brief Scroll limit in pixels.
 *
 * Movement below this threshold is treated as a click (not a drag/scroll).
 * LVGL default is 10px; reduced for tighter click detection on small screens.
 */
#define EOS_TOUCH_SCROLL_LIMIT 5

/**
 * @brief Long press time in milliseconds.
 *
 * Time to press before LV_EVENT_LONG_PRESSED is sent.
 * LVGL default is 400ms.
 */
#define EOS_TOUCH_LONG_PRESS_TIME 400
#define EOS_TOUCH_INJECT_QUEUE_SIZE 64U
#define EOS_TOUCH_TRACE_MAX_POINTS 32U
#define EOS_TOUCH_TRACE_LINE_WIDTH 14U
#define EOS_TOUCH_TRACE_START_SIZE 48U
#define EOS_TOUCH_MARKER_SIZE 64U
#define EOS_TOUCH_MARKER_CORE_SIZE 20U
#define EOS_TOUCH_INDEV_PERIOD_MS 33U
#define EOS_TOUCH_CONTROL_GLOW_INSET 4U
#define EOS_TOUCH_CONTROL_BANNER_WIDTH (EOS_DISPLAY_WIDTH - 80U)
#define EOS_TOUCH_CONTROL_BANNER_HEIGHT 30U
#define EOS_TOUCH_CONTROL_BANNER_BOTTOM_MARGIN 28U

/* Variables --------------------------------------------------*/

/* Synthetic samples are owned by the EOS/LVGL execution context.  Physical
 * samples are submitted by the platform adapter and copied under the EOS
 * critical-section abstraction.  No FreeRTOS type or platform lock leaks into
 * this broker. */
typedef struct
{
    int32_t x;
    int32_t y;
    lv_indev_state_t state;
    uint16_t repeat;
} _eos_touch_inject_sample_t;

static _eos_touch_inject_sample_t _eos_touch_inject_queue[EOS_TOUCH_INJECT_QUEUE_SIZE];
static uint8_t _eos_touch_inject_head;
static uint8_t _eos_touch_inject_tail;
static uint8_t _eos_touch_inject_count;
static _eos_touch_inject_sample_t _eos_touch_inject_current;
static uint16_t _eos_touch_inject_current_repeat;
static uint8_t _eos_touch_inject_active;
static uint8_t _eos_touch_inject_release_queued;
static uint8_t _eos_touch_inject_pressed_delivered;
static uint8_t _eos_touch_inject_abort_release_pending;
static uint8_t _eos_touch_ready;
static lv_obj_t *_eos_touch_marker;
static lv_obj_t *_eos_touch_marker_core;
static lv_obj_t *_eos_touch_trace_start;
static lv_obj_t *_eos_touch_trace_line;
static lv_draw_buf_t *_eos_touch_trace_draw_buf;
static lv_point_precise_t _eos_touch_trace_points[EOS_TOUCH_TRACE_MAX_POINTS];
static uint8_t _eos_touch_trace_point_count;
static lv_obj_t *_eos_touch_control_glow;
static lv_obj_t *_eos_touch_control_banner;
static lv_obj_t *_eos_touch_control_label;
static eos_touch_control_state_t _eos_touch_control_state = EOS_TOUCH_CONTROL_UNCONTROLLED;
static uint8_t _eos_touch_physical_suppress;
static int32_t _eos_touch_physical_x;
static int32_t _eos_touch_physical_y;
static lv_indev_state_t _eos_touch_physical_state = LV_INDEV_STATE_REL;
static uint8_t _eos_touch_physical_valid;
static lv_indev_t *_eos_touch_bound_indev;
static lv_indev_read_cb_t _eos_touch_platform_read_cb;

/* Function Implementations -----------------------------------*/

static void _eos_touch_marker_hide(void);
static void _eos_touch_trace_reset(void);
static void _eos_touch_trace_update(void);
static void _eos_touch_trace_add(int32_t x, int32_t y);
static void _eos_touch_trace_render(void);
static void _eos_touch_trace_canvas_delete_cb(lv_event_t *event);
static bool _eos_touch_coordinate_valid(int32_t x, int32_t y);
static void _eos_touch_bound_read_cb(lv_indev_t *indev, lv_indev_data_t *data);
static void _eos_touch_bound_indev_delete_cb(lv_event_t *event);

static void _eos_touch_bound_read_cb(lv_indev_t *indev, lv_indev_data_t *data)
{
    lv_indev_data_t physical_data = {0};

    if (_eos_touch_platform_read_cb != NULL)
    {
        _eos_touch_platform_read_cb(indev, &physical_data);
        (void)eos_touch_submit_physical(physical_data.point.x,
                                        physical_data.point.y,
                                        physical_data.state == LV_INDEV_STATE_PRESSED);
    }

    (void)eos_touch_read(data);
}

static void _eos_touch_bound_indev_delete_cb(lv_event_t *event)
{
    lv_indev_t *indev = (lv_indev_t *)lv_event_get_user_data(event);

    if (indev == _eos_touch_bound_indev)
    {
        _eos_touch_bound_indev = NULL;
        _eos_touch_platform_read_cb = NULL;
        EOS_LOG_W("Touch input binding was removed with its LVGL indev; ui command is unavailable");
    }
}

bool eos_touch_bind_indev(lv_indev_t *indev)
{
    lv_indev_read_cb_t read_cb;

    if ((indev == NULL) || (lv_indev_get_type(indev) != LV_INDEV_TYPE_POINTER))
    {
        EOS_LOG_E("Touch input bind failed: platform must provide a pointer LVGL indev");
        return false;
    }

    if (_eos_touch_bound_indev == indev && lv_indev_get_read_cb(indev) == _eos_touch_bound_read_cb)
    {
        return true;
    }

    if (_eos_touch_bound_indev != NULL)
    {
        EOS_LOG_E("Touch input bind failed: another LVGL indev is already bound");
        return false;
    }

    read_cb = lv_indev_get_read_cb(indev);
    if (read_cb == NULL)
    {
        EOS_LOG_E("Touch input bind failed: platform LVGL indev has no read callback");
        return false;
    }

    _eos_touch_platform_read_cb = read_cb;
    _eos_touch_bound_indev = indev;
    lv_indev_set_read_cb(indev, _eos_touch_bound_read_cb);
    lv_indev_add_event_cb(indev, _eos_touch_bound_indev_delete_cb, LV_EVENT_DELETE, indev);
    EOS_LOG_I("Touch input bound to LVGL pointer indev; ui command is available");
    return true;
}

bool eos_touch_is_bound(void)
{
    return (_eos_touch_bound_indev != NULL)
           && (lv_indev_get_read_cb(_eos_touch_bound_indev) == _eos_touch_bound_read_cb);
}

static bool _eos_touch_control_glow_init(void)
{
    /* Keep the control affordance below the synthetic touch marker. */
    _eos_touch_control_glow = lv_obj_create(eos_overlay_layer_get(EOS_TOP_LAYER_SYSTEM_TOUCH_STATUS));
    if (_eos_touch_control_glow == NULL)
    {
        EOS_LOG_E("Failed to create control glow object");
        return false;
    }

    lv_obj_remove_style_all(_eos_touch_control_glow);
    lv_obj_set_size(_eos_touch_control_glow,
                    EOS_DISPLAY_WIDTH - 2U * EOS_TOUCH_CONTROL_GLOW_INSET,
                    EOS_DISPLAY_HEIGHT - 2U * EOS_TOUCH_CONTROL_GLOW_INSET);
    lv_obj_set_pos(_eos_touch_control_glow, EOS_TOUCH_CONTROL_GLOW_INSET, EOS_TOUCH_CONTROL_GLOW_INSET);
    lv_obj_set_style_radius(_eos_touch_control_glow,
                            LV_MAX((int32_t)EOS_DISPLAY_RADIUS - (int32_t)EOS_TOUCH_CONTROL_GLOW_INSET, 0),
                            0);
    lv_obj_set_style_bg_opa(_eos_touch_control_glow, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(_eos_touch_control_glow, 6, 0);
    lv_obj_set_style_border_color(_eos_touch_control_glow, lv_color_hex(0x686CFF), 0);
    lv_obj_remove_flag(_eos_touch_control_glow, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(_eos_touch_control_glow, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_obj_add_flag(_eos_touch_control_glow, LV_OBJ_FLAG_HIDDEN);
    return true;
}

void eos_touch_init(void)
{
    lv_indev_t *indev = lv_indev_get_next(NULL);
    while (indev)
    {
        if (lv_indev_get_type(indev) == LV_INDEV_TYPE_POINTER)
        {
            lv_indev_set_scroll_limit(indev, EOS_TOUCH_SCROLL_LIMIT);
            lv_indev_set_long_press_time(indev, EOS_TOUCH_LONG_PRESS_TIME);
            EOS_LOG_I("scroll_limit=%d, long_press_time=%d", EOS_TOUCH_SCROLL_LIMIT, EOS_TOUCH_LONG_PRESS_TIME);
        }
        indev = lv_indev_get_next(indev);
    }

    if (_eos_touch_marker == NULL)
    {
        _eos_touch_marker = lv_obj_create(eos_overlay_layer_get(EOS_TOP_LAYER_SYSTEM_TOUCH_MARKER));
        lv_obj_remove_style_all(_eos_touch_marker);
        lv_obj_set_size(_eos_touch_marker, EOS_TOUCH_MARKER_SIZE, EOS_TOUCH_MARKER_SIZE);
        lv_obj_set_style_radius(_eos_touch_marker, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_opa(_eos_touch_marker, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(_eos_touch_marker, 7, 0);
        lv_obj_set_style_border_color(_eos_touch_marker, lv_color_hex(0x2D65DE), 0);
        lv_obj_set_style_border_opa(_eos_touch_marker, LV_OPA_COVER, 0);
        lv_obj_remove_flag(_eos_touch_marker, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_flag(_eos_touch_marker, LV_OBJ_FLAG_HIDDEN);
    }

    if (_eos_touch_marker_core == NULL)
    {
        _eos_touch_marker_core = lv_obj_create(eos_overlay_layer_get(EOS_TOP_LAYER_SYSTEM_TOUCH_MARKER));
        lv_obj_remove_style_all(_eos_touch_marker_core);
        lv_obj_set_size(_eos_touch_marker_core, EOS_TOUCH_MARKER_CORE_SIZE, EOS_TOUCH_MARKER_CORE_SIZE);
        lv_obj_set_style_radius(_eos_touch_marker_core, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(_eos_touch_marker_core, lv_color_hex(0xFFFFFF), 0);
        lv_obj_set_style_bg_opa(_eos_touch_marker_core, LV_OPA_COVER, 0);
        lv_obj_remove_flag(_eos_touch_marker_core, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_flag(_eos_touch_marker_core, LV_OBJ_FLAG_HIDDEN);
    }

    if (_eos_touch_trace_start == NULL)
    {
        _eos_touch_trace_start = lv_obj_create(eos_overlay_layer_get(EOS_TOP_LAYER_SYSTEM_TOUCH_MARKER));
        lv_obj_remove_style_all(_eos_touch_trace_start);
        lv_obj_set_size(_eos_touch_trace_start, EOS_TOUCH_TRACE_START_SIZE, EOS_TOUCH_TRACE_START_SIZE);
        lv_obj_set_style_radius(_eos_touch_trace_start, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(_eos_touch_trace_start, lv_color_hex(0x78A0EC), 0);
        lv_obj_set_style_bg_opa(_eos_touch_trace_start, LV_OPA_40, 0);
        lv_obj_remove_flag(_eos_touch_trace_start, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_flag(_eos_touch_trace_start, LV_OBJ_FLAG_HIDDEN);
    }

    if (_eos_touch_trace_line == NULL)
    {
        _eos_touch_trace_line = lv_canvas_create(eos_overlay_layer_get(EOS_TOP_LAYER_SYSTEM_TOUCH_MARKER));
        if (_eos_touch_trace_line == NULL)
        {
            EOS_LOG_E("Failed to create touch trace line");
        }
        else
        {
            _eos_touch_trace_draw_buf =
                lv_draw_buf_create(EOS_DISPLAY_WIDTH, EOS_DISPLAY_HEIGHT, LV_COLOR_FORMAT_ARGB8888, LV_STRIDE_AUTO);
            if (_eos_touch_trace_draw_buf == NULL)
            {
                EOS_LOG_E("Failed to create touch trace buffer");
                lv_obj_delete(_eos_touch_trace_line);
                _eos_touch_trace_line = NULL;
            }
            else
            {
                lv_canvas_set_draw_buf(_eos_touch_trace_line, _eos_touch_trace_draw_buf);
                lv_obj_remove_style_all(_eos_touch_trace_line);
                lv_obj_set_style_bg_opa(_eos_touch_trace_line, LV_OPA_TRANSP, 0);
                lv_obj_add_event_cb(_eos_touch_trace_line, _eos_touch_trace_canvas_delete_cb, LV_EVENT_DELETE, NULL);
                lv_obj_set_size(_eos_touch_trace_line, EOS_DISPLAY_WIDTH, EOS_DISPLAY_HEIGHT);
                lv_obj_set_pos(_eos_touch_trace_line, 0, 0);
                lv_obj_remove_flag(_eos_touch_trace_line, LV_OBJ_FLAG_CLICKABLE);
                lv_obj_add_flag(_eos_touch_trace_line, LV_OBJ_FLAG_HIDDEN);
            }
        }
    }

    if (_eos_touch_trace_line != NULL && _eos_touch_trace_draw_buf != NULL)
    {
        lv_canvas_fill_bg(_eos_touch_trace_line, lv_color_black(), LV_OPA_TRANSP);
    }

    /* The canvas owns the rendered pixels; the trace is rasterized whenever
     * its control-point list changes, so the draw callback never has to
     * submit a collection of independently antialiased line segments. */
    if (_eos_touch_trace_line != NULL && _eos_touch_trace_draw_buf == NULL)
    {
        lv_obj_add_flag(_eos_touch_trace_line, LV_OBJ_FLAG_HIDDEN);
    }

    _eos_touch_trace_reset();

    if (_eos_touch_control_glow == NULL)
    {
        (void)_eos_touch_control_glow_init();
    }

    if (_eos_touch_control_banner == NULL)
    {
        /* The banner and its label belong to the system layer, below the
         * top-layer touch marker and other top-most interaction visuals. */
        _eos_touch_control_banner = lv_obj_create(eos_overlay_layer_get(EOS_TOP_LAYER_SYSTEM_TOUCH_STATUS));
        lv_obj_remove_style_all(_eos_touch_control_banner);
        lv_obj_set_size(_eos_touch_control_banner, EOS_TOUCH_CONTROL_BANNER_WIDTH, EOS_TOUCH_CONTROL_BANNER_HEIGHT);
        lv_obj_set_pos(_eos_touch_control_banner,
                       (EOS_DISPLAY_WIDTH - EOS_TOUCH_CONTROL_BANNER_WIDTH) / 2,
                       EOS_DISPLAY_HEIGHT - EOS_TOUCH_CONTROL_BANNER_BOTTOM_MARGIN - EOS_TOUCH_CONTROL_BANNER_HEIGHT);
        lv_obj_set_style_radius(_eos_touch_control_banner, 12, 0);
        lv_obj_set_style_bg_color(_eos_touch_control_banner, lv_color_hex(0x282C78), 0);
        lv_obj_set_style_bg_opa(_eos_touch_control_banner, LV_OPA_70, 0);
        lv_obj_set_style_border_width(_eos_touch_control_banner, 1, 0);
        lv_obj_set_style_border_color(_eos_touch_control_banner, lv_color_hex(0x8588FF), 0);
        lv_obj_set_style_border_opa(_eos_touch_control_banner, LV_OPA_70, 0);
        lv_obj_remove_flag(_eos_touch_control_banner, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_flag(_eos_touch_control_banner, LV_OBJ_FLAG_HIDDEN);
        _eos_touch_control_label = lv_label_create(_eos_touch_control_banner);
        lv_label_set_text(_eos_touch_control_label, "AUTO CONTROL  -  TOUCH TO TAKE OVER");
        lv_obj_set_style_text_color(_eos_touch_control_label, lv_color_hex(0xFFFFFF), 0);
        lv_obj_set_style_text_opa(_eos_touch_control_label, LV_OPA_90, 0);
        lv_obj_center(_eos_touch_control_label);
        lv_obj_remove_flag(_eos_touch_control_label, LV_OBJ_FLAG_CLICKABLE);
    }
    _eos_touch_ready = 1U;

    if (!eos_touch_is_bound())
    {
        EOS_LOG_W("Touch input is not bound to an LVGL pointer indev; platform must call eos_touch_bind_indev()"
                  "; ui command is unavailable");
    }
}

bool eos_touch_submit_physical(int32_t x, int32_t y, bool pressed)
{
    if (!_eos_touch_coordinate_valid(x, y))
    {
        return false;
    }

    eos_critical_ctx_t ctx = eos_critical_enter();
    _eos_touch_physical_x = x;
    _eos_touch_physical_y = y;
    _eos_touch_physical_state = pressed ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
    _eos_touch_physical_valid = 1U;
    eos_critical_leave(ctx);
    return true;
}

lv_indev_t *eos_touch_get_indev(void)
{
    lv_indev_t *indev = lv_indev_get_next(NULL);
    while (indev)
    {
        if (lv_indev_get_type(indev) == LV_INDEV_TYPE_POINTER)
        {
            // Find touch device
            return indev;
        }
        indev = lv_indev_get_next(indev);
    }

    return NULL;
}

static bool _eos_touch_coordinate_valid(int32_t x, int32_t y)
{
    return x >= 0 && x < (int32_t)EOS_DISPLAY_WIDTH && y >= 0 && y < (int32_t)EOS_DISPLAY_HEIGHT;
}

static bool _eos_touch_enqueue(int32_t x, int32_t y, lv_indev_state_t state, uint16_t repeat)
{
    if (_eos_touch_inject_count >= EOS_TOUCH_INJECT_QUEUE_SIZE)
    {
        return false;
    }

    _eos_touch_inject_queue[_eos_touch_inject_tail].x = x;
    _eos_touch_inject_queue[_eos_touch_inject_tail].y = y;
    _eos_touch_inject_queue[_eos_touch_inject_tail].state = state;
    _eos_touch_inject_queue[_eos_touch_inject_tail].repeat = repeat == 0U ? 1U : repeat;
    _eos_touch_inject_tail = (uint8_t)((_eos_touch_inject_tail + 1U) % EOS_TOUCH_INJECT_QUEUE_SIZE);
    _eos_touch_inject_count++;
    return true;
}

static void _eos_touch_reset_queue(void)
{
    _eos_touch_inject_head = 0U;
    _eos_touch_inject_tail = 0U;
    _eos_touch_inject_count = 0U;
    _eos_touch_inject_current_repeat = 0U;
    _eos_touch_inject_release_queued = 0U;
    _eos_touch_inject_pressed_delivered = 0U;
    _eos_touch_trace_reset();
}

static void _eos_touch_control_overlay_set(bool visible)
{
    if (_eos_touch_control_glow == NULL || _eos_touch_control_banner == NULL)
    {
        return;
    }

    if (visible)
    {
        lv_obj_clear_flag(_eos_touch_control_glow, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(_eos_touch_control_banner, LV_OBJ_FLAG_HIDDEN);
    }
    else
    {
        lv_obj_add_flag(_eos_touch_control_glow, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(_eos_touch_control_banner, LV_OBJ_FLAG_HIDDEN);
    }
}

static void _eos_touch_abort_injection(void)
{
    if (_eos_touch_inject_active != 0U && _eos_touch_inject_pressed_delivered != 0U)
    {
        _eos_touch_inject_abort_release_pending = 1U;
    }

    _eos_touch_inject_active = 0U;
    _eos_touch_inject_pressed_delivered = 0U;
    _eos_touch_reset_queue();
    _eos_touch_marker_hide();
}

bool eos_touch_control_acquire(void)
{
    if (_eos_touch_ready == 0U)
    {
        return false;
    }

    if (_eos_touch_control_state == EOS_TOUCH_CONTROL_CONTROLLED)
    {
        _eos_touch_control_overlay_set(true);
        return true;
    }

    _eos_touch_abort_injection();
    _eos_touch_physical_suppress = 0U;
    _eos_touch_control_state = EOS_TOUCH_CONTROL_CONTROLLED;
    _eos_touch_control_overlay_set(true);
    return true;
}

void eos_touch_control_release(void)
{
    if (_eos_touch_ready == 0U)
    {
        return;
    }

    _eos_touch_abort_injection();
    _eos_touch_physical_suppress = 0U;
    _eos_touch_control_state = EOS_TOUCH_CONTROL_UNCONTROLLED;
    _eos_touch_control_overlay_set(false);
}

eos_touch_control_state_t eos_touch_control_get_state(void)
{
    return _eos_touch_control_state;
}

static bool _eos_touch_filter_physical(lv_indev_data_t *data)
{
    if (data == NULL)
    {
        return false;
    }

    if (_eos_touch_physical_suppress != 0U)
    {
        if (data->state == LV_INDEV_STATE_REL)
        {
            _eos_touch_physical_suppress = 0U;
        }
        data->state = LV_INDEV_STATE_REL;
        return true;
    }

    if (_eos_touch_control_state == EOS_TOUCH_CONTROL_CONTROLLED && data->state == LV_INDEV_STATE_PR)
    {
        /* A physical press is an explicit request to take control back.
         * Cancel automation before LVGL sees the real sample. */
        _eos_touch_control_state = EOS_TOUCH_CONTROL_UNCONTROLLED;
        _eos_touch_physical_suppress = 1U;
        _eos_touch_abort_injection();
        _eos_touch_control_overlay_set(false);
        data->state = LV_INDEV_STATE_REL;
        return true;
    }

    return false;
}

bool eos_touch_read(lv_indev_data_t *data)
{
    int32_t x = 0;
    int32_t y = 0;
    lv_indev_state_t state = LV_INDEV_STATE_REL;

    if (data == NULL)
    {
        return false;
    }

    {
        eos_critical_ctx_t ctx = eos_critical_enter();
        if (_eos_touch_physical_valid != 0U)
        {
            x = _eos_touch_physical_x;
            y = _eos_touch_physical_y;
            state = _eos_touch_physical_state;
        }
        eos_critical_leave(ctx);
    }

    data->point.x = (lv_coord_t)x;
    data->point.y = (lv_coord_t)y;
    data->state = state;

    (void)_eos_touch_filter_physical(data);
    if (eos_touch_read_injected(data))
    {
        return true;
    }
    return true;
}

static eos_touch_inject_result_t _eos_touch_begin(void)
{
    if (_eos_touch_ready == 0U)
    {
        return EOS_TOUCH_INJECT_NOT_READY;
    }
    if (_eos_touch_inject_active != 0U)
    {
        return EOS_TOUCH_INJECT_BUSY;
    }

    _eos_touch_reset_queue();
    _eos_touch_inject_active = 1U;
    return EOS_TOUCH_INJECT_OK;
}

static void _eos_touch_marker_set(int32_t x, int32_t y)
{
    _eos_touch_trace_add(x, y);

    if (_eos_touch_marker == NULL || _eos_touch_marker_core == NULL)
    {
        return;
    }

    lv_obj_set_pos(_eos_touch_marker,
                   (lv_coord_t)(x - (int32_t)(EOS_TOUCH_MARKER_SIZE / 2U)),
                   (lv_coord_t)(y - (int32_t)(EOS_TOUCH_MARKER_SIZE / 2U)));
    lv_obj_set_pos(_eos_touch_marker_core,
                   (lv_coord_t)(x - (int32_t)(EOS_TOUCH_MARKER_CORE_SIZE / 2U)),
                   (lv_coord_t)(y - (int32_t)(EOS_TOUCH_MARKER_CORE_SIZE / 2U)));
    lv_obj_clear_flag(_eos_touch_marker, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(_eos_touch_marker_core, LV_OBJ_FLAG_HIDDEN);
}

static void _eos_touch_marker_hide(void)
{
    if (_eos_touch_marker != NULL)
    {
        lv_obj_add_flag(_eos_touch_marker, LV_OBJ_FLAG_HIDDEN);
    }
    if (_eos_touch_marker_core != NULL)
    {
        lv_obj_add_flag(_eos_touch_marker_core, LV_OBJ_FLAG_HIDDEN);
    }
    if (_eos_touch_trace_start != NULL)
    {
        lv_obj_add_flag(_eos_touch_trace_start, LV_OBJ_FLAG_HIDDEN);
    }
    if (_eos_touch_trace_line != NULL)
    {
        lv_obj_add_flag(_eos_touch_trace_line, LV_OBJ_FLAG_HIDDEN);
    }
}

static void _eos_touch_trace_canvas_delete_cb(lv_event_t *event)
{
    (void)event;
    if (_eos_touch_trace_draw_buf != NULL)
    {
        lv_draw_buf_destroy(_eos_touch_trace_draw_buf);
        _eos_touch_trace_draw_buf = NULL;
    }
}

static void _eos_touch_trace_reset(void)
{
    _eos_touch_trace_point_count = 0U;
    if (_eos_touch_trace_line != NULL && _eos_touch_trace_draw_buf != NULL)
    {
        lv_canvas_fill_bg(_eos_touch_trace_line, lv_color_black(), LV_OPA_TRANSP);
    }
    _eos_touch_marker_hide();
}

static void _eos_touch_trace_update(void)
{
    if (_eos_touch_trace_line != NULL)
    {
        if (_eos_touch_trace_point_count > 1U)
        {
            lv_obj_clear_flag(_eos_touch_trace_line, LV_OBJ_FLAG_HIDDEN);
        }
        else
        {
            lv_obj_add_flag(_eos_touch_trace_line, LV_OBJ_FLAG_HIDDEN);
        }
        _eos_touch_trace_render();
    }

    if (_eos_touch_trace_start != NULL && _eos_touch_trace_point_count > 1U)
    {
        lv_obj_set_pos(_eos_touch_trace_start,
                       (lv_coord_t)(_eos_touch_trace_points[0].x - (int32_t)(EOS_TOUCH_TRACE_START_SIZE / 2U)),
                       (lv_coord_t)(_eos_touch_trace_points[0].y - (int32_t)(EOS_TOUCH_TRACE_START_SIZE / 2U)));
        lv_obj_clear_flag(_eos_touch_trace_start, LV_OBJ_FLAG_HIDDEN);
    }
}

static void _eos_touch_trace_add(int32_t x, int32_t y)
{
    if ((_eos_touch_trace_point_count > 0U) && (_eos_touch_trace_points[_eos_touch_trace_point_count - 1U].x == x)
        && (_eos_touch_trace_points[_eos_touch_trace_point_count - 1U].y == y))
    {
        return;
    }

    if (_eos_touch_trace_point_count >= EOS_TOUCH_TRACE_MAX_POINTS)
    {
        memmove(&_eos_touch_trace_points[0],
                &_eos_touch_trace_points[1],
                sizeof(_eos_touch_trace_points[0]) * (EOS_TOUCH_TRACE_MAX_POINTS - 1U));
        _eos_touch_trace_point_count = EOS_TOUCH_TRACE_MAX_POINTS - 1U;
    }

    _eos_touch_trace_points[_eos_touch_trace_point_count].x = x;
    _eos_touch_trace_points[_eos_touch_trace_point_count].y = y;
    _eos_touch_trace_point_count++;
    _eos_touch_trace_update();
}

static void _eos_touch_trace_render(void)
{
    const lv_point_precise_t *draw_points = _eos_touch_trace_points;
    uint32_t draw_point_count = _eos_touch_trace_point_count;
    uint32_t stride;
    int32_t min_x = EOS_DISPLAY_WIDTH;
    int32_t min_y = EOS_DISPLAY_HEIGHT;
    int32_t max_x = 0;
    int32_t max_y = 0;
    const double half_width = EOS_TOUCH_TRACE_LINE_WIDTH / 2.0;
    const double antialias_width = 1.5;
    const lv_color_t trace_color = lv_color_hex(0x0c68fa);

    if (_eos_touch_trace_draw_buf == NULL)
    {
        return;
    }

    lv_canvas_fill_bg(_eos_touch_trace_line, lv_color_black(), LV_OPA_TRANSP);
    if (_eos_touch_trace_point_count < 2U)
    {
        return;
    }

    for (uint32_t i = 0U; i < draw_point_count; i++)
    {
        min_x = LV_MIN(min_x, (int32_t)draw_points[i].x);
        min_y = LV_MIN(min_y, (int32_t)draw_points[i].y);
        max_x = LV_MAX(max_x, (int32_t)draw_points[i].x);
        max_y = LV_MAX(max_y, (int32_t)draw_points[i].y);
    }

    min_x = LV_MAX(0, (int32_t)floor((double)min_x - half_width - antialias_width));
    min_y = LV_MAX(0, (int32_t)floor((double)min_y - half_width - antialias_width));
    max_x = LV_MIN((int32_t)EOS_DISPLAY_WIDTH - 1, (int32_t)ceil((double)max_x + half_width + antialias_width));
    max_y = LV_MIN((int32_t)EOS_DISPLAY_HEIGHT - 1, (int32_t)ceil((double)max_y + half_width + antialias_width));
    stride = _eos_touch_trace_draw_buf->header.stride;

    /* Rasterize the exact piecewise-linear path once per pixel.  The minimum
     * distance makes joins naturally round and avoids the visible seams caused
     * by layering LVGL line segments.  Count separated runs of nearby line
     * segments as separate passes through the pixel.  This preserves the
     * seamless union for one pass, while allowing a retraced/overlapping path
     * to build up opacity naturally. */
    for (int32_t y = min_y; y <= max_y; y++)
    {
        for (int32_t x = min_x; x <= max_x; x++)
        {
            double px = (double)x + 0.5;
            double py = (double)y + 0.5;
            double min_distance_squared = INFINITY;
            double stroke_threshold = half_width + antialias_width;
            uint32_t path_passes = 0U;
            bool previous_segment_hit = false;

            for (uint32_t i = 0U; i + 1U < draw_point_count; i++)
            {
                double x1 = (double)draw_points[i].x;
                double y1 = (double)draw_points[i].y;
                double dx = (double)draw_points[i + 1U].x - x1;
                double dy = (double)draw_points[i + 1U].y - y1;
                double length_squared = (dx * dx) + (dy * dy);
                double ratio;
                double nearest_x;
                double nearest_y;
                double distance_x;
                double distance_y;
                double distance_squared;

                if (length_squared <= 0.0)
                {
                    continue;
                }
                ratio = (((px - x1) * dx) + ((py - y1) * dy)) / length_squared;
                ratio = LV_CLAMP(0.0, ratio, 1.0);
                nearest_x = x1 + (ratio * dx);
                nearest_y = y1 + (ratio * dy);
                distance_x = px - nearest_x;
                distance_y = py - nearest_y;
                distance_squared = (distance_x * distance_x) + (distance_y * distance_y);
                min_distance_squared = LV_MIN(min_distance_squared, distance_squared);

                /* Adjacent line segments belong to the same passage.  A
                 * second passage is only counted after a gap in segment
                 * indices, which prevents normal joins from becoming darker
                 * merely because their segments overlap spatially. */
                if (distance_squared < (stroke_threshold * stroke_threshold))
                {
                    if (!previous_segment_hit)
                    {
                        path_passes++;
                    }
                    previous_segment_hit = true;
                }
                else
                {
                    previous_segment_hit = false;
                }
            }

            double distance = sqrt(min_distance_squared);
            if (distance < stroke_threshold)
            {
                double coverage = (half_width + antialias_width - distance) / (2.0 * antialias_width);
                double base_alpha;
                double accumulated_alpha;
                lv_color32_t *pixel;

                coverage = LV_CLAMP(0.0, coverage, 1.0);
                path_passes = LV_MAX(path_passes, 1U);
                base_alpha = ((double)LV_OPA_60 / 255.0) * coverage;
                accumulated_alpha = 1.0 - pow(1.0 - base_alpha, (double)path_passes);
                pixel = (lv_color32_t *)(_eos_touch_trace_draw_buf->data + ((uint32_t)y * stride)
                                         + ((uint32_t)x * sizeof(lv_color32_t)));
                pixel->red = trace_color.red;
                pixel->green = trace_color.green;
                pixel->blue = trace_color.blue;
                pixel->alpha = (uint8_t)lround(255.0 * accumulated_alpha);
            }
        }
    }

    lv_obj_invalidate(_eos_touch_trace_line);
}

bool eos_touch_read_injected(lv_indev_data_t *data)
{
    if (data == NULL)
    {
        return false;
    }

    if (_eos_touch_inject_abort_release_pending != 0U)
    {
        data->point.x = (lv_coord_t)_eos_touch_inject_current.x;
        data->point.y = (lv_coord_t)_eos_touch_inject_current.y;
        data->state = LV_INDEV_STATE_REL;
        _eos_touch_inject_abort_release_pending = 0U;
        _eos_touch_marker_hide();
        return true;
    }

    if (_eos_touch_inject_active == 0U)
    {
        return false;
    }

    if (_eos_touch_inject_current_repeat == 0U)
    {
        if (_eos_touch_inject_count == 0U)
        {
            if (_eos_touch_inject_release_queued == 0U)
            {
                /* A standalone down() must remain pressed across indev
                 * reads until the caller supplies move() or up(). */
                data->point.x = (lv_coord_t)_eos_touch_inject_current.x;
                data->point.y = (lv_coord_t)_eos_touch_inject_current.y;
                data->state = LV_INDEV_STATE_PR;
                _eos_touch_marker_set(_eos_touch_inject_current.x, _eos_touch_inject_current.y);
                return true;
            }

            /* The release sample was returned on the previous read.  Resume
             * the physical device on this read. */
            _eos_touch_inject_active = 0U;
            _eos_touch_inject_release_queued = 0U;
            _eos_touch_inject_pressed_delivered = 0U;
            _eos_touch_marker_hide();
            return false;
        }

        _eos_touch_inject_current = _eos_touch_inject_queue[_eos_touch_inject_head];
        _eos_touch_inject_head = (uint8_t)((_eos_touch_inject_head + 1U) % EOS_TOUCH_INJECT_QUEUE_SIZE);
        _eos_touch_inject_count--;
        _eos_touch_inject_current_repeat = _eos_touch_inject_current.repeat;
    }

    data->point.x = (lv_coord_t)_eos_touch_inject_current.x;
    data->point.y = (lv_coord_t)_eos_touch_inject_current.y;
    data->state = _eos_touch_inject_current.state;
    if (data->state == LV_INDEV_STATE_PR)
    {
        _eos_touch_inject_pressed_delivered = 1U;
    }
    _eos_touch_marker_set(_eos_touch_inject_current.x, _eos_touch_inject_current.y);
    _eos_touch_inject_current_repeat--;
    return true;
}

eos_touch_inject_result_t eos_touch_inject_down(int32_t x, int32_t y)
{
    eos_touch_inject_result_t result;

    if (!_eos_touch_coordinate_valid(x, y))
    {
        return EOS_TOUCH_INJECT_INVALID;
    }
    result = _eos_touch_begin();
    if (result != EOS_TOUCH_INJECT_OK)
    {
        return result;
    }
    if (!_eos_touch_enqueue(x, y, LV_INDEV_STATE_PR, 1U))
    {
        _eos_touch_inject_active = 0U;
        return EOS_TOUCH_INJECT_QUEUE_FULL;
    }
    _eos_touch_trace_add(x, y);
    return EOS_TOUCH_INJECT_OK;
}

eos_touch_inject_result_t eos_touch_inject_move(int32_t x, int32_t y)
{
    if (!_eos_touch_coordinate_valid(x, y))
    {
        return EOS_TOUCH_INJECT_INVALID;
    }
    if (_eos_touch_inject_active == 0U)
    {
        return _eos_touch_ready == 0U ? EOS_TOUCH_INJECT_NOT_READY : EOS_TOUCH_INJECT_NO_ACTIVE;
    }
    if (_eos_touch_inject_release_queued != 0U)
    {
        return EOS_TOUCH_INJECT_BUSY;
    }
    if (!_eos_touch_enqueue(x, y, LV_INDEV_STATE_PR, 1U))
    {
        return EOS_TOUCH_INJECT_QUEUE_FULL;
    }
    _eos_touch_trace_add(x, y);
    return EOS_TOUCH_INJECT_OK;
}

eos_touch_inject_result_t eos_touch_inject_up(int32_t x, int32_t y)
{
    if (!_eos_touch_coordinate_valid(x, y))
    {
        return EOS_TOUCH_INJECT_INVALID;
    }
    if (_eos_touch_inject_active == 0U)
    {
        return _eos_touch_ready == 0U ? EOS_TOUCH_INJECT_NOT_READY : EOS_TOUCH_INJECT_NO_ACTIVE;
    }
    if (_eos_touch_inject_release_queued != 0U)
    {
        return EOS_TOUCH_INJECT_BUSY;
    }
    if (!_eos_touch_enqueue(x, y, LV_INDEV_STATE_REL, 1U))
    {
        return EOS_TOUCH_INJECT_QUEUE_FULL;
    }
    _eos_touch_inject_release_queued = 1U;
    _eos_touch_trace_add(x, y);
    return EOS_TOUCH_INJECT_OK;
}

eos_touch_inject_result_t eos_touch_inject_tap(int32_t x, int32_t y)
{
    eos_touch_inject_result_t result;

    if (!_eos_touch_coordinate_valid(x, y))
    {
        return EOS_TOUCH_INJECT_INVALID;
    }
    result = _eos_touch_begin();
    if (result != EOS_TOUCH_INJECT_OK)
    {
        return result;
    }
    if (!_eos_touch_enqueue(x, y, LV_INDEV_STATE_PR, 1U) || !_eos_touch_enqueue(x, y, LV_INDEV_STATE_REL, 1U))
    {
        _eos_touch_inject_active = 0U;
        _eos_touch_reset_queue();
        return EOS_TOUCH_INJECT_QUEUE_FULL;
    }
    _eos_touch_inject_release_queued = 1U;
    return EOS_TOUCH_INJECT_OK;
}

eos_touch_inject_result_t eos_touch_inject_swipe(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t duration_ms)
{
    eos_touch_inject_result_t result;
    uint32_t steps;
    uint32_t repeat;
    uint32_t i;

    if (!_eos_touch_coordinate_valid(x1, y1) || !_eos_touch_coordinate_valid(x2, y2) || duration_ms == 0U
        || duration_ms > 5000U)
    {
        return EOS_TOUCH_INJECT_INVALID;
    }
    result = _eos_touch_begin();
    if (result != EOS_TOUCH_INJECT_OK)
    {
        return result;
    }

    steps = (duration_ms + EOS_TOUCH_INDEV_PERIOD_MS - 1U) / EOS_TOUCH_INDEV_PERIOD_MS;
    if (steps < 2U)
    {
        steps = 2U;
    }
    if (steps > 32U)
    {
        steps = 32U;
    }
    repeat = (duration_ms + (steps * EOS_TOUCH_INDEV_PERIOD_MS) - 1U) / (steps * EOS_TOUCH_INDEV_PERIOD_MS);
    if (repeat == 0U)
    {
        repeat = 1U;
    }

    if (!_eos_touch_enqueue(x1, y1, LV_INDEV_STATE_PR, (uint16_t)repeat))
    {
        _eos_touch_inject_active = 0U;
        return EOS_TOUCH_INJECT_QUEUE_FULL;
    }
    for (i = 1U; i < steps; i++)
    {
        int32_t x = x1 + (int32_t)(((int64_t)(x2 - x1) * (int64_t)i) / (int64_t)steps);
        int32_t y = y1 + (int32_t)(((int64_t)(y2 - y1) * (int64_t)i) / (int64_t)steps);
        if (!_eos_touch_enqueue(x, y, LV_INDEV_STATE_PR, (uint16_t)repeat))
        {
            _eos_touch_inject_active = 0U;
            _eos_touch_reset_queue();
            return EOS_TOUCH_INJECT_QUEUE_FULL;
        }
    }
    if (!_eos_touch_enqueue(x2, y2, LV_INDEV_STATE_REL, 1U))
    {
        _eos_touch_inject_active = 0U;
        _eos_touch_reset_queue();
        return EOS_TOUCH_INJECT_QUEUE_FULL;
    }
    _eos_touch_inject_release_queued = 1U;
    return EOS_TOUCH_INJECT_OK;
}
