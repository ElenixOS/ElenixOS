/**
 * @file eos_service_metric.h
 * @brief Thin semantic Metric adapter for watchface and script consumers
 */

#ifndef EOS_SERVICE_METRIC_H
#define EOS_SERVICE_METRIC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include "eos_error.h"

/* Public typedefs --------------------------------------------*/

/**
 * @brief Semantic values exposed to a watchface
 */
typedef enum
{
    EOS_METRIC_TIME_NOW,
    EOS_METRIC_BATTERY_PERCENT,
    EOS_METRIC_BATTERY_CHARGING,
    EOS_METRIC_HEART_RATE,
    EOS_METRIC_SPO2,
    EOS_METRIC_STEPS,
    EOS_METRIC_TEMPERATURE,
    EOS_METRIC_PRESSURE,
    EOS_METRIC_HEADING,
    EOS_METRIC_MOON_PHASE,
    EOS_METRIC_MOON_ILLUMINATION,
    EOS_METRIC_COUNT,
} eos_metric_id_t;

/**
 * @brief One semantic Metric sample
 */
typedef struct
{
    double value;
    const char *unit;
    uint32_t timestamp;
    bool valid;
    bool stale;
    bool boolean_value;
} eos_metric_value_t;

/* Public function prototypes ---------------------------------*/

/**
 * @brief Parse a public Metric name
 * @param name Semantic name such as "battery.percent"
 * @param id Output Metric identifier
 * @return true when the name is supported
 */
bool eos_metric_parse(const char *name, eos_metric_id_t *id);

/**
 * @brief Get the canonical public name of a Metric
 * @param id Metric identifier
 * @return Static name, or NULL for an invalid identifier
 */
const char *eos_metric_name(eos_metric_id_t id);

/**
 * @brief Read one semantic Metric value
 * @param id Metric identifier
 * @param sample Output sample
 * @return EOS_OK when the Metric is known; sample.valid reports data presence
 */
eos_result_t eos_metric_read(eos_metric_id_t id, eos_metric_value_t *sample);

/**
 * @brief Acquire live data needed by a Metric subscription
 * @param id Metric identifier
 * @return EOS_OK when the live source is available
 */
eos_result_t eos_metric_acquire(eos_metric_id_t id);

/**
 * @brief Release live data acquired for a Metric subscription
 * @param id Metric identifier
 * @return EOS_OK when released
 */
eos_result_t eos_metric_release(eos_metric_id_t id);

#ifdef __cplusplus
}
#endif

#endif /* EOS_SERVICE_METRIC_H */
