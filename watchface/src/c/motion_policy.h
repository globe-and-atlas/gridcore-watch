#pragma once

static inline int orbit_step_ms(int speed) {
  return speed == 0 ? 90 : speed == 2 ? 300 : 180;
}

static inline int gesture_threshold(int sensitivity) {
  return sensitivity == 0 ? 220 : 360;
}
