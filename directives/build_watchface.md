---
generated_by: "OpenAI Codex (GPT-6.1-sol)"
timestamp: "2026-10-19T22:12:00-05:00"
---
# Build Way Back

Purpose: make the dimensional numeral concept a readable native Pebble watchface for Time 2 and Round 2.

Inputs: local time, persisted appearance settings, optional accelerometer samples.

Execution: build from `watchface/`; run `python3 execution/verify_watchface.py`; inspect native Emery and Gabbro captures.

Outputs: `watchface/build/watchface.pbw`, temporary emulator captures under `.tmp/`.

Edge cases: 12-hour midnight/noon, date Off, Round 2 circle clipping, palette quantization, motion timeout, unavailable accelerometer.

## Validation Contract
- The PBW contains `emery/pebble-app.bin`.
- The PBW contains `gabbro/pebble-app.bin`.
- Time digits remain legible on native Emery.
- Time digits remain legible on native Gabbro.
- Four colored named looks render.
- The monochrome named look renders.
- Date Off hides the date.
- YYYY-MM-DD is selectable.
- MM/DD/YYYY is selectable.
- DD/MM/YYYY is selectable.
- Custom colors persist after restart.
- Motion settles within three seconds.
- The verification script passes.
- A fresh verifier approves the output.

## Safety
- Do not read or commit `.env`.
- Keep temporary captures in `.tmp/`.
- Never commit `.lock-waf*`.
- Do not publish without a release review.
