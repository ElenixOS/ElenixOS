/**
 * @file eos_ww_analog_dial.h
 * @brief Complete analog watchface dial
 */

#ifndef EOS_WW_ANALOG_DIAL_H
#define EOS_WW_ANALOG_DIAL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include "lvgl.h"

/* Public macros ----------------------------------------------*/
#define EOS_WW_ANALOG_DIAL_HAND_COUNT 3U
#define EOS_WW_ANALOG_DIAL_IMAGE_SRC_MAX 256U

/* Public typedefs --------------------------------------------*/
typedef enum
{
    EOS_WW_ANALOG_HAND_LINE = 0,
    EOS_WW_ANALOG_HAND_BAR,
    EOS_WW_ANALOG_HAND_NEEDLE,
    EOS_WW_ANALOG_HAND_DIAMOND,
    EOS_WW_ANALOG_HAND_ARROW,
    EOS_WW_ANALOG_HAND_IMAGE,
} eos_ww_analog_hand_type_t;

typedef enum
{
    EOS_WW_ANALOG_HAND_HOUR = 0,
    EOS_WW_ANALOG_HAND_MINUTE,
    EOS_WW_ANALOG_HAND_SECOND,
} eos_ww_analog_hand_index_t;

typedef struct
{
    bool enabled;
    eos_ww_analog_hand_type_t type;
    lv_coord_t length;
    lv_coord_t width;
    lv_coord_t tail_length;
    lv_color_t color;
    lv_opa_t opacity;
    int32_t angle_offset;
    lv_coord_t offset_x;
    lv_coord_t offset_y;
    float pivot_x;
    float pivot_y;
    bool pivot_set;
    char image_src[EOS_WW_ANALOG_DIAL_IMAGE_SRC_MAX];
} eos_ww_analog_hand_config_t;

typedef struct
{
    uint16_t count;
    lv_coord_t length;
    lv_coord_t width;
    lv_color_t color;
    lv_opa_t opacity;
} eos_ww_analog_tick_config_t;

typedef struct
{
    bool enabled;
    lv_coord_t radius;
    lv_color_t color;
    lv_opa_t opacity;
} eos_ww_analog_center_cap_config_t;

typedef struct
{
    lv_coord_t width;
    lv_coord_t height;
    eos_ww_analog_tick_config_t major_ticks;
    eos_ww_analog_tick_config_t minor_ticks;
    eos_ww_analog_hand_config_t hands[EOS_WW_ANALOG_DIAL_HAND_COUNT];
    eos_ww_analog_center_cap_config_t center_cap;
    bool numerals;
    bool smooth_second;
} eos_ww_analog_dial_config_t;

typedef struct eos_ww_analog_dial_t eos_ww_analog_dial_t;

typedef struct
{
    eos_ww_analog_dial_t *owner;
    uint8_t index;
} eos_ww_analog_dial_hand_handle_t;

/* Public function prototypes ---------------------------------*/

/**
 * @brief Create an analog watchface dial.
 * @param parent Parent LVGL object.
 * @param config Dial configuration.
 * @return Root LVGL object, or NULL on failure.
 */
lv_obj_t *eos_ww_analog_dial_create(lv_obj_t *parent, const eos_ww_analog_dial_config_t *config);

/**
 * @brief Get the embedded runtime handle for one of the dial's hands.
 * @param root Dial root LVGL object.
 * @param index Hand index.
 * @return Embedded hand handle, or NULL when the dial is invalid.
 */
eos_ww_analog_dial_hand_handle_t *eos_ww_analog_dial_get_hand(lv_obj_t *root, eos_ww_analog_hand_index_t index);

/**
 * @brief Set whether a hand is rendered.
 * @param handle Hand handle.
 * @param enabled Visibility state.
 * @return true on success, otherwise false.
 */
bool eos_ww_analog_dial_hand_set_enabled(eos_ww_analog_dial_hand_handle_t *handle, bool enabled);

/**
 * @brief Set the hand primitive or image renderer type.
 * @param handle Hand handle.
 * @param type Renderer type.
 * @return true on success, otherwise false.
 */
bool eos_ww_analog_dial_hand_set_type(eos_ww_analog_dial_hand_handle_t *handle, eos_ww_analog_hand_type_t type);

/**
 * @brief Set the primitive hand color.
 * @param handle Hand handle.
 * @param color RGB color value.
 * @return true on success, otherwise false.
 */
bool eos_ww_analog_dial_hand_set_color(eos_ww_analog_dial_hand_handle_t *handle, uint32_t color);

/**
 * @brief Set the hand opacity.
 * @param handle Hand handle.
 * @param opacity Opacity in the range 0..255.
 * @return true on success, otherwise false.
 */
bool eos_ww_analog_dial_hand_set_opacity(eos_ww_analog_dial_hand_handle_t *handle, uint8_t opacity);

/**
 * @brief Set the forward hand length.
 * @param handle Hand handle.
 * @param length Length in pixels.
 * @return true on success, otherwise false.
 */
bool eos_ww_analog_dial_hand_set_length(eos_ww_analog_dial_hand_handle_t *handle, lv_coord_t length);

/**
 * @brief Set the hand width.
 * @param handle Hand handle.
 * @param width Width in pixels.
 * @return true on success, otherwise false.
 */
bool eos_ww_analog_dial_hand_set_width(eos_ww_analog_dial_hand_handle_t *handle, lv_coord_t width);

/**
 * @brief Set the true tail length behind the pivot.
 * @param handle Hand handle.
 * @param tail_length Tail length in pixels.
 * @return true on success, otherwise false.
 */
bool eos_ww_analog_dial_hand_set_tail_length(eos_ww_analog_dial_hand_handle_t *handle, lv_coord_t tail_length);

/**
 * @brief Set the hand angle offset in analog degrees.
 * @param handle Hand handle.
 * @param angle_offset Offset in degrees.
 * @return true on success, otherwise false.
 */
bool eos_ww_analog_dial_hand_set_angle_offset(eos_ww_analog_dial_hand_handle_t *handle, int32_t angle_offset);

/**
 * @brief Set the rotation-center offset relative to the dial center.
 * @param handle Hand handle.
 * @param offset_x Horizontal offset in pixels.
 * @param offset_y Vertical offset in pixels.
 * @return true on success, otherwise false.
 */
bool eos_ww_analog_dial_hand_set_offset(eos_ww_analog_dial_hand_handle_t *handle,
                                        lv_coord_t offset_x,
                                        lv_coord_t offset_y);

/**
 * @brief Set a normalized image pivot.
 * @param handle Hand handle.
 * @param x Normalized horizontal pivot in the range 0..1.
 * @param y Normalized vertical pivot in the range 0..1.
 * @return true on success, otherwise false.
 */
bool eos_ww_analog_dial_hand_set_pivot(eos_ww_analog_dial_hand_handle_t *handle, float x, float y);

/**
 * @brief Set or clear the image source.
 * @param handle Hand handle.
 * @param src Image path, or NULL to clear it.
 * @return true on success, otherwise false.
 */
bool eos_ww_analog_dial_hand_set_image(eos_ww_analog_dial_hand_handle_t *handle, const char *src);

#ifdef __cplusplus
}
#endif

#endif /* EOS_WW_ANALOG_DIAL_H */
