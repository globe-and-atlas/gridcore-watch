# Decisions

## Initial Decision

- Template profile: `workflow-python`
- Deploy target: `local-only`
- Runtime: `pebble`

## 2026-10-22 — Native vector font and optional motion

- Use seven solid chamfered segments for the clock and draw their colored depth with Pebble graphics primitives. This matches the selected digital font reference while keeping the geometry adjustable for both screen shapes.
- The original reveal was opt-in. A five-second cooldown bounds repeated wrist triggers, and disabling motion unsubscribes accelerometer sampling.
- Keep a single PBW targeting Emery and Gabbro; account for the round viewport in the common renderer.

## 2026-10-22 — Live tilt parallax

- Make the tilt setting on by default for new installs because the depth motion is central to this face; retain a static setting for people who prefer it. Existing saved choices remain in effect.
- Quantize filtered accelerometer x/y into seven positions per axis, with 50 mG hysteresis on each side of a boundary and redraw only when a position changes. This limits jitter and redraw activity at rest.
- Keep the digit front fixed and move the rear extrusion up to three pixels with three stepped colors. The accelerometer measures wrist orientation relative to gravity, not the viewer's eye position.
