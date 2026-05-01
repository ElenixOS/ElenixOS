/**
 * @file eos_service_storage.h
 * @brief Storage service file operation APIs
 */

#ifndef EOS_SERVICE_STORAGE_H
#define EOS_SERVICE_STORAGE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "eos_port.h"
#include "eos_error.h"
/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/

/**
 * @brief Check if target path is a directory
 */
bool eos_storage_is_dir(const char *path);

/**
 * @brief Check if target path is a file
 */
bool eos_storage_is_file(const char *path);

/**
 * @brief Write string to file (automatically appends newline at the end)
 * @param s String to write
 * @param fp File pointer (opened by eos_fs_open_write or eos_fs_open_readwrite)
 * @return eos_result_t EOS_OK on success, negative error code on failure
 */
eos_result_t eos_storage_puts(const char *s, eos_file_t fp);

/**
 * @brief Check if directory exists, create if it doesn't
 * @param path Directory path
 * @return eos_result_t EOS_OK on success, negative error code on failure
 */
eos_result_t eos_storage_mkdir_if_not_exist(const char *path);

/**
 * @brief Create file and write default content if file doesn't exist
 * @param path File path
 * @param default_content Default content to write when file doesn't exist, can be NULL (create empty file)
 * @return eos_result_t EOS_OK on success, negative error code on failure
 */
eos_result_t eos_storage_create_file_if_not_exist(const char *path, const char *default_content);

/**
 * @brief Read entire file content into dynamically allocated buffer (can only read text files)
 * @param path File path
 * @return char* Returns dynamically allocated buffer; returns NULL on failure
 * @warning Must use `eos_free` to release memory after using the data
 */
char *eos_storage_read_file(const char *path);

/**
 * @brief Write to file
 * @param path File path
 * @param data Data pointer
 * @param data_size Data length
 * @return eos_result_t EOS_OK on success, negative error code on failure
 */
eos_result_t eos_storage_write_file(const char *path, const void *data, size_t data_size);

/**
 * @brief Write to file immediately (bypass deferred writer)
 * @param path File path
 * @param data Data pointer
 * @param data_size Data length
 * @return eos_result_t EOS_OK on success, negative error code on failure
 */
eos_result_t eos_storage_write_file_immediate(const char *path, const void *data, size_t data_size);

/**
 * @brief Read entire file content immediately (bypass deferred writer)
 * @param path File path
 * @return char* Returns dynamically allocated buffer; returns NULL on failure
 * @warning Must use `eos_free` to release memory after using the data
 */
char *eos_storage_read_file_immediate(const char *path);

/**
 * @brief Recursively create directory tree
 * @param path Target path, e.g. a/b/c
 * @return eos_result_t EOS_OK on success, negative error code on failure
 */
eos_result_t eos_storage_mkdir_recursive(const char *path);

/**
 * @brief Recursively delete directory and all files and subdirectories inside
 * @param path Directory path
 * @return eos_result_t EOS_OK on success, negative error code on failure
 */
eos_result_t eos_storage_rm_recursive(const char *path);

#ifdef __cplusplus
}
#endif

#endif /* EOS_SERVICE_STORAGE_H */
