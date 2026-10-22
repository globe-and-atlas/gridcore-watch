---
generated_by: "OpenAI Codex (GPT-6.1-sol)"
timestamp: "2026-10-19T22:11:00-05:00"
---
# Gridcore watchface

## Contract
- Target directive: `directives/build_watchface.md`.
- Intended execution script: `execution/verify_watchface.py`.
- Expected artifact: `watchface/build/watchface.pbw`.
- Expected captures: `.tmp/emery.png` and `.tmp/gabbro.png`.
- Safety: keep `.env`, tokens, waf locks, and `.tmp/` out of Git.
- Safety: do not publish a new listing without a separate release check.

## Validation Contract
- [x] The PBW contains an Emery binary.
- [x] The PBW contains a Gabbro binary.
- [x] The default face shows Color Slices at 200×228.
- [x] The Round 2 face keeps the time inside its circular screen.
- [x] The time follows the watch's 12-hour setting.
- [x] The time follows the watch's 24-hour setting.
- [x] Lift is selectable.
- [x] Tunnel is selectable.
- [x] Color Slices is selectable.
- [x] Daylight Blueprint is selectable.
- [x] A grayscale look is selectable.
- [x] Background color is configurable.
- [x] Front digit color is configurable.
- [x] Depth color is configurable.
- [x] Grid color is configurable.
- [x] Date Off hides the date.
- [x] Date YYYY-MM-DD renders in that order.
- [x] Date MM/DD/YYYY renders in that order.
- [x] Date DD/MM/YYYY renders in that order.
- [x] Settings persist after watch restart.
- [x] The motion effect rests after a short reveal.
- [x] `python3 execution/verify_watchface.py` passes.
- [x] A fresh verifier reviews the final PBW against this contract.

## Progress
- [x] Create the project with the workspace scaffold.
- [x] Implement the watchface renderer.
- [x] Implement phone configuration.
- [x] Build both platforms.
- [x] Capture both native emulators.
- [x] Run independent verification.
- [x] Promote procedural knowledge.
