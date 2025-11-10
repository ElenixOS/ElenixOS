/**
 * @file elena_os_config_internal.h
 * @brief 配置文件内部定义
 * @author Sab1e
 * @date 2025-10-08
 */

#ifndef ELENA_OS_CONFIG_INTERNAL_H
#define ELENA_OS_CONFIG_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

/* Public macros ----------------------------------------------*/

#define DEBUG               0
#define RELEASE             1

#define EOS_FONT_C_SCALE    0
#define EOS_FONT_C_MULTI    1
#define EOS_FONT_TTF        2

#define EOS_FONT_TTF_DATA   0
#define EOS_FONT_TTF_FILE   1

/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_CONFIG_INTERNAL_H */
