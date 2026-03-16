/**
 * @file sni_api_lv_image.c
 * @brief LVGL image SNI special wrappers
 */

#include "sni_api_lv_special.h"

/* Includes ---------------------------------------------------*/
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "elena_os_img.h"
#include "elena_os_mem.h"
#include "elena_os_port.h"
#include "lvgl.h"
#include "script_engine_core.h"
#include "sni_api_export.h"
#include "sni_type_bridge.h"
#include "sni_types.h"
#include "elena_os_log.h"
#include "elena_os_watchface.h"
#include "elena_os_app.h"
#include "elena_os_fs.h"

/* Function Implementations -----------------------------------*/

static bool sni_image_get_script_root(char *out_path, size_t out_size)
{
    const char *script_id = script_engine_get_current_script_id();

    if (!out_path || out_size == 0 || !script_id || script_id[0] == '\0')
    {
        return false;
    }

    if (script_engine_get_current_script_type() == SCRIPT_TYPE_APPLICATION)
    {
        snprintf(out_path, out_size, EOS_APP_INSTALLED_DIR "%s", script_id);
        return true;
    }

    if (script_engine_get_current_script_type() == SCRIPT_TYPE_WATCHFACE)
    {
        snprintf(out_path, out_size, EOS_WATCHFACE_INSTALLED_DIR "%s", script_id);
        return true;
    }

    return false;
}

static bool sni_image_is_within_root(const char *root_path, const char *candidate_path)
{
    size_t root_len;

    if (!root_path || !candidate_path)
    {
        return false;
    }

    root_len = strlen(root_path);
    if (strncmp(root_path, candidate_path, root_len) != 0)
    {
        return false;
    }

    return candidate_path[root_len] == '\0' || candidate_path[root_len] == '/';
}

static bool sni_image_has_parent_ref(const char *src)
{
    const char *cursor = src;

    while (cursor && *cursor)
    {
        const char *slash = strchr(cursor, '/');
        size_t len = slash ? (size_t)(slash - cursor) : strlen(cursor);

        if (len == 2 && cursor[0] == '.' && cursor[1] == '.')
        {
            return true;
        }

        if (!slash)
        {
            break;
        }
        cursor = slash + 1;
    }

    return false;
}

static char *sni_image_dup_string(const char *src)
{
    char *ret;

    if (!src)
    {
        return NULL;
    }

    ret = eos_malloc(strlen(src) + 1);
    if (!ret)
    {
        return NULL;
    }

    strcpy(ret, src);
    return ret;
}

static char *sni_image_resolve_under_root(const char *root_dir, const char *candidate)
{
    char root_real[PATH_MAX];
    char candidate_real[PATH_MAX];

    if (!root_dir || !candidate)
    {
        return NULL;
    }

    if (!realpath(root_dir, root_real) || !realpath(candidate, candidate_real))
    {
        return NULL;
    }

    if (!eos_is_file(candidate_real))
    {
        return NULL;
    }

    if (!sni_image_is_within_root(root_real, candidate_real))
    {
        EOS_LOG_W("Reject image src outside script root: %s", candidate_real);
        return NULL;
    }

    return sni_image_dup_string(candidate_real);
}

static char *sni_image_resolve_asset_path(const char *src)
{
    char root_dir[PATH_MAX];
    char candidate[PATH_MAX];

    if (!src || src[0] == '\0')
    {
        return NULL;
    }

    if (!sni_image_get_script_root(root_dir, sizeof(root_dir)))
    {
        return NULL;
    }

    if (sni_image_has_parent_ref(src))
    {
        EOS_LOG_W("Reject image src with parent traversal: %s", src);
        return NULL;
    }

    if (src[0] == '/')
    {
        return sni_image_resolve_under_root(root_dir, src);
    }

    if (strchr(src, '/'))
    {
        snprintf(candidate, sizeof(candidate), "%s/%s", root_dir, src);
    }
    else
    {
        snprintf(candidate, sizeof(candidate), "%s/assets/%s", root_dir, src);
    }

    return sni_image_resolve_under_root(root_dir, candidate);
}

jerry_value_t sni_api_lv_image_set_src(const jerry_call_info_t *call_info_p,
                                       const jerry_value_t args_p[],
                                       const jerry_length_t args_count)
{
    lv_obj_t *self_obj = NULL;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (jerry_value_is_null(args_p[0]))
    {
        lv_image_set_src(self_obj, NULL);
        return jerry_undefined();
    }

    if (jerry_value_is_string(args_p[0]))
    {
        const char *raw_src = sni_tb_js2c_string(args_p[0]);
        char *resolved_path = NULL;

        if (!raw_src)
        {
            return sni_api_throw_error("Out of memory");
        }

        resolved_path = sni_image_resolve_asset_path(raw_src);
        if (resolved_path)
        {
            eos_img_set_src(self_obj, resolved_path);
            eos_free(resolved_path);
        }
        else
        {
            eos_free((void *)raw_src);
            return sni_api_throw_error("Image src must stay within current script directory");
        }

        eos_free((void *)raw_src);
        return jerry_undefined();
    }

    if (jerry_value_is_object(args_p[0]))
    {
        lv_image_dsc_t *arg_src_dsc = NULL;
        if (sni_tb_js2c(args_p[0], SNI_H_LV_IMAGE_DSC, &arg_src_dsc))
        {
            lv_image_set_src(self_obj, arg_src_dsc);
            return jerry_undefined();
        }

        /* Fallback for wrappers that expose descriptors as a different handle type. */
        const void *arg_src_ptr = NULL;
        sni_type_t actual_type = SNI_T_UNKNOWN;
        if (sni_tb_js2c_any_handle(args_p[0], &arg_src_ptr, &actual_type))
        {
            EOS_LOG_W("lv_image_set_src fallback handle type=%d ptr=%p", actual_type, arg_src_ptr);
            lv_image_set_src(self_obj, arg_src_ptr);
            return jerry_undefined();
        }

        return sni_api_throw_error("Failed to convert argument");
    }

    return sni_api_throw_error("Invalid argument type");
}

jerry_value_t sni_api_prop_set_image_src(const jerry_call_info_t *call_info_p,
                                         const jerry_value_t args_p[],
                                         const jerry_length_t args_count)
{
    return sni_api_lv_image_set_src(call_info_p, args_p, args_count);
}
