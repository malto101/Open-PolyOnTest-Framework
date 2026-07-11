/**
 * Tiny HAL surface for the host_fff example.
 * Production code would live in a separate TU; here the fake replaces it.
 */
#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h>

/** Read a sensor channel; returns millivolts or negative on error. */
int32_t sensor_read(int channel);

/** Apply a calibration offset (void side-effect API for VOID_FUNC1 demo). */
void sensor_calibrate(int channel, int32_t offset_mv);

#endif /* SENSOR_H */
