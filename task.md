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

## Tilt parallax update — 2026-10-22

- [x] Motion Off leaves the clock static when accelerometer samples arrive.
- [x] Motion On shifts only the digit depth in response to horizontal wrist tilt.
- [x] Motion On shifts only the digit depth in response to vertical wrist tilt.
- [x] Motion On changes the depth shade order in response to wrist tilt.
- [x] The foreground digit silhouette remains fixed during tilt.
- [x] A steady tilt stops triggering redraws.
- [x] The tilt effect stays inside the Emery screen.
- [x] The tilt effect stays inside the Gabbro circle.
- [x] A tap reveal returns to the current tilt pose within three seconds.
- [x] The production PBW contains Emery and Gabbro binaries.
- [x] A fresh verifier reviews the tilt implementation and native captures.

## Day and night settings — 2026-10-22

- [x] Day mode uses the day palette at 02:15.
- [x] Night mode uses the night palette at 14:15.
- [x] Auto mode uses the day palette after the selected day start.
- [x] Auto mode uses the night palette after the selected night start.
- [x] Custom day background is configurable.
- [x] Custom day digit front is configurable.
- [x] Custom day digit depth is configurable.
- [x] Custom day grid is configurable.
- [x] Custom night background is configurable.
- [x] Custom night digit front is configurable.
- [x] Custom night digit depth is configurable.
- [x] Custom night grid is configurable.
- [x] A saved day palette survives watch restart.
- [x] A saved night palette survives watch restart.
- [x] Date Off hides the date in day mode.
- [x] Date Off hides the date in night mode.
- [x] YYYY-MM-DD renders in night mode.
- [x] MM/DD/YYYY renders in night mode.
- [x] DD/MM/YYYY renders in night mode.
- [x] The dual-platform production PBW passes `python3 execution/verify_watchface.py`.
- [x] Native Emery day and night captures are reviewed.
- [x] Native Gabbro day and night captures are reviewed.
- [x] A fresh verifier reviews the final settings and native captures.

## Corner decorations — 2026-10-22

- [x] Registration corners render at native resolution.
- [x] Short brackets render at native resolution.
- [x] Double brackets render at native resolution.
- [x] Crosshairs render at native resolution.
- [x] Edge ticks render at native resolution.
- [x] Dots render at native resolution.
- [x] A full frame renders at native resolution.
- [x] None removes the decoration.
- [x] The decoration choice persists after restart.
- [x] The decoration choice works on Emery.
- [x] The decoration choice works on Gabbro.
- [x] Native screenshots show the eight choices.
- [x] The production PBW passes `python3 execution/verify_watchface.py`.
- [x] A fresh verifier reviews the final decoration choices.

## Numeral styles — 2026-10-22

- [x] Twelve numeral choices appear in phone settings.
- [x] Each numeral choice displays a readable 12:15 on Emery.
- [x] Each numeral choice displays a readable 12:15 on Gabbro.
- [x] Each numeral choice retains a fixed foreground during tilt.
- [x] Each numeral choice has a moving depth trail when motion is on.
- [x] The numeral choice persists after restart.
- [x] Native previews show the numeral choices.

## Background patterns — 2026-10-22

- [x] Blueprint grid renders at native resolution.
- [x] Fine graph renders at native resolution.
- [x] Wide graph renders at native resolution.
- [x] Dot matrix renders at native resolution.
- [x] Crosshair field renders at native resolution.
- [x] Diagonal lines render at native resolution.
- [x] Diamond mesh renders at native resolution.
- [x] Scanlines render at native resolution.
- [x] Radar rings render at native resolution.
- [x] Contour frames render at native resolution.
- [x] Starfield renders at native resolution.
- [x] Solid renders without a pattern.
- [x] Background pattern is selectable independently of the color look.
- [x] The selected background pattern persists after restart.
- [x] Each background renders at 200×228 on Emery.
- [x] Each background renders at 260×260 on Gabbro.
- [x] Twelve native previews show each background on Emery.
- [x] Twelve native previews show each background on Gabbro.
- [x] The production PBW passes `python3 execution/verify_watchface.py`.
- [x] A fresh verifier reviews the final PBW and previews.
