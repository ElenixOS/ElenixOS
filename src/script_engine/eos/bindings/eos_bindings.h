/**
 * @file eos_bindings.h
 * @brief EOS 绑定
 * @author Sab1e
 * @date 2025-12-27
 */

#ifndef EOS_BINDINGS_H
#define EOS_BINDINGS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "jerryscript.h"

/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/

/**
 * @brief 初始化绑定层
 * @param parent 父对象
 */
void eos_binding_init(jerry_value_t parent);
#ifdef __cplusplus
}
#endif

#endif /* EOS_BINDINGS_H */
