/**
 * @file eos_service_battery.c
 * @brief Battery service implementation
 */

#include "eos_service_battery.h"

/* Includes ---------------------------------------------------*/
#include <string.h>
#include "eos_port_critical.h"
#include "eos_core.h"
#include "eos_event.h"
#include "eos_service_storage.h"
#include "eos_storage_paths.h"
#include "eos_service_state.h"
#include "eos_mem.h"
#define EOS_LOG_TAG "BatteryService"
#include "eos_log.h"

/* Macros and Definitions -------------------------------------*/
/**
 * @brief Battery process timer interval
 */
#define EOS_BATTERY_PROCESS_INTERVAL_MS    1000

/**
 * @brief Battery request interval (time between update requests)
 */
#define EOS_BATTERY_REQUEST_INTERVAL_MS    8000

/**
 * @brief Battery history record interval
 */
#define EOS_BATTERY_HISTORY_INTERVAL_MS    60000

/**
 * @brief Battery state save interval
 */
#define EOS_BATTERY_SAVE_INTERVAL_MS       30000

/**
 * @brief Battery history ring buffer size
 */
#define EOS_BATTERY_HISTORY_SIZE           128

/**
 * @brief Minimum battery voltage (mV) for percentage calculation
 */
#define EOS_BATTERY_VOLTAGE_MIN            3000

/**
 * @brief Maximum battery voltage (mV) for percentage calculation
 */
#define EOS_BATTERY_VOLTAGE_MAX            4200

/* Variables --------------------------------------------------*/
/**
 * @brief Battery service internal state
 */
typedef struct {
    bool initialized;                   /**< Initialization flag */
    const eos_battery_dev_ops_t *dev_ops; /**< Device operations handle */
    uint16_t design_capacity;          /**< Battery design capacity in mAh */
    eos_battery_raw_t pending_raw;      /**< Pending raw data from device */
    volatile bool has_pending;          /**< Flag indicating pending data is available */
    eos_battery_state_t state;          /**< Current processed battery state */
    int8_t last_reported_percent;       /**< Last reported percentage (for change detection) */
    bool last_reported_charging;        /**< Last reported charging state (for change detection) */
    eos_battery_calc_fn_t calc_fn;      /**< Custom voltage-to-percent calculation function */
    eos_battery_state_t history[EOS_BATTERY_HISTORY_SIZE]; /**< Battery history ring buffer */
    uint32_t history_write_idx;         /**< Ring buffer write index */
    uint32_t history_count;             /**< Ring buffer entry count */
    lv_timer_t *timer;                  /**< LVGL timer handle */
    volatile bool request_in_flight;    /**< Flag indicating request is in flight */
    uint32_t last_request_tick;         /**< Timestamp of last request */
    uint32_t last_history_tick;         /**< Timestamp of last history record */
    uint32_t last_state_save_tick;      /**< Timestamp of last state save */
    uint32_t current_capacity;          /**< Current estimated battery capacity in mAh */
    uint32_t cycle_count;               /**< Battery cycle count */
    uint32_t event_id;                /**< Event ID for battery state changes */
} eos_battery_svc_t;

/**
 * @brief Singleton instance of battery service
 */
static eos_battery_svc_t _svc = {0};

/* Function Implementations -----------------------------------*/

/**
 * @brief Default voltage-to-percent calculation
 *
 * Linear interpolation between voltage_min and voltage_max.
 *
 * @param voltage_mv Battery voltage in millivolts
 * @return Calculated percentage (0-100)
 */
static int _default_calc_fn(int voltage_mv)
{
    if (voltage_mv <= EOS_BATTERY_VOLTAGE_MIN) {
        return 0;
    }
    if (voltage_mv >= EOS_BATTERY_VOLTAGE_MAX) {
        return 100;
    }
    return (int)((int32_t)(voltage_mv - EOS_BATTERY_VOLTAGE_MIN) * 100
                / (EOS_BATTERY_VOLTAGE_MAX - EOS_BATTERY_VOLTAGE_MIN));
}

/**
 * @brief Calculate percentage from voltage
 *
 * Uses custom calculation function if set, otherwise default.
 *
 * @param voltage_mv Battery voltage in millivolts
 * @return Calculated percentage (0-100)
 */
static int _calc_percent(int voltage_mv)
{
    if (_svc.calc_fn) {
        int result = _svc.calc_fn(voltage_mv);
        if (result >= 0 && result <= 100) {
            return result;
        }
    }
    return _default_calc_fn(voltage_mv);
}

/**
 * @brief Write entry to battery history ring buffer
 *
 * @param entry Pointer to battery state entry
 * @return None
 */
static void _history_write(const eos_battery_state_t *entry)
{
    _svc.history[_svc.history_write_idx] = *entry;
    _svc.history_write_idx = (_svc.history_write_idx + 1) % EOS_BATTERY_HISTORY_SIZE;
    if (_svc.history_count < EOS_BATTERY_HISTORY_SIZE) {
        _svc.history_count++;
    }
}

/**
 * @brief Load battery state from persistent storage
 *
 * @return None
 */
static void _load_state(void)
{
    _svc.current_capacity = (uint32_t)eos_state_get_number(EOS_STATE_KEY_CAPACITY, (double)_svc.design_capacity);
    _svc.cycle_count = (uint32_t)eos_state_get_number(EOS_STATE_KEY_CYCLE_COUNT, 0);
}

/**
 * @brief Save battery state to persistent storage
 *
 * @return None
 */
static void _save_state(void)
{
    eos_state_set_number(EOS_STATE_KEY_CAPACITY, (double)_svc.current_capacity);
    eos_state_set_number(EOS_STATE_KEY_CYCLE_COUNT, (double)_svc.cycle_count);
}

/**
 * @brief Process pending battery data
 *
 * Called from timer callback to process raw data and update state.
 *
 * @return None
 */
static void _process_pending_data(void)
{
    if (!_svc.has_pending) {
        return;
    }

    eos_critical_ctx_t ctx = eos_critical_enter();
    eos_battery_raw_t raw = _svc.pending_raw;
    _svc.has_pending = false;
    eos_critical_leave(ctx);

    bool percent_valid = (raw.percent >= 0 && raw.percent <= 100);
    bool voltage_valid = (raw.voltage_mv >= 0);

    if (!percent_valid && !voltage_valid) {
        EOS_LOG_W("Report discarded: both percent and voltage invalid");
        return;
    }

    int8_t percent;
    if (percent_valid) {
        percent = raw.percent;
    } else {
        int calc = _calc_percent(raw.voltage_mv);
        if (calc < 0) {
            calc = 0;
        }
        if (calc > 100) {
            calc = 100;
        }
        percent = (int8_t)calc;
    }

    _svc.state.percent = percent;
    _svc.state.voltage_mv = raw.voltage_mv;
    _svc.state.current_ma = raw.current_ma;
    _svc.state.charging = raw.charging;
    _svc.state.ts = eos_tick_get();
    _svc.state.valid = true;

    uint32_t now = _svc.state.ts;

    if (now - _svc.last_history_tick >= EOS_BATTERY_HISTORY_INTERVAL_MS
        || _svc.history_count == 0) {
        _history_write(&_svc.state);
        _svc.last_history_tick = now;
    }

    bool percent_changed = (_svc.last_reported_percent != -1
                            && _svc.state.percent != _svc.last_reported_percent);
    bool charging_changed = (_svc.last_reported_percent != -1
                             && _svc.state.charging != _svc.last_reported_charging);

    if (percent_changed || charging_changed) {
        eos_event_broadcast((lv_event_code_t)_svc.event_id, &_svc.state);
    }

    _svc.last_reported_percent = _svc.state.percent;
    _svc.last_reported_charging = _svc.state.charging;

    if (now - _svc.last_state_save_tick >= EOS_BATTERY_SAVE_INTERVAL_MS) {
        _save_state();
        _svc.last_state_save_tick = now;
    }

    EOS_LOG_I("Battery: %d%%, %dmV, %s",
              _svc.state.percent, _svc.state.voltage_mv,
              _svc.state.charging ? "charging" : "discharging");
}

/**
 * @brief Battery timer callback
 *
 * @param timer LVGL timer handle
 * @return None
 */
static void _battery_timer_cb(lv_timer_t *timer)
{
    (void)timer;

    _process_pending_data();

    uint32_t now = eos_tick_get();

    if (!_svc.dev_ops | !_svc.dev_ops->request_update) {
        return;
    }

    if (_svc.request_in_flight) {
        return;
    }

    if (now - _svc.last_request_tick < EOS_BATTERY_REQUEST_INTERVAL_MS) {
        return;
    }

    _svc.dev_ops->request_update();
    _svc.request_in_flight = true;
    _svc.last_request_tick = now;
}

/**
 * @brief Initialize battery service
 *
 * @return EOS_OK on success, error code on failure
 */
eos_result_t eos_service_battery_init(void)
{
    if (_svc.initialized) {
        return EOS_ERR_ALREADY_INITIALIZED;
    }

    eos_dev_battery_t *dev = eos_dev_battery_get_instance();
    _svc.dev_ops = &dev->ops;
    _svc.design_capacity = dev->design_capacity_mah;

    if (_svc.dev_ops->request_update == NULL) {
        EOS_LOG_E("Initialization failed: Battery device is not valid");
        return EOS_FAILED;
    }

    _svc.current_capacity = _svc.design_capacity;
    _svc.cycle_count = 0;

    _load_state();

    _svc.last_request_tick = eos_tick_get();

    _svc.event_id = (uint32_t)eos_event_register_id();

    _svc.timer = lv_timer_create(_battery_timer_cb, EOS_BATTERY_PROCESS_INTERVAL_MS, NULL);
    if (!_svc.timer) {
        EOS_LOG_E("Initialization failed: Failed to create battery timer");
        return EOS_ERR_MEM;
    }

    _svc.initialized = true;

    _svc.dev_ops->request_update();
    _svc.request_in_flight = true;
    _svc.last_request_tick = eos_tick_get();

    EOS_LOG_I("Battery service initialized, design capacity: %u mAh, current: %u mAh, cycles: %u",
              _svc.design_capacity, _svc.current_capacity, _svc.cycle_count);

    return EOS_OK;
}

/**
 * @brief Report raw battery data from device
 *
 * @param raw Pointer to raw battery data
 * @return None
 */
void eos_battery_report_raw(const eos_battery_raw_t *raw)
{
    if (!_svc.initialized || !raw) {
        return;
    }

    eos_critical_ctx_t ctx = eos_critical_enter();
    _svc.pending_raw = *raw;
    _svc.has_pending = true;
    _svc.request_in_flight = false;
    eos_critical_leave(ctx);
}

/**
 * @brief Get current battery percentage
 *
 * @return Battery percentage (0-100), or -1 if not available
 */
int8_t eos_battery_get_percent(void)
{
    if (!_svc.initialized || !_svc.state.valid) {
        return -1;
    }
    return _svc.state.percent;
}

/**
 * @brief Get current battery voltage
 *
 * @return Battery voltage in millivolts, or -1 if not available
 */
int16_t eos_battery_get_voltage_mv(void)
{
    if (!_svc.initialized || !_svc.state.valid) {
        return -1;
    }
    return _svc.state.voltage_mv;
}

/**
 * @brief Check if battery is charging
 *
 * @return true if charging, false otherwise
 */
bool eos_battery_is_charging(void)
{
    if (!_svc.initialized || !_svc.state.valid) {
        return false;
    }
    return _svc.state.charging;
}

/**
 * @brief Get current battery state
 *
 * @param state Pointer to state structure to fill
 * @return true on success, false on failure
 */
bool eos_battery_get_state(eos_battery_state_t *state)
{
    if (!_svc.initialized || !state || !_svc.state.valid) {
        return false;
    }
    *state = _svc.state;
    return true;
}

/**
 * @brief Get battery design capacity
 *
 * @return Design capacity in mAh
 */
uint32_t eos_battery_get_design_capacity(void)
{
    return _svc.design_capacity;
}

/**
 * @brief Get battery current capacity
 *
 * @return Current estimated capacity in mAh
 */
uint32_t eos_battery_get_current_capacity(void)
{
    return _svc.current_capacity;
}

/**
 * @brief Get battery cycle count
 *
 * @return Battery cycle count
 */
uint32_t eos_battery_get_cycle_count(void)
{
    return _svc.cycle_count;
}

/**
 * @brief Set custom voltage-to-percent calculation function
 *
 * @param fn Custom calculation function, or NULL to use default
 * @return None
 */
void eos_battery_set_calc_fn(eos_battery_calc_fn_t fn)
{
    _svc.calc_fn = fn;
}

/**
 * @brief Get event ID for battery percentage changes
 *
 * @return Event ID for percentage change events
 */
uint32_t eos_battery_get_event_id(void)
{
    return _svc.event_id;
}

/**
 * @brief Get battery history entry count
 *
 * @return Number of entries in battery history
 */
uint32_t eos_battery_get_history_count(void)
{
    if (!_svc.initialized) {
        return 0;
    }
    return _svc.history_count;
}

/**
 * @brief Get battery history entry by index
 *
 * @param index History entry index (0 to history_count-1)
 * @param entry Pointer to state structure to fill
 * @return true on success, false on invalid index
 */
bool eos_battery_get_history_entry(uint32_t index, eos_battery_state_t *entry)
{
    if (!_svc.initialized || !entry || index >= _svc.history_count) {
        return false;
    }

    if (_svc.history_count < EOS_BATTERY_HISTORY_SIZE) {
        *entry = _svc.history[index];
    } else {
        uint32_t read_idx = (_svc.history_write_idx + index) % EOS_BATTERY_HISTORY_SIZE;
        *entry = _svc.history[read_idx];
    }

    return true;
}
