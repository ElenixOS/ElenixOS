/**
 * @file eos_test_sensor_multi_chart.h
 * @brief Multi-sensor sampling status visualization test header
 */

#ifndef EOS_TEST_SENSOR_MULTI_CHART_H
#define EOS_TEST_SENSOR_MULTI_CHART_H

#include "eos_config.h"

#if EOS_ENABLE_TEST_APP

#include "eos_activity.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Start multi-sensor chart test activity
 */
void eos_test_sensor_multi_chart_start(void);

#ifdef __cplusplus
}
#endif

#endif /* EOS_ENABLE_TEST_APP */

#endif /* EOS_TEST_SENSOR_MULTI_CHART_H */
