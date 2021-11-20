#pragma once

// Accelerometer
#include <LIS2DW12Sensor.h>

class Touch {
private:
  static const uint8_t axes_count = 3;
  static const uint8_t axes_history_length = 15;
  static const int16_t DEFAULT_SENSITIVITY = 25;

  bool debug = false;

  uint8_t current_axes = 0;
  int16_t sensitivity;

  // store whether the axis is at rest (and ready for detecting event)
  bool axis_ready[axes_count];
  // store taps
  bool event_is_tap[axes_count];

  // arrays to store current and previous reads from accelerometer
  int16_t axes_read[axes_count];
  int16_t previous_axes[axes_count];
  // track sliding window of 15 accelerometer readings to detect taps
  int16_t axes_history[axes_history_length][axes_count];

protected:
public:
  static const uint8_t X_AXIS = 0;
  static const uint8_t Y_AXIS = 1;
  static const uint8_t Z_AXIS = 2;

  static LIS2DW12Sensor* accel;

  void init(TwoWire *wire, int16_t sensitivity);
  Touch(TwoWire *wire);
  Touch(TwoWire *wire, int16_t sensitivity);
  void tick(void);
  bool isTap(int axis);
};


