#pragma once

// Accelerometer
#include <LIS2DW12Sensor.h>

class Touch {
private:
  static const uint8_t AXES_COUNT = 3;
  static const uint8_t AXES_HISTORY_LENGTH = 15;
  static const int16_t DEFAULT_SENSITIVITY = 25;

  uint8_t current_axes = 0;
  int16_t sensitivity;

  // store whether the axis is at rest (and ready for detecting event)
  bool axis_ready[AXES_COUNT];
  // store taps
  bool event_is_tap[AXES_COUNT];

  // arrays to store current and previous reads from accelerometer
  int16_t axes_read[AXES_COUNT];
  int16_t previous_axes[AXES_COUNT];
  // track sliding window of 15 accelerometer readings to detect taps
  int16_t axes_history[AXES_HISTORY_LENGTH][AXES_COUNT];

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


