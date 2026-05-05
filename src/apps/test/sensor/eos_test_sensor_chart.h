/**
 * @file eos_test_sensor_chart.h
 * @brief Sensor chart visualization test module header
 */

#ifndef EOS_TEST_SENSOR_CHART_H
#define EOS_TEST_SENSOR_CHART_H

#include "eos_config.h"

#if EOS_ENABLE_TEST_APP

#include "eos_activity.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Start sensor chart test activity
 */
void eos_test_sensor_chart_start(void);

#ifdef __cplusplus
}
#endif

#endif /* EOS_ENABLE_TEST_APP */

#endif /* EOS_TEST_SENSOR_CHART_H */
