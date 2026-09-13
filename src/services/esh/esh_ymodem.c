/**
 * @file esh_ymodem.c
 * @brief YMODEM transport for ESH
 */

#include "esh_ymodem.h"

/* Includes ---------------------------------------------------*/
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "eos_core.h"
#include "eos_service_storage.h"
#include "esh.h"

/* Macros and Definitions -------------------------------------*/
#define _ESH_YMODEM_SOH 0x01U
#define _ESH_YMODEM_STX 0x02U
#define _ESH_YMODEM_EOT 0x04U
#define _ESH_YMODEM_ACK 0x06U
#define _ESH_YMODEM_NAK 0x15U
#define _ESH_YMODEM_CAN 0x18U
#define _ESH_CTRL_C 0x03U
#define _ESH_YMODEM_CRC 'C'
#define _ESH_YMODEM_DATA_SIZE 128U
#define _ESH_YMODEM_BLOCK_SIZE 1024U
#define _ESH_YMODEM_HEADER_BLOCK 0U
#define _ESH_YMODEM_TIMEOUT_MS 30000U
#define _ESH_YMODEM_MAX_RETRIES 10U
#define _ESH_YMODEM_PADDING 0x1AU
#define _ESH_YMODEM_PART_SUFFIX ".ymodem.part"

/* Variables --------------------------------------------------*/

/* Function Prototypes ----------------------------------------*/
static uint16_t _esh_ymodem_crc16(const uint8_t *data, size_t length);
static eos_result_t _esh_ymodem_write_control(esh_t *esh, uint8_t control);
static eos_result_t _esh_ymodem_send_packet(esh_t *esh);
static eos_result_t _esh_ymodem_send_header(esh_t *esh);
static eos_result_t _esh_ymodem_send_data(esh_t *esh);
static eos_result_t _esh_ymodem_send_empty_header(esh_t *esh);
static eos_result_t _esh_ymodem_finish(esh_t *esh, bool success, bool send_cancel);
static eos_result_t _esh_ymodem_process_byte(esh_t *esh, uint8_t byte);
static eos_result_t _esh_ymodem_process_packet(esh_t *esh);
static bool _esh_ymodem_parse_size(const uint8_t *data, size_t length, uint32_t *size);
static bool _esh_ymodem_path_join(char *out, size_t out_size, const char *base, const char *name);
static bool _esh_ymodem_send_next_file(esh_t *esh);
static bool _esh_ymodem_prepare_receive_file(esh_t *esh, const uint8_t *data, size_t length);
static bool _esh_ymodem_commit_receive_file(esh_t *esh);
static void _esh_ymodem_close_send_dirs(esh_t *esh);

/* Function Implementations -----------------------------------*/

static uint16_t _esh_ymodem_crc16(const uint8_t *data, size_t length)
{
    uint16_t crc = 0U;
    size_t index;
    uint8_t bit;

    for (index = 0U; index < length; index++)
    {
        crc ^= (uint16_t)data[index] << 8U;
        for (bit = 0U; bit < 8U; bit++)
        {
            crc = (crc & 0x8000U) != 0U ? (uint16_t)((crc << 1U) ^ 0x1021U) : (uint16_t)(crc << 1U);
        }
    }

    return crc;
}

static eos_result_t _esh_ymodem_write_control(esh_t *esh, uint8_t control)
{
    if (control == _ESH_YMODEM_NAK)
    {
        esh->ymodem.stats.nak_count++;
    }
    return esh_write_active(esh, &control, sizeof(control));
}

static eos_result_t _esh_ymodem_send_packet(esh_t *esh)
{
    uint16_t crc;
    size_t data_size;

    data_size = esh->ymodem.packet[0] == _ESH_YMODEM_STX ? _ESH_YMODEM_BLOCK_SIZE : _ESH_YMODEM_DATA_SIZE;
    crc = _esh_ymodem_crc16(&esh->ymodem.packet[3], data_size);
    esh->ymodem.packet[3U + data_size] = (uint8_t)(crc >> 8U);
    esh->ymodem.packet[4U + data_size] = (uint8_t)crc;
    esh->ymodem.packet_length = 5U + data_size;
    return esh_write_active(esh, esh->ymodem.packet, esh->ymodem.packet_length);
}

static eos_result_t _esh_ymodem_send_header(esh_t *esh)
{
    const char *filename = esh->ymodem.send_file_name;
    size_t filename_length;
    char size_text[16];
    int size_text_length;

    filename_length = strlen(filename);
    if (filename_length >= (_ESH_YMODEM_DATA_SIZE - 1U))
    {
        return EOS_ERR_PATH_TOO_LONG;
    }

    memset(esh->ymodem.packet, 0, sizeof(esh->ymodem.packet));
    esh->ymodem.packet[0] = _ESH_YMODEM_SOH;
    esh->ymodem.packet[1] = _ESH_YMODEM_HEADER_BLOCK;
    esh->ymodem.packet[2] = (uint8_t)~_ESH_YMODEM_HEADER_BLOCK;
    memcpy(&esh->ymodem.packet[3], filename, filename_length);
    esh->ymodem.packet[3U + filename_length] = '\0';

    size_text_length = snprintf(size_text, sizeof(size_text), "%" PRIu32, esh->ymodem.file_size);
    if (size_text_length < 0 || (size_t)size_text_length >= sizeof(size_text)
        || filename_length + 1U + (size_t)size_text_length >= _ESH_YMODEM_DATA_SIZE)
    {
        return EOS_ERR_PATH_TOO_LONG;
    }

    memcpy(&esh->ymodem.packet[4U + filename_length], size_text, (size_t)size_text_length + 1U);
    esh->ymodem.packet_length = 0U;
    return _esh_ymodem_send_packet(esh);
}

static eos_result_t _esh_ymodem_send_empty_header(esh_t *esh)
{
    memset(esh->ymodem.packet, 0, sizeof(esh->ymodem.packet));
    esh->ymodem.packet[0] = _ESH_YMODEM_SOH;
    esh->ymodem.packet[1] = _ESH_YMODEM_HEADER_BLOCK;
    esh->ymodem.packet[2] = (uint8_t)~_ESH_YMODEM_HEADER_BLOCK;
    return _esh_ymodem_send_packet(esh);
}

static eos_result_t _esh_ymodem_send_data(esh_t *esh)
{
    ssize_t read_length;

    memset(esh->ymodem.packet, _ESH_YMODEM_PADDING, sizeof(esh->ymodem.packet));
    read_length = eos_storage_file_read(esh->ymodem.file, &esh->ymodem.packet[3], _ESH_YMODEM_BLOCK_SIZE);
    if (read_length < 0)
    {
        return _esh_ymodem_finish(esh, false, true);
    }

    if (read_length == 0)
    {
        esh->ymodem.packet_length = 0U;
        esh->ymodem.state = ESH_YMODEM_SEND_WAIT_EOT_NAK;
        return _esh_ymodem_write_control(esh, _ESH_YMODEM_EOT);
    }

    esh->ymodem.packet[0] = _ESH_YMODEM_STX;
    esh->ymodem.packet[1] = esh->ymodem.expected_block;
    esh->ymodem.packet[2] = (uint8_t)~esh->ymodem.expected_block;
    esh->ymodem.file_offset += (uint32_t)read_length;
    esh->ymodem.state = ESH_YMODEM_SEND_WAIT_DATA_ACK;
    return _esh_ymodem_send_packet(esh);
}

static eos_result_t _esh_ymodem_finish(esh_t *esh, bool success, bool send_cancel)
{
    char message[ESH_PRINTF_BUFFER_SIZE];
    int message_length;

    if (esh->ymodem.stats.transfer_start_tick != 0U)
    {
        esh->ymodem.stats.transfer_elapsed_ms =
            eos_tick_get() - esh->ymodem.stats.transfer_start_tick;
    }

    if (send_cancel)
    {
        uint8_t cancel[2] = {_ESH_YMODEM_CAN, _ESH_YMODEM_CAN};
        (void)esh_write_active(esh, cancel, sizeof(cancel));
    }

    if (esh->ymodem.file_open)
    {
        eos_storage_file_close(esh->ymodem.file);
        esh->ymodem.file_open = false;
    }

    if (!success && esh->ymodem.receive_temp_path[0] != '\0')
    {
        (void)eos_storage_file_remove(esh->ymodem.receive_temp_path);
    }

    _esh_ymodem_close_send_dirs(esh);

    message_length = snprintf(message,
                              sizeof(message),
                              "ymodem: %s %s\r\n",
                              success ? "transfer complete:" : "transfer failed:",
                              esh->ymodem.path);
    if (message_length > 0 && (size_t)message_length < sizeof(message))
    {
        (void)esh_write_active(esh, message, (size_t)message_length);
    }

    esh->ymodem.state = ESH_YMODEM_IDLE;
    esh->ymodem.packet_length = 0U;
    esh->ymodem.packet_expected = 0U;
    esh->input_mode = ESH_INPUT_COMMAND;
    return success ? EOS_OK : EOS_ERR_IO;
}

static bool _esh_ymodem_path_join(char *out, size_t out_size, const char *base, const char *name)
{
    int written;

    if (!out || out_size == 0U || !base || !name || name[0] == '\0')
    {
        return false;
    }

    if (strcmp(base, "/") == 0)
    {
        written = snprintf(out, out_size, "/%s", name);
    }
    else
    {
        written = snprintf(out, out_size, "%s/%s", base, name);
    }

    return written >= 0 && (size_t)written < out_size;
}

static void _esh_ymodem_close_send_dirs(esh_t *esh)
{
    while (esh && esh->ymodem.send_dir_depth > 0U)
    {
        esh->ymodem.send_dir_depth--;
        if (esh->ymodem.send_dirs[esh->ymodem.send_dir_depth] != EOS_DIR_INVALID)
        {
            eos_storage_dir_close(esh->ymodem.send_dirs[esh->ymodem.send_dir_depth]);
            esh->ymodem.send_dirs[esh->ymodem.send_dir_depth] = EOS_DIR_INVALID;
        }
    }
}

/* Depth-first traversal.  Directory handles stay open between YMODEM files,
 * so this uses bounded RAM and does not need a complete manifest in memory. */
static bool _esh_ymodem_send_next_file(esh_t *esh)
{
    char name[EOS_FS_PATH_MAX];
    char entry_path[EOS_FS_PATH_MAX];

    if (!esh)
    {
        return false;
    }

    while (esh->ymodem.send_dir_depth > 0U)
    {
        uint8_t index = (uint8_t)(esh->ymodem.send_dir_depth - 1U);
        eos_result_t read_result = eos_storage_dir_read(esh->ymodem.send_dirs[index], name, sizeof(name));

        if (read_result != EOS_OK)
        {
            eos_storage_dir_close(esh->ymodem.send_dirs[index]);
            esh->ymodem.send_dirs[index] = EOS_DIR_INVALID;
            esh->ymodem.send_dir_depth--;
            continue;
        }

        if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0 || name[0] == '\0')
        {
            continue;
        }

        if (!_esh_ymodem_path_join(entry_path, sizeof(entry_path), esh->ymodem.send_dir_paths[index], name))
        {
            return false;
        }

        if (eos_storage_is_dir(entry_path))
        {
            eos_dir_t child;

            if (esh->ymodem.send_dir_depth >= ESH_YMODEM_MAX_DIR_DEPTH)
            {
                return false;
            }

            child = eos_storage_dir_open(entry_path);
            if (child == EOS_DIR_INVALID)
            {
                return false;
            }

            esh->ymodem.send_dirs[esh->ymodem.send_dir_depth] = child;
            strncpy(esh->ymodem.send_dir_paths[esh->ymodem.send_dir_depth],
                    entry_path,
                    sizeof(esh->ymodem.send_dir_paths[0]) - 1U);
            esh->ymodem.send_dir_paths[esh->ymodem.send_dir_depth][sizeof(esh->ymodem.send_dir_paths[0]) - 1U] = '\0';
            esh->ymodem.send_dir_depth++;
            continue;
        }

        if (!eos_storage_is_file(entry_path))
        {
            continue;
        }

        strncpy(esh->ymodem.send_file_path, entry_path, sizeof(esh->ymodem.send_file_path) - 1U);
        esh->ymodem.send_file_path[sizeof(esh->ymodem.send_file_path) - 1U] = '\0';

        /* Include the selected directory name in a batch filename.  This
         * makes `send /res/fonts` arrive as `fonts/foo.ttf`, preserving the
         * folder when the host receiver is pointed at a destination root. */
        {
            const char *root_name = strrchr(esh->ymodem.send_root, '/');
            const char *relative = entry_path;
            size_t root_length = strlen(esh->ymodem.send_root);
            size_t root_name_length;

            root_name = root_name ? root_name + 1 : esh->ymodem.send_root;
            root_name_length = strlen(root_name);
            if (strcmp(esh->ymodem.send_root, "/") != 0 && root_name_length > 0U)
            {
                if (root_length < strlen(entry_path) && entry_path[root_length] == '/')
                {
                    relative = entry_path + root_length + 1U;
                }
                if (!_esh_ymodem_path_join(esh->ymodem.send_file_name,
                                           sizeof(esh->ymodem.send_file_name),
                                           root_name,
                                           relative))
                {
                    return false;
                }
            }
            else
            {
                relative = entry_path[0] == '/' ? entry_path + 1U : entry_path;
                strncpy(esh->ymodem.send_file_name, relative, sizeof(esh->ymodem.send_file_name) - 1U);
                esh->ymodem.send_file_name[sizeof(esh->ymodem.send_file_name) - 1U] = '\0';
            }
        }

        esh->ymodem.file = eos_storage_file_open_read(esh->ymodem.send_file_path);
        if (esh->ymodem.file == EOS_FILE_INVALID
            || eos_storage_file_size(esh->ymodem.file, &esh->ymodem.file_size) != EOS_OK)
        {
            if (esh->ymodem.file != EOS_FILE_INVALID)
            {
                eos_storage_file_close(esh->ymodem.file);
            }
            esh->ymodem.file = EOS_FILE_INVALID;
            return false;
        }

        esh->ymodem.file_open = true;
        esh->ymodem.send_has_file = true;
        esh->ymodem.file_offset = 0U;
        esh->ymodem.expected_block = 1U;
        return true;
    }

    return false;
}

static bool _esh_ymodem_valid_batch_name(const uint8_t *data, size_t length, size_t *name_length)
{
    size_t index = 0U;
    size_t component_start = 0U;

    if (!data || !name_length)
    {
        return false;
    }

    while (index < length && data[index] != '\0')
    {
        uint8_t ch = data[index];
        if (ch < 32U || ch == '\\' || ch == ':' || ch == '"' || ch == '<' || ch == '>' || ch == '|' || ch == '?'
            || ch == '*')
        {
            return false;
        }
        if (ch == '/')
        {
            if (index == component_start || (index - component_start == 1U && data[component_start] == '.')
                || (index - component_start == 2U && data[component_start] == '.' && data[component_start + 1U] == '.'))
            {
                return false;
            }
            component_start = index + 1U;
        }
        index++;
    }

    if (index == 0U || index == length || index == component_start
        || (index - component_start == 1U && data[component_start] == '.')
        || (index - component_start == 2U && data[component_start] == '.' && data[component_start + 1U] == '.'))
    {
        return false;
    }

    *name_length = index;
    return true;
}

static bool _esh_ymodem_prepare_receive_file(esh_t *esh, const uint8_t *data, size_t length)
{
    eos_storage_space_t space;
    size_t name_length;
    char parent[EOS_FS_PATH_MAX];
    char *last_separator;

    if (!_esh_ymodem_valid_batch_name(data, length, &name_length))
    {
        return false;
    }

    if (esh->ymodem.receive_target_is_dir)
    {
        if (!_esh_ymodem_path_join(esh->ymodem.receive_final_path,
                                   sizeof(esh->ymodem.receive_final_path),
                                   esh->ymodem.path,
                                   (const char *)data))
        {
            return false;
        }
    }
    else
    {
        strncpy(esh->ymodem.receive_final_path, esh->ymodem.path, sizeof(esh->ymodem.receive_final_path) - 1U);
        esh->ymodem.receive_final_path[sizeof(esh->ymodem.receive_final_path) - 1U] = '\0';
    }

    if (snprintf(esh->ymodem.receive_temp_path,
                 sizeof(esh->ymodem.receive_temp_path),
                 "%s%s",
                 esh->ymodem.receive_final_path,
                 _ESH_YMODEM_PART_SUFFIX)
        >= (int)sizeof(esh->ymodem.receive_temp_path))
    {
        return false;
    }

    strncpy(parent, esh->ymodem.receive_final_path, sizeof(parent) - 1U);
    parent[sizeof(parent) - 1U] = '\0';
    last_separator = strrchr(parent, '/');
    if (last_separator && last_separator != parent)
    {
        *last_separator = '\0';
        if (eos_storage_mkdir_recursive(parent) != EOS_OK)
        {
            return false;
        }
    }

    /* The YMODEM header contains the exact file size.  Reject a file that
     * cannot fit before creating the temporary file or accepting data blocks.
     * The storage port reports usable write headroom; on the E84 Flash port it
     * already excludes LittleFS metadata reserve blocks.  This keeps the
     * protocol stop-and-wait path from entering an unbounded LittleFS
     * allocation/GC loop after several megabytes have already arrived. */
    if (eos_storage_get_space(esh->ymodem.receive_final_path, &space) == EOS_OK
        && (uint64_t)esh->ymodem.file_size > space.free_bytes)
    {
        return false;
    }

    (void)eos_storage_file_remove(esh->ymodem.receive_temp_path);
    esh->ymodem.file = eos_storage_file_open_write(esh->ymodem.receive_temp_path);
    if (esh->ymodem.file == EOS_FILE_INVALID)
    {
        return false;
    }

    (void)name_length;
    esh->ymodem.file_open = true;
    esh->ymodem.receive_file_active = true;
    esh->ymodem.file_offset = 0U;
    return true;
}

static bool _esh_ymodem_commit_receive_file(esh_t *esh)
{
    if (!esh || !esh->ymodem.receive_file_active)
    {
        return false;
    }

    if (esh->ymodem.file_open)
    {
        eos_storage_file_close(esh->ymodem.file);
        esh->ymodem.file_open = false;
    }

    if (eos_storage_file_move(esh->ymodem.receive_temp_path, esh->ymodem.receive_final_path) != EOS_OK)
    {
        return false;
    }

    esh->ymodem.receive_temp_path[0] = '\0';
    esh->ymodem.receive_file_active = false;
    esh->ymodem.receive_any_file = true;
    return true;
}

static bool _esh_ymodem_parse_size(const uint8_t *data, size_t length, uint32_t *size)
{
    const uint8_t *separator;
    const uint8_t *cursor;
    uint32_t value = 0U;
    uint8_t digit;
    bool has_digit = false;

    separator = memchr(data, '\0', length);
    if (!separator || separator == data)
    {
        return false;
    }

    cursor = separator + 1U;
    while ((size_t)(cursor - data) < length && *cursor != '\0' && *cursor != ' ')
    {
        if (*cursor < '0' || *cursor > '9')
        {
            return false;
        }

        digit = (uint8_t)(*cursor - '0');
        if (value > (UINT32_MAX - digit) / 10U)
        {
            return false;
        }

        value = value * 10U + digit;
        has_digit = true;
        cursor++;
    }

    if (!has_digit)
    {
        return false;
    }

    *size = value;
    return true;
}

static eos_result_t _esh_ymodem_process_packet(esh_t *esh)
{
    size_t data_size;
    uint16_t received_crc;
    uint16_t calculated_crc;
    uint8_t block;
    size_t write_size;

    data_size = esh->ymodem.packet[0] == _ESH_YMODEM_STX ? _ESH_YMODEM_BLOCK_SIZE : _ESH_YMODEM_DATA_SIZE;
    block = esh->ymodem.packet[1];
    received_crc = ((uint16_t)esh->ymodem.packet[3U + data_size] << 8U) | esh->ymodem.packet[4U + data_size];
    calculated_crc = _esh_ymodem_crc16(&esh->ymodem.packet[3], data_size);

    if ((uint8_t)(block + esh->ymodem.packet[2]) != 0xFFU || received_crc != calculated_crc)
    {
        esh->ymodem.stats.last_bad_block = block;
        if ((uint8_t)(block + esh->ymodem.packet[2]) != 0xFFU)
        {
            esh->ymodem.stats.block_number_error_count++;
        }
        if (received_crc != calculated_crc)
        {
            esh->ymodem.stats.crc_error_count++;
        }
        esh->ymodem.packet_length = 0U;
        return _esh_ymodem_write_control(esh, _ESH_YMODEM_NAK);
    }

    if (esh->ymodem.state == ESH_YMODEM_RECEIVE_WAIT_HEADER)
    {
        if (block != _ESH_YMODEM_HEADER_BLOCK)
        {
            esh->ymodem.stats.last_bad_block = block;
            esh->ymodem.stats.block_number_error_count++;
            esh->ymodem.packet_length = 0U;
            return _esh_ymodem_write_control(esh, _ESH_YMODEM_NAK);
        }

        if (esh->ymodem.packet[3] == '\0')
        {
            uint8_t ack = _ESH_YMODEM_ACK;
            esh->ymodem.packet_length = 0U;
            if (esh_write_active(esh, &ack, sizeof(ack)) != EOS_OK)
            {
                return _esh_ymodem_finish(esh, false, true);
            }
            return _esh_ymodem_finish(esh, esh->ymodem.receive_any_file, false);
        }

        if (!_esh_ymodem_parse_size(&esh->ymodem.packet[3], _ESH_YMODEM_DATA_SIZE, &esh->ymodem.file_size))
        {
            esh->ymodem.packet_length = 0U;
            return _esh_ymodem_write_control(esh, _ESH_YMODEM_NAK);
        }

        if (!_esh_ymodem_prepare_receive_file(esh, &esh->ymodem.packet[3], _ESH_YMODEM_DATA_SIZE))
        {
            esh->ymodem.packet_length = 0U;
            return _esh_ymodem_finish(esh, false, true);
        }

        esh->ymodem.file_offset = 0U;
        esh->ymodem.expected_block = 1U;
        esh->ymodem.state = ESH_YMODEM_RECEIVE_DATA;
        esh->ymodem.packet_length = 0U;
        {
            uint8_t response[2] = {_ESH_YMODEM_ACK, _ESH_YMODEM_CRC};
            return esh_write_active(esh, response, sizeof(response));
        }
    }

    if (block == esh->ymodem.expected_block)
    {
        esh->ymodem.stats.received_data_blocks++;
        write_size =
            esh->ymodem.file_size > esh->ymodem.file_offset ? esh->ymodem.file_size - esh->ymodem.file_offset : 0U;
        if (write_size > data_size)
        {
            write_size = data_size;
        }

        if (write_size > 0U)
        {
            ssize_t written = eos_storage_file_write(esh->ymodem.file,
                                                      &esh->ymodem.packet[3],
                                                      write_size);
            if (written != (ssize_t)write_size)
            {
                return _esh_ymodem_finish(esh, false, true);
            }
            esh->ymodem.file_offset += (uint32_t)write_size;
            esh->ymodem.stats.received_bytes += (uint32_t)write_size;
        }

        esh->ymodem.expected_block++;
        esh->ymodem.packet_length = 0U;
        return _esh_ymodem_write_control(esh, _ESH_YMODEM_ACK);
    }

    esh->ymodem.packet_length = 0U;
    if (block == (uint8_t)(esh->ymodem.expected_block - 1U))
    {
        esh->ymodem.stats.duplicate_block_count++;
        return _esh_ymodem_write_control(esh, _ESH_YMODEM_ACK);
    }

    esh->ymodem.stats.last_bad_block = block;
    esh->ymodem.stats.unexpected_block_count++;
    return _esh_ymodem_write_control(esh, _ESH_YMODEM_NAK);
}

static eos_result_t _esh_ymodem_process_byte(esh_t *esh, uint8_t byte)
{
    bool receiving_packet = (esh->ymodem.state == ESH_YMODEM_RECEIVE_WAIT_HEADER
                             || esh->ymodem.state == ESH_YMODEM_RECEIVE_DATA)
                            && esh->ymodem.packet_length != 0U;

    if (byte == _ESH_YMODEM_CAN && !receiving_packet)
    {
        return _esh_ymodem_finish(esh, false, false);
    }

    if (esh->ymodem.state == ESH_YMODEM_SEND_WAIT_C)
    {
        if (byte == _ESH_YMODEM_CRC || byte == _ESH_YMODEM_NAK)
        {
            eos_result_t result;

            if (esh->ymodem.send_end_pending)
            {
                esh->ymodem.state = ESH_YMODEM_SEND_WAIT_END_HEADER_ACK;
                result = _esh_ymodem_send_empty_header(esh);
            }
            else
            {
                esh->ymodem.state = ESH_YMODEM_SEND_WAIT_HEADER_ACK;
                result = _esh_ymodem_send_header(esh);
            }
            return result == EOS_OK ? EOS_OK : _esh_ymodem_finish(esh, false, true);
        }
        return EOS_OK;
    }

    if (esh->ymodem.state == ESH_YMODEM_SEND_WAIT_HEADER_ACK)
    {
        if (byte == _ESH_YMODEM_ACK)
        {
            esh->ymodem.state = ESH_YMODEM_SEND_WAIT_DATA_C;
        }
        else if (byte == _ESH_YMODEM_NAK)
        {
            return _esh_ymodem_send_packet(esh);
        }
        return EOS_OK;
    }

    if (esh->ymodem.state == ESH_YMODEM_SEND_WAIT_DATA_C)
    {
        if (byte == _ESH_YMODEM_CRC || byte == _ESH_YMODEM_NAK)
        {
            return _esh_ymodem_send_data(esh);
        }
        return EOS_OK;
    }

    if (esh->ymodem.state == ESH_YMODEM_SEND_WAIT_DATA_ACK)
    {
        if (byte == _ESH_YMODEM_ACK)
        {
            esh->ymodem.expected_block++;
            return _esh_ymodem_send_data(esh);
        }
        if (byte == _ESH_YMODEM_NAK)
        {
            return _esh_ymodem_send_packet(esh);
        }
        return EOS_OK;
    }

    if (esh->ymodem.state == ESH_YMODEM_SEND_WAIT_EOT_NAK)
    {
        if (byte == _ESH_YMODEM_NAK)
        {
            esh->ymodem.state = ESH_YMODEM_SEND_WAIT_EOT_ACK;
            return _esh_ymodem_write_control(esh, _ESH_YMODEM_EOT);
        }
        if (byte == _ESH_YMODEM_ACK)
        {
            if (!esh->ymodem.batch_mode)
            {
                return _esh_ymodem_finish(esh, true, false);
            }

            if (esh->ymodem.file_open)
            {
                eos_storage_file_close(esh->ymodem.file);
                esh->ymodem.file_open = false;
            }
            if (_esh_ymodem_send_next_file(esh))
            {
                esh->ymodem.send_end_pending = false;
            }
            else
            {
                esh->ymodem.send_end_pending = true;
            }
            esh->ymodem.state = ESH_YMODEM_SEND_WAIT_C;
            esh->ymodem.last_activity_tick = eos_tick_get();
            return EOS_OK;
        }
        return EOS_OK;
    }

    if (esh->ymodem.state == ESH_YMODEM_SEND_WAIT_EOT_ACK)
    {
        if (byte == _ESH_YMODEM_ACK)
        {
            if (!esh->ymodem.batch_mode)
            {
                return _esh_ymodem_finish(esh, true, false);
            }

            if (esh->ymodem.file_open)
            {
                eos_storage_file_close(esh->ymodem.file);
                esh->ymodem.file_open = false;
            }
            if (_esh_ymodem_send_next_file(esh))
            {
                esh->ymodem.send_end_pending = false;
            }
            else
            {
                esh->ymodem.send_end_pending = true;
            }
            esh->ymodem.state = ESH_YMODEM_SEND_WAIT_C;
            esh->ymodem.last_activity_tick = eos_tick_get();
            return EOS_OK;
        }
        if (byte == _ESH_YMODEM_NAK)
        {
            return _esh_ymodem_write_control(esh, _ESH_YMODEM_EOT);
        }
        return EOS_OK;
    }

    if (esh->ymodem.state == ESH_YMODEM_SEND_WAIT_END_HEADER_ACK)
    {
        if (byte == _ESH_YMODEM_ACK)
        {
            return _esh_ymodem_finish(esh, true, false);
        }
        if (byte == _ESH_YMODEM_NAK)
        {
            return _esh_ymodem_send_empty_header(esh);
        }
        return EOS_OK;
    }

    if (esh->ymodem.state == ESH_YMODEM_RECEIVE_WAIT_EOT)
    {
        if (byte == _ESH_YMODEM_EOT)
        {
            if (esh->ymodem.file_offset != esh->ymodem.file_size)
            {
                return _esh_ymodem_finish(esh, false, true);
            }
            if (!_esh_ymodem_commit_receive_file(esh))
            {
                return _esh_ymodem_finish(esh, false, true);
            }

            /* Do not acknowledge EOT until the temporary file has been
             * closed and renamed.  This keeps the standard handshake while
             * making the ACK mean that the received file is durable in the
             * storage contract, instead of merely buffered in RAM. */
            if (_esh_ymodem_write_control(esh, _ESH_YMODEM_ACK) != EOS_OK)
            {
                return _esh_ymodem_finish(esh, false, true);
            }

            if (!esh->ymodem.receive_target_is_dir)
            {
                return _esh_ymodem_finish(esh, true, false);
            }

            esh->ymodem.state = ESH_YMODEM_RECEIVE_WAIT_HEADER;
            esh->ymodem.packet_length = 0U;
            return _esh_ymodem_write_control(esh, _ESH_YMODEM_CRC);
        }
        return EOS_OK;
    }

    if ((esh->ymodem.state == ESH_YMODEM_RECEIVE_WAIT_HEADER || esh->ymodem.state == ESH_YMODEM_RECEIVE_DATA)
        && esh->ymodem.packet_length == 0U)
    {
        if (byte == _ESH_YMODEM_EOT && esh->ymodem.state == ESH_YMODEM_RECEIVE_DATA)
        {
            esh->ymodem.state = ESH_YMODEM_RECEIVE_WAIT_EOT;
            return _esh_ymodem_write_control(esh, _ESH_YMODEM_NAK);
        }

        if (byte == _ESH_YMODEM_SOH || byte == _ESH_YMODEM_STX)
        {
            esh->ymodem.packet[0] = byte;
            esh->ymodem.packet_length = 1U;
            esh->ymodem.packet_expected = byte == _ESH_YMODEM_STX ? 1029U : 133U;
        }
        return EOS_OK;
    }

    if (esh->ymodem.state == ESH_YMODEM_RECEIVE_WAIT_HEADER || esh->ymodem.state == ESH_YMODEM_RECEIVE_DATA)
    {
        if (esh->ymodem.packet_length >= sizeof(esh->ymodem.packet))
        {
            esh->ymodem.packet_length = 0U;
            return _esh_ymodem_write_control(esh, _ESH_YMODEM_NAK);
        }

        esh->ymodem.packet[esh->ymodem.packet_length++] = byte;
        if (esh->ymodem.packet_length == esh->ymodem.packet_expected)
        {
            return _esh_ymodem_process_packet(esh);
        }
    }

    return EOS_OK;
}

eos_result_t esh_ymodem_start_send(struct esh_cmd_ctx *ctx, const char *path)
{
    eos_file_t file;
    uint32_t file_size;
    esh_t *esh;

    if (!ctx || !ctx->esh || !path)
    {
        return EOS_ERR_INVALID_ARG;
    }

    esh = ctx->esh;
    memset(&esh->ymodem, 0, sizeof(esh->ymodem));
    esh->ymodem.file = EOS_FILE_INVALID;
    snprintf(esh->ymodem.path, sizeof(esh->ymodem.path), "%s", path);

    if (eos_storage_is_dir(path))
    {
        esh->ymodem.batch_mode = true;
        strncpy(esh->ymodem.send_root, path, sizeof(esh->ymodem.send_root) - 1U);
        esh->ymodem.send_root[sizeof(esh->ymodem.send_root) - 1U] = '\0';
        esh->ymodem.send_dirs[0] = eos_storage_dir_open(path);
        if (esh->ymodem.send_dirs[0] == EOS_DIR_INVALID)
        {
            return esh_printf(ctx, "ymodem: cannot open directory: %s\r\n", path);
        }
        strncpy(esh->ymodem.send_dir_paths[0], path, sizeof(esh->ymodem.send_dir_paths[0]) - 1U);
        esh->ymodem.send_dir_paths[0][sizeof(esh->ymodem.send_dir_paths[0]) - 1U] = '\0';
        esh->ymodem.send_dir_depth = 1U;
        if (!_esh_ymodem_send_next_file(esh))
        {
            _esh_ymodem_close_send_dirs(esh);
            return esh_printf(ctx, "ymodem: directory contains no regular files: %s\r\n", path);
        }
    }
    else
    {
        file = eos_storage_file_open_read(path);
        if (file == EOS_FILE_INVALID)
        {
            return esh_printf(ctx, "ymodem: cannot open file: %s\r\n", path);
        }

        if (eos_storage_file_size(file, &file_size) != EOS_OK)
        {
            eos_storage_file_close(file);
            return esh_printf(ctx, "ymodem: cannot determine file size: %s\r\n", path);
        }

        esh->ymodem.file = file;
        esh->ymodem.file_open = true;
        esh->ymodem.file_size = file_size;
        {
            const char *filename = strrchr(path, '/');
            filename = filename ? filename + 1 : path;
            strncpy(esh->ymodem.send_file_name, filename, sizeof(esh->ymodem.send_file_name) - 1U);
            esh->ymodem.send_file_name[sizeof(esh->ymodem.send_file_name) - 1U] = '\0';
        }
        esh->ymodem.expected_block = 1U;
    }

    esh->ymodem.state = ESH_YMODEM_SEND_WAIT_C;
    esh->ymodem.last_activity_tick = eos_tick_get();
    esh->ymodem.stats.transfer_start_tick = esh->ymodem.last_activity_tick;
    ctx->esh->input_mode = ESH_INPUT_YMODEM;
    if (esh_printf(ctx, "ymodem: sending %s, waiting for receiver\r\n", path) != EOS_OK)
    {
        return _esh_ymodem_finish(ctx->esh, false, true);
    }

    return EOS_OK;
}

eos_result_t esh_ymodem_start_receive(struct esh_cmd_ctx *ctx, const char *path)
{
    uint8_t crc_request = _ESH_YMODEM_CRC;
    eos_result_t result;

    if (!ctx || !ctx->esh || !path)
    {
        return EOS_ERR_INVALID_ARG;
    }

    memset(&ctx->esh->ymodem, 0, sizeof(ctx->esh->ymodem));
    ctx->esh->ymodem.file = EOS_FILE_INVALID;
    ctx->esh->ymodem.receive_target_is_dir = eos_storage_is_dir(path);
    ctx->esh->ymodem.state = ESH_YMODEM_RECEIVE_WAIT_HEADER;
    ctx->esh->ymodem.last_activity_tick = eos_tick_get();
    ctx->esh->ymodem.stats.transfer_start_tick = ctx->esh->ymodem.last_activity_tick;
    snprintf(ctx->esh->ymodem.path, sizeof(ctx->esh->ymodem.path), "%s", path);
    ctx->esh->input_mode = ESH_INPUT_YMODEM;

    result = esh_printf(ctx, "ymodem: receiving %s, waiting for sender\r\n", path);
    if (result == EOS_OK)
    {
        result = esh_write_active(ctx->esh, &crc_request, sizeof(crc_request));
    }

    if (result != EOS_OK)
    {
        return _esh_ymodem_finish(ctx->esh, false, true);
    }

    return result;
}

eos_result_t esh_ymodem_input(struct esh *esh, const uint8_t *data, size_t length)
{
    eos_result_t result = EOS_OK;
    size_t index;

    if (!esh || (!data && length > 0U))
    {
        return EOS_ERR_INVALID_ARG;
    }

	for (index = 0U; index < length && esh->input_mode == ESH_INPUT_YMODEM; index++)
	{
		/* Ctrl-C is an ESH escape only between packets.  Inside a YMODEM
		 * frame every byte, including 0x03, belongs to the payload or CRC;
		 * treating it as an escape aborts valid binary files before the first
		 * data block can be validated.  Standard YMODEM cancellation remains
		 * CAN CAN, which is handled by _esh_ymodem_process_byte at packet idle. */
		bool receiving_packet = (esh->ymodem.state == ESH_YMODEM_RECEIVE_WAIT_HEADER
		                         || esh->ymodem.state == ESH_YMODEM_RECEIVE_DATA)
		                        && esh->ymodem.packet_length != 0U;
		if (data[index] == _ESH_CTRL_C && !receiving_packet)
        {
            esh_ymodem_abort(esh);
            break;
        }

        esh->ymodem.last_activity_tick = eos_tick_get();
        esh->ymodem.retry_count = 0U;
        result = _esh_ymodem_process_byte(esh, data[index]);
        if (result != EOS_OK && esh->input_mode == ESH_INPUT_YMODEM)
        {
            break;
        }
    }

    return result;
}

void esh_ymodem_poll(struct esh *esh)
{
    uint32_t now;
    eos_result_t result = EOS_OK;

    if (!esh || esh->input_mode != ESH_INPUT_YMODEM)
    {
        return;
    }

    now = eos_tick_get();
    if ((uint32_t)(now - esh->ymodem.last_activity_tick) < _ESH_YMODEM_TIMEOUT_MS)
    {
        return;
    }

    esh->ymodem.last_activity_tick = now;
    esh->ymodem.stats.timeout_count++;
    if (esh->ymodem.retry_count++ >= _ESH_YMODEM_MAX_RETRIES)
    {
        (void)_esh_ymodem_finish(esh, false, true);
        return;
    }

    switch (esh->ymodem.state)
    {
        case ESH_YMODEM_RECEIVE_WAIT_HEADER:
            result = _esh_ymodem_write_control(esh, _ESH_YMODEM_CRC);
            break;
        case ESH_YMODEM_RECEIVE_WAIT_EOT:
            result = _esh_ymodem_write_control(esh, _ESH_YMODEM_NAK);
            break;
        case ESH_YMODEM_SEND_WAIT_HEADER_ACK:
        case ESH_YMODEM_SEND_WAIT_DATA_ACK:
            result = _esh_ymodem_send_packet(esh);
            break;
        case ESH_YMODEM_SEND_WAIT_END_HEADER_ACK:
            result = _esh_ymodem_send_empty_header(esh);
            break;
        case ESH_YMODEM_SEND_WAIT_EOT_NAK:
        case ESH_YMODEM_SEND_WAIT_EOT_ACK:
            result = _esh_ymodem_write_control(esh, _ESH_YMODEM_EOT);
            break;
        default:
            break;
    }

    if (result != EOS_OK)
    {
        (void)_esh_ymodem_finish(esh, false, true);
    }
}

void esh_ymodem_abort(struct esh *esh)
{
    esh_ymodem_stats_t stats;

    if (!esh)
    {
        return;
    }

    stats = esh->ymodem.stats;
    if (stats.transfer_start_tick != 0U)
    {
        stats.transfer_elapsed_ms = eos_tick_get() - stats.transfer_start_tick;
    }

    if (esh->ymodem.file_open)
    {
        eos_storage_file_close(esh->ymodem.file);
    }

    if (esh->ymodem.receive_temp_path[0] != '\0')
    {
        (void)eos_storage_file_remove(esh->ymodem.receive_temp_path);
    }

    _esh_ymodem_close_send_dirs(esh);

    memset(&esh->ymodem, 0, sizeof(esh->ymodem));
    esh->ymodem.stats = stats;
    esh->input_mode = ESH_INPUT_COMMAND;
}
