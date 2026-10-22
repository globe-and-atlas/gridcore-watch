---
generated_by: "OpenAI Codex (GPT-6.1-sol)"
timestamp: "2026-10-22T10:01:05-05:00"
---
# Build and check Gridcore

1. Run `npm ci`, `pebble clean`, and `pebble build` from `watchface/`.
2. Run `python3 execution/verify_watchface.py` from the project root. Inspect the PBW for both platform binaries.
3. Install into one emulator at a time: `pebble install --emulator emery` or `pebble install --emulator gabbro`. Install twice if a stale watchface remains visible.
4. Capture exact palette pixels with `pebble screenshot --emulator <platform> --no-open --no-correction <absolute output path>`.
5. Inspect the screenshot before accepting it. The emulator sometimes returns a transitional frame even after the draw callback has completed; capture again until the time, date, and registration corners are all present.
6. For deterministic art checks, clean and build with `GRIDCORE_FIXTURE=1 GRIDCORE_HOUR=2 GRIDCORE_MINUTE=15 GRIDCORE_LOOK=0 GRIDCORE_DATE=1 GRIDCORE_MODE=0 GRIDCORE_ANGLE_X=-3 GRIDCORE_ANGLE_Y=0`. Repeat with `GRIDCORE_MODE=1` for the night palette, and with `GRIDCORE_ANGLE_X=3` or positive/negative `GRIDCORE_ANGLE_Y` to compare depth positions. Clean again and rebuild without fixture variables before keeping the PBW.
7. For the automatic schedule, use a production build with `SETTING_MODE=2` and `pebble emu-set-time --emulator <platform> HH:MM:SS`. Capture immediately after setting the time because the emulator may revert to the host clock. Check 18:59/19:00 and 06:59/07:00; adjust `SETTING_NIGHT_START` and `SETTING_DAY_START` to check custom boundaries.
8. For live motion, use `pebble emu-accel --emulator <platform> gravity+x` or `gravity-x` with a production build. Compare complete captures after the face settles. The static fixture does not exercise the accelerometer subscription.
9. For style comparisons, set `GRIDCORE_HOUR=12 GRIDCORE_MINUTE=15 GRIDCORE_FONT=0 GRIDCORE_DECORATION=0` in the fixture build, then send `SETTING_FONT` (message key 10015) or `SETTING_DECORATION` (message key 10014) with `pebble send-app-message`. Capture all choices at the same time on each target. Clean and rebuild production after fixture capture.
10. For background comparisons, also set `GRIDCORE_BACKGROUND=0` in the fixture build. Send `SETTING_BACKGROUND` (message key 10016, values 0–11) while holding the look, font, date, and decoration constant. Inspect all 12 native captures on Emery and Gabbro. Clean and rebuild production afterward.

Avoid `-v` on `pebble install`: verbose output can include the emulator's OAuth token. Keep `.lock-waf*` and `.tmp/` out of Git. If Emery boot hangs, back up its SDK data, run `pebble wipe` without `--everything`, then retry. The wipe affects shared emulator state.
