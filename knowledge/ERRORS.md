# Errors

Record deterministic errors, root causes, and fixes here.
# 2026-10-22 — Emery emulator boot stalled

- Error: `pebble install --emulator emery` produced no output for two minutes while `qemu-pebble` remained running.
- Scope: infrastructure/native capture; `pebble build` succeeded for both targets.
- Action: interrupted the install at two minutes. Inspect and recover the shared emulator before accepting a screenshot.
- Cause: the existing Emery emulator data would not boot with the active SDK. After backing up the 4.33.1 emulator folder to ignored `.tmp/`, `pebble wipe` restored startup and `pebble install --emulator emery` succeeded.
- Diagnostic caution: `pebble install -v` can print an OAuth token in its spawned pypkjs command. Avoid verbose output when recording commands.

# 2026-10-22 — Transitional emulator screenshots

- Symptom: a screenshot contained only the grid or a partially drawn time after install, settings changes, or a tap.
- Diagnosis: temporary draw logging measured full callbacks at roughly 8–23 ms; the emulator screenshot sometimes captured transitional framebuffer content. A subsequent capture showed the complete clock and date.
- Remedy: inspect every screenshot and recapture until the complete face is visible. Do not use a partial frame as release media.

# 2026-10-22 — Emery command timeout after repeated clock changes

- Symptom: after repeated `pebble emu-set-time`, AppMessage, and screenshot calls, `pebble screenshot` and `pebble ping` timed out while QEMU remained running. A partially drawn date screenshot preceded the timeout.
- Cause: not established; no evidence links this to the watchface renderer. Production build and earlier native captures passed.
- Recovery observed: `pebble kill` followed by install restored command responses briefly. The emulator later stalled again, so do not treat that recovery as durable.
- Next check: restart one emulator at a time before any further native capture. Preserve the existing complete captures; do not accept transitional frames.

# 2026-10-22 — Tunnel background pattern disappeared

- Symptom: Diamond mesh selected with the Tunnel look appeared as a plain black background on native Emery.
- Cause: the old Tunnel grid color `0x392B23` quantized to the same Pebble palette value as its near-black background.
- Fix: change the Tunnel grid color to `0x5A3A1B`, which is visibly distinct on the device palette.
- Graduated-to: color quantization must be checked on native screenshots when adding patterns to an existing look.

# 2026-10-22 — Foreground jumped when echo direction flipped

- Symptom: the main time appeared to shift between paired forward/reverse GIF frames even though the user expected only the echoes to move.
- Cause: both segmented and text layout subtracted half of a direction-dependent echo base from the foreground origin. Reversing the trail therefore also reversed the foreground offset.
- Fix: center the foreground independently of echo depth and direction. Keep direction multiplication inside the echo offset alone.
- Graduated-to: `execution/capture_store_media.py` rejects paired frames when the foreground numeral pixel mask changes.
