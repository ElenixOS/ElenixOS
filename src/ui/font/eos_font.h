/**
 * @file eos_font.h
 * @brief Font system
 */

#ifndef EOS_FONT_H
#define EOS_FONT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"
#include "eos_config.h"
#include "eos_error.h"
#include "eos_log.h"

/* Public macros ----------------------------------------------*/
#define EOS_FONT_DATA_DECLARE(font_name) LV_ATTRIBUTE_EXTERN_DATA extern const unsigned char font_name[]
#define EOS_FONT_DATA_SIZE_DECLARE(font_size) LV_ATTRIBUTE_EXTERN_DATA extern const int font_size
/** @brief Smallest supported TinyTTF text size in pixels. */
#define EOS_FONT_MIN_SIZE_PX 8
/** @brief Largest supported TinyTTF text size in pixels. */
#define EOS_FONT_MAX_SIZE_PX 64
/** @brief System text size in pixels. */
#define EOS_FONT_DEFAULT_SIZE_PX 26
/* Public typedefs --------------------------------------------*/

typedef enum
{
    EOS_FONT_SIZE_LARGE = EOS_FONT_CFG_LARGE_SIZE,
    EOS_FONT_SIZE_MEDIUM = EOS_FONT_CFG_MEDIUM_SIZE,
    EOS_FONT_SIZE_SMALL = EOS_FONT_CFG_SMALL_SIZE,
} eos_font_size_t;

/* Public function prototypes ---------------------------------*/

/**
 * @brief Font system initialization
 * @return lv_font_t* Returns default font pointer on success, otherwise returns NULL
 */
lv_font_t *eos_font_init(void);
/**
 * @brief Release font instances after all LVGL displays and themes are gone
 * @note Must be called while LVGL's allocator is still initialized.
 */
void eos_font_deinit(void);
/**
 * @brief Runtime font reload is disabled in v0.7
 * @param path Unused font path
 * @return NULL; no font references are changed
 */
lv_font_t *eos_font_reload(const char *path);
/**
 * @brief Set an exact TinyTTF pixel size on an LVGL object's main part
 * @param obj Target LVGL object
 * @param size_px Exact size in pixels
 * @return EOS_OK or an error; a failed call leaves the local font unchanged
 */
eos_result_t eos_font_set_size(lv_obj_t *obj, int32_t size_px);
/**
 * @brief Remove the local main-part text font so LVGL inherits it normally
 * @param obj Target LVGL object
 * @return EOS_OK or an error
 */
eos_result_t eos_font_clear_size(lv_obj_t *obj);
/**
 * @brief Get the resident RemixIcon TinyTTF instance for an exact pixel size
 * @param size_px Exact size in pixels
 * @param out_font Receives the borrowed font pointer; it must not be destroyed
 * @return EOS_OK or an error; the pointer remains valid until font system deinit
 * @note This C-only helper is not exposed through SNI or to JavaScript.
 */
eos_result_t eos_font_get_icon_font(int32_t size_px, lv_font_t **out_font);
/**
 * @brief Compatibility wrapper for existing C callers
 * @param label Target LVGL object
 * @param size Exact size in pixels, including legacy size constants
 */
void eos_label_set_font_size(lv_obj_t *label, eos_font_size_t size);
#ifdef __cplusplus
}
#endif

#endif /* EOS_FONT_H */
