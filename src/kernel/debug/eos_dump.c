/**
 * @file eos_dump.c
 * @brief Fatal-error diagnostic dump implementation
 */

#include "eos_dump.h"

/* Includes ---------------------------------------------------*/
#include <inttypes.h>
#define EOS_LOG_TAG "Dump"
#include "eos_log.h"
#include "eos_port.h"

/* Macros and Definitions -------------------------------------*/
/* Variables --------------------------------------------------*/
static volatile bool _dump_in_progress;

/* Function Implementations -----------------------------------*/
void eos_dump_context(const char *reason, const eos_dump_context_t *context)
{
    size_t index;

    if (_dump_in_progress)
    {
        return;
    }
    _dump_in_progress = true;

    EOS_LOG_E("========== FATAL DUMP BEGIN ==========");
    EOS_LOG_E("reason: %s", reason ? reason : "unspecified");

    if (!context)
    {
        EOS_LOG_E("register context: unavailable");
        EOS_LOG_E("========== FATAL DUMP END ==========");
        _dump_in_progress = false;
        return;
    }

    EOS_LOG_E("PC=0x%" PRIxPTR " SP=0x%" PRIxPTR " LR=0x%" PRIxPTR " FP=0x%" PRIxPTR,
              context->pc,
              context->sp,
              context->lr,
              context->fp);

    for (index = 0U; index < context->register_count && index < EOS_DUMP_MAX_REGISTERS; index++)
    {
        EOS_LOG_E("REG[%s]=0x%" PRIxPTR,
                  context->register_names[index] ? context->register_names[index] : "?",
                  context->registers[index]);
    }

    for (index = 0U; index < context->fault_status_count && index < EOS_DUMP_MAX_FAULT_STATUS; index++)
    {
        EOS_LOG_E("FAULT[%s]=0x%" PRIxPTR,
                  context->fault_status_names[index] ? context->fault_status_names[index] : "?",
                  context->fault_status[index]);
    }

    if (context->backtrace_count == 0U)
    {
        EOS_LOG_E("backtrace: unavailable");
    }
    else
    {
        for (index = 0U; index < context->backtrace_count && index < EOS_DUMP_MAX_BACKTRACE; index++)
        {
            EOS_LOG_E("BT[%02zu]=0x%" PRIxPTR, index, context->backtrace[index]);
        }
    }

    if (context->stack_word_count == 0U)
    {
        EOS_LOG_E("stack: unavailable");
    }
    else
    {
        EOS_LOG_E("stack words at 0x%" PRIxPTR ":", context->stack_address);
        for (index = 0U; index < context->stack_word_count && index < EOS_DUMP_MAX_STACK_WORDS; index++)
        {
            EOS_LOG_E("STACK[%02zu] 0x%" PRIxPTR ": 0x%" PRIxPTR,
                      index,
                      context->stack_address + index * sizeof(uintptr_t),
                      context->stack_words[index]);
        }
    }

    EOS_LOG_E("========== FATAL DUMP END ==========");
    _dump_in_progress = false;
}

void eos_dump(const char *reason)
{
    eos_dump_context_t context = {0};

    eos_port_dump_capture(&context);
    eos_dump_context(reason, &context);
}

void eos_dump_assert(const char *file, int line, const char *expression)
{
    EOS_LOG_E("assertion location: %s:%d (%s)", file ? file : "?", line, expression ? expression : "?");
    eos_dump("assertion failure");
}
