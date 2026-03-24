/**
 * @file elena_os_test.h
 * @brief 系统功能测试
 * @author Sab1e
 * @date 2025-08-20
 */

#include "elena_os_config.h"
#if EOS_SYSTEM_MODE == TEST_MODE

#ifndef ELENA_OS_TEST_H
#define ELENA_OS_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/
/**
 * @brief 开始系统功能测试
 */
void eos_test_start(void);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_TEST_H */

#endif /* EOS_SYSTEM_MODE == TEST_MODE */
