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
