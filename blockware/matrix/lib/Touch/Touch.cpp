#include "Touch.h"

#define ACCEL_ADDR 0x31U

LIS2DW12Sensor* Touch::accel;

void Touch::init(TwoWire *wire, int16_t _sensitivity) {
  // setup global static accelerometer
   Touch::accel = new LIS2DW12Sensor(wire, ACCEL_ADDR);

  // `accel->begin()` always returns `false`
  // if (!accel->begin()) {
  //   Serial.printf("\n[ERROR] Unable to begin accelerometer");
  //   // err(250);
  // }

  Touch::accel->Enable_X();

  sensitivity = _sensitivity;

  // init axes_history with zeroes
  for (uint8_t i = 0; i < axes_history_length; i++) {
    for (uint8_t axis = 0; axis < axes_count; axis++) {
      axes_history[i][axis] = 0;
    }
  }
}

Touch::Touch(TwoWire *wire) {
  init(wire, DEFAULT_SENSITIVITY);
}

Touch::Touch(TwoWire *wire, int16_t sensitivity) {
  init(wire, sensitivity);
}

void Touch::tick() {
  // reset events on every tick
  for (uint8_t axis = 0; axis < axes_count; axis++) {
    event_is_tap[axis] = false;
  }

  // safely increment current_axes (read into axes_history slot)
  // `% axes_history_length` keeps index valid to length of history
  current_axes = (current_axes + 1) % axes_history_length;

  // read acceleromteter into global axes_read
  // each slot is an array of 3 signed 16-bit integers values (-32768 to +32767)
  Touch::accel->Get_X_AxesRaw(axes_read);
  for (uint8_t axis = 0; axis < axes_count; axis++) {
    // store absolute delta from previous measurement (previous_axes)
    axes_history[current_axes][axis] = std::abs(axes_read[axis] - previous_axes[axis]);
  }
  // copy read values into previous_axes
  memcpy(previous_axes, axes_read, axes_count * sizeof(int16_t));

  // debug axes array only if magnitude above small threshold
  if (debug) {
    if (axes_history[current_axes][0] > 5 || axes_history[current_axes][1] > 5 || axes_history[current_axes][2] > 5) {
      Serial.printf("\n [accel] axes[%02d][%05d, %05d, %05d]", current_axes, axes_history[current_axes][0], axes_history[current_axes][1], axes_history[current_axes][2]);
    }
  }


  // What is a tap?
  // Well on the accelerometer a tap looks like a steady state, followed by changes, followed by steady state
  // We can detect this pattern in accelerometer readings and fire a 'tap' event when it is observed
  // Example accelerometer readings for taps (x,y,z)


  // evaluate current axes_history array to detect taps
  // check each axis independently
  for (uint8_t axis = 0; axis < axes_count; axis++) {
    bool hasJump = false;

    for (uint8_t i = 0; i < axes_history_length; i++) {
      uint8_t index = (current_axes + i) % axes_history_length;

      // is magnitude of delta stored in axes_history at this index greater than threshold?
      if (axes_history[index][axis] > sensitivity) {
        hasJump = true;
      }
    }

    // if we observe a jump and axis was ready, then this is a tap
    if (hasJump && axis_ready[axis]) {
      axis_ready[axis] = false;
      event_is_tap[axis] = true;

      if (debug) {
        Serial.printf("[axis=%d] hasJump!", axis);
      }

    } else if (!hasJump && !axis_ready[axis]) {
      // no jumps detected, reset axis ready state
      axis_ready[axis] = true;

      if (debug) {
        Serial.printf("[axis=%d] resetting back to ready state", axis);
      }

    }
  }
}

bool Touch::isTap(int axis) {
  return event_is_tap[axis];
}
