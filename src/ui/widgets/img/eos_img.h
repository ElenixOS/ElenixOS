/**
 * @file eos_img.h
 * @brief Image display
 */

#ifndef EOS_IMG_H
#define EOS_IMG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"
#include "eos_sys.h"
/* Public macros ----------------------------------------------*/
/************************** System Built-in Image Resource Definitions **************************/
#define EOS_IMG_APP EOS_SYS_RES_IMG_DIR "app.bin"
#define EOS_IMG_SETTINGS EOS_SYS_RES_IMG_DIR "settings.bin"
#define EOS_IMG_APP_HEADER_BG EOS_SYS_RES_IMG_DIR "app_header.bin"
#define EOS_IMG_FLASH_LIGHT EOS_SYS_RES_IMG_DIR "flash_light.bin"
#define EOS_IMG_WATCHFACE EOS_SYS_RES_IMG_DIR "watchface.bin"
#define EOS_IMG_LOGO EOS_SYS_RES_IMG_DIR "logo.bin"
/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/

/**
 * @brief Scale image to specified resolution
 * @param img_obj Target image object
 * @param w Target width (px)
 * @param h Target height (px)
 */
void eos_img_set_size(lv_obj_t *img_obj, const uint32_t w, const uint32_t h);
/**
 * @brief Open image from Flash, load to memory, and set lvgl image source.
 * @param img_obj Image object to set image source
 * @param bin_path Path to bin file
 * @warning Only supports LVGL bin files
 * @note Memory is automatically released when lv_img_t object is deleted
 */
void eos_img_set_src(lv_obj_t *img_obj, const char *bin_path);
#ifdef __cplusplus
}
#endif

#endif /* EOS_IMG_H */
