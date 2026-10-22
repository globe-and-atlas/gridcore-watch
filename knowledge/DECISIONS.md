# Decisions

## Initial Decision

- Template profile: `workflow-python`
- Deploy target: `local-only`
- Runtime: `pebble`

## 2026-10-22 — Native vector font and optional motion

- Use seven solid chamfered segments for the clock and draw their colored depth with Pebble graphics primitives. This matches the selected digital font reference while keeping the geometry adjustable for both screen shapes.
- Keep motion opt-in. A five-second cooldown bounds repeated wrist triggers, and disabling motion unsubscribes accelerometer sampling.
- Keep a single PBW targeting Emery and Gabbro; account for the round viewport in the common renderer.
