/**
 * @file eos_test_package.c
 * @brief Package installation test module
 */

#include "eos_config.h"
#if EOS_ENABLE_TEST_APP

#include "eos_test_package.h"
#include "eos_pkg_mgr.h"
#include "eos_pkg_installer.h"
#include "eos_log.h"
#include "eos_activity.h"
#include "eos_basic_widgets.h"
#include "eos_storage_paths.h"
#include "lvgl.h"
#include <inttypes.h>
#include <string.h>
#include <stdio.h>

#define EOS_LOG_TAG "PackageTest"
#define MAX_PATH_LEN 256

/* ============================================
 * Internal types and variables
 * ============================================ */

typedef struct
{
    lv_obj_t *container;
    lv_obj_t *input_field;
    lv_obj_t *preview_btn;
    lv_obj_t *install_btn;
    lv_obj_t *status_label;
    char path_buffer[MAX_PATH_LEN];
} _package_context_t;

static _package_context_t _ctx = {0};

/* ============================================
 * Helper functions
 * ============================================ */

static bool _is_epk_file(const char *path)
{
    size_t len = strlen(path);
    if (len < 5)
        return false;
    return strcmp(path + len - 4, ".epk") == 0;
}

static bool _get_package_path(char *full_path, size_t full_path_size)
{
    const char *input_path;
    int written;

    if (!_ctx.input_field || !full_path || full_path_size == 0U)
    {
        return false;
    }

    input_path = lv_textarea_get_text(_ctx.input_field);
    if (!input_path || input_path[0] == '\0')
    {
        return false;
    }

    if (input_path[0] == '/')
    {
        written = snprintf(full_path, full_path_size, "%s", input_path);
    }
    else
    {
        written = snprintf(full_path, full_path_size, "%s%s", EOS_SYS_ROOT_DIR, input_path);
    }
    return written >= 0 && (size_t)written < full_path_size;
}

static const char *_package_type_name(script_pkg_type_t type)
{
    switch (type)
    {
        case SCRIPT_TYPE_APPLICATION:
            return "application";
        case SCRIPT_TYPE_WATCHFACE:
            return "watchface";
        default:
            return "unknown";
    }
}

/* ============================================
 * Preview button callback
 * ============================================ */

static void _preview_btn_cb(lv_event_t *e)
{
    LV_UNUSED(e);

    if (!_ctx.status_label)
    {
        return;
    }

    char full_path[MAX_PATH_LEN];
    if (!_get_package_path(full_path, sizeof(full_path)))
    {
        lv_label_set_text(_ctx.status_label, "Error: Please enter a package path");
        lv_obj_set_style_text_color(_ctx.status_label, lv_color_hex(0xFF0000), 0);
        return;
    }
    if (!_is_epk_file(full_path))
    {
        lv_label_set_text(_ctx.status_label, "Error: Unsupported file type");
        lv_obj_set_style_text_color(_ctx.status_label, lv_color_hex(0xFF0000), 0);
        return;
    }

    lv_label_set_text(_ctx.status_label, "Reading package preview...");
    lv_obj_set_style_text_color(_ctx.status_label, lv_color_hex(0xFFFF00), 0);
    lv_refr_now(NULL);

    eos_pkg_t *package = NULL;
    eos_pkg_preview_t preview = {0};
    eos_pkg_manifest_info_t manifest_info = {0};
    eos_result_t result = eos_pkg_open(full_path, &package);
    if (result == EOS_OK)
    {
        result = eos_pkg_read_preview(package, &preview);
    }
    if (result == EOS_OK)
    {
        result = eos_pkg_read_manifest_info(package, &manifest_info);
    }

    if (result != EOS_OK)
    {
        char message[128];
        snprintf(message, sizeof(message), "Preview failed (code: %d)", result);
        lv_label_set_text(_ctx.status_label, message);
        lv_obj_set_style_text_color(_ctx.status_label, lv_color_hex(0xFF0000), 0);
        eos_pkg_manifest_info_free(&manifest_info);
        eos_pkg_preview_free(&preview);
        eos_pkg_close(package);
        return;
    }

    const eos_pkg_header_t *header = eos_pkg_get_header(package);
    char message[768];
    snprintf(message,
             sizeof(message),
             "EPKG v%" PRIu32 "  FPMA v%" PRIu32 "\n"
             "Type: %s\n"
             "ID: %s\n"
             "Name: %s\n"
             "Version: %s\n"
             "API: %" PRIu16 "..%" PRIu16 "\n"
             "Files: %" PRIu32 "\n"
             "Manifest: %" PRIu32 " bytes\n"
             "Icon: %s (%" PRIu32 " bytes)",
             header->format_version,
             preview.fpm.fpm_version,
             _package_type_name(eos_pkg_get_package_type(package)),
             manifest_info.id,
             manifest_info.name,
             manifest_info.version,
             manifest_info.min_api_level,
             manifest_info.target_api_level,
             header->file_count,
             preview.manifest_size,
             (preview.fpm.flags & EOS_PKG_FPM_FLAG_HAS_ICON) != 0U ? "present" : "none",
             preview.icon_size);
    lv_label_set_text(_ctx.status_label, message);
    lv_obj_set_style_text_color(_ctx.status_label, lv_color_white(), 0);

    eos_pkg_manifest_info_free(&manifest_info);
    eos_pkg_preview_free(&preview);
    eos_pkg_close(package);
}

/* ============================================
 * Install button callback
 * ============================================ */

static void _install_btn_cb(lv_event_t *e)
{
    LV_UNUSED(e);

    if (!_ctx.input_field || !_ctx.status_label)
    {
        return;
    }

    char full_path[MAX_PATH_LEN];
    if (!_get_package_path(full_path, sizeof(full_path)))
    {
        lv_label_set_text(_ctx.status_label, "Error: Please enter a path");
        lv_obj_set_style_text_color(_ctx.status_label, lv_color_hex(0xFF0000), 0);
        return;
    }

    lv_label_set_text(_ctx.status_label, "Installing...");
    lv_obj_set_style_text_color(_ctx.status_label, lv_color_hex(0xFFFF00), 0);
    lv_refr_now(NULL);

    EOS_LOG_I("Full package path: %s", full_path);

    eos_result_t ret;
    if (_is_epk_file(full_path))
    {
        EOS_LOG_I("Installing EPK package: %s", full_path);
        ret = eos_pkg_install(full_path);
    }
    else
    {
        lv_label_set_text(_ctx.status_label, "Error: Unsupported file type");
        lv_obj_set_style_text_color(_ctx.status_label, lv_color_hex(0xFF0000), 0);
        return;
    }

    if (ret == EOS_OK)
    {
        lv_label_set_text(_ctx.status_label, "Installation successful!");
        lv_obj_set_style_text_color(_ctx.status_label, lv_color_hex(0x4CAF50), 0);
        lv_textarea_set_text(_ctx.input_field, "");
    }
    else
    {
        char msg[128];
        snprintf(msg, sizeof(msg), "Installation failed (code: %d)", ret);
        lv_label_set_text(_ctx.status_label, msg);
        lv_obj_set_style_text_color(_ctx.status_label, lv_color_hex(0xFF0000), 0);
    }
}

/* ============================================
 * Activity lifecycle
 * ============================================ */

static void _package_test_on_destroy(eos_activity_t *activity)
{
    LV_UNUSED(activity);

    /* Reset context */
    _ctx.container = NULL;
    _ctx.input_field = NULL;
    _ctx.preview_btn = NULL;
    _ctx.install_btn = NULL;
    _ctx.status_label = NULL;
    memset(_ctx.path_buffer, 0, sizeof(_ctx.path_buffer));
}

static const eos_activity_lifecycle_t _s_package_test_lifecycle = {.on_enter = NULL,
                                                                   .on_destroy = _package_test_on_destroy,
                                                                   .on_pause = NULL,
                                                                   .on_resume = NULL};

/* ============================================
 * Main test function
 * ============================================ */

void eos_test_package_start(void)
{
    eos_activity_t *activity = eos_activity_create(&_s_package_test_lifecycle);
    if (!activity)
    {
        return;
    }

    lv_obj_t *view = eos_activity_get_view(activity);
    if (!view)
    {
        return;
    }

    eos_activity_set_title(activity, "Package Installer");
    eos_activity_set_type(activity, EOS_ACTIVITY_TYPE_APP);

    /* Create container */
    _ctx.container = lv_obj_create(view);
    lv_obj_set_size(_ctx.container, lv_pct(100), lv_pct(100));
    lv_obj_set_style_pad_all(_ctx.container, 15, 0);
    lv_obj_set_flex_flow(_ctx.container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(_ctx.container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_SPACE_EVENLY);

    /* Create path label */
    lv_obj_t *path_label = lv_label_create(_ctx.container);
    lv_label_set_text(path_label, "Package Path:");
    lv_obj_set_style_text_color(path_label, lv_color_white(), 0);

    /* Create input field */
    _ctx.input_field = lv_textarea_create(_ctx.container);
    lv_textarea_set_one_line(_ctx.input_field, true);
    lv_textarea_set_placeholder_text(_ctx.input_field, "my_package.epk");
    lv_obj_set_width(_ctx.input_field, lv_pct(85));
    lv_textarea_set_max_length(_ctx.input_field, MAX_PATH_LEN);
    lv_obj_set_style_bg_color(_ctx.input_field, lv_color_black(), 0);
    lv_obj_set_style_border_color(_ctx.input_field, lv_color_white(), 0);
    lv_obj_set_style_border_width(_ctx.input_field, 1, 0);
    lv_obj_set_style_text_color(_ctx.input_field, lv_color_white(), 0);

    /* Create preview button */
    _ctx.preview_btn = lv_button_create(_ctx.container);
    lv_obj_set_size(_ctx.preview_btn, lv_pct(60), 40);
    lv_obj_add_event_cb(_ctx.preview_btn, _preview_btn_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *preview_btn_label = lv_label_create(_ctx.preview_btn);
    lv_label_set_text(preview_btn_label, "Preview");
    lv_obj_center(preview_btn_label);

    /* Create install button */
    _ctx.install_btn = lv_button_create(_ctx.container);
    lv_obj_set_size(_ctx.install_btn, lv_pct(60), 40);
    lv_obj_add_event_cb(_ctx.install_btn, _install_btn_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *btn_label = lv_label_create(_ctx.install_btn);
    lv_label_set_text(btn_label, "Install");
    lv_obj_center(btn_label);

    /* Create status label */
    _ctx.status_label = lv_label_create(_ctx.container);
    lv_label_set_text(_ctx.status_label, " ");
    lv_obj_set_style_text_color(_ctx.status_label, lv_color_white(), 0);
    lv_obj_set_width(_ctx.status_label, lv_pct(85));
    lv_label_set_long_mode(_ctx.status_label, LV_LABEL_LONG_WRAP);

    /* Create hint label */
    lv_obj_t *hint_label = lv_label_create(_ctx.container);
    lv_label_set_text(hint_label,
                      "Supported: .epk (application or watchface)\nPath auto-prefixed with '" EOS_SYS_ROOT_DIR "'");
    lv_obj_set_style_text_color(hint_label, lv_color_hex(0x808080), 0);

    /* Enter activity */
    eos_activity_enter(activity);
}

#endif /* EOS_ENABLE_TEST_APP */
