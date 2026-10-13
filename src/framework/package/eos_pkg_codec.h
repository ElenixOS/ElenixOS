/**
 * @file eos_pkg_codec.h
 * @brief EPKG codec and checksum helpers
 */

#ifndef EOS_PKG_CODEC_H
#define EOS_PKG_CODEC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stddef.h>
#include <stdint.h>

/* Public macros ----------------------------------------------*/
#define EOS_PKG_CRC32_INITIAL 0xFFFFFFFFU

/* Public function prototypes ---------------------------------*/
uint32_t eos_pkg_crc32_update(uint32_t state, const void *data, size_t size);
uint32_t eos_pkg_crc32_finish(uint32_t state);
uint32_t eos_pkg_crc32(const void *data, size_t size);

/**
 * @brief Decode one raw LZ4 block.
 * @return Decompressed byte count, or a negative value on failure/unavailable
 *         codec support.
 */
int eos_pkg_lz4_decompress_block(const uint8_t *compressed, size_t compressed_size, uint8_t *raw, size_t raw_capacity);

#ifdef __cplusplus
}
#endif

#endif /* EOS_PKG_CODEC_H */
