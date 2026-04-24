/**
 * @file elena_os_fs.h
 * @brief File system
 * @author Sab1e
 * @date 2025-11-18
 */

#ifndef ELENA_OS_FS_H
#define ELENA_OS_FS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "elena_os_port.h"
/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/

/**
 * @brief Check if target path is a directory
 */
bool eos_is_dir(const char *path);

/**
 * @brief Check if target path is a file
 */
bool eos_is_file(const char *path);

/**
 * @brief Write string to file (automatically appends newline at the end)
 * @param s String to write
 * @param fp File pointer (opened by eos_fs_open_write or eos_fs_open_readwrite)
 * @return int Number of bytes written, or error code (negative value)
 */
int eos_fs_puts(const char *s, eos_file_t fp);

/**
 * @brief Check if directory exists, create if it doesn't
 * @param path Directory path
 * @return int 0 indicates success; otherwise returns error code
 */
int eos_fs_mkdir_if_not_exist(const char *path);

/**
 * @brief Create file and write default content if file doesn't exist
 * @param path File path
 * @param default_content Default content to write when file doesn't exist, can be NULL (create empty file)
 * @return int 0 indicates success; otherwise returns error code
 */
int eos_create_file_if_not_exist(const char *path, const char *default_content);

/**
 * @brief Read entire file content into dynamically allocated buffer (can only read text files)
 * @param path File path
 * @return char* Returns dynamically allocated buffer; returns NULL on failure
 * @warning Must use `eos_free` to release memory after using the data
 */
char *eos_fs_read_file(const char *path);

/**
 * @brief Write to file
 * @param path File path
 * @param data Data pointer
 * @param data_size Data length
 * @return int Actual number of bytes written, returns -1 on error
 */
int eos_fs_write_file(const char *path, const void *data, size_t data_size);

/**
 * @brief Recursively create directory tree
 * @param path Target path, e.g. a/b/c
 * @return int 0 indicates all created successfully; otherwise returns error code
 */
int eos_fs_mkdir_recursive(const char *path);

/**
 * @brief Recursively delete directory and all files and subdirectories inside
 * @param path Directory path
 * @return int 0 indicates success; otherwise returns error code
 */
int eos_fs_rm_recursive(const char *path);

#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_FS_H */