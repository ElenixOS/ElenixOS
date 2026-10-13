/**
 * @file eos_font.c
 * @brief Text and RemixIcon TinyTTF font management
 */

#include "eos_font.h"

/* Includes ---------------------------------------------------*/
#define EOS_LOG_TAG "FontSystem"
#include "eos_log.h"
#include "eos_mem.h"

/* Macros and Definitions -------------------------------------*/
#if EOS_FONT_TTF_TYPE == EOS_FONT_TTF_DATA
EOS_FONT_DATA_DECLARE(EOS_FONT_TTF_DATA_NAME);
EOS_FONT_DATA_SIZE_DECLARE(EOS_FONT_TTF_DATA_SIZE);
#endif

typedef enum
{
    _EOS_FONT_SOURCE_TEXT,
    _EOS_FONT_SOURCE_ICON,
} _eos_font_source_t;

typedef struct _eos_font_instance
{
    _eos_font_source_t source;
    int32_t size_px;
    lv_font_t *font;
    struct _eos_font_instance *next;
} _eos_font_instance_t;

/* Variables --------------------------------------------------*/
static _eos_font_instance_t *_font_instances;
static bool _font_inited;
static bool _using_builtin_fallback;
static eos_result_t _font_init_error;
#if EOS_FONT_TTF_TYPE == EOS_FONT_TTF_FILE
static const char _text_font_path[] = EOS_SYS_RES_FONT_DIR EOS_FONT_TTF_FILE_PATH;
#endif
static const char _icon_font_path[] = EOS_SYS_RES_FONT_DIR EOS_FONT_ICON_TTF_FILE_PATH;

/* TODO(v0.8): Bound resident TinyTTF memory and file handles by releasing font instances no longer in use. */

/* Function Implementations -----------------------------------*/

static lv_font_t *_find_font(_eos_font_source_t source, int32_t size_px)
{
    for (_eos_font_instance_t *instance = _font_instances; instance; instance = instance->next)
    {
        if (instance->source == source && instance->size_px == size_px)
            return instance->font;
    }

    return NULL;
}

static _eos_font_instance_t *_create_font_instance(_eos_font_source_t source, int32_t size_px, lv_font_t *font)
{
    _eos_font_instance_t *instance = eos_malloc(sizeof(*instance));
    if (!instance)
        return NULL;

    instance->source = source;
    instance->size_px = size_px;
    instance->font = font;
    instance->next = NULL;
    return instance;
}

static void _store_font_instance(_eos_font_instance_t *instance)
{
    instance->next = _font_instances;
    _font_instances = instance;
}

static lv_font_t *_create_ttf_font(_eos_font_source_t source, int32_t size_px)
{
    lv_font_t *font = NULL;

    if (source == _EOS_FONT_SOURCE_ICON)
    {
#if LV_TINY_TTF_FILE_SUPPORT != 0
#if EOS_FONT_TTF_ENABLE_EXTENDED
        font = lv_tiny_ttf_create_file_ex(_icon_font_path, size_px, EOS_FONT_TTF_KERNING, EOS_FONT_TTF_CACHE_SIZE);
#else
        font = lv_tiny_ttf_create_file(_icon_font_path, size_px);
#endif
#endif
        if (!font)
            EOS_LOG_E("Unable to load RemixIcon TTF at %s (%ldpx)", _icon_font_path, (long)size_px);
        else
            font->fallback = (lv_font_t *)EOS_FONT_TTF_FALLBACK_PTR;
        return font;
    }

#if EOS_FONT_TTF_TYPE == EOS_FONT_TTF_DATA
#if EOS_FONT_TTF_ENABLE_EXTENDED
    font = lv_tiny_ttf_create_data_ex(EOS_FONT_TTF_DATA_NAME,
                                      EOS_FONT_TTF_DATA_SIZE,
                                      size_px,
                                      EOS_FONT_TTF_KERNING,
                                      EOS_FONT_TTF_CACHE_SIZE);
#else
    font = lv_tiny_ttf_create_data(EOS_FONT_TTF_DATA_NAME, EOS_FONT_TTF_DATA_SIZE, size_px);
#endif
#elif EOS_FONT_TTF_TYPE == EOS_FONT_TTF_FILE
#if EOS_FONT_TTF_ENABLE_EXTENDED
    font = lv_tiny_ttf_create_file_ex(_text_font_path, size_px, EOS_FONT_TTF_KERNING, EOS_FONT_TTF_CACHE_SIZE);
#else
    font = lv_tiny_ttf_create_file(_text_font_path, size_px);
#endif
#endif

    if (!font)
    {
#if EOS_FONT_TTF_TYPE == EOS_FONT_TTF_FILE
        EOS_LOG_E("Unable to load text TTF at %s (%ldpx)", _text_font_path, (long)size_px);
#else
        EOS_LOG_E("Unable to load text TTF data (%ldpx)", (long)size_px);
#endif
    }
    return font;
}

static eos_result_t _get_or_create_icon_font(int32_t size_px, lv_font_t **out_font)
{
    lv_font_t *font;
    _eos_font_instance_t *instance;

    if (!out_font)
        return EOS_ERR_VAR_NULL;
    *out_font = NULL;

    if (size_px < EOS_FONT_MIN_SIZE_PX || size_px > EOS_FONT_MAX_SIZE_PX)
        return EOS_ERR_INVALID_ARG;
    if (!_font_inited)
        return EOS_ERR_NOT_INITIALIZED;
    if (_using_builtin_fallback)
        return _font_init_error;

    font = _find_font(_EOS_FONT_SOURCE_ICON, size_px);
    if (font)
    {
        *out_font = font;
        return EOS_OK;
    }

    font = _create_ttf_font(_EOS_FONT_SOURCE_ICON, size_px);
    if (!font)
        return EOS_ERR_FILE_ERROR;

    instance = _create_font_instance(_EOS_FONT_SOURCE_ICON, size_px, font);
    if (!instance)
    {
        lv_tiny_ttf_destroy(font);
        return EOS_ERR_MEM;
    }

    _store_font_instance(instance);
    *out_font = font;
    return EOS_OK;
}

static eos_result_t _get_or_create_text_font(int32_t size_px, lv_font_t **out_font)
{
    lv_font_t *text_font;
    lv_font_t *icon_font;
    _eos_font_instance_t *icon_instance = NULL;
    _eos_font_instance_t *text_instance;
    bool create_icon;

    if (!out_font)
        return EOS_ERR_VAR_NULL;
    *out_font = NULL;

    if (size_px < EOS_FONT_MIN_SIZE_PX || size_px > EOS_FONT_MAX_SIZE_PX)
        return EOS_ERR_INVALID_ARG;
    if (!_font_inited)
        return EOS_ERR_NOT_INITIALIZED;
    if (_using_builtin_fallback)
        return _font_init_error;

    text_font = _find_font(_EOS_FONT_SOURCE_TEXT, size_px);
    if (text_font)
    {
        *out_font = text_font;
        return EOS_OK;
    }

    icon_font = _find_font(_EOS_FONT_SOURCE_ICON, size_px);
    create_icon = icon_font == NULL;
    if (create_icon)
    {
        icon_font = _create_ttf_font(_EOS_FONT_SOURCE_ICON, size_px);
        if (!icon_font)
            return EOS_ERR_FILE_ERROR;
    }

    text_font = _create_ttf_font(_EOS_FONT_SOURCE_TEXT, size_px);
    if (!text_font)
    {
        if (create_icon)
            lv_tiny_ttf_destroy(icon_font);
        return EOS_ERR_FILE_ERROR;
    }

    /* Allocate every manager record before publishing either half of the font pair. */
    if (create_icon)
    {
        icon_instance = _create_font_instance(_EOS_FONT_SOURCE_ICON, size_px, icon_font);
        if (!icon_instance)
        {
            lv_tiny_ttf_destroy(text_font);
            lv_tiny_ttf_destroy(icon_font);
            return EOS_ERR_MEM;
        }
    }
    text_instance = _create_font_instance(_EOS_FONT_SOURCE_TEXT, size_px, text_font);
    if (!text_instance)
    {
        if (icon_instance)
            eos_free(icon_instance);
        lv_tiny_ttf_destroy(text_font);
        if (create_icon)
            lv_tiny_ttf_destroy(icon_font);
        return EOS_ERR_MEM;
    }

    /* LVGL resolves missing text glyphs through its same-size RemixIcon font. */
    text_font->fallback = icon_font;
    if (icon_instance)
        _store_font_instance(icon_instance);
    _store_font_instance(text_instance);

    *out_font = text_font;
    return EOS_OK;
}

lv_font_t *eos_font_init(void)
{
    lv_font_t *default_font;

    if (_font_inited)
    {
        if (_using_builtin_fallback)
            return (lv_font_t *)EOS_FONT_TTF_FALLBACK_PTR;
        return _find_font(_EOS_FONT_SOURCE_TEXT, EOS_FONT_DEFAULT_SIZE_PX);
    }

    _font_instances = NULL;
    _font_inited = true;
    _font_init_error = _get_or_create_text_font(EOS_FONT_DEFAULT_SIZE_PX, &default_font);
    if (_font_init_error != EOS_OK)
    {
        _using_builtin_fallback = true;
        EOS_LOG_E("Dual TTF font initialization failed (%ld); using rescue font", (long)_font_init_error);
        return (lv_font_t *)EOS_FONT_TTF_FALLBACK_PTR;
    }

    _using_builtin_fallback = false;
    return default_font;
}

void eos_font_deinit(void)
{
    if (!_font_inited)
        return;

    /* Fonts may be referenced by objects, displays, and theme until teardown completes. */
    if (!lv_is_initialized() || lv_display_get_next(NULL) != NULL)
    {
        EOS_LOG_W("Font deinit deferred until LVGL displays and theme are released");
        return;
    }
#if LV_USE_THEME_DEFAULT
    if (lv_theme_default_get() != NULL)
    {
        EOS_LOG_W("Font deinit deferred until the default theme is released");
        return;
    }
#endif

    while (_font_instances)
    {
        _eos_font_instance_t *instance = _font_instances;
        _font_instances = instance->next;
        lv_tiny_ttf_destroy(instance->font);
        eos_free(instance);
    }
    _using_builtin_fallback = false;
    _font_init_error = EOS_OK;
    _font_inited = false;
}

lv_font_t *eos_font_reload(const char *path)
{
    LV_UNUSED(path);
    EOS_LOG_W("Runtime font reload is disabled in v0.7");
    return NULL;
}

eos_result_t eos_font_set_size(lv_obj_t *obj, int32_t size_px)
{
    lv_font_t *font;
    eos_result_t result;

    if (!obj)
        return EOS_ERR_VAR_NULL;
    if (!_font_inited)
        return EOS_ERR_NOT_INITIALIZED;
    if (!lv_obj_is_valid(obj) || size_px < EOS_FONT_MIN_SIZE_PX || size_px > EOS_FONT_MAX_SIZE_PX)
        return EOS_ERR_INVALID_ARG;
    if (_using_builtin_fallback)
        return _font_init_error;

    result = _get_or_create_text_font(size_px, &font);
    if (result != EOS_OK)
        return result;

    lv_obj_set_style_text_font(obj, font, LV_PART_MAIN);
    if (!lv_obj_is_valid(obj))
        return EOS_ERR_INVALID_STATE;

    lv_style_value_t actual;
    if (lv_obj_get_local_style_prop(obj, LV_STYLE_TEXT_FONT, &actual, LV_PART_MAIN) != LV_STYLE_RES_FOUND
        || actual.ptr != font)
        return EOS_ERR_MEM;
    return EOS_OK;
}

eos_result_t eos_font_clear_size(lv_obj_t *obj)
{
    if (!obj)
        return EOS_ERR_VAR_NULL;
    if (!lv_obj_is_valid(obj))
        return EOS_ERR_INVALID_ARG;

    lv_obj_remove_local_style_prop(obj, LV_STYLE_TEXT_FONT, LV_PART_MAIN);
    if (!lv_obj_is_valid(obj))
        return EOS_ERR_INVALID_STATE;
    lv_style_value_t actual;
    if (lv_obj_get_local_style_prop(obj, LV_STYLE_TEXT_FONT, &actual, LV_PART_MAIN) == LV_STYLE_RES_FOUND)
        return EOS_FAILED;
    return EOS_OK;
}

eos_result_t eos_font_get_icon_font(int32_t size_px, lv_font_t **out_font)
{
    return _get_or_create_icon_font(size_px, out_font);
}

void eos_label_set_font_size(lv_obj_t *label, eos_font_size_t size)
{
    eos_result_t result = eos_font_set_size(label, (int32_t)size);
    if (result != EOS_OK)
        EOS_LOG_W("Failed to set font size %ld: %ld", (long)size, (long)result);
}
