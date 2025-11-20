/**
 * @file elena_os_afw.c
 * @brief 异步文件写入器（Async File Writer）
 * @author Sab1e
 * @date 2025-11-20
 */

#include "elena_os_afw.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "elena_os_config.h"
#define EOS_LOG_TAG "AFW"
#include "elena_os_log.h"
#include "elena_os_port.h"
#include "elena_os_misc.h"
/* Macros and Definitions -------------------------------------*/

typedef enum
{
    EOS_AFW_STATE_IDLE = 0,    /**< AFW 已经停止 */
    EOS_AFW_STATE_RUNNING = 1, /**< AFW 正在执行任务 */
} eos_afw_state_t;

typedef enum
{
    EOS_AFW_TASK_STATE_WAITING,       /**< 任务已经创建，等待执行 */
    EOS_AFW_TASK_STATE_CREATE_FILE,   /**< 任务正在创建临时文件 */
    EOS_AFW_TASK_STATE_WRITING_BLOCK, /**< 任务正在写入块 */
    EOS_AFW_TASK_STATE_RENAME_FILE,   /**< 任务正在重命名临时文件 */
    EOS_AFW_TASK_STATE_DONE,          /**< 任务写入完成 */
    EOS_AFW_TASK_STATE_ERROR,         /**< 任务运行时出现错误 */

} eos_afw_task_state_t;

typedef struct
{
    eos_afw_task_state_t state;
    void *data;
    size_t data_size;
    const char *path;
} eos_afw_task_t;

typedef struct
{
    eos_afw_state_t state;
    eos_afw_task_t *task_list[EOS_AFW_TASK_MAX];
    eos_afw_task_t *current_task;
    eos_file_t *current_fp;
    uint32_t current_offset;
    uint32_t todo_task;
    char tmp_path[PATH_MAX];
} eos_afw_t;

/* Variables --------------------------------------------------*/
static eos_afw_t afw = {0};
/* Function Implementations -----------------------------------*/

void _reset_afw(void)
{
    eos_afw_task_t *old_task = afw.current_task;

    if (afw.current_fp)
    {
        eos_fs_close(afw.current_fp);
        afw.current_fp = NULL;
    }

    afw.tmp_path[0] = '\0';
    afw.current_offset = 0;
    afw.state = EOS_AFW_STATE_IDLE;

    if (afw.todo_task > 0)
        afw.todo_task--;

    if (old_task)
    {
        for (int i = 0; i < EOS_AFW_TASK_MAX; i++)
        {
            if (afw.task_list[i] == old_task)
            {
                afw.task_list[i] = NULL;
                break;
            }
        }
        if (old_task->data)
            eos_free(old_task->data);
        if (old_task->path)
            eos_free(old_task->path);
        eos_free(old_task);
    }
    afw.current_task = NULL;
}

void eos_afw_handler(void)
{
    // 间隔指定的周期后开始写入一次文件
    static uint32_t count = 0;
    count++;
    if (count < EOS_AFW_SCHEDULE_INTERVAL)
        return;
    count = 0;
    // 开始执行写入任务

    // 查找空闲任务并启动
    if (afw.todo_task > 0 && afw.state == EOS_AFW_STATE_IDLE)
    {
        // 查找空闲任务
        for (int i = 0; i < EOS_AFW_TASK_MAX; i++)
        {
            if (afw.task_list[i] && afw.task_list[i]->state == EOS_AFW_TASK_STATE_WAITING)
            {
                afw.current_task = afw.task_list[i];
                afw.current_task->state = EOS_AFW_TASK_STATE_CREATE_FILE;
                afw.state = EOS_AFW_STATE_RUNNING;
                break;
            }
        }
    }
    if (afw.state == EOS_AFW_STATE_RUNNING)
    {
        switch (afw.current_task->state)
        {
        case EOS_AFW_TASK_STATE_CREATE_FILE:
            snprintf(afw.tmp_path, PATH_MAX, "%s.tmp", afw.current_task->path);
            afw.current_fp = eos_fs_open_write(afw.tmp_path);
            if (!afw.current_fp)
            {
                EOS_LOG_E("Can't open tmp file");
                afw.current_task->state = EOS_AFW_TASK_STATE_ERROR;
                break;
            }
            afw.current_task->state = EOS_AFW_TASK_STATE_WRITING_BLOCK;
            afw.current_offset = 0;
            EOS_LOG_I("Created temp file: %s", afw.tmp_path);
            break;
        case EOS_AFW_TASK_STATE_WRITING_BLOCK:
            size_t remain = afw.current_task->data_size - afw.current_offset;
            size_t write_size = remain > EOS_AFW_FILE_BLOCK_SIZE ? EOS_AFW_FILE_BLOCK_SIZE : remain;

            if (eos_fs_seek(afw.current_fp, afw.current_offset) != 0)
            {
                EOS_LOG_E("Seek failed");
                afw.current_task->state = EOS_AFW_TASK_STATE_ERROR;
                break;
            }

            ssize_t written = eos_fs_write(afw.current_fp, afw.current_task->data + afw.current_offset, write_size);

            if (written <= 0)
            {
                EOS_LOG_E("Write data failed, write: %u/%zu", written, write_size);
                afw.current_task->state = EOS_AFW_TASK_STATE_ERROR;
                break;
            }
            afw.current_offset += (size_t)written;

            if (afw.current_offset >= afw.current_task->data_size)
            {
                afw.current_task->state = EOS_AFW_TASK_STATE_RENAME_FILE;
            }
            EOS_LOG_I("Progress %uB/%uB", afw.current_offset, afw.current_task->data_size);
            break;
        case EOS_AFW_TASK_STATE_RENAME_FILE:
            EOS_LOG_I("Write done");
            eos_fs_remove(afw.current_task->path);
            eos_fs_mv(afw.tmp_path, afw.current_task->path);
            afw.current_task->state = EOS_AFW_TASK_STATE_DONE;
            _reset_afw();
            break;
        case EOS_AFW_TASK_STATE_ERROR:
            EOS_LOG_W("AFW encountered a fatal error");
            eos_fs_remove(afw.tmp_path);
            _reset_afw();
            break;
        default:
            EOS_LOG_E("Unknown state");
            break;
        }
    }
}

bool eos_afw_add_task(const char *path, void *data, size_t data_size)
{
    EOS_CHECK_PTR_RETURN_VAL(path && data, false);
    if (data_size <= 0)
        return false;

    eos_afw_task_t *t = eos_malloc_zeroed(sizeof(eos_afw_task_t));
    EOS_CHECK_PTR_RETURN_VAL(t, false);

    t->state = EOS_AFW_TASK_STATE_WAITING;
    t->data = eos_strdup(data);
    t->path = eos_strdup(path);
    if (!t->data || !t->path)
    {
        eos_free(t->data);
        eos_free(t->path);
        eos_free(t);
        return false;
    }
    t->data_size = data_size;
    // 查找空闲位
    for (int i = 0; i < EOS_AFW_TASK_MAX; i++)
    {
        if (afw.task_list[i] == NULL)
        {
            afw.task_list[i] = t;
            afw.todo_task++;
            EOS_LOG_I("Added AFW task: %s", path);
            return true;
        }
    }
    EOS_LOG_W("AFW task list full, task not accepted");
    eos_free(t);
    return false;
}
