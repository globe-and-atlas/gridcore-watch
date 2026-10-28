from pathlib import Path
import subprocess


def test_native_motion_policy(tmp_path):
    root = Path(__file__).resolve().parents[1]
    source = tmp_path / "motion.c"
    source.write_text("""#include <assert.h>
#include "motion_policy.h"
int main(void) {
  assert(orbit_step_ms(0) * 8 == 720);
  assert(orbit_step_ms(1) * 8 == 1440);
  assert(orbit_step_ms(2) * 8 == 2400);
  assert(gesture_threshold(0) == 220);
  assert(gesture_threshold(1) == 360);
  assert(280 >= gesture_threshold(0));
  assert(280 < gesture_threshold(1));
  return 0;
}
""")
    binary = tmp_path / "motion"
    subprocess.run(["cc", "-std=c11", "-Wall", "-Wextra", "-Werror", "-I", str(root / "watchface/src/c"), str(source), "-o", str(binary)], check=True)
    subprocess.run([str(binary)], check=True)
