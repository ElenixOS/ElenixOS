/**
 * @file eos_test.h
 * @brief System function test
 */

#include "eos_config.h"
#if EOS_SYSTEM_MODE == TEST_MODE

#ifndef ELENIX_OS_TEST_H
#define ELENIX_OS_TEST_H

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
 * @brief Start system function test
 */
void eos_test_start(void);
#ifdef __cplusplus
}
#endif

#endif /* ELENIX_OS_TEST_H */

#endif /* EOS_SYSTEM_MODE == TEST_MODE */
