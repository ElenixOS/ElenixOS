/**
 * @file sni_api_lv_grid.c
 * @brief LVGL grid layout SNI bindings
 */

#include "sni_api_lv_special.h"

/* Includes ---------------------------------------------------*/
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include "eos_mem.h"
#include "lvgl.h"
#include "sni_api_export.h"
#include "sni_type_bridge.h"
#include "sni_types.h"

/* Macros and Definitions -------------------------------------*/
/* TODO: This fixed track cap is temporary. Replace it with target-configured
 * descriptor and LVGL layout-scratch byte budgets plus per-Realm accounting
 * after the supported targets' memory limits are audited.
 */
#define SNI_GRID_MAX_TRACKS 256U

/* Types ------------------------------------------------------*/
typedef struct sni_grid_ctx
{
    lv_obj_t *obj;
    int32_t *col_dsc;
    int32_t *row_dsc;
    lv_event_dsc_t *delete_dsc;
    struct sni_grid_ctx *next;
} sni_grid_ctx_t;

/* Variables --------------------------------------------------*/
static sni_grid_ctx_t *sni_grid_ctx_head = NULL;

/* Function Implementations -----------------------------------*/
static void _sni_grid_free_desc(int32_t *desc)
{
    if (desc)
    {
        eos_free(desc);
    }
}

static sni_grid_ctx_t *_sni_grid_find_ctx(lv_obj_t *obj)
{
    sni_grid_ctx_t *ctx = sni_grid_ctx_head;

    while (ctx)
    {
        if (ctx->obj == obj)
        {
            return ctx;
        }
        ctx = ctx->next;
    }

    return NULL;
}

bool sni_api_lv_grid_is_cleanup_event(const void *event_dsc)
{
    sni_grid_ctx_t *ctx = sni_grid_ctx_head;

    while (ctx)
    {
        if (ctx->delete_dsc == event_dsc)
        {
            return true;
        }
        ctx = ctx->next;
    }

    return false;
}

static void _sni_grid_detach_ctx(sni_grid_ctx_t *ctx)
{
    sni_grid_ctx_t *cursor = sni_grid_ctx_head;
    sni_grid_ctx_t *previous = NULL;

    while (cursor)
    {
        if (cursor == ctx)
        {
            if (previous)
            {
                previous->next = cursor->next;
            }
            else
            {
                sni_grid_ctx_head = cursor->next;
            }

            _sni_grid_free_desc(cursor->col_dsc);
            _sni_grid_free_desc(cursor->row_dsc);
            eos_free(cursor);
            return;
        }
        previous = cursor;
        cursor = cursor->next;
    }
}

static void _sni_grid_obj_delete_cb(lv_event_t *event)
{
    sni_grid_ctx_t *ctx = (sni_grid_ctx_t *)lv_event_get_user_data(event);

    if (ctx)
    {
        _sni_grid_detach_ctx(ctx);
    }
}

static bool _sni_grid_build_desc(jerry_value_t js_desc, int32_t **out_desc)
{
    jerry_length_t length;
    int32_t *desc;
    bool terminated = false;
    jerry_length_t index;

    *out_desc = NULL;
    if (jerry_value_is_null(js_desc))
    {
        return true;
    }
    if (!jerry_value_is_array(js_desc))
    {
        return false;
    }

    length = jerry_array_length(js_desc);
    if (length == 0 || length > SNI_GRID_MAX_TRACKS)
    {
        return false;
    }

    desc = eos_malloc(sizeof(*desc) * ((size_t)length + 1U));
    if (!desc)
    {
        return false;
    }

    for (index = 0; index < length; index++)
    {
        jerry_value_t js_value = jerry_object_get_index(js_desc, index);
        double number;
        int32_t value;

        if (!jerry_value_is_number(js_value))
        {
            jerry_value_free(js_value);
            _sni_grid_free_desc(desc);
            return false;
        }

        number = sni_tb_js2c_number(js_value);
        jerry_value_free(js_value);
        if (!isfinite(number) || number < (double)INT32_MIN || number > (double)INT32_MAX)
        {
            _sni_grid_free_desc(desc);
            return false;
        }

        value = (int32_t)number;
        if ((double)value != number || (value == LV_GRID_TEMPLATE_LAST && index + 1U != length))
        {
            _sni_grid_free_desc(desc);
            return false;
        }

        desc[index] = value;
        if (value == LV_GRID_TEMPLATE_LAST)
        {
            terminated = true;
        }
    }

    if (!terminated)
    {
        desc[length] = LV_GRID_TEMPLATE_LAST;
    }

    *out_desc = desc;
    return true;
}

jerry_value_t sni_api_lv_obj_set_grid_dsc_array(const jerry_call_info_t *call_info_p,
                                                const jerry_value_t args_p[],
                                                const jerry_length_t args_count)
{
    lv_obj_t *obj = NULL;
    sni_grid_ctx_t *ctx;
    int32_t *col_dsc = NULL;
    int32_t *row_dsc = NULL;
    int32_t *old_col_dsc;
    int32_t *old_row_dsc;
    bool is_new_ctx;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert object");
    }
    if (!_sni_grid_build_desc(args_p[0], &col_dsc) || !_sni_grid_build_desc(args_p[1], &row_dsc))
    {
        _sni_grid_free_desc(col_dsc);
        _sni_grid_free_desc(row_dsc);
        return sni_api_throw_error("Grid descriptors must be null or arrays of integer track sizes");
    }

    ctx = _sni_grid_find_ctx(obj);
    is_new_ctx = ctx == NULL;
    if (is_new_ctx)
    {
        ctx = eos_malloc_zeroed(sizeof(*ctx));
        if (!ctx)
        {
            _sni_grid_free_desc(col_dsc);
            _sni_grid_free_desc(row_dsc);
            return sni_api_throw_error("Out of memory");
        }
        ctx->obj = obj;
        ctx->col_dsc = col_dsc;
        ctx->row_dsc = row_dsc;
        ctx->delete_dsc = lv_obj_add_event_cb(obj, _sni_grid_obj_delete_cb, LV_EVENT_DELETE, ctx);
        if (!ctx->delete_dsc)
        {
            _sni_grid_free_desc(ctx->col_dsc);
            _sni_grid_free_desc(ctx->row_dsc);
            eos_free(ctx);
            return sni_api_throw_error("Failed to register grid cleanup");
        }
        ctx->next = sni_grid_ctx_head;
        sni_grid_ctx_head = ctx;
        old_col_dsc = NULL;
        old_row_dsc = NULL;
    }
    else
    {
        old_col_dsc = ctx->col_dsc;
        old_row_dsc = ctx->row_dsc;
        ctx->col_dsc = col_dsc;
        ctx->row_dsc = row_dsc;
    }

    lv_obj_set_grid_dsc_array(obj, col_dsc, row_dsc);
    _sni_grid_free_desc(old_col_dsc);
    _sni_grid_free_desc(old_row_dsc);
    return jerry_undefined();
}
