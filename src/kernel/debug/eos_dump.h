/**
 * @file eos_dump.h
 * @brief Fatal-error diagnostic dump interface
 */

#ifndef EOS_DUMP_H
#define EOS_DUMP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stddef.h>
#include <stdint.h>

/* Public macros ----------------------------------------------*/
#define EOS_DUMP_MAX_REGISTERS 32U
#define EOS_DUMP_MAX_FAULT_STATUS 8U
#define EOS_DUMP_MAX_BACKTRACE 32U
#define EOS_DUMP_MAX_STACK_WORDS 32U

/* Public typedefs --------------------------------------------*/

/**
 * @brief Register and stack state collected by a platform port
 */
typedef struct
{
    uintptr_t pc;
    uintptr_t sp;
    uintptr_t lr;
    uintptr_t fp;
    uintptr_t registers[EOS_DUMP_MAX_REGISTERS];
    const char *register_names[EOS_DUMP_MAX_REGISTERS];
    size_t register_count;
    uintptr_t fault_status[EOS_DUMP_MAX_FAULT_STATUS];
    const char *fault_status_names[EOS_DUMP_MAX_FAULT_STATUS];
    size_t fault_status_count;
    uintptr_t backtrace[EOS_DUMP_MAX_BACKTRACE];
    size_t backtrace_count;
    uintptr_t stack_address;
    uintptr_t stack_words[EOS_DUMP_MAX_STACK_WORDS];
    size_t stack_word_count;
} eos_dump_context_t;

/* Public function prototypes ---------------------------------*/

/**
 * @brief Emit a best-effort dump of the current execution context
 * @param reason Human-readable crash or fatal-error reason
 */
void eos_dump(const char *reason);

/**
 * @brief Emit a dump using an exception context supplied by a platform
 * @param reason Human-readable crash or fatal-error reason
 * @param context Register and stack context, or NULL for no context
 */
void eos_dump_context(const char *reason, const eos_dump_context_t *context);

/**
 * @brief Emit an assertion-failure dump
 * @param file Source file containing the assertion
 * @param line Source line containing the assertion
 * @param expression Failed assertion expression
 */
void eos_dump_assert(const char *file, int line, const char *expression);

#ifdef __cplusplus
}
#endif

#endif /* EOS_DUMP_H */
