/**
 * @file eos_pkg_codec.c
 * @brief EPKG codec and checksum helpers
 */

#include "eos_pkg_codec.h"

/* Includes ---------------------------------------------------*/
#include "lvgl.h"
#include "libs/lz4/lz4.h"

/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

uint32_t eos_pkg_crc32_update(uint32_t state, const void *data, size_t size)
{
    const uint8_t *bytes = (const uint8_t *)data;

    if (!data && size != 0U)
    {
        return state;
    }

    for (size_t index = 0U; index < size; index++)
    {
        state ^= bytes[index];
        for (uint32_t bit = 0U; bit < 8U; bit++)
        {
            state = (state & 1U) ? ((state >> 1U) ^ 0xEDB88320U) : (state >> 1U);
        }
    }
    return state;
}

uint32_t eos_pkg_crc32_finish(uint32_t state)
{
    return state ^ 0xFFFFFFFFU;
}

uint32_t eos_pkg_crc32(const void *data, size_t size)
{
    return eos_pkg_crc32_finish(eos_pkg_crc32_update(EOS_PKG_CRC32_INITIAL, data, size));
}

int eos_pkg_lz4_decompress_block(const uint8_t *compressed, size_t compressed_size, uint8_t *raw, size_t raw_capacity)
{
#if defined(LV_USE_LZ4_INTERNAL) && LV_USE_LZ4_INTERNAL
    if (!compressed || !raw || compressed_size > (size_t)INT32_MAX || raw_capacity > (size_t)INT32_MAX)
    {
        return -1;
    }

    return LZ4_decompress_safe((const char *)compressed, (char *)raw, (int)compressed_size, (int)raw_capacity);
#else
    LV_UNUSED(compressed);
    LV_UNUSED(compressed_size);
    LV_UNUSED(raw);
    LV_UNUSED(raw_capacity);
    return -1;
#endif
}
