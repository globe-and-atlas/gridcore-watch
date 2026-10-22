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
- [ ] The PBW contains an Emery binary.
- [ ] The PBW contains a Gabbro binary.
- [ ] The default face shows Color Slices at 200×228.
- [ ] The Round 2 face keeps the time inside its circular screen.
- [ ] The time follows the watch's 12-hour setting.
- [ ] The time follows the watch's 24-hour setting.
- [ ] Lift is selectable.
- [ ] Tunnel is selectable.
- [ ] Color Slices is selectable.
- [ ] Daylight Blueprint is selectable.
- [ ] A grayscale look is selectable.
- [ ] Background color is configurable.
- [ ] Front digit color is configurable.
- [ ] Depth color is configurable.
- [ ] Grid color is configurable.
- [ ] Date Off hides the date.
- [ ] Date YYYY-MM-DD renders in that order.
- [ ] Date MM/DD/YYYY renders in that order.
- [ ] Date DD/MM/YYYY renders in that order.
- [ ] Settings persist after watch restart.
- [ ] The motion effect rests after a short reveal.
- [ ] `python3 execution/verify_watchface.py` passes.
- [ ] A fresh verifier reviews the final PBW against this contract.

## Progress
- [x] Create the project with the workspace scaffold.
- [ ] Implement the watchface renderer.
- [ ] Implement phone configuration.
- [ ] Build both platforms.
- [ ] Capture both native emulators.
- [ ] Run independent verification.
- [ ] Promote procedural knowledge.
